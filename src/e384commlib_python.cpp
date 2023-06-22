#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "messagedispatcher.h"

namespace py = pybind11;
using namespace e384CommLib;

static MessageDispatcher * md;
static int16_t * data;

static int voltageChannelsNum;
static int currentChannelsNum;
static int totalChannelsNum;
static std::vector<std::vector<double>> voltageDataOut;
static std::vector<std::vector<double>> currentDataOut;
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

    m.def("connectDevice",[](std::string deviceName){
        ErrorCodes_t ret = md->allocateRxDataBuffer(data);
        if (ret != Success) {
            return ret;
        }
        ret = MessageDispatcher::connectDevice(deviceName, md);
        if (ret != Success) {
            return ret;
        }
        uint16_t vNum;
        uint16_t cNum;
        ret = md->getChannelNumberFeatures(vNum, cNum);
        if (ret != Success) {
            return ret;
        }
        voltageChannelsNum = static_cast<int>(vNum);
        currentChannelsNum = static_cast<int>(cNum);
        totalChannelsNum = currentChannelsNum + voltageChannelsNum;
        return ret;
    }, "Connect to one of the plugged in device");

    m.def("getSamplingRate",[](){
        std::vector <Measurement_t> samplingRates;
        ErrorCodes_t err = md->getSamplingRatesFeatures(samplingRates);
        return  std::make_tuple(err, samplingRates);
    }, "Get all the sampling rate the device can handle");

    m.def("setVoltageHoldTuner",[](std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> voltages, bool applyFlag){
        return md->setVoltageHoldTuner(channelIndexes, voltages, applyFlag);
    });

    m.def("getBufferedVoltagesAndCurrents", [](){
        RxOutput_t rxOutput;
        ErrorCodes_t err = md->getNextMessage(rxOutput, data);
        voltageDataOut.resize(rxOutput.dataLen/totalChannelsNum);
        for (int i=0; i < voltageDataOut.size(); i++) {
            voltageDataOut[i].resize(voltageChannelsNum);
        }
        currentDataOut.resize(rxOutput.dataLen/totalChannelsNum);
        for (int i=0; i < currentDataOut.size(); i++) {
            currentDataOut[i].resize(currentChannelsNum);
        }
        if (err!= Success){
            return std::make_tuple(err, voltageDataOut, currentDataOut);
        }
        if (rxOutput.msgTypeId != MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData){
            return std::make_tuple(ErrorNoDataAvailable, voltageDataOut, currentDataOut);
        }
        for (unsigned long wordsIdx = 0; wordsIdx < rxOutput.dataLen; wordsIdx += totalChannelsNum) {
            for (int chIdx = 0; chIdx < voltageChannelsNum; chIdx++) {
                md->convertVoltageValue(data[wordsIdx+chIdx], voltageDataOut[wordsIdx/totalChannelsNum][chIdx]);
            }
            for (int chIdx = 0; chIdx < currentChannelsNum; chIdx++) {
                md->convertCurrentValue(data[wordsIdx+chIdx+voltageChannelsNum], currentDataOut[wordsIdx/totalChannelsNum][chIdx]);
            }
        }
        return std::make_tuple(err, voltageDataOut, currentDataOut);
    }, "Get buffered voltage and current values");

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

//    py::class_<RxOutput>(m, "RxOutput")
//            .def(py::init<const std::string &>())
//            .def_readonly("msgTypeId", &RxOutput::msgTypeId)
//            .def_readonly("channelIdx", &RxOutput::channelIdx)
//            .def_readonly("protocolId", &RxOutput::protocolId)
//            .def_readonly("protocolItemIdx", &RxOutput::protocolItemIdx)
//            .def_readonly("protocolRepsIdx", &RxOutput::protocolRepsIdx)
//            .def_readonly("protocolSweepIdx", &RxOutput::protocolSweepIdx)
//            .def_readonly("firstSampleOffset", &RxOutput::firstSampleOffset)
//            .def_readonly("dataLen", &RxOutput::dataLen);
}
