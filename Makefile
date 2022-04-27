i2cdump_over_ipmi: main.o ipmi.o
	gcc main.o ipmi.o -o i2cdump_over_ipmi

clean:
	rm *.o i2cdump_over_ipmi
