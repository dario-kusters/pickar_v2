/**
 * Generated Driver File
 * 
 * @file pins.c
 * 
 * @ingroup  pinsdriver
 * 
 * @brief This is generated driver implementation for pins. 
 *        This file provides implementations for pin APIs for all pins selected in the GUI.
 *
 * @version Driver Version 1.1.0
*/

/*
© [2026] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

#include "../pins.h"

static void (*LED_R_InterruptHandler)(void);
static void (*LED_G_InterruptHandler)(void);
static void (*Ain_I_InterruptHandler)(void);
static void (*Ain_V_InterruptHandler)(void);
static void (*RXD_InterruptHandler)(void);
static void (*TXD_InterruptHandler)(void);
static void (*ONbtn_InterruptHandler)(void);
static void (*OFF_SMPS_InterruptHandler)(void);
static void (*PWR_SW_InterruptHandler)(void);
static void (*Buzzer_InterruptHandler)(void);

void PIN_MANAGER_Initialize()
{
  /* DIR Registers Initialization */
    PORTA.DIR = 0x1;
    PORTC.DIR = 0xE;
    PORTD.DIR = 0x30;
    PORTF.DIR = 0x0;

  /* OUT Registers Initialization */
    PORTA.OUT = 0x1;
    PORTC.OUT = 0x2;
    PORTD.OUT = 0x0;
    PORTF.OUT = 0x0;

  /* PINxCTRL registers Initialization */
    PORTA.PIN0CTRL = 0x0;
    PORTA.PIN1CTRL = 0x8;
    PORTA.PIN2CTRL = 0x0;
    PORTA.PIN3CTRL = 0x0;
    PORTA.PIN4CTRL = 0x0;
    PORTA.PIN5CTRL = 0x0;
    PORTA.PIN6CTRL = 0x0;
    PORTA.PIN7CTRL = 0x0;
    PORTC.PIN0CTRL = 0x0;
    PORTC.PIN1CTRL = 0x0;
    PORTC.PIN2CTRL = 0x0;
    PORTC.PIN3CTRL = 0x0;
    PORTC.PIN4CTRL = 0x0;
    PORTC.PIN5CTRL = 0x0;
    PORTC.PIN6CTRL = 0x0;
    PORTC.PIN7CTRL = 0x0;
    PORTD.PIN0CTRL = 0x0;
    PORTD.PIN1CTRL = 0x0;
    PORTD.PIN2CTRL = 0x0;
    PORTD.PIN3CTRL = 0x0;
    PORTD.PIN4CTRL = 0x0;
    PORTD.PIN5CTRL = 0x0;
    PORTD.PIN6CTRL = 0x0;
    PORTD.PIN7CTRL = 0x0;
    PORTF.PIN0CTRL = 0x0;
    PORTF.PIN1CTRL = 0x0;
    PORTF.PIN2CTRL = 0x0;
    PORTF.PIN3CTRL = 0x0;
    PORTF.PIN4CTRL = 0x0;
    PORTF.PIN5CTRL = 0x0;
    // PF6 is a polled active-low button input; PORT interrupt sensing stays disabled.
    PORTF.PIN6CTRL = 0x8;
    PORTF.PIN7CTRL = 0x0;

  /* PORTMUX Initialization */
    PORTMUX.CCLROUTEA = 0x0;
    PORTMUX.EVSYSROUTEA = 0x0;
    PORTMUX.SPIROUTEA = 0x0;
    PORTMUX.TCAROUTEA = 0x0;
 //   PORTMUX.TCBROUTEA = 0x0;
    PORTMUX.TCDROUTEA = 0x4;
    PORTMUX.TWIROUTEA = 0x0;
    PORTMUX.USARTROUTEA = 0x0;

  // register default ISC callback functions at runtime; use these methods to register a custom function
    LED_R_SetInterruptHandler(LED_R_DefaultInterruptHandler);
    LED_G_SetInterruptHandler(LED_G_DefaultInterruptHandler);
    Ain_I_SetInterruptHandler(Ain_I_DefaultInterruptHandler);
    Ain_V_SetInterruptHandler(Ain_V_DefaultInterruptHandler);
    RXD_SetInterruptHandler(RXD_DefaultInterruptHandler);
    TXD_SetInterruptHandler(TXD_DefaultInterruptHandler);
    ONbtn_SetInterruptHandler(ONbtn_DefaultInterruptHandler);
    OFF_SMPS_SetInterruptHandler(OFF_SMPS_DefaultInterruptHandler);
    PWR_SW_SetInterruptHandler(PWR_SW_DefaultInterruptHandler);
    Buzzer_SetInterruptHandler(Buzzer_DefaultInterruptHandler);
}

/**
  Allows selecting an interrupt handler for LED_R at application runtime
*/
void LED_R_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    LED_R_InterruptHandler = interruptHandler;
}

void LED_R_DefaultInterruptHandler(void)
{
    // add your LED_R interrupt custom code
    // or set custom function using LED_R_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for LED_G at application runtime
*/
void LED_G_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    LED_G_InterruptHandler = interruptHandler;
}

void LED_G_DefaultInterruptHandler(void)
{
    // add your LED_G interrupt custom code
    // or set custom function using LED_G_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for Ain_I at application runtime
*/
void Ain_I_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    Ain_I_InterruptHandler = interruptHandler;
}

void Ain_I_DefaultInterruptHandler(void)
{
    // add your Ain_I interrupt custom code
    // or set custom function using Ain_I_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for Ain_V at application runtime
*/
void Ain_V_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    Ain_V_InterruptHandler = interruptHandler;
}

void Ain_V_DefaultInterruptHandler(void)
{
    // add your Ain_V interrupt custom code
    // or set custom function using Ain_V_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for RXD at application runtime
*/
void RXD_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    RXD_InterruptHandler = interruptHandler;
}

void RXD_DefaultInterruptHandler(void)
{
    // add your RXD interrupt custom code
    // or set custom function using RXD_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for TXD at application runtime
*/
void TXD_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    TXD_InterruptHandler = interruptHandler;
}

void TXD_DefaultInterruptHandler(void)
{
    // add your TXD interrupt custom code
    // or set custom function using TXD_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for ONbtn at application runtime
*/
void ONbtn_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    ONbtn_InterruptHandler = interruptHandler;
}

void ONbtn_DefaultInterruptHandler(void)
{
    // add your ONbtn interrupt custom code
    // or set custom function using ONbtn_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for OFF_SMPS at application runtime
*/
void OFF_SMPS_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    OFF_SMPS_InterruptHandler = interruptHandler;
}

void OFF_SMPS_DefaultInterruptHandler(void)
{
    // add your OFF_SMPS interrupt custom code
    // or set custom function using OFF_SMPS_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for PWR_SW at application runtime
*/
void PWR_SW_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    PWR_SW_InterruptHandler = interruptHandler;
}

void PWR_SW_DefaultInterruptHandler(void)
{
    // add your PWR_SW interrupt custom code
    // or set custom function using PWR_SW_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for Buzzer at application runtime
*/
void Buzzer_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    Buzzer_InterruptHandler = interruptHandler;
}

void Buzzer_DefaultInterruptHandler(void)
{
    // add your Buzzer interrupt custom code
    // or set custom function using Buzzer_SetInterruptHandler()
}
ISR(PORTA_PORT_vect)
{ 
    // Call the interrupt handler for the callback registered at runtime
    if(VPORTA.INTFLAGS & PORT_INT1_bm)
    {
       RXD_InterruptHandler(); 
    }
    if(VPORTA.INTFLAGS & PORT_INT0_bm)
    {
       TXD_InterruptHandler(); 
    }
    /* Clear interrupt flags */
    VPORTA.INTFLAGS = 0xff;
}

ISR(PORTC_PORT_vect)
{ 
    // Call the interrupt handler for the callback registered at runtime
    if(VPORTC.INTFLAGS & PORT_INT1_bm)
    {
       OFF_SMPS_InterruptHandler(); 
    }
    if(VPORTC.INTFLAGS & PORT_INT2_bm)
    {
       PWR_SW_InterruptHandler(); 
    }
    if(VPORTC.INTFLAGS & PORT_INT3_bm)
    {
       Buzzer_InterruptHandler(); 
    }
    /* Clear interrupt flags */
    VPORTC.INTFLAGS = 0xff;
}

ISR(PORTD_PORT_vect)
{ 
    // Call the interrupt handler for the callback registered at runtime
    if(VPORTD.INTFLAGS & PORT_INT4_bm)
    {
       LED_R_InterruptHandler(); 
    }
    if(VPORTD.INTFLAGS & PORT_INT5_bm)
    {
       LED_G_InterruptHandler(); 
    }
    if(VPORTD.INTFLAGS & PORT_INT6_bm)
    {
       Ain_I_InterruptHandler(); 
    }
    if(VPORTD.INTFLAGS & PORT_INT7_bm)
    {
       Ain_V_InterruptHandler(); 
    }
    /* Clear interrupt flags */
    VPORTD.INTFLAGS = 0xff;
}

ISR(PORTF_PORT_vect)
{ 
    // Call the interrupt handler for the callback registered at runtime
    if(VPORTF.INTFLAGS & PORT_INT6_bm)
    {
       ONbtn_InterruptHandler(); 
    }
    /* Clear interrupt flags */
    VPORTF.INTFLAGS = 0xff;
}

/**
 End of File
*/
