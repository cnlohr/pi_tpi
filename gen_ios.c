#include "gen_ios.h"
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BCM2708_PERI_BASE       0x3F000000
#define GPIO_BASE                         (BCM2708_PERI_BASE + 0x200000)

static int gpiofd;
static uint32_t * gpiomem;

//Based on http://www.pieter-jan.com/node/15

int InitGenGPIO()
{
	int pagesize = getpagesize();

	//Obtain handle to physical memory
	if ((gpiofd = open ("/dev/mem", O_RDWR | O_SYNC) ) < 0)
	{
		fprintf( stderr, "Unable to open /dev/mem: %s\n", strerror(errno));
		gpiomem = 0;
		gpiofd = 0;
		return -1;
    }

	//map a page of memory to gpio at offset 0x20200000 which is where GPIO goodnessstarts
	gpiomem = (uint32_t *)mmap(0, pagesize, PROT_READ|PROT_WRITE, MAP_SHARED, gpiofd, GPIO_BASE );

	if ((int32_t)gpiomem < 0)
	{
		printf("Mmap (GPIO) failed: %s\n", strerror(errno));
		gpiomem = 0;
		gpiofd = 0;
		close( gpiofd );
		return -2;
	}

	return 0;
}

void GPIODirection( int iono, int out )
{
	#define OUT_GPIO(g)   *(gpiomem + ((g)/10)) |=  (1<<(((g)%10)*3))
	#define INP_GPIO(g)   *(gpiomem + ((g)/10)) &= ~(7<<(((g)%10)*3))
	if( out )
		OUT_GPIO( iono );
	else
		INP_GPIO( iono );
}

 
 
#define GPIO_READ(g)  *(gpiomem  + 13) & (1<<(g))

int  GPIOGet( int iono )
{
	return GPIO_READ(iono);
}

#define GPIO_SET  *(gpiomem + 7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR  *(gpiomem + 10) // clears bits which are 1 ignores bits which are 0

void GPIOSet( int iono, int turnon )
{
	if( turnon )
		GPIO_SET = 1<<iono;
	else
		GPIO_CLR = 1<<iono;
}


