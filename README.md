This program is intendend to provide `i2cdump` similar output for the devices under I2C busses of the BMC controller from the main CPU:
```
CPU <---------> BMC ----- I2C0
    inband ipmi      |
    (KCS/BT/...)     ---- I2C1
                     |
                     ...
                     |
                     ---- I2C7
```

The communication is based on the IPMI command `Master Write-Read`:

![ipmi1.png](ipmi1.png?raw=true "ipmi1")

Its description can be found in the IPMI specification:

![ipmi2.png](ipmi2.png?raw=true "ipmi2")

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
