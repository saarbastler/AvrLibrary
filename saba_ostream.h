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

#include <stdint.h>
#include <avr/pgmspace.h>

namespace SABA
{
  //! the putch function needed for output
  typedef void (*PUTCH)(uint8_t);

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
    
    static constexpr _Fmtflags adjust = (_Fmtflags)0x8; //! bit mask for adjust 
    static constexpr _Fmtflags left = (_Fmtflags)0x8; //! left adjust
    static constexpr _Fmtflags right = (_Fmtflags)0x0; //! right adjust
    
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

    uint8_t width(uint8_t newWidth)
    {
      uint8_t tmp= fwidth;
      fwidth= newWidth;
      
      return tmp;
    }
        
    protected:
    _Fmtflags fmtflags= 0;
    uint8_t fwidth = 0;
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

    OStream& print(const char *str)
    {
      while(*str)
        putch(*str++);

      return *this;
    }

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

    OStream& operator<<(OStream& (*pfn)(OStream&,int))
    {
      return ((*pfn)(*this));
    }

    OStream& operator<<( bool b ) //! serializes a bool value as 'true' or 'false'
    {
      uint8_t fsize= 0;
      if( fwidth != 0 )
        fsize= b ? 4 : 5;

      if( (fmtflags & ios_base::adjust) == ios_base::right && fwidth > fsize)
        printFill(fwidth - fsize);

      if(b)
        *this << PSTR("true");
      else
        *this << PSTR("false");

      if( (fmtflags & ios_base::adjust) == ios_base::left && fwidth > fsize)
        printFill(fwidth - fsize);

      return *this;
    }

    OStream& operator<<( uint8_t i ) //! serializes an 8 bit unsigned integer
    {
      if( (fmtflags & base) == hex )
      {
        if( (fmtflags & ios_base::adjust) == ios_base::right && fwidth > 2)
          printFill(fwidth - 2);
      
        printHex( i );
      
        if( (fmtflags & ios_base::adjust) == ios_base::left && fwidth > 2)
          printFill(fwidth - 2);
      }
      else
      {
        printDec(i);
      }

      return *this;
    }

    OStream& operator<<( int8_t i ) //! serializes an 8 bit signed integer
    {
      if( (fmtflags & base) == hex )
        this->operator <<(uint8_t(i));
      else
        printDec( uint8_t(i), true);

      return *this;
    }

    OStream& operator<<( uint16_t i )  //! serializes an 16 bit unsigned integer
    {
      if( (fmtflags & base) == hex )
      {
        if( (fmtflags & ios_base::adjust) == ios_base::right && fwidth > 4)
          printFill(fwidth - 4);
        
        printHex(i);

        if( (fmtflags & ios_base::adjust) == ios_base::left && fwidth > 4)
          printFill(fwidth - 4);
      }
      else
      {
        printDec(i);
      }

      return *this;
    }

    OStream& operator<<( int16_t i ) //! serializes an 16 bit signed integer
    {
      if( (fmtflags & base) == hex )
        this->operator <<(uint16_t(i));
      else
        printDec( uint16_t(i), true);

      return *this;
    }

    OStream& operator<<( uint32_t i ) //! serializes an 32 bit unsigned integer
    {
      if( (fmtflags & base) == hex )
      {
        if( (fmtflags & ios_base::adjust) == ios_base::right && fwidth > 8)
          printFill(fwidth - 8);

        printHex(i);
        if( (fmtflags & ios_base::adjust) == ios_base::left && fwidth > 8)
          printFill(fwidth - 8);
      }
      else
      {
        printDec(i);
      }

      return *this;
    }

    OStream& operator<<( int32_t i ) //! serializes an 32 bit signed integer
    {
      if( (fmtflags & base) == hex )
        this->operator <<(uint32_t(i));
      else
        printDec( uint32_t(i), true);

      return *this;
    }


    private:

    void printFill(uint8_t count)
    {
      for(;count != 0;count--)
        putch(' ');
    }
    
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

    void printDec(uint8_t b, bool signedInt = false)
    {
      uint8_t fsize= 0;
      bool printMinus= false;

      if( signedInt && int8_t(b) < 0)
      {
        printMinus= true;
        b= uint8_t(- int8_t(b));
      }

      if( fwidth != 0 )
      {
        if(b >= 100)
          fsize= 3;
        else if(b >= 10)
          fsize= 2;
        else
          fsize= 1;

        if( printMinus )
          ++fsize;
      }
      
      if( (fmtflags & ios_base::adjust) == ios_base::right && fwidth > fsize)
        printFill(fwidth - fsize);
        
      if( printMinus )
        putch('-');

      bool p= false;
      if( b >= 100 )
      {
        putch( '0' + ( b / 100 ) );
        b %= 100;
        p= true;
      }

      if( p || b >= 10 )
      {
        putch( '0' + (b / 10) );
        b %= 10;
      }

      putch( '0' + b );
      
      if( (fmtflags & ios_base::adjust) == ios_base::left && fwidth > fsize)
        printFill(fwidth - fsize);
    }

    void printDec(uint16_t w, bool signedInt = false) /*const*/
    {
      bool p= false;
      bool printMinus= false;
      uint16_t div= 10000;
      uint8_t fsize= 0;

      if( signedInt && int16_t(w) < 0)
      {
        printMinus= true;
        w= uint16_t(- int16_t(w));
      }

      if( fwidth != 0 )
      {
        if(w >= 10000)
          fsize= 5;
        else if(w >= 1000)
          fsize= 4;
        else if(w >= 100)
          fsize= 3;
        else if(w >= 10)
          fsize= 2;
        else
          fsize= 1;

        if( printMinus )
          ++fsize;
      }
      
      if( (fmtflags & ios_base::adjust) == ios_base::right && fwidth > fsize)
        printFill(fwidth - fsize);

      if( printMinus )
        putch('-');

      for(;div > 0; div /= 10)
      {
        if( p || div == 1 || w >= div)
        {
          putch( '0' + ( w / div ) );
          w %= div;
          p= true;
        }
      }
      
      if( (fmtflags & ios_base::adjust) == ios_base::left && fwidth > fsize)
        printFill(fwidth - fsize);
    }

    void printDec(uint32_t dw, bool signedInt = false) /*const*/
    {
      bool p= false;
      uint32_t div= 1000000000;
      uint8_t fsize= 0;
      bool printMinus= false;

      if( signedInt && int16_t(dw) < 0)
      {
        printMinus= true;
        dw= uint32_t(- int32_t(dw));
      }

      if( fwidth != 0 )
      {
        fsize= 10;
        for(;div > 1; div /= 10,--fsize)
          if(dw >= div)
            break;

        if( printMinus )
          ++fsize;

        div= 1000000000;
      }
      
      if( (fmtflags & ios_base::adjust) == ios_base::right && fwidth > fsize)
        printFill(fwidth - fsize);
        
      if( printMinus )
        putch('-');

      for(;div > 0; div /= 10)
      {
        if( p || div == 1 || dw >= div)
        {
          putch( '0' + ( dw / div ) );
          dw %= div;
          p= true;
        }
      }
       
      if( (fmtflags & ios_base::adjust) == ios_base::left && fwidth > fsize)
        printFill(fwidth - fsize);
     
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

  //! left adjust
  template<PUTCH p> OStream<p>& left(OStream<p>& ostr)
  {
    ostr.setf( OStream<p>::left, OStream<p>::adjust);

    return ostr;
  }

  //! right adjust
  template<PUTCH p> OStream<p>& right(OStream<p>& ostr)
  {
    ostr.setf( OStream<p>::right, OStream<p>::adjust);

    return ostr;
  }

  //! set width
  template<PUTCH p> OStream<p>& setw(OStream<p>& ostr, const int w)
  {
    ostr.width( uint8_t(w) );

    return ostr;
  }
  
}

#endif // SABA_OSTREAM_H_
