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
  /**
   @namespace SABA::Timer8
   @brief 8 Bit Hardware Timer support class templates
  */
  namespace Timer8
  {
    /// Clock selection
    enum ClockSelect
    {
      NoClockSource= 0,   /**< No clock source (Timer/Counter stopped). */
      By1= 1,             /**< clkIO by 1 (No prescaling) */
      By8=2,              /**< clkIO by 8 (From prescaler) */
      By64=3,             /**< clkIO by 64 (From prescaler) */
      By256=4,            /**< clkIO by 256 (From prescaler) */
      By1024=5,           /**< clkIO by 1024 (From prescaler) */
      TxFalling= 6,       /**< External clock source on T0/1 pin. Clock on falling edge. */
      TxRising= 7         /**< External clock source on T0/1 pin. Clock on rising edge. */
    };

    /// The 8 Bit Timer WaveformGenerationMode enum
    enum WaveformGenerationMode
    {
      Normal= 0,                    /**< Normal */
      PWM_PHASE_CORR= 1,            /**< PWM, Phase Correct */
      CTC_OCRA= 2,                  /**< CTC OCRA */
      FAST_PWM = 3,                 /**< Fast PWM */
      PWM_PHASE_CORR_OCRA= 5,       /**< PWM, Phase and Frequency Correct OCRA */
      FAST_PWM_OCRA = 7             /**< Fast PWM OCRA*/
    };

    //ATMega8/16/32
    template<SFRA _TCCR,SFRA _TCNT,SFRA _TIMSK>
    class TC
    {
      public:

      TC& clockSelect(ClockSelect c)
      {
        SFRBITS<_TCCR,BIT(CS00)|BIT(CS01)|BIT(CS02),CS00> c012;
        c012 = c;

        return *this;
      }

      TC& enableOverflowInterrupt(bool enable)
      {
        SFRBIT<_TIMSK,TOIE0> bit;
        bit= enable;

        return *this;
      }

      // using only one Byte
      SFREG16<_TCNT> tcnt;
    };

   /** A template class to access a 8 Bit AVR Timer with PWM
  
    @tparam _TCCRA the TCCRA address
    @tparam _TCCRB the TCCRB address
    @tparam _TCNT the TCCRC address
    @tparam _OCRA the OCRA address
    @tparam _OCRB the OCRB address
    @tparam _ICR the ICR address
    @tparam _TIMSK the TIMSK address

    Usage:
    ~~~{.c}
    SABA::Timer::Timer0 timer0; // use the typedef instead of the template will save space

    timer0
      .waveformGenerationMode(SABA::Timer::Normal)
      .clockSelect(SABA::Timer::By8);
    ~~~ 
    */
    template<SFRA _TCCRA,SFRA _TCCRB,SFRA _TCNT,SFRA _OCRA,SFRA _OCRB,SFRA _TIMSK,SFRA _TIFR>
    class TCPWM
    {
    public:
      /** set Timer waveform generation mode
       * @param c the WaveformGenerationMode enum constant, see also the WGMx bits in TCCRxx register description
       * @return the this object for creating fluent calls
      */
      TCPWM& waveformGenerationMode(WaveformGenerationMode m)
      {
        SFRBITS<_TCCRA,BIT(WGM00)|BIT(WGM01),WGM00> w01;
        SFRBIT<_TCCRB,WGM02> w2;

        w01 = m & 3;
        w2 = (m & 4) != 0;

        return *this;
      }

      /** set Timer clock selection
       * @param c the ClockSelect enum constant, see also the CSxx bits in TCCRxx register description
       * @return the this object for creating fluent calls
      */
      TCPWM& clockSelect(ClockSelect c)
      {
        SFRBITS<_TCCRB,BIT(CS00)|BIT(CS01)|BIT(CS02),CS00> c012;
        c012 = c;

        return *this;
      }

      /** enable or disable the Timer overflow interrupt
       * @param enable, if true, enable the overflow interrupt, if false disable the overflow interrupt
       * @return the this object for creating fluent calls
      */
      TCPWM& enableOverflowInterrupt(bool enable)
      {
        SFRBIT<_TIMSK,TOIE0> bit;
        bit= enable;

        return *this;
      }

      /** check, if the the Timer overflow interrupt is enabled
       * @return true, if the interrupt is enabled.
      */
      bool isOverflowInterruptEnabled()
      {
        SFRBIT<_TIMSK,TOIE1> bit;

        return bit();
      }

      /** enable or disable the Timer Comparator A match interrupt
       * @param enable, if true, enable the Comparator A match interrupt, if false disable the Comparator A match interrupt
       * @return the this object for creating fluent calls
      */
      TCPWM& enableOutputCompAMatchInterrupt(bool enable)
      {
        SFRBIT<_TIMSK,OCIE0A> bit;
        bit= enable;

        return *this;
      }

      /** enable or disable the Timer Comparator B match interrupt
       * @param enable, if true, enable the Comparator B match interrupt, if false disable the Comparator B match interrupt
       * @return the this object for creating fluent calls
      */
      TCPWM& enableOutputCompBMatchInterrupt(bool enable)
      {
        SFRBIT<_TIMSK,OCIE0B> bit;
        bit= enable;
      
        return *this;
      }

      /** the C++ operator () returns the TCNT register value
       * @return the ADC value
       */
      uint8_t operator() ()
      {
        SFREG <_TCNT> tcnt;

        return tcnt();
      }

      /** the C++ operator = allows write access to the TCNT register
       */
      TCPWM& operator= (uint8_t value)
      {
        SFREG <_TCNT> tcnt;

        tcnt= value;

        return *this;
      }

      // using only one Byte 
      union
      {
        SFREG16<_TCNT> tcnt; //! direct access to the TCNT register
        SFREG16<_OCRA> ocra; //! direct access to the OCRA register
        SFREG16<_OCRB> ocrb; //! direct access to the OCRB register
      };
    };
  }

  namespace Timer16
  {
    /// Clock selection
    enum ClockSelect
    {
      NoClockSource= 0,   /**< No clock source (Timer/Counter stopped). */
      By1= 1,             /**< clkIO by 1 (No prescaling) */
      By8=2,              /**< clkIO by 8 (From prescaler) */
      By64=3,             /**< clkIO by 64 (From prescaler) */
      By256=4,            /**< clkIO by 256 (From prescaler) */
      By1024=5,           /**< clkIO by 1024 (From prescaler) */
      TxFalling= 6,       /**< External clock source on T0/1 pin. Clock on falling edge. */
      TxRising= 7         /**< External clock source on T0/1 pin. Clock on rising edge. */
    };

    /// The 16 Bit Timer WaveformGenerationMode enum
    enum WaveformGenerationMode
    {
      Normal= 0,                    /**< Normal */
      PWM_PC8= 1,                   /**< PWM, Phase Correct, 8-bit */
      PWM_PC9= 2,                   /**< PWM, Phase Correct, 9-bit */
      PWM_PC10= 3,                  /**< PWM, Phase Correct, 10-bit */
      CTC_OCR1A= 4,                 /**< CTC OCR1A */
      FAST_PWM8= 5,                 /**< Fast PWM, 8-bit */
      FAST_PWM9= 6,                 /**< Fast PWM, 9-bit */
      FAST_PWM10= 7,                /**< Fast PWM, 10-bit */
      PWM_PHASE_FREQ_CORR_ICR1= 8,  /**< PWM, Phase and Frequency Correct ICR1 */
      PWM_PHASE_FREQ_CORR_OCR1A= 9, /**< PWM, Phase and Frequency Correct OCR1A */
      PWM_PHASE_CORR_ICR1= 10,      /**< PWM, Phase and Frequency Correct ICR1 */
      PWM_PHASE_CORR_OCR1A= 11,     /**< PWM, Phase and Frequency Correct OCR1A */
      CTC_ICR1= 12,                 /**< CTC ICR1 */
      FAST_PWM_ICR1= 14,            /**< Fast PWM ICR1 */
      FAST_PWM_OCR1A=15             /**< Fast PWM OCR1A */
    };

    /** A template class to access a 16 Bit AVR Timer
  
      @tparam _TCCRA the TCCRA address
      @tparam _TCCRB the TCCRB address
      @tparam _TCNT the TCCRC address
      @tparam _OCRA the OCRA address
      @tparam _OCRB the OCRB address
      @tparam _ICR the ICR address
      @tparam _TIMSK the TIMSK address

      Usage:
      ~~~{.c}
      SABA::Timer::Timer1 timer1; // use the typedef instead of the template will save space

      timer1
        .waveformGenerationMode(SABA::Timer::Normal)
        .clockSelect(SABA::Timer::By8);
      ~~~ 
      */
    template<SFRA _TCCRA,SFRA _TCCRB,SFRA _TCNT,SFRA _OCRA,SFRA _OCRB,SFRA _ICR,SFRA _TIMSK,uint8_t _TICIE1>
    class TCPWM
    {
    public:
      /** set Timer waveform generation mode
       * @param c the WaveformGenerationMode enum constant, see also the WGMx bits in TCCRxx register description
       * @return the this object for creating fluent calls
      */
      TCPWM& waveformGenerationMode(WaveformGenerationMode m)
      {
        SFRBITS<_TCCRA,BIT(WGM10)|BIT(WGM11),WGM10> w01;
        SFRBITS<_TCCRB,BIT(WGM12)|BIT(WGM13),WGM12> w23;

        w01 = m & 3;
        w23 = m >> 2;

        return *this;
      }

      /** set Timer clock selection
       * @param c the ClockSelect enum constant, see also the CSxx bits in TCCRxx register description
       * @return the this object for creating fluent calls
      */
      TCPWM& clockSelect(ClockSelect c)
      {
        SFRBITS<_TCCRB,BIT(CS10)|BIT(CS11)|BIT(CS12),CS10> c012;
        c012 = c;

        return *this;
      }

      /** enable or disable the Timer overflow interrupt
       * @param enable, if true, enable the overflow interrupt, if false disable the overflow interrupt
       * @return the this object for creating fluent calls
      */
      TCPWM& enableOverflowInterrupt(bool enable)
      {
        SFRBIT<_TIMSK,TOIE1> bit;
        bit= enable;

        return *this;
      }

      /** check, if the the Timer overflow interrupt is enabled
       * @return true, if the interrupt is enabled.
      */
      bool isOverflowInterruptEnabled()
      {
        SFRBIT<_TIMSK,TOIE1> bit;

        return bit();
      }

      /** enable or disable the Timer Comparator A match interrupt
       * @param enable, if true, enable the Comparator A match interrupt, if false disable the Comparator A match interrupt
       * @return the this object for creating fluent calls
      */
      TCPWM& enableOutputCompAMatchInterrupt(bool enable)
      {
        SFRBIT<_TIMSK,OCIE1A> bit;
        bit= enable;

        return *this;
      }

      /** enable or disable the Timer Comparator B match interrupt
       * @param enable, if true, enable the Comparator B match interrupt, if false disable the Comparator B match interrupt
       * @return the this object for creating fluent calls
      */
      TCPWM& enableOutputCompBMatchInterrupt(bool enable)
      {
        SFRBIT<_TIMSK,OCIE1B> bit;
        bit= enable;
      
        return *this;
      }

      /** enable or disable the Timer Input Capture interrupt
       * @param enable, if true, enable the Input Capture interrupt, if false disable the Input Capture interrupt
       * @return the this object for creating fluent calls
      */
      TCPWM& enableInputCaptureInterrupt(bool enable)
      {
        SFRBIT<_TIMSK,_TICIE1> bit;
        bit= enable;

        return *this;
      }

      /** the C++ operator () returns the TCNT register value
       * @return the ADC value
       */
      uint16_t operator() ()
      {
        SFREG16 <_TCNT> tcnt;

        return tcnt();
      }

      /** the C++ operator = allows write access to the TCNT register
       */
      TCPWM& operator= (uint16_t value)
      {
        SFREG16 <_TCNT> tcnt;

        tcnt= value;

        return *this;
      }

      // using only one Byte 
      union
      {
        SFREG16<_TCNT> tcnt; //! direct access to the TCNT register
        SFREG16<_OCRA> ocra; //! direct access to the OCRA register
        SFREG16<_OCRB> ocrb; //! direct access to the OCRB register
        SFREG16<_ICR> icr; //! direct access to the ICR register
      };
    };

  }

#if defined(TIMSK)
// AtMega 8
typedef Timer8::TC<(SFRA)&TCCR0,(SFRA)&TCNT0,(SFRA)&TIMSK> Timer0;
typedef Timer16:TCPWM<(SFRA)&TCCR1A,(SFRA)&TCCR1B,(SFRA)&TCNT1,(SFRA)&OCR1A,(SFRA)&OCR1B,(SFRA)&ICR1,(SFRA)&TIMSK,TIEC1> Timer1;
#endif

#ifdef TIMSK0
typedef Timer8::TCPWM<(SFRA)&TCCR0A,(SFRA)&TCCR0B,(SFRA)&TCNT0,(SFRA)&OCR0A,(SFRA)&OCR0B,(SFRA)&TIMSK0,(SFRA)&TIFR0> Timer0;
#endif

#ifdef TIMSK1
// AtMega xx8
typedef Timer16::TCPWM<(SFRA)&TCCR1A,(SFRA)&TCCR1B,(SFRA)&TCNT1,(SFRA)&OCR1A,(SFRA)&OCR1B,(SFRA)&ICR1,(SFRA)&TIMSK1,ICIE1> Timer1;
#endif

#ifdef TIMSK3
// AtMega 2560
typedef Timer16::TCPWM<(SFRA)&TCCR3A,(SFRA)&TCCR3B,(SFRA)&TCNT3,(SFRA)&OCR3A,(SFRA)&OCR3B,(SFRA)&ICR3,(SFRA)&TIMSK3,ICIE3> Timer3;
#endif

}

 #endif // SABA_TIMER16_H_
