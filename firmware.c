#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

//Example firmware for an ATTiny 4/5/9/10 which outputs to two SK6812RGBYs.

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

	OSCCAL = 140;

	DDRB = WSA | WSB;

	while(1)
	{

		int i;
		for( i = 0; i < 9; i++ )
		{
			uint8_t g = hue(frame+i*9);
			uint8_t y = hue(frame+42+i*9);
			uint8_t r = hue(frame+85+i*9);
			uint8_t b = hue(frame+171+i*9);

			ws_outA( g );
			ws_outA( r );
			ws_outA( b );
			ws_outA( y );
			PORTB &= ~WSA;
		}

		for( i = 0; i < 9; i++ )
		{
			uint8_t g = hue(frame-i*9-9);
			uint8_t y = hue(frame+42-i*9-9);
			uint8_t r = hue(frame+85-i*9-9);
			uint8_t b = hue(frame+171-i*9-9);


			ws_outB( g );
			ws_outB( r );
			ws_outB( b );
			ws_outB( y );
			PORTB &= ~WSB;
		}


		for( i = 0; i < 10000; i++ )
		{
			asm volatile( "WDR;" );
		}
		frame++;
	}
}




/*
	Copyright 2018 <>< Charles Lohr / Unit-E Technologies

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice,
		this list of conditions and the following disclaimer.

	2. Redistributions in binary form must reproduce the above copyright
		notice, this list of conditions and the following disclaimer in the
		documentation and/or other materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its
		contributors may be used to endorse or promote products derived from
		this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
	ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
	LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
	CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
	SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
	POSSIBILITY OF SUCH DAMAGE.
*/


