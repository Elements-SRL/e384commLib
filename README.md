# e384commLib
## Requirements
Currently supported platforms:
- Windows 10+
### Drivers
Install EMCR to make sure all the needed drivers are correctly installed [elements-ic.com/downloads/](https://elements-ic.com/downloads/)
### Dynamic Libraries
Download the following dynamic libraries and add them to the system path or the final application path:
- ftd2xx.dll: additional third party library (developed and released by FTDI [www.ftdichip.com](https://www.ftdichip.com))
- okFrontPanel.dll, okimpl_fpoip.dll: additional third party libraries (developed and released by Opal Kelly [www.opalkelly.com](https://www.opalkelly.com))
## Working with the Library
### Prebuilt
The prebuilt libraries are available on Elements website [elements-ic.com/emcr/#api](https://elements-ic.com/emcr/#api)
### Compilation
#### Requirements
Download the needed static libraries and the corresponding header files []
For the provided sln to correctly work, set the following environmental variables to match the location on your computer:
- CY_API_PATH
- FRONT_PANEL_PATH
- FTD2XX_PATH
- TOML_PP_PATH
#### Compiler
To compile the library, the recommended settings can be found in the provided sln file.
#### Static Libraries
The static libraries needed by this project can be downloaded from the Elements website [elements-ic.com/emcr/#api](https://elements-ic.com/emcr/#api).
The static libraries can be found inside the unzipped archive, make sure to include them with
Download the following static or import libraries and add them to the list of the imported libraries in your project:
