/*
 * test_saba_vt100.cpp
 *
 * Created: 27.03.2018 19:37:20
 *  Author: Joerg
 */ 

 #include <saba_ostream.h>

 #include "saba_vt100.h"

 extern void putch(uint8_t c);
 extern SABA::OStream <&putch> out;

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

  virtual void putch( const char c )
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

  virtual void backspace()
  {
    method= BACKSPACE;
    ++count;
  }

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

const char TEST_TXT1[] PROGMEM = "EQUAL( ";
const char TEST_TXT2[] PROGMEM = " ) failed, line ";
#define EQUAL( a, b ) if( (a) != (b) ) out << TEST_TXT1 << (a) << ',' << (b) << TEST_TXT2 << __LINE__ << SABA::endl

void testVT100_putch()
{
  VT100TargetMock mock;
  SABA::VT100 vt100(&mock);

  vt100.putch('a');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, 'a');
  EQUAL( mock.count, 1);

  vt100.putch('b');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, 'b');
  EQUAL( mock.count, 2);

  vt100.putch('c');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, 'c');
  EQUAL( mock.count, 3);

  vt100.putch('d');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, 'd');
  EQUAL( mock.count, 4);

  vt100.putch('e');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, 'e');
  EQUAL( mock.count, 5);

  vt100.putch('f');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, 'f');
  EQUAL( mock.count, 6);
}

void testVT100_backspace()
{
  VT100TargetMock mock;
  SABA::VT100 vt100(&mock);

  vt100.putch('x');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, 'x');
  EQUAL( mock.count, 1);

  vt100.putch(8);
  EQUAL( mock.method, VT100TargetMock::BACKSPACE);
  EQUAL( mock.count, 2);

  vt100.putch('y');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, 'y');
  EQUAL( mock.count, 3);
}

void testVT100_clearScreen()
{
  VT100TargetMock mock;
  SABA::VT100 vt100(&mock);

  vt100.putch('1');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, '1');
  EQUAL( mock.count, 1);

  vt100.putch(27);
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, '1');
  EQUAL( mock.count, 1);
  vt100.putch('[');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, '1');
  EQUAL( mock.count, 1);
  vt100.putch('2');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, '1');
  EQUAL( mock.count, 1);
  vt100.putch('J');
  EQUAL( mock.method, VT100TargetMock::CLEAR_SCREEN);
  EQUAL( mock.count, 2);

  vt100.putch('y');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, 'y');
  EQUAL( mock.count, 3);

  vt100.putch(27);
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, 'y');
  EQUAL( mock.count, 3);
  vt100.putch('[');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, 'y');
  EQUAL( mock.count, 3);
  vt100.putch('0');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, 'y');
  EQUAL( mock.count, 3);
  vt100.putch('2');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, 'y');
  EQUAL( mock.count, 3);
  vt100.putch('J');
  EQUAL( mock.method, VT100TargetMock::CLEAR_SCREEN);
  EQUAL( mock.count, 4);

  vt100.putch('z');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, 'z');
  EQUAL( mock.count, 5);
}

void testVT100_cursorHome()
{
  VT100TargetMock mock;
  SABA::VT100 vt100(&mock);

  vt100.putch('1');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, '1');
  EQUAL( mock.count, 1);

  vt100.putch(27);
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, '1');
  EQUAL( mock.count, 1);
  vt100.putch('[');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, '1');
  EQUAL( mock.count, 1);
  vt100.putch('H');
  EQUAL( mock.method, VT100TargetMock::CURSOR_HOME);
  EQUAL( mock.count, 2);

  vt100.putch('y');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, 'y');
  EQUAL( mock.count, 3);
}

void testVT100_cursorPos()
{
  VT100TargetMock mock;
  SABA::VT100 vt100(&mock);

  vt100.putch('1');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, '1');
  EQUAL( mock.count, 1);

  vt100.putch(27);
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, '1');
  EQUAL( mock.count, 1);
  vt100.putch('[');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, '1');
  EQUAL( mock.count, 1);
  vt100.putch('0');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, '1');
  EQUAL( mock.count, 1);
  vt100.putch('2');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, '1');
  EQUAL( mock.count, 1);
  vt100.putch(';');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, '1');
  EQUAL( mock.count, 1);
  vt100.putch('2');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, '1');
  EQUAL( mock.count, 1);
  vt100.putch('0');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, '1');
  EQUAL( mock.count, 1);
  vt100.putch('9');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, '1');
  EQUAL( mock.count, 1);

  vt100.putch('H');
  EQUAL( mock.method, VT100TargetMock::SET_CURSOR_POS);
  EQUAL( mock.count, 2);
  EQUAL( mock.xarg, 2);
  EQUAL( mock.yarg, 209);

  vt100.putch('y');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, 'y');
  EQUAL( mock.count, 3);
}

void testVT100_saveAndRestoreCursor()
{
  VT100TargetMock mock;
  SABA::VT100 vt100(&mock);

  vt100.putch('X');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, 'X');
  EQUAL( mock.count, 1);

  vt100.putch(0x1b);
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, 'X');
  EQUAL( mock.count, 1);
  vt100.putch('7');
  EQUAL( mock.method, VT100TargetMock::SAVE_CURSOR);
  EQUAL( mock.count, 2);

  vt100.putch('Z');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, 'Z');
  EQUAL( mock.count, 3);

  vt100.putch(0x1b);
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, 'Z');
  EQUAL( mock.count, 3);
  vt100.putch('8');
  EQUAL( mock.method, VT100TargetMock::RESTORE_CURSOR);
  EQUAL( mock.count, 4);

  vt100.putch('U');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, 'U');
  EQUAL( mock.count, 5);
}

void testVT100_Error()
{
  VT100TargetMock mock;
  SABA::VT100 vt100(&mock);

  vt100.putch('f');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, 'f');
  EQUAL( mock.count, 1);

  vt100.putch(0x1b);
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, 'f');
  EQUAL( mock.count, 1);

  vt100.putch('a');
  EQUAL( mock.method, VT100TargetMock::ERROR);
  EQUAL( mock.ch, 'a');
  EQUAL( mock.count, 2);

  vt100.putch('g');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, 'g');
  EQUAL( mock.count, 3);

  vt100.putch(0x1b);
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, 'g');
  EQUAL( mock.count, 3);

  vt100.putch('[');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, 'g');
  EQUAL( mock.count, 3);

  vt100.putch('q');
  EQUAL( mock.method, VT100TargetMock::ERROR);
  EQUAL( mock.ch, 'q');
  EQUAL( mock.count, 4);

  vt100.putch('h');
  EQUAL( mock.method, VT100TargetMock::PUTCH);
  EQUAL( mock.ch, 'h');
  EQUAL( mock.count, 5);
}

void testVT100()
{
  out.width(0);
  out << SABA::dec << PSTR("Starting Tests") << SABA::endl;

  testVT100_putch();
  testVT100_backspace();
  testVT100_clearScreen();
  testVT100_cursorHome();
  testVT100_cursorPos();
  testVT100_saveAndRestoreCursor();
  testVT100_Error();

  out << SABA::dec << PSTR("Tests Finished") << SABA::endl;
}