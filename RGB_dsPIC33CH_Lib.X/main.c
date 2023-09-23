/**
  Section: Included Files
*/
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/delay.h"
#include "mcc_generated_files/pin_manager.h"
#include "RGB.h"
/*
                         Main application
 */
/*
 * 
 * 
void dispMsgLast(short line,_RGB RGB) {
 static int col,lastCol;
 col = GetCol();
   if(line == 1){
     static char p[25];
      if((strcmp(p,Str.strMsgTopLine)!=0)|| (col != lastCol)){
        err = GFXWriteStringXY(3,5,Str.strMsgTopLine,STRING,RGB);
        #ifdef DeBug
         UART1_Write_Text(Str.strMsgTopLine);
         UART1_Write(0x0A);
        #endif
        strcpy(p,Str.strMsgTopLine);
      }
   }
   if(line == 2){
      static char q[25];
      if((strcmp(q,Str.strMsgBotLine)!=0)||(col != lastCol)){
        err = GFXWriteStringXY(3,19, Str.strMsgBotLine,STRING,RGB);
        #ifdef DeBug
          UART1_Write(0x0A);
          UART1_Write_Text(Str.strMsgBotLine);
        #endif
        strcpy(q,Str.strMsgBotLine);
      }
   }
 lastCol = col;
}
*/

int main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    while (1)
    {
        G1_Toggle();
        DELAY_milliseconds(500);
        
        // Add your application code
    }
    return 1; 
}
/**
 End of File
*/

