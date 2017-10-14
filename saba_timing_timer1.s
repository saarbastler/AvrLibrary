
/*
 * saba_timing_timer1.s
 *
 * Created: 12.10.2017
 * Author: Jörg
 * include this file in your project to increment the Timing Ticker by Timer1 Overflow interrupt
 */ 

#include <avr/interrupt.h>

.extern Timingticker

.global TIMER1_OVF_vect

.func TIMER1_OVF_vect
TIMER1_OVF_vect:
				push 	r16
				in 		r16, _SFR_IO_ADDR(SREG)
				push	r16

				lds		r16, Timingticker
				inc		r16
				sts		Timingticker, r16
        brne  noTick
      
				lds		r16, Timingticker+1
				inc		r16
				sts		Timingticker+1, r16

noTick:
				pop		r16
				out 	_SFR_IO_ADDR(SREG), r16
				pop 	r16
				reti
.endfunc
.end
