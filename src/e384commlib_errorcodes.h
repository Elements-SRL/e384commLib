/*! \file e384commlib_errorcodes.h
 * \brief Defines error codes returned by methods in e384CommLib namespace.
 */
#ifndef E384COMMLIB_ERRORCODES_H
#define E384COMMLIB_ERRORCODES_H

#ifndef E384COMMLIB_LABVIEW_WRAPPER
namespace e384CommLib {
#endif

/*! \enum ErrorGroups_t
 *  \brief Enumerated error groups.
 */
typedef enum ErrorGroups {
    ErrorGroupDeviceDetection =     0x00010000, /*!< Error codes related to device detection errors. */
    ErrorGroupEepromCommunication = 0x00020000, /*!< Error codes related to eeprom communication errors. */
    ErrorGroupDeviceConnection =    0x00030000, /*!< Error codes related to connection errors. */
    ErrorGroupDeviceCommands =      0x00040000, /*!< Error codes related to failed commands to the device. */
    ErrorGroupDeviceFeatures =      0x00050000, /*!< Error codes related to wrongly used features. */
    ErrorGroupDeviceIssues =        0x00060000, /*!< Error codes related to issues with the device. */
    ErrorGroupLibraryIssues =       0x00070000, /*!< Error codes related to issues with the library. */
    ErrorGroupCalibration =         0x00080000  /*!< Error codes related to issues with calibration parameters. */
} ErrorGroups_t;

/*! \enum ErrorCodes_t
 *  \brief Enumerated error codes.
 */
typedef enum ErrorCodes {
    Success =                                                               0x00000000, /*!< Method returns with no errors. */

    ErrorNoDeviceFound =                    ErrorGroupDeviceDetection +     0x00000001, /*!< Error returned when no devices are detected. */
    ErrorListDeviceFailed =                 ErrorGroupDeviceDetection +     0x00000002, /*!< Error returned when device detection fails.
                                                                                         *   This error may be due to problems with FTDI driver installation too. */
    ErrorDeviceNotFound =                   ErrorGroupDeviceDetection +     0x00000003, /*!< Error returned when the required device for connection is not found. */

    ErrorEepromAlreadyConnected =           ErrorGroupEepromCommunication + 0x00000001, /*!< Error returned when trying to connect to a device eeprom which is already connected. */
    ErrorEepromConnectionFailed =           ErrorGroupEepromCommunication + 0x00000002, /*!< Error returned when connection to a device eeprom fails. */
    ErrorEepromDisconnectionFailed =        ErrorGroupEepromCommunication + 0x00000003, /*!< Error returned when disconnection from a device eeprom fails. */
    ErrorEepromNotConnected =               ErrorGroupEepromCommunication + 0x00000004, /*!< Error returned when trying to communicate with a device eeprom if none is connected. */
    ErrorEepromReadFailed =                 ErrorGroupEepromCommunication + 0x00000005, /*!< Error returned when reading from a device eeprom fails. */
    ErrorEepromWriteFailed =                ErrorGroupEepromCommunication + 0x00000006, /*!< Error returned when reading from a device eeprom fails. */
    ErrorEepromNotRecognized =              ErrorGroupEepromCommunication + 0x00000007, /*!< Error returned when the eeprom is not recognized. */
    ErrorEepromInvalidAddress =             ErrorGroupEepromCommunication + 0x00000008, /*!< Error returned when trying to read or write an invalid eeprom address. */

    ErrorDeviceTypeNotRecognized =          ErrorGroupDeviceConnection +    0x00000001, /*!< Error returned when the device type is not recognized. */
    ErrorDeviceAlreadyConnected =           ErrorGroupDeviceConnection +    0x00000002, /*!< Error returned when trying to connect to a device which is already connected. */
    ErrorDeviceNotConnected =               ErrorGroupDeviceConnection +    0x00000003, /*!< Error returned when trying to communicate with a device if none is connected. */
    ErrorDeviceConnectionFailed =           ErrorGroupDeviceConnection +    0x00000004, /*!< Error returned when connection to a device fails. */
    ErrorFtdiConfigurationFailed =          ErrorGroupDeviceConnection +    0x00000005, /*!< Error returned when FTDI communication channel configuration fails. */
    ErrorConnectionPingFailed =             ErrorGroupDeviceConnection +    0x00000006, /*!< Error returned when failed to ping the device during connection. */
    ErrorConnectionFpgaResetFailed =        ErrorGroupDeviceConnection +    0x00000007, /*!< Error returned when failed to reset the FPGA during connection. */
    ErrorConnectionChipResetFailed =        ErrorGroupDeviceConnection +    0x00000008, /*!< Error returned when failed to reset the chip during connection. */
    ErrorDeviceDisconnectionFailed =        ErrorGroupDeviceConnection +    0x00000009, /*!< Error returned when disconnection from a device fails. */
    ErrorDeviceFwLoadingFailed =            ErrorGroupDeviceConnection +    0x0000000A, /*!< Error returned when the FPGA FW is not correctly loaded. */
    ErrorDeviceToBeUpgraded =               ErrorGroupDeviceConnection +    0x0000000B, /*!< Error returned when the FPGA FW is old and must be upgraded. */

    ErrorSendMessageFailed =                ErrorGroupDeviceCommands +      0x00000001, /*!< Error returned when sending a message to a device fails. */
    ErrorCommandNotImplemented =            ErrorGroupDeviceCommands +      0x00000002, /*!< Error returned when trying to use a command that is not implemented for the current device. */
    ErrorValueOutOfRange =                  ErrorGroupDeviceCommands +      0x00000003, /*!< Error returned when trying to set a value that is out of range for the current device,
                                                                                         *   e.g. a current range index bigger than the total number of current ranges available. */
    ErrorUnchangedValue =                   ErrorGroupDeviceCommands +      0x00000004, /*!< Error returned when trying to set a value that is already been set,
                                                                                         *   e.g. a compensation value identical to the one currently set on the device. */
    ErrorBadlyFormedProtocolLoop =          ErrorGroupDeviceCommands +      0x00000005, /*!< Error returned when trying to set a loop within a protocol in which the loop closing item
                                                                                             should be followed by an item following the next one (available values are items up to the current one
                                                                                             for loops or the next item for a linear sequence). */
    ErrorNoDataAvailable =                  ErrorGroupDeviceCommands +      0x00000006, /*!< Error returned when trying to read data but there's no data available. */
    ErrorRepeatedHeader =                   ErrorGroupDeviceCommands +      0x00000007, /*!< Error returned when reading copies of the same data header. */
    ErrorRepeatedTail =                     ErrorGroupDeviceCommands +      0x00000008, /*!< Error returned when reading copies of the same protocol tail. */
    ErrorIllFormedMessage =                 ErrorGroupDeviceCommands +      0x00000009, /*!< Error returned when reading an ill formed message
                                                                                             e.g. a data acq. packet without first sample index. */
    ErrorWrongClampModality =               ErrorGroupDeviceCommands +      0x0000000A, /*!< Error returned a command is used in the wrong clamp modality
                                                                                             e.g. set holding voltage in current clamp. */
    WarningValueClipped =                   ErrorGroupDeviceCommands +      0x8000000B, /*!< Warning returned when trying to set a value that is out of range for the current device,
                                                                                             but differently from the ErrorValueOutOfRange, the command is still applied but clipped,
                                                                                         *   e.g. a voltage offset bigger then the DAC range. */
    ErrorCompensationNotEnabled =           ErrorGroupDeviceCommands +      0x0000000C, /*!< Error returned when trying setting a compensation that is not enabled */

    ErrorFeatureNotImplemented =            ErrorGroupDeviceFeatures +      0x00000000, /*!< Error returned when trying to use a feature that is not implemented for the current device. */
    ErrorUpgradesNotAvailable =             ErrorGroupDeviceFeatures +      0x00000001, /*!< Error returned when there are no upgrades available for the current device. */

    ErrorExpiredDevice =                    ErrorGroupDeviceIssues +        0x00000000, /*!< Error returned when the device has expired */

    ErrorMemoryInitialization =             ErrorGroupLibraryIssues +       0x00000000, /*!< Error returned when the library fails to initialize the memory */

    ErrorCalibrationDirMissing =            ErrorGroupCalibration +         0x00000000, /*!< Error returned when the calibration folder is missing */
    ErrorCalibrationMappingNotOpened =      ErrorGroupCalibration +         0x00000001, /*!< Error returned when the calibration mapping file cannot be opened */
    ErrorCalibrationMappingCorrupted =      ErrorGroupCalibration +         0x00000002, /*!< Error returned when the calibration mapping file is corrupted */
    ErrorCalibrationFileCorrupted =         ErrorGroupCalibration +         0x00000003, /*!< Error returned when a calibration file is corrupted */
    ErrorCalibrationFileMissing =           ErrorGroupCalibration +         0x00000004, /*!< Error returned when a calibration file is missing */
    ErrorCalibrationSoftwareBug =           ErrorGroupCalibration +         0x00000005, /*!< Error returned when there's a bug in the calibration loading procedure */
    ErrorCalibrationNotLoadedYet =          ErrorGroupCalibration +         0x00000006, /*!< Error returned when calibration were not loaded yet */
    ErrorCalibrationMappingWrongNumbering = ErrorGroupCalibration +         0x00000007, /*!< Error returned when boards numbering in mapping file is wrong */

    ErrorUnknown =                                                          0xFFFFFFFF  /*!< Undefined error. */
} ErrorCodes_t;

#ifndef E384COMMLIB_LABVIEW_WRAPPER
} // e384CommLib
#endif

#endif // E384COMMLIB_ERRORCODES_H
