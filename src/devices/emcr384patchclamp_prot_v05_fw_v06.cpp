#include "emcr384patchclamp_prot_v05_fw_v06.h"

Emcr384PatchClamp_prot_v05_fw_v06::Emcr384PatchClamp_prot_v05_fw_v06(std::string di) :
    Emcr384PatchClamp_prot_v04_fw_v05(di) {

    rxWordOffsets[RxMessageDataLoad] = 0;
    rxWordLengths[RxMessageDataLoad] = (voltageChannelsNum+currentChannelsNum)*packetsPerFrame;

    rxWordOffsets[RxMessageDataHeader] = rxWordOffsets[RxMessageDataLoad] + rxWordLengths[RxMessageDataLoad];
    rxWordLengths[RxMessageDataHeader] = 6;

    rxWordOffsets[RxMessageDataTail] = 0xFFFF;
    rxWordLengths[RxMessageDataTail] = 0xFFFF;

    rxWordOffsets[RxMessageStatus] = rxWordOffsets[RxMessageDataHeader] + rxWordLengths[RxMessageDataHeader];
    rxWordLengths[RxMessageStatus] = 2;
}
