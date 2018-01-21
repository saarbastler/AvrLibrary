/*
 * saba_avr.h
 *
 * Saarbastler AVR C++ 11 Library
 *
 * Created: 12.10.2017
 * Author: Joerg
 *
 * Debug monitor helper
 */ 

#ifndef SABA_MONITOR_H_
#define SABA_MONITOR_H_

#include <avr/eeprom.h>

#include <saba_avr.h>
#include <saba_ostream.h>
#include <saba_cmdline.h>

namespace SABA
{
  #define LOWER_CASE(x)       (((x) >= 'A' && (x) <= 'Z') ? ((x) + 0x20) : (x))

  template<typename INDEX_TYPE,INDEX_TYPE BUFFER_SIZE, template<PUTCH putch> class OSTREAM, PUTCH putch>
  class Monitor
  {
  public:

    static bool ports(CmdReader<INDEX_TYPE,BUFFER_SIZE>& cmdReader)
    {      
      char ch= cmdReader.nextCharIgnoreBlank();
      ch= LOWER_CASE( ch );

      uint8_t b= 0;
      char mode= cmdReader.nextCharIgnoreBlank();
      mode= LOWER_CASE( mode );

      if( mode != 0 )
      {
        b= cmdReader.template nextHex<uint8_t>();
        if( !cmdReader() )
        {
          mode= 0;
        }
        else if( mode == 's')
        {
          mode= MODE_SETBIT;
          b= 1 << (7 & b);
        }
        else if( mode == 't')
        {
          mode= MODE_TOGGLE;
          b= 1 << (7 & b);
        }
        else if( mode == 'r' )
        {
          mode= MODE_RESET;
          b= 1 << (7 & b);
        }
        else if( mode == '=' )
        {
          mode= MODE_SET;
        }
        else
        {
          return false;
        }
      }

#if defined(PORTA) && defined(PINA) && defined (DDRA)
      if( ch == 'a' )
        modifyPort<(SFRA)&PINA>(mode, b);
      if( ch == 0 || ch == 'a' )
        dumpPort<(SFRA)&PINA>('A');
#endif
#if defined(PORTB) && defined(PINB) && defined (DDRB)
      if( ch == 'b' )
        modifyPort<(SFRA)&PINB>(mode, b);
      if( ch == 0 || ch == 'b' )
        dumpPort<(SFRA)&PINB>('B');
#endif
#if defined(PORTC) && defined(PINC) && defined (DDRC)
      if( ch == 'c' )
        modifyPort<(SFRA)&PINC>(mode, b);
      if( ch == 0 || ch == 'c' )
        dumpPort<(SFRA)&PINC>('C');
#endif
#if defined(PORTD) && defined(PIND) && defined (DDRD)
      if( ch == 'd' )
        modifyPort<(SFRA)&PIND>(mode, b);
      if( ch == 0 || ch == 'd' )
        dumpPort<(SFRA)&PIND>('D');
#endif
#if defined(PORTE) && defined(PINE) && defined (DDRE)
      if( ch == 'e' )
        modifyPort<(SFRA)&PINE>(mode, b);
      if( ch == 0 || ch == 'e' )
        dumpPort<(SFRA)&PINE>('E');
#endif
#if defined(PORTF) && defined(PINF) && defined (DDRF)
      if( ch == 'f' )
        modifyPort<(SFRA)&PINF>(mode, b);
      if( ch == 0 || ch == 'f' )
        dumpPort<(SFRA)&PINF>('F');
#endif
#if defined(PORTG) && defined(PING) && defined (DDRG)
      if( ch == 'g' )
        modifyPort<(SFRA)&PING>(mode, b);
      if( ch == 0 || ch == 'g' )
        dumpPort<(SFRA)&PING>('G');
#endif
#if defined(PORTH) && defined(PINH) && defined (DDRH)
      if( ch == 'b' )
        modifyPort<(SFRA)&PINH>(mode, b);
      if( ch == 0 || ch == 'h' )
        dumpPort<(SFRA)&PINH>('H');
#endif
#if defined(PORTJ) && defined(PINJ) && defined (DDRJ)
      if( ch == 'j' )
        modifyPort<(SFRA)&PINJ>(mode, b);
      if( ch == 0 || ch == 'j' )
        dumpPort<(SFRA)&PINJ>('J');
#endif
#if defined(PORTK) && defined(PINK) && defined (DDRK)
      if( ch == 'k' )
        modifyPort<(SFRA)&PINK>(mode, b);
      if( ch == 0 || ch == 'k' )
        dumpPort<(SFRA)&PINK>('K');
#endif
#if defined(PORTL) && defined(PINL) && defined (DDRL)
      if( ch == 'l' )
        modifyPort<(SFRA)&PINL>(mode, b);
      if( ch == 0 || ch == 'l' )
        dumpPort<(SFRA)&PINL>('L');
#endif

      return true;
    }


    static bool adc(CmdReader<INDEX_TYPE,BUFFER_SIZE>& cmdReader)
    {
      OStream<putch> ostr;

      ostr << PSTR("ADMUX:  ") << hex << ADMUX << endl
      << PSTR("ADCSRA: ") << ADCSRA << endl
      << PSTR("ADC:    ") << ADC << endl;

      return true;
    }

    static bool spi(CmdReader<INDEX_TYPE,BUFFER_SIZE>& cmdReader)
    {
      OStream<putch> ostr;

      ostr << PSTR("SPCR: ") << hex << SPCR << endl
      << PSTR("SPSR: ") << SPSR << endl;

      return true;
    }

    static bool clock(CmdReader<INDEX_TYPE,BUFFER_SIZE>& cmdReader)
    {
      OStream<putch> ostr;

      ostr << PSTR("OSCAL: ") << hex << OSCCAL << PSTR(" CLKPR: ") << CLKPR << endl;

      return true;
    }
    
    static bool eeprom(CmdReader<INDEX_TYPE,BUFFER_SIZE>& cmdReader)
    {
      uint16_t start= cmdReader.template nextHex<uint8_t>();
      uint8_t rows;
      if( !cmdReader() )
        return false;
        
      rows= cmdReader.template nextHex<uint8_t>();
      if( !cmdReader() )
        return false;
  
      if( rows == 0 )
        rows= 1;
  
      OStream<putch> ostr;
      ostr << hex;
      for( ;rows > 0;rows--)
      {
        ostr <<  start << ':';
        for(uint8_t i=16;i != 0;i--)
        {
          ostr << ' ' << eeprom_read_byte( (const uint8_t *)start );
          start++;
        }
        ostr << endl;
      }

      return true;
    }

  protected:
    static constexpr uint8_t MODE_SETBIT = 1;
    static constexpr uint8_t MODE_RESET = 2;
    static constexpr uint8_t MODE_TOGGLE = 3;
    static constexpr uint8_t MODE_SET = 4;

    static uint16_t lastEEPromsAddr;
    static uint8_t lastEEPromRows;
    
    template<SFRA PIN_ADDR> static void modifyPort(uint8_t mode, uint8_t value)
    {
      Port8<PIN_ADDR> port8;

      switch(mode)
      {
        case MODE_SETBIT:
          port8.port |= value;
          break;

        case MODE_RESET:
          port8.port &= ~value;
          break;

        case MODE_TOGGLE:
          port8.port ^= value;
          break;

        case MODE_SET:
          port8.port= value;
          break;
      }
    }

    template<SFRA PIN_ADDR> static void dumpPort(char pch)
    {
      OStream<putch> ostr;
      Port8<PIN_ADDR> port8;

      ostr << PSTR("PIN") << pch << hex << ':' << port8.pin()
        << PSTR(" DDR") << pch << ':' << port8.ddr()
        << PSTR(" PORT") << pch << ':' << port8.port()
        << endl;
    }
  };
}

#endif // SABA_MONITOR_H_
