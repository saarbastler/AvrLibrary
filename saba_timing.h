/*
 * saba_timing.h
 *
 * Saarbastler AVR C++ 11 Library
 *
 * Created: 12.10.2017
 * Author: Joerg
 *
 * Timing functionality
 */ 

#ifndef SABA_TIMING_H_
#define SABA_TIMING_H_

#define ISR_INCREMENTUINT16(VAR)  \
  asm volatile                    \
  (                               \
  "push r16"        "\n\t"        \
  "in r16,%i1"      "\n\t"        \
  "push r16"        "\n\t"        \
  "lds  r16, %A0"   "\n\t"        \
  "inc  r16"        "\n\t"        \
  "sts  %A0, r16"   "\n\t"        \
  "brne 0f"         "\n\t"        \
  "lds	r16, %A0+1" "\n\t"        \
  "inc	r16"        "\n\t"        \
  "sts	%A0+1, r16" "\n\t"        \
  "0:"              "\n\t"        \
  "pop	r16"        "\n\t"        \
  "out 	%i1, r16"   "\n\t"        \
  "pop 	r16"        "\n\t"        \
  "reti"                          \
  :: "m" (VAR), "i" (&SREG)       \
  )                               \
  
namespace SABA
{
  /**
   @namespace SABA::Timing
   @brief Delay and Timing helper classes
  */
  namespace Timing
  {
    // increment this system ticker periodically by an timer interrupt
    // define it as volatile uint16_t SABA::Timing::ticker;
    // The macro ISR_INCREMENTUINT16 is an ISR increment method
    extern volatile uint16_t ticker;

    // delay a fixed time. TYPE is either uint8_t or uint16_t for max 255/65535 delay time
    // Can be re triggered or re started again.
    template<typename TYPE, TYPE delay>
    class SingleDelayFixed
    {
    public:
      void stop()
      {
        mode= Done;
      }
      
      void start()
      {
        startValue= ticker;
        mode= Running;
      }

      bool isRunning()
      {
        return mode == Running;
      }

      // is true only once, if the delay is done.
      // has to be started again after delay is done.
      bool operator()()
      {
        if(mode != Running)
        {
          return false;
        }
        
        if( mode == Running )
        {
          TYPE diff= ticker - startValue;

          if(diff >= delay)
            mode= Stopped;
        }

        return (bool)mode;
      }

    private:

      TYPE startValue;
      enum Mode
      {
        Running= 0, Stopped= 1, Done= 2
      };

      Mode mode= Done;
    };
    
    // delay a  time. TYPE is either uint8_t or uint16_t for max 255/65535 delay time
    // Can be re triggered or re started again.
    template<typename TYPE>
    class SingleDelay
    {
    public:
      void stop()
      {
        mode= Done;
      }
          
      void start(TYPE delay_)
      {
        delay= delay_;
        startValue= ticker;
        mode= Running;
      }

      bool isRunning()
      {
        return mode == Running;
      }

      // is true only once, if the delay is done.
      // has to be started again after delay is done.
      bool operator()()
      {
        if(mode != Running)
        {
          return false;
        }
            
        if( mode == Running )
        {
          TYPE diff= ticker - startValue;

          if(diff >= delay)
            mode= Stopped;
        }

        return (bool)mode;
      }

      private:

      TYPE startValue;
      TYPE delay;
      
      enum Mode
      {
        Running= 0, Stopped= 1, Done= 2
      };

      Mode mode= Done;

    };
  }

}
#endif
