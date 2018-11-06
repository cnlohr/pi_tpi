#include "gpio_tpi.h"
#include "gen_ios.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>

//Portions of this are roughly based on:
//	https://pcm1723.hateblo.jp/entry/20111208/1323351725


void ClockDelay() { int i = 1000; do { asm volatile ("nop"); } while( i-- ); } //700ksps (@i = 500) (slow enough for most systems) ... on further use, i = 1000 seems more reliable.

int did_send_last = 0;

static void SendBit( uint8_t bit )
{
	GPIOSet( GPCLK, 0 );
	GPIOSet( GPDAT, bit );
	ClockDelay();
	GPIOSet( GPCLK, 1 );
	ClockDelay();
}

static uint8_t Parity( uint8_t val )
{
	uint8_t par = val;
	par ^= (par >> 4); // b[7:4] (+) b[3:0]
	par ^= (par >> 2); // b[3:2] (+) b[1:0]
	par ^= (par >> 1); // b[1] (+) b[0]
	return par&1;
}

void TPISend( uint8_t tx )
{
	uint16_t word;
	int i;
	GPIODirection( GPDAT, 1 );

	word = 0 | (((uint16_t)tx)<<1) | (Parity(tx)<<9) | ( 0b1111 << 10 );

	if( !did_send_last )
	{
		//Insert 2 idle ticks.
		for( i = 0; i < 2; i++ )
		{
			SendBit( 1 );
		}
	}
	for( i = 0; i < 12; i++ )
	{
		SendBit( (word & (1<<i))?1:0 );
	}

	did_send_last = 1;
}

int TPIReceive()
{
	int i;
	uint16_t datl = 0 ;
	GPIODirection( GPDAT, 0 );

	did_send_last = 0;

	for( i = 0; i < 20; i++ )
	{
		SendBit( 1 );
		if( GPIOGet( GPDAT ) == 0 ) break;
	}
	if( i == 20 ) return -1;

	//Includes extra dummy cycle to flush the attiny.  It "works" with 11 it seems.
	for( i = 0; i < 12; i++ )
	{
		GPIOSet( GPCLK, 0 );
		ClockDelay();
		GPIOSet( GPCLK, 1 );
		ClockDelay();
		if( GPIOGet( GPDAT ) )
		{
			datl |= 1<<i;
		}
	}
	if( ((datl >> 9)&3) != 3 )	return -1;
	if( Parity( datl ) ^ ((datl>>8)&1) ) return -2;
	return datl & 0xff; 
}



void TPIBreak()
{
	int i;

	GPIODirection( GPDAT, 1 );
	for( i = 0; i < 12; i++ )
	{
		SendBit( 0 );
	}
}

int TPIInit()
{
	int i;
	InitGenGPIO();

	//NOTE: MUST HAVE PULL-UP ON DAT.

	GPIODirection( GPRST, 1 );
	GPIODirection( GPCLK, 1 );
	GPIODirection( GPDAT, 1 );
	GPIOSet( GPRST, 1 );

	usleep( 1000 );
	GPIOSet( GPRST, 0 );
	usleep( 10 );

	for( i = 0; i < 16; i++ )
	{
		SendBit( 1 );
	}


	TPISend( 0xC2 );	//SSTCS 0b0010 0000 0100
	TPISend( 0x07 );	//Configures the dead time on bus turn-around.

	{
		uint64_t nvm_key = 0x1289AB45CDD888FFULL;
		TPISend( 0b11100000 ); // SKEY function
		while(nvm_key)
		{
			TPISend(nvm_key & 0xFF);
			nvm_key >>= 8;
		} // while
	}

	TPISend( 0b00010000 );
	if( TPIReceive() < 0 ) return -1;

	uint8_t devid[3];
	TPIReadData( 0x3fc0, devid, 3 );

	return devid[0]<<16 | devid[1]<<8 | devid[2];
}

void TPIEnd()
{
	//Exit NVMEN
	TPISend( 0xC0 );	//SSTCS 0b0010 0000 0100
	TPISend( 0x00 );

	SendBit(1); //Flush.
	SendBit(1);

	GPIODirection( GPRST, 0 );
	GPIODirection( GPCLK, 0 );
	GPIODirection( GPDAT, 0 );
}

int TPIReadIO( uint8_t address )
{
	int low = address & 0xf;
	int high = ( address & 0x30 ) << 1;
	TPISend( 0b00010000 | low | high ); // SIN 0aa1 aaaa
	return TPIReceive();
}

void TPIWriteIO( uint8_t address, uint8_t value )
{
	int low = address & 0xf;
	int high = ( address & 0x30 ) << 1;
	TPISend( 0b10010000 | low | high ); // SIN 0aa1 aaaa
	TPISend( value );
}

void TPISetPR( uint16_t address )
{
	//Initialize Memory Pointer Transfer
	TPISend(0x68); // SSTPR 0
	TPISend(address); //
	TPISend(0x69); // SSTPR 1
	TPISend(address>>8); //
}

int TPIReadData( uint16_t address, uint8_t * data, int length )
{
	TPISetPR( address );
	while( length-- )
	{
		TPISend( 0b00100100 ); // SLD with post-increment.
		int r = TPIReceive();
		if( r < 0 ) return r;
		*(data++) = r;
	}
	return 0;
}

void TPIWriteData( uint16_t address, const uint8_t * data, int length )
{
	TPISetPR( address );
	while( length-- )
	{
		TPISend( 0b01100100 ); // SST with post-increment. Bug in datasheet.  
		TPISend( *(data++) );
	}
	return;
}

int TPIEraseSection( uint16_t secadd )
{
	int i;
	TPIWriteIO( NVMCMD, 0x14 ); //Section erase.
	TPIWriteData( secadd|1, "x", 1 ); //Dummy write.
	for( i = 0; i < 1000; i++ )
	{
		int rx = TPIReadIO( NVMCSR );
		if( rx < 0 ) return -1;
		if( rx == 0 ) return 0;
		usleep(100);
	}
	return -2;
}

int TPIErase()
{
	int i;
	TPIWriteIO( NVMCMD, 0x10 ); //Chip erase.
	TPIWriteData( 0x4001, "x", 1 ); //Dummy write.
	for( i = 0; i < 1000; i++ )
	{
		int rx = TPIReadIO( NVMCSR );
		if( rx < 0 ) return -1;
		if( rx == 0 ) return 0;
		usleep(100);
	}
	return -2;
}

int TPIWriteFlashWord( uint16_t address, const uint8_t * data )
{
	int i;
	TPIWriteIO( NVMCMD, 0x1D ); //Write word.
	TPIWriteData( address, data, 2 ); //Data write.
	for( i = 0; i < 1000; i++ )
	{
		int rx = TPIReadIO( NVMCSR );
		if( rx < 0 ) return -1;
		if( rx == 0 ) return 0;
		usleep(100);
	}
	return -2;
}





void TPIDump( int start, int length, const char * name )
{
	uint8_t buffer[length];

	printf( "%04x - %04x: %s\n", start, start + length, name );
	int r =  TPIReadData( start, buffer, sizeof( buffer ) );
	if( r )
	{
		fprintf( stderr, "Could not read data space\n" );
	}
	else
	{
		int i;
		for( i = 0; i < length; i++ )
		{
			if( ( i & 0x0f ) == 0x00 ) printf( "%04x: ", i  + start) ;
			printf( "%02x ", buffer[i] );
			if( ( i & 0x0f ) == 0x0f ) printf("\n" );
		}
	}
	printf("\n");
}



int TPIEraseAndWriteAllFlash( const uint8_t * data, int length, int quiet )
{
	int i;

	if( TPIErase() )
	{
		fprintf( stderr, "Error: Can't erase chip\n" );
		return -1;
	}

	if( !quiet ) fprintf( stderr, "Erased.\n" );

	for( i = 0; i < length; i+=2 )
	{
		TPIWriteFlashWord( i + 0x4000, data + i );
		if( !quiet && ( i & 0x20 ) == 0 )
		{
			fprintf( stderr, "." );
			fflush( stderr );
		}
	}

	if( !quiet ) fprintf( stderr, ".\n" );
	uint8_t verify[length];
	int r =  TPIReadData( 0x4000, verify, length );

	if( memcmp( verify, data, length ) == 0 )
	{
		if( !quiet ) fprintf( stderr, "Verified OK.\n" );
		return 0;
	}
	else
	{
		fprintf( stderr, "Verification failed.\n" );
		return -1;
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

