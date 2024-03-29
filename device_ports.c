#include "device_ports.h"

#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <string.h>

#include <process.h>
#include <io.h>

#include "io.h"

intptr_t device_handle;
const char* device = NULL;

#ifdef _WIN32

inline int createDevice(const char * name) {
	if (device) return -1;
	device = name;
	srlio[0] = CreateNamedPipe("\\\\.\\pipe\\srlin", PIPE_ACCESS_INBOUND, PIPE_TYPE_BYTE | PIPE_NOWAIT, 1, 0, 32, 0, NULL);
	if (!srlio[0]) return -1;
	srlio[1] = CreateNamedPipe("\\\\.\\pipe\\srlout", PIPE_ACCESS_OUTBOUND, PIPE_TYPE_BYTE | PIPE_NOWAIT, 1, 32, 0, 0, NULL);
	if (!srlio[1]) return -1;
	device_handle = _spawnlp(_P_NOWAIT, "cmd", "cmd", "/C", "conhost", name, NULL);
	if (!device_handle) return -1;
	if (!ConnectNamedPipe(srlio[0], NULL)) return -1;
	if (!ConnectNamedPipe(srlio[1], NULL)) return -1;
	return 0;
}

inline int removeDevice() {
	if (!DisconnectNamedPipe(srlio[0])) return -1;
	if (!DisconnectNamedPipe(srlio[1])) return -1;
	if (!TerminateProcess((HANDLE) device_handle, 0)) return -1;
	return 0;
}

#else

#endif

int toggleDevice(const char * name) {
	if (device) {
		if (strcmp(device, name) == 0) {
			return unplugDevice();
		}
		else return -1;
	}
	return plugInDevice(name);
}

int plugInDevice(const char * name) {
	if (device) return -1;
	return createDevice(name);
}

int unplugDevice() {
	return removeDevice();
}