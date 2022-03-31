#include "volitile_storage.h"

#include <string.h>
#include <stdio.h>

#include "io.h"

TRYTE ram[RAM_SIZE];
TRYTE stack[STACK_SIZE];
INST programMemory[PROGRAM_SIZE];

static int stackPointer = 0;

int init_volitile_storage() {
	if (!memset(ram, 0, sizeof(TRYTE) * RAM_SIZE)) {
		return 1;
	}
	if (!memset(stack, 0, sizeof(TRYTE) * STACK_SIZE)) {
		return 1;
	}
	if (!memset(programMemory, 0, sizeof(TRYTE) * PROGRAM_SIZE)) {
		return 1;
	}
	return 0;
}

void init_ram() {

	memset(ram, 0, sizeof(ram));
	puts("memory initialized");

	memset(stack, 0, sizeof(stack));
	puts("stack initialized");

	memset(programMemory, 0, sizeof(programMemory));
	puts("program memory initialized");

}

TRYTE __ram_get(ADDRESS address) {
	switch (address) {
	case RX_REG:
		return __io_read();
	case TX_REG:
		return 0;
	default:
		return ram[address];
	}
}

void __ram_set(ADDRESS address, TRYTE value) {
	switch (address) {
	case RX_REG:
		break;
	case TX_REG:
		__io_write(value);
		break;
	default:
		if (address > 27 || address == TRX_REG) ram[address] = value;
	}
}

TRIT __ram_get_trit(ADDRESS address, TRYTE trit) {
	if (address == TRX_REG && trit == RX_BIT) {
		return __io_has_input();
	}
	else if (address == TRX_REG && trit == TX_BIT) {
		return __io_has_output();
	}
	TRYTE copy = ram[address];
	while (trit) {
		copy /= 3;
		trit--;
	}
	return copy % 3;
}

void __ram_set_trit(ADDRESS address, TRYTE trit, TRIT value) {
	TRYTE shiftVal = 1;
	while (trit) {
		shiftVal *= 3;
		trit--;
	}
	TRYTE h = ram[address] - ram[address] % (shiftVal * 3);
	TRYTE t = value * shiftVal;
	TRYTE l = ram[address] % shiftVal;
	ram[address] = h + t + l;
}

void __stack_push(void) {
	if (stackPointer < 26) stackPointer++;
	stack[stackPointer] = ram[PC_REG];
}

void __stack_pop(void) {
	ram[PC_REG] = stack[stackPointer];
	if (stackPointer > 0) stackPointer--;
}
