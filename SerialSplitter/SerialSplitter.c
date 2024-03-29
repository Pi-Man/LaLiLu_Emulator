
#include <process.h>
#include <Windows.h>

// packet format:
//   To Address: does not exist if at destination
//     Indicator Value: 2
//     Length: 3 trits
//   From Address: optional but highly reccommended
//     Indicator Value: 1
//     Length: 3 trits
//   Size: size of the data in trytes after the size field
//     Indicator Value: 0
//     Length: 6 trits (1 tryte)
//   Data: packet data
//   
typedef uintptr_t Thread;

volatile char connected = 1;
volatile HANDLE srlout[27] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
volatile HANDLE srlin[27]  = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

const char * const srlin_name = "\\\\.\\pipe\\srlout";
const char * const srlout_name = "\\\\.\\pipe\\srlin";

const char address[1024] = "";

void io_loop(char * address_name);

int main(int argc, char ** args) {

	if (argc == 2) {
		strncpy(address, args[1], 1024);
	}

	Thread thread = _beginthread(io_loop, 1024, address);

	return 0;
}

enum Destination {
	PORT,
	BUFFER,
	SPLITTER,
};

enum ReadState {
	WAIT,
	ADDR,
	SNDR,
	SIZE,
	DATA,
};

// max 27 address length (*2 for return/to address) plus 728 trit message length
#define MAX_MESSAGE_SIZE (4*27*2+728)

struct PortState {
	unsigned short read_count;
	unsigned short read_size;
	unsigned short buffer_pos;
	unsigned short buffer_size;
	unsigned char
		open       : 1,
		busy       : 1,
		dest       : 2,
		read_state : 3;
	unsigned char addr;
	unsigned char buffer[MAX_MESSAGE_SIZE]
} states[27];

const char command[MAX_MESSAGE_SIZE];

char get_trit(HANDLE srlin);
void send_trit(char trit, HANDLE srlout);
char get_address(HANDLE srlin);
void read_packet_header(HANDLE srlin);

void io_loop(char * address_name) {

	char address = -1;

	size_t n = strlen(address_name) + 18;
	char * s = malloc(n);

	strcat_s(s, n, srlout_name);
	strcat_s(s, n, address_name);
	srlout[0] = CreateNamedPipe(s, PIPE_ACCESS_OUTBOUND, PIPE_TYPE_BYTE | PIPE_NOWAIT, 1, 32, 0, 0, NULL);

	*s = 0;
	strcat_s(s, n, srlin_name);
	strcat_s(s, n, address_name);
	srlin[0] = CreateNamedPipe(s, PIPE_ACCESS_INBOUND, PIPE_TYPE_BYTE | PIPE_NOWAIT, 1, 0, 32, 0, NULL);

	address = get_address(srlin[0]);

	while (connected) {
		int i = 27;
		while (i--) {
			if (states[i].open) {
				switch (states[i].read_state) {
				case WAIT: {
					char trit = get_trit(srlin[i]);
					switch (trit) {
					case 0:
						puts("No From Address");
						states[i].read_state = SIZE;
						states[i].read_count = 0;
						states[i].dest = SPLITTER;
						break;
					case 1:
						puts("Message to Splitter");
						states[i].read_state = SNDR;
						states[i].read_count = 0;
						states[i].dest = SPLITTER;
						break;
					case 2:
						states[i].read_state = ADDR;
						states[i].read_count = 0;
						break;
					default:
						break;
					}
					break;
				}
				case ADDR: {
					char trit = get_trit(srlin[i]);
					switch (trit) {
					case 0: case 1: case 2: {
						states[i].addr *= 3;
						states[i].addr += trit;
						states[i].read_count++;
						if (states[i].read_count >= 3) {
							if (states[states[i].addr].busy) {
								states[i].dest = BUFFER;
								states[i].buffer_pos = 0;
								states[i].buffer_size = 0;
							}
							else {
								states[i].dest = PORT;
								states[states[i].addr].busy = 1;
							}
							states[i].read_state = SNDR;
							states[i].read_count = 0;
						}
						break;
					}
					default:
						break;
					}
					break;
				}
				case SNDR: {
					char trit = get_trit(srlin[i]);
					if (states[i].read_count < 3) {
						switch (trit) {
						case 0: case 1: case 2: {
							switch (states[i].dest) {
							case PORT:
								send_trit(trit, srlout[states[i].addr]);
								break;
							case BUFFER:
								states[i].buffer[states[i].buffer_size] = trit;
								states[i].buffer_size++;
								break;
							case SPLITTER:
								break;
							default:
								break;
							}
							states[i].read_count++;
							break;
						}
						default:
							break;
						}
					}
					else {
						switch (trit) {
						case 0:
							states[i].read_state = SIZE;
							states[i].read_size = 0;
							states[i].read_count = 0;
							break;
						case 1:
							//states[i].read_state = SNDR;
							states[i].read_count = 0;
							break;
						case 2:
							puts("malformed packet");
							//states[i].read_state = ADDR;
							//states[i].read_count = 0;
							break;
						default:
							break;
						}
					}
					break;
				}
				case SIZE: {
					char trit = get_trit(srlin[i]);
					switch (trit) {
					case 0: case 1: case 2:
						switch (states[i].dest) {
						case PORT:
							send_trit(trit, srlout[states[i].addr]);
							break;
						case BUFFER:
							states[i].buffer[states[i].buffer_size] = trit;
							states[i].buffer_size++;
							break;
						case SPLITTER:
							break;
						default:
							break;
						}
						states[i].read_size *= 3;
						states[i].read_size += trit;
						states[i].read_count++;
						if (states[i].read_count >= 6) {
							states[i].read_state = DATA;
							states[i].read_count = 0;
						}
						break;
					default:
						break;
					}
				}
				case DATA: {
					char trit = get_trit(srlin[i]);
					switch (trit) {
					case 0: case 1: case 2:
						switch (states[i].dest) {
						case PORT:
							send_trit(trit, srlout[states[i].addr]);
							break;
						case BUFFER:
							states[i].buffer[states[i].buffer_size] = trit;
							states[i].buffer_size++;
							break;
						case SPLITTER:
							break;
						default:
							break;
						}
						if (states[i].read_count >= states[i].read_size) {
							states[i].read_state = WAIT;
							states[i].read_count = 0;
							if (states[i].dest == PORT) {
								states[states[i].addr].busy = 0;
							}
						}
						break;
					default:
						break;
					}
				}
				}
			}
		}
	}

}

char get_trit(HANDLE srlin) {
	char in[2];
	DWORD bytes_read;
	BOOL data_read = ReadFile(srlin, in, 2, &bytes_read, NULL);
	if (data_read && bytes_read == 2) {
		if (in[0] == 'l') {
			if (in[1] == 'a') {
				return 0;
			}
			else if (in[1] == 'i') {
				return 1;
			}
			else if (in[1] == 'u') {
				return 2;
			}
		}
	}
	return -1;
}

void send_trit(char trit, HANDLE srlout) {
	if (srlout) {

	}
}

char get_address(HANDLE srlin) {
	int i = 3;
	char address = 0;
	while (i && connected) {
		char trit = get_trit(srlin);
		if (trit != -1) {
			address *= 3;
			address += trit;
			i--;
		}
	}
	return address;
}

void read_packet_header(HANDLE srlin) {
	char trit = get_trit(srlin);
	if (trit == 2) {
		int to_address = get_address(srlin);
	}
}