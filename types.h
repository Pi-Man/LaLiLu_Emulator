#ifndef TYPES_H
#define TYPES_H

#include <inttypes.h>

#define TRYTE_TRITS 6
#define TRYTE_SIZE 1
#define HTWORD_SIZE 2
#define TWORD_SIZE 4

#define TRYTE_MAX 728

typedef uint8_t TRIT;
typedef uint16_t TRYTE;
typedef uint32_t HTWORD;
typedef struct __mul_t_ { TRYTE l, h; } __mul_t;
typedef struct __div_t_ { TRYTE qot, rem; } __div_t;
typedef uint64_t TWORD;
typedef uint64_t INST;

#endif