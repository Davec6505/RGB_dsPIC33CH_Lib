/**
  Section: Included Files
*/
#include <string.h>
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/delay.h"
#include "mcc_generated_files/pin_manager.h"
#include "RGB.h"
#include "../../XC_GFX_Lib/GFX_Lib.X/Fonts.h"
#include "../../XC_GFX_Lib/GFX_Lib.X/GFX.h"
#include "utilities.h"
/*
                         Main application
 */

/////////////////////////////////////////////////////////
//ETHERNET SETTINGS DEFAULT SET HERE
unsigned char   myMacAddr[6] = {0x00, 0x14, 0xA5, 0x76, 0x19, 0x31};// my MAC address
unsigned char   myIpAddr[4]  = {10, 0,  0, 49 };                    // my IP address
unsigned char   gwIpAddr[4]  = {10, 0,  0, 2 };                     // gateway (router) IP address
unsigned char   ipMask[4]    = {255, 255, 255,  0 };                // network mask (for example : 255.255.255.0)
unsigned char   dnsIpAddr[4] = {10, 0,  0,  1 };                  // DNS server IP address


int8_t DisplayIPAddress(){
  int8_t err;
  char addTxt[4];

   err = GFXWriteStringXY(2,1, "Addr:",STRING,green);
   ByteToStr(myIpAddr[0],addTxt);
   err = GFXWriteStringXY(32,1, addTxt,STRING,red);
   ByteToStr(myIpAddr[1],addTxt);
   err = GFXWriteStringXY(2,11, addTxt,STRING,blue);
   ByteToStr(myIpAddr[2],addTxt);
   err = GFXWriteStringXY(32,11, addTxt,STRING,cyan);
   ByteToStr(myIpAddr[3],addTxt);
   err = GFXWriteStringXY(2,21, addTxt,STRING,white);
   
   return err;
}

int main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    _GFXSetFont(Arial8x11);  //initial font !!
    P6V.Data_Top_RGB = yellow;
    P6V.Data_Bot_RGB = green << 3;
    DisplayIPAddress();
   // memset((void*)P6V.P_Vram,0xFF,sizeof(P6V.P_Vram));
    while (1)
    {
       // G1_Toggle();
       // DELAY_milliseconds(10);
       // runPixSeq();
       // Int_Handler(); 
       // Add your application code
    }
    return 1; 
}
/**
 End of File
*/

