#include "okfrontpanelwrapper.h"

#include <atomic>
#include <mutex>

#ifdef _MSC_VER
#include <windows.h> /*! Must be included before cfgmgr32.h */
#include <cfgmgr32.h>
#include <delayimp.h>

#include <cstdio>
#include <cstring>
#include <cwctype>
#include <filesystem>
#include <system_error>
#include <vector>

#pragma comment(lib, "cfgmgr32.lib")

/*! Defined in cfgmgr32.h only for recent Windows SDK targets: same value as the SDK */
#ifndef CM_GETIDLIST_FILTER_PRESENT
#define CM_GETIDLIST_FILTER_PRESENT (0x00000100)
#endif
#endif // _MSC_VER

/*! Only for the version constants (OK_API_VERSION_*) and the version functions */
#include "okFrontPanel.h"

namespace {

/*! true once the DLL is loaded and checked. Never goes back to false: the DLL is not unloaded. */
std::atomic <bool> loadedFlag(false);

/*! Serializes load() and protects lastLoadError */
std::mutex loadMutex;
std::string lastLoadError;

#ifdef _MSC_VER

/*! Opal Kelly USB vendor ID, as it appears in the USB instance ids (e.g. "USB\VID_151F&PID_xxxx\serial").
 *  \todo Check with Filippo on a real device.
 *  If the Elements boards use a custom VID, add it here. */
constexpr const wchar_t * OpalKellyVidPattern = L"VID_151F";

/*! Must be the same name the import library okFrontPanel.lib uses and /DELAYLOAD names in CMakeLists.txt */
constexpr const wchar_t * DllName = L"okFrontPanel.dll";
constexpr const char * DllNameAnsi = "okFrontPanel.dll";

/*! Any object of this module: its address tells GetModuleHandleExW which module contains e384commLib */
const int moduleMarker = 0;

/* The helpers below (pathToText, windowsErrorText, folderOfThisModule, readPresentUsbDeviceIds,
 * idContains) are the same as in cyusbwrapper.cpp. \todo Make a shared file. */

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

/*! Code as 8 hex digits, e.g. "C06D007F" */
std::string hexText(unsigned long code) {
    char text[16];
    sprintf_s(text, "%08lX", code);
    return std::string(text);
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

/*! Loads okFrontPanel.dll: first next to the module of e384commLib, then from System32.
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
        error = "okFrontPanel.dll not found in " + (folder.empty() ? std::string("the application folder") : pathToText(folder))
                + " nor in System32 (" + windowsErrorText(errorCode) + ")";
    }
    return module;
}

/*! Details of a delay-load failure, filled by the exception filter */
struct DelayLoadFailure {
    DWORD exceptionCode = 0;
    char functionName[128] = {0}; /*!< Name of the missing function, empty if unknown */
};

/*! Exception filter for resolveAllImports: handles only the two exceptions raised by the
 *  delay-load helper (DLL not found, function not found), as ok_DelayLoadDllExceptionFilter
 *  in okFrontPanel.h does. Any other exception goes on as usual. */
int delayLoadFilter(EXCEPTION_POINTERS *exceptionPointers, DelayLoadFailure &failure) {
    const DWORD code = exceptionPointers->ExceptionRecord->ExceptionCode;
    if (code != VcppException(ERROR_SEVERITY_ERROR, ERROR_MOD_NOT_FOUND) &&
            code != VcppException(ERROR_SEVERITY_ERROR, ERROR_PROC_NOT_FOUND)) {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    failure.exceptionCode = code;

    /*! The delay-load helper passes a DelayLoadInfo as first parameter of the exception */
    if (exceptionPointers->ExceptionRecord->NumberParameters >= 1) {
        const DelayLoadInfo *info = reinterpret_cast <const DelayLoadInfo *> (exceptionPointers->ExceptionRecord->ExceptionInformation[0]);
        if (info != nullptr && info->dlp.fImportByName && info->dlp.szProcName != nullptr) {
            strncpy_s(failure.functionName, sizeof(failure.functionName), info->dlp.szProcName, _TRUNCATE);
        }
    }
    return EXCEPTION_EXECUTE_HANDLER;
}

/*! Binds now all the delay-loaded imports of okFrontPanel.dll, instead of one by one at the first call.
 *  Returns the HRESULT of __HrLoadAllImportsForDll, or E_FAIL if the helper raised an exception.
 *  No C++ objects with destructors in this function: MSVC does not allow __try/__except together
 *  with C++ object unwinding in the same function. */
HRESULT resolveAllImports(DelayLoadFailure &failure) {
    __try {
        return __HrLoadAllImportsForDll(DllNameAnsi);
    }
    __except (delayLoadFilter(GetExceptionInformation(), failure)) {
        return E_FAIL;
    }
}

/*! Asks Windows for the instance ids of the USB devices currently plugged in.
 *  The answer is a single buffer with all the ids one after the other, each terminated by '\0',
 *  and an extra '\0' at the end, e.g. "USB\VID_151F&PID_xxxx\123\0USB\VID_046D...\0\0".
 *  (Same code as cyusbwrapper.cpp and usbdevicepresence.cpp in ftdi_utils.) */
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

#endif // _MSC_VER

} // namespace

/* ------------------------------------------------------------------------- */
/*  Presence and loading                                                     */
/* ------------------------------------------------------------------------- */

#ifdef _MSC_VER

bool OkFrontPanelWrapper::isDevicePresent() {
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

        if (idContains(id, OpalKellyVidPattern)) {
            return true;
        }
    }
    return false;
}

bool OkFrontPanelWrapper::load() {
    if (loadedFlag.load(std::memory_order_acquire)) {
        return true;
    }

    std::lock_guard <std::mutex> lock(loadMutex);
    if (loadedFlag.load(std::memory_order_acquire)) {
        return true; /*!< Loaded by another thread while waiting for the lock */
    }

    /*! 1. Load the DLL from the allowed folders. The delay-load helper loads the DLL by name
     *  (LoadLibraryExA("okFrontPanel.dll")): since a module with that name is already loaded,
     *  Windows returns this one and never searches the PATH. */
    std::string error;
    HMODULE module = openDll(error);
    if (module == nullptr) {
        lastLoadError = error;
        return false;
    }

    /*! From here on the module is never freed, even on failure: some imports may already point into it */

    /*! 2. Bind all the imports now: a missing function is reported here */
    DelayLoadFailure failure;
    HRESULT result = resolveAllImports(failure);
    if (failure.exceptionCode == VcppException(ERROR_SEVERITY_ERROR, ERROR_PROC_NOT_FOUND)) {
        lastLoadError = "okFrontPanel.dll does not export "
                + (failure.functionName[0] != '\0' ? std::string(failure.functionName) : std::string("a function used by e384commLib"))
                + " (the DLL is probably too old)";
        return false;
    }
    if (failure.exceptionCode != 0) {
        lastLoadError = "okFrontPanel.dll cannot be loaded (delay-load exception 0x" + hexText(failure.exceptionCode) + ")";
        return false;
    }
    /*! HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND): okFrontPanel.dll is not in the delay-load table of
     *  this module, i.e. it is imported normally (/DELAYLOAD missing in the link options). Then it was
     *  already loaded when the program started and the functions are already bound: nothing to do. */
    if (FAILED(result) && result != HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND)) {
        lastLoadError = "okFrontPanel.dll cannot be loaded (__HrLoadAllImportsForDll returned 0x"
                + hexText(static_cast <unsigned long> (result)) + ")";
        return false;
    }

    /*! 3. The DLL must be at least as recent as the okFrontPanel.h used to build e384commLib */
    if (!okFrontPanel_CheckAPIVersion(OK_API_VERSION_MAJOR, OK_API_VERSION_MINOR, OK_API_VERSION_MICRO)) {
        const char *dllVersion = okFrontPanel_GetAPIVersionString();
        lastLoadError = "okFrontPanel.dll has version " + std::string(dllVersion != nullptr ? dllVersion : "unknown")
                + ", e384commLib needs version " + OK_API_VERSION_STRING + " or newer";
        return false;
    }

    lastLoadError.clear();
    loadedFlag.store(true, std::memory_order_release);
    return true;
}

#else // not MSVC: no delay-loading, okFrontPanel is loaded with the program

bool OkFrontPanelWrapper::isDevicePresent() {
    return true;
}

bool OkFrontPanelWrapper::load() {
    loadedFlag.store(true, std::memory_order_release);
    return true;
}

#endif

bool OkFrontPanelWrapper::isLoaded() {
    return loadedFlag.load(std::memory_order_acquire);
}

std::string OkFrontPanelWrapper::getLastLoadError() {
    std::lock_guard <std::mutex> lock(loadMutex);
    return lastLoadError;
}
