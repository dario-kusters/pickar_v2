/**
  * TCD0 Generated Driver File
  *
  * @file tcd0.c
  *
  * @ingroup tcd0
  *
  * @brief This file contains the API implementations for the TCD0 driver.
  *
  * @version TCD0 Driver Version 2.0.3
*/


#include "../tcd0.h"
#include "../../../include/globals.h"

const struct TMR_INTERFACE TCD0_Interface = {
    .Initialize = TCD0_Initialize,
    .Start = TCD0_Start,
    .Stop = TCD0_Stop,
    .PeriodCountSet = NULL,
    .TimeoutCallbackRegister = NULL,
    .Tasks = NULL
};

void (*TCD0_TRIG_isr_cb)(void) = NULL;
void (*TCD0_OVF_isr_cb)(void) = NULL;

void TCD0_SetTRIGIsrCallback(TCD0_cb_t cb)
{
    TCD0_TRIG_isr_cb = cb;
}

void TCD0_SetOVFIsrCallback(TCD0_cb_t cb)
{
   TCD0_OVF_isr_cb = cb;
}

void TCD0_Initialize(void)
{
    //Clear A
    TCD0.CMPACLR = 0xFFE;

    //SET A
    TCD0.CMPASET = 0x7FF;

    //Clear B
    TCD0.CMPBCLR = 0xFFE;

    //SET B
    TCD0.CMPBSET = 0xD7F;

    //WGMODE One ramp mode; 
    TCD0.CTRLB = 0x0;

    //AUPDATE disabled; CMPCSEL PWM A output; CMPDSEL PWM B output; CMPOVR disabled; FIFTY disabled; 
    TCD0.CTRLC = 0x80;

    //CMPAVAL 0; CMPBVAL 0; 
    TCD0.CTRLD = 0x0;

    //DISEOC disabled; RESTART disabled; SCAPTUREA disabled; SCAPTUREB disabled; SYNC disabled; SYNCEOC disabled; 
    TCD0.CTRLE = 0x0;

    //DBGRUN disabled; FAULTDET disabled; 
    TCD0.DBGCTRL = 0x0;

    //DITHERSEL ONTIMEAB; 
    TCD0.DITCTRL = 0x1;

    //DITHER 0x0; 
    TCD0.DITVAL = 0x0;

    //DLYPRESC DIV1; DLYSEL OFF; DLYTRIG CMPASET; 
    TCD0.DLYCTRL = 0x0;

    //DLYVAL undefined; 
    TCD0.DLYVAL = 0x0;

    //ACTION Event trigger a fault; CFG Neither Filter nor Asynchronous Event is enabled; EDGE The falling edge or low level of event generates retrigger or fault action; TRIGEI disabled; 
    TCD0.EVCTRLA = 0x0;

    //ACTION Event trigger a fault; CFG Neither Filter nor Asynchronous Event is enabled; EDGE The falling edge or low level of event generates retrigger or fault action; TRIGEI disabled; 
    TCD0.EVCTRLB = 0x0;

    //CMPA disabled; CMPAEN disabled; CMPB disabled; CMPBEN disabled; CMPC enabled; CMPCEN enabled; CMPD enabled; CMPDEN enabled; 
    ccp_write_io((void*)&(TCD0.FAULTCTRL),0xCC);

    //INPUTMODE Input has no actions; 
    TCD0.INPUTCTRLA = 0x0;

    //INPUTMODE Input has no actions; 
    TCD0.INPUTCTRLB = 0x0;

    //OVF enabled; TRIGA disabled; TRIGB disabled; 
    TCD0.INTCTRL = 0x1;

    while ((TCD0.STATUS & TCD_ENRDY_bm) == 0); // Wait for Enable Ready to be high.

    //PWMACTA disabled; PWMACTB disabled; 
    TCD0.STATUS = 0x0;

    //CLKSEL Internal High-Frequency oscillator; CNTPRES DIV1; ENABLE enabled; SYNCPRES DIV1; 
    TCD0.CTRLA = 0x1;

}

void TCD0_Start(void)
{
    TCD0.CTRLA|= TCD_ENABLE_bm;
}

void TCD0_Stop(void)
{
    TCD0.CTRLA&= ~TCD_ENABLE_bm;
}

ISR(TCD0_OVF_vect)
{
    /* The interrupt flag has to be cleared manually */
    TCD0.INTFLAGS = TCD_OVF_bm;

    //  TCD0.CMPACLR = 4094;
/*    TCD0.CMPASET = 4095 - PWMset[0];
    //  TCD0.CMPBCLR = 4094;
    TCD0.CMPBSET = 4095 - PWMset[1];
*/
    TCD0.CMPASET = PWMset[0];
    //  TCD0.CMPBCLR = 4094;
    TCD0.CMPBSET = PWMset[1];

     TCD0.CTRLE |= TCD_SYNCEOC_bm;
  
    /* Insert your TCD Over Flow interrupt handling code here */
    if(TCD0_OVF_isr_cb != NULL)
    {
        TCD0_OVF_isr_cb();
    }
}

ISR(TCD0_TRIG_vect)
{
    /* TRIGA interrupt flag has to be cleared manually */
    TCD0.INTFLAGS = TCD_TRIGA_bm;

    /* Insert your TCD interrupt handling code here */
    if(TCD0_TRIG_isr_cb != NULL)
    {
        TCD0_TRIG_isr_cb();
    }
}
