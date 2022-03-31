#ifndef IO_H
#define IO_H

#include "types.h"

#ifdef _WIN32
#include <Windows.h>
extern HANDLE srlio[27 * 2];
#else

#endif

void start_USRT(void);

TRYTE __io_read(void);
void __io_write(TRYTE);
TRIT __io_has_input(void);
TRIT __io_has_output(void);

#endif