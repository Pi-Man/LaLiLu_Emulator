#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include <signal.h>
#include <stdlib.h>
#include <inttypes.h>

// TODO: MAKE MODULES (timer, io, program, ALU, clock(s), ...)
// io done, ALU done, volitile storage done

#include "../device_ports.h"
#include "../io.h"
#include "../volitile_storage.h"
#include "../SFR.h"
#include "../ALU.h"

sig_atomic_t running = 1;

void interrupt_handler(int sig) {
	puts("\nforce quitting");
	running = 0;
}

int readTrit(char *, FILE *);

INST makeInstruction(TRYTE, TRYTE, TRYTE, TRYTE, TRYTE);
void decomposeInstruction(INST, TRYTE *);

void processInstruction(void);

#include "../device_ports.h"
#include "../io.h"

inline void shiftLeft(tryte_t *tryte, trit_t trit) {
	memmove(tryte, 1 + (char*)tryte, TRYTE_TRITS - 1);
	tryte->trits[TRYTE_TRITS - 1] = trit;
}

inline void shiftRight(tryte_t *tryte, trit_t trit) {
	memmove(sizeof(trit_t) + (char*)tryte, tryte, TRYTE_TRITS - 1);
	tryte->trits[0] = trit;
}

inline void incPC(void) {
	ram[PC_REG].trits[TRYTE_TRITS - 1]++;
	for (int i = TRYTE_TRITS - 2; i >= 0 && ram[PC_REG].trits[i + 1] >= 3; i--) {
		ram[PC_REG].trits[i + 1] = 0;
		ram[PC_REG].trits[i]++;
	}
}

int main(int argc, char ** args) {

	//TODO change to open using stdio
	//toggleDevice("RawInput.exe");
	toggleDevice("Keyboard.exe");
	toggleDevice("CharacterPrinter.exe");

	for (int _ = 0; _ < 100000; _++);

	if (argc != 2) {
		puts("invalid args, must privide only a single file");
		return 1;
	}

	signal(SIGINT, interrupt_handler);

	FILE * file = fopen(args[1], "r");

	if (!file) {
		printf("could not open file: %s\n", args[1]);
		return 2;
	}

	char trit[4];

	init_volitile_storage();

	start_USRT();
	puts("started USSRT");

	int tritNum = 0;
	puts("loading program from file");

	int readerror;
	// TODO read from virtual harddrive memory, emulated as a file     apparently this is what the BIOS is
	while ((readerror = readTrit(trit, file)) != EOF) {
		if (readerror) {
			printf("illformed program at trit %d, value was %s.  truncating program\n", tritNum, trit);
			break;
		}
		if (strncmp(trit, "ta", 2) == 0) {
			if (fromTryte(ram[0]) != 0) {
				unsigned short PC = fromTryte(ram[PC_REG]);
				programMemory[PC][0] = ram[1];
				programMemory[PC][1] = ram[2];
				programMemory[PC][2] = ram[3];
				programMemory[PC][3] = ram[4];
				programMemory[PC][4] = ram[5];
				incPC();
			}
			ram[0] = makeTryte(1);
			ram[1] = makeTryte(0);
			ram[2] = makeTryte(0);
			ram[3] = makeTryte(0);
			ram[4] = makeTryte(0);
			ram[5] = makeTryte(0);
		}
		else if (strncmp(trit, "tu", 2) == 0) {
			ram[0] = __add(ram[0], makeTryte(1));
		}
		else if (strncmp(trit, "la", 2) == 0) {
			shiftLeft(&ram[fromTryte(ram[0])], 0);
		}
		else if (strncmp(trit, "li", 2) == 0) {
			shiftLeft(&ram[fromTryte(ram[0])], 1);
		}
		else if (strncmp(trit, "lu", 2) == 0) {
			shiftLeft(&ram[fromTryte(ram[0])], 2);
		}
		else {
			trit[3] = 0;
			printf("illformed program at trit %d, value was %s.  truncating program\n", tritNum, trit);
			break;
		}
		tritNum++;
	}
	unsigned short PC = fromTryte(ram[PC_REG]);
	programMemory[PC][0] = ram[1];
	programMemory[PC][1] = ram[2];
	programMemory[PC][2] = ram[3];
	programMemory[PC][3] = ram[4];
	programMemory[PC][4] = ram[5];
	ram[0] = makeTryte(0);
	ram[1] = makeTryte(0);
	ram[2] = makeTryte(0);
	ram[3] = makeTryte(0);
	ram[4] = makeTryte(0);
	ram[5] = makeTryte(0);
	puts("finished loading program from file");

	puts("running program");
	ram[PC_REG] = makeTryte(0);
#ifndef _WIN32
  setImmediate(1);
#endif

	while (running) {
		processInstruction();
		incPC();
	}
#ifndef _WIN32
  setImmediate(0);
#endif

	puts("program finished executing");

	getchar();

	return 0;
}

inline int readTrit(char * out, FILE * file) {
	char c;
	while (isspace((c = fgetc(file))));
	out[0] = c;
	out[1] = fgetc(file);
	if (out[1] == EOF) return EOF;
	if (isspace(out[1]) || !isspace(fgetc(file))) return 1;
	return 0;
}

INST makeInstruction(TRYTE id, TRYTE arg1, TRYTE arg2, TRYTE arg3, TRYTE arg4) {
	return (INST)id * 729ull * 729ull * 729ull * 729ull + (INST)arg1 * 729ull * 729ull * 729ull + (INST)arg2 * 729ull * 729ull + (INST)arg3 * 729ull + (INST)arg4;
}
void decomposeInstruction(INST instruction, TRYTE * out) {
	out[0] = (instruction / (729ull * 729ull * 729ull * 729ull)) % 729;
	out[1] = (instruction / (729ull * 729ull * 729ull)) % 729;
	out[2] = (instruction / (729ull * 729ull)) % 729;
	out[3] = (instruction / (729ull)) % 729;
	out[4] = instruction % 729;
}

void processInstruction(void) {
	tryte_t * instruction = &programMemory[fromTryte(ram[PC_REG])];
	tryte_t * args = &(instruction[1]);
	___mul_t mul_res;
	___div_t div_res;
	switch (fromTryte(instruction[0]))
	{
	case 0:
		running = 0;
		break;
	case 1:
		__ram_set(args[0], makeTryte(0));
		break;
	case 2:
		__ram_set(args[0], args[1]);
		break;
	case 3:
		__ram_set(args[1], __ram_get(args[0]));
		break;
	case 4:
		__ram_set(args[0], makeTryte(rand() % 729));
		break;
	case 9:
		__ram_set(args[2], __add(__ram_get(args[0]), __ram_get(args[1])));
		break;
	case 10:
		__ram_set(args[2], __sub(__ram_get(args[0]), __ram_get(args[1])));
		break;
	case 11:
		mul_res = __mul(__ram_get(args[0]), __ram_get(args[1]));
		__ram_set(args[2], mul_res.l);
		__ram_set(args[3], mul_res.h);
		break;
	case 12:
		div_res = __div(__ram_get(args[0]), __ram_get(args[1]));
		__ram_set(args[2], div_res.qot);
		__ram_set(args[3], div_res.rem);
		break;
	case 18:
		__ram_set(args[0], __sub(__ram_get(args[0]), makeTryte(1)));
		break;
	case 19:
		__ram_set(args[0], __sub(__ram_get(args[0]), makeTryte(1)));
		if (__ram_get_trit(STATUS_REG_V, ZERO_BIT_V) == 2) incPC();
		break;
	case 20:
		__ram_set(args[0], __add(__ram_get(args[0]), makeTryte(1)));
		break;
	case 21:
		__ram_set(args[0], __add(__ram_get(args[0]), makeTryte(1)));
		if (__ram_get_trit(STATUS_REG_V, ZERO_BIT_V) == 2) incPC();
		break;
	case 27:
		if (fromTryte(__ram_get(args[0])) == 0) incPC();
		break;
	case 28:
		if (fromTryte(__ram_get(args[0])) != 0) incPC();
		break;
	case 36:
		__ram_set(args[2], __and(__ram_get(args[0]), __ram_get(args[1])));
		break;
	case 37:
		__ram_set(args[2], __or(__ram_get(args[0]), __ram_get(args[1])));
		break;
	case 38:
		__ram_set(args[2], __xor(__ram_get(args[0]), __ram_get(args[1])));
		break;
	case 39:
		__ram_set(args[1], __cmp(__ram_get(args[0])));
		break;
	case 40:
		__ram_set(args[2], __rot_l(__ram_get(args[0]), __ram_get(args[1])));
		break;
	case 41:
		__ram_set(args[2], __rot_l_c(__ram_get(args[0]), __ram_get(args[1])));
		break;
	case 42:
		__ram_set(args[2], __rot_r(__ram_get(args[0]), __ram_get(args[1])));
		break;
	case 43:
		__ram_set(args[2], __rot_r_c(__ram_get(args[0]), __ram_get(args[1])));
		break;
	case 45:
		__ram_set_trit(args[0], args[1], 0);
		break;
	case 46:
		__ram_set_trit(args[0], args[1], args[2].trits[TRYTE_TRITS - 1]);
		break;
	case 47:
		if (__ram_get_trit(args[0], args[1]) == 0) incPC();
		break;
	case 48:
		if (__ram_get_trit(args[0], args[1]) != 0) incPC();
		break;
	case 49:
		if (__ram_get_trit(args[0], args[1]) == 1) incPC();
		break;
	case 50:
		if (__ram_get_trit(args[0], args[1]) != 1) incPC();
		break;
	case 51:
		if (__ram_get_trit(args[0], args[1]) == 2) incPC();
		break;
	case 52:
		if (__ram_get_trit(args[0], args[1]) != 2) incPC();
		break;
	case 54:
		__stack_push();
		ram[PC_REG] = __sub(args[0], makeTryte(1));
		break;
	case 55:
		__stack_pop();
		break;
	case 56:
		ram[PC_REG] = __sub(args[0], makeTryte(1));
		break;
	case 57:
		ram[PC_REG] = makeTryte(fromTryte(ram[PC_REG]) + (fromTryte(args[0]) == 0 ? fromTryte(args[1]) : -fromTryte(args[1])) - 1);
	case 58:
		break;
	default:
		break;
	}
}