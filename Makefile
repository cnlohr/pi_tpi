all : burn

AVRPART=attiny10
AVRCFLAGS=-g -Wall -Os -mmcu=$(AVRPART) -DF_CPU=20000000UL
AVRASFLAGS:=$(AVRCFLAGS)


test_tpi : gpio_tpi.c gen_ios.c
	gcc -o $@ $^

firmware.bin : firmware.elf
	avr-objcopy -j .text -j .data -O binary firmware.elf firmware.bin 

firmware.elf : firmware.c
	avr-gcc -I  -g $(AVRCFLAGS)   -mmcu=$(AVRPART) -Wl,-Map,firmware.map -o $@ $^

test.bin : test.elf
	avr-objcopy -j .text -j .data -O binary test.elf test.bin 


burn : test_tpi firmware.bin
	./test_tpi firmware.bin

clean :
	rm -rf *.o *~ test_tpi *.elf *.bin


