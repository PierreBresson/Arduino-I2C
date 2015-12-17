#include "LiquidTWI.h"
#include "TinyWireM.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <util/delay.h>

#define LOW 0x0
#define HIGH 0x1
#define INPUT 0x0
#define OUTPUT 0x1
#define NOT_A_PIN 0
#define NOT_A_PORT 0
#define INPUT_PULLUP 0x2

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))



void LiquidTWI_init(uint8_t i2cAddr) {
	if (i2cAddr > 7) i2cAddr = 7;
	_i2cAddr = i2cAddr;
	_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS; 
}



void LiquidTWI_begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
	_delay_ms(50);

	TinyWireM_begin();

	TinyWireM_beginTransmission(MCP23008_ADDRESS | _i2cAddr);

	TinyWireM_send( MCP23008_IODIR);
	TinyWireM_send( 0xFF);
	TinyWireM_send( 0x00);
	TinyWireM_send( 0x00);
	TinyWireM_send( 0x00);
	TinyWireM_send( 0x00);
	TinyWireM_send( 0x00);
	TinyWireM_send( 0x00);
	TinyWireM_send( 0x00);
	TinyWireM_send( 0x00);
	TinyWireM_send( 0x00);

	TinyWireM_endTransmission();


	TinyWireM_beginTransmission(MCP23008_ADDRESS | _i2cAddr);
	
	TinyWireM_send( MCP23008_IODIR);
	TinyWireM_send( 0x00); 

	TinyWireM_endTransmission();

	if (lines > 1) {
		_displayfunction |= LCD_2LINE;
	}
	_numlines = lines;
	_currline = 0;


	if ((dotsize != 0) && (lines == 1)) {
		_displayfunction |= LCD_5x10DOTS;
	}

	LiquidTWI_burstBits(0b10011100); 
	LiquidTWI_burstBits(0b10011000); 
	LiquidTWI_burstBits(0b10011100); 
	LiquidTWI_burstBits(0b10011000); 
	LiquidTWI_burstBits(0b10011100);
	LiquidTWI_burstBits(0b10011000);
	LiquidTWI_burstBits(0b10010100);
	LiquidTWI_burstBits(0b10010000);
	_delay_ms(5);
	
	LiquidTWI_command(LCD_FUNCTIONSET | _displayfunction); 
	_delay_ms(5);
	LiquidTWI_command(LCD_FUNCTIONSET | _displayfunction); 
	_delay_ms(5); 

	_displaycontrol = LCD_DISPLAYON;  
	LiquidTWI_display();
	LiquidTWI_clear();

	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	LiquidTWI_command(LCD_ENTRYMODESET | _displaymode);

	LiquidTWI_setBacklight(HIGH); 
}



void LiquidTWI_clear()
{
	LiquidTWI_command(LCD_CLEARDISPLAY);  
	_delay_ms(2000); 
}



void LiquidTWI_home()
{
	LiquidTWI_command(LCD_RETURNHOME); 
	_delay_ms(2000); 
}



void LiquidTWI_setCursor(uint8_t col, uint8_t row)
{
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if ( row > _numlines ) row = _numlines - 1;   
	LiquidTWI_command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}



void LiquidTWI_noDisplay() {
	_displaycontrol &= ~LCD_DISPLAYON;
	LiquidTWI_command(LCD_DISPLAYCONTROL | _displaycontrol);
}



void LiquidTWI_display() {
	_displaycontrol |= LCD_DISPLAYON;
	LiquidTWI_command(LCD_DISPLAYCONTROL | _displaycontrol);
}



void LiquidTWI_noCursor() {
	_displaycontrol &= ~LCD_CURSORON;
	LiquidTWI_command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidTWI_cursor() {
	_displaycontrol |= LCD_CURSORON;
	LiquidTWI_command(LCD_DISPLAYCONTROL | _displaycontrol);
}



void LiquidTWI_noBlink() {
	_displaycontrol &= ~LCD_BLINKON;
	LiquidTWI_command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidTWI_blink() {
	_displaycontrol |= LCD_BLINKON;
	LiquidTWI_command(LCD_DISPLAYCONTROL | _displaycontrol);
}



void LiquidTWI_scrollDisplayLeft(void) {
	LiquidTWI_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void LiquidTWI_scrollDisplayRight(void) {
	LiquidTWI_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}



void LiquidTWI_leftToRight(void) {
	_displaymode |= LCD_ENTRYLEFT;
	LiquidTWI_command(LCD_ENTRYMODESET | _displaymode);
}



void LiquidTWI_rightToLeft(void) {
	_displaymode &= ~LCD_ENTRYLEFT;
	LiquidTWI_command(LCD_ENTRYMODESET | _displaymode);
}



void LiquidTWI_autoscroll(void) {
	_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	LiquidTWI_command(LCD_ENTRYMODESET | _displaymode);
}



void LiquidTWI_noAutoscroll(void) {
	_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	LiquidTWI_command(LCD_ENTRYMODESET | _displaymode);
}



void LiquidTWI_createChar(uint8_t location, uint8_t charmap[]) {
	location &= 0x7; // we only have 8 locations 0-7
	LiquidTWI_command(LCD_SETCGRAMADDR | (location << 3));
	for (int i=0; i<8; i++) {
		LiquidTWI_write(charmap[i]);
	}
}



inline void LiquidTWI_command(uint8_t value) {
	LiquidTWI_send(value, LOW);
}



void LiquidTWI_write(uint8_t value) {
	LiquidTWI_send(value, HIGH);
}



void LiquidTWI_setBacklight(uint8_t status) {
	bitWrite(_displaycontrol,3,status); 
	LiquidTWI_burstBits((_displaycontrol & LCD_BACKLIGHT)?0x80:0x00);
}



void LiquidTWI_send(uint8_t value, uint8_t mode) {
	char buf;
	buf = (value & 0b11110000) >> 1; 
	if (mode){
		buf |= 3 << 1;
	} else { 
		buf |= 2 << 1; 
	}
	buf |= (_displaycontrol & LCD_BACKLIGHT)?0x80:0x00; 
	LiquidTWI_burstBits(buf); 
	buf &= ~(1<<2);
	LiquidTWI_burstBits(buf);
	buf = (value & 0b1111) << 3; 
	if (mode) {
		buf |= 3 << 1;
	} else {
		buf |= 2 << 1; 
	}
	buf |= (_displaycontrol & LCD_BACKLIGHT)?0x80:0x00; 
	LiquidTWI_burstBits(buf);
	buf &= ~( 1 << 2 ); 
	LiquidTWI_burstBits(buf);
}



void LiquidTWI_burstBits(uint8_t value) {
	TinyWireM_beginTransmission(MCP23008_ADDRESS | _i2cAddr);

	TinyWireM_send(MCP23008_GPIO);
	TinyWireM_send(value); 

	while (TinyWireM_endTransmission()) ;
}



uint8_t LiquidTWI_print(char str[]){
	uint8_t n = 0;
	for(uint16_t i = 0; str[i] != '\0'; i++){
		LiquidTWI_send( str[i],1);
	}

	return n;
}