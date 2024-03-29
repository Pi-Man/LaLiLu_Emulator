#include "SFR.h"

const tryte_t STATUS_REG_V = { .trits = {0, 0, 0, 0, 0, 1} };
const tryte_t CARRY_TRIT_V = { .trits = {0, 0, 0, 0, 0, 0} };
const tryte_t ZERO_BIT_V = { .trits = {0, 0, 0, 0, 0, 1} };
const tryte_t TRX_REG_V = { .trits = {0, 0, 0, 0, 0, 2} };
const tryte_t TX_BIT_V = { .trits = {0, 0, 0, 0, 0, 0} };
const tryte_t RX_BIT_V = { .trits = {0, 0, 0, 0, 0, 1} };
const tryte_t TX_COUNT_L_TRIT_V = { .trits = {0, 0, 0, 0, 0, 2} };
const tryte_t BAUD_RATE_REG_V = { .trits = {0, 0, 0, 0, 1, 0} };
const tryte_t TX_REG_V = { .trits = {0, 0, 0, 0, 1, 1} };
const tryte_t RX_REG_V = { .trits = {0, 0, 0, 0, 1, 2} };
const tryte_t TIMER_OPT_REG_V = { .trits = {0, 0, 0, 0, 2, 0} };
const tryte_t TIMER_L_REG_V = { .trits = {0, 0, 0, 0, 2, 1} };
const tryte_t TIMER_H_REG_V = { .trits = {0, 0, 0, 0, 2, 2} };
const tryte_t PC_REG_V = { .trits = {0, 0, 0, 1, 0, 0} };
const tryte_t FS_REG_V = { .trits = {0, 0, 0, 1, 0, 1} };
const tryte_t IND_REG_V = { .trits = {0, 0, 0, 1, 0, 2} };