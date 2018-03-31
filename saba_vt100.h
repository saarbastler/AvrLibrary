/*
 * saba_vt100.h
 *
 * Created: 27.03.2018 18:09:24
 *  Author: Joerg
 */ 


#ifndef SABA_VT100_H_
#define SABA_VT100_H_

#include <saba_avr.h>

#define VT100_CLEAR_SCREEN        "\x1b" "[2J"
#define VT100_CURSOR_HOME         "\x1b" "[H"
#define VT100_SET_POS(x,y)        "\x1b" "[" #y ";" #x "H"
#define VT100_SAVE_CURSOR         "\x1b" "7"
#define VT100_RESTORE_CURSOR      "\x1b" "8"
// non standard:
#define VT100_BACKLIGHT_ON        "\x1b" "B"
#define VT100_BACKLIGHT_OFF       "\x1b" "b"
#define VT100_CLEAR_SCREEN_HOME   "\x1b" "c"

namespace SABA
{
  class VT100Target
  {
  public:
    enum SpecialFunction
    {
      ClearScreen,      //! Clear the screen
      CursorHome,       //! Cursor Home
      SaveCursor,       //! Save Cursor Position
      RestoreCursor,    //! Restore the cursor position
      BacklightOn,      //! turn the backlight on
      BacklightOff,     //! turn the backlight off
      ClearScreenHome,  //! Clear Screen and Home
    };

    /** execute the SpecialFunction
      * @param sf: see enum
      */
    virtual void specialFunction(SpecialFunction sf) = 0;

    /** print the char 
      * @param ch: the char code: 00 .. FF
      */
    virtual void putchar( const char ch ) = 0;

    /** an error happened in ESC handling
      * @param ch: the error char
      */
    virtual void error( const char ch ) = 0;


    /** set the cursor Position
      * @param x: the x position, 0 .. Width-1
      * @param y: the y position, 0 .. height-1
      */
    virtual void setCursorPosition(uint8_t x, uint8_t y) = 0;

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

            default:
              vt100Target->putchar(ch);
          }
      }
    }  

  private:

    void bracket(const char ch)
    {
      if( ch == 'H' )
      {
        vt100Target->specialFunction(VT100Target::CursorHome);
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
          vt100Target->specialFunction(VT100Target::SaveCursor);
          break;

        case '8':
          mode= NORMAL;
          vt100Target->specialFunction(VT100Target::RestoreCursor);
          break;

        case 'B':
          mode= NORMAL;
          vt100Target->specialFunction(VT100Target::BacklightOn);
          break;

        case 'b':
          mode= NORMAL;
          vt100Target->specialFunction(VT100Target::BacklightOff);
          break;

        case 'c':
          mode= NORMAL;
          vt100Target->specialFunction(VT100Target::ClearScreenHome);
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
            vt100Target->specialFunction(VT100Target::ClearScreen);
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
