# pi_tpi
Using a Raspberry Pi to program an ATTiny4/5/9/10.  But, mostly the $0.25 ATTiny10.  And, not necessarily a Raspberry Pi, but anything with GPIOs!

Yep.  It works.  At least tested pretty thurrougly with my ATTiny10.  It can also read all the memories _AND_ run timings against the AVR to do processor clock calibration, as well as fuses.

For this demo, connect an ATTiny10 in the following way:

RPI
 GPIO 2 -> RESET
 GPIO 3 -> TPICLK
 GPIO 4 -> TPIDAT

Conveniently, you can also manually do high-voltage programming by connecting RESET to a 12V source instead of the Raspberry Pi.


On Raspbian:
```
apt-get install avr-libc gcc-avr build-essential git
git clone https://github.com/cnlohr/pi_tpi
```

