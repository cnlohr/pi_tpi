#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

int main()
{
	cli();

	OSCCAL = pgm_read_byte( 0xfe );

	DDRB = 1;
	PORTB = 0;
	CCP = 0xD8; //Enable CCP.
	CLKPSR = 0; //Go for 8 MHz operation.

	//We do WDR resetting in here in case the WDR always on fuse is flashed.

	uint16_t i;

	while(1)
	{
		i = 9999;
		do{	//5 cycles per iteration.
			asm volatile( "WDR;" );
			i--;
		} while( i );
		PORTB = 1;
		i = 10000;
		do{	//5 cycles per iteration.
			asm volatile( "WDR;" );
			i--;
		} while( i );
		PORTB = 0;
	}

}


