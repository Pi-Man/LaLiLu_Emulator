#include "ALU.h"

#include "volitile_storage.h"

inline TRYTE min(TRYTE a, TRYTE b) {
	return a > b ? b : a;
}

inline TRYTE max(TRYTE a, TRYTE b) {
	return a < b ? b : a;
}

TRYTE __add(TRYTE a, TRYTE b) {
	TRYTE res = a + b;
	TRIT carry = res / 729;
	__ram_set_trit(STATUS_REG, CARRY_TRIT, carry);
	__ram_set_trit(STATUS_REG, ZERO_BIT, (res - carry * 729) ? 0 : 2);
	return res - carry * 729;
}

TRYTE __sub(TRYTE a, TRYTE b) {
	TRYTE res = (a + 729) - b;
	TRIT carry = res / 729;
	__ram_set_trit(STATUS_REG, CARRY_TRIT, carry);
	__ram_set_trit(STATUS_REG, ZERO_BIT, (res - carry * 729) ? 0 : 2);
	return res - carry * 729;
}

__mul_t __mul(TRYTE a, TRYTE b) {
	HTWORD res = a * b;
	__mul_t ret;
	ret.h = res / 729;
	ret.h = res - ret.h * 729;
	__ram_set_trit(STATUS_REG, CARRY_TRIT, 0);
	__ram_set_trit(STATUS_REG, ZERO_BIT, (a || b) ? 0 : 2);
	return ret;
}

__div_t __div(TRYTE a, TRYTE b) {
	__div_t ret;
	ret.qot = a / b;
	ret.rem = a - ret.qot * b;
	__ram_set_trit(STATUS_REG, CARRY_TRIT, 0);
	__ram_set_trit(STATUS_REG, ZERO_BIT, a ? 0 : 2);
	return ret;
}


TRYTE __rot_l(TRYTE value, TRYTE amount) {
	while (amount) {
		TRIT r = value / 243;
		value *= 3;
		value %= 729;
		value += r;
		amount--;
	}
	__ram_set_trit(STATUS_REG, ZERO_BIT, value ? 0 : 2);
	return value;
}

TRYTE __rot_r(TRYTE value, TRYTE amount) {
	while (amount) {
		TRIT r = value % 3;
		value /= 3;
		value += r * 243;
		amount--;
	}
	__ram_set_trit(STATUS_REG, ZERO_BIT, value ? 0 : 2);
	return value;
}

TRYTE __rot_l_c(TRYTE value, TRYTE amount) {
	while (amount) {
		TRIT r = value / 243;
		value *= 3;
		value %= 729;
		value += __ram_get_trit(STATUS_REG, CARRY_TRIT);
		__ram_set_trit(STATUS_REG, CARRY_TRIT, r);
		amount--;
	}
	__ram_set_trit(STATUS_REG, ZERO_BIT, value ? 0 : 2);
	return value;
}

TRYTE __rot_r_c(TRYTE value, TRYTE amount) {
	while (amount) {
		TRIT r = value % 3;
		value /= 3;
		value += __ram_get_trit(STATUS_REG, CARRY_TRIT) * 243;
		__ram_set_trit(STATUS_REG, CARRY_TRIT, r);
		amount--;
	}
	__ram_set_trit(STATUS_REG, ZERO_BIT, value ? 0 : 2);
	return value;
}


TRYTE __and(TRYTE a, TRYTE b) {
	TRYTE c = 0;
	int i = 1;
	for (; a && b; a /= 3, b /= 3) {
		c += min(a % 3, b % 3) * i;
		i *= 3;
	}
	__ram_set_trit(STATUS_REG, ZERO_BIT, c ? 0 : 2);
	return c;
}

TRYTE __or(TRYTE a, TRYTE b) {
	TRYTE c = 0;
	int i = 1;
	for (; a || b; a /= 3, b /= 3) {
		c += max(a % 3, b % 3) * i;
		i *= 3;
	}
	__ram_set_trit(STATUS_REG, ZERO_BIT, c ? 0 : 2);
	return c;
}

TRYTE __xor(TRYTE a, TRYTE b) {
	TRYTE c = 0;
	int i = 1;
	for (; a || b; a /= 3, b /= 3) {
		c += min(max(a % 3, b % 3), 2 - min(a % 3, b % 3)) * i;
		i *= 3;
	}
	__ram_set_trit(STATUS_REG, ZERO_BIT, c ? 0 : 2);
	return c;
}

TRYTE __cmp(TRYTE a) {
	TRYTE c = 0;
	int i = 1;
	for (; i < 729; a /= 3) {
		c += (2 - a % 3) * i;
		i *= 3;
	}
	__ram_set_trit(STATUS_REG, ZERO_BIT, c ? 0 : 2);
	return c;
}