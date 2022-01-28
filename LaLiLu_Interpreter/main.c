#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include <signal.h>
#include <stdlib.h>
#include <inttypes.h>



sig_atomic_t running = 1;

void interrupt_handler(int sig) {
	puts("force quitting");
	running = 0;
}

int readTrit(char *, FILE *);

typedef uint8_t TRIT;
typedef uint16_t TRYTE;
typedef uint32_t HTWORD;
typedef uint64_t TWORD;
typedef uint64_t INST;

#define MEMORY_SIZE 729
#define PROGRAM_SIZE 729
#define STACK_SIZE 27
#define INPUT_BUFFER_SIZE 27

#define STATUS_REG 1
	#define CARRY_TRIT 4
	#define ZERO_BIT 5
#define TRX_REG 2
	#define TX_BIT 0
	#define RX_BIT 1
	#define TX_COUNT_L_BIT 2
	#define TX_COUNT_H_BIT 3
	#define RX_COUNT_L_BIT 4
	#define RX_COUNT_H_BIT 5
#define RX_REG 3
#define TX_REG 4

TRYTE * memory;
HTWORD stack[27];
TRYTE stackPointer;
TRIT inputBuffer[27];
INST * programMemory;
HTWORD programCounter;
TRYTE printByte;

INST makeInstruction(TRYTE, TRYTE, TRYTE, TRYTE, TRYTE);
void decomposeInstruction(INST, TRYTE *);

void processInstruction(void);

void setMemory(TRYTE, TRYTE);
TRYTE getMemory(TRYTE);
TRIT getTrit(TRYTE, TRYTE);
TRYTE setTRIT(TRYTE, TRYTE, TRIT);
TWORD and(TWORD, TWORD);
TWORD or(TWORD, TWORD);
TWORD xor(TWORD, TWORD);
TWORD cmp(TWORD);
TWORD shiftL(TWORD, TRYTE);
TWORD shiftR(TWORD, TRYTE);
TRYTE rotateL(TRYTE, TRYTE);
TRYTE rotateR(TRYTE, TRYTE);
TRYTE rotateLC(TRYTE, TRYTE);
TRYTE rotateRC(TRYTE, TRYTE);

int main(int argc, char ** args) {

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

	if (!(memory = calloc(729, sizeof(TRYTE)))) {
		puts("could not initialize virtual memory");
		return 3;
	}
	puts("memory initialized");

	memset(stack, 0, sizeof(stack));
	puts("stack initialized");

	stackPointer = 0;
	puts("stack pointer initialized");

	memset(inputBuffer, 0, sizeof(inputBuffer));
	puts("input buffer initialized");

	if (!(programMemory = calloc(729, sizeof(TWORD)))) {
		puts("could not initialize virtual program memory");
		return 3;
	}
	puts("program memory initialized");

	programCounter = 0;
	puts("program counter initialized");

	int tritNum = 0;
	puts("loading program from file");
	int readerror;
	while ((readerror = readTrit(trit, file)) != EOF) {
		if (readerror) {
			printf("illformed program at trit %d, value was %s.  truncating program\n", tritNum, trit);
			break;
		}
		if (strncmp(trit, "ta", 2) == 0) {
			if (memory[0] != 0) {
				programMemory[programCounter] = makeInstruction(memory[1], memory[2], memory[3], memory[4], memory[5]);
				programCounter++;
			}
			memory[0] = 1;
			memory[1] = 0;
			memory[2] = 0;
			memory[3] = 0;
			memory[4] = 0;
			memory[5] = 0;
			programMemory[programCounter] = 0;
		}
		else if (strncmp(trit, "tu", 2) == 0) {
			memory[0]++;
		}
		else if (strncmp(trit, "la", 2) == 0) {
			memory[memory[0]] = shiftL(memory[memory[0]], 1) + 0;
		}
		else if (strncmp(trit, "li", 2) == 0) {
			memory[memory[0]] = shiftL(memory[memory[0]], 1) + 1;
		}
		else if (strncmp(trit, "lu", 2) == 0) {
			memory[memory[0]] = shiftL(memory[memory[0]], 1) + 2;
		}
		else {
			trit[3] = 0;
			printf("illformed program at trit %d, value was %s.  truncating program\n", tritNum, trit);
			break;
		}
		tritNum++;
	}
	programMemory[programCounter] = makeInstruction(memory[1], memory[2], memory[3], memory[4], memory[5]);
	memory[0] = 0;
	memory[1] = 0;
	memory[2] = 0;
	memory[3] = 0;
	memory[4] = 0;
	memory[5] = 0;
	puts("finished loading program from file");

	puts("running program");
	programCounter = 0;
	while (running) {
		if (getTrit(memory[TRX_REG], TX_BIT) == 2) {
			printByte = printByte / 3 + getTrit(memory[TX_REG], 0) * 243;
			memory[TX_REG] /= 3;
			TRYTE transmitCount = shiftR(memory[TRX_REG], TX_COUNT_L_BIT) % 9 + 1;
			if (transmitCount == 6) {
				memory[TRX_REG] = setTRIT(memory[TRX_REG], TX_BIT, 0);
				transmitCount = 0;
				printf("%d\n", printByte);
			}
			memory[TRX_REG] = setTRIT(memory[TRX_REG], TX_COUNT_H_BIT, transmitCount / 3);
			memory[TRX_REG] = setTRIT(memory[TRX_REG], TX_COUNT_L_BIT, transmitCount % 3);
		}
		processInstruction();
		programCounter++;
	}

	puts("program finished executing");

	return 0;
}

int readTrit(char * out, FILE * file) {
	char c;
	int i;
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
	TRYTE instruction[5];
	TRYTE * args = &(instruction[1]);
	decomposeInstruction(programMemory[programCounter], instruction);
	switch (instruction[0])
	{
	case 0:
		running = 0;
		break;
	case 1:
		memory[args[0]] = 0;
		break;
	case 2:
		memory[args[0]] = args[1];
		break;
	case 3:
		memory[args[1]] = memory[args[0]];
		if (args[1] == 728) printf("%d\n", memory[args[1]]); // TODO: change to serial output
		break;
	case 4:
		memory[args[0]] = rand();
		break;
	case 9:
		memory[args[2]] = memory[args[0]] + memory[args[1]];
		break;
	case 10:
		memory[args[2]] = memory[args[0]] - memory[args[1]];
		break;
	case 11:
		memory[args[2]] = memory[args[0]] * memory[args[1]];
		break;
	case 12:
		memory[args[2]] = memory[args[0]] / memory[args[1]];
		if (args[3]) memory[args[3]] = memory[args[0]] - memory[args[2]] * memory[args[1]];
		break;
	case 18:
		memory[args[0]]--;
		break;
	case 19:
		memory[args[0]]--;
		if (memory[args[0]] == 0) programCounter++;
		break;
	case 20:
		memory[args[0]]++;
		break;
	case 21:
		memory[args[0]]++;
		if (memory[args[0]] == 0) programCounter++;
		break;
	case 27:
		if (memory[args[0]] == 0) programCounter++;
		break;
	case 28:
		if (memory[args[0]] != 0) programCounter++;
		break;
	case 36:
		memory[args[2]] = and (memory[args[0]], memory[args[1]]);
		break;
	case 37:
		memory[args[2]] = or (memory[args[0]], memory[args[1]]);
		break;
	case 38:
		memory[args[2]] = xor (memory[args[0]], memory[args[1]]);
		break;
	case 39:
		memory[args[1]] = cmp(memory[args[0]]);
		break;
	case 40:
		memory[args[2]] = rotateL(memory[args[0]], memory[args[1]]);
		break;
	case 41:
		memory[args[2]] = rotateLC(memory[args[0]], memory[args[1]]);
		break;
	case 42:
		memory[args[2]] = rotateR(memory[args[0]], memory[args[1]]);
		break;
	case 43:
		memory[args[2]] = rotateRC(memory[args[0]], memory[args[1]]);
		break;
	case 45:
		memory[args[0]] = and (memory[args[0]], cmp(shiftL(2, args[1])));
		break;
	case 46:
		memory[args[0]] = and (memory[args[0]], cmp(shiftL(2, args[1]))) + shiftL(args[2], args[1]);
		break;
	case 47:
		if (and (shiftR(memory[args[0]], memory[args[1]]), 2) == 0) programCounter++;
		break;
	case 48:
		if (and (shiftR(memory[args[0]], memory[args[1]]), 2) != 0) programCounter++;
		break;
	case 49:
		if (and (shiftR(memory[args[0]], memory[args[1]]), 2) == 1) programCounter++;
		break;
	case 50:
		if (and (shiftR(memory[args[0]], memory[args[1]]), 2) != 1) programCounter++;
		break;
	case 51:
		if (and (shiftR(memory[args[0]], memory[args[1]]), 2) == 2) programCounter++;
		break;
	case 52:
		if (and (shiftR(memory[args[0]], memory[args[1]]), 2) != 2) programCounter++;
		break;
	case 54:
		stack[stackPointer] = programCounter;
		stackPointer = (stackPointer + 1) % 27;
		programCounter = args[0] - 1;
		break;
	case 55:
		programCounter = stack[stackPointer];
		stackPointer--;
		if (stackPointer < 0) stackPointer += 27;
		break;
	case 56:
		programCounter = args[0] - 1;
		break;
	case 57:
		programCounter += (args[0] == 0 ? args[1] : -args[1]) - 1;
	case 58:
		break;
	default:
		break;
	}
}

void setMemory(TRYTE address, TRYTE value) {
	if (address > 27 || address == TRX_REG || address == TX_REG) memory[address] = value;
}
TRYTE getMemory(TRYTE address) {
	return memory[address];
}
TRIT getTrit(TRYTE byte, TRYTE trit) {
	return shiftR(byte, trit) % 3;
}
TRYTE setTRIT(TRYTE byte, TRYTE trit, TRIT value) {
	return and (byte, cmp(shiftL(2, trit))) + shiftL(value, trit);
}
TWORD and(TWORD a, TWORD b) {
	short c = 0;
	int i = 1;
	for (; a > 0 && b > 0; a /= 3, b /= 3) {
		c += min(a % 3, b % 3) * i;
		i *= 3;
	}
	return c;
}
TWORD or(TWORD a, TWORD b) {
	short c = 0;
	int i = 1;
	for (; a > 0 && b > 0; a /= 3, b /= 3) {
		c += max(a % 3, b % 3) * i;
		i *= 3;
	}
	return c;
}
TWORD xor(TWORD a, TWORD b) {
	short c = 0;
	int i = 1;
	for (; a > 0 && b > 0; a /= 3, b /= 3) {
		c += min(max(a % 3, b % 3), 2 - min(a % 3, b % 3)) * i;
		i *= 3;
	}
	return c;
}
TWORD cmp(TWORD a) {
	short c = 0;
	int i = 1;
	for (; i < 729; a /= 3) {
		c += (2 - a % 3) * i;
		i *= 3;
	}
	return c;
}
TWORD shiftL(TWORD val, TRYTE trits) {
	while (trits) {
		val *= 3;
		trits--;
	}
	return val;
}
TWORD shiftR(TWORD val, TRYTE trits) {
	while (trits) {
		val /= 3;
		trits--;
	}
	return val;
}
TRYTE rotateL(TRYTE a, TRYTE b) {
	while (b) {
		short c = and (a, 486) / 243;
		a *= 3;
		a += c;
		b--;
	}
	return a;
}
TRYTE rotateR(TRYTE a, TRYTE b) {
	while (b) {
		short c = and (a, 2) * 243;
		a /= 3;
		a += c;
		b--;
	}
	return a;
}
TRYTE rotateLC(TRYTE a, TRYTE b) {
	while (b) {
		short c = and (a, 486) / 243;
		a *= 3;
		a += c;
		b--;
	}
	return a;
}
TRYTE rotateRC(TRYTE a, TRYTE b) {
	while (b) {
		short c = and (a, 2) * 243;
		a /= 3;
		a += c;
		b--;
	}
	return a;
}