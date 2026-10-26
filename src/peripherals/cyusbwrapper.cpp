#include "cyusbwrapper.h"

#include <atomic>
#include <filesystem>
#include <mutex>
#include <vector>

#ifdef _WIN32
#include <windows.h> /*! Must be included before cfgmgr32.h */
#include <cfgmgr32.h>

#include <cwctype>
#include <system_error>

#ifdef _MSC_VER
#pragma comment(lib, "cfgmgr32.lib")
#endif

/*! Defined in cfgmgr32.h only for recent Windows SDK targets: same value as the SDK */
#ifndef CM_GETIDLIST_FILTER_PRESENT
#define CM_GETIDLIST_FILTER_PRESENT (0x00000100)
#endif
#endif

namespace {

/*! \brief Pointers to the functions of e384_cyusb.dll.
 *  The type of each pointer is taken from e384_cyusb.h with decltype: if a parameter changes in
 *  the header, the pointer type changes with it. decltype does not use the function, so the
 *  linker does not need e384_cyusb.lib. */
struct CyUsbFunctions {
    decltype(::cyw_api_version) *apiVersion = nullptr;
    decltype(::cyw_status_string) *statusString = nullptr;
    decltype(::cyw_device_count) *deviceCount = nullptr;
    decltype(::cyw_device_info) *deviceInfo = nullptr;
    decltype(::cyw_fx3_load_fw_if_bootloader) *fx3LoadFwIfBootloader = nullptr;
    decltype(::cyw_open) *open = nullptr;
    decltype(::cyw_close) *close = nullptr;
    decltype(::cyw_bulk_config) *bulkConfig = nullptr;
    decltype(::cyw_bulk_read) *bulkRead = nullptr;
    decltype(::cyw_bulk_write) *bulkWrite = nullptr;
    decltype(::cyw_bulk_abort) *bulkAbort = nullptr;
    decltype(::cyw_bulk_reset) *bulkReset = nullptr;
    decltype(::cyw_vendor_request) *vendorRequest = nullptr;
    decltype(::cyw_last_status) *lastStatus = nullptr;
    decltype(::cyw_last_error_message) *lastErrorMessage = nullptr;
};

/*! Written only once, under loadMutex, before loadedFlag becomes true.
 *  After that it is only read: no lock is needed to call the functions. */
CyUsbFunctions functions;

/*! true once the DLL is loaded and checked. Never goes back to false:
 *  the DLL is not unloaded, the function pointers stay valid. */
std::atomic <bool> loadedFlag(false);

/*! Serializes load() and protects lastLoadError */
std::mutex loadMutex;
std::string lastLoadError;

#ifdef _WIN32

/*! IDs from the Elements CyUSB3 INF (cyusb3.inf), as they appear in the USB instance ids */
constexpr const wchar_t * ElementsVidPattern = L"VID_3523";              /*!< Elements UDB devices, any PID */
constexpr const wchar_t * Fx3BootloaderPattern = L"VID_04B4&PID_00F3";  /*!< FX3 bootloader, used to load the firmware */

constexpr const wchar_t * DllName = L"e384_cyusb.dll";

/*! Any object of this module: its address tells GetModuleHandleExW which module contains e384commLib */
const int moduleMarker = 0;

/*! Path as UTF-8 text, for the error messages */
std::string pathToText(const std::filesystem::path &path) {
    const std::u8string utf8 = path.u8string();
    return std::string(utf8.begin(), utf8.end());
}

/*! Text of a Windows error code, e.g. "error 126 (The specified module could not be found.)" */
std::string windowsErrorText(DWORD errorCode) {
    return "error " + std::to_string(errorCode) + " ("
            + std::system_category().message(static_cast <int> (errorCode)) + ")";
}

/*! Folder of the module that contains this code: the executable when e384commLib is linked
 *  statically (EMCR), e384commlib_labview.dll for the LabVIEW wrapper.
 *  Empty if it cannot be found. */
std::filesystem::path folderOfThisModule() {
    HMODULE module = nullptr;
    const DWORD flags = GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;
    if (!GetModuleHandleExW(flags, reinterpret_cast <LPCWSTR> (&moduleMarker), &module)) {
        return {};
    }

    /*! GetModuleFileNameW truncates silently: grow the buffer until the whole path fits */
    std::vector <wchar_t> buffer(MAX_PATH);
    while (true) {
        DWORD length = GetModuleFileNameW(module, buffer.data(), static_cast <DWORD> (buffer.size()));
        if (length == 0) {
            return {};
        }
        if (length < buffer.size()) {
            return std::filesystem::path(std::wstring(buffer.data(), length)).parent_path();
        }
        buffer.resize(buffer.size()*2);
    }
}

/*! Loads e384_cyusb.dll: first next to the module of e384commLib, then from System32.
 *  On failure returns nullptr and writes the reason in error. */
HMODULE openDll(std::string &error) {
    std::filesystem::path folder = folderOfThisModule();

    if (!folder.empty()) {
        std::filesystem::path fullPath = folder / DllName;
        std::error_code existsError;
        if (std::filesystem::exists(fullPath, existsError)) {
            /*! LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR: the dependencies of the DLL (MSVC runtime) are
             *  searched in its folder too, then in System32. Never in the PATH. */
            HMODULE module = LoadLibraryExW(fullPath.c_str(), nullptr,
                                            LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32);
            if (module == nullptr) {
                /*! The file is there but cannot be loaded (wrong architecture, missing dependency...) */
                error = "cannot load " + pathToText(fullPath) + ": " + windowsErrorText(GetLastError());
            }
            return module;
        }
    }

    HMODULE module = LoadLibraryExW(DllName, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (module == nullptr) {
        const DWORD errorCode = GetLastError();
        error = "e384_cyusb.dll not found in " + (folder.empty() ? std::string("the application folder") : pathToText(folder))
                + " nor in System32 (" + windowsErrorText(errorCode) + ")";
    }
    return module;
}

/*! Resolves one function. If it is missing, appends its name to missingNames. */
template <typename Function>
void resolve(HMODULE module, const char *name, Function * &pointer, std::string &missingNames) {
    /*! Through void* to convert the generic FARPROC into the typed pointer */
    void *address = reinterpret_cast <void *> (GetProcAddress(module, name));
    pointer = reinterpret_cast <Function *> (address);
    if (pointer == nullptr) {
        missingNames += (missingNames.empty() ? "" : ", ") + std::string(name);
    }
}

/*! Resolves all the functions of e384_cyusb.h. Returns false if any is missing. */
bool resolveAll(HMODULE module, CyUsbFunctions &table, std::string &error) {
    std::string missingNames;
    resolve(module, "cyw_api_version", table.apiVersion, missingNames);
    resolve(module, "cyw_status_string", table.statusString, missingNames);
    resolve(module, "cyw_device_count", table.deviceCount, missingNames);
    resolve(module, "cyw_device_info", table.deviceInfo, missingNames);
    resolve(module, "cyw_fx3_load_fw_if_bootloader", table.fx3LoadFwIfBootloader, missingNames);
    resolve(module, "cyw_open", table.open, missingNames);
    resolve(module, "cyw_close", table.close, missingNames);
    resolve(module, "cyw_bulk_config", table.bulkConfig, missingNames);
    resolve(module, "cyw_bulk_read", table.bulkRead, missingNames);
    resolve(module, "cyw_bulk_write", table.bulkWrite, missingNames);
    resolve(module, "cyw_bulk_abort", table.bulkAbort, missingNames);
    resolve(module, "cyw_bulk_reset", table.bulkReset, missingNames);
    resolve(module, "cyw_vendor_request", table.vendorRequest, missingNames);
    resolve(module, "cyw_last_status", table.lastStatus, missingNames);
    resolve(module, "cyw_last_error_message", table.lastErrorMessage, missingNames);

    if (!missingNames.empty()) {
        error = "e384_cyusb.dll does not export: " + missingNames;
        return false;
    }
    return true;
}

/*! Asks Windows for the instance ids of the USB devices currently plugged in.
 *  The answer is a single buffer with all the ids one after the other, each terminated by '\0',
 *  and an extra '\0' at the end, e.g. "USB\VID_3523&PID_00F0\123\0USB\VID_046D...\0\0".
 *  (Same code as usbdevicepresence.cpp in ftdi_utils.) */
bool readPresentUsbDeviceIds(std::vector <wchar_t> &idBuffer) {
    const ULONG filter = CM_GETIDLIST_FILTER_ENUMERATOR | CM_GETIDLIST_FILTER_PRESENT;
    const int maxAttempts = 5;

    for (int attempt = 0; attempt < maxAttempts; attempt++) {
        /*! First ask how big the buffer must be, then fill it */
        ULONG length = 0;
        if (CM_Get_Device_ID_List_SizeW(&length, L"USB", filter) != CR_SUCCESS) {
            return false;
        }

        idBuffer.assign(length, L'\0');
        CONFIGRET result = CM_Get_Device_ID_ListW(L"USB", idBuffer.data(), length, filter);
        if (result == CR_SUCCESS) {
            return true;
        }

        /*! CR_BUFFER_SMALL: a device was plugged in between the two calls, try again */
        if (result != CR_BUFFER_SMALL) {
            return false;
        }
    }
    return false;
}

/*! true if the instance id contains the pattern, ignoring the case */
bool idContains(std::wstring id, const wchar_t *pattern) {
    for (wchar_t &character : id) {
        character = towupper(character);
    }
    return id.find(pattern) != std::wstring::npos;
}

#endif // _WIN32

} // namespace

/* ------------------------------------------------------------------------- */
/*  Presence and loading                                                     */
/* ------------------------------------------------------------------------- */

#ifdef _WIN32

bool CyUsbWrapper::isDevicePresent() {
    std::vector <wchar_t> idBuffer;
    if (!readPresentUsbDeviceIds(idBuffer)) {
        /*! Unknown: better to try loading the DLL than to hide a device */
        return true;
    }

    /*! Walk the '\0' separated ids */
    size_t position = 0;
    while (position < idBuffer.size() && idBuffer[position] != L'\0') {
        std::wstring id(&idBuffer[position]); /*!< Copies up to the next '\0' */
        position += id.size()+1;

        if (idContains(id, ElementsVidPattern) || idContains(id, Fx3BootloaderPattern)) {
            return true;
        }
    }
    return false;
}

bool CyUsbWrapper::load() {
    if (loadedFlag.load(std::memory_order_acquire)) {
        return true;
    }

    std::lock_guard <std::mutex> lock(loadMutex);
    if (loadedFlag.load(std::memory_order_acquire)) {
        return true; /*!< Loaded by another thread while waiting for the lock */
    }

    std::string error;
    HMODULE module = openDll(error);
    if (module == nullptr) {
        lastLoadError = error;
        return false;
    }

    CyUsbFunctions table;
    if (!resolveAll(module, table, error)) {
        FreeLibrary(module);
        lastLoadError = error;
        return false;
    }

    /*! The header copied in e384commLib and the DLL must describe the same interface */
    int dllVersion = table.apiVersion();
    if (dllVersion != CYW_API_VERSION) {
        FreeLibrary(module);
        lastLoadError = "e384_cyusb.dll has API version " + std::to_string(dllVersion)
                + ", e384commLib needs version " + std::to_string(CYW_API_VERSION);
        return false;
    }

    /*! Publish the table before the flag (release): a thread that sees the flag sees the table */
    functions = table;
    lastLoadError.clear();
    loadedFlag.store(true, std::memory_order_release);
    return true;
}

#else // not Windows: CyAPI and the CyUSB3 driver exist only on Windows

bool CyUsbWrapper::isDevicePresent() {
    return false;
}

bool CyUsbWrapper::load() {
    std::lock_guard <std::mutex> lock(loadMutex);
    lastLoadError = "UDB devices are supported only on Windows";
    return false;
}

#endif

bool CyUsbWrapper::isLoaded() {
    return loadedFlag.load(std::memory_order_acquire);
}

std::string CyUsbWrapper::getLastLoadError() {
    std::lock_guard <std::mutex> lock(loadMutex);
    return lastLoadError;
}

std::string CyUsbWrapper::getLastErrorMessage() {
    if (!isLoaded()) {
        return getLastLoadError();
    }
    char buffer[512];
    if (functions.lastErrorMessage(buffer, static_cast <int> (sizeof(buffer))) != CYW_OK) {
        return "";
    }
    return buffer;
}

/* ------------------------------------------------------------------------- */
/*  Forwarding methods                                                       */
/* ------------------------------------------------------------------------- */

int CyUsbWrapper::deviceCount(int *count) {
    if (!isLoaded()) {
        return CYW_ERR_NOT_LOADED;
    }
    return functions.deviceCount(count);
}

int CyUsbWrapper::deviceInfo(int index, unsigned short *vid, unsigned short *pid, char *serial, int serialLen) {
    if (!isLoaded()) {
        return CYW_ERR_NOT_LOADED;
    }
    return functions.deviceInfo(index, vid, pid, serial, serialLen);
}

int CyUsbWrapper::fx3LoadFwIfBootloader(int index, const char *fwPath, int *loaded, int *fx3Error) {
    if (!isLoaded()) {
        return CYW_ERR_NOT_LOADED;
    }
    return functions.fx3LoadFwIfBootloader(index, fwPath, loaded, fx3Error);
}

int CyUsbWrapper::open(int index, cyw_device **dev) {
    if (!isLoaded()) {
        return CYW_ERR_NOT_LOADED;
    }
    return functions.open(index, dev);
}

void CyUsbWrapper::close(cyw_device *dev) {
    if (!isLoaded()) {
        return;
    }
    functions.close(dev);
}

int CyUsbWrapper::bulkConfig(cyw_device *dev, unsigned long inXferSize, unsigned long *actualInXferSize,
                             unsigned long inTimeoutMs, unsigned long outTimeoutMs) {
    if (!isLoaded()) {
        return CYW_ERR_NOT_LOADED;
    }
    return functions.bulkConfig(dev, inXferSize, actualInXferSize, inTimeoutMs, outTimeoutMs);
}

int CyUsbWrapper::bulkRead(cyw_device *dev, unsigned char *buf, long *len, int packetMode) {
    if (!isLoaded()) {
        return CYW_ERR_NOT_LOADED;
    }
    return functions.bulkRead(dev, buf, len, packetMode);
}

int CyUsbWrapper::bulkWrite(cyw_device *dev, const unsigned char *buf, long *len) {
    if (!isLoaded()) {
        return CYW_ERR_NOT_LOADED;
    }
    return functions.bulkWrite(dev, buf, len);
}

int CyUsbWrapper::bulkAbort(cyw_device *dev, int direction) {
    if (!isLoaded()) {
        return CYW_ERR_NOT_LOADED;
    }
    return functions.bulkAbort(dev, direction);
}

int CyUsbWrapper::bulkReset(cyw_device *dev) {
    if (!isLoaded()) {
        return CYW_ERR_NOT_LOADED;
    }
    return functions.bulkReset(dev);
}

int CyUsbWrapper::vendorRequest(cyw_device *dev, int direction, unsigned char request,
                                unsigned short value, unsigned short index, unsigned char *buf, long *len) {
    if (!isLoaded()) {
        return CYW_ERR_NOT_LOADED;
    }
    return functions.vendorRequest(dev, direction, request, value, index, buf, len);
}

int CyUsbWrapper::lastStatus(cyw_device *dev, unsigned long *ntStatus, unsigned long *usbdStatus) {
    if (!isLoaded()) {
        return CYW_ERR_NOT_LOADED;
    }
    return functions.lastStatus(dev, ntStatus, usbdStatus);
}
