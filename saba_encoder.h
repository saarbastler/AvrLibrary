/* 
* TurnEncoder.h
*
* Created: 18.11.2016 19:37:39
* Author: Joerg
*/


#ifndef __TURNENCODER_H__
#define __TURNENCODER_H__

#include <stdint.h>

#include <saba_avr.h>
#include <saba_timing.h>

namespace SABA
{
  template<SFRA PIN_PHASE1, uint8_t BIT_PHASE1,SFRA PIN_PHASE2,uint8_t BIT_PHASE2,SFRA PIN_BUTTON,uint8_t BIT_BUTTON,uint8_t DEBOUNCE_DELAY=2>
  class TurnEncoder
  {
  public:
    typedef void (*TURN)(void *env, int8_t counter, int8_t increment );

    typedef void (*CLICK)(void *env, bool down);

	  TurnEncoder( void *env= nullptr ) : env(env)
    {
      SABA::PortPin<PIN_PHASE1,BIT_PHASE1> phase1;
      SABA::PortPin<PIN_PHASE2,BIT_PHASE2> phase2;
      SABA::PortPin<PIN_BUTTON,BIT_BUTTON> button;

      phase1.asInputPullUp();
      phase2.asInputPullUp();
      button.asInput()= true;

      waiting= false;
      counter= 0;

      oldValue= inputs();
    };

    void setEnv(void *env)
    {
      this->env= env;
    }

    void cyclic(TURN turn, CLICK click)
    {
      uint8_t newValue= inputs();
      if( waiting )
      {
        uint8_t diff= Timing::ticker - waitStart;
        if( diff >= DEBOUNCE_DELAY )
        {
          waiting= false;

          if( newValue != oldValue )
          {
            if( (oldValue & BUTTON) != (newValue & BUTTON))
            {
              if( click != nullptr) 
                click( env, (newValue & BUTTON) == 0);
            }
            if( (oldValue & PHASE1) == 0 && (newValue & PHASE1) )
            {
              if( newValue & PHASE2 )
                doTurn(turn, 1);
              else
                doTurn(turn, -1);
            }
            else if( (oldValue & PHASE2) && (newValue & PHASE2) == 0)
            {
              if( newValue & PHASE1 )
                doTurn(turn, 1);
              else
                doTurn(turn, -1);
            }
            oldValue= newValue;
          }
        }
      }
      else if( newValue != oldValue )
      {
        waitStart= Timing::ticker;
        waiting= true;
      }
    }
  protected:

    uint8_t inputs()
    {
      SABA::PortPin<PIN_PHASE1,BIT_PHASE1> phase1;
      SABA::PortPin<PIN_PHASE2,BIT_PHASE2> phase2;
      SABA::PortPin<PIN_BUTTON,BIT_BUTTON> button;

      uint8_t newValue= 0;

      if( phase1()) newValue = PHASE1;
      if( phase2()) newValue |= PHASE2;
      if( button()) newValue |= BUTTON;

      return newValue;
    }

    void doTurn( TURN turnFunc, int8_t increment)
    {
      counter += increment;

      if( turnFunc != nullptr )
        turnFunc(env, counter, increment);
    }    
    
    uint8_t waitStart;
    bool waiting;

    static constexpr uint8_t PHASE1= BIT(0);
    static constexpr uint8_t PHASE2= BIT(1);
    static constexpr uint8_t BUTTON= BIT(2);

    uint8_t oldValue;

    int8_t counter;

    void *env;
  }; //TurnEncoder

}

#endif //__TURNENCODER_H__
