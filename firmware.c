#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

int main()
{
	cli();
	DDRB = 1;

	CCP = 0xD8; //Enable CCP.
	CLKPSR = 0; //Go for 8 MHz operation.

	while(1) {
		asm volatile( "WDR" );
		PORTB = 1;
		PORTB = 0;
		PORTB = 1;
		PORTB = 0;
	}
}


