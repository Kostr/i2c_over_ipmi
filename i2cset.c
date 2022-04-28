#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/io.h>
#include "ipmi.h"

#define APP_NETFN             0x06
#define MASTER_READ_WRITE_CMD 0x52 


int write_i2c(uint8_t bus, uint8_t addr, uint8_t reg, uint8_t val)
{
	uint8_t cmd_data[5];
	cmd_data[0] = ((bus & 0x7) << 1) | 0x1;
	cmd_data[1] = (addr << 1);
	cmd_data[2] = 1;
	cmd_data[3] = reg;
	cmd_data[4] = val;

	int status, data_sz;
	int rc = ipmicmd(BMC_SA, 0, APP_NETFN, MASTER_READ_WRITE_CMD, sizeof(cmd_data), &cmd_data, 1, &data_sz, &status);
#ifdef DEBUG
	if (rc) {
        	printf("Error! ipmicmd: rc=%d, status=0x%08x\n", rc, status);
	}
#endif
	return rc;
}

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
		printf("Error! Need sudo priviliges\n");
		exit(1);	
	}

	if (argc != 5) {
		printf("Error! Not enough arguments\n");
		usage(argv[0]);
		return 1;
	}

	uint8_t bus, addr, reg, val;
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
	if (str_to_uint8(argv[3], &reg)) {
		printf("Error! Wrong number for the <I2C_REG> value\n");
		usage(argv[0]);
		return 4;
	}
	if (str_to_uint8(argv[4], &val)) {
		printf("Error! Wrong number for the <VAL> value\n");
		usage(argv[0]);
		return 5;
	}

	if (write_i2c(bus, addr, reg, val)) {
		printf("Error\n");
	}

	return 0;
}
