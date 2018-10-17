#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gpio_tpi.h"

int main( int argc, char ** argv )
{
	int i;

	if( argc < 3 || strlen( argv[2] ) != 6 )
	{
		fprintf( stderr, "Error: Usage: tpiflash [w/e/c/r] [device id]\n\
	1e8f0a - ATTiny4\n\
	1e8f09 - ATTiny5\n\
	1e9008 - ATTiny9\n\
	1e9003 - ATTiny10\n\n\
	w: write flash.  Extra parameter  [binary file to flash]\n\
	e: erase chip.\n\
	c: config chip. Extra parameter   [config byte, default 0 (inverted)]\n\
	r: dump chip memories\n" );
		return -1;
	}


	{
		int initcode = TPIInit();
		if( initcode < 0 )
		{
			fprintf( stderr, "Error: Cannot connect to ATTiny\n" );
			return -3;
		}
		char initcodesat[10];
		sprintf( initcodesat, "%06x", initcode );
		if( memcmp( argv[2], initcodesat, 6 ) != 0 )
		{
			fprintf( stderr, "Error: Device code mismatch.  Expected: \"%s\", got \"%s\"\n", argv[1], initcodesat ); 
			return -5;
		}

	}

	if( argv[1][0] == 'E' || argv[1][0] == 'e' )
	{
		if( TPIErase() )
		{
			fprintf( stderr, "Error: Can't erase chip\n" );
			return -1;
		}

		printf( "Erased.\n" );
	}
	if( argv[1][0] == 'C' || argv[1][0] == 'c' )
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

		FILE * f = fopen( argv[3], "rb" );
		if( !f )
		{
			fprintf( stderr, "Error: can't open bin file: %s\n", argv[3] );
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

		if( TPIEraseAndWriteAllFlash( buffer, len ) )
		{
			TPIDump( 0x4000, len, "FLASH" );
			printf( "\n" );
		}
	}

	TPIEnd();


//	TPISend( 0b10000000 ); //SLDCS
//	TPISend( 0b00000000 );

#if 0
	//Read IO registers
	for( i = 0; i < 64; i++ )
	{
		printf( "%02x ", TPIReadIO(i) );
	}
	printf( "\n" );
#endif
#if 0
	//Command-and-control registers, example TPISR or NVMEN.
	for( i = 0; i < 16; i++ )
	{
		TPISend( 0b10000000 | i ); // SLDCS
		printf( "%02x ", TPIReceive() );
	}
	printf( "\n" );
#endif
#if 0
	//Command-and-control registers, example TPISR or NVMEN.
	for( i = 0; i < 16; i++ )
	{
		TPISend( 0b10000000 | i ); // SLDCS
		printf( "%02x ", TPIReceive() );
	}
	printf( "\n" );
#endif

#if 0
	TPIDump( 0x00, 0x60, "IO + RAM" );
	TPIDump( 0x3f00, 0x02, "NVM" );
	TPIDump( 0x3f40, 0x02, "CONFIG" );
	TPIDump( 0x3f80, 0x02, "CALIB" );
	TPIDump( 0x3fC0, 0x04, "DEVID" );

	if( TPIErase() )
	{
		fprintf( stderr, "Error: Can't erase chip\n" );
		return -1;
	}

	printf( "Erased.\n" );

	for( i = 0; i < 100; i++ )
	{
		printf( "WR: %d\n", TPIWriteFlashWord( i+0x4000, "xx" ) );
	}

#endif

}



