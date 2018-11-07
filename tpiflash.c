#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gpio_tpi.h"
#include "gen_ios.h"
#include <sys/time.h>

double GTime()
{
	struct timeval tv;
	gettimeofday(&tv, 0);
	return tv.tv_sec + tv.tv_usec / 1000000.0;
}

int FlashFile( const char * filename, int silent )
{
	FILE * f = fopen( filename, "rb" );
	if( !f )
	{
		fprintf( stderr, "Error: can't open bin file: %s\n", filename );
		return -6;
	}
	fseek( f, 0, SEEK_END );
	int len = ftell( f );
	fseek( f, 0, SEEK_SET );
	uint8_t buffer[len+1];
	if( fread( buffer, len, 1, f ) != 1 )
	{
		fprintf( stderr, "Error: can't read contents of binary\n" );
		return -7;
	}
	fclose( f );

	if( len & 1 ) len++; //Make sure we have full words.

	if( TPIEraseAndWriteAllFlash( buffer, len, silent ) )
	{
		if( !silent )
		{
			TPIDump( 0x4000, len, "FLASH" );
			printf( "\n" );
		}
		return -1;
	}
	return 0;
}


int Init( const char * code )
{
	int initcode = TPIInit();
	if( initcode < 0 )
	{
		fprintf( stderr, "Error: Cannot connect to ATTiny (Error %d)\n", initcode );
		exit( -3 );
	}
	char initcodesat[10];
	sprintf( initcodesat, "%06x", initcode );
	if( memcmp( code, initcodesat, 6 ) != 0 )
	{
		fprintf( stderr, "Error: Device code mismatch.  Expected: \"%s\", got \"%s\"\n", code, initcodesat ); 
		exit( -5 );
	}

}

//This assumes you are NOT in programming mode.
double GetMHzOfOSCCAL( uint8_t osccal, const char * code )
{
	int i;
	Init( code );

	if( FlashFile( "osccal.bin", 1 ) ) { TPIEnd(); return -11; }
	uint8_t wordset[2] = { osccal, 0xff };
	TPIWriteFlashWord( 0x40fe, wordset );
	TPIEnd();

	//wait for line to go low.
	double Last = GTime();
	double Now = GTime();
	int tries = 5;
	double Times[tries];
	double TotalTimes = 0;

	for( i = 0; i < tries; i++ )
	{
		Last = GTime();
		while( GPIOGet( GPDAT ) && ( (Now = GTime()) - Last ) < 1.0 );
		if( Now-Last >= 1.0 ) goto timeout;
		Last = GTime();
		while( !GPIOGet( GPDAT ) && ( (Now = GTime()) - Last ) < 1.0 );
		if( Now-Last >= 1.0 ) goto timeout;
		Times[i] = Now-Last;
		TotalTimes += Now - Last;
	}

	TotalTimes /= tries;

	double mostaccuratediff = 1e20;
	double mostaccurate = 0;
	//Find time closest to average.
	for( i = 0; i < tries; i++ )
	{
		double diff = Times[i] - TotalTimes;
		if( diff < 0 ) diff = -diff;
		if( diff < mostaccuratediff )
		{
			mostaccuratediff = diff;
			mostaccurate = Times[i];
		}
	}

	double MHz = 1.0/(mostaccurate/50000.0);
	return MHz;

timeout:
	fprintf( stderr, "Error: Timeout on timer\n" );
	return -5;
}

int ReadEasy( const char * readdata )
{
	if( readdata[0] == '0' )
	{
		if( readdata[1] == 'x' || readdata[1] == 'X' )
		{
			return strtol( readdata+2, NULL, 16 );
		}
		else
			return strtol( readdata+1, NULL, 8 );
	}
	else
		return strtol(readdata, NULL, 10);
}

int main( int argc, char ** argv )
{
	int i;

	if( argc < 3 || strlen( argv[2] ) != 6 )
	{
		fprintf( stderr, "Error: Usage: tpiflash [w/e/c/r/o/p] [device id]\n\
	1e8f0a - ATTiny4\n\
	1e8f09 - ATTiny5\n\
	1e9008 - ATTiny9\n\
	1e9003 - ATTiny10\n\n\
	w: write flash.  Extra parameter  [binary file to flash]\n\
	e: erase chip.\n\
	c: config chip. Extra parameter   [config byte, default 0 (inverted)]\n\
	r: dump chip memories\n\
	o: oscillator calibration. Extra par [target MHz, optional or 'all']\n\
	p: poke.  Extra: [address in flash] [value]\n");
		return -1;
	}


	Init( argv[2] );

	if( argv[1][0] == 'E' || argv[1][0] == 'e' )
	{
		if( TPIErase() )
		{
			fprintf( stderr, "Error: Can't erase chip\n" );
			return -1;
		}

		printf( "Erased.\n" );
	}
	else if( argv[1][0] == 'C' || argv[1][0] == 'c' )
	{
		if( argc < 4 )
		{
			fprintf( stderr, "Error: Need more flags.\n" );
			return -4;
		}

		int word = ~atoi( argv[3] );

		if( TPIEraseSection( 0x3f41 ) )
		{
			fprintf( stderr, "Error: Can't erase config.\n" );
			return -14;
		}

		uint8_t wordset[2] = { word, 0xff };
		TPIWriteFlashWord( 0x3f40, wordset );

		printf( "Set.\n" );
		TPIDump( 0x3f40, 0x02, "CONFIG" );

	}
	else if( argv[1][0] == 'P' || argv[1][0] == 'p' )
	{
		if( argc != 5 )
		{
			TPIEnd();
			fprintf( stderr, "Usage: p [dev id] [address, START AT 0x4000 for flash, must be word aligned] [value (word)]\n" );
			return -47;
		}
		uint16_t addy = ReadEasy( argv[3] );
		uint16_t val = ReadEasy( argv[4] );
		uint8_t matching[2];
		int r =  TPIReadData( addy, matching, 2 );
		if( r < 0 )
		{
			TPIEnd();
			fprintf( stderr, "Can't read specified value.\n" );
			return -46;
		}
		if( matching[0] != 0xff || matching[1] != 0xff )
		{
			TPIEnd();
			fprintf( stderr, "Can't write to unerased flash.\n" );
			return -45;
		}
		matching[0] = val&0xff;
		matching[1] = val>>8;
		TPIWriteFlashWord( addy, matching );
		uint8_t verify[2] = { 0xff, 0xff };
		r =  TPIReadData( addy, verify, 2 );
		if( r == 0  && matching[0] == verify[0] && matching[1] == verify[1] )
		{
			printf( "[0x%04x] = 0x%02x%02x\n", addy, verify[0], verify[1] );
		}
		else
		{
			TPIEnd();
			printf( "%d: [0x%04x] = 0x%02x%02x\n", r , addy, verify[0], verify[1] );
			fprintf( stderr, "Verification failed.\n" );
			return -44;
		}
	}
	else if( argv[1][0] == 'R' || argv[1][0] == 'r' )
	{
		TPIDump( 0x00, 0x60, "IO + RAM" );
		TPIDump( 0x3f00, 0x02, "NVM" );
		TPIDump( 0x3f40, 0x02, "CONFIG" );
		TPIDump( 0x3f80, 0x02, "CALIB" );
		TPIDump( 0x3fC0, 0x04, "DEVID" );
		TPIDump( 0x4000, 0x400, "FLASH" );
	}
	else if( argv[1][0] == 'W' || argv[1][0] == 'w' )
	{
		if( argc < 4 )
		{
			fprintf( stderr, "Error: Need a binary file\n" );
			return -4;
		}

		FlashFile( argv[3], 0 );
	}
	else if( argv[1][0] == 'O' || argv[1][0] == 'o' )
	{
		if( argc < 4 )
		{
			fprintf( stderr, "No target parameter for speed set.  Printing speed at default cal.\n" );
			uint8_t osccal[2];
			int r =  TPIReadData( 0x3f80, osccal, 2 );
			if( r < 0 )
			{
				fprintf( stderr, "Error: can't read initial osccal\n" );
				TPIEnd();
				return -5;
			}
			TPIEnd();
			double MHz = GetMHzOfOSCCAL( osccal[0], argv[2] );
			printf( "%d, %f\n", osccal[0], MHz );
		}
		else if( strcmp( argv[3], "all" ) == 0 )
		{
			int cal = 0;
			TPIEnd();
			for( ; cal < 256; cal++ )
			{
				double MHz = GetMHzOfOSCCAL( cal, argv[2] );
				printf( "%d, %f\n", cal, MHz );
				fflush( stdout );
			}

		}
		else
		{
			double target = atof( argv[3] );
			int osccal = 0x80;
			int jump = 0x40;
			int tries = 0;

			int best_osc = 0x80;
			double best_cal = 1e20;
			double freqat = 1e20;
			TPIEnd();
			for( tries = 0; tries < 16; tries++ )
			{
				double MHz = GetMHzOfOSCCAL( osccal, argv[2] );
				if( MHz < 0 ) return -98;
				jump = jump * .8;
				fprintf( stderr, "%d, %f\n", osccal, MHz );
				double diff = MHz - target;
				if( diff < 0 ) diff = -diff;
				if( diff < best_cal ) {
					best_osc = osccal;
					best_cal = diff;
					freqat = MHz;
				} 


				if( MHz < target ) osccal += jump;
				else osccal -= jump;
				if( osccal < 0 ) osccal = 0;
				if( osccal > 0xff ) osccal = 0xff;
			}
			printf( "%d, %f, %f, %3.4f%%\n", best_osc, freqat, best_cal, best_cal/freqat*100. );
		}
		return 0;
timeout:
		fprintf( stderr, "Error: Timed out on calibration tool\n" );
		return 0;
	}
	else
	{
		printf( "Unknown flash command\n" );
	}
	TPIEnd();

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


