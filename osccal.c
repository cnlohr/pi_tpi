#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

//A file that executes an even interval up-and-down to calibrate internal oscillator timings.  Designed to operate on an ATTiny4/5/9/10

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


