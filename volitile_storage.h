#ifndef RAM_H
#define RAM_H

#include "types.h"
#include "SFR.h"

#define RAM_ADDRESS_SIZE_TRITS 6

#define RAM_SIZE 729 // 6 trit addressing
#define STACK_SIZE 27
#define PROGRAM_SIZE 729

#define ADDRESS TRYTE

extern TRYTE ram[RAM_SIZE];
extern TRYTE stack[STACK_SIZE];
extern INST programMemory[PROGRAM_SIZE];

void init_ram();

TRYTE __ram_get(ADDRESS address);
void __ram_set(ADDRESS address, TRYTE value);

TRIT __ram_get_trit(ADDRESS address, TRYTE trit);
void __ram_set_trit(ADDRESS address, TRYTE trit, TRIT value);

void __stack_push(void);
void __stack_pop(void);


#endif