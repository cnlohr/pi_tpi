#include <avr/io.h>

int main()
{
	DDRB = 0xFF;
	while(1) { PORTB = 0xFF; PORTB = 0x00; }
}
