/*
 * saba_spi.h
 *
 * Saarbastler AVR C++ 11 Library
 *
 * Created: 05.11.2017
 * Author: Joerg
 *
 * SPI functionality
 */ 

#ifndef SABA_SPI_H_
#define SABA_SPI_H_

namespace SABA
{
  /**
   @namespace SABA::SPI
   @brief SPI Serial Peripheral Interface
  */
  namespace SPI
  {
    /// The SPI clock rate select enum
    enum ClockRateSelect
    {
      By4= 0,     /**< Oscillator clock by 4, SPI2X= 0*/
      By16= 1,    /**< Oscillator clock by 16, SPI2X= 0*/
      By64= 2,    /**< Oscillator clock by 64, SPI2X= 0*/
      By128= 3,   /**< Oscillator clock by 128, SPI2X= 0*/
      By2= 4,     /**< Oscillator clock by 2, SPI2X= 1*/
      By8= 5,     /**< Oscillator clock by 8, SPI2X= 1*/
      By32= 6,    /**< Oscillator clock by 32, SPI2X= 1*/
      //By64= 7     /**< Oscillator clock by 64, SPI2X= 1*/
    };

    /// The SPI clock polarity and phase selection
    enum ClockPolarityPhase
    {
      Mode0= 0, /**< SPI Mode 0: Sample on Rising, Setup on Falling */
      Mode1= 1, /**< SPI Mode 1: Setup on Rising, Sample on Falling */
      Mode2= 2, /**< SPI Mode 2: Sample on Falling, Setup on Rising */
      Mode3= 3  /**< SPI Mode 3: Setup on Falling, Sample on Rising */
    };

    /** A template class to access the SPI hardware
  
      @tparam _SPCR the SPCR address
      @tparam _SPSR the SPSR address
      @tparam _SPDR the SPDR address

      Usage:
      ~~~{.c}
      SABA::SPI::Timer1 timer1; // use the typedef instead of the template will save space

      timer1
        .waveformGenerationMode(SABA::Timer::Normal)
        .clockSelect(SABA::Timer::By8);
      ~~~ 
      */
    template<SFRA _SPCR,SFRA _SPSR,SFRA _SPDR>
    class _SerialPeripheralInterface
    {
    public:

      /** enables or disable the SPI
       * @param e true: enable, false: disable
       * @return the this object for creating fluent calls
      */
      _SerialPeripheralInterface& enable(bool e)
      {
        SFRBIT<_SPCR,SPE> spe;

        spe= e;

        return *this;
      }

      /** get the SPI enabled bit
       * @return true: enable, false: disable
       */
      bool isEnabled()
      {
        SFRBIT<_SPCR,SPE> spe;

        return spe;
      }

      /** enables or disable the SPI interrupt
       * @param e true: enable, false: disable
       * @return the this object for creating fluent calls
      */
      _SerialPeripheralInterface& enableInterrupt(bool e)
      {
        SFRBIT<_SPCR,SPIE> spie;

        spie= e;

        return *this;
      }

      /** get the SPI interrupt enabled bit
       * @return true: enable, false: disable
       */
      bool isInterruptEnabled()
      {
        SFRBIT<_SPCR,SPIE> spie;

        return spie;
      }

      /** sets the SPI data order
       * @param lsbFirst true: LSB first, false: MSB first
       * @return the this object for creating fluent calls
      */
      _SerialPeripheralInterface& dataorder(bool lsbFirst)
      {
        SFRBIT<_SPCR,DORD> dord;

        dord= lsbFirst;

        return *this;
      }

      /** get the SPI data order
       * @return true: LSB first, false: MSB first
       */
      bool getDataorder()
      {
        SFRBIT<_SPCR,DORD> dord;

        return dord;
      }

      /** sets the SPI master mode
       * @param master true: master, false: slave
       * @return the this object for creating fluent calls
      */
      _SerialPeripheralInterface& master(bool master)
      {
        SFRBIT<_SPCR,MSTR> mstr;

        mstr= master;

        return *this;
      }

      /** get the SPI master mode
       * @return true: master, false: slave
       */
      bool getMaster()
      {
        SFRBIT<_SPCR,MSTR> mstr;

        return mstr;
      }


      /** set the SPI clock rate select
       * @param c the ClockRateSelect enum constant, see also the SPRx bits in SPCR and SP2X in SPSR register description
       * @return the this object for creating fluent calls
      */
      _SerialPeripheralInterface& clockRateSelect(ClockRateSelect c)
      {
        SFRBITS<_SPCR,_BV(SPR1)|_BV(SPR0),SPR0> spr;
        SFRBIT<_SPSR,SPI2X> sp2x;

        spr= c & 3;
        sp2x= (c & 4);

        return *this;
      }

      /** get the SPI clock rate select
       * @return the ClockRateSelect enum read from SPCR and SPSR register
       */
      ClockRateSelect getClockRateSelect()
      {
        SFRBITS<_SPCR,_BV(SPR1)|_BV(SPR0),SPR0> spr;
        SFRBIT<_SPSR,SPI2X> sp2x;

        return static_cast<ClockRateSelect>( spr | (sp2x ? 4 : 0));
      }

      /** set the SPI mode select see CPHA and CPOL bits in SPCR register description
       * @param c the ClockPolarityPhase enum constant
       * @return the this object for creating fluent calls
      */
      _SerialPeripheralInterface& clockPolarityPhase(ClockPolarityPhase mode)
      {
        SFRBITS<_SPCR,_BV(CPHA)|_BV(CPOL),CPHA> cphacpol;

        cphacpol= mode;

        return *this;
      }

      /** get the SPI mode select
       * @return the ClockPolarityPhase enum read from SPCR register
       */
      ClockPolarityPhase getClockPolarityPhase()
      {
        SFRBITS<_SPCR,_BV(CPHA)|_BV(CPOL),CPHA> cphacpol;

        return static_cast<ClockPolarityPhase>(cphacpol);
      }
      
      /** get the SPI interrupt flag
       * @return the SPI interrupt flag SPIF of SPSR register
       */
      bool interruptFlag()
      {
        SFRBIT<_SPSR,SPIF> spif;

        return spif();
      }

      /** get the SPI write collision flag
       * @return the write collision flag WCOL of SPSR register
       */
      bool isWriteCollision()
      {
        SFRBIT<_SPSR,WCOL> wcol;

        return wcol();
      }

      /** the C++ operator () returns the SPDR register value
       * @return the SPDR value
       */
      uint8_t operator() ()
      {
        SFREG <_SPDR> spdr;

        return spdr();
      }

      /** the C++ operator = allows write access to the SPDR register
       * @param value the SPDR value
       */
      _SerialPeripheralInterface& operator= (uint8_t value)
      {
        SFREG <_SPDR> spdr;

        spdr= value;

        return *this;
      }
    };

    typedef _SerialPeripheralInterface<(SFRA)&SPCR,(SFRA)&SPSR,(SFRA)&SPDR> SerialPeripheralInterface;

  }
}

#endif // SABA_SPI_H_

