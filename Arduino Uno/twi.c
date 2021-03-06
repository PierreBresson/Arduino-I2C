#include <math.h>
#include <stdlib.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <compat/twi.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#include "twi.h"

static volatile uint8_t twi_state;
static volatile uint8_t twi_slarw;
static volatile uint8_t twi_sendStop;
static volatile uint8_t twi_inRepStart;

static void (*twi_onSlaveTransmit)(void);
static void (*twi_onSlaveReceive)(uint8_t*, int);

static uint8_t twi_masterBuffer[TWI_BUFFER_LENGTH];
static volatile uint8_t twi_masterBufferIndex;
static volatile uint8_t twi_masterBufferLength;

static uint8_t twi_txBuffer[TWI_BUFFER_LENGTH];
static volatile uint8_t twi_txBufferIndex;
static volatile uint8_t twi_txBufferLength;

static uint8_t twi_rxBuffer[TWI_BUFFER_LENGTH];
static volatile uint8_t twi_rxBufferIndex;

static volatile uint8_t twi_error;




void twi_init(void)
{
  twi_state = TWI_READY;
  twi_sendStop = 1;
  twi_inRepStart = 0;
  
  PORTC |= 1 << PORTC4;
  PORTC |= 1 << PORTC5;

  cbi(TWSR, TWPS0);
  cbi(TWSR, TWPS1);
  TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;

  TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA);
}



void twi_disable(void)
{
  TWCR &= ~(_BV(TWEN) | _BV(TWIE) | _BV(TWEA));

  PORTC |= 0 << PORTC4;
  PORTC |= 0 << PORTC5;
}



void twi_setAddress(uint8_t address)
{
  TWAR = address << 1;
}



uint8_t twi_readFrom(uint8_t address, uint8_t* data, uint8_t length, uint8_t sendStop)
{
  uint8_t i;

  if(TWI_BUFFER_LENGTH < length){
    return 0;
  }

  while(TWI_READY != twi_state){
    continue;
  }
  twi_state = TWI_MRX;
  twi_sendStop = sendStop;
  twi_error = 0xFF;

  twi_masterBufferIndex = 0;
  twi_masterBufferLength = length-1; 

  twi_slarw = TW_READ;
  twi_slarw |= address << 1;

  if (1 == twi_inRepStart) {
    twi_inRepStart = 0;
    do {
      TWDR = twi_slarw;
    } while(TWCR & _BV(TWWC));
    TWCR = _BV(TWINT) | _BV(TWEA) | _BV(TWEN) | _BV(TWIE);
  }
  else
    TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT) | _BV(TWSTA);
  while(TWI_MRX == twi_state){
    continue;
  }

  if (twi_masterBufferIndex < length)
    length = twi_masterBufferIndex;

  for(i = 0; i < length; ++i){
    data[i] = twi_masterBuffer[i];
  }
	
  return length;
}



uint8_t twi_writeTo(uint8_t address, uint8_t* data, uint8_t length, uint8_t wait, uint8_t sendStop)
{
  uint8_t i;

  if(TWI_BUFFER_LENGTH < length){
    return 1;
  }

  while(TWI_READY != twi_state){
    continue;
  }
  twi_state = TWI_MTX;
  twi_sendStop = sendStop;
  twi_error = 0xFF;

  twi_masterBufferIndex = 0;
  twi_masterBufferLength = length;
  
  for(i = 0; i < length; ++i){
    twi_masterBuffer[i] = data[i];
  }
  
  twi_slarw = TW_WRITE;
  twi_slarw |= address << 1;
  
  if (1 == twi_inRepStart) {

    twi_inRepStart = 0;
    do {
      TWDR = twi_slarw;				
    } while(TWCR & _BV(TWWC));
    TWCR = _BV(TWINT) | _BV(TWEA) | _BV(TWEN) | _BV(TWIE);
  }
  else
    TWCR = _BV(TWINT) | _BV(TWEA) | _BV(TWEN) | _BV(TWIE) | _BV(TWSTA);

  while(wait && (TWI_MTX == twi_state)){
    continue;
  }
  
  if (twi_error == 0xFF)
    return 0;
  else if (twi_error == TW_MT_SLA_NACK)
    return 2;
  else if (twi_error == TW_MT_DATA_NACK)
    return 3;
  else
    return 4;
}



uint8_t twi_transmit(const uint8_t* data, uint8_t length)
{
  uint8_t i;

  if(TWI_BUFFER_LENGTH < length){
    return 1;
  }
  
  if(TWI_STX != twi_state){
    return 2;
  }
  
  twi_txBufferLength = length;
  for(i = 0; i < length; ++i){
    twi_txBuffer[i] = data[i];
  }
  
  return 0;
}



void twi_attachSlaveRxEvent( void (*function)(uint8_t*, int) )
{
  twi_onSlaveReceive = function;
}



void twi_attachSlaveTxEvent( void (*function)(void) )
{
  twi_onSlaveTransmit = function;
}



void twi_reply(uint8_t ack)
{

  if(ack){
    TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | _BV(TWEA);
  }else{
	  TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT);
  }
}



void twi_stop(void)
{
  TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT) | _BV(TWSTO);

  while(TWCR & _BV(TWSTO)){
    continue;
  }

  twi_state = TWI_READY;
}



void twi_releaseBus(void)
{
  TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT);

  twi_state = TWI_READY;
}



ISR(TWI_vect)
{
  switch(TW_STATUS){
    case TW_START:   
    case TW_REP_START: 
      TWDR = twi_slarw;
      twi_reply(1);
      break;
    case TW_MT_SLA_ACK:
    case TW_MT_DATA_ACK:
      if(twi_masterBufferIndex < twi_masterBufferLength){
        TWDR = twi_masterBuffer[twi_masterBufferIndex++];
        twi_reply(1);
      }else{
        if (twi_sendStop)
          twi_stop();
        else {
      	  twi_inRepStart = 1;	
      	  TWCR = _BV(TWINT) | _BV(TWSTA)| _BV(TWEN) ;
      	  twi_state = TWI_READY;
        }
      }
      break;
    case TW_MT_SLA_NACK: 
      twi_error = TW_MT_SLA_NACK;
      twi_stop();
      break;
    case TW_MT_DATA_NACK:
      twi_error = TW_MT_DATA_NACK;
      twi_stop();
      break;
    case TW_MT_ARB_LOST:
      twi_error = TW_MT_ARB_LOST;
      twi_releaseBus();
      break;
    case TW_MR_DATA_ACK: 
      twi_masterBuffer[twi_masterBufferIndex++] = TWDR;
      break;
    case TW_MR_SLA_ACK:
      if(twi_masterBufferIndex < twi_masterBufferLength){
        twi_reply(1);
      }else{
        twi_reply(0);
      }
      break;
    case TW_MR_DATA_NACK:
      twi_masterBuffer[twi_masterBufferIndex++] = TWDR;
      if (twi_sendStop)
          twi_stop();
      else {
        twi_inRepStart = 1;
        TWCR = _BV(TWINT) | _BV(TWSTA)| _BV(TWEN) ;
        twi_state = TWI_READY;
      }    
      break;
    case TW_MR_SLA_NACK:
      twi_stop();
      break;
    case TW_SR_SLA_ACK:   
    case TW_SR_GCALL_ACK:
    case TW_SR_ARB_LOST_SLA_ACK:  
    case TW_SR_ARB_LOST_GCALL_ACK:
      twi_state = TWI_SRX;
      twi_rxBufferIndex = 0;
      twi_reply(1);
      break;
    case TW_SR_DATA_ACK:       
    case TW_SR_GCALL_DATA_ACK: 
      if(twi_rxBufferIndex < TWI_BUFFER_LENGTH){
        twi_rxBuffer[twi_rxBufferIndex++] = TWDR;
        twi_reply(1);
      }else{
        twi_reply(0);
      }
      break;
    case TW_SR_STOP: 
      twi_releaseBus();
      if(twi_rxBufferIndex < TWI_BUFFER_LENGTH){
        twi_rxBuffer[twi_rxBufferIndex] = '\0';
      }
      twi_onSlaveReceive(twi_rxBuffer, twi_rxBufferIndex);
      twi_rxBufferIndex = 0;
      break;
    case TW_SR_DATA_NACK:       
    case TW_SR_GCALL_DATA_NACK:
      twi_reply(0);
      break;
    case TW_ST_SLA_ACK:          
    case TW_ST_ARB_LOST_SLA_ACK:
      twi_state = TWI_STX;
      twi_txBufferIndex = 0;
      twi_txBufferLength = 0;
      twi_onSlaveTransmit();
      if(0 == twi_txBufferLength){
        twi_txBufferLength = 1;
        twi_txBuffer[0] = 0x00;
      }
      break;
    case TW_ST_DATA_ACK: 
      TWDR = twi_txBuffer[twi_txBufferIndex++];
      if(twi_txBufferIndex < twi_txBufferLength){
        twi_reply(1);
      }else{
        twi_reply(0);
      }
      break;
    case TW_ST_DATA_NACK: 
    case TW_ST_LAST_DATA: 
      twi_reply(1);
      twi_state = TWI_READY;
      break;
    case TW_NO_INFO:   
      break;
    case TW_BUS_ERROR:
      twi_error = TW_BUS_ERROR;
      twi_stop();
      break;
    default:
      break;
  }
}

