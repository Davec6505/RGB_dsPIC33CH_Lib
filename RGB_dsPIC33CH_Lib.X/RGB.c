#include "RGB.h"

#include <string.h>
#include <stdlib.h>
#include "../RGB_dsPIC33CH_Lib.X/mcc_generated_files/delay.h"
#include "../RGB_dsPIC33CH_Lib.X/mcc_generated_files/pin_manager.h"
#include "../../XC_GFX_Lib/GFX_Lib.X/GFX.h"
#include "../../XC_GFX_Lib/GFX_Lib.X/Fonts.h"

#define TOP_MASK 0xFE3F
#define BOT_MASK 0xF1FF

#define  RowA _LATA0
#define  RowB _LATA1
#define  RowC _LATA2
#define  RowD _LATA3
#define  SR_R1 _LATB6
#define  SR_G1 _LATB7
#define  SR_B1 _LATB8
#define  SR_R2 _LATB9
#define  SR_G2 _LATB10
#define  SR_B2 _LATB11

pixSwitch pixState;//enum for pixel on or off condition
volatile P6_Var P6V;
_RGB RGB;

struct FontArr fontInfo;

volatile uint16_t screen_width;
volatile uint16_t half_screen;

//static int16_t i,j,k;
//static uint8_t rowNo;
static uint16_t iX,iL;//,iBlk,iK,iY
extern uint8_t txt[5];

////////////////////////////////////////////////////////////////////////////////
//P10 initialize module connections
void initP10(uint8_t wide){

   iX = iL = 0;
   //set the number of screens tied together
   screen_width = SCREEN_WIDTH(wide);
   half_screen  = SCREEN_HALF(wide);
}

////////////////////////////////////////////////////////////////////////////////
//interrupt handler interrupts must be turned on to allow buffer setup
void Int_Handler(void){
static uint8_t i1;
      i1 = 0;
      __builtin_disable_interrupts();
      TMR1_Test_SetLow();
      while(i1 < 16){
           for(iX=0;iX<64;iX++){     //iX = 32 leds wide
               writeData(P6V.P_Vram[0][iX+(i1*64)],P6V.P_Vram[0][(iX+(i1*64))+1024]);
               #ifdef RowSelectUnconventional
               toggle_SCLK();        //toggle each bit into SR
               #else
               toggle_SCLK_Inv();
               #endif
           }
           latchLeds(i1++);

      }
      TMR1_Test_SetHigh();
      __builtin_enable_interrupts();
}

////////////////////////////////////////////////////////////////////////////////
//write the DATA to the shift register MSB first
void writeData(uint8_t Dat_T,uint8_t Dat_B){
//static int16_t iDat;

      if(Dat_T & 0x7){
           LATB  = (LATB & TOP_MASK)| ((uint16_t)Dat_T << 6);// P6V.Data_Top_RGB;
      }
      else
           LATB  = LATB & TOP_MASK;

     if(Dat_B & 0x38)
           LATB  = (LATB & BOT_MASK)| ((uint16_t)Dat_B << 6);  //check if Dat MSB is high
     else 
           LATB  = LATB & BOT_MASK;
}

////////////////////////////////////////////////////////////////////////////////
//set one pixel fonts and bitmaps
void setPix(uint16_t x,uint16_t y,pixSwitch pixState){
static unsigned int xx,yy,tempY;
int xOffSet;

       x = x - 1;
       tempY = y - 1;
       
       if(x<0 || x>64 || y<0 || y>32)return;
       if(y > 16)tempY =  tempY - 16; //16-16=0

       if((tempY > 3)&&(tempY < 8))xOffSet = 4;
       else if((tempY > 7)&&(tempY < 12))xOffSet = -4;
       else xOffSet = 0;

       
       if(y > 16)yy = tempY + xOffSet + 16;
       else yy =  tempY  + xOffSet;

       xx = x +(yy * 64);


       if(y <= 16){
          if(pixState == 1) P6V.P_Vram[0][xx] = P6V.Data_Top_RGB & 0x07;//set color of pix
          else P6V.P_Vram[0][xx] = P6V.Data_Top_RGB & 0x00;             //clear the pixel
       }
       if(y > 16){
          if(pixState == 1) P6V.P_Vram[0][xx] = P6V.Data_Bot_RGB  & 0x38;//set color of pix
          else  P6V.P_Vram[0][xx] = P6V.Data_Bot_RGB  & 0x00;            //clear the pixel
       }
}

////////////////////////////////////////////////////////////////////////////////
//Latch Leds to output of SR
void latchLeds(uint8_t yClk){
    OE_disable(); //=1
    
   #ifdef RowSelectUnconventional
     rowSelectUnConventional(yClk);
     toggle_RCLK();
   #else
     rowSelectConventional(yClk);
     toggle_RCLK_Inv();
   #endif
    OE_enable(); //=0

}
////////////////////////////////////////////////////////////////////////////////
//pulse the shift clock to enter data
void toggle_SCLK(){
      CLK_SetLow();// = 0;
      asm("NOP");    //ignore 1 cycle
      CLK_SetHigh();// = 1;
}
void toggle_SCLK_Inv(){
      CLK_SetHigh();// = 1;
      asm("NOP");   //ignore 1 cycle
      CLK_SetLow();//= 0;
}
////////////////////////////////////////////////////////////////////////////////
//pulse the dtore clock to latch the DATA to the output buffer
void toggle_RCLK(){
      LAT_SetLow();// = 0;
      asm("NOP");     //1 cycle ignore;
      LAT_SetHigh();// = 1;
}
void toggle_RCLK_Inv(){
      LAT_SetHigh();// = 1;
      asm("NOP");     //1 cycle ignore;
      LAT_SetLow();// = 0;
}
////////////////////////////////////////////////////////////////////////////////
//control latch independantly
void LAT_on(){
      LAT_SetHigh();//= 1;
}
void LAT_off(){
     LAT_SetLow();// = 0;
}
////////////////////////////////////////////////////////////////////////////////
//OE enable the output
void OE_enable(){
   OE_SetLow();// = 0;
}
////////////////////////////////////////////////////////////////////////////////
//OE disable the outputs
void OE_disable(){
  OE_SetHigh();// = 1;//~SR_OE;
}
////////////////////////////////////////////////////////////////////////////////
//P10 Display off
void P10_DispOff(){
    OE_disable();
}

////////////////////////////////////////////////////////////////////////////////
//channel select inline 1-2-3-4-5-6....
void rowSelectConventional(uint8_t ch){
        switch(ch){                                                  /**Blk 1**/
            case 0:RowA  = 0;RowB = 0;RowC = 0;RowD = 0;//lines 1,8
                   break;
            case 1:RowA  = 1;RowB = 0;RowC = 0;RowD = 0;//lines 2,9
                   break;
            case 2:RowA  = 0;RowB = 1;RowC = 0;RowD = 0;//lines 3,10
                   break;
            case 3:RowA  = 1;RowB = 1;RowC = 0;RowD = 0;//lines 4,11
                   break;                                            /**Blk 3**/
            case 4:RowA  = 0;RowB = 0;RowC = 1;RowD = 0;//lines 9,20
                   break;
            case 5:RowA  = 1;RowB = 0;RowC = 1;RowD = 0;//lines 10,21
                   break;
            case 6:RowA  = 0;RowB = 1;RowC = 1;RowD = 0;//lines 11,22
                   break;
            case 7:RowA  = 1;RowB = 1;RowC = 1;RowD = 0;//lines 12,23
                   break;                                            /**Blk 2**/
            case 8:RowA  = 0;RowB = 0;RowC = 0;RowD = 1;//lines 5,24
                   break;
            case 9:RowA  = 1;RowB = 0;RowC = 0;RowD = 1;//lines 6,25
                   break;
            case 10:RowA = 0;RowB = 1;RowC = 0;RowD = 1;//lines 7,26
                   break;
            case 11:RowA = 1;RowB = 1;RowC = 0;RowD = 1;//lines 8,27
                   break;                                           /**Blk 4**/
            case 12:RowA = 0;RowB = 0;RowC = 1;RowD = 1;//lines 13,28
                   break;
            case 13:RowA = 1;RowB = 0;RowC = 1;RowD = 1;//lines 14,29
                   break;
            case 14:RowA = 0;RowB = 1;RowC = 1;RowD = 1;//lines 15,30
                   break;
            case 15:RowA = 1;RowB = 1;RowC = 1;RowD = 1;//lines 16,31
                   break;
            default : ch = 0;
                   break;
        }
}
////////////////////////////////////////////////////////////////////////////////
//channel select odd 1-2-3-4-9-10-11-12-5-6-7-8-13-14-15-16
void rowSelectUnConventional(uint8_t ch){
        switch(ch){                                                  /**Blk 1**/
            case 0:RowA  = 0;RowB = 0;RowC = 0;RowD = 0;//lines 1,8
                   break;
            case 1:RowA  = 1;RowB = 0;RowC = 0;RowD = 0;//lines 2,9
                   break;
            case 2:RowA  = 0;RowB = 1;RowC = 0;RowD = 0;//lines 3,10
                   break;
            case 3:RowA  = 1;RowB = 1;RowC = 0;RowD = 0;//lines 4,11
                   break;                                            /**Blk 3**/
            case 4:RowA  = 0;RowB = 0;RowC = 0;RowD = 1;//lines 9,20
                   break;
            case 5:RowA  = 1;RowB = 0;RowC = 0;RowD = 1;//lines 10,21
                   break;
            case 6:RowA  = 0;RowB = 1;RowC = 0;RowD = 1;//lines 11,22
                   break;
            case 7:RowA  = 1;RowB = 1;RowC = 0;RowD = 1;//lines 12,23
                   break;                                            /**Blk 2**/
            case 8:RowA  = 0;RowB = 0;RowC = 1;RowD = 0;//lines 5,24
                   break;
            case 9:RowA  = 1;RowB = 0;RowC = 1;RowD = 0;//lines 6,25
                   break;
            case 10:RowA = 0;RowB = 1;RowC = 1;RowD = 0;//lines 7,26
                   break;
            case 11:RowA = 1;RowB = 1;RowC = 1;RowD = 0;//lines 8,27
                   break;                                           /**Blk 4**/
            case 12:RowA = 0;RowB = 0;RowC = 1;RowD = 1;//lines 13,28
                   break;
            case 13:RowA = 1;RowB = 0;RowC = 1;RowD = 1;//lines 14,29
                   break;
            case 14:RowA = 0;RowB = 1;RowC = 1;RowD = 1;//lines 15,30
                   break;
            case 15:RowA = 1;RowB = 1;RowC = 1;RowD = 1;//lines 16,31
                   break;
            default : ch = 0;
                   break;
        }
}


////////////////////////////////////////////////////////////////////////////////
//                            LINEAR BUFFER CONDITIONING                      //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//CHAR conditioning and setting data to place into buffer from setPix function
int8_t GFXPutCharXY(int8_t x,int8_t y,char c,type t){
//static bool error;
uint16_t bytes;//,i, dta_;
uint8_t _x,_y,fx,b;//fy;
       c     = c & 0x7F;
       if(t == CHAR){
           clearPix();
           GFXGetCharInfo(c);
       }
        //info comes from mikrochips advanced font appnote AN1182
        bytes =  (fontInfo.width+7)/8;
         if(c<fontInfo.FirstChar || c>=(fontInfo.FirstChar+fontInfo.CharCount)){
             return -1; //error report
         }

         for(_y=0;_y<(fontInfo.CharHeight*bytes);_y++){
             b = 0;
             if(y>(GFX_SCREEN_HEIGHT - fontInfo.CharHeight) || y < 0 || x>(GFX_SCREEN_WIDTH - fontInfo.width) || x<0){
                if(x>(GFX_SCREEN_WIDTH - fontInfo.width))break;
                x=27;y=25;c='?';
                GFXGetCharInfo(c);
                bytes =  (fontInfo.width+7)/8;
             }
             for (_x = 0; _x < bytes; _x++) {
                 fontInfo.Data =  _GFXReadFontData(fontInfo.BMP_Pos+_y+_x);
                 for(fx = 0;fx<8;fx++){
                    if (fontInfo.Data & (1 << fx))setPix(x+fx+b,y+(_y/bytes),pixOn);
                    else setPix(x+fx+b,y+(_y/bytes),pixOff);
                 }
                 if(bytes > 1) b = 8;
             }
             if(bytes>1)_y++;
         }

         return 1; //1 = no error
}
////////////////////////////////////////////////////////////////////////////////
//string function
int8_t GFXWriteStringXY(int8_t x,int8_t y, char *string,type C_S,_RGB RGB){
short w;
      //test the STRING to see if it is empty then set to black

     if(RGB != none) setCol(RGB);
          
      while(*string != '\n'){
            GFXGetCharInfo(*string);
            w = fontInfo.width;
            if(GFXPutCharXY(x,y,*string,STRING)==-1) return -1;
            x+=w;
            x+=GFX_CHAR_SPACING;
            string++;
      }
      return 1;
}
////////////////////////////////////////////////////////////////////////////////
//get the char width
void GFXGetCharInfo(char c){
        fontInfo.CharHeight  = _GFXGetFontHeight();
        fontInfo.FirstChar   = _GFXGetFontFirstChar();
        fontInfo.LastChar    = _GFXGetFontLastChar();
        fontInfo.CharCount   = (fontInfo.LastChar - fontInfo.FirstChar)+1;
        fontInfo.CharPosWorkingTable = (uint16_t)(c - (short)fontInfo.FirstChar);
        fontInfo.width       = _GFXGetCharWidth((fontInfo.CharPosWorkingTable*4) + FONT_OFFSET_WTABLE);
        fontInfo.BMP_Pos     = _GFXGetBMPLocation((fontInfo.CharPosWorkingTable*4) + FONT_OFFSET_WTABLE);
         return;
}

////////////////////////////////////////////////////////////////////////////////
//
int16_t GFXGetStringWidthN(char *string,uint8_t n){
int16_t w;//,j;
int8_t i = 0;
    w = 0;
    while(*string != 0){
        fontInfo.CharPosWorkingTable = (uint16_t)(*string - (short)fontInfo.FirstChar);
        fontInfo.width  = _GFXGetCharWidth((fontInfo.CharPosWorkingTable*4) + FONT_OFFSET_WTABLE);//GFXGetCharInfo(*string);
        w += fontInfo.width;
        w += GFX_CHAR_SPACING;
        string++;
        if(i==n)break;
        else i++;

    }
     return w;
}
////////////////////////////////////////////////////////////////////////////////
//character index of the pixel
int8_t CharIndexOfPixel(char *string, uint16_t pixel){
uint8_t index = 0;

         while(1){
             if(pixel < GFXGetStringWidthN(string,index))return index;
             else index++;
         }
}
////////////////////////////////////////////////////////////////////////////////
//Scrolling a message
void ScrollMsg(char *msg){
uint16_t msg_pixel_len, scroll_1,scroll_2,start_char;//,len;
uint8_t first_char_width = 0,i;
char temp_string[30];

     msg_pixel_len = strlen(msg);
     for(i=0;i<msg_pixel_len-1;i++){
         msg_pixel_len += GFX_CHAR_SPACING;
     }

      msg_pixel_len = GFX_SCREEN_WIDTH - msg_pixel_len; //64

     for(scroll_1=0;scroll_1<50/*msg_pixel_len*/;scroll_1++){
          start_char = CharIndexOfPixel(msg,scroll_1);
          strncpy(temp_string,msg+start_char,msg_pixel_len);
          first_char_width += GFX_CHAR_SPACING;
          for(scroll_2 = 0;scroll_2<first_char_width;scroll_2++){
              clearPix();
              GFXWriteStringXY(-scroll_2,0,temp_string,0,RGB);
              if(scroll_1 == 0){
                   DELAY_milliseconds(100);
              }
              else DELAY_milliseconds(100);
              scroll_1++;
          }
     }
    // letter = 0;
    // oneShot = 0;
}
////////////////////////////////////////////////////////////////////////////////
//fill picture
void fillPicture(const uint8_t *Pict){
static uint16_t x,_x,fx,y;//length,_fx,_y;
   _x = 0;
     for(y=0;y<32;y++){
       for(x = 0;x<4;x++){
      // _fx = 3-x;
         fontInfo.Data = PicA[_x+x];
         for(fx=0;fx<8;fx++){
           if (fontInfo.Data & (1 << fx))setPix(7+(x*8)-fx,y,1); //set color of pix
           else setPix(7+(x*8)-fx,y,0);                          //clear the pixel
         }
       }
       _x+=4;
     }
}
////////////////////////////////////////////////////////////////////////////////
//Draw square
void DRAW_Rect(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,type t){
unsigned int i,y;
 if(y0+y1>(GFX_SCREEN_HEIGHT-1) || y0 < 0 || x0+x1>(GFX_SCREEN_WIDTH-1) || x0<0)return;
 if(t == FILL){
     for(y=0;y<y1;y++){
      for(i=0;i<x1;i++){
         setPix(x0+i,y+y0,1);
      }
     }
 }else{
    for(i=0;i<x1;i++){
         setPix(i+x0,y0,1);
         setPix(i+x0,y0+y1,1);
    }
      for(i=0;i<y1;i++){
         setPix(x0,i+y0,1);
         setPix(x0+x1,i+y0,1);
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
//Draw a line
void DRAW_Line(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1){
 int p,p0,x,y,k,Dx,Dy,steps;
 //char *C;
  Dx = abs(x1-x0);
  Dy = abs(y1-y0);

  x = x0;
  y = y0;
  setPix(x,y,1);
  if(Dx>Dy)steps = Dx;
  else steps = Dy;

   p0 = 2*(Dy-Dx);
   p = p0;

   for(k=0;k<steps;k++){
       x = x+1;
       if(p<0)p=p+(2*Dy);
       else{
          if(y0<y1)y = y+1;
          else y = y-1;
          p = p+((2*Dy) - (2*Dx));
       }
       
       setPix(x,y,1);
   }
}


////////////////////////////////////////////////////////////////////////////////
//fill the top panel
void fillTop(){
    memset(P6V.P_Vram, 0xFF, 1024);
}
////////////////////////////////////////////////////////////////////////////////
//fill bottom panel
void fillBot(){
    memset(P6V.P_Vram + 1024, 0xFF, 1024);
}
////////////////////////////////////////////////////////////////////////////////
//fill buffer with 1's
void fillPix(){
     memset(P6V.P_Vram, 0xFF, 2048); //total of 384 bytes for 3 screens 6x64=384
}
////////////////////////////////////////////////////////////////////////////////
//fill buffer with 0's
void clearPix(){
     memset(P6V.P_Vram, 0x00, 2048);
}
////////////////////////////////////////////////////////////////////////////////
//set the pixel colour
void setCol(_RGB RGB){
  P6V.Data_Top_RGB = /*RGB*/0x04 & 0x07;
  P6V.Data_Bot_RGB = ( /*RGB*/0x02 << 3)& 0x38;
}
////////////////////////////////////////////////////////////////////////////////
//get the pixel colour
int GetCol(){
    return P6V.Data_Top_RGB | P6V.Data_Bot_RGB;
}
////////////////////////////////////////////////////////////////////////////////
//scrolls pixels for debug and pixel sequence turn off interrupts to check pix
void runPixSeq(){
//static uint8_t Dat;
static uint16_t iXX,Row,temp;//,Cnt;

if(Row > 31){
     Row = 0;
/*iXX = 0;*/
  }

  if(iXX > 63){ //64 pixels per row
     Row++;
     iXX = 0;
  }

  if(Row < 16)temp = Row; //16 rows per panel
  else temp =  Row - 16;

 // rowSelect(temp);
  for(iX=0;iX<64;iX++){     //iX = 64 leds wide
         // rowSelect is for 1 row per block 4x4 = 16
     if(iX == iXX){
        SR_R1 = 1;SR_G1 = 0;SR_B1 = 0; //databits
        SR_R2 = 0;SR_G2 = 1;SR_B2 = 0; //databits
     }else{
        SR_R1 = 0;SR_G1 = 0;SR_B1 = 0; //databits
        SR_R2 = 0;SR_G2 = 0;SR_B2 = 0; //databits
     }   
     toggle_SCLK();        
  }
  iXX++;
  latchLeds(temp);
 // DELAY_milliseconds(10);
}
////////////////////////////////////////////////////////////////////////////////
//string function

int GFXWriteStringPos(char *string){
short w;
int x;
      w = 0;x = 0;
      //test the STRING to see if it is empty then set to black
      while(*string != '\n'){
            GFXGetCharInfo(*string);
            w = fontInfo.width;
            x+=w;
            x+=GFX_CHAR_SPACING;
            string++;
      }
      return x;
}
