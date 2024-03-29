#ifndef RAM_H
#define RAM_H

#include "types.h"
#include "SFR.h"

#define RAM_ADDRESS_SIZE_TRITS 6

#define RAM_SIZE 729 // 6 trit addressing
#define STACK_SIZE 27
#define PROGRAM_SIZE 729

#define ADDRESS tryte_t

extern tryte_t ram[RAM_SIZE];
extern tryte_t stack[STACK_SIZE];
extern tryte_t programMemory[PROGRAM_SIZE][5];

int init_volitile_storage();

tryte_t __ram_get(ADDRESS address);
tryte_t __ram_get_(ADDRESS address);
void __ram_set(ADDRESS address, tryte_t value);

trit_t __ram_get_trit(ADDRESS address, tryte_t trit);
void __ram_set_trit(ADDRESS address, tryte_t trit, trit_t value);
void __ram_set_trit_(ADDRESS address, tryte_t trit, trit_t value);

void __stack_push(void);
void __stack_pop(void);


#endif