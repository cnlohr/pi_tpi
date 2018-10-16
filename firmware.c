#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

volatile const char fv[100] PROGMEM = { 1 };

int main()
{
	cli();
	DDRB = 1;

	pgm_read_byte( fv + 100 );
	while(1) {
		PORTB |= 1;
		PORTB &= ~1;
	}
}


