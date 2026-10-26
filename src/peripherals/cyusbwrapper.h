#ifndef CYUSBWRAPPER_H
#define CYUSBWRAPPER_H

#include <string>

/*! Copy of the public header of e384_cyusb.dll (private repository e384_cyusb).
 *  Only the types and the function declarations are used: the functions are never
 *  called directly (that would need e384_cyusb.lib), only through CyUsbWrapper. */
#include "e384_cyusb.h"

/*! Status returned by the CyUsbWrapper methods when e384_cyusb.dll is not loaded.
 *  Negative, so it never collides with the cyw_status values of the DLL. */
#define CYW_ERR_NOT_LOADED (-1)

/*! \brief Runtime loading of e384_cyusb.dll, the only binary that contains the Cypress CyAPI library.
 *
 *  e384commLib does not link e384_cyusb.dll: the DLL is loaded with LoadLibraryExW only when a
 *  UDB device is used, so applications start and work with the other devices without it.
 *  Same approach as Ftd2xxWrapper in ftdi_utils.
 *
 *  - The DLL is searched in the folder of the module that contains e384commLib (the executable
 *    for the static library, e384commlib_labview.dll for the LabVIEW wrapper) and then in System32.
 *    The current directory and the PATH are never searched.
 *  - The DLL is accepted only if it exports every function of e384_cyusb.h and
 *    cyw_api_version() == CYW_API_VERSION.
 *  - Once loaded the DLL is never unloaded (the function pointers stay valid until the process ends).
 *  - All the methods are static and thread-safe.
 *
 *  The other methods forward to the cyw_* function with the same name (see e384_cyusb.h);
 *  they return CYW_ERR_NOT_LOADED if the DLL is not loaded.
 */
class CyUsbWrapper {
public:
    /*! \brief true if an Elements UDB device (VID 0x3523) or an FX3 in bootloader (VID 0x04B4, PID 0x00F3)
     *  is plugged in. Does not load the DLL. If Windows cannot be asked, returns true (try anyway). */
    static bool isDevicePresent();

    /*! \brief Loads e384_cyusb.dll. Returns true if it is loaded (now or before). */
    static bool load();

    static bool isLoaded();

    /*! \brief Description of the last loading failure (file not found, missing function,
     *  wrong API version), empty if the last load succeeded. */
    static std::string getLastLoadError();

    /*! \brief Detailed message of the last failed cyw_* call on this thread (cyw_last_error_message). */
    static std::string getLastErrorMessage();

    /*! \name Forwarding methods, one per function of e384_cyusb.h */
    /*! @{ */
    static int deviceCount(int *count);
    static int deviceInfo(int index, unsigned short *vid, unsigned short *pid, char *serial, int serialLen);
    static int fx3LoadFwIfBootloader(int index, const char *fwPath, int *loaded, int *fx3Error);
    static int open(int index, cyw_device **dev);
    static void close(cyw_device *dev);
    static int bulkConfig(cyw_device *dev, unsigned long inXferSize, unsigned long *actualInXferSize,
                          unsigned long inTimeoutMs, unsigned long outTimeoutMs);
    static int bulkRead(cyw_device *dev, unsigned char *buf, long *len, int packetMode);
    static int bulkWrite(cyw_device *dev, const unsigned char *buf, long *len);
    static int bulkAbort(cyw_device *dev, int direction);
    static int bulkReset(cyw_device *dev);
    static int vendorRequest(cyw_device *dev, int direction, unsigned char request,
                             unsigned short value, unsigned short index, unsigned char *buf, long *len);
    static int lastStatus(cyw_device *dev, unsigned long *ntStatus, unsigned long *usbdStatus);
    /*! @} */
};

#endif // CYUSBWRAPPER_H
