#ifndef TwoWire_h
#define TwoWire_h

#include <inttypes.h>

#define BUFFER_LENGTH 32

#define WIRE_HAS_END 1

static uint8_t rxBuffer[];
static uint8_t rxBufferIndex;
static uint8_t rxBufferLength;

static uint8_t txAddress;
static uint8_t txBuffer[];
static uint8_t txBufferIndex;
static uint8_t txBufferLength;

static uint8_t transmitting;
static void (*user_onRequest)(void);
static void (*user_onReceive)(int);
static void onRequestService(void);
static void onReceiveService(uint8_t*, int);


void TwoWire_begin();
void TwoWire_end();
void TwoWire_setClock(uint32_t);
void TwoWire_beginTransmission(int);
uint8_t TwoWire_endTransmission(uint8_t);

uint8_t TwoWire_requestFrom(int, int);
uint8_t TwoWire_write(uint8_t );

int TwoWire_available(void);
int TwoWire_read(void);
int TwoWire_peek(void);
void TwoWire_flush(void);
void onReceive( void (*)(int) );
void onRequest( void (*)(void) );


#endif

