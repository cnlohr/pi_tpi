#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

int main()
{
	cli();
	DDRB = 1;
	WDTCSR = 0;
	while(1) {
		PORTB |= 1;
		PORTB &= ~1;
	}
}


