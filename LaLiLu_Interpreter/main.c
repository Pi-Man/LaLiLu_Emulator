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

typedef uint16_t BYTE;
typedef uint32_t HWORD;
typedef uint64_t WORD;
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
#define RX_REG 3
#define TX_REG 4

BYTE * memory;
HWORD stack[27];
BYTE stackPointer;
char inputBuffer[27];
INST * programMemory;
HWORD programCounter;

INST makeInstruction(BYTE, BYTE, BYTE, BYTE, BYTE);
void decomposeInstruction(INST, BYTE *);

void setMemory(BYTE, BYTE);
BYTE getMemory(BYTE);
WORD and(WORD, WORD);
WORD or(WORD, WORD);
WORD xor(WORD, WORD);
WORD cmp(WORD);
WORD shiftL(WORD, BYTE);
WORD shiftR(WORD, BYTE);
BYTE rotateL(BYTE, BYTE);
BYTE rotateR(BYTE, BYTE);
BYTE rotateLC(BYTE, BYTE);
BYTE rotateRC(BYTE, BYTE);

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

	if (!(memory = calloc(729, sizeof(BYTE)))) {
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

	if (!(programMemory = calloc(729, sizeof(WORD)))) {
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
				programMemory[programCounter] = makeInstruction(getMemory(1), getMemory(2), getMemory(3), getMemory(4), getMemory(5));
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
	programMemory[programCounter] = shiftL(memory[1], 4 * 6) + shiftL(memory[2], 3 * 6) + shiftL(memory[3], 2 * 6) + shiftL(memory[4], 1 * 6) + memory[5];
	memory[0] = 0;
	memory[1] = 0;
	memory[2] = 0;
	memory[3] = 0;
	memory[4] = 0;
	memory[5] = 0;
	programCounter++;
	programMemory[programCounter] = 0;
	puts("finished loading program from file");
	//for (int i = 0; i < programCounter; i++) {
	//	printf("%zd\n", programMemory[i]);
	//}
	puts("running program");
	programCounter = 0;
	while (running) {
		BYTE instruction[5];
		BYTE * args = &(instruction[1]);
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
			memory[args[2]] = cmp(memory[args[0]]);
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
			memory[args[0]] = and (memory[args[0]], cmp(shiftL(2 - args[2], args[1])));
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
			break;
		default:
			break;
		}
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

INST makeInstruction(BYTE id, BYTE arg1, BYTE arg2, BYTE arg3, BYTE arg4) {
	return (INST)id * 3 * 6 * 4 + (INST)arg1 * 3 * 6 * 3 + (INST)arg2 * 3 * 6 * 2 + (INST)arg3 * 3 * 6 + (INST)arg4;
}
void decomposeInstruction(INST instruction, BYTE * out) {
	out[0] = instruction / (3 * 6 * 4) % 729;
	out[1] = instruction / (3 * 6 * 3) % 729;
	out[2] = instruction / (3 * 6 * 2) % 729;
	out[3] = instruction / (3 * 6) % 729;
	out[4] = instruction % 729;
}

void setMemory(BYTE address, BYTE value) {
	if (address > 27) memory[address] = value;
}
BYTE getMemory(BYTE address) {
	return memory[address];
}
WORD and(WORD a, WORD b) {
	short c = 0;
	int i = 1;
	for (; a > 0 && b > 0; a /= 3, b /= 3) {
		c += min(a % 3, b % 3) * i;
		i *= 3;
	}
	return c;
}
WORD or(WORD a, WORD b) {
	short c = 0;
	int i = 1;
	for (; a > 0 && b > 0; a /= 3, b /= 3) {
		c += max(a % 3, b % 3) * i;
		i *= 3;
	}
	return c;
}
WORD xor(WORD a, WORD b) {
	short c = 0;
	int i = 1;
	for (; a > 0 && b > 0; a /= 3, b /= 3) {
		c += min(max(a % 3, b % 3), 2 - min(a % 3, b % 3)) * i;
		i *= 3;
	}
	return c;
}
WORD cmp(WORD a) {
	short c = 0;
	int i = 1;
	for (; a > 0; a /= 3) {
		c += (2 - a % 3) * i;
		i *= 3;
	}
	return c;
}
WORD shiftL(WORD val, BYTE trits) {
	while (trits) {
		val *= 3;
		trits--;
	}
	return val;
}
WORD shiftR(WORD val, BYTE trits) {
	while (trits) {
		val /= 3;
		trits--;
	}
	return val;
}
BYTE rotateL(BYTE a, BYTE b) {
	while (b) {
		short c = and (a, 486) / 243;
		a *= 3;
		a += c;
		b--;
	}
	return a;
}
BYTE rotateR(BYTE a, BYTE b) {
	while (b) {
		short c = and (a, 2) * 243;
		a /= 3;
		a += c;
		b--;
	}
	return a;
}
BYTE rotateLC(BYTE a, BYTE b) {
	while (b) {
		short c = and (a, 486) / 243;
		a *= 3;
		a += c;
		b--;
	}
	return a;
}
BYTE rotateRC(BYTE a, BYTE b) {
	while (b) {
		short c = and (a, 2) * 243;
		a /= 3;
		a += c;
		b--;
	}
	return a;
}