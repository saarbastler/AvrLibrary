/**
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

  /// Address of Special function Register
  typedef volatile uint16_t SFRA;

  // \brief  8 Bit Special Function Register template class
  /** 
  A template class to access AVR special function registers. The C++ compiler will compile the methods to 
  single assembler instructions.
  @tparam SFR_ADDRESS the SFR address

  Usage:
  ~~~{.c}
  SABA::SFREG<(SFRA)&TCNT0> tcnt0;

  uint8_t value= tcnt0(); // read the TCNT0 register
  tcnt0= 0x12; // write the TCNT0 register
  ~~~ 
  */
  template<SFRA SFR_ADDRESS>
  class SFREG
  {
  public:
    
    uint8_t operator () () //! the () operator read the SFR value
    {
      return *(volatile uint8_t *)SFR_ADDRESS;
    }
    
    void operator =(uint8_t value) //! the = operator write the SFR
    {
      (*(volatile uint8_t *)SFR_ADDRESS)= value;
    }
    
    void operator |=(uint8_t value) //! or a value to the SFR
    {
      (*(volatile uint8_t *)SFR_ADDRESS) |= value;
    }

    void operator &=(uint8_t value) //! and a value to the SFR
    {
      (*(volatile uint8_t *)SFR_ADDRESS) &= value;
    }
    
    void operator ^=(uint8_t value) //! xor a value to the SFR
    {
      (*(volatile uint8_t *)SFR_ADDRESS) ^= value;
    }
  };

  // \brief 16 Bit Special Function Register template class
  /** 
  A template class to access AVR 16 Bit special function registers. 
  @tparam SFR_ADDRESS the SFR address

  Usage:
  ~~~{.c}
  SABA::SFREG<(SFRA)&OCR1A> ocr1a;

  uint16_t value= ocr1a(); // read the OCR1A register
  ocr1a= 0x1234; // write the OCR1A register
  ~~~ 
  */
  template<SFRA SFR_ADDRESS>
  class SFREG16
  {
  public:
    
    uint16_t operator () () //! read the SFR
    {
      return *(volatile uint16_t *)SFR_ADDRESS;
    }
    
    void operator =(uint16_t value) //! write the SFR
    {
      (*(volatile uint16_t *)SFR_ADDRESS)= value;
    }
  };

  // \brief Single Bit Special Function Register template class
  /** 
  A template class to access a single bit in an AVR special function register. 
  @tparam SFR_ADDRESS the SFR address
  @tparam BIT_POS the bit position, dont use the _BV macro it is done inside this class
    
  Usage:
  ~~~{.c}
  SABA::SFREG<(SFRA)&TIMSK1,OCIE1A> ocie1a;

  bool value= ocie1a(); // read the OCIE1A bit in TIMSK1 register
  ocie1a= true; // write the OCIE1A bit in TIMSK1 register
  ~~~ 
  */
  template<SFRA SFR_ADDRESS, uint8_t BIT_POS>
  class SFRBIT
  {
    public:
    
    bool operator () () //! read the bit
    {
      SFREG<SFR_ADDRESS> sfr;

      return sfr() & _BV(BIT_POS);
    }
    
    void operator =(bool value) //! write the bit
    {
      SFREG<SFR_ADDRESS> sfr;

      if( value)
        sfr |= _BV(BIT_POS);
      else
        sfr &= (uint8_t)~_BV(BIT_POS);
    }
  };

  // \brief Multiple Bit Special Function Register template class
  /** A template class to access multiple bits in an AVR special function register.
  @tparam SFR_ADDRESS the SFR address
  @tparam BIT_MASK the combination of the desired bit mask
  @tparam BIT_SHIFT the lowest bit position 

  Usage:
  ~~~{.c}
  SABA::SFRBITS<(SFRA)&TCCRB,_BV(WGM12)|_BV(WGM13),WGM12> w23;

  uint8_t value= w23(); // read the WGM12/13 bis as a 2 bit value
  ocie1a= true; // write the WGM12/13 bis as a 2 bit value
  ~~~ 
  */
  template<SFRA SFR_ADDRESS, uint8_t BIT_MASK, uint8_t BIT_SHIFT>
  class SFRBITS
  {
    public:
    
    uint8_t operator () () //! read the bit
    {
      SFREG<SFR_ADDRESS> sfr;

      return (sfr() & BIT_MASK) >> BIT_SHIFT;
    }
    
    void operator =(uint8_t value) //! write the bits
    {
      SFREG<SFR_ADDRESS> sfr;

      sfr= (sfr() & ~BIT_MASK) | ((value & BIT_MASK) << BIT_SHIFT);
    }
    
    void operator |=(uint8_t value) //! or the bits with a value
    {
      SFREG<SFR_ADDRESS> sfr;

      sfr |= ((value << BIT_SHIFT) & BIT_MASK);
    }

    void operator &=(uint8_t value)  //! and the bits with a value
    {
      SFREG<SFR_ADDRESS> sfr;

      sfr &= ((value << BIT_SHIFT) & BIT_MASK);
    }

    void operator ^=(uint8_t value)  //! xor the bits with a value
    {
      SFREG<SFR_ADDRESS> sfr;

      sfr ^= ((value << BIT_SHIFT) & BIT_MASK);
    }

  };

  // \brief The combination of three SFR PINx, DDRx and PORTx
  /** The union will reduce the size to one byte instead of three bytes. Even an empty C++ class will consume one byte.
  Do not use this union to access single port pins, use the PortPin class instead.

  @tparam PIN_ADDR the PINx address, DDRx and PORTx is calculated

  Usage:
  ~~~{.c}
  SABA::Port8<(SFRA)PINC> pc;

  pc.ddr= 0xff; // all outputs
  pc.port= 0x00; // all zero
  ~~~
  */
  template<SFRA PIN_ADDR>
  union Port8
  {
    //! The PINx SFR
    SFREG<PIN_ADDR> pin;

    //! The DDRx SFR
    SFREG<PIN_ADDR+1> ddr; 

    //! The PORTx SFR
    SFREG<PIN_ADDR+2> port; 
  };

  // \brief Multiple Bit Special Function Register template class
  /** A template class to access multiple bits in an AVR special function register.
  @tparam SFR_ADDRESS the SFR address
  @tparam BIT_POS The desired Port bit

  Usage:
  ~~~{.c}
  SABA::PortPin<(SFRA)PIND,3> led;
  SABA::PortPin<(SFRA)PIND,4> button;

  led.asOutput() = false; // set as ouput and set to 0
  led= true; // set to 1;
  button.asInput()= true; // set as input, turn on pull up
  bool value= button(); // read pin
  ~~~ 
  */
  template<SFRA PIN_ADDR, uint8_t BIT_POS>
  class PortPin
  {
    public:

    bool operator() () //! read the pin value (input)
    {
      Port8<PIN_ADDR> port8;

      return port8.pin() & _BV(BIT_POS);
    }

    void operator= (bool value) //! write the output
    {
      Port8<PIN_ADDR> port8;

      if(value)
        port8.port |= _BV(BIT_POS);
      else
        port8.port &= ~_BV(BIT_POS);
    }

    PortPin& toggle() //! toggle the output. Depending on the SUPPORTS_PIN_TOGGLE (see saba_controller.h) macro, the PIN is used or PORT is toggled
    {
      Port8<PIN_ADDR> port8;

#ifdef SUPPORTS_PIN_TOGGLE
      port8.pin = _BV(BIT_POS);
#else
      port8.port ^= _BV(BIT_POS);
#endif

      return *this;
    }

    PortPin& asOutput() //! the Pin is an output
    {
      Port8<PIN_ADDR> port8;

      port8.ddr |= _BV(BIT_POS);

      return *this;
    }

    PortPin& asInput() //! the Pin is an input
    {
      Port8<PIN_ADDR> port8;

      port8.ddr &= ~_BV(BIT_POS);

      return *this;
    }

    PortPin& asInputPullUp() //! the Pin is an input with pull up
    {
      Port8<PIN_ADDR> port8;

      port8.ddr &= ~_BV(BIT_POS);
      *this = true;

      return *this;
    }
  };
  
}

#endif /* SABA_AVR_H_ */