#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/io.h>
#include "ipmi_i2c_commands.h"


void usage(char* program_name)
{
	printf("Usage:\n");
	printf("  %s <I2C_BUS> <I2C_ADDR> <I2C_REG>\n", program_name);
}

int str_to_uint8(const char* str, uint8_t* result)
{
	errno = 0;
	char* temp;

	unsigned long int val = strtoul(str, &temp, 0);

	if (temp == str ||
	    *temp != '\0' ||
	    (val == ULONG_MAX && errno == ERANGE)
	    ) {
		return -1;
	}

	if ((val & 0xFF) != val) {
		return -2;
	}

	*result = (uint8_t)(val & 0xFF);

	return 0;
}

int main(int argc, char* argv[])
{
	if (iopl(3)) {
		fprintf(stderr, "Error! Need sudo priviliges\n");
		exit(1);	
	}

	if (argc != 4) {
		fprintf(stderr, "Error! Not enough arguments\n");
		usage(argv[0]);
		exit(2);
	}

	uint8_t bus, addr, reg;
	if (str_to_uint8(argv[1], &bus)) {
		fprintf(stderr, "Error! Wrong number for the <I2C_BUS> value\n");
		usage(argv[0]);
		exit(3);
	}
#ifndef EXTENDED_BUS_ID
	if (bus > 7) {
		fprintf(stderr, "Error! IPMI specification permits bus ID only in range 0..7\n");
		fprintf(stderr, "If you want to use bus ID above 7, please recompile with EXTENDED_BUS_ID define\n");
		fprintf(stderr, "Please see README.md for details\n");
		exit(3);
	}
#endif
	if (str_to_uint8(argv[2], &addr)) {
		fprintf(stderr, "Error! Wrong number for the <I2C_ADDR> value\n");
		usage(argv[0]);
		exit(4);
	}
	if (str_to_uint8(argv[3], &reg)) {
		fprintf(stderr, "Error! Wrong number for the <I2C_REG> value\n");
		usage(argv[0]);
		exit(5);
	}

	uint8_t data;
	int ret = read_i2c(bus, addr, reg, &data);
	if (!ret) {
		printf("0x%02x\n", data);
	} else {
		fprintf(stderr, "Error\n");
	}
	return ret;
}
