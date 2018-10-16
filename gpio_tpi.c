#include "gpio_tpi.h"
#include "gen_ios.h"
#include <stdio.h>
#include <unistd.h>

#define GPRST 2
#define GPCLK 3
#define GPDAT 4

int did_send_last = 0;

void SendBit( uint8_t bit )
{
	GPIOSet( GPCLK, 0 );
	GPIOSet( GPDAT, bit );
	usleep(2);
	GPIOSet( GPCLK, 1 );
	usleep(2);
	printf( ">%d", bit );
}

void TPISend( uint8_t tx )
{
	uint16_t word;
	int i;
	GPIODirection( GPDAT, 1 );

	{
		//Thanks, Stanford Bit Twiddling Hax.
		int p = 0;
		i = tx;
		while (i)
		{
			p = !p;
			i = i & (i - 1);
		}
		word = 0 | (((uint16_t)tx)<<1) | (p<<9) | ( 0b11 << 10 );
		printf( "WORD: %08x Parity: %d\n", word, p );
	}

	if( !did_send_last )
	{
		//Insert 2 idle ticks.
		for( i = 0; i < 2; i++ )
		{
			SendBit( 1 );
		}
	}
printf( "\n" );
	for( i = 0; i < 12; i++ )
	{
		SendBit( (word & (1<<i))?1:0 );
	}
printf( "\n" );

	did_send_last = 1;
}

uint8_t TPIReceive()
{
	int i;
	if( did_send_last )
	{
		GPIODirection( GPDAT, 1 );
		for( i = 0; i < 2; i++ )
		{
			SendBit( 1 );
		}
	}
	GPIODirection( GPDAT, 0 );

	for( i = 0; i < 12; i++ )
	{
		GPIOSet( GPCLK, 0 );
		usleep(2);
		GPIOSet( GPCLK, 1 );
		printf( "%d: %d\n", i, GPIOGet( GPDAT ) );
		usleep(2);
	}

	return 0;
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


int main( int argc, char ** argv )
{
	int i;
	InitGenGPIO();

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
	
	TPISend( 0b00010000 );
	printf( "%d\n", TPIReceive() );
}
/*
int  
void GPIODirection( int iono, int out );
int  GPIOGet( int iono );
void GPIOSet( int iono, int turnon );
*/

