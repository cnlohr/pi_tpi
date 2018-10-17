/*

	Example Assembly Code file for including in project.

*/

//Output to SK6812.

#define WS_A_PIN 0
#define WS_B_PIN 2

//void ws_outA( unsigned char v )

.global ws_outA
ws_outA:
	push r22
	push r18
	//Input 'v' is in r24.
	ldi	r22, 0x80	// r22 = 0x80 (mask)
.loop_wsA:
	mov r18, r24
	and r18, r22
	sbi	0x02, WS_A_PIN
	brne .ws_oneA
	cbi	0x02, WS_A_PIN
	rjmp .ws_endA
.ws_oneA:
	sbi	0x02, WS_A_PIN
	nop;
.ws_endA:
	cbi	0x02, WS_A_PIN
	lsr	r22
	brne .loop_wsA
	pop r18
	pop r22
	ret

.global ws_outB
ws_outB:
	//Input 'v' is in r24.
	push r22
	push r18
	ldi	r22, 0x80	// r22 = 0x80 (mask)
.loop_wsB:
	mov r18, r24
	and r18, r22
	sbi	0x02, WS_B_PIN
	brne .ws_oneB
	cbi	0x02, WS_B_PIN
	rjmp .ws_endB
.ws_oneB:
	sbi	0x02, WS_B_PIN
	nop;
.ws_endB:
	cbi	0x02, WS_B_PIN
	lsr	r22
	brne .loop_wsB
	pop r18
	pop r22
	ret



//unsigned char hue( unsigned char x )
.global hue
hue:
//__attribute__ ((section (".user"))) unsigned char hue( unsigned char x )
//{
//	if( x < 42 )
			push r18
			push r19
        	cpi	r24, 0x2A	; 42
        	brcc	.hue_else128     	; 0x30a <hue+0x1a>
//	{
//		unsigned short ret = x << 2;
        	ldi	r25, 0x00	; 0
			//Now r24:r25 make a pair.
			mov r18, r24
			mov r19, r25

        	add	r24, r24
        	adc	r25, r25
        	add	r24, r24
        	adc	r25, r25
//		ret += x << 1;
        	add	r18, r18
        	adc	r19, r19
//		if( ret > 255 ) ret = 255;
//		return ret;
        	add	r24, r18
        	rjmp .huequit
//	}
//	else if( x < 128 )
.hue_else128:
        	sbrs	r24, 7
        	rjmp	.hue_return255     	; 0x328 <hue+0x38>
//	{
//		return 255;
//	}
//	else if( x < 170 )
        	cpi	r24, 0xAA	; 170
        	brcc	.hue_return0    	; 0x32c <hue+0x3c>
//	{
//		unsigned short ret = x - 128;
        	mov	r18, r24
        	ldi	r19, 0x00	; 0
        	subi	r18, 0x80	; 128
			eor r24, r24
        	sbc	r19, r24
//		return 255 - ((ret<<2) +(ret<<1));
        	mov	r24, r18
        	add	r24, r24
        	add	r18, r18
        	add	r18, r18
        	add	r24, r18
        	com	r24
        	rjmp .huequit
//		if( ret > 255 ) ret = 255;
//		return ret;
//	}
//	else if( x < 128 )
//	{
//		return 255;
.hue_return255:
        	ldi	r24, 0xFF	; 255
        	rjmp .huequit
//		unsigned short ret = x - 128;
//		return 255 - ((ret<<2) +(ret<<1));
//	}
//	else
//	{
//		return 0;
.hue_return0:
        	ldi	r24, 0x00	; 0
//	}
//}

.huequit:
			pop r19
			pop r18
        	ret





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

