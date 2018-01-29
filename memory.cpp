#include "memory.h"

_Heap::_Heap(){}

void _Heap::init()
{
  init(512);
}


void _Heap::init(const size_t n)
{
  unsigned int i;
  Wire.begin();
  Serial.print(">Heap: initalise memory: ");
  HeapSize = n * 1024;

  //clear eeprom
  for( i = 0; i < (HeapSize); i++ )
  {
    HeapWriteByte( (unsigned char)0xFF, i );
  }
  Serial.println("Done");
}

//return zero on failure, returns memory location on success
unsigned int _Heap::alloc( const size_t n )
{
  Serial.print("emalloc: n = ");
  Serial.println(n);
  Serial.print("nextSlot: ");Serial.println(nextSlot);
  unsigned int ret = nextSlot + n;
  if( (ret) < HeapSize){
     nextSlot += n + 1;
     Serial.print(" Next slot at "); Serial.println(ret); 
     return ret;
  }
  return 0;
}

//returns 0 on success
int _Heap::HeapWriteByte(unsigned char cdata,unsigned int address)
{
  //set address
  Wire.beginTransmission(EPROM_DEVICE);
  Wire.write((byte)(address >> 8)); //high byte
  Wire.write((byte)(address & 0xFF)); //low byte
  Wire.write(cdata);
  return (int)Wire.endTransmission();
}

unsigned char _Heap::HeapReadByte( unsigned int address)
{
  unsigned char cdata;
  
  //set address
  Wire.beginTransmission(EPROM_DEVICE);
  Wire.write((byte)(address >> 8)); //high byte
  Wire.write((byte)(address & 0xFF)); //low byte
  Wire.endTransmission();
  
  //get data
  Wire.requestFrom(EPROM_DEVICE,1);
  cdata = (unsigned char)Wire.read();
  
  return (unsigned char) cdata;
}


void _Heap::HeapWriteWord(unsigned int idata, unsigned int address)
{
  unsigned char data = (unsigned char)idata;
  unsigned char data2  = idata >> 8;

  //set address for 1st byte and send
  Wire.beginTransmission(EPROM_DEVICE);
  Wire.write((byte)(address >> 8)); //high byte
  Wire.write((byte)(address & 0xFF)); //low byte
  Wire.write(data);
  Wire.endTransmission();

  //set address for 2st byte and send
  Wire.beginTransmission(EPROM_DEVICE);
  Wire.write((byte)((address + 1) >> 8)); //high byte
  Wire.write((byte)(address & 0xFF)); //low byte
  Wire.write(data2);
  Wire.endTransmission();
  
}
unsigned int _Heap::HeapReadWord( unsigned int address)
{
  unsigned char data;
  unsigned char data2;
  unsigned int idata;
  
  //set address
  Wire.beginTransmission(EPROM_DEVICE);
  Wire.write((byte)(address >> 8)); //high byte
  Wire.write((byte)(address & 0xFF)); //low byte
  Wire.endTransmission();
  
  //get data
  Wire.requestFrom(EPROM_DEVICE,2);
  data = (unsigned char)Wire.read();
  data2 = (unsigned char)Wire.read();

  idata = (unsigned int)data;
  idata << 8;
  idata += (unsigned int)data2;
  return (unsigned int)idata;
}

//
_Heap Heap;
