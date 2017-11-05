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

/**
 @namespace SABA
 @brief Saarbastler's library namespace
*/
namespace SABA
{
  /**
   @namespace SABA::Analog
   @brief Analog Digital converter classes, constants and typedefs
  */
  namespace Analog
  {
    ///  ADC reference selection
    enum Reference
    {
      Off= 0        /**< turned off */
      ,AVcc= 1      /**< AVcc as ARef */
      ,Internal= 3  /**< internal ARef */
    };

    /// The ADC prescaler selection
    enum Prescaler
    {
      By2= 0,   /** divide by 2 */
      By4= 2,   /** divide by 4 */
      By8= 3,   /** divide by 8 */
      By16= 4,  /** divide by 16 */
      By32= 5,  /** divide by 32 */
      By64= 6,  /** divide by 64 */
      By128= 7  /** divide by 128 */
    };
  }

  /** A template class to access the AVR analog digital converter.
  @addtogroup SABA::Analog  
  
  @tparam _ADMUX the ADMUX address
  @tparam _ADCSRA the ADCSRA address
  @tparam _ADC the ADC address

  Usage:
  ~~~{.c}
  SABA::Adc1 adc1; // use the typedef instead of the template will save space

   adc1
   .reference(SABA::Analog::AVcc)
   .leftAdjustResult(false)
   .multiplexer(0)
   .prescaler(SABA::Analog::By64)
   .enable()
   .startConversion();  
  ~~~ 
  */
  template<SFRA _ADMUX,SFRA _ADCSRA, SFRA _ADC>
  class Adc
  {
  public:
  
    /** set the ADC reference
     * @param c the Reference enum, see also the ADMUX register description
     * @return the this object for creating fluent calls
    */
    Adc& reference(Analog::Reference c)
    {
      SFRBITS<_ADMUX,_BV(REFS1)|_BV(REFS0),REFS0> ref;

      ref= c;

      return *this;
    }

    /**  left adjust (true) or right adjust (false) the result, see ADMUX register ADLAR bit
    @param b if true, left adjust the result, if false right adjust the result.
    @return the this object for creating fluent calls
    */
    Adc& leftAdjustResult(bool b)
    {
      SFRBIT<_ADMUX,ADLAR> adlar;

      adlar= b;

      return *this;
    }

    /** choose the analog input
     * @param channel the ADC input channel, see register ADMUX for details
     * @return the this object for creating fluent calls
    */
    Adc& multiplexer(uint8_t channel)
    {
      SFRBITS<_ADMUX,_BV(MUX3)|_BV(MUX2)|_BV(MUX1)|_BV(MUX0),MUX0> ref;

      ref= channel;

      return *this;
    }

    /** enable the ADC, see ADCSRA register ADEN bit
     * @return the this object for creating fluent calls
    */
    Adc& enable() 
    {
      SFRBIT<_ADCSRA,ADEN> aden;
      aden= true;

      return *this;
    }

    /** disable the ADC, see ADCSRA register ADEN bit
     * @return the this object for creating fluent calls
    */
    Adc& disable()
    {
      SFRBIT<_ADCSRA,ADEN> aden;
      aden= false;

      return *this;
    }

    /** start the conversion, see ADCSRA register ADSC bit
     * @return the this object for creating fluent calls
    */
    Adc& startConversion() //! start the conversion
    {
      SFRBIT<_ADCSRA,ADSC> adsc;
      adsc= true;

      return *this;
    }

    /** free running select, see ADCSRA register ADFR bit
     * @param b if true, free running is selected, if false, each conversion has to be started
     * @return the this object for creating fluent calls
    */
    Adc& freeRunningSelect(bool b)
    {
      SFRBIT<_ADCSRA,ADFR> adfr;
      adfr= b;

      return *this;
    }

    /** enable or disable the ADC interrupt, see ADCSRA register ADIE bit
     * @param b if true, enable ADC interrupt, if false disable ADC interrupt
     * @return the this object for creating fluent calls
    */
    Adc& interuptEnable(bool b) //! ADC interrupt enabling
    {
      SFRBIT<_ADCSRA,ADIE> adie;
      adie= b;

      return *this;
    }

    /** reset interrupt flag by setting ADIF in ADCSRA
     * @return the this object for creating fluent calls
    */
    Adc& resetInteruptFlag()
    {
      SFREG<_ADCSRA> adcsra;
      adcsra= _BV(ADIF);

      return *this;
    }

    /** set the ADB prescaler see also ADPSx bits in ADCSRA register
     * @param p the Prescaler enum constant
     * @return the this object for creating fluent calls
    */
    Adc& prescaler( Analog::Prescaler p) //! prescaler selection
    {
      SFRBITS<_ADCSRA,_BV(ADPS2)|_BV(ADPS1)|_BV(ADPS0),ADPS0> prescale;

      prescale= p;

      return *this;
    }

    /** check, if the ADC is enabled
     * @return true, if the ADC is enabled
    */
    bool isEnabled()
    {
      SFRBIT<_ADCSRA,ADEN> aden;

      return aden();
    }

    /** check, if the ADC conversion is running
     * @return true, if the conversion is still running
    */
    bool isConversionRunning()
    {
       SFRBIT<_ADCSRA,ADSC> adsc;

       return adsc();
    }

    /** check, if the ADC free running is selected
     * @return true, if the free running mode is selected
    */
    bool isFreeRunningSelected()
    {
       SFRBIT<_ADCSRA,ADFR> adfr;

       return adfr();
    }

    /** check, if the ADC interrupt flag is set
     * @return true, if the ADC interrupt flag is set
    */
    bool isInteruptFlagSet()
    {
      SFRBIT<_ADCSRA,ADIF> adif;
      
      return adif();
    }

    /** check, if the ADC interrupt is enabled
     * @return true, if the ADC interrupt is enabled
    */
    bool isInteruptEnabled(bool b)
    {
      SFRBIT<_ADCSRA,ADIE> adie;

      return adie();
    }

    /** get the ADC input channel
     * @return the selected multiplexer channel
    */
    uint8_t getChannel()
    {
      SFRBITS<_ADMUX,_BV(MUX3)|_BV(MUX2)|_BV(MUX1)|_BV(MUX0),MUX0> ref;

      return ref();
    }

    /** the C++ operator () returns the ADC register value
     * @return the ADC value
     */
    uint16_t operator() ()
    {
      SFREG16 <_ADC> adc;

      return adc();
    }
  };

  /// Adc1 as a predefined template 
  typedef Adc<(SFRA)&ADMUX,(SFRA)&ADCSRA,(SFRA)&ADC> Adc1;
}

#endif // SABA_ADC_H_
