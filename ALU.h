#ifndef ALU_H
#define ALU_H

#include "types.h"

tryte_t __add(tryte_t a, tryte_t b);

tryte_t __sub(tryte_t a, tryte_t b);

___mul_t __mul(tryte_t a, tryte_t b);

___div_t __div(tryte_t a, tryte_t b);


tryte_t __rot_l(tryte_t value, tryte_t amount);

tryte_t __rot_r(tryte_t value, tryte_t amount);

tryte_t __rot_l_c(tryte_t value, tryte_t amount);

tryte_t __rot_r_c(tryte_t value, tryte_t amount);


tryte_t __and(tryte_t a, tryte_t b);

tryte_t __or(tryte_t a, tryte_t b);

tryte_t __xor(tryte_t a, tryte_t b);

tryte_t __cmp(tryte_t a);

#endif