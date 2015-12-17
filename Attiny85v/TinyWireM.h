#ifndef TinyWireM_h
#define TinyWireM_h

#include <inttypes.h>
#define USI_SEND         0              
#define USI_RCVE         1             
#define USI_BUF_SIZE    16             


void TinyWireM_begin();
void TinyWireM_beginTransmission(uint8_t);
void TinyWireM_send(uint8_t);
uint8_t TinyWireM_endTransmission();
uint8_t TinyWireM_requestFrom(uint8_t, uint8_t);
uint8_t TinyWireM_receive();
uint8_t TinyWireM_available();


#endif

