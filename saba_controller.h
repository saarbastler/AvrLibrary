/*
 * saba_controller.h
 *
 * Saarbastler AVR C++ 11 Library
 *
 * Created: 12.10.2017 
 * Author: Joerg
 *
 * Controller specific defines
 */ 

#ifndef SABA_CONTROLLER_H_
#define SABA_CONTROLLER_H_

//! The list is not completed
#if defined (__AVR_ATmega48__) | defined (__AVR_ATmega48A__) | defined (__AVR_ATmega48PA__) | defined (__AVR_ATmega48PB__) | defined (__AVR_ATmega48P__) \
  | defined (__AVR_ATmega88__) |  defined (__AVR_ATmega88A__) | defined (__AVR_ATmega88P__) |  defined (__AVR_ATmega88PA__) |  defined (__AVR_ATmega88PB__) \
  | defined (__AVR_ATmega168__) | defined (__AVR_ATmega168A__) | defined (__AVR_ATmega168P__) | defined (__AVR_ATmega168PA__) | defined (__AVR_ATmega168PB__) \
  | defined (__AVR_ATmega328P__) | (defined __AVR_ATmega328__) | defined (__AVR_ATmega2560__)

//! this controller supports a PORT toggle writing to the PIN register
#define SUPPORTS_PIN_TOGGLE

#endif


#endif // SABA_CONTROLLER_H_
