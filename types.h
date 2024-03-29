#ifndef TYPES_H
#define TYPES_H

#include <inttypes.h>

#define TRYTE_TRITS 6
#define TRYTE_SIZE 1
#define TWORD_SIZE 3
#define TDWORD_SIZE 9
#define TQWORD_SIZE 27

#define TRYTE_MAX 728

typedef unsigned char trit_t;
typedef struct { trit_t trits[TRYTE_TRITS]; } tryte_t;
typedef struct { tryte_t trytes[TWORD_SIZE]; } tword_t;
typedef struct { tryte_t trytes[TDWORD_SIZE]; } tdword_t;
typedef struct { tryte_t trytes[TQWORD_SIZE]; } tqword_t;

typedef uint8_t TRIT;
typedef uint16_t TRYTE;
typedef uint32_t HTWORD;
typedef struct __mul_t_ { TRYTE l, h; } __mul_t;
typedef struct __div_t_ { TRYTE qot, rem; } __div_t;
typedef uint64_t TWORD;
typedef uint64_t INST;

typedef struct ___mul_t_ { tryte_t l, h; } ___mul_t;
typedef struct ___div_t_ { tryte_t qot, rem; } ___div_t;

tryte_t makeTryte(unsigned short value);
unsigned short fromTryte(tryte_t tryte);

#endif