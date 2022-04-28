#ifndef IPMI_I2C_COMMANDS_H
#define IPMI_I2C_COMMANDS_H

#define APP_NETFN             0x06
#define MASTER_READ_WRITE_CMD 0x52 

int read_i2c(uint8_t bus, uint8_t addr, uint8_t reg, uint8_t* data);
int write_i2c(uint8_t bus, uint8_t addr, uint8_t reg, uint8_t val);

#endif
