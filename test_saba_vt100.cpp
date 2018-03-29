/*
 * test_saba_vt100.cpp
 *
 * Created: 27.03.2018 19:37:20
 *  Author: Joerg
 */ 

 #define SABA_TEST_SHORT_FORM
 #include <saba_test.h>
 
 #include "saba_vt100.h"

 //extern void putch(uint8_t c);
 //extern SABA::OStream <&putch> out;

class VT100TargetMock : public SABA::VT100Target
{
public:
  static constexpr uint8_t UNDEF= 0;
  static constexpr uint8_t PUTCH = 1;
  static constexpr uint8_t BACKSPACE = 2;
  static constexpr uint8_t CURSOR_HOME = 3;
  static constexpr uint8_t CLEAR_SCREEN = 4;
  static constexpr uint8_t SET_CURSOR_POS = 5;
  static constexpr uint8_t SAVE_CURSOR = 6;
  static constexpr uint8_t RESTORE_CURSOR = 7;
  static constexpr uint8_t ERROR= 8;

  virtual void putchar( const char c )
  {    
    ch= c;
    method= PUTCH;
    ++count;
  }

  virtual void error( const char c )
  {
    ch= c;
    method= ERROR;
    ++count;
  }

  /*virtual void backspace()
  {
    method= BACKSPACE;
    ++count;
  }*/

  virtual void cursorHome()
  {
    method= CURSOR_HOME;
    ++count;
  }

  virtual void clearScreen()
  {
    method= CLEAR_SCREEN;
    ++count;
  }

  virtual void setCursorPosition( uint8_t x, uint8_t y)
  {
    xarg= x;
    yarg= y;
    method= SET_CURSOR_POS;
    ++count;
  }

  virtual void saveCursor()
  {
    method= SAVE_CURSOR;
    ++count;
  }

  virtual void restoreCursor()
  {
    method= RESTORE_CURSOR;
    ++count;
  }


  uint8_t count = 0;
  char ch = 0;
  uint8_t method = 0;
  uint8_t xarg;
  uint8_t yarg;
}; 

/*
const char TEST_TXT1[] PROGMEM = "SABA_EQUAL( ";
const char TEST_TXT2[] PROGMEM = " ) failed, line ";
#define SABA_EQUAL( a, b ) if( (a) != (b) ) out << TEST_TXT1 << (a) << ',' << (b) << TEST_TXT2 << __LINE__ << SABA::endl
*/

void testVT100_putch()
{
  VT100TargetMock mock;
  SABA::VT100 vt100(&mock);

  vt100.putch('a');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, 'a');
  SABA_EQUAL( mock.count, 1);

  vt100.putch('b');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, 'b');
  SABA_EQUAL( mock.count, 2);

  vt100.putch('c');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, 'c');
  SABA_EQUAL( mock.count, 3);

  vt100.putch('d');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, 'd');
  SABA_EQUAL( mock.count, 4);

  vt100.putch('e');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, 'e');
  SABA_EQUAL( mock.count, 5);

  vt100.putch('f');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, 'f');
  SABA_EQUAL( mock.count, 6);
}

void testVT100_backspace()
{
  VT100TargetMock mock;
  SABA::VT100 vt100(&mock);

  vt100.putch('x');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, 'x');
  SABA_EQUAL( mock.count, 1);

  vt100.putch(8);
  SABA_EQUAL( mock.method, VT100TargetMock::BACKSPACE);
  SABA_EQUAL( mock.count, 2);

  vt100.putch('y');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, 'y');
  SABA_EQUAL( mock.count, 3);
}

void testVT100_clearScreen()
{
  VT100TargetMock mock;
  SABA::VT100 vt100(&mock);

  vt100.putch('1');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, '1');
  SABA_EQUAL( mock.count, 1);

  vt100.putch(27);
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, '1');
  SABA_EQUAL( mock.count, 1);
  vt100.putch('[');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, '1');
  SABA_EQUAL( mock.count, 1);
  vt100.putch('2');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, '1');
  SABA_EQUAL( mock.count, 1);
  vt100.putch('J');
  SABA_EQUAL( mock.method, VT100TargetMock::CLEAR_SCREEN);
  SABA_EQUAL( mock.count, 2);

  vt100.putch('y');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, 'y');
  SABA_EQUAL( mock.count, 3);

  vt100.putch(27);
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, 'y');
  SABA_EQUAL( mock.count, 3);
  vt100.putch('[');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, 'y');
  SABA_EQUAL( mock.count, 3);
  vt100.putch('0');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, 'y');
  SABA_EQUAL( mock.count, 3);
  vt100.putch('2');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, 'y');
  SABA_EQUAL( mock.count, 3);
  vt100.putch('J');
  SABA_EQUAL( mock.method, VT100TargetMock::CLEAR_SCREEN);
  SABA_EQUAL( mock.count, 4);

  vt100.putch('z');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, 'z');
  SABA_EQUAL( mock.count, 5);
}

void testVT100_cursorHome()
{
  VT100TargetMock mock;
  SABA::VT100 vt100(&mock);

  vt100.putch('1');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, '1');
  SABA_EQUAL( mock.count, 1);

  vt100.putch(27);
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, '1');
  SABA_EQUAL( mock.count, 1);
  vt100.putch('[');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, '1');
  SABA_EQUAL( mock.count, 1);
  vt100.putch('H');
  SABA_EQUAL( mock.method, VT100TargetMock::CURSOR_HOME);
  SABA_EQUAL( mock.count, 2);

  vt100.putch('y');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, 'y');
  SABA_EQUAL( mock.count, 3);
}

void testVT100_cursorPos()
{
  VT100TargetMock mock;
  SABA::VT100 vt100(&mock);

  vt100.putch('1');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, '1');
  SABA_EQUAL( mock.count, 1);

  vt100.putch(27);
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, '1');
  SABA_EQUAL( mock.count, 1);
  vt100.putch('[');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, '1');
  SABA_EQUAL( mock.count, 1);
  vt100.putch('0');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, '1');
  SABA_EQUAL( mock.count, 1);
  vt100.putch('2');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, '1');
  SABA_EQUAL( mock.count, 1);
  vt100.putch(';');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, '1');
  SABA_EQUAL( mock.count, 1);
  vt100.putch('2');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, '1');
  SABA_EQUAL( mock.count, 1);
  vt100.putch('0');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, '1');
  SABA_EQUAL( mock.count, 1);
  vt100.putch('9');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, '1');
  SABA_EQUAL( mock.count, 1);

  vt100.putch('H');
  SABA_EQUAL( mock.method, VT100TargetMock::SET_CURSOR_POS);
  SABA_EQUAL( mock.count, 2);
  SABA_EQUAL( mock.xarg, 209);
  SABA_EQUAL( mock.yarg, 2);

  vt100.putch('y');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, 'y');
  SABA_EQUAL( mock.count, 3);
}

void testVT100_saveAndRestoreCursor()
{
  VT100TargetMock mock;
  SABA::VT100 vt100(&mock);

  vt100.putch('X');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, 'X');
  SABA_EQUAL( mock.count, 1);

  vt100.putch(0x1b);
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, 'X');
  SABA_EQUAL( mock.count, 1);
  vt100.putch('7');
  SABA_EQUAL( mock.method, VT100TargetMock::SAVE_CURSOR);
  SABA_EQUAL( mock.count, 2);

  vt100.putch('Z');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, 'Z');
  SABA_EQUAL( mock.count, 3);

  vt100.putch(0x1b);
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, 'Z');
  SABA_EQUAL( mock.count, 3);
  vt100.putch('8');
  SABA_EQUAL( mock.method, VT100TargetMock::RESTORE_CURSOR);
  SABA_EQUAL( mock.count, 4);

  vt100.putch('U');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, 'U');
  SABA_EQUAL( mock.count, 5);
}

void testVT100_Error()
{
  VT100TargetMock mock;
  SABA::VT100 vt100(&mock);

  vt100.putch('f');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, 'f');
  SABA_EQUAL( mock.count, 1);

  vt100.putch(0x1b);
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, 'f');
  SABA_EQUAL( mock.count, 1);

  vt100.putch('a');
  SABA_EQUAL( mock.method, VT100TargetMock::ERROR);
  SABA_EQUAL( mock.ch, 'a');
  SABA_EQUAL( mock.count, 2);

  vt100.putch('g');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, 'g');
  SABA_EQUAL( mock.count, 3);

  vt100.putch(0x1b);
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, 'g');
  SABA_EQUAL( mock.count, 3);

  vt100.putch('[');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, 'g');
  SABA_EQUAL( mock.count, 3);

  vt100.putch('q');
  SABA_EQUAL( mock.method, VT100TargetMock::ERROR);
  SABA_EQUAL( mock.ch, 'Q');
  SABA_EQUAL( mock.count, 4);

  vt100.putch('h');
  SABA_EQUAL( mock.method, VT100TargetMock::PUTCH);
  SABA_EQUAL( mock.ch, 'h');
  SABA_EQUAL( mock.count, 5);
}

void testVT100()
{
  out.width(0);
  out << SABA::dec << PSTR("Starting VT100 Tests") << SABA::endl;

  testVT100_putch();
  //testVT100_backspace();
  testVT100_clearScreen();
  testVT100_cursorHome();
  testVT100_cursorPos();
  testVT100_saveAndRestoreCursor();
  testVT100_Error();

  out << SABA::dec << PSTR("VT100 Tests Finished") << SABA::endl;
}