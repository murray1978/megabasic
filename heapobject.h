#pragma once
#include "memory.h"

template <typename T>
class HeapObject
{
  public:
    T value;
    HeapObject( T data ) {
      value = data;
    }
    
    T operator +=( T val) {
      value += val;
    }
    T operator +( T val ) {
      value += val;
      return value;
    }
};

template <>
class HeapObject<int> {

  private:
    int element;
    //memeroy size
    unsigned int size;
    unsigned int length;
    //memory location
    unsigned int offset;

  public:
    HeapObject( int arg )
    {
      //request memory location
      size = sizeof(int);
      offset = Heap.alloc(size);
      if (offset == 0) {
        size = 0;
        offset = 0;
        return;
      }
      Heap.HeapWriteWord((int)arg, offset);
      //element=arg;
    }
    int get() {
      return (int)Heap.HeapReadWord(offset);
    }
    void set( int arg) {
      Heap.HeapWriteWord((int)arg, offset);
    }
    int operator =( int arg ) {
      Heap.HeapWriteWord((int)arg, offset);
    }
    int operator +=( int arg)
    {
      element = (int)Heap.HeapReadWord(offset) + arg;
      Heap.HeapWriteWord(element, offset);
      return (int)element;
    }
    int operator +( int arg )
    {
      element = (int)Heap.HeapReadWord(offset) + arg;
      return (int)element;
    }

    int operator []( int n )
    {
      if( n+offset > length) return 0;
      return (int)Heap.HeapReadWord( offset + n );
    }
};

template <>
class HeapObject<char> {
  private:
    //char element;
    //memeroy size
    unsigned int size;
    //memory location
    unsigned int offset;

  public:
    HeapObject( char arg ) {
      //request memory location
      size = sizeof(char);
      offset = Heap.alloc(size);
      if (offset == 0) {
        size = 0;
        offset = 0;
        return;
      }
      Heap.HeapWriteByte(arg, offset);
    }

    char get() {
      return (char)Heap.HeapReadByte(offset);;
    }
    void set( char arg) {
      Heap.HeapWriteByte(arg, offset);
    }
    char operator =( char arg ) {
      Heap.HeapWriteByte(arg, offset);
    }
};

