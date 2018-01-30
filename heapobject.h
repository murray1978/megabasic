#pragma once
/*
 *  Storing the datatypes,
 *   
 *   TODO:
 *    Reduce all down to a single template <typename T>
*/
#include "memory.h"

template <typename T>
class HeapObject
{
  public:
    T element;
    static unsigned int size;
    static unsigned int length;
    static unsigned char typeSize;
    
    HeapObject( T data ) {
      typeSize = sizeof(T);
    }
    HeapObject(){}
    
    T operator +=( T val) {
      element += val;
    }
    T operator +( T val ) {
      element += val;
      return element;
    }
};

template<>
class HeapObject<float>{
  private:
  int element;
  //memory size
  static unsigned int size;
  static unsigned int length;
  //memory location
  static unsigned int offset;

  public:
  HeapObject(){}
  //Need new, delete, new[], delete[], *
};

template <>
class HeapObject<int> {

  private:
    int element;
    //memeroy size
    static unsigned int size;
    static unsigned int length;
    //memory location
    static unsigned int offset;

  public:
    HeapObject(){}
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

    //Return data at offset
    int get() {
      return (int)Heap.HeapReadWord(offset);
    }

    //set data at offset
    void set( int arg) {
      Heap.HeapWriteWord((int)arg, offset);
    }

    //
    int operator =( int arg ) {
      Heap.HeapWriteWord((int)arg, offset);
    }
    
    //
    int operator +=( int arg)
    {
      element = (int)Heap.HeapReadWord(offset) + arg;
      Heap.HeapWriteWord(element, offset);
      return (int)element;
    }

    //
    int operator +( int arg )
    {
      element = (int)Heap.HeapReadWord(offset) + arg;
      return (int)element;
    }

    //
    int operator []( int n )
    {
      if( n+offset > length) return 0;//get max memory hole size
      return (int)Heap.HeapReadWord( offset + n );
    }

        //
    void* operator new[]( int n )
    {
      if( n+offset > length) return 0;//get max memory hole size
      return (void*)Heap.HeapReadWord( offset + n );
    }
    //Need new, delete, new[], delete[], *
};

template <>
class HeapObject<char> {
  private:
    //char element;
    //memeroy size
    static unsigned int size;
    //memory location
    static unsigned int offset;
    static unsigned int length;

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

    //Length needs to be asigned
    int operator []( int n )
    {
      if( n+offset > length) return 0; //get max memory hole size
      return (int)Heap.HeapReadWord( offset + n );
    }
    
    //
    void* operator new[]( int n )
    {
      if( n+offset > length) return 0;//get max memory hole size
      return (void*)Heap.HeapReadWord( offset + n );
    }
    //Need new, delete, new[], delete[], *
};

