/**
  * RTC Generated Driver File
  *
  * @file rtc.c
  *
  * @ingroup rtc
  *
  * @brief This file contains the driver code for RTC module.
  *
  * version RTC Driver Version 2.0.2
*/

#include "../rtc.h"
#include "../../system/pins.h"
#include "../../../include/globals.h"
void (*RTC_OVF_isr_cb)(void) = NULL;
void (*RTC_CMP_isr_cb)(void) = NULL;
void (*RTC_PIT_isr_cb)(void) = NULL;

int8_t RTC_Initialize(void)
{
    while (RTC.STATUS > 0) { /* Wait for all register to be synchronized */
    }
    //Compare 
    RTC.CMP = 0x3FFF;

    //Count
    RTC.CNT = 0x0;

    //Period
    RTC.PER = 0x3FFF;

    //Clock selection
    RTC.CLKSEL = 0x0;

    // CMP enabled; OVF enabled; 
    RTC.INTCTRL = 0x3;

    // CORREN disabled; PRESCALER RTC Clock / 2; RTCEN enabled; RUNSTDBY disabled; 
    RTC.CTRLA = 0x9;


        while (RTC.PITSTATUS > 0) { /* Wait for all register to be synchronized */
    }
    // PI enabled; 
	RTC.PITINTCTRL = 0x1;

    return 0;
}

void RTC_Start(void)
{
    RTC.CTRLA |= RTC_RTCEN_bm;
}

void RTC_Stop(void)
{
    RTC.CTRLA &= ~RTC_RTCEN_bm;
}

void RTC_SetOVFIsrCallback(RTC_cb_t cb)
{
    RTC_OVF_isr_cb = cb;
}

void RTC_SetCMPIsrCallback(RTC_cb_t cb)
{
    RTC_CMP_isr_cb = cb;
}

void RTC_SetPITIsrCallback(RTC_cb_t cb)
{
    RTC_PIT_isr_cb = cb;
}

ISR(RTC_CNT_vect)
{
    if (RTC.INTFLAGS & RTC_OVF_bm )
    {
        if (RTC_OVF_isr_cb != NULL) 
        {
            (*RTC_OVF_isr_cb)();
        } 
    }  
    
    if (RTC.INTFLAGS & RTC_CMP_bm )
    {
        if (RTC_CMP_isr_cb != NULL) 
        {
            (*RTC_CMP_isr_cb)();
        } 
    }
    RTC.INTFLAGS = (RTC_OVF_bm | RTC_CMP_bm);
    
    // Aqui deberia ir el RTC
    //
    if (++RTC_ss >= 60) {
        RTC_ss = 0;
        if (++RTC_mm >= 60) {
            RTC_mm = 0;
            if (++RTC_hh >= 24) {
                RTC_hh = 0;
            }
        }
    }
    GPR_GPR0 |= (1 << bit1Sec_int); 
    
}

ISR(RTC_PIT_vect)
{
   if (RTC_PIT_isr_cb != NULL) 
   {
    (*RTC_PIT_isr_cb)();
   } 
   RTC.PITINTFLAGS = RTC_PI_bm;
}

inline void RTC_WriteCounter(uint16_t timerVal)
{
    while (RTC.STATUS & RTC_CNTBUSY_bm);
    RTC.CNT = timerVal;
}

inline uint16_t RTC_ReadCounter(void)
{
    return RTC.CNT;
}

inline void RTC_WritePeriod(uint16_t timerVal)
{
    while (RTC.STATUS & RTC_PERBUSY_bm);
    RTC.PER = timerVal;
}

inline uint16_t RTC_ReadPeriod(void)
{
    return RTC.PER;
}

inline void RTC_EnableCMPInterrupt(void)
{
    RTC.INTCTRL |= RTC_CMP_bm;
}

inline void RTC_DisableCMPInterrupt(void)
{
    RTC.INTCTRL &= ~RTC_CMP_bm; 
}

inline void RTC_EnableOVFInterrupt(void)
{
    RTC.INTCTRL |= RTC_OVF_bm; 
}

inline void RTC_DisableOVFInterrupt(void)
{
    RTC.INTCTRL &= ~RTC_OVF_bm; 
}

inline void RTC_EnablePITInterrupt(void)
{
    RTC.PITINTCTRL |= RTC_PI_bm;  
}

inline void RTC_DisablePITInterrupt(void)
{
    RTC.PITINTCTRL &= ~RTC_PI_bm; 
}

inline void RTC_ClearOVFInterruptFlag(void)
{
    RTC.INTFLAGS = RTC_OVF_bm;
}

inline bool RTC_IsOVFInterruptEnabled(void)
{
    return ((RTC.INTCTRL & RTC_OVF_bm) > 0);
}