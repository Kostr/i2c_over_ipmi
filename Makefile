all: i2cdump_over_ipmi i2cget_over_ipmi i2cset_over_ipmi

i2cdump_over_ipmi: i2cdump.o ipmi.o
	gcc i2cdump.o ipmi.o -o i2cdump_over_ipmi

i2cget_over_ipmi: i2cget.o ipmi.o
	gcc i2cget.o ipmi.o -o i2cget_over_ipmi

i2cset_over_ipmi: i2cset.o ipmi.o
	gcc i2cset.o ipmi.o -o i2cset_over_ipmi

clean:
	rm *.o i2cdump_over_ipmi i2cget_over_ipmi i2cset_over_ipmi
