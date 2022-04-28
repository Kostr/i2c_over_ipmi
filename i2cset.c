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
	printf("  ./%s <I2C_BUS> <I2C_ADDR> <I2C_REG> <VAL>\n", program_name);
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

	if (argc != 5) {
		fprintf(stderr, "Error! Not enough arguments\n");
		usage(argv[0]);
		return 1;
	}

	uint8_t bus, addr, reg, val;
	if (str_to_uint8(argv[1], &bus)) {
		fprintf(stderr, "Error! Wrong number for the <I2C_BUS> value\n");
		usage(argv[0]);
		return 2;
	}
	if (str_to_uint8(argv[2], &addr)) {
		fprintf(stderr, "Error! Wrong number for the <I2C_ADDR> value\n");
		usage(argv[0]);
		return 3;
	}
	if (str_to_uint8(argv[3], &reg)) {
		fprintf(stderr, "Error! Wrong number for the <I2C_REG> value\n");
		usage(argv[0]);
		return 4;
	}
	if (str_to_uint8(argv[4], &val)) {
		fprintf(stderr, "Error! Wrong number for the <VAL> value\n");
		usage(argv[0]);
		return 5;
	}

	if (write_i2c(bus, addr, reg, val)) {
		fprintf(stderr, "Error\n");
	}

	return 0;
}
