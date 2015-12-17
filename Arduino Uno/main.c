#define F_CPU 16000000UL

#include "LiquidTWI.h"
#include "Adafruit_MPL115A2.h"
#include "Wire.h"

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

float pressureKPA = 0, temperatureC = 0 ;

void main (void){
	char temp[80],press[80];

	sei();

	LiquidTWI_init(0);
	LiquidTWI_begin(16,2,0);

	Adafruit_MPL115A2_init();
	Adafruit_MPL115A2_begin();

	while(1){
		Adafruit_MPL115A2_getPT(&pressureKPA, &temperatureC);
		pressureKPA = Adafruit_MPL115A2_getPressure();
		itoa((int)temperatureC, temp, 10);
		_delay_ms(20);
		LiquidTWI_setCursor(0,0);
		LiquidTWI_print("Temp :");
		LiquidTWI_setCursor(6,0);
		LiquidTWI_print(temp);
		_delay_ms(20);
		itoa((int)(pressureKPA*10), press, 10);
		LiquidTWI_setCursor(0,1);
		LiquidTWI_print("Press :");
		LiquidTWI_setCursor(7,1);
		LiquidTWI_print(press);
	}
}
