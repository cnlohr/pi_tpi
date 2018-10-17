all : osccal.bin burn

AVRPART=attiny10
AVRCFLAGS=-g -Wall -Os -mmcu=$(AVRPART) -DF_CPU=8000000UL
AVRASFLAGS:=$(AVRCFLAGS)


tpiflash : gpio_tpi.c gen_ios.c tpiflash.c
	gcc -o $@ $^ -Os

firmware.bin : firmware.c firmware.S
	avr-gcc -I  -g $(AVRCFLAGS)   -mmcu=$(AVRPART) -Wl,-Map,firmware.map -o firmware.elf $^
	avr-objdump firmware.elf -S > firmware.lst
	avr-objcopy -j .text -j .data -O binary firmware.elf firmware.bin 

osccal.bin : osccal.c
	avr-gcc -I  -g $(AVRCFLAGS)   -mmcu=$(AVRPART) -o osccal.elf $^
	avr-objcopy -j .text -j .data -O binary osccal.elf osccal.bin
	rm osccal.elf


calibrate : osccal.bin tpiflash
	./tpiflash o 1e9003

burn : tpiflash firmware.bin
	./tpiflash w 1e9003 firmware.bin

clean :
	rm -rf *.o *~ tpiflash *.elf *.bin


