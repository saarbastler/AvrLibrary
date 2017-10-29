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

namespace SABA
{
  namespace Timing
  {
    // increment this system ticker periodically by an timer interrupt
    // define it as volatile uint16_t SABA::Timing::ticker asm("Timingticker");
    extern volatile uint16_t ticker asm("Timingticker");

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
  }

}
#endif
