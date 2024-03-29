#include "types.h"

#include <string.h>

tryte_t makeTryte(unsigned short value) {
	tryte_t tryte;
	for (int i = 5; i >= 0; i--, value /= 3) {
		unsigned char trit = value % 3;
		tryte.trits[i] = trit;
	}
	return tryte;
}

unsigned short fromTryte(tryte_t tryte) {
	unsigned short value = 0;
	for (int i = 0; i < 6; i++) {
		value *= 3;
		value += tryte.trits[i];
	}
	return value;
}
