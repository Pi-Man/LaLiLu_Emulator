#ifndef SFR_H
#define SFR_H

#define STATUS_REG 1 // TODO
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
#define TX_SELECT_REG 6 // TODO
#define RX_SELECT_REG 7 // TODO
#define TIMER_OPT_REG 8
	#define TIMER_SPEED_L_TRIT 0
	#define TIMER_SPEED_H_TRIT 1
	#define TIMER_ENABLE_BIT 2
#define TIMER_L_REG 9
#define TIMER_H_REG 10
#define PC_REG 11

#endif