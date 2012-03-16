// modbus_client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace std;

#define READ_COMMAND  0x03
#define WRITE_COMMAND 0x10

#define MAXIMUM_NUMBER_OF_REGISTERS 123

#define IP_ADDRESS "192.168.1.230"
#define IP_PORT    502

#define BUFFER_SIZE 16

uint16_t register_values[MAXIMUM_NUMBER_OF_REGISTERS];

unsigned int i;
char buffer[BUFFER_SIZE];

unsigned int command;
unsigned int starting_address;
unsigned int number_of_registers;


struct timeval response_timeout;

modbus_t *ctx;
int register_count;

int init_modbus(){
	// init modbus
  ctx = modbus_new_tcp(IP_ADDRESS, IP_PORT);
	modbus_set_debug(ctx, true);

	response_timeout.tv_sec = 10;
  response_timeout.tv_usec = 0;
	modbus_set_response_timeout(ctx, &response_timeout);

	modbus_get_response_timeout(ctx, &response_timeout);
	printf("Timeout: %d.%ds\n", response_timeout.tv_sec, response_timeout.tv_usec);

	if (modbus_connect(ctx) == -1) {
    fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
    modbus_free(ctx);
    return -1;
	}
	return 0;
}

int process_write_command() {
	if (-1 == init_modbus()) {
		return -1;
	}

	// get the write values
	for (i = 0; i < number_of_registers; i++) {
		cin.getline(buffer, BUFFER_SIZE);
		sscanf_s(buffer, "%u", &register_values[i]);
		printf("reg[%d]=%u\n", starting_address + i, register_values[i]);
	}

	register_count  = modbus_write_registers(ctx, starting_address, number_of_registers, register_values);
	if (-1 == register_count) {
	  fprintf(stderr, "%s\n", modbus_strerror(errno));
    return -1;
	}

	modbus_close(ctx);
	modbus_free(ctx);

	return 0;
}

int process_read_command(){
	if (-1 == init_modbus()) {
		return -1;
	}

	register_count  = modbus_read_registers(ctx, starting_address, number_of_registers, register_values);
	if (-1 == register_count) {
	  fprintf(stderr, "%s\n", modbus_strerror(errno));
    return -1;
	}

	for (i = 0; i < number_of_registers; i++) {
		printf("reg[%d]=%u\n", starting_address + i, register_values[i]);
	}

	modbus_close(ctx);
	modbus_free(ctx);

	return 0;
}

int _tmain(int argc, _TCHAR* argv[]) {
	cin.getline(buffer, 16);
	sscanf_s(buffer, "%x %u %u", &command, &starting_address, &number_of_registers);
	printf("got command 0x%X start addr: %u num of regs %u\r\n", command, starting_address, number_of_registers);

	if (WRITE_COMMAND == command) {
		return process_write_command();
	} else if (READ_COMMAND == command) {
		return process_read_command();
	} else {
		printf("Unknown command supplied: 0x%X\r\n", command);
		return -1;
	}
}
