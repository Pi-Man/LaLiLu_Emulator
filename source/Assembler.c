#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>

char * getLine(FILE *);

int parseInteger(char **);

int isndigit(char, int);

int writeInteger(int, FILE *);

int writeCommand(int, FILE *);

int writeArgument(int, FILE *);

int parseCommand(const char *, int, int, char *, FILE *);

int main(int argc, char** args) {

	char ** inputFiles = malloc(sizeof(char*));
	size_t inputFilesSize = 0;
	size_t inputFilesCapacity = 1;

	char * outputFile = NULL;

	for (int i = 1; i < argc; i++) {
		if (args[i][0] == '-') {
			if (args[i][1] == 'o') {
				if (outputFile) {
					puts("only one output file can be specified");
					return 4;
				}
				outputFile = args[i + 1];
				i++;
				continue;
			}
		}
		else {
			if (inputFilesSize == inputFilesCapacity) {
				if (!realloc(inputFiles, inputFilesCapacity *= 2)) {
					puts("memory allocation error");
					return 1;
				}
			}
			inputFiles[inputFilesSize] = args[i];
			inputFilesSize++;
		}
	}

	if (inputFilesSize == 0) {
		puts("no input files provided");
		return 2;
	}

	if (!outputFile) {
		outputFile = "out.scwt";
	}

	FILE * iFile = fopen(inputFiles[0], "r");
	FILE * oFile = fopen(outputFile, "w");

	if (!iFile) {
		puts("file could not be opened");
		puts(inputFiles[0]);
		return 3;
	}
	if (!oFile) {
		puts("file could not be opened");
		puts(outputFile);
		return 3;
	}

	while (!feof(iFile)) {
		char * line = getLine(iFile);
		char * copy = line;
		if (!line) {
			free(copy);
			return 6;
		}
		if (*line == 0) continue;
		puts(line);
		int error;

		fputs("\n", oFile);

		if (error = parseCommand("clr", 1, 1, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}
		if (error = parseCommand("set", 2, 2, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}
		if (error = parseCommand("mov", 3, 2, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}
		if (error = parseCommand("rnd", 4, 1, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}

		if (error = parseCommand("add", 9, 3, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}
		if (error = parseCommand("sub", 10, 3, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}
		if (error = parseCommand("mul", 11, 3, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}
		if (error = parseCommand("div", 12, 4, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}

		if (error = parseCommand("dec", 18, 1, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}
		if (error = parseCommand("decsz", 19, 1, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}
		if (error = parseCommand("inc", 20, 1, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}
		if (error = parseCommand("incsz", 21, 1, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}

		if (error = parseCommand("tsz", 27, 1, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}
		if (error = parseCommand("tsnz", 28, 1, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}

		if (error = parseCommand("and", 36, 3, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}
		if (error = parseCommand("or", 37, 3, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}
		if (error = parseCommand("xor", 38, 3, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}
		if (error = parseCommand("cmp", 39, 2, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}
		if (error = parseCommand("rl", 40, 3, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}
		if (error = parseCommand("rlc", 41, 3, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}
		if (error = parseCommand("rr", 42, 3, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}
		if (error = parseCommand("rrc", 43, 3, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}

		if (error = parseCommand("tc", 45, 2, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}
		if (error = parseCommand("ts", 46, 3, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}
		if (error = parseCommand("ttsa", 47, 2, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}
		if (error = parseCommand("ttsna", 48, 2, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}
		if (error = parseCommand("ttsi", 49, 2, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}
		if (error = parseCommand("ttsni", 50, 2, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}
		if (error = parseCommand("ttsu", 51, 2, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}
		if (error = parseCommand("ttsnu", 52, 2, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}

		if (error = parseCommand("call", 54, 1, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}
		if (error = parseCommand("return", 55, 0, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}
		if (error = parseCommand("goto", 56, 1, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}
		if (error = parseCommand("jmp", 57, 2, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}
		if (error = parseCommand("nop", 58, 0, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			free(copy);
			continue;
		}

		printf("unknown Command: %s, skipping line\n", line);
		free(copy);
	}

}

char * getLine(FILE * file) {
	fpos_t pos;
	if (fgetpos(file, &pos)) {
		puts("error getting file pos");
		return NULL;
	}
	int n = 0;
	char c;
	while (c = fgetc(file), c != '\n' && c != EOF) n++;
	char * line = malloc(n + 2);
	if (fsetpos(file, &pos)) {
		puts("error setting file pos");
		return NULL;
	}
	if (!fgets(line, n + 2, file)) {
		if (!feof(file)) {
			puts("error reading file");
		}
		return NULL;
	}
	char * temp;
	if ((temp = strchr(line, ';')) || (temp = strchr(line, '\n'))) {
		*temp = 0;
	}
	return line;
}

int parseInteger(char ** string) {
	while (isspace(**string) && **string != '\n') (*string)++;
	char b = **string;
	(*string)++;
	int base = 0;
	switch (b) {
	case 't':
		base = 3;
		break;
	case 'd':
		base = 10;
		break;
	case 0:
		return 0;
	}

	if (base == 0) {
		puts("invalid integer literal");
		return -1;
	}

	int value = 0;
	for (char c = **string; isndigit(c, base); (*string)++, c = **string) {
		value *= base;
		value += c - '0';
	}
	return value;
}

int isndigit(char c, int base) {
	return c >= '0' && c < ('0' + base);
}

int writeInteger(int value, FILE * file) {
	if (value < 0) {
		puts("can not write negative integers");
		return 1;
	}
	if (value == 0) {
		fprintf(file, "la ");
		return 0;
	}
	int hpt = 1;
	for (int i = value; i > 2; i /= 3) hpt *= 3;
	while (hpt > 0) {
		int q, r;
		q = value / hpt;
		r = value - q * hpt;
		value = r;
		hpt /= 3;
		switch (q)
		{
		case 0:
			fprintf(file, "la ");
			break;
		case 1:
			fprintf(file, "li ");
			break;
		case 2:
			fprintf(file, "lu ");
			break;
		}

	}
	return 0;
}

int writeCommand(int command, FILE * file) {
	fprintf(file, "ta ");
	return writeInteger(command, file);
}

int writeArgument(int argument, FILE * file) {
	fprintf(file, "tu ");
	return writeInteger(argument, file);
}

int parseCommand(const char * name, int code, int numArgs, char * line, FILE * oFile) {
	int n = strlen(name);
	if (strncmp(line, name, n) == 0) {
		line += n;
		if (!(isspace(*line) || (*line == 0 && numArgs == 0))) return 0;
		int * args = malloc(sizeof(int) * numArgs);
		for (int i = 0; i < numArgs; i++) {
			args[i] = parseInteger(&line);
			if (args[i] == -1) {
				return -1;
			}
		}
		writeCommand(code, oFile);
		for (int i = 0; i < numArgs; i++) {
			if (writeArgument(args[i], oFile)) {
				return -1;
			}
		}
		return 1;
	}
	return 0;
}