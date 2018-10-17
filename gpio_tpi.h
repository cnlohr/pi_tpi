#ifndef _GPIO_TPI_H
#define _GPIO_TPI_H

#include <stdint.h>

//You probably want these...
int TPIInit();	//Verify your device ID.
int TPIErase();
int TPIEraseSection( uint16_t secadd );
int TPIWriteFlashWord( uint16_t address, const uint8_t * data ); //NOTE: address should have 0x4000 offset.
int TPIEraseAndWriteAllFlash( const uint8_t * data, int length );
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

