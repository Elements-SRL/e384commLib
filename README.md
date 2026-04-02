# REQUIREMENTS
Compatibility has been verified for the following systems:
- Windows 10+

Note: Elements only provides free support for Windows 11+.

## C++
e384commlib requires c++20 or newer.

# BUILD
## Toolchain
Elements builds the er4commlib statically using Qt Creator with the Qt libraries 6.7.3 and MSVC 2022 [Qt Community](https://www.qt.io/download-dev)

Note: the Qt Libraries are not used. Qt Creator is used just to compile on different platforms seamlessly.

## Drivers
Install EMCR to make sure all the needed drivers are correctly installed [elements-ic.com](https://elements-ic.com/downloads/)

## Dependencies
The e384commlib requires the following libraries:
- ftdi_utils for the handling of FTDI based devices [github.com](https://github.com/Elements-SRL/ftdi_utils/)
- okFrontPanel.dll, okimpl_fpoip.dll as part of FrontPanel driver for the handling of Opal Kelly based devices [opalkelly.com](https://pins.opalkelly.com/downloads?ucacid=1617031807.247011)
- cyUSB.lib as part of the EZ-USB FX3 SDK for the handling of Cypress based devices [infineon.com](https://www.infineon.com/design-resources/development-tools/sdk/usb-controllers-sdk/ez-usb-fx3-software-development-kit)
- toml++ for toml files parsing [github.com](https://github.com/marzer/tomlplusplus/)

# PREBUILT BINARIES
Prebuilt versions of the library are available on Elements website [elements-ic.com](https://elements-ic.com/emcr/#api)
