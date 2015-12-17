#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <avr/io.h>

#include "twi.h"
#include "Wire.h"


static uint8_t  rxBuffer[BUFFER_LENGTH];
static uint8_t  rxBufferIndex = 0;
static uint8_t  rxBufferLength = 0;

static uint8_t  txAddress = 0;
static uint8_t  txBuffer[BUFFER_LENGTH];
static uint8_t  txBufferIndex = 0;
static uint8_t  txBufferLength = 0;

static uint8_t  transmitting = 0;
static void (*user_onRequest)(void);
static void (*user_onReceive)(int);



void  TwoWire_begin(void)
{
  rxBufferIndex = 0;
  rxBufferLength = 0;

  txBufferIndex = 0;
  txBufferLength = 0;

  twi_init();
}



void  TwoWire_end(void)
{
  twi_disable();
}



void  TwoWire_setClock(uint32_t frequency)
{
  TWBR = ((F_CPU / frequency) - 16) / 2;
}



uint8_t  requestFrom(uint8_t address, uint8_t quantity, uint32_t iaddress, uint8_t isize, uint8_t sendStop)
{
  if (isize > 0) {
  TwoWire_beginTransmission(address);

  if (isize > 3){
    isize = 3;
  }

  while (isize-- > 0) {
	  TwoWire_write((uint8_t)(iaddress >> (isize*8)));
  }

  TwoWire_endTransmission(0);
  }

  if(quantity > BUFFER_LENGTH){
    quantity = BUFFER_LENGTH;
  }

  uint8_t read = twi_readFrom(address, rxBuffer, quantity, sendStop);

  rxBufferIndex = 0;
  rxBufferLength = read;

  return read;
}



uint8_t  TwoWire_requestFrom(int address, int quantity)
{
	return requestFrom((uint8_t)address, (uint8_t)quantity, (uint32_t)0, (uint8_t)0, (uint8_t)1);
}



void  beginTransmission(uint8_t address)
{
  transmitting = 1;
  txAddress = address;
  txBufferIndex = 0;
  txBufferLength = 0;
}



void  TwoWire_beginTransmission(int address)
{
  beginTransmission((uint8_t)address);
}



uint8_t  TwoWire_endTransmission(uint8_t sendStop)
{
  uint8_t ret = twi_writeTo(txAddress, txBuffer, txBufferLength, 1, sendStop);

  txBufferIndex = 0;
  txBufferLength = 0;

  transmitting = 0;
  return ret;
}



uint8_t  TwoWire_write(uint8_t data)
{
  if(transmitting){
    if(txBufferLength >= BUFFER_LENGTH){
      return 0;
    }
    txBuffer[txBufferIndex] = data;
    ++txBufferIndex;
    txBufferLength = txBufferIndex;
  }else{
    twi_transmit(&data, 1);
  }
  return 1;
}



int  TwoWire_available(void)
{
  return rxBufferLength - rxBufferIndex;
}



int  TwoWire_read(void)
{
  int value = -1;
  
  if(rxBufferIndex < rxBufferLength){
    value = rxBuffer[rxBufferIndex];
    ++rxBufferIndex;
  }

  return value;
}



int  TwoWire_peek(void)
{
  int value = -1;
  
  if(rxBufferIndex < rxBufferLength){
    value = rxBuffer[rxBufferIndex];
  }

  return value;
}



void  TwoWire_flush(void)
{

}



void  onReceiveService(uint8_t* inBytes, int numBytes)
{
  if(!user_onReceive){
    return;
  }
  if(rxBufferIndex < rxBufferLength){
    return;
  }
  for(uint8_t i = 0; i < numBytes; ++i){
    rxBuffer[i] = inBytes[i];    
  }
  rxBufferIndex = 0;
  rxBufferLength = numBytes;
  user_onReceive(numBytes);
}



void  onRequestService(void)
{
  if(!user_onRequest){
    return;
  }

  txBufferIndex = 0;
  txBufferLength = 0;

  user_onRequest();
}



void  onReceive( void (*function)(int) )
{
  user_onReceive = function;
}



void  onRequest( void (*function)(void) )
{
  user_onRequest = function;
}
