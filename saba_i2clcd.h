/*
 * saba_LcdText.h
 *
 * Created: 17.03.2018 17:33:47
 *  Author: Joerg
 */ 


#ifndef SABA_LcdText_H_
#define SABA_LcdText_H_

#include "saba_timing.h"
#include "saba_i2cm.h"

// The I2C Input/Output Bits to the LCD Controller
#define LCD_BUSYFLAG  7
#define LCD_BACKLIGHT 3
#define LCD_ENABLE    2
#define LCD_RW        1
#define LCD_RS        0

#ifdef DEBUG_LcdText_MESSAGE
extern void putch(uint8_t c);
extern SABA::OStream <&putch> out;
#endif

namespace SABA
{
  namespace I2C
  {
    // \brief I2C Lcd Text Display Driver
    /** 
    A template class to access a I2C Text LCD.
    @tparam FIFO_SIZE the putch Fifo size for non blocking printing
    @tparam address the I2C Device Adress, tpyicalle 0x4e
    @tparam DIPLAY_LINES false for 1 line, false for 2 lines (see LCD Datasheet)
    @tparam FONT false = 5*7 font
    */
    template<bool DISPLAY_LINES=false,bool FONT=false>
    class LcdText
    {
    public:

      LcdText(SABA::I2C::I2CMaster& master, uint8_t address) : master(&master), address(address) { } //! Constructs the Driver, set the I2CMaster
  
      typedef void(*ERROR_RETURN)(uint8_t errorCode);
      typedef void(*WRITE_RETURN)(void *env);
      typedef void(*Callback)(void *env);
  
    /** Initialize the Display, function blocks only, if the I2CMaster is busy.
      * @param initReturn: callback in case of successful initialization.
      * @param errorReturn: callback in case of any Error, the error callback is stored and also called from other methods
      */
      void initialize(Callback initReturn, ERROR_RETURN errorReturn, void *callbackEnv = nullptr)
      {
        // wait, if I2C Master is busy
        while((*master)());
    
        context2= (void*)initReturn;
        contextEnv= callbackEnv;
        this->errorReturn= errorReturn;
        master->startWrite(address, 1, &orMask,[](void *env, SABA::I2C::CMD* cmd)
        {
          LcdText *me= (LcdText*)env;
          me->delay( 100, [](LcdText* me)
          {
            me->writeNibble(0x30, [](void *env)
            {
              LcdText *me= (LcdText*)env;
              me->delay(5, [](LcdText* me)
              {
                me->writeNibble(0x30, [](void *env)
                {
                  LcdText *me= (LcdText*)env;
                  me->delay(5, [](LcdText* me)
                  {
                    me->writeNibble(0x30, [](void *env)
                    {
                      LcdText *me= (LcdText*)env;
                      me->delay(1, [](LcdText* me)
                      {
                        me->writeNibble(0x20, [](void *env)
                        {
                          LcdText *me= (LcdText*)env;                    
                          me->write(0x28 | (DISPLAY_LINES ? 8 : 0) | (FONT ? 4 : 0), [](void *env)
                          {
                            LcdText *me= (LcdText*)env;
                            me->write( 0x0c, [](void *env)
                            {
                              LcdText *me= (LcdText*)env;
                              me->write( 0x01, [](void *env)
                              {
                                LcdText *me= (LcdText*)env;
                                me->write( 0x6, [](void *env)
                                {
                                  LcdText *me= (LcdText*)env;
                                  me->initialized= true;
                                  ((Callback)me->context2)( me->contextEnv );
                                });
                              });
                            });
                          });
                        });
                      });
                    });
                  });
                });
              });
            });
          } );
   
        },(void*)this);
      }
  
    /** Turn on or off the Backlight
      * @param on: true= Backlight on, false= Backlight off
      * @param callback: the optional callback, called on successful command
      * @return false, if I2C Hardware is busy, command will not be sent
      */
      bool backlight( bool on, Callback callback= nullptr, void *callbackEnv = nullptr )
      {
        if((*master)())
          return false;
      
        if(on)
          orMask |= _BV(LCD_BACKLIGHT);
        else
          orMask &= ~_BV(LCD_BACKLIGHT);
    
        context2= (void*)callback;
        contextEnv= callbackEnv;
        return master->startWrite(address, 1, &orMask, [](void *env, SABA::I2C::CMD* cmd)
        {
          LcdText *me= (LcdText*)env;
          if( cmd->error != 0 )
            me->errorReturn(cmd->error);
          else if(me->context2 != nullptr)
            ((Callback)me->context2)(me->contextEnv);        
        }, (void*)this);
      }
  
    /** Clear the LCD screen
      * @param callback: the optional callback, called on successful command
      * @return false, if I2C Hardware is busy, command will not be sent
      */
      bool clearScreen(Callback callback= nullptr, void *callbackEnv = nullptr)
      {
        return command(1, callback);
      }

    /** Cursor home
      * @param callback: the optional callback, called on successful command
      * @return false, if I2C Hardware is busy, command will not be sent
      */
      bool home(Callback callback= nullptr, void *callbackEnv = nullptr)
      {
        return command(2, callback);
      }

    /** Display options
      * @param on: Display on (true) or off (false)
      * @param cursorOn: Cursor on (true) or off (false)
      * @param cursorBlink: Cursor is blinking (true) or not (false)
      * @param callback: the optional callback, called on successful command
      * @return false, if I2C Hardware is busy, command will not be sent
      */
      bool display(bool on, bool cursorOn, bool cursorBlink, Callback callback= nullptr, void *callbackEnv = nullptr)
      {
        uint8_t data= 8;
        if(on)
          data |= 4;
        if(cursorOn)
          data |= 2;
        if(cursorBlink)
          data |= 1;
    
        return command(data, callback, callbackEnv);
      }
  
    /** Set the Display Data Ram Address
      * @param address: The data Ram address
      * @param callback: the optional callback, called on successful command
      * @return false, if I2C Hardware is busy, command will not be sent
      */
      bool ddram(uint8_t address, Callback callback= nullptr, void *callbackEnv = nullptr)
      {
        return command( 0x80 | address, callback, callbackEnv);
      }
  
    /** Cyclic has to be called regularly, it is the internal state machine
      */
      void cyclic()
      {
        if(delayCallback != nullptr && singleDelay())
        {
          DelayCallback tmp= delayCallback;
          delayCallback= nullptr;
          tmp(this);
        }
      }
  
    /** test, if the display has been initialized
      * @return true, if the display has been successfully initialized
      */
      bool isInitialized()
      {
        return initialized;
      }
  
      bool isMasterBusy()
      {
        return (*master)();
      }

    /** non blocking putch function
      * @param ch: char to print
      * @param callback: the optional callback, called on successful command
      * @return false, if I2C Hardware is busy, command will not be sent
      */
      bool putch(uint8_t ch, Callback putchReturn = nullptr, void *callbackEnv = nullptr)
      {
        if((*master)())
        return false;
          
        context2= (void*)putchReturn;
        contextEnv= callbackEnv;

        orMask |= _BV(LCD_RS);
        return write(ch, [](void *env)
        {
          LcdText *me= (LcdText*)env;
          if(me->context2 != nullptr)
          ((Callback)me->context2)( me->contextEnv );
        });
      }
        

  
    private:

      bool command(uint8_t cmd, Callback callback, void *callbackEnv = nullptr)
      {
        if((*master)())
          return false;
    
        context2= (void*)callback;
        contextEnv= callbackEnv;

        orMask &= ~_BV(LCD_RS);
    
        return write(cmd, [](void *env)
        {
          LcdText *me= (LcdText*)env;
          me->delay( 2, [](LcdText* me)
          {
            if(me->context2 != nullptr)
            ((Callback)me->context2)( me->contextEnv );
          });
        });
      }

      /*bool readBusy(BUSY_RETURN busyReturn)
      {
        if((*master)())
          return false;
    
        orMask &= ~_BV(LCD_RS);
        writeData[0]= orMask | _BV(LCD_RW);
        writeData[1]= orMask | _BV(LCD_BUSYFLAG) | _BV(LCD_RW) | _BV(LCD_ENABLE);
        context= (void*)busyReturn;
    
        return master->startWriteAndRead(address, 2, writeData, 1, &readData, [](void *env, SABA::I2C::CMD* cmd)
        {
          LcdText *me= (LcdText*)env;
          if( cmd->error != 0 )
          {
            me->errorReturn(cmd->error);
          }
          else
          {
            cmd->bytesToWrite= 1;
            cmd->bytesToRead= 0;
            me->master->continueWriteAndRead([](void *env, SABA::I2C::CMD* cmd)
            {
              LcdText *me= (LcdText*)env;
              if( cmd->error != 0 )
                me->errorReturn(cmd->error);
              else
                ((BUSY_RETURN)me->context)(env, me->readData & _BV(LCD_BUSYFLAG));
            } );
          }
        }, (void *)this);
      }*/
  
      bool writeNibble( uint8_t d, WRITE_RETURN writeReturn )
      {
        if( (*master)())
          return false;
    
        d &= 0xf0;
        d |= orMask;
    
        writeData[0]= d | _BV(LCD_ENABLE);
        writeData[1]= d;
    
    #ifdef DEBUG_LcdText_MESSAGE
      out << PSTR("N:") << writeData[0] << ' ' << writeData[1] << SABA::endl;
    #endif
        context= (void*)writeReturn;

        return master->startWrite(address, 2, writeData, [](void *env, SABA::I2C::CMD* cmd)
        {
          LcdText *me= (LcdText*)env;
          if( cmd->error != 0 )
          {
            me->errorReturn(cmd->error);
          }
          else
          {
            ((WRITE_RETURN)me->context)(env);
          }
        }, (void *)this);
      }
  
      bool write( uint8_t d, WRITE_RETURN writeReturn )
      {
        if( (*master)())
          return false;

        uint8_t tmp= (d & 0xf0) | orMask;
        writeData[0]= tmp | _BV(LCD_ENABLE);
        writeData[1]= tmp;
    
        tmp= ((d <<= 4) & 0xf0) | orMask;
        writeData[2]= tmp | _BV(LCD_ENABLE);
        writeData[3]= tmp;

    #ifdef DEBUG_LcdText_MESSAGE
      out << PSTR("B:") << d << ' ' << writeData[0] << ' ' << writeData[1] << ' ' << writeData[2] << ' ' << writeData[3] << SABA::endl;
    #endif

    
        context= (void *)writeReturn;
    
        return master->startWrite(address, 4, writeData, [](void *env, SABA::I2C::CMD* cmd)
        {
          LcdText *me= (LcdText*)env;
          if( cmd->error != 0 )
          {
            me->errorReturn(cmd->error);
          }
          else
          {
            ((WRITE_RETURN)me->context)(env);        
          }
        }, (void *)this);
      }
 
      // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=60972
      // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=58798
      SABA::I2C::I2CMaster *master;
      uint8_t writeData[4];

      uint8_t address;
      uint8_t readData;
      uint8_t orMask = 0;
      bool initialized= false;
  
      void *context = nullptr;  // inner callback
      void *context2= nullptr;  // public method callback - init, putch, ...
      void *contextEnv= nullptr;

      ERROR_RETURN errorReturn = nullptr;
  
      typedef void (*DelayCallback)(LcdText *me);
      DelayCallback delayCallback= nullptr;
      SABA::Timing::SingleDelay<uint8_t> singleDelay;
  
      void delay(uint8_t ms, DelayCallback delayCallback_)
      {
    #ifdef DEBUG_LcdText_MESSAGE
      out << PSTR("D:") << ms << SABA::endl;
    #endif
        delayCallback= delayCallback_;
        singleDelay.start( ms);
      }
  
    };

  }
}


#endif /* SABA_LcdText_H_ */