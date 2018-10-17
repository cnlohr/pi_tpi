all : burn

AVRPART=attiny10
AVRCFLAGS=-g -Wall -Os -mmcu=$(AVRPART) -DF_CPU=20000000UL
AVRASFLAGS:=$(AVRCFLAGS)


tpiflash : gpio_tpi.c gen_ios.c tpiflash.c
	gcc -o $@ $^ -Os

firmware.bin : firmware.elf
	avr-objcopy -j .text -j .data -O binary firmware.elf firmware.bin 

firmware.elf : firmware.c
	avr-gcc -I  -g $(AVRCFLAGS)   -mmcu=$(AVRPART) -Wl,-Map,firmware.map -o $@ $^
	avr-objdump $@ -S > firmware.lst

test.bin : test.elf
	avr-objcopy -j .text -j .data -O binary test.elf test.bin 


burn : tpiflash firmware.bin
	./tpiflash w 1e9003 firmware.bin

clean :
	rm -rf *.o *~ tpiflash *.elf *.bin


