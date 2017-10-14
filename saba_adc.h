/*
 * saba_adc.h
 *
 * Saarbastler AVR C++ 11 Library
 *
 * Created: 13.10.2017
 * Author: Joerg
 *
 * ADC driver
 */ 

#ifndef SABA_ADC_H_
#define SABA_ADC_H_

#include <saba_avr.h>

namespace SABA
{
  namespace Analog
  {
    enum Reference
    {
      Off= 0, AVcc= 1, Internal= 3
    };

    enum Prescaler
    {
      By2= 0, By4= 2, By8= 3, By16=4, By32= 5, By64= 6, By128= 7
    };
  }

  template<SFRA _ADMUX,SFRA _ADCSRA, SFRA _ADC>
  class Adc
  {
  public:
  
    Adc& reference(Analog::Reference c)  
    {
      SFRBITS<_ADMUX,_BV(REFS1)|_BV(REFS0)> ref;

      ref= c << REFS0;

      return *this;
    }

    Adc& leftAdjustResult(bool b)
    {
      SFRBIT<_ADMUX,ADLAR> adlar;

      adlar= b;

      return *this;
    }

    Adc& multiplexer(uint8_t channel)
    {
      SFRBITS<_ADMUX,_BV(MUX3)|_BV(MUX2)|_BV(MUX1)|_BV(MUX0)> ref;

      ref= channel << MUX0;

      return *this;
    }

    Adc& enable()
    {
      SFRBIT<_ADCSRA,ADEN> aden;
      aden= true;

      return *this;
    }

    Adc& startConversion()
    {
      SFRBIT<_ADCSRA,ADSC> adsc;
      adsc= true;

      return *this;
    }

    Adc& freeRunningSelect(bool b)
    {
      SFRBIT<_ADCSRA,ADFR> adfr;
      adfr= b;

      return *this;
    }

    Adc& interuptEnable(bool b)
    {
      SFRBIT<_ADCSRA,ADIE> adie;
      adie= b;

      return *this;
    }

    Adc& resetInteruptFlag()
    {
      SFREG<_ADCSRA> adcsra;
      adcsra= _BV(ADIF);

      return *this;
    }

    Adc& prescaler( Analog::Prescaler p)
    {
      SFRBITS<_ADCSRA,_BV(ADPS2)|_BV(ADPS1)|_BV(ADPS0)> prescale;

      prescale= p << ADPS0;

      return *this;
    }

    bool isEnabled()
    {
      SFRBIT<_ADCSRA,ADEN> aden;

      return aden();
    }

    bool isConversionRunning()
    {
       SFRBIT<_ADCSRA,ADSC> adsc;

       return adsc();
    }

    bool isFreeRunningSelected()
    {
       SFRBIT<_ADCSRA,ADFR> adfr;

       return adfr();
    }

    bool isInteruptFlagSet()
    {
      SFRBIT<_ADCSRA,ADIF> adif;
      
      return adif();
    }

    bool isInteruptEnabled(bool b)
    {
      SFRBIT<_ADCSRA,ADIE> adie;

      return adie();
    }

    uint8_t getChannel()
    {
      SFRBITS<_ADMUX,_BV(MUX3)|_BV(MUX2)|_BV(MUX1)|_BV(MUX0)> ref;

      return ref() >> MUX0;
    }

    uint16_t operator() ()
    {
      SFREG16 <_ADC> adc;

      return adc();
    }
  };

  typedef Adc<(SFRA)&ADMUX,(SFRA)&ADCSRA,(SFRA)&ADC> Adc1;
}

#endif // SABA_ADC_H_
