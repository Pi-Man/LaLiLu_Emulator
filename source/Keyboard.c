#include <stdio.h>
#include <stdlib.h>

#include <io.h>

#ifdef _WIN32
#include <Windows.h>
#include <conio.h>
HANDLE srlout;
#else

#endif

int sendKey(char c);

int main(int argc, char ** argv) {
	srlout = CreateFile("\\\\.\\pipe\\srlin", GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	printf("%p: \n", srlout);
	if (srlout == INVALID_HANDLE_VALUE) {
		if (GetLastError() == ERROR_PIPE_BUSY) {
			puts("pipe is busy");
		}
		else {
			puts("pipe error");
		}
	}
	while (1) {
		if (_kbhit()) {
			char c = _getch();
			sendKey(c);
			_putch(c);
			_putch('\n');
		}
	}
	return 0;
}

int sendKey(char c) {
	int i = 6;
	while (i--) {
		char trit = c % 3;
		c = c / 3;
		switch (trit) {
		case 0:
			WriteFile(srlout, "la", 2, NULL, NULL);
			break;
		case 1:
			WriteFile(srlout, "li", 2, NULL, NULL);
			break;
		case 2:
			WriteFile(srlout, "lu", 2, NULL, NULL);
			break;
		}
	}
	return 0;
}