#ifndef FTDIUTILS_H
#define FTDIUTILS_H

#include "ftdieeprom.h"
#include "ftdieeprom56.h"
#include "ftdieepromdemo.h"

/*! \todo FCON molte di queste cose sono specifiche per EZ Patch e vanno tolte da qui */

#define FTD_RX_WORD_SIZE (sizeof(uint16_t)) // 16 bit word
#define FTD_RX_SYNC_WORD_SIZE (FTD_RX_WORD_SIZE) // could be different size
#define FTD_RX_CRC_WORD_SIZE (FTD_RX_WORD_SIZE) // could be different size but requires several modifications, e.g. data type and algo
#define FTD_RX_HB_TY_LN_SIZE (FTD_RX_WORD_SIZE*3) // size of heartbeat + type ID + length, computed here just once for efficience
#define FTD_RX_RAW_BUFFER_SIZE 0x100000 /*! \todo FCON valutare che questo numero sia adeguato */ // ~1M
#define FTD_RX_RAW_BUFFER_MASK (FTD_RX_RAW_BUFFER_SIZE-1) // 0b11...1 for all bits of the buffer indexes
#define FTD_RX_HEARTBEAT_MASK 0xFFFF // mask for 16 bit word

#define FTD_TX_WORD_SIZE (sizeof(uint16_t)) // 16 bit word
#define FTD_TX_SYNC_WORD_SIZE (FTD_TX_WORD_SIZE) // could be different size
#define FTD_TX_CRC_WORD_SIZE (FTD_TX_WORD_SIZE) // could be different size but requires several modifications, e.g. data type and algo
#define FTD_TX_HB_TY_LN_SIZE (FTD_TX_WORD_SIZE*3) // size of heartbeat + type ID + length, computed here just once for efficience
#define FTD_TX_RAW_BUFFER_SIZE 0x1000 /*! \todo FCON valutare che questo numero sia adeguato */ // ~4k
#define FTD_TX_RAW_BUFFER_MASK (FTD_TX_RAW_BUFFER_SIZE-1) // 0b11...1 for all bits of the buffer indexes
#define FTD_TX_HEARTBEAT_MASK 0xFFFF // mask for 16 bit word
#define FTD_BYTES_TO_WRITE_ALWAYS (FTD_TX_SYNC_WORD_SIZE+FTD_TX_HB_TY_LN_SIZE+FTD_TX_CRC_WORD_SIZE*2)

#define FTD_FRAME_SIZE (FTD_RX_SYNC_WORD_SIZE+FTD_RX_HB_TY_LN_SIZE+2*FTD_RX_CRC_WORD_SIZE) // could be different size

typedef enum {
    FtdiFpgaFwLoadAutomatic,
    FtdiFpgaFwLoad8PatchArtix7_V01
} FtdiFpgaLoadType_t;

#endif // FTDIUTILS_H
