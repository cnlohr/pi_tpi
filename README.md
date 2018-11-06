# pi_tpi
Using a Raspberry Pi to program an ATTiny4/5/9/10.  But, mostly the $0.25 ATTiny10.  And, not necessarily a Raspberry Pi, but anything with GPIOs!

Yep.  It works.  At least tested pretty thurrougly with my ATTiny10.  It can also read all the memories _AND_ run timings against the AVR to do processor clock calibration, as well as fuses and manual poking to flash en post to allow for custom configuration.

## It's fast.  No, really.

This is if I re-compile my AVR code from source, and blast it onto the device.

```
avr-gcc -I  -g -g -Wall -Os -mmcu=attiny10 -DF_CPU=8000000UL   -mmcu=attiny10 -Wl,-Map,firmware.map -o firmware.elf firmware.c firmware.S
avr-objdump firmware.elf -S > firmware.lst
avr-objcopy -j .text -j .data -O binary firmware.elf firmware.bin 
./tpiflash w 1e9003 firmware.bin
Erased.
............................................................................................................
Verified OK.

real	0m1.116s
user	0m0.397s
sys	0m0.033s
```

## Physical connections

For this demo, connect an ATTiny10 in the following way:

```
  RPI      ATTINY
 GPIO 2 -> RESET
 GPIO 3 -> TPICLK
 GPIO 4 { optional 100 - 1k ohm resistor } TPIDAT 
```

Of course, you may hook it up however you want, but as a warning, if you do hook it up significantly differently, RESET and TPIDAT may require pull-up resistors.

You may modify the GPIO # the ports are on in gpio_tpi.h.

The optional resistor on TPIDAT is to allow programming at better voltages (5V)


```
#ifndef GPRST
#define GPRST 2
#endif
#ifndef GPCLK
#define GPCLK 3
#endif
#ifndef GPDAT
#define GPDAT 4
#endif
```

## High-voltage programming (to reuse the RESET pin)

Conveniently, you can also manually do high-voltage programming by connecting RESET to a 12V source instead of the Raspberry Pi!  That means you can use all 4 pins of this glorious little bugger.


## Setup instructions on Raspbian:
```
apt-get install avr-libc gcc-avr build-essential git
git clone https://github.com/cnlohr/pi_tpi
```

## Configuring use with other devices.

You can actually use this code on just about anything, since it's written as standard C code.  This means you could say have an AVR co-processor to your ESP32 or an ESP8266 or something and be able to flash it directly from that device!  Simply swap out "gen_ios.c" with the pertinent function calls and override GPRST, GPCLK and GPDAT.

## Example Usage

The 'help':

```
Error: Usage: tpiflash [w/e/c/r/o/p] [device id]
	1e8f0a - ATTiny4
	1e8f09 - ATTiny5
	1e9008 - ATTiny9
	1e9003 - ATTiny10

	w: write flash.  Extra parameter  [binary file to flash]
	e: erase chip.
	c: config chip. Extra parameter   [config byte, default 0 (inverted)]
	r: dump chip memories
	o: oscillator calibration. Extra p[target MHz, optional]
	p: poke.  Extra: [address in flash] [value]
```


```
root@rasvive:~/git/pi_tpi# ./tpiflash w 1e9003 firmware.bin
Erased.
............................................................................................................
Verified OK.
root@rasvive:~/git/pi_tpi# ./tpiflash p 1e9003 0x43f0 1
[0x43f0] = 0x0100
root@rasvive:~/git/pi_tpi# ./tpiflash r 1e9003 firmware.bin
0000 - 0060: IO + RAM
0000: 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
0010: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
0020: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
0030: 00 00 00 00 00 00 03 00 00 6f 00 03 00 00 00 00 
0040: 94 1d 25 c6 39 cc a0 6d b5 59 bb c1 d0 2b c0 4f 
0050: b2 5f a0 a2 ca 18 00 00 b6 ff 04 00 00 5f 00 12 

3f00 - 3f02: NVM
3f00: ff ff 
3f40 - 3f42: CONFIG
3f40: fd ff 
3f80 - 3f82: CALIB
3f80: 6f ff 
3fc0 - 3fc4: DEVID
3fc0: 1e 90 03 ff 
4000 - 4400: FLASH
4000: 0a c0 11 c0 10 c0 0f c0 0e c0 0d c0 0c c0 0b c0 
4010: 0a c0 09 c0 08 c0 11 27 1f bf cf e5 d0 e0 de bf 
4020: cd bf 49 d0 b6 c0 ec cf 6f 93 2f 93 60 e8 28 2f 
4030: 26 23 10 9a 11 f4 10 98 02 c0 10 9a 00 00 10 98 
4040: 66 95 a9 f7 2f 91 6f 91 08 95 6f 93 2f 93 60 e8 
4050: 28 2f 26 23 12 9a 11 f4 12 98 02 c0 12 9a 00 00 
4060: 12 98 66 95 a9 f7 2f 91 6f 91 08 95 2f 93 3f 93 
4070: 8a 32 58 f4 90 e0 28 2f 39 2f 88 0f 99 1f 88 0f 
4080: 99 1f 22 0f 33 1f 82 0f 13 c0 87 ff 0e c0 8a 3a 
4090: 70 f4 28 2f 30 e0 20 58 88 27 38 0b 82 2f 88 0f 
40a0: 22 0f 22 0f 82 0f 80 95 03 c0 8f ef 01 c0 80 e0 
40b0: 3f 91 2f 91 08 95 cf 93 df 93 00 d0 1f 93 cd b7 
40c0: de b7 f8 94 48 ed 4c bf 16 bf 4c e8 49 bf 45 e0 
40d0: 41 b9 ce 5f df 4f 18 83 c2 50 d0 40 ce 5f df 4f 
40e0: 88 81 c2 50 d0 40 c2 df 28 2f ce 5f df 4f 88 81 
40f0: c2 50 d0 40 86 5d ba df cf 5f df 4f 88 83 c1 50 
4100: d0 40 ce 5f df 4f 88 81 c2 50 d0 40 8b 5a ae df 
4110: 38 2f ce 5f df 4f 88 81 c2 50 d0 40 85 55 a6 df 
4120: cd 5f df 4f 88 83 c3 50 d0 40 82 2f 7d df 83 2f 
4130: 7b df cd 5f df 4f 88 81 c3 50 d0 40 75 df cf 5f 
4140: df 4f 88 81 c1 50 d0 40 6f df 10 98 82 2f 7d df 
4150: 83 2f 7b df cd 5f df 4f 88 81 c3 50 d0 40 75 df 
4160: cf 5f df 4f 88 81 c1 50 d0 40 6f df 12 98 40 e1 
4170: 57 e2 a8 95 41 50 51 0b e1 f7 ce 5f df 4f 48 81 
4180: c2 50 d0 40 4f 5f ce 5f df 4f 48 83 c2 50 d0 40 
4190: a5 cf f8 94 ff cf ff ff ff ff ff ff ff ff ff ff 
41a0: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
41b0: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
41c0: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
41d0: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
41e0: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
41f0: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
4200: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
4210: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
4220: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
4230: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
4240: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
4250: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
4260: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
4270: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
4280: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
4290: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
42a0: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
42b0: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
42c0: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
42d0: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
42e0: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
42f0: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
4300: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
4310: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
4320: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
4330: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
4340: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
4350: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
4360: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
4370: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
4380: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
4390: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
43a0: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
43b0: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
43c0: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
43d0: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
43e0: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
43f0: 01 00 ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
```

Here is an example of seeking to find the ideal OSCCAL value for 12.000 MHz...
```
root@rasvive:~/git/pi_tpi# ./tpiflash o 1e9003 12000000
128, 8522236.671001
179, 10526286.201877
219, 12269787.034870
187, 10862695.535067
212, 11950264.972363
232, 12873071.020809
216, 12132785.652300
204, 11600575.284877
213, 11998809.932487
220, 12315179.987081
215, 12088724.925063
211, 11910222.626079
214, 12036687.137692
212, 11952989.455685
213, 11993320.370582
213, 11993320.370582
213, 11998809.932487, 1190.067513, 0.0099%
```

You could use this to flash in a value to flash to be read by your app!

Note: You provide the device id as to prevent you from accidentally clobbering the wrong chip type.  That's what's up with the "device id" field.


## Added notes.

This was actually written in two days for Unit-E Technologies and is made available freely.  This project is licensable via the newbsd license.


## Additional reading

An Article about someone else doing something like this 6 years ago:
https://hackaday.com/2012/08/23/programming-the-attiny10-with-an-arduino/

Notes on initializing TPI mode:
https://pcm1723.hateblo.jp/entry/20111208/1323351725


