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
	printf("  %s <I2C_BUS> <I2C_ADDR>\n", program_name);
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

	if (argc != 3) {
		fprintf(stderr, "Error! Not enough arguments\n");
		usage(argv[0]);
		exit(2);
	}

	uint8_t bus, addr;
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

	uint8_t data;
	uint8_t reg = 0x00;
	printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f");
	do {
		if (!(reg % 0x10)) {
			printf("\n");
			printf("%02x: ", reg/0x10);
		}

		if (!read_i2c(bus, addr, reg, &data)) {
			printf("%02x ", data);
		} else {
			printf("ER ");
		}


		reg++;
	} while (reg != 0x00);

	printf("\n");
	return 0;
}
