/*
 * saba_test.h
 *
 * Created: 29.03.2018 20:24:23
 *  Author: Joerg
 */ 


#ifndef SABA_TEST_H_
#define SABA_TEST_H_

#include <saba_ostream.h>

extern void putch(uint8_t c);
extern SABA::OStream <&putch> out;

const char TEST_TXT1[] PROGMEM = "EQUAL( ";
const char TEST_TXT2[] PROGMEM = " ) failed, line ";

#ifdef SABA_TEST_SHORT_FORM

#define SABA_EQUAL( a, b ) if( (a) != (b) ) out << TEST_TXT1 << (a) << ',' << (b) << TEST_TXT2 << __LINE__ << SABA::endl

#else

#define SABA_EQUAL( a, b ) if( (a) != (b) ) out << TEST_TXT1 << (a) << ',' << (b) << TEST_TXT2 << __LINE__ << ' ' << '[' << PSTR( #a) << ',' << PSTR(#b) << ']' << SABA::endl

#endif



#endif /* SABA_TEST_H_ */