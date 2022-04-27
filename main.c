#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/io.h>
#include "ipmi.h"

#define APP_NETFN             0x06
#define MASTER_READ_WRITE_CMD 0x52 


int read_i2c(uint8_t bus, uint8_t addr, uint8_t reg, uint8_t* data)
{
	uint8_t cmd_data[5];
	cmd_data[0] = ((bus & 0x7) << 1) | 0x1;
	cmd_data[1] = (addr << 1);
	cmd_data[2] = 1;
	cmd_data[3] = reg;

	int status, data_sz;
	int rc = ipmicmd(BMC_SA, 0, APP_NETFN, MASTER_READ_WRITE_CMD, 4, &cmd_data, 1, &data_sz, &status);
	if (rc) {
#ifdef DEBUG
        	printf("Error! ipmicmd: rc=%d, status=0x%08x\n", rc, status);
#endif
	} else {
		*data = (uint8_t)(status & 0xff);
	}
	return rc;
}

void usage(char* program_name)
{
	printf("Usage:\n");
	printf("  ./%s <I2C_BUS> <I2C_ADDR>\n", program_name);
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
		printf("Error! Need sudo priviliges\n");
		exit(1);	
	}

	if (argc != 3) {
		printf("Error! Not enough arguments\n");
		usage(argv[0]);
		return 1;
	}

	uint8_t bus, addr;
	if (str_to_uint8(argv[1], &bus)) {
		printf("Error! Wrong number for the <I2C_BUS> value\n");
		usage(argv[0]);
		return 2;
	}
	if (str_to_uint8(argv[2], &addr)) {
		printf("Error! Wrong number for the <I2C_ADDR> value\n");
		usage(argv[0]);
		return 3;
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
