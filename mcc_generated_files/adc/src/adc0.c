/**
 * ADC0 Generated Driver File
 *
 * @file adc0.c
 * 
 * @ingroup adc0 
 * 
 * @brief API Implementations for ADC0 module driver.
 * 
 * @version ADC0 Driver Version 1.0.0
*/


#include "../adc0.h"
#include "../../system/pins.h"
#include "../../../include/globals.h"

adc_irq_cb_t ADC0_window_cb = NULL;
adc_irq_cb_t ADC0_resrdy_cb = NULL;

int8_t ADC0_Initialize(void)
{
     
    // SAMPNUM 8 results accumulated; 
    ADC0.CTRLB = 0x3;

    // PRESC CLK_PER divided by 256; 
    ADC0.CTRLC = 0xD;

    // INITDLY DLY16; SAMPDLY DLY1; 
    ADC0.CTRLD = 0x21;

    // WINCM No Window Comparison; 
    ADC0.CTRLE = 0x0;

    // DBGRUN disabled; 
    ADC0.DBGCTRL = 0x0;

    // STARTEI disabled; 
    ADC0.EVCTRL = 0x0;

    // RESRDY enabled; WCMP disabled; 
    ADC0.INTCTRL = 0x1;

    // MUXPOS ADC input pin 6; 
    ADC0.MUXPOS = 0x6;

    // MUXNEG Ground; 
    ADC0.MUXNEG = 0x40;

    // SAMPLEN 255; 
    ADC0.SAMPCTRL = 0xFF;

    // Window comparator high threshold 
    ADC0.WINHT = 0xFFF;

    // Window comparator low threshold 
    ADC0.WINLT = 0x0;

    // ENABLE enabled; FREERUN enabled; RESSEL 12-bit mode; RUNSTBY disabled; CONVMODE disabled; LEFTADJ disabled; 
    ADC0.CTRLA = 0x3;
 
    // Realizar una conversión inicial "dummy"
    ADC0.COMMAND = ADC_STCONV_bm;
    while (!ADC0_IsConversionDone());
    (void)ADC0_GetConversionResult();  // Descartamos el resultado
 

    return 0;
}

void ADC0_Enable(void)
{
    ADC0.CTRLA |= ADC_ENABLE_bm;
}

void ADC0_Disable(void)
{
    ADC0.CTRLA &= ~ADC_ENABLE_bm;
}

void ADC0_EnableAutoTrigger(void)
{
    ADC0.EVCTRL |= ADC_STARTEI_bm;
}

void ADC0_DisableAutoTrigger(void)
{
    ADC0.EVCTRL &= ~ADC_STARTEI_bm;
}

void ADC0_SetWindowHigh(adc_result_t high)
{
    ADC0.WINHT = high;
}

void ADC0_SetWindowLow(adc_result_t low)
{
    ADC0.WINLT = low;
}

void ADC0_SetWindowMode(ADC0_window_mode_t mode)
{
    ADC0.CTRLE = mode;
}

void ADC0_SetWindowChannel(adc_0_channel_t channel)
{
    ADC0.MUXPOS = channel;
}

void ADC0_StartConversion(adc_0_channel_t channel)
{
    ADC0.MUXPOS  = channel;
    ADC0.COMMAND = ADC_STCONV_bm;
}

void ADC0_StartDiffConversion(adc_0_channel_t channel, adc_0_muxneg_channel_t channel1)
{
    ADC0.MUXPOS  = channel;
    ADC0.MUXNEG  = channel1;
    ADC0.COMMAND = ADC_STCONV_bm;
}

void ADC0_StopConversion(void)
{
    ADC0.COMMAND = ADC_SPCONV_bm;
}

bool ADC0_IsConversionDone(void)
{
    return (ADC0.INTFLAGS & ADC_RESRDY_bm);
}

adc_result_t ADC0_GetConversionResult(void)
{
    return (ADC0.RES);
}

bool ADC0_GetWindowResult(void)
{
    bool temp     = (ADC0.INTFLAGS & ADC_WCMP_bm);
    ADC0.INTFLAGS = ADC_WCMP_bm; // Clear intflag if set
    return temp;
}

adc_result_t ADC0_GetConversion(adc_0_channel_t channel)
{
    adc_result_t res;

    ADC0_StartConversion(channel);
    while (!ADC0_IsConversionDone());
    res           = ADC0_GetConversionResult();
    ADC0.INTFLAGS = ADC_RESRDY_bm;
    return res;
}

 diff_adc_result_t ADC0_GetDiffConversion(adc_0_channel_t channel, adc_0_muxneg_channel_t channel1)
{
    diff_adc_result_t res;

    ADC0_StartDiffConversion(channel, channel1);
    while (!ADC0_IsConversionDone());
    res = ADC0_GetConversionResult();
    ADC0.INTFLAGS |= ADC_RESRDY_bm;
    return res;
}

uint8_t ADC0_GetResolution(void)
{
    return (ADC0.CTRLA & ADC_RESSEL0_bm) ? 10 : 12;
}

void ADC0_RegisterWindowCallback(adc_irq_cb_t f)
{
    ADC0_window_cb = f;
}

void ADC0_RegisterResrdyCallback(adc_irq_cb_t f)
{
    ADC0_resrdy_cb = f;
}

ISR(ADC0_WCMP_vect)
{        
    // Clear the interrupt flag
    ADC0.INTFLAGS = ADC_WCMP_bm;

    if (ADC0_window_cb != NULL)
    {
        ADC0_window_cb();
    }
}

ISR(ADC0_RESRDY_vect)
{
    // The interrupt flag is cleared when ADC0.RES is read.
    adc_0_channel_t currentChannel = ADC0.MUXPOS;

    switch(currentChannel) {
        case 6:
            adcValues[0] = ADC0_GetConversionResult() >> 3;
            currentChannel++;
            break;
        case 7:
            adcValues[1] = ADC0_GetConversionResult() >> 3;
            currentChannel = 6; // Reset to the first channel after reaching the last one
            GPR_GPR0 |= (1 << bitADCvalues);
            break;
        default:
            currentChannel = 6;// Si hay error desconocido iniciar de nuevo.
            break;
    }
    // Set the next channel
    ADC0.MUXPOS = currentChannel;
}
