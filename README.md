# pi_tpi
Using a Raspberry Pi to program an ATTiny4/5/9/10.  But, mostly the $0.25 ATTiny10.  And, not necessarily a Raspberry Pi, but anything with GPIOs!

Yep.  It works.  At least tested pretty thurrougly with my ATTiny10.  It can also read all the memories _AND_ run timings against the AVR to do processor clock calibration, as well as fuses and manual poking to flash en post to allow for custom configuration.

## Physical connections

For this demo, connect an ATTiny10 in the following way:

```RPI
 GPIO 2 -> RESET
 GPIO 3 -> TPICLK
 GPIO 4 -> TPIDAT
```

Of course, you may hook it up however you want, but as a warning, if you do hook it up significantly differently, RESET and TPIDAT may require pull-up resistors.

You may modify the GPIO # the ports are on in gpio_tpi.h.


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

## Added notes.

This was actually written in two days for Unit-E Technologies and is made available freely.  This project is licensable via the newbsd license.

