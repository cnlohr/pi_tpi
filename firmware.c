#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#define WSA _BV(0)
#define WSB _BV(2)

void ws_outA( uint8_t data );
void ws_outB( uint8_t data );
uint8_t hue( uint8_t place );

int main()
{
	int i = 0;
	int frame = 0;
	cli();

	CCP = 0xD8; //Enable CCP.
	CLKPSR = 0; //Go for 8 MHz operation.

	DDRB = WSA | WSB;

	while(1)
	{
		uint8_t g = hue(frame);
		uint8_t y = hue(frame+42);
		uint8_t r = hue(frame+85);
		uint8_t b = hue(frame+171);

		ws_outA( g );
		ws_outA( r );
		ws_outA( b );
		ws_outA( y );
		PORTB &= ~WSA;

		ws_outB( g );
		ws_outB( r );
		ws_outB( b );
		ws_outB( y );
		PORTB &= ~WSB;


		for( i = 0; i < 10000; i++ )
		{
			asm volatile( "WDR;" );
		}
		frame++;
	}
}


