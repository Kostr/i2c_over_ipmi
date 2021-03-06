Set of utilities to work with the devices under I2C busses of the BMC controller from the main CPU:
```
CPU <---------> BMC ----- I2C0
    inband ipmi      |
    (KCS/BT/...)     ---- I2C1
                     |
                     ...
                     |
                     ---- I2C7
```

The repository includes these utilities:
- `i2cdump_over_ipmi`
- `i2cget_over_ipmi`
- `i2cset_over_ipmi`

They naming mimics standard utilities from the `i2c-tools` package.

The communication is based on the IPMI command `Master Write-Read`:

![ipmi1.png](doc/ipmi1.png?raw=true "ipmi1")

Its description can be found in the IPMI specification:

![ipmi2.png](doc/ipmi2.png?raw=true "ipmi2")

Off course this means that for this program the `BIOS<-->BMC` communication should be working (for example via KCS interface) and the BMC controller should implement this IPMI operation in its IPMI stack.

`phosphor-ipmi-host` application from the OpenBMC project implements this command, but you need to provide a proper whitelist file `master_write_read_white_list.json` or disable whitelist functionality:

https://github.com/openbmc/phosphor-host-ipmid/blob/master/apphandler.cpp
```
    // Note: For security reason, this command will be registered only when
    // there are proper I2C Master write read whitelist
    if (populateI2CMasterWRWhitelist())
    {
        // Note: For security reasons, registering master write read as admin
        // privilege command, even though IPMI 2.0 specification allows it as
        // operator privilege.
        ipmi::registerHandler(ipmi::prioOpenBmcBase, ipmi::netFnApp,
                              ipmi::app::cmdMasterWriteRead,
                              ipmi::Privilege::Admin, ipmiMasterWriteRead);
    }
```

Also keep in mind that IPMI specification uses only 3 bits in command for I2C bus encoding. This means that it is not possible to access I2C busses with a bus number higher than 7 (`i2c-7`).

There is a solution that can be used to use bus numbers higher than 7. This goes beyond the IPMI standard, so it probably wouldn't make it upstream in OpenBMC. But you can use this extension in your designs:

`47821: Master Write-Read: Extend bus ID.` | https://gerrit.openbmc-project.xyz/c/openbmc/phosphor-host-ipmid/+/47821

In this case 7 higher bits in the first byte of `Master Write-Read` IPMI command are used to encode I2C bus. Therefore you can encode I2C busses 0..127. This is more than enough for typical design.

In case you are using this custom extension recompile programs with the `EXTENDED_BUS_ID` define:
```
make clean
make CFLAGS=-DEXTENDED_BUS_ID
```
