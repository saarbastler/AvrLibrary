/*
 * saba_tm1638.h
 * Titanmec TM1638 Bitbang driver
 *
 * Created: 08.04.2018 14:06:32
 *  Author: Joerg
 */ 


#ifndef SABA_TM1638_H_
#define SABA_TM1638_H_

#define S_A   (1 << 0)
#define S_B   (1 << 1)
#define S_C   (1 << 2)
#define S_D   (1 << 3)
#define S_E   (1 << 4)
#define S_F   (1 << 5)
#define S_G   (1 << 6)
#define S_DP  (1 << 7)

namespace SABA
{
  /** TM1638 Display and Key driver
    * @tparam PIN_ADDR_STB the TM1638 STB Port: &PINX
    * @tparam PIN_POS_STB the TM1638 STB port pin: 0..7
    * @tparam PIN_ADDR_CLK the TM1638 CLK Port: &PINX
    * @tparam PIN_POS_CLK the TM1638 CLK port pin: 0..7
    * @tparam PIN_ADDR_DIO the TM1638 DIO Port: &PINX
    * @tparam PIN_POS_DIO the TM1638 DIO port pin: 0..7
    */
  template<SFRA PIN_ADDR_STB, uint8_t BIT_POS_STB, SFRA PIN_ADDR_CLK, uint8_t BIT_POS_CLK, SFRA PIN_ADDR_DIO, uint8_t BIT_POS_DIO>
  class TM1638
  {
    public:

    /** Initialize the Port Pins and enable the TM1638
      */
    void init()
    {
      PortPin<PIN_ADDR_CLK,BIT_POS_CLK> clk;
      PortPin<PIN_ADDR_DIO,BIT_POS_DIO> dio;
      PortPin<PIN_ADDR_STB,BIT_POS_STB> stb;

      stb.asOutput()= true;
      clk.asOutput()= true;
      dio.asOutput();

      writeByte(0x40);
      writeByte(0x88);
    }

    /** Enable/Disable and set the pulsewidth the Display
      * @param enable: true: enable, false disable
      * @param pulstwidth: 0..7 see data sheet
      */
    void enableAndPulsewidth(bool enable, uint8_t pulsewidth)
    {
      writeByte( 0x80 | (enable ? 0x8 : 0) | (pulsewidth & 7));
    }

    /** 7 Segment Character generator assuming segment A= Bit 0... G= Bit6
      * @param c nibble 0..f to convert
      * @return bitvector of 7 segment display showing 0..7 and A..F
      */
    static uint8_t __attribute__((naked)) chargen(uint8_t c)
    {
      asm 
      (
        ".set A,1<<0" "\n\t"
        ".set B,1<<1" "\n\t"
        ".set C,1<<2" "\n\t"
        ".set D,1<<3" "\n\t"
        ".set E,1<<4" "\n\t"
        ".set F,1<<5" "\n\t"
        ".set G,1<<6" "\n\t"
        ".pushsection .progmem.data, \"SM\", @progbits, 1" "\n\t"
        "0: .byte A|B|C|D|E|F, B|C, A|B|D|E|G, A|B|C|D|G, B|C|F|G, A|C|D|F|G, A|C|D|E|F|G, A|B|C"             "\n\t"
        "   .byte A|B|C|D|E|F|G, A|B|C|D|F|G, A|B|C|E|F|G, C|D|E|F|G, A|D|E|F, B|C|D|E|G, A|D|E|F|G, A|E|F|G" "\n\t"
        ".popsection"                                      "\n\t"
      );
      asm volatile
      (
        "ldi r30, lo8(0b)"  "\n\t"
        "ldi r31, hi8(0b)"  "\n\t"
        "andi r24,0xf"      "\n\t"
        "add r30, r24"      "\n\t"
        "adc r31, r1"       "\n\t"
        "lpm r24,Z"         "\n\t"
        "ret"               "\n\t"
      );
    }

    /** write data to the TM1638
      * @param address: Ram address 0..F
      * @param data: 16 Bit data to write
      */
    void writeData( uint8_t address, uint8_t data)
    {
      strobe(false);
      writeByteNoStrobe(0xc0 | (address & 0xf));
      
      writeByteNoStrobe(data);

      strobe(true);
    }

    /** write data to the TM1638
      * @param address: Ram address 0..F
      * @param size: Number of bates to write, < 16
      * @param ptr: Data array to write
      */
    void writeData( uint8_t address, uint8_t size, uint8_t *ptr)
    {
      strobe(false);
      writeByteNoStrobe(0xc0 | (address & 0xf));
      
      for( size &= 0xf; size != 0;--size,++ptr)
        writeByteNoStrobe(*ptr);

      strobe(true);
    }
    
    uint32_t readData()
    {
      strobe(false);
      writeByteNoStrobe(0x42);

      PortPin<PIN_ADDR_CLK,BIT_POS_CLK> clk;
      PortPin<PIN_ADDR_DIO,BIT_POS_DIO> dio;
      dio.asInputPullUp();

      uint32_t result = 0;
      for(uint8_t i=0;i < 32;++i)
      {
        clk= false;
        result <<= 1;

        clk= true;
        if( dio() )
          result |= uint8_t(1);
      }
      
      strobe(true);
      dio.asOutput();

      return result;
    }

  protected:

    void writeByteNoStrobe(uint8_t data)
    {
      PortPin<PIN_ADDR_CLK,BIT_POS_CLK> clk;
      PortPin<PIN_ADDR_DIO,BIT_POS_DIO> dio;

      for(uint8_t i=0;i < 8;++i)
      {
        dio= (data & 1);
        clk= false;
        data >>= 1;
        clk= true;
      }
    }

    void strobe(bool value)
    {
      PortPin<PIN_ADDR_STB,BIT_POS_STB> stb;

      stb= value;
    }

    void writeByte(uint8_t data)
    {
      strobe(false);
      writeByteNoStrobe(data);
      strobe(true);
    }
  };

    /** TM1638 8 Digit Display, 8 Key and 8 Led driver
    * @tparam PIN_ADDR_STB the TM1638 STB Port: &PINX
    * @tparam PIN_POS_STB the TM1638 STB port pin: 0..7
    * @tparam PIN_ADDR_CLK the TM1638 CLK Port: &PINX
    * @tparam PIN_POS_CLK the TM1638 CLK port pin: 0..7
    * @tparam PIN_ADDR_DIO the TM1638 DIO Port: &PINX
    * @tparam PIN_POS_DIO the TM1638 DIO port pin: 0..7
    */
  template<SFRA PIN_ADDR_STB, uint8_t BIT_POS_STB, SFRA PIN_ADDR_CLK, uint8_t BIT_POS_CLK, SFRA PIN_ADDR_DIO, uint8_t BIT_POS_DIO>
  class TM1638Display : public TM1638<PIN_ADDR_STB, BIT_POS_STB, PIN_ADDR_CLK, BIT_POS_CLK, PIN_ADDR_DIO, BIT_POS_DIO>
  {
    typedef TM1638<PIN_ADDR_STB, BIT_POS_STB, PIN_ADDR_CLK, BIT_POS_CLK, PIN_ADDR_DIO, BIT_POS_DIO> SUPER;
    public:

    /** clear the complete display incl Leds
      */
    void clearDisplay()
    {
      SUPER::strobe(false);
      SUPER::writeByteNoStrobe(0xc0);
            
      for( uint8_t i=0;i < 16;++i)
        SUPER::writeByteNoStrobe(0);

      SUPER::strobe(true);
    }

    /** print a digit using segment bitvector
      * @param digit the position 0= left, 7= right
      * @param the segment bitvector, bit 0=A ... Bit 7=DP
      */      
    void printSegments(uint8_t digit, uint8_t value)
    {
      SUPER::writeData( (digit & 0x7) << 1, value);      
    }

    /** print a single digit at position
      * @param digit the position 0= left, 7= right
      * @param value the value to write 0..F using the chargen
      * @param decimalPoint if true show the decimal point at this position
      */
    void printDigit(uint8_t digit, uint8_t value, bool decimalPoint= false)
    {
      printSegments( digit, SUPER::chargen(value) | (decimalPoint ? 0x80 : 0));
    }

    /** turn on or off leds
      * @param a bitvector Bit 0= Led left, Bit 7 = Led right
      */
    void leds(uint8_t bitvector)
    {
      for(uint8_t i=1;i < 16;i+=2, bitvector >>= 1)
        SUPER::writeData( i,  bitvector & 1);
    }

    /** print a 32 Bit Hex number
      * @param value the value to print
      * @param decimalPoint the decimal point position 0=right, 7=right, >7 no decimal point
      */
    void printHex(uint32_t value, uint8_t decimalPoint = 8)
    {
      for(uint8_t i=7;i != 0xff;--i,value >>= 4)
        printDigit( i, value & 0xf, decimalPoint == i);
    }


    void printDecimal(uint32_t value, uint8_t decimalPoint = 8)
    {
      if( value > 99999999)
      {
        printDigit(0, 0xe);
        for(uint8_t i=1;i < 8;++i)
          printSegments(i,0);
      }
      else
      {
        uint32_t d= 10000000;
        bool p= false;

        for(uint8_t i=0;i != 8;++i)
        {
          if(i == 7 || p || value >= d)
          {
            p= true;
            printDigit(i, uint8_t(value / d));
            value %= d;
          }
          else
          {
            printSegments(i, 0);
          }

          d /= 10;
        }
      }
    }

    /**read the buttons
      * @return Bit 0 = Button left ... Bit 7 = Button right
      */
    uint8_t readButtons()
    {
      uint32_t r= SUPER::readData();
      uint8_t b= 0;

      b |= ( r & 0x80000000) ? 0x01 : 0;
      b |= ( r & 0x00800000) ? 0x02 : 0;
      b |= ( r & 0x00008000) ? 0x04 : 0;
      b |= ( r & 0x00000080) ? 0x08 : 0;
      b |= ( r & 0x08000000) ? 0x10 : 0;
      b |= ( r & 0x00080000) ? 0x20 : 0;
      b |= ( r & 0x00000800) ? 0x40 : 0;
      b |= ( r & 0x00000008) ? 0x80 : 0;

      return b;
    }
  };
}

#endif /* SABA_TM1638_H_ */