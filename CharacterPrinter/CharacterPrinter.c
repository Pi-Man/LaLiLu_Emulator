#include <stdio.h>

#include <Windows.h>

HANDLE srlin;

char getTrit(char* trit);

int main(void) {

	srlin = CreateFile("\\\\.\\pipe\\srlout", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	char trit[3];
	trit[2] = 0;

	int inputCount = 0;
	int printVal = 0;

	while (1) {
		if (ReadFile(srlin, trit, 2, NULL, NULL)) {
			char tritVal = getTrit(trit);
			printVal /= 3;
			printVal += tritVal * 243;
			inputCount++;
			if (inputCount == 6) {
				inputCount = 0;
				printf("%c", printVal);
				printVal = 0;
			}
		}
	}

}

char getTrit(char* trit) {

	if (trit[0] == 'l') {
		if (trit[1] == 'a') {
			return 0;
		}
		else if (trit[1] == 'i') {
			return 1;
		}
		else if (trit[1] == 'u') {
			return 2;
		}
	}

	return -1;
}