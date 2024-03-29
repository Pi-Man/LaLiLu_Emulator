#include "io.h"

#include <stdio.h>

#ifdef _WIN32
#include <process.h>
#include <Windows.h>

typedef uintptr_t Thread;
HANDLE srlio[27 * 2];

Thread createThread(_beginthread_proc_type routine, unsigned int stackSize, void * arguments) {
	return _beginthread(routine, stackSize, arguments);
}

inline int read(char * out) {
	for (int i = 0; i < 27; i++) {
		DWORD readCount;
		if (ReadFile(srlio[i], out, 2, &readCount, NULL) && readCount == 2) return 1;
	}
	return 0;
}

inline int write(char * out) {
	int res = 1;
	for (int i = 27; i < 27 * 2; i++) {
		res &= !!WriteFile(srlio[i], out, 2, NULL, NULL);
	}
	return res;
}

#else
#include <pthread.h>

typedef pthread_t * Thread;

Thread createThread(void * (*routine)(void*), unsigned int stackSize, void * arguments) {
	Thread thread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	void * stackAddr;
	size_t _;
	pthread_attr_getstack(&attr, &stackAddr, &_);
	pthread_attr_setstack(&attr, stackAddr, stackSize);
	pthread_create(&thread, &attr, routine, arguments);
	return thread;
}
#endif

#define INPUT_HEAD inputBuffer[inputBufferHeadPointer]
#define INPUT_TAIL inputBuffer[inputBufferTailPointer]

#define OUTPUT_HEAD outputBuffer[outputBufferHeadPointer]
#define OUTPUT_TAIL outputBuffer[outputBufferTailPointer]

tryte_t inputBuffer[27];
tryte_t outputBuffer[27];
int inputBufferHeadPointer = 0;
int inputBufferTailPointer = 0;
int outputBufferHeadPointer = 0;
int outputBufferTailPointer = 0;

unsigned char inputCount = 0;
unsigned char outputCount = 0;

static int io_running = 0;
static Thread thread = 0;

inline trit_t shiftLeft(tryte_t *tryte, trit_t trit) {
	trit_t ret = tryte->trits[0];
	memmove(tryte, sizeof(trit_t) + (char*)tryte, TRYTE_TRITS - 1);
	tryte->trits[TRYTE_TRITS - 1] = trit;
	return ret;
}

inline trit_t shiftRight(tryte_t *tryte, trit_t trit) {
	trit_t ret = tryte->trits[TRYTE_TRITS - 1];
	memmove(sizeof(trit_t) + (char*)tryte, tryte, TRYTE_TRITS - 1);
	tryte->trits[0] = trit;
	return ret;
}

inline void pushInput() {
	inputCount = 0;
}

inline int getTrit(trit_t * out) {

	char trit[2];

	if (!read(trit)) return 0;

	if (trit[0] == 'l') {
		if (trit[1] == 'a') {
			*out = 0;
			return 1;
		}
		else if (trit[1] == 'i') {
			*out = 1;
			return 1;
		}
		else if (trit[1] == 'u') {
			*out = 2;
			return 1;
		}
	}
	else if (trit[0] == 't') {
		if (trit[1] == 'a' || trit[1] == 'u') {
			pushInput();
		}
	}

	return 0;
}

inline void _readTrit(void) {

	trit_t trit;
	if (!getTrit(&trit)) return;

	shiftRight(&INPUT_TAIL, trit);

	if (inputCount == 5) {
		inputCount = 0;
		//puts("tryte read");
		inputBufferTailPointer++;
		if (inputBufferTailPointer == 27) inputBufferTailPointer = 0;
	}
	else {
		inputCount++;
	}
}

inline void _writeTrit(void) {

	trit_t trit = shiftRight(&OUTPUT_HEAD, 0);

	switch (trit) {
	case 0:
		write("la");
		//puts("la");
		break;
	case 1:
		write("li");
		//puts("li");
		break;
	case 2:
		write("lu");
		//puts("lu");
		break;
	}

	outputCount++;
	if (outputCount == 6) {
		//puts("tryte written");
		outputCount = 0;
		outputBufferHeadPointer++;
		if (outputBufferHeadPointer == 27) outputBufferHeadPointer = 0;
	}

}

inline void main_loop(void*_) {
	while (io_running) {
		_readTrit();
		if (outputBufferHeadPointer != outputBufferTailPointer) {
			_writeTrit();
		}
	}
}

void start_USRT(void) {
	memset(inputBuffer, 0, sizeof(inputBuffer));
	memset(outputBuffer, 0, sizeof(outputBuffer));
	io_running = 1;
	thread = createThread(main_loop, 256, NULL);
}

tryte_t __io_read(void) {
	tryte_t tryte = INPUT_HEAD;
	//printf("reading %d\n", fromTryte(tryte));
	inputBufferHeadPointer++;
	if (inputBufferHeadPointer == 27) inputBufferHeadPointer = 0;
	return tryte;
}

void __io_write(tryte_t tryte) {
	//printf("writing %d\n", fromTryte(tryte));
	OUTPUT_TAIL = tryte;
	outputBufferTailPointer++;
	if (outputBufferTailPointer == 27) outputBufferTailPointer = 0;
}

trit_t __io_has_input(void) {
	return inputBufferHeadPointer == inputBufferTailPointer || inputCount != 0 ? 0 : 2;
}

trit_t __io_has_output(void) {
	return outputBufferHeadPointer == outputBufferTailPointer ? 0 : 2;
}