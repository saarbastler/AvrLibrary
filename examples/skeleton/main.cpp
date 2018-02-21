#include "io_config.h"
#include "saba_spi.h"

// Timing, ~100 Hz
SABA::Timer1 timer1;

// Led flashing delay 
SABA::Timing::SingleDelayFixed<uint8_t,50> ledDelay;

void initializeApplication()
{
  LED led;
  led.asOutput()= false;

  timer1.clockSelect(SABA::Timer16::By256)
    .waveformGenerationMode(SABA::Timer16::FAST_PWM_ICR1)
    .enableOverflowInterrupt(true)
    .icr= 625;

  sei();
  ledDelay.start();

  out << PSTR("ALT") << SABA::endl;
}

void cyclic()
{
  if(ledDelay())
  {
    LED led;
    led.toggle();
    ledDelay.start();
  }
}


bool applicationCommand(char cmd)
{
  return false;
}
