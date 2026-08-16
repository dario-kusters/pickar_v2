
/* 
 * File:   globals.h
 * Author: dario
 *
 * Created on 15 de julio de 2023, 18:02
 */

#ifndef GLOBALS_H
#define	GLOBALS_H

#ifdef	__cplusplus
extern "C" {
#endif
    

    
    // GPR_GPR0:
    // bit  funcion                          Reset
    //  0:  Caracter CR recibido por usart0     0
    //  1:  bit1mS_int Set en TCB0 int, Reset en Main
    //  2:  
    //  3:  bit1Sec_int Set en RTC int, Reset en Main
    //  4:  
    //  5:  
    //  6:  Nuevos valores ADcC. en todas las entradas. Set en adc0.c
    //  7:  bitBuzzer
#define bit_CR      0
#define bit1mS_int  1

#define bit1Sec_int 3
    
#define bitADCvalues 6
#define bitBuzzer 7
#define _bitBuzzer (GPR_GPR0 & (1 << bitBuzzer))
    // GPR_GPR1:
    // bit  funcion                          Reset
    //  0:  
    //  1:  bit1mS_Main Set en Main_header si bit1mS_int, Reset en Main_end
    //  2:  bit100mS_Main Set en Main_header si bit1mS_int, Reset en Main_end
    //  3:  bit1Sec_Main Set al inicio de Main, Reset al final de en Main
    //  4:  Commando AL. muestra medidos del ADC1 ADC2 
    //  5:  Set con Comando Log, reset con 'z' + CR. envio datos por uart0
    //  6:  
    //  7:  

#define bit1mS_Main   1
#define _1mS_PULSE (GPR_GPR1 & (1 << bit1mS_Main))
#define bit100mS_Main 2
#define _100mS_PULSE (GPR_GPR1 & (1 << bit100mS_Main))
#define bit1Sec_Main  3
#define _1Sec_PULSE (GPR_GPR1 & (1 << bit1Sec_Main))
#define adc_Log_cmd 4   
#define LOG_cmd     5
//#define bitVbatt_30_PCT 7
    
    // GPR_GPR2:
    // bit  funcion                          Reset
    //  0:  bitBatt_LOW En PC_ON y en BMS flags si PC is OFF
    //  1:  bitBatt_Recovery
    //  2:  bitCharger_ON
    //  3:  bitNoCurrent 3x seguidas I_Batt_AVG = 0
    //  4:  
    //  5:  
    //  6:  
    //  7:  
#define bitBatt_LOW      0
#define _bitBatt_LOW (GPR_GPR2 & (1 << bitBatt_LOW))  
#define bitBatt_Recovery 1
#define _bitBatt_Recovery (GPR_GPR2 & (1 << bitBatt_Recovery))  
#define bitCharger_ON    2
#define _bitCharger_ON (GPR_GPR2 & (1 << bitCharger_ON))
#define bitNoCurrent    3
#define _bitNoCurrent (GPR_GPR2 & (1 << bitNoCurrent))
    



    
extern volatile uint16_t adcValues[2];  //AIN3..7 ver datasheet tabla I/O Multiplexing - Este mismo vector usado en main.
//extern uint8_t cnt_1mS; //Contador 0 - 100 que incrementa cada 1mS
extern uint16_t PWMset[2]; //Declarada como global en main.contiene el setpoint de cada PWM
extern float V_batt;
extern float I_batt;

extern volatile uint8_t RTC_hh;
extern volatile uint8_t RTC_mm;
extern volatile uint8_t RTC_ss;

extern uint16_t PC_ON_hh;
extern uint16_t PC_ON_mm;
extern uint16_t PC_ON_ss;



#ifdef	__cplusplus
}
#endif


#endif	/* GLOBALS_H */

/***************************************************************
 * Di is de STATE MACHINE van de BSM Main_Body gecreerd met
 * http:www.plantuml.com/
 * 
 @startuml

[*] -> POWER_ON : Reset
state POWER_ON
POWER_ON : Timer 10Sec

state BMS_IDLE
state NEW_BATT
state SMPS_ON
state SMPS_OFF
state Batt_FULL
state Batt_LOW
state Batt_Charging
state PC_ON {
  state Batt_95_PCT
  state Batt_80_PCT
  state Batt_30_PCT
  state Batt_20_PCT
  
  Batt_95_PCT -> Batt_80_PCT : Vbatt < 80%
  Batt_95_PCT : Full GREEN
  Batt_80_PCT --> Batt_30_PCT : Vbatt < 30%
  Batt_80_PCT : LED PWM 
  Batt_30_PCT -left-> Batt_20_PCT : Vbatt < 20%
  Batt_30_PCT : flash Red 1Hz
  Batt_20_PCT : flash Red 5Hz
  Batt_20_PCT : Timer 3min
  Batt_20_PCT : Beep 1Hz
  Batt_20_PCT : bitBatt_LOW
}


POWER_ON -down-> BMS_IDLE : +10 Sec
POWER_ON -> NEW_BATT : Button
NEW_BATT --> BMS_IDLE : Button relase
NEW_BATT : 2x beep
BMS_IDLE -down-> SMPS_ON : Button
BMS_IDLE --> BMS_IDLE : VBatt > 30%
BMS_IDLE -down-> Batt_LOW : Vbatt < 30%
BMS_IDLE --> Batt_Charging : Vbatt Up
BMS_IDLE : LED PWM

SMPS_ON -u-> BMS_IDLE : TimeOut
SMPS_ON --> PC_ON : IBatt > 100mA
SMPS_ON : . out : 19V\n Timer 60Sec\nflashes GREEN
PC_ON --> SMPS_OFF : IBatt < 20mA\nOR\nTimeout
PC_ON : out 19V
SMPS_OFF : Energy over calc\nignore recovery
SMPS_OFF -u-> BMS_IDLE : VBatt > 40%
SMPS_OFF --> Batt_LOW : VBatt < 40%\nOR\nbitBatt_LOW


Batt_FULL -u-> BMS_IDLE : VBatt = 27,5V
Batt_FULL : cycle++\ndata to EPROM
Batt_LOW -> Batt_Charging : Vbatt > 50%
Batt_LOW : Ignore recovery
Batt_LOW : clear bitBatt_LOW
Batt_Charging -> Batt_FULL : Vbatt > 28,52V
Batt_Charging -right[#Red]-> Batt_Charging : \nTimer
Batt_Charging -u-> SMPS_ON : Button
Batt_Charging : Timer 3Hours



@enduml


 
 */