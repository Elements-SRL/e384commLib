#include "emcr192blm_el03c_mb03_mez04_fw_v06.h"

Emcr192Blm_EL03c_Mb03_Mez04_fw_v06::Emcr192Blm_EL03c_Mb03_Mez04_fw_v06(std::string di) :
    Emcr192Blm_EL03c_Mb03_Mez04_fw_v05(di) {

    rxWordOffsets[RxMessageTemperature] = rxWordOffsets[RxMessageStatus] + rxWordLengths[RxMessageStatus];
    rxWordLengths[RxMessageTemperature] = 2;

    temperatureChannelsNum = TemperatureChannelsNum;

    temperatureChannelsNames.resize(temperatureChannelsNum);
    temperatureChannelsNames[TemperatureSensor0] = "Sensor0";
    temperatureChannelsNames[TemperatureSensor1] = "Sensor1";

    temperatureChannelsRanges.resize(temperatureChannelsNum);
    temperatureChannelsRanges[TemperatureSensor0].step = 0.25;
    temperatureChannelsRanges[TemperatureSensor0].min = -8192.0;
    temperatureChannelsRanges[TemperatureSensor0].max = temperatureChannelsRanges[TemperatureSensor0].min+temperatureChannelsRanges[TemperatureSensor0].step*USHORT_MAX;
    temperatureChannelsRanges[TemperatureSensor0].prefix = UnitPfxNone;
    temperatureChannelsRanges[TemperatureSensor0].unit = "°C";
    temperatureChannelsRanges[TemperatureSensor1].step = 0.25;
    temperatureChannelsRanges[TemperatureSensor1].min = -8192.0;
    temperatureChannelsRanges[TemperatureSensor1].max = temperatureChannelsRanges[TemperatureSensor1].min+temperatureChannelsRanges[TemperatureSensor1].step*USHORT_MAX;
    temperatureChannelsRanges[TemperatureSensor1].prefix = UnitPfxNone;
    temperatureChannelsRanges[TemperatureSensor1].unit = "°C";
}
