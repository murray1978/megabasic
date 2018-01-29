/*
   Arduino Mega Basic Interpriter
   Getting close to a Ardiuno Mega computer
   21/12/17
    Working on memory manager, need to use <T>? for data.
    working on HeapObject, heapobject needs to call heap for memory acces.
*/
#include "basic.h"
#include "memory.h"
#include "heapobject.h"

/*What are we testing today*/
//#define TEST_BASIC
#define TEST_MEMORY

int PRINTS_() {
  Serial.print((char*)*sp++);
  STEP;
}

int kwdhook_(char *msg) {
  if (!strcmp(msg, "PRINTS")) {
    expr(), emit(PRINTS_);
  }
  else {
    return 0;
  }
  return 1;
}

// run once:
void setup() {
  Serial.begin(9600);
  Serial.println(">MegaBasic V0.1 loading");
  Serial.println(">Init Memory");
  
#ifdef TEST_BASIC
  Heap.init();
  initbasic(0);
  kwdhook = kwdhook_;
  compile++;
#endif
#ifdef TEST_MEMORY
  Heap.init();

  HeapObject<char> cbuf('x');// = new HeapObject<char>('x');
  HeapObject<int>  ibuf(4);// = new HeapObject<int>(4);
  HeapObject<char> cat = "ralph"; //need to implement in heapObject.h
  HeapObject<int> catsAge = 4;

  Serial.println("Testing heapObjects");
  Serial.print(">cbuf.getValue() = ");
  Serial.println( (char)cbuf.get() );
  Serial.print(">ibuf.getValue() = ");
  Serial.println((int)ibuf.get() ); 
  cbuf.set('c');
  ibuf.set(10);
  Serial.print(">cbuf.getValue() = ");
  Serial.println( (char)cbuf.get() );
  Serial.print(">ibuf.getValue() = ");
  Serial.println((int)ibuf.get() ); 
  ibuf = ibuf + 10;
  Serial.print(">ibuf.getValue() = ");
  Serial.println((int)ibuf.get() ); 
  cbuf = 'f';
  Serial.print(">cbuf.getValue() = ");
  Serial.println( (char)cbuf.get() );
  
#endif
 
}

//run repeatedly
void loop() {
#ifdef TEST_BASIC
  interp();
#endif
}



