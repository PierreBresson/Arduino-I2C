#ifndef LiquidTWI_h
#define LiquidTWI_h

#include <inttypes.h>

#define MCP23008_ADDRESS 0x20

#define MCP23008_IODIR 0x00
#define MCP23008_IPOL 0x01
#define MCP23008_GPINTEN 0x02
#define MCP23008_DEFVAL 0x03
#define MCP23008_INTCON 0x04
#define MCP23008_IOCON 0x05
#define MCP23008_GPPU 0x06
#define MCP23008_INTF 0x07
#define MCP23008_INTCAP 0x08
#define MCP23008_GPIO 0x09
#define MCP23008_OLAT 0x0A

#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00
#define LCD_BACKLIGHT 0x08 

#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00


void LiquidTWI_init(uint8_t i2cAddr);

void LiquidTWI_begin(uint8_t cols, uint8_t rows, uint8_t charsize );

void LiquidTWI_clear();
void LiquidTWI_home();

void LiquidTWI_noDisplay();
void LiquidTWI_display();
void LiquidTWI_noBlink();
void LiquidTWI_blink();
void LiquidTWI_noCursor();
void LiquidTWI_cursor();
void LiquidTWI_scrollDisplayLeft();
void LiquidTWI_scrollDisplayRight();
void LiquidTWI_leftToRight();
void LiquidTWI_rightToLeft();
void LiquidTWI_autoscroll();
void LiquidTWI_noAutoscroll();

void LiquidTWI_setBacklight(uint8_t status); 

void LiquidTWI_createChar(uint8_t, uint8_t[]);
void LiquidTWI_setCursor(uint8_t, uint8_t); 

void LiquidTWI_write(uint8_t);

void LiquidTWI_command(uint8_t);

void LiquidTWI_send(uint8_t, uint8_t);
void LiquidTWI_burstBits(uint8_t);


uint8_t LiquidTWI_print(char[]);

uint8_t _displayfunction;
uint8_t _displaycontrol;
uint8_t _displaymode;
uint8_t _numlines,_currline;
uint8_t _i2cAddr;

#endif
