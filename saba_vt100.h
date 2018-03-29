/*
 * saba_vt100.h
 *
 * Created: 27.03.2018 18:09:24
 *  Author: Joerg
 */ 


#ifndef SABA_VT100_H_
#define SABA_VT100_H_

#include <saba_avr.h>

#define VT100_CLEAR_SCREEN    "\x1b" "[2J"
#define VT100_CURSOR_HOME     "\x1b" "[H"
#define VT100_SET_POS(x,y)    "\x1b" "[" #y ";" #x "H"
#define VT100_SAVE_CURSOR     "\x1b" "7"
#define VT100_RESTORE_CURSOR  "\x1b" "8"

namespace SABA
{
  class VT100Target
  {
  public:
    virtual void putchar( const char ch ) = 0;
    virtual void error( const char ch ) = 0;
    //virtual void backspace() = 0;
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

            /*case 8:
              vt100Target->backspace();
              break;*/

            default:
              vt100Target->putchar(ch);
          }
      }
    }

  protected:

    void putchDelegateInt(uint8_t i)
    {
      bool p= false;
      if( i > 100 )
      {
        vt100Target->putchar( '0' + (i / 100));
        i %= 100;
        p= true;
      }
      if( p || i > 10 )
      {
        vt100Target->putchar( '0' + (i / 10));
        i %= 10;
      }
      vt100Target->putchar( '0' + i );
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
          mode= NORMAL;
          vt100Target->saveCursor();
          break;

        case '8':
          mode= NORMAL;
          vt100Target->restoreCursor();
          break;

        default:
          mode= NORMAL;
          vt100Target->error(ch);
       }
    }

    void argument(const char ch)
    {
      uint8_t a= (mode == ARG1) ? arg1 : arg2;
      if( isdigit(ch))
      {
        if( a > 99 )
        {
          mode= NORMAL;
          vt100Target->error(ch);
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
            mode= NORMAL;
            vt100Target->clearScreen();
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
          mode= NORMAL;
          vt100Target->setCursorPosition(arg2, arg1);
          return;
        }

        mode= NORMAL;
        vt100Target->error(ch);
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