# e384commLib
## Requirements
Currently supported platforms:
- Windows 10+
### Drivers
Install EMCR to make sure all the needed drivers are correctly installed [elements-ic.com/downloads/](https://elements-ic.com/downloads/)

## Working with the Library
### Prebuilt
The prebuilt library is available on Elements website [elements-ic.com/emcr/#api](https://elements-ic.com/emcr/#api)
### Compilation
#### Requirements
Download the needed libraries and the corresponding header files from the following link [elements-ic.com/wp-content/uploads/2024/10/e384_distributables.zip](https://elements-ic.com/wp-content/uploads/2024/10/e384_distributables.zip).

**The archive contains everything you need to compile the project**, in particular there will also be third party dynamic libraries such as:
- ftd2xx.dll: developed and released by FTDI [www.ftdichip.com](https://www.ftdichip.com)
- okFrontPanel.dll, okimpl_fpoip.dll: developed and released by Opal Kelly [www.opalkelly.com](https://www.opalkelly.com)
  
Make sure to add these libraries to the system path or the final application path.

#### Project configuration
To compile the library, the recommended settings can be found in the provided sln file.

For the provided sln to correctly work, set the following environmental variables to match the location on your computer of the downloaded libraries:
- CY_API_PATH
- FRONT_PANEL_PATH
- FTD2XX_PATH
- TOML_PP_PATH
