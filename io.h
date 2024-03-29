#ifndef IO_H
#define IO_H

#include "types.h"

#ifdef _WIN32
#include <Windows.h>
extern HANDLE srlio[27 * 2];
#else

#endif

void start_USRT(void);

tryte_t __io_read(void);
void __io_write(tryte_t);
trit_t __io_has_input(void);
trit_t __io_has_output(void);

#endif