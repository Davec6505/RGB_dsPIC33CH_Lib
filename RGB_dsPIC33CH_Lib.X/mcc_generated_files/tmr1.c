#include <stdio.h>
#include "tmr1.h"
#include "pin_manager.h"
#include "../RGB.h"
/**
 Section: File specific functions
*/
void (*TMR1_InterruptHandler)(void) = NULL;
void (*TMR1_RGB_Handler)(void) = NULL;
void TMR1_CallBack(void);

/**
  Section: Data Type Definitions
*/

#define RGB_CLOCK 5

typedef struct RTC{
    uint32_t millis;
    uint16_t ms;
    uint32_t secs;
    uint8_t sec;
}rtc;

static rtc T0 = {
    0
    ,0
    ,0
    ,0
};

typedef struct RGB_VAR{
    uint8_t cycle_tmr;
}RGB_CYCLE;

static RGB_CYCLE rgb_cyc={
    0
};
/** TMR Driver Hardware Instance Object

  @Summary
    Defines the object required for the maintenance of the hardware instance.

  @Description
    This defines the object required for the maintenance of the hardware
    instance. This object exists once per hardware instance of the peripheral.

  Remarks:
    None.
*/

typedef struct _TMR_OBJ_STRUCT
{
    /* Timer Elapsed */
    volatile bool           timerElapsed;
    /*Software Counter value*/
    volatile uint8_t        count;

} TMR_OBJ;

static TMR_OBJ tmr1_obj;

/**
  Section: Driver Interface
*/

void TMR1_Initialize (void)
{
    //TMR 0; 
    TMR1 = 0x00;
    //Period = 0.001 s; Frequency = 4000000 Hz; PR 499; 
    PR1 = 0x1F3;
    //TCKPS 1:8; PRWIP Write complete; TMWIP Write complete; TON enabled; TSIDL disabled; TCS FOSC/2; TECS T1CK; TSYNC disabled; TMWDIS disabled; TGATE disabled; 
    T1CON = 0x8010;

    if(TMR1_InterruptHandler == NULL)
    {
        TMR1_SetInterruptHandler(&TMR1_CallBack);
    }
    if(TMR1_RGB_Handler == NULL)
    {
        RGB_SetInterruptHandler(Int_Handler);
    }
	
    IFS0bits.T1IF = false;
    IEC0bits.T1IE = true;
	
    tmr1_obj.timerElapsed = false;

}


void __attribute__ ( ( interrupt, no_auto_psv ) ) _T1Interrupt (  )
{
     IEC0bits.T1IE = false;
    /* Check if the Timer Interrupt/Status is set */
    // ticker function call;
    // ticker is 1 -> Callback function gets called everytime this ISR executes
    if(TMR1_InterruptHandler) 
    {
           TMR1_InterruptHandler(); 
    }
    //***User Area Begin


    //***User Area End

    tmr1_obj.count++;
    tmr1_obj.timerElapsed = true;
    IFS0bits.T1IF = false;
    IEC0bits.T1IE = true;
}

void TMR1_Period16BitSet( uint16_t value )
{
    /* Update the counter values */
    PR1 = value;
    /* Reset the status information */
    tmr1_obj.timerElapsed = false;
}

uint16_t TMR1_Period16BitGet( void )
{
    return( PR1 );
}

void TMR1_Counter16BitSet ( uint16_t value )
{
    /* Update the counter values */
    TMR1 = value;
    /* Reset the status information */
    tmr1_obj.timerElapsed = false;
}

uint16_t TMR1_Counter16BitGet( void )
{
    return( TMR1 );
}


void __attribute__ ((weak)) TMR1_CallBack(void)
{
    // Add your custom callback code here
  /*  T0.millis++;
    T0.ms++;
    if(T0.ms > 9999){
        T0.ms = 0;
        T0.sec++;
        T0.secs++;
        if(T0.sec > 59){
            T0.sec = 0;
        }      
    }*/
    
    rgb_cyc.cycle_tmr++;
    if(rgb_cyc.cycle_tmr >= RGB_CLOCK){
        rgb_cyc.cycle_tmr = 0;
        if(TMR1_RGB_Handler){
            TMR1_RGB_Handler();
        }
    }
   //  TMR1_Test_Toggle();
}

void  TMR1_SetInterruptHandler(void (* InterruptHandler)(void))
{ 
    IEC0bits.T1IE = false;
    TMR1_InterruptHandler = InterruptHandler; 
    IEC0bits.T1IE = true;
}

void RGB_SetInterruptHandler(void(*RGB_Handler)(void)){
    IEC0bits.T1IE = false;
    TMR1_RGB_Handler = RGB_Handler; 
    IEC0bits.T1IE = true;
}

void TMR1_Start( void )
{
    /* Reset the status information */
    tmr1_obj.timerElapsed = false;

    /*Enable the interrupt*/
    IEC0bits.T1IE = true;

    /* Start the Timer */
    T1CONbits.TON = 1;
}

void TMR1_Stop( void )
{
    /* Stop the Timer */
    T1CONbits.TON = false;

    /*Disable the interrupt*/
    IEC0bits.T1IE = false;
}

bool TMR1_GetElapsedThenClear(void)
{
    bool status;
    
    status = tmr1_obj.timerElapsed;

    if(status == true)
    {
        tmr1_obj.timerElapsed = false;
    }
    return status;
}

int TMR1_SoftwareCounterGet(void)
{
    return tmr1_obj.count;
}

void TMR1_SoftwareCounterClear(void)
{
    tmr1_obj.count = 0; 
}

/**
 End of File
*/
