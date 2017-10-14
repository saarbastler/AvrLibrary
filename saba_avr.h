/*
 * saba_avr.h
 *
 * Saarbastler AVR C++ 11 Library
 *
 * Created: 24.06.2017 
 * Author: Joerg
 *
 * SFR, Port Templates
 */ 


#ifndef SABA_AVR_H_
#define SABA_AVR_H_

#include <saba_controller.h>

namespace SABA
{

  // Address of Special function Register
  typedef volatile uint16_t SFRA;

  // Special Function Register 8 Bit or 16 Bit
  /*template<typename TYPE,SFRA SFR_ADDRESS>
  class _SFREG
  {
    public:
    // read the SFR
    TYPE operator () ()
    {
      return *(volatile TYPE *)SFR_ADDRESS;
    }

    // write the SFR
    void operator =(TYPE value)
    {
      (*(volatile TYPE *)SFR_ADDRESS)= value;
    }

  };*/

  // Special Function Register
  template<SFRA SFR_ADDRESS>
  class SFREG //: public _SFREG<uint8_t,SFR_ADDRESS>
  {
  public:
    // read the SFR
    uint8_t operator () ()
    {
      return *(volatile uint8_t *)SFR_ADDRESS;
    }

    // write the SFR
    void operator =(uint8_t value)
    {
      (*(volatile uint8_t *)SFR_ADDRESS)= value;
    }

    // or SFR
    void operator |=(uint8_t value)
    {
      (*(volatile uint8_t *)SFR_ADDRESS) |= value;
    }

    // and SFR
    void operator &=(uint8_t value)
    {
      (*(volatile uint8_t *)SFR_ADDRESS) &= value;
    }

    // exor SFR
    void operator ^=(uint8_t value)
    {
      (*(volatile uint8_t *)SFR_ADDRESS) ^= value;
    }
  };

  // 16 Bit Special Function Register
  template<SFRA SFR_ADDRESS>
  class SFREG16 //: public _SFREG<uint16_t,SFR_ADDRESS>
  {
  public:
    // read the SFR
    uint16_t operator () ()
    {
      return *(volatile uint16_t *)SFR_ADDRESS;
    }

    // write the SFR
    void operator =(uint16_t value)
    {
      (*(volatile uint16_t *)SFR_ADDRESS)= value;
    }
  };

  template<SFRA SFR_ADDRESS, uint8_t BIT_POS>
  class SFRBIT
  {
    public:
    // read the bit
    bool operator () ()
    {
      SFREG<SFR_ADDRESS> sfr;

      return sfr() & _BV(BIT_POS);
    }

    // write the bit
    void operator =(bool value)
    {
      SFREG<SFR_ADDRESS> sfr;

      if( value)
        sfr |= _BV(BIT_POS);
      else
        sfr &= (uint8_t)~_BV(BIT_POS);
    }
  };

  template<SFRA SFR_ADDRESS, uint8_t BIT_MASK>
  class SFRBITS
  {
    public:
    // read the bit
    uint8_t operator () ()
    {
      SFREG<SFR_ADDRESS> sfr;

      return sfr() & BIT_MASK;
    }

    // write the bits
    void operator =(uint8_t value)
    {
      SFREG<SFR_ADDRESS> sfr;

      sfr= (sfr() & ~BIT_MASK) | (value & BIT_MASK);
    }

    // or SFR
    void operator |=(uint8_t value)
    {
      SFREG<SFR_ADDRESS> sfr;

      sfr |= (value & BIT_MASK);
    }

    // and SFR
    void operator &=(uint8_t value)
    {
      SFREG<SFR_ADDRESS> sfr;

      sfr &= (value & BIT_MASK);
    }

    // exor SFR
    void operator ^=(uint8_t value)
    {
      SFREG<SFR_ADDRESS> sfr;

      sfr ^= (value & BIT_MASK);
    }

  };

  // AVR Port definition, the template parameter is the PIN address.
  template<SFRA PIN_ADDR>
  union Port8
  {
    //public:

    SFREG<PIN_ADDR> pin;

    SFREG<PIN_ADDR+1> ddr;

    SFREG<PIN_ADDR+2> port;
  };

  // as single AVR port pin definition
  template<SFRA PIN_ADDR, uint8_t BIT_POS>
  class PortPin
  {
    public:

    bool operator() ()
    {
      Port8<PIN_ADDR> port8;

      return port8.pin & _BV(BIT_POS);
    }

    void operator= (bool value)
    {
      Port8<PIN_ADDR> port8;

      if(value)
        port8.port |= _BV(BIT_POS);
      else
        port8.port &= ~_BV(BIT_POS);
    }

    void toggle()
    {
      Port8<PIN_ADDR> port8;

#ifdef SUPPORTS_PIN_TOGGLE
      port8.pin = _BV(BIT_POS);
#else
      port8.port ^= _BV(BIT_POS);
#endif
    }

    void asOutput()
    {
      Port8<PIN_ADDR> port8;

      port8.ddr |= _BV(BIT_POS);
    }

    void asInput()
    {
      Port8<PIN_ADDR> port8;

      port8.ddr &= ~_BV(BIT_POS);
    }

    void asInputPullUp()
    {
      Port8<PIN_ADDR> port8;

      port8.ddr &= ~_BV(BIT_POS);
      *this = true;
    }
  };
  
}

#endif /* SABA_AVR_H_ */