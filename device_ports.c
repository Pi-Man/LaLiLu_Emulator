#include "device_ports.h"

#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <string.h>

#include <process.h>
#include <io.h>

#include "io.h"

int id = 0;

#ifdef _WIN32

inline void createDevice(const char * name) {
	srlio[id] = CreateNamedPipe("\\\\.\\pipe\\srlin", PIPE_ACCESS_INBOUND, PIPE_TYPE_BYTE | PIPE_NOWAIT, 27, 0, 32, 0, NULL);
	srlio[id + 27] = CreateNamedPipe("\\\\.\\pipe\\srlout", PIPE_ACCESS_OUTBOUND, PIPE_TYPE_BYTE | PIPE_NOWAIT, 27, 32, 0, 0, NULL);
	id++;
	_spawnlp(_P_NOWAIT, "cmd", "cmd", "/C", "conhost", name, NULL);
	ConnectNamedPipe(srlio[id], NULL);
	ConnectNamedPipe(srlio[id + 27], NULL);
}

#else

#endif

const char* devicePorts[27] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

int toggleDevice(const char * name) {
	for (int i = 0; i < 27; i++) {
		if (devicePorts[i] && strcmp(devicePorts[i], name) == 0) {
			return unplugDevice(name);
		}
	}
	return plugInDevice(name);
}

int plugInDevice(const char * name) {
	int i;
	for (i = 0; i < 27; i++) {
		if (!devicePorts[i]) break;
	}
	if (i == 27) return -1;
	createDevice(name);
	devicePorts[i] = name;
	return i;
}

int unplugDevice(const char * name) {
	for (int i = 0; i < 27; i++) {
		if (devicePorts[i] && strcmp(devicePorts[i], name) == 0) {
			devicePorts[i] = 0;
			return i;
		}
	}
}