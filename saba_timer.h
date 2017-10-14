/*
 * saba_timer16.h
 *
 * Saarbastler AVR C++ 11 Library
 *
 * Created: 12.10.2017
 * Author: Joerg
 *
 * 16 Bit Timer
 */ 

#ifndef SABA_TIMER16_H_
#define SABA_TIMER16_H_

#include <avr/io.h>
#include <saba_avr.h>

namespace SABA
{
  namespace Timer
  {
    enum WaveformGenerationMode
    {
      Normal= 0, PWM_PC8= 1, PWM_PC9= 2, PWM_PC10= 3, CTC_OCR1A= 4, FAST_PWM8= 5, FAST_PWM9= 6,
      FAST_PWM10= 7, PWM_PHASE_FREQ_CORR_ICR1= 8,  PWM_PHASE_FREQ_CORR_OCR1A= 9, PWM_PHASE_CORR_ICR1= 10,
      PWM_PHASE_CORR_OCR1A= 11, CTC_ICR1= 12, FAST_PWM_ICR1= 14, FAST_PWM_OCR1A=15
    };

    enum ClockSelect
    {
      NoClockSource= 0, By1= 1, By8=2, By64=3, By256=4, By1024=5, T1Falling= 6, T1Rising= 7
    };

    template<SFRA _TCCRA,SFRA _TCCRB,SFRA _TCNT,SFRA _OCRA,SFRA _OCRB,SFRA _ICR,SFRA _TIMSK>
    class Timer16
    {
    public:
      Timer16& waveformGenerationMode(WaveformGenerationMode m)
      {
        SFRBITS<_TCCRA,_BV(WGM10)|_BV(WGM11)> w01;
        SFRBITS<_TCCRB,_BV(WGM12)|_BV(WGM13)> w23;

        w01 = m & 3;
        w23 = (m & 0x0c) << WGM12;

        return *this;
      }

      Timer16& clockSelect(ClockSelect c)
      {
        SFRBITS<_TCCRB,_BV(CS10)|_BV(CS11)|_BV(CS12)> c012;
        c012 = c;

        return *this;
      }

      Timer16& enableOverflowInterrupt(bool enable)
      {
        SFRBIT<(SFRA)&TIMSK,TOIE1> bit;
        bit= enable;

        return *this;
      }

      Timer16& enableOutputCompAMatchInterrupt(bool enable)
      {
        SFRBIT<(SFRA)&TIMSK,OCIE1A> bit;
        bit= enable;

        return *this;
      }

      Timer16& enableOutputCompBMatchInterrupt(bool enable)
      {
        SFRBIT<(SFRA)&TIMSK,OCIE1B> bit;
        bit= enable;
      
        return *this;
      }

      Timer16& enableInputCaptureInterrupt(bool enable)
      {
        SFRBIT<(SFRA)&TIMSK,TICIE1> bit;
        bit= enable;

        return *this;
      }

      // using only one Byte 
      union
      {
        SFREG16<_TCNT> tcnt;
        SFREG16<_OCRA> ocra;
        SFREG16<_OCRB> ocrb;
        SFREG16<_ICR> icr;
      };
    };

    template<SFRA _TCCR,SFRA _TCNT,SFRA _TIMSK>
    class Timer8
    {
      public:

      Timer8& clockSelect(ClockSelect c)
      {
        SFRBITS<_TCCR,_BV(CS00)|_BV(CS01)|_BV(CS02)> c012;
        c012 = c;

        return *this;
      }

      Timer8& enableOverflowInterrupt(bool enable)
      {
        SFRBIT<(SFRA)&TIMSK,TOIE0> bit;
        bit= enable;

        return *this;
      }

      // using only one Byte
      SFREG16<_TCNT> tcnt;
    };
//#if defined(TCCR1A) && defined(TCCR1B) && !defined(TCCR1C)
// AtMega 8
typedef Timer16<(SFRA)&TCCR1A,(SFRA)&TCCR1B,(SFRA)&TCNT1,(SFRA)&OCR1A,(SFRA)&OCR1B,(SFRA)&ICR1,(SFRA)&TIMSK> Timer1;
typedef Timer8<(SFRA)&TCCR0,(SFRA)&TCNT0,(SFRA)&TIMSK> Timer0;
//#endif
  }
}

 #endif // SABA_TIMER16_H_
