#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "messagedispatcher.h"

namespace py = pybind11;
using namespace e384CommLib;

PYBIND11_MODULE(e384CommLibPython, m) {
    m.doc() = "Typical order of calls\n"
              "detectDevices();\n"
              "connect();\n"
              "<use the device>\n"
              "disconnect();";

    m.def("detectDevices", []() {
        std::vector <std::string> deviceIds;
        ErrorCodes_t err = MessageDispatcher::detectDevices(deviceIds);
        return std::make_tuple(err, deviceIds);
    }, "Detect plugged in devices");

//    todo completare gli error codes
    py::enum_<ErrorCodes_t>(m, "ErrorCodes")
            .value("Success",                           Success)
            .value("ErrorNoDeviceFound",                ErrorNoDeviceFound)
            .value("ErrorListDeviceFailed",             ErrorListDeviceFailed)
            .value("ErrorEepromAlreadyConnected",       ErrorEepromAlreadyConnected)
            .value("ErrorEepromConnectionFailed",       ErrorEepromConnectionFailed)
            .value("ErrorEepromDisconnectionFailed",    ErrorEepromDisconnectionFailed)
            .value("ErrorEepromNotConnected",           ErrorEepromNotConnected)
            .value("ErrorEepromReadFailed",             ErrorEepromReadFailed)
            .value("ErrorEepromNotRecognized",          ErrorEepromNotRecognized)
            .value("ErrorDeviceTypeNotRecognized",      ErrorDeviceTypeNotRecognized)
            .value("ErrorDeviceAlreadyConnected",       ErrorDeviceAlreadyConnected)
            .value("ErrorDeviceNotConnected",           ErrorDeviceNotConnected)
            .value("ErrorDeviceConnectionFailed",       ErrorDeviceConnectionFailed)
            .value("ErrorFtdiConfigurationFailed",      ErrorFtdiConfigurationFailed)
            .value("ErrorDeviceDisconnectionFailed",    ErrorDeviceDisconnectionFailed)
            .value("ErrorSendMessageFailed",            ErrorSendMessageFailed)
            .value("ErrorCommandNotImplemented",        ErrorCommandNotImplemented)
            .value("ErrorValueOutOfRange",              ErrorValueOutOfRange)
            .value("ErrorFeatureNotImplemented",        ErrorFeatureNotImplemented)
            .value("ErrorUpgradesNotAvailable",         ErrorUpgradesNotAvailable)
            .value("ErrorExpiredDevice",                ErrorExpiredDevice)
            .value("ErrorUnknown",                      ErrorUnknown)
            .export_values();

    py::enum_<UnitPfx_t>(m, "UnitPfx")
            .value("UnitPfxFemto",      UnitPfxFemto)
            .value("UnitPfxPico",       UnitPfxPico)
            .value("UnitPfxNano",       UnitPfxNano)
            .value("UnitPfxMicro",      UnitPfxMicro)
            .value("UnitPfxMilli",      UnitPfxMilli)
            .value("UnitPfxNone",       UnitPfxNone)
            .value("UnitPfxKilo",       UnitPfxKilo)
            .value("UnitPfxMega",       UnitPfxMega)
            .value("UnitPfxGiga",       UnitPfxGiga)
            .value("UnitPfxTera",       UnitPfxTera)
            .value("UnitPfxPeta",       UnitPfxPeta)
            .export_values();

    py::class_<Measurement_t>(m, "Measurement")
            .def(py::init<double, UnitPfx_t, std::string>())
            .def_readonly("value", &Measurement_t::value)
            .def_readonly("prefix", &Measurement_t::prefix)
            .def_readonly("unit", &Measurement_t::unit);
}
