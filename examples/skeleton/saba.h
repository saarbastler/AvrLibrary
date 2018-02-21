#ifndef SABA_H_
#define SABA_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "Device.h"

#include <saba_avr.h>
#include <saba_usart.h>
#include <saba_ostream.h>
#include <saba_cmdline.h>
#include <saba_monitor.h>
#include <saba_timer.h>
#include <saba_timing.h>

// the global putch method used to redirect the out
extern void putch(uint8_t c);

// The application output stream, similar to std::cout.
// It will use Usart for output
extern SABA::OStream <&putch> out;

// The command line execution method
extern bool execute(char c);

// The command line will use out as input. It will be filled within the main loop
extern SABA::CmdLine<uint8_t,CMD_LINE_SIZE,&putch,SABA::OStream,execute> cmdline;

/*********************************************************
** Methods to be implemented by application
*********************************************************/

// called once after Reset, used to initialize Hardware
extern void initializeApplication();

// called periodically from the main loop
extern void cyclic();

extern bool applicationCommand(char cmd);

#endif /* SABA_H_ */
