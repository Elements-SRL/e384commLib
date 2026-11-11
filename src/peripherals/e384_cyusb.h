/*
 * e384_cyusb.h - Public C interface of e384_cyusb.dll
 *
 * Copyright (c) Elements srl. All rights reserved.
 *
 * e384_cyusb.dll is the only binary that contains the Cypress/Infineon CyAPI
 * library. This header contains no Cypress types.
 *
 * Rules of the interface:
 *  - Plain C (extern "C", __cdecl), no C++ classes and no std:: types cross
 *    the DLL boundary: a Release DLL works with Debug and Release callers.
 *  - Every function returns a cyw_status (CYW_OK on success), except
 *    cyw_api_version, cyw_close and cyw_status_string.
 *  - Devices are addressed by index (0 .. count-1) for enumeration and by an
 *    opaque cyw_device handle once opened.
 *  - Threading: same rules as CyAPI. Calls on the same cyw_device must not
 *    run concurrently; different handles can be used from different threads.
 *  - Lengths are in bytes. "long *len" parameters are in/out: requested
 *    length on input, transferred length on output (also on failure).
 */

#ifndef E384_CYUSB_H
#define E384_CYUSB_H

#ifdef _WIN32
#  ifdef E384_CYUSB_BUILD
#    define CYW_API __declspec(dllexport)
#  else
#    define CYW_API __declspec(dllimport)
#  endif
#  define CYW_CALL __cdecl
#else
#  define CYW_API
#  define CYW_CALL
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*! Version of this interface. Incremented only for incompatible changes.
 *  The caller refuses the DLL if cyw_api_version() differs from this value.
 *  Functions added later in a compatible way are detected with GetProcAddress. */
#define CYW_API_VERSION 1

/*! Opaque handle to an opened device. */
typedef struct cyw_device cyw_device;

/*! Return codes. */
typedef enum cyw_status {
    CYW_OK = 0,
    CYW_ERR_INVALID_ARG = 1,         /*!< NULL pointer, negative length, buffer too small */
    CYW_ERR_INDEX_OUT_OF_RANGE = 2,  /*!< No device at this index (devices may have been unplugged) */
    CYW_ERR_OPEN_FAILED = 3,         /*!< CyAPI could not open the device */
    CYW_ERR_NO_BULK_ENDPOINT = 4,    /*!< The device has no bulk IN or OUT endpoint */
    CYW_ERR_TRANSFER_FAILED = 5,     /*!< XferData returned false (timeout included); see cyw_last_status */
    CYW_ERR_FW_DOWNLOAD_FAILED = 6,  /*!< CCyFX3Device::DownloadFw failed; detail in fx3Error */
    CYW_ERR_OUT_OF_MEMORY = 7,       /*!< std::bad_alloc caught inside the DLL */
    CYW_ERR_INTERNAL = 99            /*!< Any other C++ exception caught inside the DLL;
                                      *   detail in cyw_last_error_message */
} cyw_status;

/*! Direction of a bulk endpoint or of a vendor request. */
typedef enum cyw_direction {
    CYW_DIR_TO_DEVICE = 0,   /*!< OUT */
    CYW_DIR_FROM_DEVICE = 1  /*!< IN */
} cyw_direction;

/* ------------------------------------------------------------------------- */
/*  Library                                                                  */
/* ------------------------------------------------------------------------- */

/*! Returns CYW_API_VERSION of the DLL. First function called after loading. */
CYW_API int CYW_CALL cyw_api_version(void);

/*! Short English description of a status code, for logs. Never NULL. */
CYW_API const char * CYW_CALL cyw_status_string(int status);

/* ------------------------------------------------------------------------- */
/*  Enumeration (by index, no handle needed)                                 */
/* ------------------------------------------------------------------------- */

/*! Number of devices bound to the CyUSB3 driver (any VID/PID).
 *  The caller filters by VID/PID with cyw_device_info. */
CYW_API int CYW_CALL cyw_device_count(int *count);

/*! VID, PID and serial number (ASCII, NUL-terminated) of the device at index.
 *  Opens the device at that index. Serial may be NULL if not needed.
 *  serialLen: size of serial in bytes, including the terminator. */
CYW_API int CYW_CALL cyw_device_info(int index,
                                     unsigned short *vid,
                                     unsigned short *pid,
                                     char *serial, int serialLen);

/*! If the device at index is running the FX3 bootloader, downloads the
 *  firmware image to RAM (CCyFX3Device::DownloadFw(..., RAM)).
 *  fwPath: path of the .img file in the ANSI code page, as CyAPI requires.
 *  loaded: set to 1 if the firmware was downloaded, 0 if the device was not
 *          in bootloader (not an error).
 *  fx3Error: optional (may be NULL), receives the FX3_FWDWNLOAD_ERROR_CODE.
 *  After a download the device re-enumerates: the caller waits and
 *  enumerates again before cyw_open */
CYW_API int CYW_CALL cyw_fx3_load_fw_if_bootloader(int index,
                                                   const char *fwPath,
                                                   int *loaded,
                                                   int *fx3Error);

/* ------------------------------------------------------------------------- */
/*  Open / close                                                             */
/* ------------------------------------------------------------------------- */

/*! Opens the device at index and looks for its bulk IN and OUT endpoints.
 *  Succeeds also without bulk endpoints (e.g. for control-only use such as
 *  reading the tuple); bulk functions then return CYW_ERR_NO_BULK_ENDPOINT. */
CYW_API int CYW_CALL cyw_open(int index, cyw_device **dev);

/*! Closes the device and frees the handle. Accepts NULL. */
CYW_API void CYW_CALL cyw_close(cyw_device *dev);

/* ------------------------------------------------------------------------- */
/*  Bulk endpoints                                                           */
/* ------------------------------------------------------------------------- */

/*! Configures both bulk endpoints.
 *  inXferSize: requested IN transfer size; rounded down to a multiple of the
 *              IN MaxPktSize. The value actually used is written to
 *              actualInXferSize (may be NULL).
 *  The OUT transfer size is set to the OUT MaxPktSize. */
CYW_API int CYW_CALL cyw_bulk_config(cyw_device *dev,
                                     unsigned long inXferSize,
                                     unsigned long *actualInXferSize,
                                     unsigned long inTimeoutMs,
                                     unsigned long outTimeoutMs);

/*! Reads from the bulk IN endpoint.
 *  len: in = buffer size, out = bytes received. Like CyAPI, len can be > 0
 *       even when CYW_ERR_TRANSFER_FAILED is returned (timeout with partial
 *       data): the caller must check len, as readDataFromDevice.
 *  packetMode: 0 for continuous acquisition, 1 for flash reads
 *              (XferData pktMode argument). */
CYW_API int CYW_CALL cyw_bulk_read(cyw_device *dev,
                                   unsigned char *buf, long *len,
                                   int packetMode);

/*! Writes to the bulk OUT endpoint. len: in = bytes to send, out = bytes sent. */
CYW_API int CYW_CALL cyw_bulk_write(cyw_device *dev,
                                    const unsigned char *buf, long *len);

/*! Aborts pending transfers on one bulk endpoint. */
CYW_API int CYW_CALL cyw_bulk_abort(cyw_device *dev, int direction);

/*! Resets both bulk endpoints (today: UdbUtils::resetBulkEndpoints). */
CYW_API int CYW_CALL cyw_bulk_reset(cyw_device *dev);

/* ------------------------------------------------------------------------- */
/*  Control endpoint                                                         */
/* ------------------------------------------------------------------------- */

/*! Vendor request to the device (Target = device, ReqType = vendor).
 *  Covers all CYP_CMD_* commands. buf may be NULL when *len == 0.
 *  len: in = bytes to send/receive, out = bytes transferred. */
CYW_API int CYW_CALL cyw_vendor_request(cyw_device *dev, int direction,
                                        unsigned char request,
                                        unsigned short value,
                                        unsigned short index,
                                        unsigned char *buf, long *len);

/* ------------------------------------------------------------------------- */
/*  Diagnostics                                                              */
/* ------------------------------------------------------------------------- */

/*! NtStatus and UsbdStatus of the last failed transfer on this handle
 *  (bulk or control). Each output pointer may be NULL. */
CYW_API int CYW_CALL cyw_last_status(cyw_device *dev,
                                     unsigned long *ntStatus,
                                     unsigned long *usbdStatus);

/*! Detailed English message of the last failure on the calling thread
 *  (thread-local, like GetLastError). Written by every function that does
 *  not return CYW_OK; left untouched on success.
 *  It names the function and the cause, for example:
 *    "cyw_open(2): CCyUSBDevice::Open failed, LastError=0x1F"
 *    "cyw_fx3_load_fw_if_bootloader(0): DownloadFw returned INVALID_FWSIGNATURE"
 *    "cyw_bulk_read: exception std::bad_alloc: bad allocation"
 *  buf receives a NUL-terminated string, truncated to bufLen-1 characters.
 *  Returns CYW_ERR_INVALID_ARG if buf is NULL or bufLen <= 0. */
CYW_API int CYW_CALL cyw_last_error_message(char *buf, int bufLen);

#ifdef __cplusplus
}
#endif

#endif /* E384_CYUSB_H */
