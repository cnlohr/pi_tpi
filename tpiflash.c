#include <stdio.h>
#include <string.h>
#include "gpio_tpi.h"

int main( int argc, char ** argv )
{
	int i;

	if( argc != 3 || strlen( argv[1] ) != 6 )
	{
		fprintf( stderr, "Error: Usage: tpiflash [device id] [binary file to flash]\n\
	1e8f0a - ATTiny4\n\
	1e8f09 - ATTiny5\n\
	1e9008 - ATTiny9\n\
	1e9003 - ATTiny10\n" );
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
		if( memcmp( argv[1], initcodesat, 6 ) != 0 )
		{
			fprintf( stderr, "Error: Device code mismatch.  Expected: \"%s\", got \"%s\"\n", argv[1], initcodesat ); 
			return -5;
		}

	}


	FILE * f = fopen( argv[2], "rb" );
	if( !f )
	{
		fprintf( stderr, "Error: can't open bin file: %s\n", argv[2] );
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

	TPIEraseAndWriteAllFlash( buffer, len );

	TPIDump( 0x4000, len, "FLASH" );
	printf( "\n" );

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



