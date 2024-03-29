#ifndef SFR_H
#define SFR_H

#include "types.h"

const tryte_t STATUS_REG_V;
	const tryte_t CARRY_TRIT_V;
	const tryte_t ZERO_BIT_V;
const tryte_t TRX_REG_V;
	const tryte_t TX_BIT_V;
	const tryte_t RX_BIT_V;
	const tryte_t TX_COUNT_L_TRIT_V;
const tryte_t BAUD_RATE_REG_V;
const tryte_t TX_REG_V;
const tryte_t RX_REG_V;
const tryte_t TIMER_OPT_REG_V;
const tryte_t TIMER_L_REG_V;
const tryte_t TIMER_H_REG_V;
const tryte_t PC_REG_V;
const tryte_t FS_REG_V;
const tryte_t IND_REG_V;


#define STATUS_REG 1
	#define CARRY_TRIT 0
	#define ZERO_BIT 1
#define TRX_REG 2
	#define TX_BIT 0
	#define RX_BIT 1
	#define TX_COUNT_L_TRIT 2
	#define TX_COUNT_H_TRIT 3
	#define RX_COUNT_L_TRIT 4
	#define RX_COUNT_H_TRIT 5
#define BAUD_RATE_REG 3 // TODO
#define TX_REG 4
#define RX_REG 5
#define TIMER_OPT_REG 6
	#define TIMER_SPEED_L_TRIT 0
	#define TIMER_SPEED_H_TRIT 1
	#define TIMER_ENABLE_BIT 2
#define TIMER_L_REG 7
#define TIMER_H_REG 8
#define PC_REG 9
#define FS_REG 10
#define IND_REG 11

#endif