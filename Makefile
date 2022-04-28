all: i2cdump_over_ipmi i2cget_over_ipmi i2cset_over_ipmi

i2cdump_over_ipmi: i2cdump.o ipmi.o ipmi_i2c_commands.o
	gcc $^ -o $@

i2cget_over_ipmi: i2cget.o ipmi.o ipmi_i2c_commands.o
	gcc $^ -o $@

i2cset_over_ipmi: i2cset.o ipmi.o ipmi_i2c_commands.o
	gcc $^ -o $@

.PHONY: clean

clean:
	rm -f *.o i2cdump_over_ipmi i2cget_over_ipmi i2cset_over_ipmi
