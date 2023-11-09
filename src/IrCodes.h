// IrData.h

#ifndef IrCodes_H
#define IrCodes_H

#include <stdint.h>
#include <IRsend.h>


#define RAW_DATA_LEN 200
#define RAW_DATA_LEN_FAN 68
extern IRsend irsend;  //(15);

extern uint16_t AC23[RAW_DATA_LEN];
extern uint16_t AC24[RAW_DATA_LEN];
extern uint16_t AC25[RAW_DATA_LEN];
extern uint16_t AC26[RAW_DATA_LEN];
extern uint16_t AC27[RAW_DATA_LEN];
extern uint16_t AC28[RAW_DATA_LEN];
extern uint16_t AC29[RAW_DATA_LEN];
extern uint16_t AC0[RAW_DATA_LEN];

extern uint16_t SF1[RAW_DATA_LEN_FAN];
extern uint16_t SF2[RAW_DATA_LEN_FAN];
extern uint16_t SF3[RAW_DATA_LEN_FAN];
extern uint16_t SF4[RAW_DATA_LEN_FAN];
extern uint16_t SF5[RAW_DATA_LEN_FAN];
extern uint16_t SF0[RAW_DATA_LEN_FAN];

#endif
