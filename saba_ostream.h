/*
 * saba_ostream.h
 *
 * Saarbastler AVR C++ 11 Library
 *
 * Created: 12.10.2017 
 * Author: Joerg
 *
 * std::ostream compatible class need a PUCH function as output
 */ 

#ifndef SABA_OSTREAM_H_
#define SABA_OSTREAM_H_

namespace SABA
{
  //! the putch function needed for output
  typedef void PUTCH(uint8_t);

  // \brief C++ output stream super class
  /** 
  The SABA::ios_base is a similar super class to std::out
  */
  class ios_base
  {
    public:

    //! format flags internal type
    typedef uint8_t _Fmtflags;

    static constexpr _Fmtflags base = (_Fmtflags)3; //! bit mask for base bits hex and dec (oct is not yet implemented)
    static constexpr _Fmtflags hex = (_Fmtflags)0; //! hexadecimal output
    static constexpr _Fmtflags dec = (_Fmtflags)1; //! decimal output

    //! set bits in the format byte
    _Fmtflags setf(_Fmtflags flags)
    {
      fmtflags |= flags;

      return fmtflags;
    }

    //! set masked bits in the format byte
    _Fmtflags setf(_Fmtflags flags, _Fmtflags mask)
    {
      fmtflags &= ~mask;
      fmtflags |= (flags & mask);

      return fmtflags;
    }

    protected:
    _Fmtflags fmtflags= 0;
  };

  // \brief A C++ output stream class
  /** 
  This class is used to output text similar to std::ostream.
  @tparam PUTCH The putch function receiving the output

  Usage:
  ~~~{.c}
  // the OStream putch method redirects output to the Usart
  void putch(uint8_t c)
  {
    usart.putch(c);
  }

  SABA::OStream <&putch> out;

  out << PSTR("Hello World!") << SABA::endl;
  out << PSTR("The answer to everything is:") <<  SABA::dec << 42 << SABA::endl;
  ~~~ 
  */
  template<PUTCH putch>
  class OStream : public ios_base
  {
    public:

    OStream& operator << (char c) //! serializes a single char
    {
      putch(c);

      return *this;
    }

    /*OStream& operator << (const char *str)
    {
      while(*str)
        putch(*str++);

      return *this;
    }*/

    OStream& operator << (const /*PROGMEM*/ char *str) //! serializes a string, use PSTR("xxx") as a string stored in FLASH
    {
      uint8_t ch;
      while((ch= pgm_read_byte(str++)) != 0)
        putch(ch);

      return *this;
    }

    OStream& operator<<(OStream& (*pfn)(OStream&))
    {
      return ((*pfn)(*this));
    }

    OStream& operator<<( bool b ) //! serializes a bool value as 'true' or 'false'
    {
      if(b)
        *this << PSTR("true");
      else
        *this << PSTR("false");

      return *this;
    }

    OStream& operator<<( uint8_t i ) //! serializes an 8 bit unsigned integer
    {
      if( (fmtflags & base) == hex )
        printHex(i);
      else
        printDec(i);

      return *this;
    }

    OStream& operator<<( uint16_t i )  //! serializes an 16 bit unsigned integer
    {
      if( (fmtflags & base) == hex )
        printHex(i);
      else
        printDec(i);

      return *this;
    }

    OStream& operator<<( uint32_t i ) //! serializes an 32 bit unsigned integer
    {
      if( (fmtflags & base) == hex )
        printHex(i);
      else
        printDec(i);

      return *this;
    }

    private:

    void printHex(uint8_t c) /*const*/
    {
      hexnibble( c >> 4 );
      hexnibble( c );
    }

    void printHex(uint16_t w) /*const*/
    {
      printHex( (uint8_t)(w >> 8) );
      printHex( (uint8_t)w );
    }

    void printHex(uint32_t dw) /*const*/
    {
      printHex( (uint16_t)(dw >> 16) );
      printHex( (uint16_t)dw );
    }

    void hexnibble(uint8_t c) /*const*/
    {
      c &= 0xf;
      c |= '0';
      if(c > '9')
        c += 'A' - '9' -1;

      putch( (char) c );
    }

    void printDec(uint8_t b) /*const*/
    {
      if( b >= 100 )
      {
        putch( '0' + ( b / 100 ) );
        b %= 100;
      }

      if( b >= 10 )
      {
        putch( '0' + (b / 10) );
        b %= 10;
      }

      putch( '0' + b );
    }

    void printDec(uint16_t w) /*const*/
    {
      bool p= false;
      uint16_t div= 10000;

      for(;div > 0; div /= 10)
      {
        if( p || div == 1 || w >= div)
        {
          putch( '0' + ( w / div ) );
          w %= div;
          p= true;
        }
      }
    }

    void printDec(uint32_t dw) /*const*/
    {
      bool p= false;
      uint32_t div= 1000000000;

      for(;div > 0; div /= 10)
      {
        if( p || div == 1 || dw >= div)
        {
          putch( '0' + ( dw / div ) );
          dw %= div;
          p= true;
        }
      }
    }
  };

  //! add a CR and LF (new line)
  template<PUTCH p> OStream<p>& /*inline*/ endl(OStream<p>& ostr)
  {
    ostr << '\r' << '\n';

    return ostr;
  }

  //! switch to hexadecimal output
  template<PUTCH p> OStream<p>& hex(OStream<p>& ostr)
  {
    ostr.setf( OStream<p>::hex, OStream<p>::base);

    return ostr;
  }

  //! switch to decimal output
  template<PUTCH p> OStream<p>& dec(OStream<p>& ostr)
  {
    ostr.setf( OStream<p>::dec, OStream<p>::base);

    return ostr;
  }
  
}

#endif // SABA_OSTREAM_H_
