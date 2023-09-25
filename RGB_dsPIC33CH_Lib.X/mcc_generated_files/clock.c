#include <stdint.h>
#include "xc.h"
#include "clock.h"

void CLOCK_Initialize(void)
{
    // FRCDIV FRC/1; PLLPRE 1; DOZE 1:8; DOZEN disabled; ROI disabled; 
    CLKDIV = 0x3001;
    // PLLFBDIV 125; 
    PLLFBD = 0x7D;
    // TUN Center frequency; 
    OSCTUN = 0x00;
    // POST1DIV 1:5; VCODIV FVCO; POST2DIV 1:1; 
    PLLDIV = 0x351;
    // APLLEN enabled; FRCSEL FRC; APLLPRE 1:1; 
    ACLKCON1 = 0x8101;
    // APLLFBDIV 125; 
    APLLFBD1 = 0x7D;
    // APOST1DIV 1:5; APOST2DIV 1:1; AVCODIV FVCO/4; 
    APLLDIV1 = 0x51;
    // CANCLKEN disabled; CANCLKSEL No Clock Selected; CANCLKDIV Divide by 1; 
    CANCLKCON = 0x00;
    // ROEN enabled; ROSWEN disabled; ROSLP disabled; ROSEL FOSC/2; ROOUT disabled; ROSIDL disabled; 
    REFOCONL = 0x8001;
    // RODIV 0; 
    REFOCONH = 0x00;
    // ROTRIM 0; 
    REFOTRIMH = 0x00;
    // IOLOCK disabled; 
    RPCON = 0x00;
    // ADC1MD enabled; T1MD enabled; U2MD enabled; U1MD enabled; SPI2MD enabled; SPI1MD enabled; QEIMD enabled; C1MD enabled; PWMMD enabled; I2C1MD enabled; 
    PMD1 = 0x00;
    // CCP2MD enabled; CCP1MD enabled; CCP4MD enabled; CCP3MD enabled; CCP7MD enabled; CCP8MD enabled; CCP5MD enabled; CCP6MD enabled; 
    PMD2 = 0x00;
    // CRCMD enabled; I2C2MD enabled; 
    PMD3 = 0x00;
    // REFOMD enabled; 
    PMD4 = 0x00;
    // DMA5MD enabled; DMA1MD enabled; DMA2MD enabled; DMA3MD enabled; DMA4MD enabled; DMA0MD enabled; 
    PMD6 = 0x00;
    // PTGMD enabled; CMP1MD enabled; 
    PMD7 = 0x00;
    // DMTMD enabled; CLC3MD enabled; BIASMD enabled; CLC4MD enabled; SENT2MD enabled; SENT1MD enabled; CLC1MD enabled; CLC2MD enabled; 
    PMD8 = 0x00;
    // CF no clock failure; NOSC FRCDIV; CLKLOCK unlocked; OSWEN Switch is Complete; 
    __builtin_write_OSCCONH((uint8_t) (0x07));
    __builtin_write_OSCCONL((uint8_t) (0x00));
}

bool CLOCK_AuxPllLockStatusGet()
{
    return ACLKCON1bits.APLLCK;
}
