#include <stdint.h>
#include "ipmi_i2c_commands.h"
#include "ipmi.h"

int read_i2c(uint8_t bus, uint8_t addr, uint8_t reg, uint8_t* data)
{
	uint8_t cmd_data[4];
	cmd_data[0] = ((bus & 0x7) << 1) | 0x1;
	cmd_data[1] = (addr << 1);
	cmd_data[2] = 1;
	cmd_data[3] = reg;

	int status, data_sz;
	int rc = ipmicmd(BMC_SA, 0, APP_NETFN, MASTER_READ_WRITE_CMD, sizeof(cmd_data), &cmd_data, 1, &data_sz, &status);
	if (rc) {
#ifdef DEBUG
        	fprintf(stderr, "Error! ipmicmd: rc=%d, status=0x%08x\n", rc, status);
#endif
	} else {
		*data = (uint8_t)(status & 0xff);
	}
	return rc;
}

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
        	fprintf(stderr, "Error! ipmicmd: rc=%d, status=0x%08x\n", rc, status);
	}
#endif
	return rc;
}
