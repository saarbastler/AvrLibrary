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

  // \brief Usart controlling class
  /** 
  The Usart template class is used to control an AVR Usart. There are predefined typedefs USARTx in saba_controller.h
  @tparam _UDR the UDR SFR address
  @tparam _UCSRA the UCSRA address
  @tparam _UCSRB the UCSRB address
  @tparam _UCSRC the UCSRC address
  @tparam _UBRRL the UBRRL address
  @tparam _UBRRH the UBRRH address

  Usage:
  ~~~{.c}
  SABA::USART0 usart(19200);  // use one of the defined typedefs
  ~~~ 
  */
  template <SFRA _UDR,SFRA _UCSRA,SFRA _UCSRB,SFRA _UCSRC,SFRA _UBRRL,SFRA _UBRRH
  ,uint8_t _RXEN,uint8_t _TXEN,uint8_t _UCSZ0,uint8_t _UCSZ1,uint8_t _TXC,uint8_t _RXC,uint8_t _UDRE>
  class Usart
  {
  public:

    Usart(uint32_t baudrate)  //! initializes the Usart using the desired baudrate
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
      ucsrb= _BV(_RXEN)|_BV(_TXEN);

// Atmega 8
#ifdef URSEL
      ucsrc= _BV(URSEL) | _BV(UCSZ0) | _BV(UCSZ1);
#else
      ucsrc= _BV(_UCSZ0) | _BV(_UCSZ1);
#endif
    }
    
    bool transmitComplete() //! return true, if the last transmit was completed
    {
      SFRBIT<_UCSRA,_TXC> txc;

      return txc();
    }
    
    bool readyToSend() //! return true, if the data register is empty, the next char can be written
    {
      SFRBIT<_UCSRA,_UDRE> udre;

      return udre();
    }
    
    void putch( uint8_t ch ) //! waits until data register is empty and transmits the byte
    {
      while(! readyToSend())
        ;

      SFREG<_UDR> udr;
      udr= ch;
    }
    
    bool receiverComplete() //! return true, if data has been received complete, the data register can be read
    {
      SFRBIT<_UCSRA,_RXC> rxc;

      return rxc();
    }
    
    uint8_t getch() //! wait until a receive was completed, then return the received char
    {
      while(!receiverComplete())
        ;

      SFREG<_UDR> udr;

      return udr();
    }
  };


#ifdef UDR
  //! Atmega 8 
  typedef Usart<(SFRA)&UDR,(SFRA)&UCSRA,(SFRA)&UCSRB,(SFRA)&UCSRC,(SFRA)&UBRRL,(SFRA)&UBRRH,RXEN,TXEN,UCSZ0,UCSZ1,TXC,RXC,UDRE> USART0;
#endif
#ifdef UDR0
  //! Atmega xx8
  typedef Usart<(SFRA)&UDR0,(SFRA)&UCSR0A,(SFRA)&UCSR0B,(SFRA)&UCSR0C,(SFRA)&UBRR0L,(SFRA)&UBRR0H,RXEN0,TXEN0,UCSZ00,UCSZ01,TXC0,RXC0,UDRE0> USART0;
#endif
}

#endif // SABA_USART_H_
