#ifndef OKFRONTPANELWRAPPER_H
#define OKFRONTPANELWRAPPER_H

#include <string>

/*! \brief Runtime loading of okFrontPanel.dll (Opal Kelly FrontPanel), needed only by the Opal Kelly devices.
 *
 *  e384commLib uses the FrontPanel C++ API (okCFrontPanel, OpalKelly::FrontPanelManager). Those classes
 *  are written inline in okFrontPanel.h and call the C functions exported by okFrontPanel.dll, so the
 *  import of the DLL comes from the e384commLib code. To let applications start and work with the
 *  other devices when the DLL is missing, the DLL is DELAY-LOADED:
 *  - e384commLib adds /DELAYLOAD:okFrontPanel.dll and delayimp.lib to the link options of the
 *    executable or DLL that links it (PUBLIC link options in CMakeLists.txt, also in the installed
 *    package). This way Windows does not load okFrontPanel.dll when the program starts.
 *  - Before any FrontPanel object is created, the code must call load() and use FrontPanel only if it
 *    returns true. Creating an okCFrontPanel or a FrontPanelManager without the DLL would end
 *    the process with a delay-load exception.
 *
 *  Unlike CyUsbWrapper there are no forwarding methods: after load() the FrontPanel C++ API is used
 *  directly, as before.
 *
 *  load():
 *  - searches okFrontPanel.dll in the folder of the module that contains e384commLib (the executable
 *    for the static library, e384commlib_labview.dll for the LabVIEW wrapper) and then in System32.
 *    The current directory and the PATH are never searched (same rules as e384_cyusb.dll).
 *    The delay-load helper then finds the DLL already loaded and uses that one.
 *  - resolves ALL the FrontPanel functions used by e384commLib at once (__HrLoadAllImportsForDll):
 *    a DLL that does not export one of them is rejected here, not in the middle of an acquisition;
 *  - accepts only a DLL with API version >= the version of okFrontPanel.h used to build e384commLib
 *    (OK_API_VERSION_MAJOR.MINOR.MICRO).
 *  Once loaded the DLL is never unloaded. All the methods are static and thread-safe.
 *
 *  Only MSVC builds use delay-loading. With other toolchains okFrontPanel is a normal dependency,
 *  already loaded when the program starts: isDevicePresent() and load() always return true.
 */
class OkFrontPanelWrapper {
public:
    /*! \brief true if an Opal Kelly device (USB vendor ID 0x151F) is plugged in. Does not load the DLL.
     *  If Windows cannot be asked, returns true (try anyway). */
    static bool isDevicePresent();

    /*! \brief Loads okFrontPanel.dll and checks it. Returns true if it is loaded (now or before). */
    static bool load();

    static bool isLoaded();

    /*! \brief Description of the last loading failure (file not found, missing function,
     *  old API version), empty if the last load succeeded. */
    static std::string getLastLoadError();
};

#endif // OKFRONTPANELWRAPPER_H
