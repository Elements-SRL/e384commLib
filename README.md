# REQUIREMENTS
Compatibility has been verified for the following systems:
- Windows 10+

Note: Elements only provides free support for Windows 11+.

# Prebuilt Binaries
Prebuilt versions of the library are available in the [Releases](https://github.com/Elements-SRL/er4CommLib/releases) section.

Each release archive contains:
- **C++ static library** (compiled binaries and headers)
- **LabVIEW wrapper** (`e384commlib_labview.dll` and headers): see [Using the library from LabVIEW](#using-the-library-from-labview)

## C++
e384commlib requires c++20 or newer.

# BUILD
## Toolchain
Elements builds the er4commlib statically using Qt Creator with the Qt libraries 6.7.3 and MSVC 2022 [Qt Community](https://www.qt.io/download-dev)

Note: the Qt Libraries are not used. Qt Creator is used just to compile on different platforms seamlessly.

Building requires CMake 3.27 or newer and Windows x64.

## Drivers
Install EMCR to make sure all the needed drivers are correctly installed [elements-ic.com](https://elements-ic.com/downloads/).
Expect in future relases a driver only installer to be provided.

## Dependencies
The e384commlib requires the following libraries:
- `ftdi_utils` 1.6.1 or newer for the handling of FTDI based devices [here](https://github.com/Elements-SRL/ftdi_utils/): it loads the FTDI libraries (FTD2XX.dll, MPSSE.dll) at runtime, only when an FTDI device is used. Only the libMPSSE headers are needed to build.
- `okFrontPanel.dll` as part of FrontPanel driver for the handling of Opal Kelly based devices: linked with /DELAYLOAD (MSVC) and loaded only when an Opal Kelly device is used [visit opalkelly.com](https://pins.opalkelly.com/downloads?ucacid=1617031807.247011)
- `e384_cyusb.dll` for the handling of UDB devices (Cypress FX3): loaded at runtime, only when a UDB device is used. It is not needed to build the e384commlib, and the EZ-USB FX3 SDK is not needed either.
- `toml++` for toml files parsing [here](https://github.com/marzer/tomlplusplus/)

The vendor libraries (FTD2XX.dll, MPSSE.dll, okFrontPanel.dll, e384_cyusb.dll) are searched first in the folder of the module that contains the e384commlib (the executable, or e384commlib_labview.dll for the LabVIEW wrapper), then in System32. The PATH is never used.

## Environment variables
The CMake project finds the dependencies through these environment variables:
- `FRONT_PANEL_PATH`: FrontPanel SDK
- `TOML_PP_PATH`: toml++
- `FTDI_UTILS_PATH`: installed ftdi_utils
- `LIBMPSSE_PATH`: libMPSSE (headers only, needed by ftdi_utils)
- `LABVIEW_TO_C_PATH`: LabVIEW headers (only with `-DBUILD_FOR_LABVIEW=ON`)
- `E384COMMLIB_PATH`: install folder (optional)

# Using the library from LabVIEW
The LabVIEW wrapper `e384commlib_labview.dll` exposes the e384commlib as C functions, declared in `e384commlib_labview.h`. Call them from LabVIEW with a Call Library Function Node (calling convention: C).

## Requirements
- Windows 10+ x64 and LabVIEW 64-bit (LabVIEW 32-bit is not supported).
- Microsoft Visual C++ Redistributable 2015-2022 x64.
- The USB drivers of the device: install EMCR [elements-ic.com](https://elements-ic.com/downloads/).
- The vendor library of the device family, see below.

## Vendor libraries
`e384commlib_labview.dll` does not contain the vendor libraries and the release archive does not include them. Each device family needs its own library, loaded only when a device of that family is used:

| Device family | Libraries |
|---|---|
| FTDI based devices | `FTD2XX.dll` and `MPSSE.dll` |
| UDB devices (Cypress FX3) | `e384_cyusb.dll` |
| Opal Kelly based devices | `okFrontPanel.dll` |

Elements provides these libraries together with the devices: contact Elements support if you do not have them.

Put them **in the same folder as `e384commlib_labview.dll`**. System32 is also searched (the FTDI driver usually installs `FTD2XX.dll` there).
The PATH is not used: a library that is only in a folder listed in the PATH is not found.

When you build a LabVIEW application (Application Builder), add the vendor libraries to the build and give them the same destination folder as `e384commlib_labview.dll` (by default the `data` folder).

`e384commlib_labview.dll` is loaded by LabVIEW even if the vendor libraries are missing. In this case the devices of that family are not listed by `detectDevices`, and `connectDevice` returns one of the error codes below.

## Checking the vendor libraries
These functions tell why a plugged in device is not listed. They do not need a connected device:

| Function | Device family | Error code if the library cannot be loaded |
|---|---|---|
| `getFtdiDriverStatus` | FTDI based | `ErrorFtdiDriverNotFound` (0x10004) |
| `getCyUsbDriverStatus` | UDB (Cypress FX3) | `ErrorCyUsbDriverNotFound` (0x10005) |
| `getOkDriverStatus` | Opal Kelly based | `ErrorOkDriverNotFound` (0x10006) |

Each function returns Success when no device of its family is plugged in, or when the library is loaded. The `details` output describes the last loading error (empty if none).

Recommended sequence:
1. Call `detectDevices`.
2. If an expected device is not listed, call the three functions above and show the `details` of the ones that do not return Success.

## Troubleshooting
| `details` contains | Cause | Fix |
|---|---|---|
| `not found in <folder> nor in System32`, or `cannot load FTD2XX.dll` / `cannot load MPSSE.dll` without a folder | The library is missing | Copy it next to `e384commlib_labview.dll` |
| `cannot load <full path>: error 126` | The library is there, but a library it needs is missing | Install the Visual C++ Redistributable 2015-2022 x64 |
| `does not export`, `has version` or `has API version` | The library is too old or does not match this release | Use the library provided by Elements for this release |

## Testing without a device
Create an empty file named `e384_DEMO.pls` in your user folder (`%USERPROFILE%`) and restart LabVIEW (the file is checked only once, when the library is first used). `detectDevices` then lists also some demo devices, among them these test devices, which only check that the vendor library can be loaded (they produce no data):
- `DEMO_FTDI_LibLoadTest`: FTD2XX.dll and MPSSE.dll
- `DEMO_CYUSB_LibLoadTest`: e384_cyusb.dll
- `DEMO_OK_LibLoadTest`: okFrontPanel.dll

`connectDevice` succeeds if the library is found, otherwise it returns the error code of the family and the matching function above gives the details. When you are done, delete `e384_DEMO.pls` and restart LabVIEW.
