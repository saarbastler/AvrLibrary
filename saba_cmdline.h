/*
 * saba_cmdline.h
 *
 * Saarbastler AVR C++ 11 Library
 *
 * Created: 12.10.2017 
 * Author: Joerg
 *
 * Cmdline typically for serial 
 */ 

 #ifndef SABA_CMDLINE_H_
 #define SABA_CMDLINE_H_

 #include <saba_ostream.h>

namespace SABA
{
  typedef bool EXECUTE(char c);

  template<typename INDEX_TYPE,INDEX_TYPE BUFFER_SIZE>
  class CmdReader
  {
    public:
    char nextChar()
    {
      char ch= 0;
      if( indexOut < BUFFER_SIZE )
      {
        ch= lineBuffer[ indexOut ];
        if( ch != 0 )
          ++indexOut;
      }
      
      return ch;
    }

    char nextCharIgnoreBlank()
    {
      char ch= nextChar();
      
      while( ch == ' ' || ch == 9 )
        ch= nextChar();
      
      return ch;
    }

    template<typename T> T nextHex()
    {
      T result= 0;
      
      uint8_t ch= fromHex( nextCharIgnoreBlank() );
      if( ch == (char)0xff )
      {
        error = 1;
        return 0;
      }
      
      do
      {
        if( result & (0xf << ((uint8_t)(8*sizeof(T))-4)))
        {
          error=1;
          return 0;
        }
        
        result <<= 4;
        result |= ch;
        
        ch= fromHex( lineBuffer[indexOut] );
        if( ch != (char)0xff )
        {
          nextChar();
        }
      }
      while( ch != (char)0xff );
      
      return result;
    }

    template<typename T> T nextDec()
    {
      T result= 0;
          
      uint8_t ch= fromDec( nextCharIgnoreBlank() );
      if( ch == (char)0xff )
      {
        error = 1;
        return 0;
      }
          
      do
      {
        if( result  > (sizeof(T) == 1 ? 25 : (sizeof(T) == 2 ? 6553 : 429496729)))
        {
          error=1;
          return 0;
        }
            
        result *= 10;
        if( result + ch < result)
        {
          error=1;
          return 0;
        }

        result += ch;
            
        ch= fromDec( lineBuffer[indexOut] );
        if( ch != (char)0xff )
        {
          nextChar();
        }
      }
      while( ch != (char)0xff );
          
      return result;
    }

    bool operator () ()
    {
      return !error;
    }

    protected:

    uint8_t fromHex( char ch )
    {
      if( ch < '0' )
      {
        return 0xff;
      }
      else if( ch <= '9' )
      {
        return ch -'0';
      }
      else if( ch < 'A' )
      {
        return 0xff;
      }
      else if( ch <= 'F' )
      {
        return 10+ ch - 'A';
      }
      else if( ch < 'a' )
      {
        return 0xff;
      }
      else if( ch <= 'f' )
      {
        return 10+ ch - 'a';
      }
      else
      {
        return 0xff;
      }
    }

    uint8_t fromDec( char ch )
    {
      if( ch < '0' )
      {
        return 0xff;
      }
      else if( ch <= '9' )
      {
        return ch -'0';
      }
      else
      {
        return 0xff;
      }
    }

    char lineBuffer[BUFFER_SIZE];
    INDEX_TYPE indexOut = 0;
    bool error;
  };

  template<typename INDEX_TYPE,INDEX_TYPE BUFFER_SIZE, PUTCH putch, template<PUTCH putch> class OStream, EXECUTE execute>
  class CmdLine : public CmdReader<INDEX_TYPE, BUFFER_SIZE>
  {
  public:
    void appendChar( char ch )
    {
      OStream<putch> ostr;

      if( ch == 13 )
      {
        CmdReader<INDEX_TYPE, BUFFER_SIZE>::lineBuffer[indexIn]= 0;
        evaluateLine();
      }
      else if( ch == 8 )
      {
        if( indexIn > 0 )
        {          
          indexIn--;
          ostr << ch << ' ' << ch;
        }
      }
      else if( ch >= (char)' ' && indexIn < BUFFER_SIZE-1 )
      {
        CmdReader<INDEX_TYPE, BUFFER_SIZE>::lineBuffer[indexIn++]= ch;
        ostr << ch;
      }
    }

    void evaluateLine()
    {
      OStream<putch> ostr;
      ostr << endl;

      indexIn= 0;
      CmdReader<INDEX_TYPE, BUFFER_SIZE>::indexOut= 0;
      CmdReader<INDEX_TYPE, BUFFER_SIZE>::error= false;

      if(!execute(CmdReader<INDEX_TYPE, BUFFER_SIZE>::nextCharIgnoreBlank()))
        printError(PSTR("???"));
    }

    void printError( const char *string )
    {
      OStream<putch> ostr;
      for(uint8_t i=1;i < CmdReader<INDEX_TYPE, BUFFER_SIZE>::indexOut; i++ )
          ostr << '-';

      ostr << '^' << endl << string << endl;
    }

    private:

    INDEX_TYPE indexIn = 0;
  };

}

 #endif // SABA_CMDLINE_H_
