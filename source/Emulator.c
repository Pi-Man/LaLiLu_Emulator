#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include <signal.h>
#include <stdlib.h>
#include <inttypes.h>

// TODO: MAKE MODULES (timer, io, program, ALU, clock(s), ...)

#include "../device_ports.h"
#include "../io.h"
#include "../volitile_storage.h"
#include "../SFR.h"
#include "../ALU.h"

#ifdef _WIN32
#include <Windows.h>
#include <conio.h>
#else
// modified from
// https://cboard.cprogramming.com/c-programming/63166-kbhit-linux-post449307.html#post449307
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

void setImmediate(int set) {
  static struct termios oldt, newt;
  static int oldf;
  static int firstCall = 1;

  if (firstCall) {
    tcgetattr(STDIN_FILENO, &oldt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    firstCall = 0;
  }
  if (set) {
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
  }
  else {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
  }
}
 
int _kbhit(void) {
  int ch;
  ch = getchar();
  if(ch == 0 || ch == -1) {
    return 0;
  }
  ungetc(ch, stdin);
  return 1;
}

int _getch(void) {
  return getchar();
}

void _putch(int c) {
  putc(c, stdout);
}

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

sig_atomic_t running = 1;

void interrupt_handler(int sig) {
	puts("\nforce quitting");
	running = 0;
}

int readTrit(char *, FILE *);

//char inputDisplayString[14];
//char * inputDisplayStringStart = inputDisplayString + 11;
//
//typedef uint8_t TRIT;
//typedef uint16_t TRYTE;
//typedef uint32_t HTWORD;
//typedef uint64_t TWORD;
//typedef uint64_t INST;
//
//#define MEMORY_SIZE 729
//#define PROGRAM_SIZE 729
//#define STACK_SIZE 27
//#define INPUT_BUFFER_SIZE 27
//
//#define STATUS_REG 1 // TODO
//	#define CARRY_TRIT 0
//	#define ZERO_BIT 1
//#define TRX_REG 2
//	#define TX_BIT 0
//	#define RX_BIT 1
//	#define TX_COUNT_L_TRIT 2
//	#define TX_COUNT_H_TRIT 3
//	#define RX_COUNT_L_TRIT 4
//	#define RX_COUNT_H_TRIT 5
//#define TX_REG 3
//#define RX_REG 4
//#define TX_SELECT 5 // TODO
//#define RX_SELECT 6 // TODO
//#define TIMER_OPT_REG 7
//	#define	TIMER_SPEED_L_TRIT 0
//	#define	TIMER_SPEED_H_TRIT 1
//	#define TIMER_ENABLE_BIT 2
//#define TIMER_L_REG 8
//#define TIMER_H_REG 9
//#define PC_REG 10
//
//TRYTE timerSpeed;

//TRYTE * memory;
//HTWORD stack[27];
//TRYTE stackPointer;
//TRIT inputBuffer[27];
//INST * programMemory;
//#define programCounter memory[PC_REG]
//TRYTE outputTryte;
//TRYTE inputTryte;

INST makeInstruction(TRYTE, TRYTE, TRYTE, TRYTE, TRYTE);
void decomposeInstruction(INST, TRYTE *);

void processInstruction(void);

//void setMemory(TRYTE, TRYTE);
//TRYTE getMemory(TRYTE);
//TRIT getTrit(TRYTE, TRYTE);
//TRYTE setTRIT(TRYTE, TRYTE, TRIT);
//TWORD and(TWORD, TWORD);
//TWORD or(TWORD, TWORD);
//TWORD xor(TWORD, TWORD);
//TWORD cmp(TWORD);
//TWORD shiftL(TWORD, TRYTE);
//TWORD shiftR(TWORD, TRYTE);
//TRYTE rotateL(TRYTE, TRYTE);
//TRYTE rotateR(TRYTE, TRYTE);
//TRYTE rotateLC(TRYTE, TRYTE);
//TRYTE rotateRC(TRYTE, TRYTE);

#include "../device_ports.h"
#include "../io.h"

int main(int argc, char ** args) {

	toggleDevice("RawInput.exe");
	toggleDevice("Keyboard.exe");
	toggleDevice("ValuePrinter.exe");

	getchar();

  //inputDisplayString[13] = 0;

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

	//if (!(memory = calloc(729, sizeof(TRYTE)))) {
	//	puts("could not initialize virtual memory");
	//	return 3;
	//}
	//puts("memory initialized");

	//memset(stack, 0, sizeof(stack));
	//puts("stack initialized");

	//stackPointer = 0;
	//puts("stack pointer initialized");

	//memset(inputBuffer, 0, sizeof(inputBuffer));
	//puts("input buffer initialized");

	//if (!(programMemory = calloc(729, sizeof(TWORD)))) {
	//	puts("could not initialize virtual program memory");
	//	return 3;
	//}
	//puts("program memory initialized");

	//programCounter = 0;
	//puts("program counter initialized");

	init_ram();

	start_USRT();
	puts("started USSRT");

	int tritNum = 0;
	puts("loading program from file");

	int readerror;
	// TODO read from virtual harddrive memory, emulated as a file
	while ((readerror = readTrit(trit, file)) != EOF) {
		if (readerror) {
			printf("illformed program at trit %d, value was %s.  truncating program\n", tritNum, trit);
			break;
		}
		if (strncmp(trit, "ta", 2) == 0) {
			if (ram[0] != 0) {
				programMemory[ram[PC_REG]] = makeInstruction(ram[1], ram[2], ram[3], ram[4], ram[5]);
				ram[PC_REG]++;
			}
			ram[0] = 1;
			ram[1] = 0;
			ram[2] = 0;
			ram[3] = 0;
			ram[4] = 0;
			ram[5] = 0;
			programMemory[ram[PC_REG]] = 0;
		}
		else if (strncmp(trit, "tu", 2) == 0) {
			ram[0]++;
		}
		else if (strncmp(trit, "la", 2) == 0) {
			ram[ram[0]] = ram[ram[0]] * 3 + 0;
		}
		else if (strncmp(trit, "li", 2) == 0) {
			ram[ram[0]] = ram[ram[0]] * 3 + 1;
		}
		else if (strncmp(trit, "lu", 2) == 0) {
			ram[ram[0]] = ram[ram[0]] * 3 + 2;
		}
		else {
			trit[3] = 0;
			printf("illformed program at trit %d, value was %s.  truncating program\n", tritNum, trit);
			break;
		}
		tritNum++;
	}
	programMemory[ram[PC_REG]] = makeInstruction(ram[1], ram[2], ram[3], ram[4], ram[5]);
	ram[0] = 0;
	ram[1] = 0;
	ram[2] = 0;
	ram[3] = 0;
	ram[4] = 0;
	ram[5] = 0;
	puts("finished loading program from file");

    //ram[TRX_REG] = setTRIT(ram[TRX_REG], RX_BIT, 2);

	puts("running program");
	ram[PC_REG] = 0;
#ifndef _WIN32
  setImmediate(1);
#endif

 // int inputStart = 0;
 // int inputCount = 0;
	while (running) {
	//	while (_kbhit()) {
	//		int c = _getch();
	//		if (isspace(c)) c = ' ';
	//		if (iscntrl(c)) continue;
 //     if (inputStart == 1) {
 //       switch (c)
 //       {
 //       case 'a':
 //         inputTryte += shiftL(0, inputCount);
 //         inputDisplayStringStart[1] = c;
 //         printf("\r%s", inputDisplayStringStart);
 //         inputDisplayStringStart -= 1;
 //         inputStart = 3;
 //         break;
 //       case 'i':
 //         inputTryte += shiftL(1, inputCount);
 //         inputDisplayStringStart[1] = c;
 //         printf("\r%s", inputDisplayStringStart);
 //         inputDisplayStringStart -= 1;
 //         inputStart = 3;
 //         break;
 //       case 'u':
 //         inputTryte += shiftL(2, inputCount);
 //         inputDisplayStringStart[1] = c;
 //         printf("\r%s", inputDisplayStringStart);
 //         inputDisplayStringStart -= 1;
 //         inputStart = 3;
 //         break;
 //       default:
 //         puts("invalid trit");
 //       }
 //     }
 //     else {
 //       if (isdigit(c)) {
 //         inputStart = 2;
 //         inputTryte *= 10;
 //         inputTryte += c - '0';
 //         _putch(c);
 //       }
 //       else if (inputStart == 0 && c == 'l') {
 //         inputStart = 1;
 //         inputDisplayStringStart[0] = c;
 //         inputDisplayStringStart[1] = ' ';
 //         printf("\r%s", inputDisplayStringStart);
 //       }
 //       else if (c == ' ') {
 //         if (inputStart == 2) {
 //           inputCount += 6;
 //           inputStart = 0;
 //         }
 //         else if (inputStart == 3) {
 //           inputCount++;
 //           *inputDisplayStringStart = c;
 //           printf("\r%s", inputDisplayStringStart);
 //           inputDisplayStringStart -= 2;
 //           inputStart = 0;
 //         }
 //         else {
 //           puts("not ready to end input");
 //         }
 //       }
 //       else {
 //         puts("invalid input");
 //       }
 //     }
	//}
 //   if (inputCount && getTrit(memory[TRX_REG], RX_BIT) == 0) {
 //     inputCount--;
 //     memory[RX_REG] = memory[RX_REG] / 3 + inputTryte % 3 * 243;
 //     inputTryte /= 3;
 //     TRYTE receiveCount = shiftR(memory[TRX_REG], RX_COUNT_L_TRIT) % 9 + 1;
 //     if (receiveCount == 6) {
 //       memory[TRX_REG] = setTRIT(memory[TRX_REG], RX_BIT, 2);
 //       receiveCount = 0;
 //       inputDisplayStringStart = inputDisplayString + 11;
 //       printf("\n");
 //     }
 //     memory[TRX_REG] = setTRIT(memory[TRX_REG], RX_COUNT_H_TRIT, receiveCount / 3);
 //     memory[TRX_REG] = setTRIT(memory[TRX_REG], RX_COUNT_L_TRIT, receiveCount % 3);
 //   }
	//	if (getTrit(memory[TRX_REG], TX_BIT) == 2) {
	//		outputTryte = outputTryte / 3 + getTrit(memory[TX_REG], 0) * 243;
	//		memory[TX_REG] /= 3;
	//		TRYTE transmitCount = shiftR(memory[TRX_REG], TX_COUNT_L_TRIT) % 9 + 1;
	//		if (transmitCount == 6) {
	//			memory[TRX_REG] = setTRIT(memory[TRX_REG], TX_BIT, 0);
	//			transmitCount = 0;
	//			printf("%d\n", outputTryte);
	//		}
	//		memory[TRX_REG] = setTRIT(memory[TRX_REG], TX_COUNT_H_TRIT, transmitCount / 3);
	//		memory[TRX_REG] = setTRIT(memory[TRX_REG], TX_COUNT_L_TRIT, transmitCount % 3);
	//	}
	//	if (getTrit(memory[TIMER_OPT_REG], TIMER_ENABLE_BIT) == 2) {
	//		TRYTE timerSpeedSetting = memory[TIMER_OPT_REG] % 9;
	//		timerSpeed += shiftL(1, 6 - timerSpeedSetting);
	//		timerSpeed %= 729;// TODO: make operations mod by 729 automatically
	//		if (timerSpeed == 0) {
	//			memory[TIMER_L_REG]++;
	//			if (memory[TIMER_L_REG] == 729) {
	//				memory[TIMER_L_REG] = 0;
	//				memory[TIMER_H_REG]++;
	//				if (memory[TIMER_H_REG] == 729) {
	//					memory[TIMER_H_REG] = 0;
	//				}
	//			}
	//		}
	//	}
		processInstruction();
		ram[PC_REG]++;
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
	TRYTE instruction[5];
	TRYTE * args = &(instruction[1]);
	decomposeInstruction(programMemory[ram[PC_REG]], instruction);
	__mul_t mul_res;
	__div_t div_res;
	switch (instruction[0])
	{
	case 0:
		running = 0;
		break;
	case 1:
		__ram_set(args[0], 0);
		//memory[args[0]] = 0;
		break;
	case 2:
		__ram_set(args[0], args[1]);
		//memory[args[0]] = args[1];
		break;
	case 3:
		__ram_set(args[1], __ram_get(args[0]));
		//memory[args[1]] = memory[args[0]];
		break;
	case 4:
		__ram_set(args[0], rand() % 729);
		//memory[args[0]] = rand();
		break;
	case 9:
		__ram_set(args[2], __add(__ram_get(args[0]), __ram_get(args[1])));
		//memory[args[2]] = memory[args[0]] + memory[args[1]];
		break;
	case 10:
		__ram_set(args[2], __sub(__ram_get(args[0]), __ram_get(args[1])));
		//memory[args[2]] = memory[args[0]] - memory[args[1]];
		break;
	case 11:
		mul_res = __mul(__ram_get(args[0]), __ram_get(args[1]));
		__ram_set(args[2], mul_res.l);
		__ram_set(args[3], mul_res.h);
		//memory[args[2]] = memory[args[0]] * memory[args[1]];
		break;
	case 12:
		div_res = __div(__ram_get(args[0]), __ram_get(args[1]));
		__ram_set(args[2], div_res.qot);
		__ram_set(args[3], div_res.rem);
		//memory[args[2]] = memory[args[0]] / memory[args[1]];
		//if (args[3]) memory[args[3]] = memory[args[0]] - memory[args[2]] * memory[args[1]];
		break;
	case 18:
		__ram_set(args[0], __sub(__ram_get(args[0]), 1));
		//memory[args[0]]--;
		break;
	case 19:
		__ram_set(args[0], __sub(__ram_get(args[0]), 1));
		if (__ram_get_trit(STATUS_REG, ZERO_BIT) == 2) ram[PC_REG]++;
		//memory[args[0]]--;
		//if (memory[args[0]] == 0) programCounter++;
		break;
	case 20:
		__ram_set(args[0], __add(__ram_get(args[0]), 1));
		//memory[args[0]]++;
		//memory[args[0]] %= 729;
		break;
	case 21:
		__ram_set(args[0], __add(__ram_get(args[0]), 1));
		if (__ram_get_trit(STATUS_REG, ZERO_BIT) == 2) ram[PC_REG]++;
		//memory[args[0]]++;
		//memory[args[0]] %= 729;
		//if (memory[args[0]] == 0) programCounter++;
		break;
	case 27:
		if (__ram_get(args[0]) == 0) ram[PC_REG]++;
		//if (memory[args[0]] == 0) programCounter++;
		break;
	case 28:
		if (__ram_get(args[0]) != 0) ram[PC_REG]++;
		//if (memory[args[0]] != 0) programCounter++;
		break;
	case 36:
		__ram_set(args[2], __and(__ram_get(args[0]), __ram_get(args[1])));
		//memory[args[2]] = and (memory[args[0]], memory[args[1]]);
		break;
	case 37:
		__ram_set(args[2], __or(__ram_get(args[0]), __ram_get(args[1])));
		//memory[args[2]] = or (memory[args[0]], memory[args[1]]);
		break;
	case 38:
		__ram_set(args[2], __xor(__ram_get(args[0]), __ram_get(args[1])));
		//memory[args[2]] = xor (memory[args[0]], memory[args[1]]);
		break;
	case 39:
		__ram_set(args[1], __cmp(__ram_get(args[0])));
		//memory[args[1]] = cmp(memory[args[0]]);
		break;
	case 40:
		__ram_set(args[2], __rot_l(__ram_get(args[0]), __ram_get(args[1])));
		//memory[args[2]] = rotateL(memory[args[0]], memory[args[1]]);
		break;
	case 41:
		__ram_set(args[2], __rot_l_c(__ram_get(args[0]), __ram_get(args[1])));
		//memory[args[2]] = rotateLC(memory[args[0]], memory[args[1]]);
		break;
	case 42:
		__ram_set(args[2], __rot_r(__ram_get(args[0]), __ram_get(args[1])));
		//memory[args[2]] = rotateR(memory[args[0]], memory[args[1]]);
		break;
	case 43:
		__ram_set(args[2], __rot_r_c(__ram_get(args[0]), __ram_get(args[1])));
		//memory[args[2]] = rotateRC(memory[args[0]], memory[args[1]]);
		break;
	case 45:
		__ram_set_trit(args[0], args[1], 0);
		//memory[args[0]] = and (memory[args[0]], cmp(shiftL(2, args[1])));
		break;
	case 46:
		__ram_set_trit(args[0], args[1], args[2]);
		//memory[args[0]] = and (memory[args[0]], cmp(shiftL(2, args[1]))) + shiftL(args[2], args[1]);
		break;
	case 47:
		if (__ram_get_trit(args[0], args[1]) == 0) ram[PC_REG]++;
		//if (and (shiftR(memory[args[0]], args[1]), 2) == 0) programCounter++;
		break;
	case 48:
		if (__ram_get_trit(args[0], args[1]) != 0) ram[PC_REG]++;
		//if (and (shiftR(memory[args[0]], args[1]), 2) != 0) programCounter++;
		break;
	case 49:
		if (__ram_get_trit(args[0], args[1]) == 1) ram[PC_REG]++;
		//if (and (shiftR(memory[args[0]], args[1]), 2) == 1) programCounter++;
		break;
	case 50:
		if (__ram_get_trit(args[0], args[1]) != 1) ram[PC_REG]++;
		//if (and (shiftR(memory[args[0]], args[1]), 2) != 1) programCounter++;
		break;
	case 51:
		if (__ram_get_trit(args[0], args[1]) == 2) ram[PC_REG]++;
		//if (and (shiftR(memory[args[0]], args[1]), 2) == 2) programCounter++;
		break;
	case 52:
		if (__ram_get_trit(args[0], args[1]) != 2) ram[PC_REG]++;
		//if (and (shiftR(memory[args[0]], args[1]), 2) != 2) programCounter++;
		break;
	case 54:
		__stack_push();
		ram[PC_REG] = args[0] - 1;
		//stack[stackPointer] = programCounter;
		//stackPointer = (stackPointer + 1) % 27;
		//programCounter = args[0] - 1;
		break;
	case 55:
		__stack_pop();
		//stackPointer--;
		//programCounter = stack[stackPointer];
		//if (stackPointer < 0) stackPointer += 27;
		break;
	case 56:
		ram[PC_REG] = args[0] - 1;
		//programCounter = args[0] - 1;
		break;
	case 57:
		ram[PC_REG] += (args[0] == 0 ? args[1] : -args[1]) - 1;
		//programCounter += (args[0] == 0 ? args[1] : -args[1]) - 1;
	case 58:
		break;
	default:
		break;
	}
}

//void setMemory(TRYTE address, TRYTE value) {
//	if (address > 27 || address == TRX_REG || address == TX_REG) memory[address] = value;
//}
//TRYTE getMemory(TRYTE address) {
//	return memory[address];
//}
//TRIT getTrit(TRYTE byte, TRYTE trit) {
//	return shiftR(byte, trit) % 3;
//}
//TRYTE setTRIT(TRYTE byte, TRYTE trit, TRIT value) {
//	return and (byte, cmp(shiftL(2, trit))) + shiftL(value, trit);
//}
//TWORD and(TWORD a, TWORD b) {
//	short c = 0;
//	int i = 1;
//	for (; a > 0 && b > 0; a /= 3, b /= 3) {
//		c += min(a % 3, b % 3) * i;
//		i *= 3;
//	}
//	return c;
//}
//TWORD or(TWORD a, TWORD b) {
//	short c = 0;
//	int i = 1;
//	for (; a > 0 && b > 0; a /= 3, b /= 3) {
//		c += max(a % 3, b % 3) * i;
//		i *= 3;
//	}
//	return c;
//}
//TWORD xor(TWORD a, TWORD b) {
//	short c = 0;
//	int i = 1;
//	for (; a > 0 && b > 0; a /= 3, b /= 3) {
//		c += min(max(a % 3, b % 3), 2 - min(a % 3, b % 3)) * i;
//		i *= 3;
//	}
//	return c;
//}
//TWORD cmp(TWORD a) {
//	short c = 0;
//	int i = 1;
//	for (; i < 729; a /= 3) {
//		c += (2 - a % 3) * i;
//		i *= 3;
//	}
//	return c;
//}
//TWORD shiftL(TWORD val, TRYTE trits) {
//	while (trits) {
//		val *= 3;
//		trits--;
//	}
//	return val;
//}
//TWORD shiftR(TWORD val, TRYTE trits) {
//	while (trits) {
//		val /= 3;
//		trits--;
//	}
//	return val;
//}
//TRYTE rotateL(TRYTE a, TRYTE b) {
//	while (b) {
//		short c = and (a, 486) / 243;
//		a *= 3;
//		a += c;
//		b--;
//	}
//	return a;
//}
//TRYTE rotateR(TRYTE a, TRYTE b) {
//	while (b) {
//		short c = and (a, 2) * 243;
//		a /= 3;
//		a += c;
//		b--;
//	}
//	return a;
//}
//TRYTE rotateLC(TRYTE a, TRYTE b) {
//	while (b) {
//		short c = and (a, 486) / 243;
//		a *= 3;
//		a += c;
//		b--;
//	}
//	return a;
//}
//TRYTE rotateRC(TRYTE a, TRYTE b) {
//	while (b) {
//		short c = and (a, 2) * 243;
//		a /= 3;
//		a += c;
//		b--;
//	}
//	return a;
//}