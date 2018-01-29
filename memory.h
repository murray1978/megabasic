/*
   Memory.h
   Memory management for an external EEPROM on an arduino mega
   20/12/17
    inital construction
   25/12/17
    removed heap struct
    adding memory size constraints
   28/12/17
    Bitmap for memory will take up to 64Kb for 1 byte minimum size
    Bitmap for memory will take up to 32Kb for 2 byte minimum size
   29/1/18
    Linked list of used memory and Linked list of free memory.
    Memory Size free will never equal actual memory size.
*/
#pragma once
#include <Wire.h>
#include <Arduino.h>
#include "HeapTypes.h"

/*Device address for read and write*/
#define EPROM_DEVICE 0x50

class _Heap {

  public:

    _Heap();

    //Startup  
    void init();
    void init(const size_t n );

    //allocate n size of memory
    h_uint alloc( const size_t n);

    //heap info
    h_uint getSize( void ); 
    h_uint getFree( void );
    h_uint getBlockSize( void );

    //Byte functionsh_uint
    int HeapWriteByte(h_uchar cdata, h_uint address);
    h_uchar HeapReadByte( h_uint address);

    //Word functions
    void HeapWriteWord( h_uint idata, h_uint address );
    h_uint HeapReadWord( h_uint address );

  private:
    //current size of heap,will be external EEPROM size
    h_uint HeapSize;
    h_uint nextSlot;
    h_uint freeSize;

   
};

extern _Heap Heap;
