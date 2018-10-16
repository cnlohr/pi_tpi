#ifndef _GEN_IOS_H
#define _GEN_IOS_H

int  InitGenGPIO();
void GPIODirection( int iono, int out );
int  GPIOGet( int iono );
void GPIOSet( int iono, int turnon );

#endif

