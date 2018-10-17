#ifndef _GPIO_TPI_H
#define _GPIO_TPI_H

#include <stdint.h>

#ifndef GPRST
#define GPRST 2
#endif
#ifndef GPCLK
#define GPCLK 3
#endif
#ifndef GPDAT
#define GPDAT 4
#endif


//You probably want these...
int TPIInit();	//Verify your device ID.
int TPIErase();
int TPIEraseSection( uint16_t secadd );
int TPIWriteFlashWord( uint16_t address, const uint8_t * data ); //NOTE: address should have 0x4000 offset.
int TPIEraseAndWriteAllFlash( const uint8_t * data, int length, int quiet );
void TPIBreak();
void TPIEnd();




int TPIReadIO( uint8_t address );
void TPIWriteIO( uint8_t address, uint8_t value );

//Read data from memory mapped space.
int TPIReadData( uint16_t address, uint8_t * data, int length );
void TPIWriteData( uint16_t address, const uint8_t * data, int length );

//Set pointer (For manual operations)
void TPISetPR( uint16_t address );
//Raw send/receive.
void TPISend( uint8_t tx );
int TPIReceive();


//Convenience, printf's values.
void TPIDump( int start, int length, const char * name );


#define NVMCMD 0x33
#define NVMCSR 0x32



#endif



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


