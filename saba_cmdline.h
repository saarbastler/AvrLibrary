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
  //! the execute function is called if the user send the line
  typedef bool EXECUTE(char c);

  // \brief A command line buffer reader super class
  /** 
  A derived class has to fill the buffer. This class provides methods to read characters, hex numbers or decimal numbers.

  @tparam INDEX_TYPE the index type, if the buffer size is < 256 use uint8_t, if >= 256 use uint16_t
  @tparam BUFFER_SIZE the size of the buffer. Max number of chars in one line.
  */
  template<typename INDEX_TYPE,INDEX_TYPE BUFFER_SIZE>
  class CmdReader
  {
    public:
    char nextChar() //! return the next character of the buffer. If no more character is available, return 0
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

    char nextCharIgnoreBlank() //! return the next character of the buffer. Blank and TAB is ignored. If no more character is available, return 0
    {
      char ch= nextChar();
      
      while( ch == ' ' || ch == 9 )
        ch= nextChar();
      
      return ch;
    }

    template<typename T> T nextHex() //! interpret the next characters as a hexadecimal number. If there is no hexadecimal character or the input is bigger than the variable, the error flag is set.
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

    template<typename T> T nextDec() //! interpret the next characters as a decimal number. If there is no decimal character or the input is bigger than the variable, the error flag is set.
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

    bool operator () () //! return true, if there was no error in nextHex or nextDec function.
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

  // \brief A command line buffer, typically used as serial input buffer
  /** 
  The user input from (typically) serial port is collected in a line. Backspace will delete the last character.
  Enter will send the line to the EXECUTE function.
  
  @tparam INDEX_TYPE the index type, if the buffer size is < 256 use uint8_t, if >= 256 use uint16_t
  @tparam BUFFER_SIZE the size of the buffer. Max number of chars in one line.
  @tparam PUTCH The putch function receiving the output
  @tparam EXECUTE The execute function called after Enter has been received. If function returns false an error message is printed.
  */
  template<typename INDEX_TYPE,INDEX_TYPE BUFFER_SIZE, PUTCH putch, template<PUTCH putch> class OStream, EXECUTE execute>
  class CmdLine : public CmdReader<INDEX_TYPE, BUFFER_SIZE>
  {
  public:
    void appendChar( char ch ) //! used to add a character to the buffer. The character will be echoed to the putch function. Backspace will delete the last character in the buffer. Enter will call the EXECUTE function. If the buffer is full, the character is not stored and echoed.
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

    protected:

    void printError( const char *string )
    {
      OStream<putch> ostr;
      for(uint8_t i=1;i < CmdReader<INDEX_TYPE, BUFFER_SIZE>::indexOut; i++ )
          ostr << '-';

      ostr << '^' << endl << string << endl;
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

    private:

    INDEX_TYPE indexIn = 0;
  };

}

 #endif // SABA_CMDLINE_H_
