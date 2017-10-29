/*
 * saba_debounce.h
 *
 * Created: 29.10.2017 17:49:15
 *  Author: Joerg
 */ 


#ifndef SABA_DEBOUNCE_H_
#define SABA_DEBOUNCE_H_

template<SABA::SFRA PIN_ADDR, uint8_t BIT_POS,uint8_t delay,bool invert=false>
class DebouncedInput
{
  public:

  DebouncedInput()
  {
    SABA::PortPin<PIN_ADDR,BIT_POS> input;

    input.asInputPullUp();
    lastInput= input();
  }

  bool hasChanged()
  {
    SABA::PortPin<PIN_ADDR,BIT_POS> input;

    if( lastInput != input())
    if( !debounceDelay.isRunning() )
    debounceDelay.start();

    if( debounceDelay() )
    if( lastInput != input() )
    {
      lastInput= input();
      return true;
    }

    return false;
  }

  bool operator() ()
  {
    return invert ? !lastInput : lastInput;
  }
  protected:

  bool lastInput;
  SABA::Timing::SingleDelayFixed<uint8_t,delay> debounceDelay;
};


#endif /* SABA_DEBOUNCE_H_ */