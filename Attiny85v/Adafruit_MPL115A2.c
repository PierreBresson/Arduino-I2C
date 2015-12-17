#include "Adafruit_MPL115A2.h"
#include <inttypes.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "TinyWireM.h"



static uint8_t i2cread(void) {
  uint8_t x;
  x = TinyWireM_receive();
  return x;
}



static void i2cwrite(uint8_t x) {
  TinyWireM_send((uint8_t)x);
}



void Adafruit_MPL115A2_readCoefficients() {
  int16_t a0coeff;
  int16_t b1coeff;
  int16_t b2coeff;
  int16_t c12coeff;

  TinyWireM_beginTransmission(MPL115A2_ADDRESS);
  i2cwrite((uint8_t)MPL115A2_REGISTER_A0_COEFF_MSB);
  TinyWireM_endTransmission();

  TinyWireM_requestFrom(MPL115A2_ADDRESS, 8);
  a0coeff = (( (uint16_t) i2cread() << 8) | i2cread());
  b1coeff = (( (uint16_t) i2cread() << 8) | i2cread());
  b2coeff = (( (uint16_t) i2cread() << 8) | i2cread());
  c12coeff = (( (uint16_t) (i2cread() << 8) | i2cread())) >> 2;

  _mpl115a2_a0 = (float)a0coeff / 8;
  _mpl115a2_b1 = (float)b1coeff / 8192;
  _mpl115a2_b2 = (float)b2coeff / 16384;
  _mpl115a2_c12 = (float)c12coeff;
  _mpl115a2_c12 /= 4194304.0;
}



void Adafruit_MPL115A2_init() {
  _mpl115a2_a0 = 0.0;
  _mpl115a2_b1 = 0.0;
  _mpl115a2_b2 = 0.0;
  _mpl115a2_c12 = 0.0;
}



void Adafruit_MPL115A2_begin() {
  TinyWireM_begin();
  Adafruit_MPL115A2_readCoefficients();
}



float Adafruit_MPL115A2_getPressure() {
  float     pressureComp,centigrade;
  Adafruit_MPL115A2_getPT(&pressureComp, &centigrade);
  return pressureComp;
}




float Adafruit_MPL115A2_getTemperature() {
  float     pressureComp, centigrade;
  Adafruit_MPL115A2_getPT(&pressureComp, &centigrade);
  return centigrade;
}



void Adafruit_MPL115A2_getPT(float *P, float *T) {
  uint16_t 	pressure, temp;
  float     pressureComp;

  TinyWireM_beginTransmission(MPL115A2_ADDRESS);
  i2cwrite((uint8_t)MPL115A2_REGISTER_STARTCONVERSION);
  i2cwrite((uint8_t)0x00);
  TinyWireM_endTransmission();

  _delay_ms(2);

  TinyWireM_beginTransmission(MPL115A2_ADDRESS);
  i2cwrite((uint8_t)MPL115A2_REGISTER_PRESSURE_MSB);
  TinyWireM_endTransmission();

  TinyWireM_requestFrom(MPL115A2_ADDRESS, 4);
  pressure = (( (uint16_t) i2cread() << 8) | i2cread()) >> 6;
  temp = (( (uint16_t) i2cread() << 8) | i2cread()) >> 6;

  pressureComp = _mpl115a2_a0 + (_mpl115a2_b1 + _mpl115a2_c12 * temp ) * pressure + _mpl115a2_b2 * temp;

  *P = ((65.0/ 1023.0) * pressureComp) + 50.0;       
  *T = ((float) temp - 498.0) / -5.35 +25.0;          
}