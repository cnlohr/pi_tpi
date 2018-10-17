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

