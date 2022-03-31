#ifndef ALU_H
#define ALU_H

#include "types.h"

TRYTE __add(TRYTE a, TRYTE b);

TRYTE __sub(TRYTE a, TRYTE b);

__mul_t __mul(TRYTE a, TRYTE b);

__div_t __div(TRYTE a, TRYTE b);


TRYTE __rot_l(TRYTE value, TRYTE amount);

TRYTE __rot_r(TRYTE value, TRYTE amount);

TRYTE __rot_l_c(TRYTE value, TRYTE amount);

TRYTE __rot_r_c(TRYTE value, TRYTE amount);


TRYTE __and(TRYTE a, TRYTE b);

TRYTE __or(TRYTE a, TRYTE b);

TRYTE __xor(TRYTE a, TRYTE b);

TRYTE __cmp(TRYTE a);

#endif