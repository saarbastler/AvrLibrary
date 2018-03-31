/*
 * saba_fifo.h
 * FIFO template class
 * Created: 18.03.2018
 *  Author: Joerg
 */ 


#ifndef SABA_FIFO_H_
#define SABA_FIFO_H_

namespace SABA
{
  // \brief A Fifo ring memory
  /** 
  A template class to access a single bit in an AVR special function register. 
  @tparam FIFO_TYPE the type to store in the FIFO
  @tparam INDEX_TYPE the index type, use uint8_t for < 255 or uint16_t for more
  @tparam the size of the memory    
  */
  template<typename FIFO_TYPE, typename INDEX_TYPE, INDEX_TYPE size>
  class Fifo
  {
    public:
    
      /** push a value on the FIFO
       * @param data: the value to push
       * @return true, if successful, false, if the FIFO was full
      */
      bool push(FIFO_TYPE data)
      {
        if( isFull())
          return false;
          
        buffer[writeIndex++] = data;
        if( writeIndex >= size )
        {
          writeIndex= 0;
        }
        allocation++;
        
        return true;
      }
      
      /** pop a value from the FIFO
       * @return  a value from FIFO, 0 if it was empty
      */
      FIFO_TYPE pop()
      {
        FIFO_TYPE ret= 0;
        
        if( !isEmpty() )
        {
          ret= buffer[readIndex++];
          if( readIndex >= size )
          {
            readIndex= 0;
          }
          allocation--;
        }
        
        return ret;
      }

      /** check, if the FIFO is empty
       * @return true, if the FIFO is empty
      */
      bool isEmpty()
      {
        return allocation == 0;
      }

      /** check, if the FIFO is full
       * @return true, if the FIFO is full
      */
      bool isFull()
      {
        return allocation >= size;
      }
    
    void dumpFifo()
    {
      out << PSTR("WI: ") << SABA::dec << writeIndex << PSTR(" RI: ") << readIndex << PSTR(" AL: ") << allocation << SABA::endl << SABA::hex;
      for(INDEX_TYPE i=0;i < writeIndex;i+= 16)
      {
        for(INDEX_TYPE n=0;n < 16;n++ )
          out << uint8_t(buffer[i+n]) << ' ';

        out << ' ';
        for(INDEX_TYPE n=0;n < 16;n++ )
        {
          char ch= buffer[i+n];
          if( ch < ' ')
          ch= '.';

          out << ch;
        }

        out << SABA::endl;
      }

      readIndex= writeIndex= allocation= 0;
    }
      
    private:
    
      FIFO_TYPE buffer[size];
      INDEX_TYPE writeIndex = 0;
      INDEX_TYPE readIndex = 0;
      INDEX_TYPE allocation = 0;
  };
}

#endif // SABA_FIFO_H_

