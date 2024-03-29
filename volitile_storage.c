#include "volitile_storage.h"

#include <string.h>
#include <stdio.h>

#include "io.h"

tryte_t ram[RAM_SIZE];
tryte_t stack[STACK_SIZE];
tryte_t programMemory[PROGRAM_SIZE][5];

static int stackPointer = 0;

int init_volitile_storage() {
	if (!memset(ram, 0, sizeof(ram))) {
		return 1;
	}
	if (!memset(stack, 0, sizeof(stack))) {
		return 1;
	}
	if (!memset(programMemory, 0, sizeof(programMemory))) {
		return 1;
	}
	return 0;
}

tryte_t __ram_get(ADDRESS address) {
	if (memcmp(&address, &RX_REG_V, sizeof(tryte_t)) == 0) {
		return __io_read();
	}
	else if (memcmp(&address, &TX_REG_V, sizeof(tryte_t)) == 0) {
		return makeTryte(0);
	}
	else if (memcmp(&address, &IND_REG_V, sizeof(tryte_t)) == 0) {
		return ram[fromTryte(ram[FS_REG])];
	}
	else {
		return ram[fromTryte(address)];
	}
}

tryte_t __ram_get_(ADDRESS address) {
	return ram[fromTryte(address)];
}

void __ram_set(ADDRESS address, tryte_t value) {
	if (memcmp(&address, &RX_REG_V, sizeof(tryte_t)) == 0) {
		;
	}
	else if (memcmp(&address, &TX_REG_V, sizeof(tryte_t)) == 0) {
		__io_write(value);
	}
	else if (memcmp(&address, &IND_REG_V, sizeof(tryte_t)) == 0) {
		ram[fromTryte(ram[FS_REG])] = value;
	}
	else if (memcmp(&address, &TRX_REG_V, sizeof(tryte_t)) == 0 || memcmp(&address, &FS_REG_V, sizeof(tryte_t)) == 0 || fromTryte(address) > 27) {
		ram[fromTryte(address)] = value;
	}
}

void __ram_set_(ADDRESS address, tryte_t value) {
	ram[fromTryte(address)] = value;
}

trit_t __ram_get_trit(ADDRESS address, tryte_t trit) {
	unsigned short t = fromTryte(trit);
	if (t < TRYTE_TRITS) {
		if (memcmp(&address, &TRX_REG_V, sizeof(tryte_t)) == 0) {
			if (t == TX_BIT) {
				return __io_has_output();
			}
			else if (t == RX_BIT) {
				return __io_has_input();
			}
		}
		return __ram_get(address).trits[t];
	}
	return 0;
}

void __ram_set_trit(ADDRESS address, tryte_t trit, trit_t value) {
	unsigned short t = fromTryte(trit);
	if (t < TRYTE_TRITS) {
		tryte_t tryte = __ram_get(address);
		tryte.trits[t] = value;
		__ram_set(address, tryte);
	}
}

void __ram_set_trit_(ADDRESS address, tryte_t trit, trit_t value) {
	unsigned short t = fromTryte(trit);
	if (t < TRYTE_TRITS) {
		tryte_t tryte = __ram_get_(address);
		tryte.trits[t] = value;
		__ram_set_(address, tryte);
	}
}

void __stack_push(void) {
	if (stackPointer < 26) stackPointer++;
	stack[stackPointer] = ram[PC_REG];
}

void __stack_pop(void) {
	ram[PC_REG] = stack[stackPointer];
	if (stackPointer > 0) stackPointer--;
}
