/*
 * saba_usart.h
 *
 * Saarbastler AVR C++ 11 Library
 *
 * Created: 12.10.2017 
 * Author: Joerg
 *
 * Uart templates
 */ 

#include <avr/io.h>
#include <saba_avr.h>

#include "Device.h"

#ifndef SABA_USART_H_
#define SABA_USART_H_

namespace SABA
{
  template <SFRA _UDR,SFRA _UCSRA,SFRA _UCSRB,SFRA _UCSRC,SFRA _UBRRL,SFRA _UBRRH>
  class Usart
  {
  public:

    Usart(uint32_t baudrate)
    {
      uint16_t ubrr= (uint16_t)(.5 + F_CPU / (baudrate * 16))-1;

      SFREG<_UCSRA> ucsra;
      SFREG<_UCSRB> ucsrb;
      SFREG<_UCSRC> ucsrc;
      SFREG<_UBRRL> ubrrl;
      SFREG<_UBRRH> ubrrh;

      ubrrl= (uint8_t)ubrr & (uint8_t)0xff;
      ubrrh= ubrr >> 8;
      ucsra= 0;
      ucsrb= _BV(RXEN)|_BV(TXEN);

// Atmega 8
#ifdef URSEL
      ucsrc= _BV(URSEL) | _BV(UCSZ0) | _BV(UCSZ1);
#else
      ucsrc= _BV(UCSZ0) | _BV(UCSZ1);
#endif
    }

    // test, if transmit was completed
    bool transmitComplete()
    {
      SFRBIT<_UCSRA,TXC> txc;

      return txc();
    }

    // test, if the data register is empty. If true the next char can be written
    bool readyToSend()
    {
      SFRBIT<_UCSRA,UDRE> udre;

      return udre();
    }

    // waits until data register is empty and transmits 
    void putch( uint8_t ch )
    {
      while(! readyToSend())
        ;

      SFREG<_UDR> udr;
      udr= ch;
    }

    // test, if data has been received complete, the data register can be read
    bool receiverComplete()
    {
      SFRBIT<_UCSRA,RXC> rxc;

      return rxc();
    }

    // wait until a receive was completed, then return the received char
    uint8_t getch()
    {
      while(!receiverComplete())
        ;

      SFREG<_UDR> udr;

      return udr();
    }
  };


#ifdef UDR
  typedef Usart<(SFRA)&UDR,(SFRA)&UCSRA,(SFRA)&UCSRB,(SFRA)&UCSRC,(SFRA)&UBRRL,(SFRA)&UBRRH> USART0;
#endif
#ifdef UDR0
  typedef Usart<(SFRA)&UDR0,(SFRA)&UCSRA0,(SFRA)&UCSRB0,(SFRA)&UCSRC0,(SFRA)&UBRRL0,(SFRA)&UBRRH0> USART0;
#endif
}

#endif // SABA_USART_H_
