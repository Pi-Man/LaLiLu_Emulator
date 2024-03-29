#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "../SFR.h"

#define nameof(x) #x


struct LabelPair {
	char * name;
	size_t address;
} *labels;
size_t label_size = 0, label_capacity = 0;

struct Argument {
	int value;
	char * text;
} *arguments;
size_t argument_size = 0, argument_capacity = 0;

struct Command {
	int value;
	int arg_count;
	int args[5];
} *commands;
size_t command_size = 0, command_capacity = 0;

void addLabel(char * line, size_t address) {
	if (!label_capacity) {
		labels = malloc(sizeof(struct LabelPair) * 8);
		label_capacity = 8;
	}
	if (label_size >= label_capacity) {
		label_capacity *= 2;
		labels = realloc(labels, sizeof(struct LabelPair) * label_capacity);
	}
	labels[label_size].name = malloc(strlen(line) + 1);
	memcpy(labels[label_size].name, line, strlen(line) + 1);
	labels[label_size].address = address;
	label_size++;
}

void addArgument(char * text, int value) {
	if (!argument_capacity) {
		arguments = malloc(sizeof(struct Argument) * 8);
		argument_capacity = 8;
	}
	if (argument_size >= argument_capacity) {
		argument_capacity *= 2;
		arguments = realloc(arguments, sizeof(struct Argument) * argument_capacity);
	}
	if (text) {
		arguments[argument_size].text = malloc(strlen(text) + 1);
		memcpy(arguments[argument_size].text, text, strlen(text) + 1);
	}
	else {
		arguments[argument_size].text = NULL;
	}
	arguments[argument_size].value = value;
	argument_size++;
}

void addCommand(int value, int arg_count, int args[5]) {
	if (!command_capacity) {
		commands = malloc(sizeof(struct Command) * 8);
		command_capacity = 8;
	}
	if (command_size >= command_capacity) {
		command_capacity *= 2;
		commands = realloc(commands, sizeof(struct Command) * command_capacity);
	}
	commands[command_size].value = value;
	commands[command_size].arg_count = arg_count;
	memcpy(commands[command_size].args, args, sizeof(int) * 5);
	command_size++;
}


char * getLine(FILE *);

int parseInteger(char **);

int isndigit(char, int);

int writeInteger(int, FILE *);

int writeCommand(int, FILE *);

int writeArgument(int, FILE *);

int parseCommand(const char *, int, int, char *, FILE *);

int main(int argc, char** args) {
	size_t address = 0;
	int macros_size = 22, macros_capacity = 30;
	char *(*macros)[2] = malloc(sizeof(char *[2]) * 30);

#define INSERT_REG(id, a) \
macros[id][0] = #a; \
macros[id][1] = "d"nameof(a)

	INSERT_REG(0, STATUS_REG);
	INSERT_REG(1, CARRY_TRIT);
	INSERT_REG(2, ZERO_BIT);

	INSERT_REG(3, TRX_REG);
	INSERT_REG(4, TX_BIT);
	INSERT_REG(5, RX_BIT);
	INSERT_REG(6, TX_COUNT_L_TRIT);
	INSERT_REG(7, TX_COUNT_H_TRIT);
	INSERT_REG(8, RX_COUNT_L_TRIT);
	INSERT_REG(9, RX_COUNT_H_TRIT);

	INSERT_REG(10, BAUD_RATE_REG);

	INSERT_REG(11, TX_REG);

	INSERT_REG(12, RX_REG);

	INSERT_REG(13, TIMER_OPT_REG);
	INSERT_REG(14, TIMER_SPEED_L_TRIT);
	INSERT_REG(15, TIMER_SPEED_H_TRIT);
	INSERT_REG(16, TIMER_ENABLE_BIT);

	INSERT_REG(17, TIMER_L_REG);

	INSERT_REG(18, TIMER_H_REG);

	INSERT_REG(19, PC_REG);

	INSERT_REG(20, FS_REG);

	INSERT_REG(21, IND_REG);

#undef INSERT_REG

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
		if (*line == 0) {
			free(copy);
			continue;
		}
		
		for (int i = 0; i < macros_size; i++) {
			char * token = strstr(line, macros[i][0]);
			while (token && (token[strlen(macros[i][0])] == ' ' || token[strlen(macros[i][0])] == 0)) {
				if (macros[i][1]) {
					if (strlen(macros[i][1]) > strlen(macros[i][0])) {
						copy = line = realloc(line, strlen(line) + 1 + strlen(macros[i][1]) - strlen(macros[i][0]));
						token = strstr(line, macros[i][0]);
					}
					memmove(token + strlen(macros[i][1]), token + strlen(macros[i][0]), strlen(token) - strlen(macros[i][0]) + 1);
					memcpy(token, macros[i][1], strlen(macros[i][1]));
				}
				else {
					memmove(token, token + strlen(macros[i][0]), strlen(token) - strlen(macros[i][0]) + 1);
				}
				token = strstr(line, macros[i][0]);
			}
		}

		if (strncmp("#define ", line, 8) == 0) {
			if (macros_size == macros_capacity) {
				macros_capacity *= 2;
				macros = realloc(macros, sizeof(char *[2]) * macros_capacity);
			}
			char * key = line + 8;
			if (!*key) {
				puts("no macro key given, skipping line");
				free(copy);
				continue;
			}
			char * value = strchr(key, ' ');
			if (value) {
				value++;
				value[-1] = 0;
				macros[macros_size][1] = malloc(strlen(value) + 1);
				memcpy(macros[macros_size][1], value, strlen(value) + 1);
			}
			else {
				macros[macros_size][1] = NULL;
			}
			macros[macros_size][0] = malloc(strlen(key) + 1);
			memcpy(macros[macros_size][0], key, strlen(key) + 1);
			macros_size++;
			free(copy);
			continue;
		}
		
		char * labelEnd;
		if (!strchr(line, ' ') && (labelEnd = strchr(line, ':')) && labelEnd[1] == 0) {
			*labelEnd = 0;
			addLabel(line, address);
			printf("%s:\n", line);
			free(copy);
			continue;
		}

		puts(line);
		int error;

		if (error = parseCommand("clr", 1, 1, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}
		if (error = parseCommand("set", 2, 2, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}
		if (error = parseCommand("mov", 3, 2, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}
		if (error = parseCommand("rnd", 4, 1, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}

		if (error = parseCommand("add", 9, 3, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}
		if (error = parseCommand("sub", 10, 3, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}
		if (error = parseCommand("mul", 11, 3, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}
		if (error = parseCommand("div", 12, 4, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}

		if (error = parseCommand("dec", 18, 1, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}
		if (error = parseCommand("decsz", 19, 1, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}
		if (error = parseCommand("inc", 20, 1, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}
		if (error = parseCommand("incsz", 21, 1, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}

		if (error = parseCommand("tsz", 27, 1, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}
		if (error = parseCommand("tsnz", 28, 1, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}

		if (error = parseCommand("and", 36, 3, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}
		if (error = parseCommand("or", 37, 3, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}
		if (error = parseCommand("xor", 38, 3, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}
		if (error = parseCommand("cmp", 39, 2, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}
		if (error = parseCommand("rl", 40, 3, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}
		if (error = parseCommand("rlc", 41, 3, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}
		if (error = parseCommand("rr", 42, 3, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}
		if (error = parseCommand("rrc", 43, 3, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}

		if (error = parseCommand("tc", 45, 2, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}
		if (error = parseCommand("ts", 46, 3, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}
		if (error = parseCommand("ttsa", 47, 2, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}
		if (error = parseCommand("ttsna", 48, 2, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}
		if (error = parseCommand("ttsi", 49, 2, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}
		if (error = parseCommand("ttsni", 50, 2, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}
		if (error = parseCommand("ttsu", 51, 2, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}
		if (error = parseCommand("ttsnu", 52, 2, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}

		if (error = parseCommand("call", 54, 1, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}
		if (error = parseCommand("return", 55, 0, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}
		if (error = parseCommand("goto", 56, 1, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}
		if (error = parseCommand("jmp", 57, 2, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}
		if (error = parseCommand("nop", 58, 0, line, oFile)) {
			if (error == -1) {
				return 5;
			}
			address++;
			free(copy);
			continue;
		}

		printf("unknown Command: \"%s\", skipping line\n", line);
		free(copy);
	}

	for (int i = 0; i < command_size; i++) {
		writeCommand(commands[i].value, oFile);
		for (int j = 0; j < commands[i].arg_count; j++) {
			char * text = arguments[commands[i].args[j]].text;
			if (text) {
				int k;
				for (k = 0; k < label_size; k++) {
					if (strncmp(text, labels[k].name, strlen(labels[k].name)) == 0) {
						writeArgument(labels[k].address, oFile);
						break;
					}
				}
				if (k == label_size) {
					printf("unknown label \"%s\"\n", text);
				}
			}
			else {
				writeArgument(arguments[commands[i].args[j]].value, oFile);
			}
		}
		fprintf(oFile, "\n");
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
		if (b == '\'' && (*string)[1] == '\'') {
			char c = **string;
			*string += 2;
			return c;
		}
		//puts("invalid integer literal");
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
		line++;
		int args[5];
		for (int i = 0; i < numArgs; i++) {
			args[i] = argument_size;
			char * arg_start = line;
			int arg = parseInteger(&line);
			if (arg == -1) {
				while (*line != ' ' && *line != 0) line++;
				if (!*line && i != numArgs - 1) return -1;
				if (*line && i == numArgs - 1) return -1;
				*line = 0;
				line++;
				addArgument(arg_start, 0);
			}
			else {
				addArgument(NULL, arg);
			}
		}
		addCommand(code, numArgs, args);
		return 1;
	}
	return 0;
}