/* 
 * File:   RGB.h
 * Author: Dave Coetzee
 * Created on September 19, 2023, 6:53 PM
 * RGB 64x32 screen 
 */

#ifndef RGB_H
#define	RGB_H

#include <stdbool.h>
#include <stdint.h>
#include <xc.h>

/////////////////////////////////////////////////
//defines
#define GFX_SCREEN_WIDTH  64
#define GFX_SCREEN_HEIGHT 32

#define NO_OF_SCREENS_WIDE 1
#define NO_OF_SCREENS_HIGH 1

#define GFX_CHAR_SPACING   1

#define RowSelectUnconventional
//#define NEW_SCREEN_ISR

////////////////////////////////////////////////
//macros
//one screen wide is 64 pix 2 wide is 64*2
#define SCREEN_WIDTH(x) ((GFX_SCREEN_WIDTH)*(x))
//one screen is divided into 2 halves, 2 screens must be 2 halves too
#define SCREEN_HALF(y) ((GFX_SCREEN_HEIGHT*GFX_SCREEN_WIDTH*y)/2)

////////////////////////////////////////////////
//variables
extern volatile uint16_t screen_width;
extern volatile uint16_t half_screen;
////////////////////////////////////////////////
//structs and enums
typedef enum pixelState{pixOff,pixOn}pixSwitch;
extern  pixSwitch pixState;

typedef enum Type{CHAR=-1,STRING=1,PIC=2,FILL,NOFILL}type;
extern type C_S;

typedef enum RGB_{
  none = -1,
  black = 0,
  red ,
  green,
  yellow,
  blue,
  magenta,
  cyan,
  white
}_RGB;
extern _RGB RGB;

typedef struct p6{
  uint8_t Data_Top_RGB;
  uint8_t Data_Top;
  uint8_t Data_Bot_RGB;
  uint8_t Data_Bot;
  uint8_t noOfPanelWide;
  uint8_t noOfPanelDeep;
  uint16_t bufferSize;
  uint8_t P_Vram[2][2048]; //64 as (8*8=32)
}P6_Var;
extern volatile P6_Var P6V;


///////////////////////////////////////////////
//types


extern bool oneShot;
extern unsigned char Letter;
///////////////////////////////////////////////
//function prototypes

void initP10(uint8_t wide);
void Int_Handler(void);
void writeData(uint8_t Dat_T,uint8_t Dat_B);
void BlkSelect(uint8_t ch);
void latchLeds(uint8_t yClk);
void runPixSeq();
void setPix(uint16_t x,uint16_t y,pixSwitch pixState);
void toggle_SCLK();
void toggle_SCLK_Inv();
void toggle_RCLK();
void toggle_RCLK_Inv();
void OE_enable();
void OE_disable();
void LAT_on();
void LAT_off();
void P10_DispOff();
void Channel(uint8_t ch);
void rowSelectConventional(uint8_t ch);
void rowSelectUnConventional(uint8_t ch);



void GFXGetCharInfo(char c);
void sortBuff();
int8_t GFXPutCharXY(int8_t x,int8_t y,char c, type t);
int8_t GFXWriteStringXY(int8_t x,int8_t y, char *string,type C_S,_RGB RGB);
void GFXGetCharInfo(char c);
int16_t GFXGetStringWidthN( char *string,uint8_t n);
int8_t CharIndexOfPixel( char *string, uint16_t pixel);
void ScrollMsg(char *msg);
void fillPicture(const uint8_t *Pict);
void DRAW_Rect(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,type t);
void DRAW_Line(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1);
void fillTop();
void fillBot();
void fillPix();
void clearPix();
void setCol(_RGB RGB);
int GetCol();
int GFXWriteStringPos(char *string);



#endif	/* RGB_H */

