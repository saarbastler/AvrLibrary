/*
 * saba_i2cm.h
 * I2C Master Library
 * Created: 20.11.2017 20:03:00
 *  Author: Joerg
 */ 


#ifndef SABA_I2CM_H_
#define SABA_I2CM_H_

#include <util/twi.h>

#include <saba_avr.h>

#ifdef DEBUG_I2C_MESSAGE
#include <saba_ostream.h>

extern void putch(uint8_t c);
extern SABA::OStream <&putch> out;
#endif

namespace SABA
{
  namespace I2C
  {
    /// The SPI clock rate select enum
    enum ClockRateSelect
    {
      By1= 0,     /**< Oscillator clock by 1 */
      By4= 1,     /**< Oscillator clock by 4 */
      By16= 2,    /**< Oscillator clock by 16 */
      By64= 3     /**< Oscillator clock by 64 */
    };

    struct CMD
    {
      uint8_t address;
      uint8_t bytesToWrite;
      uint8_t *writeBuffer;
      uint8_t bytesToRead;
      uint8_t *readBuffer;
      uint8_t error;
    };

    typedef void(*DONE_FUNC)(void *env, CMD* cmd);

    class I2CMaster
    {
    public:

      /** start a Write and Read operation, returns immediate, the DONE_FUNC is called if the operation is done.
        * @param address: the I2C address to access
        * @param bytesToWrite: number of bytes to write, if 0, no write is done.
        * @param writeBuffer: data buffer written to the device, may be nullptr, if bytesToWrite is 0
        * @param bytesToRead: number of bytes to read, if 0 no read operation is done.
        * @param readBuffer: read data buffer, ensure its large enough to keep bytesToRead bytes. May be nullptr, if bytesToRead is 0
        * @param doneFunc: function is called, if operation is done or an error happened.
        * @param env: void * pointer for the done function
        * @return true, if the I2C was ready to start the operation. false, if the I2C is running an other operation, try later again.
        */
      virtual bool startWriteAndRead(uint8_t address, uint8_t bytesToWrite, uint8_t *writeBuffer, uint8_t bytesToRead, uint8_t *readBuffer, DONE_FUNC doneFunc= nullptr, void* env= nullptr) = 0;

      /** continues a Write and Read operation is only allowed to be called from a DONE_FUNC, returns immediate, the DONE_FUNC is called if the operation is done.
        * The cmd can get manipulated within the DONE_FUNC.
        * @param doneFunc: function is called, if operation is done or an error happened.
        */
      virtual void continueWriteAndRead(DONE_FUNC doneFunc_= nullptr) = 0;
      
      /** Convenience method for only writing, startWriteAndRead is called */
      bool startWrite(uint8_t address, uint8_t bytesToWrite, uint8_t *writeBuffer, DONE_FUNC doneFunc= nullptr, void* env= nullptr)
      {
        return startWriteAndRead(address, bytesToWrite, writeBuffer, 0, nullptr, doneFunc, env);
      }

      /** Convenience method for only reading, startWriteAndRead is called */
      bool startRead(uint8_t address, uint8_t bytesToRead, uint8_t *readBuffer, DONE_FUNC doneFunc= nullptr, void* env= nullptr)
      {
        return startWriteAndRead(address, 0, nullptr, bytesToRead, readBuffer, doneFunc, env);
      }

      /** the bool operator tells if the I2C Hardware is busy
       * @return true, if the I2C hardware is busy, false if the I2C hardware is ready to send or receive
      */
      virtual bool operator () () = 0;
    };

    template<SFRA TWBR_,SFRA TWSR_,SFRA TWAR_,SFRA TWDR_,SFRA TWCR_,SFRA TWAMR_>
    class Master : public I2CMaster
    {
    public:

      /** set the TWI baudrate and clock rate 
       * @param br the 8 bit baud rate value
       * @param clock the ClockRateSelect enum constant, see also the TWPSx bits in TWSR register description
       * @return the this object for creating fluent calls
      */
      Master& baudrate(uint8_t br, ClockRateSelect clock)
      {
        SFREG<TWBR_> twbr;
        SFRBITS<TWSR_,BIT(TWPS1)|BIT(TWPS0),TWPS0> twsr;

        twbr= br;
        twsr= clock;

        return *this;
      }

      /** enable or disable the TWI
       * @param e true= enable, false= disable
       * @return the this object for creating fluent calls
      */
      Master& enable( bool e )
      {
        SFRBIT<TWCR_,TWEN> twen;

        twen= e;

        return *this;
      }

      virtual void continueWriteAndRead(DONE_FUNC doneFunc_= nullptr)
      {
        busy= true;
        doneFunc= doneFunc_;
        
        bytesRead= 0;
        bytesWritten= 0;
        cmd.error= 0;

  /*out << PSTR("W:") << cmd.bytesToWrite;
  for(uint8_t i=0;i < cmd.bytesToWrite;++i)
    out << ' ' << cmd.writeBuffer[i];
  out << PSTR(" R:") << cmd.bytesToRead << SABA::endl;*/
  
        SFREG<TWCR_> twcr;
        twcr=  BIT(TWINT) | BIT(TWSTA) | BIT(TWEN);

        statemachine();
      }
      
      virtual bool startWriteAndRead(uint8_t address, uint8_t bytesToWrite, uint8_t *writeBuffer, uint8_t bytesToRead, uint8_t *readBuffer, DONE_FUNC doneFunc_= nullptr, void* env_= nullptr)
      {
        if( !busy )
        {
          env= env_;

          cmd.address= address;
          cmd.bytesToWrite= bytesToWrite;
          cmd.writeBuffer= writeBuffer;
          cmd.bytesToRead= bytesToRead;
          cmd.readBuffer= readBuffer;

          continueWriteAndRead( doneFunc_ );
          
          return true;
        }

        return false;
      }

      void statemachine()
      {
        SFREG<TWCR_> twcr;
        SFREG<TWDR_> twdr;

        if( busy && (twcr() & (BIT(TWINT)|BIT(TWEN))) == (BIT(TWINT)|BIT(TWEN)) )
        {
          uint8_t cr= 0;
          uint8_t status= TWSR & 0xf8;

          switch(status)
          {
  	        case TW_START:
  	        case TW_REP_START:
              if( cmd.bytesToWrite == bytesWritten )
                twdr= cmd.address | BIT(0);
              else
                twdr= cmd.address;

              cr= BIT(TWINT) | BIT(TWEN);
              break;

            case TW_MT_SLA_ACK:
              if( bytesWritten >= cmd.bytesToWrite )
              {
                cr= BIT(TWINT) | BIT(TWSTO) | BIT(TWEN);
              }
              else
              {
                twdr= cmd.writeBuffer[bytesWritten++];
                cr= BIT(TWINT) | BIT(TWEN);
              }        
              break;            

            case TW_MR_SLA_ACK:
              if( bytesRead >= cmd.bytesToRead )
                cr= BIT(TWINT) | BIT(TWEN);
              else
                cr= BIT(TWINT) | BIT(TWEN) | BIT(TWEA);
              break;

            case TW_MT_DATA_ACK:
              if( bytesWritten >= cmd.bytesToWrite )
              {
                if( cmd.bytesToRead > 0)
                  cr= BIT(TWINT) | BIT(TWSTA) | BIT(TWEN);
                else
                  cr= BIT(TWINT) | BIT(TWSTO) | BIT(TWEN);
              }
              else
              {
                twdr= cmd.writeBuffer[bytesWritten++];
                cr= BIT(TWINT) | BIT(TWEN);
              }
              break;

            case TW_MR_DATA_ACK:
              cmd.readBuffer[bytesRead++]= twdr();
              if( bytesRead >= cmd.bytesToRead-1 )
                cr= BIT(TWINT) |BIT(TWEN);
              else
                cr= BIT(TWINT) | BIT(TWEA) | BIT(TWEN);
              break;  

            case TW_MR_DATA_NACK:
              cmd.readBuffer[bytesRead++]= twdr();
              cr= BIT(TWINT) | BIT(TWSTO) | BIT(TWEN);
              break;

            default:
              cmd.error= status;
              cr= BIT(TWINT) | BIT(TWSTO) | BIT(TWEN);
              break;
          }

#ifdef DEBUG_I2C_MESSAGE
          out << status << ':' << cr << ' ' << '[' << twdr() << ']' << SABA::endl;
#endif

          twcr= cr;

          if( cr & BIT(TWSTO) )
          {
            busy= false;

            if(doneFunc)
              doneFunc(env, &cmd);
          }
        }
      }

      /** the bool operator tells if the I2C Hardware is busy
       * @return true, if the I2C hardware is busy, false if the I2C hardware is ready to send or receive
      */
      virtual bool operator () ()
      {
        if(busy)
          statemachine();
          
        return busy;
      }

    private:

      bool busy = false;
      CMD cmd;
      uint8_t bytesWritten = 0;
      uint8_t bytesRead = 0;
      DONE_FUNC doneFunc= nullptr;
      void *env = nullptr;
    };

    typedef Master<(SFRA)&TWBR,(SFRA)&TWSR,(SFRA)&TWAR,(SFRA)&TWDR,(SFRA)&TWCR,(SFRA)&TWAMR> Master0;
  }
}


#endif /* SABA_I2CM_H_ */