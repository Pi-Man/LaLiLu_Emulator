#include <stdio.h>
#include <ctype.h>

#ifdef _WIN32
#include <Windows.h>
#include <conio.h>
HANDLE srlout;
#else

#endif

int sendNumber(int n);

enum {
	WAITING,
	NUMBER,
	TRIT,
};

int status = WAITING;
short num = 0;

int main(void) {
	srlout = CreateFile("\\\\.\\pipe\\srlin", GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	printf("%p: \n", srlout);
	while (1) {
		char c = _getch();
		if (isspace(c)) c = ' ';
		if (iscntrl(c)) continue;
		switch (status) {
		case WAITING:
			if (isdigit(c)) {
				num = c - '0';
				status = NUMBER;
				_putch(c);
			}
			else if (c == 'l') {
				status = TRIT;
				_putch(c);
			}
			break;
		case NUMBER:
			if (isdigit(c)) {
				num *= 10;
				num += c - '0';
				status = NUMBER;
				_putch(c);
			}
			else if (c == ' ') {
				sendNumber(num);
				num = 0;
				status = WAITING;
				_putch('\n');
			}
			break;
		case TRIT:
			switch (c) {
			case 'a':
				WriteFile(srlout, "la", 2, NULL, NULL);
				_putch(c);
				_putch('\n');
				status = WAITING;
				break;
			case 'i':
				WriteFile(srlout, "li", 2, NULL, NULL);
				_putch(c);
				_putch('\n');
				status = WAITING;
				break;
			case 'u':
				WriteFile(srlout, "lu", 2, NULL, NULL);
				_putch(c);
				_putch('\n');
				status = WAITING;
				break;
			}
		}
	}
}

int sendNumber(int n) {
	int i = 6;
	while (i--) {
		char trit = n % 3;
		n = n / 3;
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