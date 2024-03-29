#include "ALU.h"

#include <string.h>

#include "volitile_storage.h"

//inline tryte_t min(tryte_t a, tryte_t b) {
//	return a > b ? b : a;
//}
//
//inline tryte_t max(tryte_t a, tryte_t b) {
//	return a < b ? b : a;
//}

tryte_t __add(tryte_t a, tryte_t b) {
	tryte_t res;
	unsigned char carry = 0;
	char zero = 1;
	for (int i = TRYTE_TRITS - 1; i >= 0; i--) {
		unsigned char c = a.trits[i] + b.trits[i] + carry;
		zero &= c == 0;
		res.trits[i] = c < 3 ? c : c - 3;
		carry = c >= 3;
	}
	__ram_set_trit_(STATUS_REG_V, ZERO_BIT_V, zero ? 2 : 0);
	__ram_set_trit_(STATUS_REG_V, CARRY_TRIT_V, carry);
	return res;
}

tryte_t __sub(tryte_t a, tryte_t b) {
	tryte_t res;
	signed char carry = 0;
	char zero = 1;
	for (int i = TRYTE_TRITS - 1; i >= 0; i--) {
		signed char c = a.trits[i] - b.trits[i] - carry;
		zero &= c == 0;
		carry = c < 0;
		res.trits[i] = carry ? c + 3 : c;
	}
	__ram_set_trit_(STATUS_REG_V, ZERO_BIT_V, zero ? 2 : 0);
	__ram_set_trit_(STATUS_REG_V, CARRY_TRIT_V, 2 - carry);
	return res;
}

___mul_t __mul(tryte_t a, tryte_t b) {
	tword_t res = { { {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0} } };
	unsigned char carry = 0;
	unsigned char k = 0;
	unsigned char l = 0;
	unsigned char n = 0;
	char zero = 1;
	for (int i = TRYTE_TRITS - 1; i >= 0; i--) {
		carry = 0;
		for (int j = TRYTE_TRITS - 1; j >= 0; j--) {
			l = TRYTE_TRITS * 2 - 2 - j - i;
			k = l < TRYTE_TRITS ? 0 : 1;
			n = k ? TRYTE_TRITS * 2 - 1 - l : TRYTE_TRITS - 1 - l;
			unsigned char c = a.trits[i] * b.trits[j] + carry + res.trytes[k].trits[n];
			zero &= c == 0;
			carry = c >= 3;
			res.trytes[k].trits[n] = carry ? c - 3 : c;
		}
		res.trytes[1].trits[i] += carry;
	}
	___mul_t ret;
	ret.l = res.trytes[0];
	ret.h = res.trytes[1];
	__ram_set_trit_(STATUS_REG_V, CARRY_TRIT_V, 0);
	__ram_set_trit_(STATUS_REG_V, ZERO_BIT_V, zero ? 2 : 0);
	return ret;
}

___div_t __div(tryte_t a, tryte_t b) {
	unsigned short qot, rem, a_, b_;
	a_ = fromTryte(a);
	b_ = fromTryte(b);
	___div_t ret;
	qot = a_ / b_;
	rem = a_ - qot * b_;
	ret.qot = makeTryte(qot);
	ret.rem = makeTryte(rem);
	__ram_set_trit_(STATUS_REG_V, CARRY_TRIT_V, 0);
	__ram_set_trit_(STATUS_REG_V, ZERO_BIT_V, a_ ? 0 : 2);
	return ret;
}


tryte_t __rot_l(tryte_t value, tryte_t amount) {
	tryte_t ret;
	unsigned short amount_ = fromTryte(amount) % TRYTE_TRITS;
	memmove(&ret, (trit_t*)&value + amount_, TRYTE_TRITS - amount_);
	memmove((trit_t*)&ret + TRYTE_TRITS - amount_, &value, amount_);
	__ram_set_trit_(STATUS_REG_V, ZERO_BIT_V, fromTryte(ret) ? 0 : 2);
	return ret;
}

tryte_t __rot_r(tryte_t value, tryte_t amount) {
	tryte_t ret;
	unsigned short amount_ = fromTryte(amount) % TRYTE_TRITS;
	memmove(&ret, (trit_t*)&value + TRYTE_TRITS - amount_, amount_);
	memmove((trit_t*)&ret + amount_, &value, TRYTE_TRITS - amount_);
	__ram_set_trit_(STATUS_REG_V, ZERO_BIT_V, fromTryte(ret) ? 0 : 2);
	return ret;
}

tryte_t __rot_l_c(tryte_t value, tryte_t amount) {
	tryte_t ret;
	unsigned short amount_ = fromTryte(amount) % (TRYTE_TRITS + 1);
	if (amount_) {
		memmove(&ret, (trit_t*)&value + amount_, TRYTE_TRITS - amount_);
		memmove((trit_t*)&ret + TRYTE_TRITS - amount_ + 1, &value, amount_ - 1);
		ret.trits[TRYTE_TRITS - amount_] = __ram_get_trit(STATUS_REG_V, CARRY_TRIT_V);
		__ram_set_trit_(STATUS_REG_V, CARRY_TRIT_V, value.trits[amount_ - 1]);
		__ram_set_trit_(STATUS_REG_V, ZERO_BIT_V, fromTryte(ret) ? 0 : 2);
		return ret;
	}
	else {
		return value;
	}
}

tryte_t __rot_r_c(tryte_t value, tryte_t amount) {
	tryte_t ret;
	unsigned short amount_ = fromTryte(amount) % (TRYTE_TRITS + 1);
	if (amount_) {
		memmove(&ret, (trit_t*)&value + TRYTE_TRITS - amount_ + 1, amount_ - 1);
		memmove((trit_t*)&ret + amount_, &value, TRYTE_TRITS - amount_);
		ret.trits[amount_ - 1] = __ram_get_trit(STATUS_REG_V, CARRY_TRIT_V);
		__ram_set_trit_(STATUS_REG_V, CARRY_TRIT_V, value.trits[TRYTE_TRITS - amount_]);
		__ram_set_trit_(STATUS_REG_V, ZERO_BIT_V, fromTryte(ret) ? 0 : 2);
		return ret;
	}
	else {
		return value;
	}
}


tryte_t __and(tryte_t a, tryte_t b) {
	tryte_t ret;
	char zero = 1;
	for (int i = TRYTE_TRITS - 1; i >= 0; i--) {
		ret.trits[i] = a.trits[i] < b.trits[i] ? a.trits[i] : b.trits[i];
		zero &= ret.trits[i] == 0;
	}
	__ram_set_trit_(STATUS_REG_V, ZERO_BIT_V, zero ? 2 : 0);
	return ret;
}

tryte_t __or(tryte_t a, tryte_t b) {
	tryte_t ret;
	char zero = 1;
	for (int i = TRYTE_TRITS - 1; i >= 0; i--) {
		ret.trits[i] = a.trits[i] > b.trits[i] ? a.trits[i] : b.trits[i];
		zero &= ret.trits[i] == 0;
	}
	__ram_set_trit_(STATUS_REG_V, ZERO_BIT_V, zero ? 2 : 0);
	return ret;
}

tryte_t __xor(tryte_t a, tryte_t b) {
	tryte_t ret;
	char zero = 1;
	for (int i = TRYTE_TRITS - 1; i >= 0; i--) {
		ret.trits[i] = 1 - (a.trits[i] - 1) * (b.trits[i] - 1);
		zero &= ret.trits[i] == 0;
	}
	__ram_set_trit_(STATUS_REG_V, ZERO_BIT_V, zero ? 2 : 0);
	return ret;
}

tryte_t __cmp(tryte_t a) {
	tryte_t ret;
	char zero = 1;
	for (int i = TRYTE_TRITS - 1; i >= 0; i--) {
		ret.trits[i] = 2 - a.trits[i];
		zero &= ret.trits[i] == 0;
	}
	__ram_set_trit_(STATUS_REG_V, ZERO_BIT_V, zero ? 2 : 0);
	return ret;
}