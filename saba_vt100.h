/*
 * saba_vt100.h
 *
 * Created: 27.03.2018 18:09:24
 *  Author: Joerg
 */ 


#ifndef SABA_VT100_H_
#define SABA_VT100_H_

#include <saba_avr.h>

namespace SABA
{
  class VT100Target
  {
  public:
    virtual void putch( const char ch ) = 0;
    virtual void error( const char ch ) = 0;
    virtual void backspace() = 0;
    virtual void cursorHome() = 0;
    virtual void clearScreen() = 0;
    virtual void setCursorPosition(uint8_t x, uint8_t y) = 0;
    virtual void saveCursor() = 0;
    virtual void restoreCursor() = 0;
  };

  /** VT100 emulating Terminal. Supports these Sequences (ESC = 0x1b)
    * Backspace:            0x08
    * Clear Screen:         ESC [2J
    * Cursor Home:          ESC [H
    * Cursor Pos:           ESC [Line;ColumnH
    * Save Cursor Pos:      ESC 7
    * Restore Cursor Pos:   ESC 8
    */
  class VT100
  {
  public:

    VT100(VT100Target *vt100Target) : vt100Target(vt100Target) {}

    void putch(const char ch)
    {
      switch( mode )
      {
        case ARG1:
        case ARG2:
          argument(ch);
          break;

        case BRACKET:
          bracket(ch);
          break;

        case ESCAPE:
          escape(ch);
          break;

        default:
          switch(ch)
          {
            case 0x1b:
              mode= ESCAPE;
              break;

            case 8:
              vt100Target->backspace();
              break;

            default:
              vt100Target->putch(ch);
          }
      }
    }

  protected:

    void putchDelegateInt(uint8_t i)
    {
      bool p= false;
      if( i > 100 )
      {
        vt100Target->putch( '0' + (i / 100));
        i %= 100;
        p= true;
      }
      if( p || i > 10 )
      {
        vt100Target->putch( '0' + (i / 10));
        i %= 10;
      }
      vt100Target->putch( '0' + i );
    }

  private:

    void bracket(const char ch)
    {
      if( ch == 'H' )
      {
        vt100Target->cursorHome();
        mode= 0;
        return;
      }
      else if( isdigit(ch) )
      {
        arg1= ch - '0';
        mode = ARG1;
        return;
      }
      vt100Target->error(ch);
      mode= NORMAL;
    }

    void escape(const char ch)
    {
      switch(ch)
      {
        case '[':
          mode = BRACKET;
          break;

        case '7':
          vt100Target->saveCursor();
          mode= NORMAL;
          break;

        case '8':
          vt100Target->restoreCursor();
          mode= NORMAL;
          break;

        default:
          vt100Target->error(ch);
          mode= NORMAL;
       }
    }

    void argument(const char ch)
    {
      uint8_t a= (mode == ARG1) ? arg1 : arg2;
      if( isdigit(ch))
      {
        if( a > 99 )
        {
          vt100Target->error(ch);
          mode= NORMAL;
          return;
        }

        a *= 10;
        a += (ch - '0');

        if(mode == ARG1)
          arg1 = a;
         else
          arg2 = a;

        return;
      }
      else 
      {
        if( mode == ARG1 )
        {
          if( arg1 == 2 && ch == 'J' )
          {
            vt100Target->clearScreen();
            mode= NORMAL;
            return;
          }
          else if( ch == ';' )
          {
            arg2= 0;
            mode= ARG2;
            return;
          }
        }
        else if( ch == 'H')        
        {
          vt100Target->setCursorPosition(arg1, arg2);
          mode= NORMAL;
          return;
        }

        vt100Target->error(ch);
        mode= NORMAL;
      }
    }

    VT100Target *vt100Target;
  
    uint8_t mode= NORMAL;  
    uint8_t arg1;
    uint8_t arg2;

    static constexpr uint8_t NORMAL= 0;
    static constexpr uint8_t ESCAPE= 1;
    static constexpr uint8_t BRACKET= 2;
    static constexpr uint8_t ARG1= 3;
    static constexpr uint8_t ARG2= 4;

    constexpr static bool isdigit( const char c )
    {
      return c >= '0' && c <= '9';
    }
  };

}

#endif /* SABA_VT100_H_ */
