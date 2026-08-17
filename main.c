
/*
 * MAIN Generated Driver File
 * @file main.c
 * @defgroup main MAIN
 * @brief This is the generated driver implementation file for the MAIN driver.
 * @version MAIN Driver Version 1.0.0
  *@Target: AVR64DD14 microcontroller
  *@Author Dario Kusters  
  * Controlar y revisar :
  *     Si aun queda algo de prog. para SPI
  * 
  * 
  * 
//TCD0
// PIN2	in		ONbtn
// PIN3	UPDI
// PIN4	TxD
// PIN5	RxD
// PIN6     out     OFF_SMPS
// PIN7     out     PWR_SW
// PIN8     out     Buzzer

// PIN10    WOC     LED_R	PWMset[0]
// PIN11    WOD    	LED_G	PWMset[1]
// PIN12    adc_ch6 Ain_I	
// PIN13    adc_ch7 Ain_V
//
*/

#include "mcc_generated_files/system/system.h"

#include "include/globals.h"

void command_interpreter_task(void);

/*
    Main application
*/

//App vars
volatile uint16_t adcValues[2] = {0, 0}; // inicializar con los valores del ADC
uint16_t PWMset[2] = {0};     // [0]:LED_R, [1]:LED_G contiene el setpoint de cada PWM
uint16_t LED_R = 0, LED_G = 0;
// Valores de miedicones convertidos a voltaje y Corriente.
float V_batt = 0;
float I_batt = 0;
char V_battStr[10] = {0};
char I_battStr[10] = {0};
uint16_t Batt_cycles = 0; //Cuenta la cantidad de ciclos (Carga/descarga) de la abateria.)

char adcStr[2][10] = {0};      //Valores AIN4..AIN7 convertidos a tipo string
char PWMsetStr[2][10] = {0};   //Valores de setpoint PWM pasados a string
// Variables globales para almacenar la hora, minutos y segundos
volatile uint8_t RTC_hh = 0;
volatile uint8_t RTC_mm = 0;
volatile uint8_t RTC_ss = 0;
uint8_t cnt100mS = 0;
//declaracion de variables del tiempo que lleva encendido la mini PC
uint16_t PC_ON_hh = 0;
uint16_t PC_ON_mm = 0;
uint16_t PC_ON_ss = 0;

// Buzzer control
uint8_t AL_timeVal;
uint8_t *AL_selected;
uint8_t AL_index = 0;
// Patrones de alarma: {n x 0,1Sec buzzer ON, n x 0,1Sec buzzer OFF, y se repite secuencia, 0  secuencia fin.}
uint8_t alarm1[] = {12, 4, 0};      //NEW_BATT    
uint8_t alarm2[] = {1, 2, 0}; // Batt under 30%
uint8_t alarm3[] = {2, 2, 0}; // Batt under 20%
uint8_t alarm4[] = {1, 1, 1, 2, 0}; // PC_schhutdown
uint8_t alarm5[] = {1, 1, 5, 3, 5, 0};


// Declaracion de ESTADOS del state-machine que controla la carga/descarga
// Funcionamiento del BMS:
// 

typedef enum {
    STATE_POWER_ON,     // Inicilizacion de power_ON
    STATE_BMS_IDLE,     // BMS in rust toestand SMPS is OFF
    STATE_NEW_BATT_Init, // .
    STATE_NEW_BATT,           // Bij POR binnen 10 sec button press geeft aan nieuwe batterijen.
    STATE_SMPS_ON,      // is button gedrukt.
    STATE_SMPS_OFF,     // 
    STATE_Batt_FULL,    // 
    STATE_Batt_LOW,     //
    STATE_Batt_Charging,// En carga: LED_G se enciende suavemente durante 2Sec y se apaga, ciclicamente
    STATE_PC_ON,
} StateType;

typedef enum {
    SUB_NONE,
    SUB_Batt_95_PCT,
    SUB_Batt_80_PCT,
    SUB_Batt_30_PCT,
    SUB_Batt_20_PCT,
    SUB_PC_shutdown,
} SubStateType;

// Variables globales o locales según necesite.
StateType CURRENT_STATE = STATE_POWER_ON;
StateType NEXT_STATE = STATE_POWER_ON;

SubStateType CURRENT_SUB_STATE = SUB_NONE;
SubStateType NEXT_SUB_STATE = SUB_NONE;

//_________________________________________________________________________

//#include <stdint.h>
//#include <stdbool.h>
//#include <stdlib.h>  // Para la función abs()

typedef enum {
    NO_EVENT,
    CHARGER_CONNECTED,
    LOAD_DISCONNECTED
} BatteryEvent;

#define BUFFER_SIZE 60
#define VOLTAGE_THRESHOLD 20
#define MIN_EVENT_DURATION 30

static uint16_t voltage_buffer[BUFFER_SIZE];
static uint8_t buffer_index = 0;
static uint32_t voltage_sum = 0;
//static bool buffer_full = false;

BatteryEvent check_event(uint16_t new_adc_value);

uint8_t NoCurrent_Cnt = 0;



//_________________________________________________________________________

//vars dentro del STATE MAchINE
uint8_t TimerSec = 10;
uint8_t Timer1mS = 0;
uint8_t Timer100mS = 0;
uint8_t heartbeatcnt = 3;
uint8_t btn_counter = 0;
uint16_t i = 0; //aux var
uint16_t Timer_shutdown = 600; // seconden
uint16_t Min_i_var = 0;// Variable modificable iniciada con el valor de la constante



int main(void)
//  Main esta dividido en 3 partes separadas en el code por lineas "=====" 
//  1-  Main_header: Todo lo que debe ejecutarse al principio de main
//  2-  Main_body:   Cuerpo principal del Main.
//  3-  Main_end:    todo lo que debe ejecutarse al final de Main.
{
    SYSTEM_Initialize();
    RTC_Initialize();   // overflow cada segundo
    TCB0_Initialize();  // overflow cada 1 milisegundo
    TCD0_Initialize();  // Genera los PWM 1 y 2 para el LED bicolor Rojo Verde
    ADC0_Initialize();  // Mide voltage y corriente de las baterias
    NVM_Initialize();
    TCD0_Start();
    RTC_Start();
    TCB0_Start();

    //Inicializa General Purpose Register 0.
    GPR.GPR0 = 0x00;
    GPR.GPR1 = 0x00;
    PWMset[0]  = 0;     //LED Rojo
    PWMset[1]  = 0;     //LED Verde
    // Para calculo de promedio Vbatt
    #define MAX_SAMPLES 16	// cantidad de mediciones por segundo adc.

//    uint16_t V_batt_Buffer[MAX_SAMPLES]; // Buffer circular para V_batt
//    uint16_t I_batt_Buffer[MAX_SAMPLES]; // Buffer circular para I_batt

    uint16_t V_batt_AVG = 0;           // Valor promedio de V_batt
    uint16_t I_batt_AVG = 0;           // Valor promedio de I_batt

    uint8_t bufferIndex = 0;           // Índice para el buffer circular
    uint8_t measurementCount = 0;      // Cantidad de mediciones hechas

    uint16_t V_batt_Sum = 0;           // Suma total de V_batt
    uint16_t I_batt_Sum = 0;           // Suma total de I_batt
    
    
    // Variables para commando ADC Log. <AL<CR>
    //char V_batt_BufferStr[5]; // Asumiendo un valor máximo de 4095
    //char I_batt_BufferStr[5]; // Asumiendo un valor máximo de 4095

    // Umbrales de descarga debaterias. Mirar en main para explicacion.
    #define Vbatt_100_PCT 3437   //Vbatt = 27,50V
    #define Vbatt_80_PCT  3260   //Vbatt = 26,08V
    #define Vbatt_50_PCT  2975   //Vbatt = 23,81V
    #define Vbatt_40_PCT  2880   //Vbatt = 23,05V
    #define Vbatt_30_PCT  2785   //Vbatt = 22,28V
    #define Vbatt_20_PCT  2690   //Vbatt = 21,52V
    #define M_PENDIENTE ((4095LL * 1000LL) / (Vbatt_80_PCT - Vbatt_30_PCT))
    
    #define TimeOut_OFF_SMPS_const 3 // tiempo en segundos apagado SMPS si I<30mA
    //uint8_t TimeOut_OFF_SMPS = TimeOut_OFF_SMPS_const;
    #define MINIMALE_CURRENT    29  // Valor mínimo del ADC para corriente = 0,000A

   
 
    GPR_GPR2 |= (1 << bitNoCurrent);  // =1 cuando 3x Sec current < Min_i_var
    
    printf("--- BMS-V1 Module\r\nType ? for help\r\n");
    Min_i_var = MINIMALE_CURRENT;
 
    while(1)
    {
        //================  Main_header  BEGIN =================================
        // bit1mS_Main Set si TCB0 ha llegado a contar 1mS
        if (GPR_GPR0 & (1 << bit1mS_int)) {
            GPR_GPR0 &= ~(1 << bit1mS_int); // reset bit int
            GPR_GPR1 |= (1 << bit1mS_Main); // Set bit1mS Main
            cnt100mS++;
            if (cnt100mS==99){
                cnt100mS = 0;
                GPR_GPR1 |= (1 << bit100mS_Main);
            }
 
        }// bit1mS_Main Fin ************************
        
        // bit1Sec_Main Set si RTC ha llegado a contar 1 Segundo
        if (GPR_GPR0 & (1 << bit1Sec_int)) {
            GPR_GPR0 &= ~(1 << bit1Sec_int); // reset bit int
            GPR_GPR1 |= (1 << bit1Sec_Main);
            
            
        }// bit1Sec_Main ***************************
        
         // Si se recibio un CR ejecutar command interpreter
        if (GPR_GPR0 & (1 << 0)) {
            command_interpreter_task();
            GPR_GPR0 &= ~(1 << 0);
        } 
 
        // Si hay nuevos valores en el ADC, agregarlos al buffer y sumarlos
        if (GPR_GPR0 & (1 << bitADCvalues)) {
            GPR_GPR0 &= ~(1 << bitADCvalues); // Apagar el flag de nuevos valores ADC
            
            // Nuevos valores al buffer
//            V_batt_Buffer[bufferIndex] = adcValues[0];
//            I_batt_Buffer[bufferIndex] = adcValues[1];

            // Sumar los nuevos valores
            V_batt_Sum += adcValues[0];
            I_batt_Sum += adcValues[1];

            // Incrementar el índice del buffer circular y el contador de mediciones
            bufferIndex = (bufferIndex + 1) % MAX_SAMPLES;
            measurementCount++;
        }
        //Commando LOG
        if ((GPR_GPR1 & (1 << bit1Sec_Main)) && (GPR_GPR1 & (1 << LOG_cmd))){
            V_batt = ((float)V_batt_AVG*8) / 1000.0;
            I_batt = I_batt_AVG * 8;
            I_batt /= 10000;
            sprintf(adcStr[0], "%u", V_batt_AVG);
            sprintf(adcStr[1], "%u", I_batt_AVG);
            sprintf(V_battStr, "%5.3f", V_batt);
            sprintf(I_battStr, "%5.3f", I_batt);
            sprintf(PWMsetStr[0], "%u", PWMset[0]);
            sprintf(PWMsetStr[1], "%u", PWMset[1]);
            printf("AIN1=%4s %5sV R=%4s G=%4s AIN2=%4s %5sA\r", adcStr[0], V_battStr, PWMsetStr[0], PWMsetStr[1], adcStr[1], I_battStr);
            
        }
        // Comando AL (ADC Log)
        if ((GPR_GPR1 & (1 << bit1Sec_Main)) && (GPR_GPR1 & (1 << adc_Log_cmd))){
            V_batt = ((float)V_batt_AVG*8) / 1000.0;
            I_batt = I_batt_AVG * 8;
            I_batt /= 10000;
            sprintf(adcStr[0], "%u", V_batt_AVG);
            sprintf(adcStr[1], "%u", I_batt_AVG);
            sprintf(V_battStr, "%5.3f", V_batt);
            sprintf(I_battStr, "%5.3f", I_batt);

            printf("%02u:%02u:%02u  %5.3fV  %5.3fA\r", RTC_hh, RTC_mm, RTC_ss, V_batt, I_batt);

        }
        


        //================  Main_header  END ===================================
           
        //================  Main_body  BEGIN ===================================
//****** Procesos cada 1 Segundo ****
        if (GPR_GPR1 & (1 << bit1Sec_Main)) {
            //******** Inicio del cálculo de tiempo PC_ON. ************
            if(!OFF_SMPS_GetValue()){  // CSi SMPS is ON
                // Incrementa el contador de segundos
                PC_ON_ss++;
                if (PC_ON_ss >= 60) {   // Incrementa los minutos y 
                    PC_ON_mm++;         // reinicia segundos si necesario
                    PC_ON_ss = 0;
                }
                if (PC_ON_mm >= 60) {   // Incrementa las horas y 
                    PC_ON_hh++;         // reinicia minutos si necesario
                    PC_ON_mm = 0;
                }
                if (PC_ON_hh >= 24) {   // Reiniciar horas si nuwvo dia.
                    PC_ON_hh = 0; 
                }
            }//Calculo SMPS ON Fin*************************            
            
            // Calculo del promedio adcvalues en buffer
                V_batt_AVG = V_batt_Sum / measurementCount;
                I_batt_AVG = I_batt_Sum / measurementCount;
                // Resetear las variables para la próxima ronda de mediciones
                V_batt_Sum = 0;
                I_batt_Sum = 0;
                measurementCount = 0;
            
            // ***** Niveles de carga de baterias ****************
            if (V_batt_AVG > Vbatt_80_PCT) {
                    LED_R = 0;
                    LED_G = 4095;
            } else if (V_batt_AVG <= Vbatt_80_PCT && V_batt_AVG > Vbatt_30_PCT) {
                    LED_G = (uint16_t)((M_PENDIENTE * (V_batt_AVG - Vbatt_30_PCT)) >> 10); 
                    LED_R = 4095 - LED_G;
            } else {
                    LED_R = 4095;
                    LED_G = 0;
            }
            // ***** FIN Niveles de carga de baterias ****************
            
            
        } // Fin procesos cada 1 Segundo
        
        //**********************************************************************
        // BMS  -- Valores calculados en Excel: BMS_V1.xls
        //   100% de carga: 27,50V  adc = 3437
        //    80% de carga: 26,08V  adc = 3260
        //    
        //    30% de carga: 22,28V  adc = 2785 
        //    20% de carga: 21,52V  adc = 2690
        //    Nivel 1 (carga > 80%): > 26,08V
        //    Nivel 2 (carga entre 80% y 30%): entre 26,08V y 22,28V
        //    Nivel 3 (carga entre 30% y 20%): entre 22,28V y 21,52V
        //    Nivel 4 (carga < 20%): < 21,52V





//************************************************************************        
//********************* BMS_FSM  begin*******************************************

        switch (CURRENT_STATE) {
            case STATE_POWER_ON:
                OFF_SMPS_SetHigh();
                if _1Sec_PULSE {
                    TimerSec--;
                    if (PWMset[1] == 0) PWMset[1] = 4095;
                    else PWMset[1] = 0;
                }// {Timer 10Sec, TimerClr Button}
                if (TimerSec == 0) NEXT_STATE = STATE_BMS_IDLE;     // "+10 Sec"
                else if (!ONbtn_GetValue()) NEXT_STATE = STATE_NEW_BATT_Init; // "Button"
                break;

            case STATE_NEW_BATT_Init:
                // Aqui inicializo las bariables correspondientes a la nueva Bateria.
                GPR_GPR0 |= (1 << bitBuzzer);
                AL_selected = alarm1;
                TimerSec = 0;
                printf("New Batteries\r\n");
                NEXT_STATE = STATE_NEW_BATT;     // "Button release"
                break;
            case STATE_NEW_BATT: // En este estado solo retardo para pasar al proximo.
                if(ONbtn_GetValue() && _100mS_PULSE) Timer100mS++;
                if (Timer100mS == 3){
                    NEXT_STATE = STATE_BMS_IDLE;     // "Button release"
                }
                break;

            case STATE_BMS_IDLE:
                // Manejo de leds:
                if _100mS_PULSE{
                    heartbeatcnt--;
                    if (heartbeatcnt == 0){
                        if (PWMset[0] == 0){   // && (PWMset[1] == 0)){
                            PWMset[0] = LED_R;
                            PWMset[1] = LED_G;
                            heartbeatcnt = 2;
                        } else {
                            PWMset[0] = 0;
                            PWMset[1] = 0;
                            heartbeatcnt = 16;
                        }
                    }
                }
                // {LED PWM}
                if (!ONbtn_GetValue()) NEXT_STATE = STATE_SMPS_ON;         // "Button"
                else if _bitBatt_LOW NEXT_STATE = STATE_Batt_LOW;   // "Vbatt < 30%"
               // else if _bitCharger_ON NEXT_STATE = STATE_Batt_Charging;  // "Vbatt Up"
             break;

            case STATE_SMPS_ON:
                if _100mS_PULSE {
                    PWMset[0] = 0;
                    if (PWMset[1] == 0) PWMset[1] = 3000;
                    else PWMset[1] = 0;
                }
                if(OFF_SMPS_GetValue()){
                    OFF_SMPS_SetLow();
                    printf("SMPS ON\r\n");
                    TimerSec = 60;
                } else {
                    if _1Sec_PULSE TimerSec--;
                    if (TimerSec == 59){
                        PWR_SW_SetHigh();
                    } else {
                        PWR_SW_SetLow();
                    }
                    if (TimerSec == 0){
                        OFF_SMPS_SetHigh();
                        printf("SMPS OFF\r\n");
                        NEXT_STATE = STATE_BMS_IDLE;  
                    }     // "TimeOut"
                    else if (I_batt_AVG > 125) NEXT_STATE = STATE_PC_ON;  // "IBatt > 100mA"
                }
                break;

            case STATE_PC_ON:
                // Código común para STATE_PC_ON aquí
                // Aca preguntar si I_batt_AVG < 13 (10mA aprox)
                //Aca si btn pulsado mas de 5 segundos. Shutdwn
                
                
                
                if(_1Sec_PULSE){
                    if(_bitNoCurrent){
                        Timer_shutdown = 2;
                        NEXT_SUB_STATE = SUB_PC_shutdown;
                    }
                    if (!ONbtn_GetValue()) {
                        // El botón está pulsado (entrada baja)
                        btn_counter++;  // durante 5 seggundos
                        if(btn_counter > 5){
                         btn_counter = 0;
                         NEXT_STATE = STATE_SMPS_OFF;
                        }
                    }else{
                     btn_counter = 0;   
                    }
                    }
                
                
                

                // Si el estado principal cambia, reseteamos el subestado
                if (CURRENT_STATE != NEXT_STATE) {
                    CURRENT_SUB_STATE = SUB_NONE;
                }

                // Actualiza el subestado actual solo si hay un cambio.
                if (CURRENT_SUB_STATE != NEXT_SUB_STATE) {
                    CURRENT_SUB_STATE = NEXT_SUB_STATE;
                }

                switch (CURRENT_SUB_STATE) {
                    case SUB_NONE:
                        // Determinar % de carga de bateria con corriente. 
                        if (V_batt_AVG > Vbatt_80_PCT) NEXT_SUB_STATE = SUB_Batt_95_PCT;
                        else if (V_batt_AVG > Vbatt_30_PCT) NEXT_SUB_STATE = SUB_Batt_80_PCT;
                        else if (V_batt_AVG > Vbatt_20_PCT) NEXT_SUB_STATE = SUB_Batt_30_PCT;
                        else NEXT_SUB_STATE = SUB_Batt_20_PCT;
                        // Aquí podrías también manipular NEXT_STATE si es necesario
                        break;
                        
                    case SUB_Batt_95_PCT:
                        if _1Sec_PULSE{
                            PWMset[0] = 0;
                            PWMset[1] = 4090;
                        }
                        if (V_batt_AVG <= Vbatt_80_PCT) NEXT_SUB_STATE = SUB_Batt_80_PCT;
                        break;

                    case SUB_Batt_80_PCT:
                        if _1Sec_PULSE{
                            PWMset[0] = LED_R;
                            PWMset[1] = LED_G;
                        }
                        if (V_batt_AVG < Vbatt_30_PCT) {
                            NEXT_SUB_STATE = SUB_Batt_30_PCT;
                            i = 1;
                        }

                        break;
                    case SUB_Batt_30_PCT:
                        if _1Sec_PULSE{
                            GPR_GPR2 &= ~(1 << bitBatt_LOW);
                            i--;
                            if (i == 0 ){
                                GPR_GPR0 |= (1 << bitBuzzer);
                                AL_selected = alarm2;
                                i = 60;
                            }
                            PWMset[1] = 0;
                            if (PWMset[0] == 0) PWMset[0] = 4095;
                            else PWMset[0] = 0;
                        }
                        if (V_batt_AVG < Vbatt_20_PCT) {
                            NEXT_SUB_STATE = SUB_Batt_20_PCT;
                            Timer_shutdown = 600;
                        }

                        // ...
                        break;
                    case SUB_Batt_20_PCT:
                        if _100mS_PULSE{
                            i--;
                            if (i == 0 ){
                                GPR_GPR0 |= (1 << bitBuzzer);
                                AL_selected = alarm3;
                                i = 100;
                            }
                            PWMset[1] = 0;
                            if (PWMset[0] == 0) PWMset[0] = 4095;
                            else PWMset[0] = 0;
                        }
                        if _1Sec_PULSE {
                            GPR_GPR2 &= ~(1 << bitBatt_LOW);
                            if(Timer_shutdown != 0) Timer_shutdown--;
                            if ((Timer_shutdown == 0)||(_bitNoCurrent)){
                                NEXT_SUB_STATE = SUB_PC_shutdown;
                                Timer_shutdown = 120;
                            }
                        }
                        // ...
                        break;
                        
//PC_Shutdown solo puede venir de SUB_Batt_20_PCT porque la batt es muy baja NO la corriente                        
                    case SUB_PC_shutdown: //Recibe Timer_shutdown = 120
                        if _100mS_PULSE{
                            PWMset[1] = 0;
                            if (PWMset[0] == 300) PWMset[0] = 4095;
                            else PWMset[0] = 300;
                        }
                        if _1Sec_PULSE{
                            GPR_GPR0 |= (1 << bitBuzzer);
                            AL_selected = alarm4;
                            if (Timer_shutdown != 0) Timer_shutdown--;
                            if (Timer_shutdown > 114) PWR_SW_SetHigh();
                            else PWR_SW_SetLow();
                            if ((_bitNoCurrent )||(Timer_shutdown == 0)){
                                NEXT_SUB_STATE = SUB_NONE;
                                NEXT_STATE = STATE_SMPS_OFF;
                               // Timer_shutdown = 600;
                            }
                        }
                        break;
                    default:
                        // Código para subestado desconocido o no aplicable
                        NEXT_SUB_STATE = SUB_NONE;
                        break;
                }
                break;


// En este estado calcular la energia restante en baterias, la sumantoria de horas de las
//baterias (Lifetime) y guardar datos en EPROM,                
            case STATE_SMPS_OFF:
                if (!OFF_SMPS_GetValue()){
                    OFF_SMPS_SetHigh();
                    printf("SMPS is OFF\r\n");
                    PWMset[0] = 0;
                    PWMset[1] = 0;
                }
                cnt100mS = 3; //Para el LED en STATE_Batt_LOW

				if (ONbtn_GetValue()) {
                    if (bitBatt_LOW){
                        NEXT_STATE = STATE_Batt_LOW;  // "VBatt < 40% OR bitBatt_LOW"
                        //else if (V_batt_AVG > Vbatt_30_PCT) NEXT_STATE = STATE_BMS_IDLE;
                        //else if (que el LOAD_Disconnected AAN y haya pasado los 40% y luego UIT)
                        //para ir al idle 
                    } else NEXT_STATE = STATE_BMS_IDLE;
                }
            break;   

            case STATE_Batt_LOW:
                if(_100mS_PULSE){
                    cnt100mS--;
                    if (cnt100mS == 0){
                        cnt100mS = 3;
                        PWMset[0] = 0;
                        if (PWMset[1] == 0) PWMset[1] = 2023;
                            else PWMset[1] = 0;
                    }
                }
                
                if (_1Sec_PULSE){
                    if (_bitBatt_LOW) GPR_GPR2 &= ~(1 << bitBatt_LOW); // Clear flag
                }
                
                // {Ignore recovery, clear bitBatt_LOW}
                if ((_bitCharger_ON) || (V_batt_AVG > Vbatt_50_PCT)) NEXT_STATE = STATE_Batt_Charging;  // "Vbatt > 50%"
                break;

            case STATE_Batt_Charging:
                if(_1Sec_PULSE){
                    if (V_batt_AVG > Vbatt_80_PCT) GPR_GPR2 &= ~(1 << bitCharger_ON); //Clear flag charger ON
                }
                // {Timer 3Hours} y despues puede esto
                if((!ONbtn_GetValue())&&(V_batt_AVG > Vbatt_50_PCT)) NEXT_STATE = STATE_BMS_IDLE;
                else if (V_batt_AVG > 3558) NEXT_STATE = STATE_Batt_FULL;      // "Vbatt > 28,52V"
                
                if _1Sec_PULSE{
                    if ((PWMset[0] == 0) && (PWMset[1] == 0)){
                        PWMset[0] = LED_R;
                        PWMset[1] = LED_G;
                    } else {
                        PWMset[0] = 0;
                        PWMset[1] = 0;
                    }
                }
                break;

                
            case STATE_Batt_FULL:
                // {cycle++, data to EPROM}
                if (1) NEXT_STATE = STATE_BMS_IDLE;    // "VBatt = 27,5V"
                break;
                
                // Incluir estados adicionales y sus transiciones aquí...

            default:
                // Manejo de error o reinicio al estado inicial.
                NEXT_STATE = STATE_POWER_ON;
                break;
        }

        // Actualiza el estado actual solo si hay un cambio.
        if (CURRENT_STATE != NEXT_STATE) {
            CURRENT_STATE = NEXT_STATE;
        }
        

        // Tu código aquí después del switch-case.

        
//********************* BMS_FSM  End*******************************************
//************************************************************************        
        
        
         
        
        
        //================  Main_body  END =====================================

        //================  Main_end  BEGIN ====================================

    
        // // Control del Buzzer
        if (GPR_GPR0 & (1 << bitBuzzer)) {
            if(GPR_GPR1 & (1 << bit100mS_Main)) {
                if(AL_index == 0) {
                    AL_timeVal = AL_selected[AL_index];
                    AL_index++;
                    Buzzer_SetHigh();
                } else {
                    AL_timeVal--;
                    if(AL_timeVal == 0) {
                        Buzzer_Toggle();
                        AL_timeVal = AL_selected[AL_index];
                        AL_index++;
                        if(AL_timeVal == 0) {
                            AL_index = 0;
                            Buzzer_SetLow(); // Asegurarse de que el buzzer se apague al finalizar la alarma
                            GPR_GPR0 &= ~(1 << bitBuzzer);
                        }
                    }
                }
            }
        }// Buzzer Ctrl Fin ************

        
        // BMS flags
        if _1Sec_PULSE{
            BatteryEvent event = check_event(V_batt_AVG);
        
            switch(event) {
                case NO_EVENT:
                    // Ningún evento detectado
                    break;
                case CHARGER_CONNECTED:
                    // Código para manejar la conexión del cargador
                    GPR_GPR2 &= ~(1 << bitBatt_LOW);    //Clear Batt_LOW flag
                    GPR_GPR2 |= (1 <<bitCharger_ON );
                    
                    printf("CHARGER_CONNECTED\n\r");

                    break;
                default:
                    event = NO_EVENT;
                    
                    break;
            }
        
        // bitNoCurrent se setea (no current) solo si se mide 3 segundos seguidos  I = 0)
            if (I_batt_AVG > Min_i_var){
                GPR_GPR2 &= ~(1 << bitNoCurrent);  //clear bit
                NoCurrent_Cnt = 0;
            } else {
                NoCurrent_Cnt++;
                if (NoCurrent_Cnt >= 3){
                    NoCurrent_Cnt = 0;
                    GPR_GPR2 |= (1 << bitNoCurrent);
                }
            }

        }
        
        GPR_GPR1 &= ~((1 << bit1mS_Main) | (1 << bit100mS_Main) | (1 << bit1Sec_Main));
        //================  Main_end  END ======================================

        //*********************************************************************
    }    
}

#include <string.h>
#include <stdlib.h>

#define USART0_RX_BUFFER_SIZE 128 

void command_interpreter_task() {
    extern uint8_t usart0RxBuffer[USART0_RX_BUFFER_SIZE];
    static uint8_t rxBufferIndex = 0; 
    char commandBuffer[USART0_RX_BUFFER_SIZE];
    uint8_t commandLength = 0;

    while (usart0RxBuffer[rxBufferIndex] != '\r' && rxBufferIndex < USART0_RX_BUFFER_SIZE) {
        commandBuffer[commandLength++] = usart0RxBuffer[rxBufferIndex];
        usart0RxBuffer[rxBufferIndex] = 0; 
        rxBufferIndex = (rxBufferIndex + 1) % USART0_RX_BUFFER_SIZE;
    }

    if (commandLength > 0 && usart0RxBuffer[rxBufferIndex] == '\r') {
        commandBuffer[commandLength] = '\0';
        usart0RxBuffer[rxBufferIndex] = 0; 
        rxBufferIndex = (rxBufferIndex + 1) % USART0_RX_BUFFER_SIZE;

        if (commandLength == 1 && commandBuffer[0] == '?') {
            printf("\r\nCommands:\r\nTIME OFF\r\nRead:\r\nPWMn  TIME  TON  AIN LOG AL\r\n\r\nn: 1 to 2\r\nx: 0 to 4095\r\nuse UPPER CASE.\r\n");
            return; 
        }
        else if (commandLength >= 4 && strncmp(commandBuffer, "PWM", 3) == 0 
                 && commandBuffer[3] >= '1' && commandBuffer[3] <= '2') {
            int pwmNumber = commandBuffer[3] - '1';
            if (commandLength == 4) {
                printf("PWM%d=%d\r\n", pwmNumber + 1, PWMset[pwmNumber]);
            } 
            else if (commandLength > 5 && commandBuffer[4] == '=') {
                int pwmValue = atoi(&commandBuffer[5]);
                if (pwmValue >= 0 && pwmValue <= 4095) {
                    PWMset[pwmNumber] = pwmValue;
                    printf("PWM%d set to %d\r\n", pwmNumber + 1, pwmValue);
                } else {
                    printf("?\r\n"); 
                }
            } else {
                printf("?\r\n");
            }
        } 
        else if (commandLength == 3 && strncmp(commandBuffer, "AIN", 3) == 0) {
            sprintf(adcStr[0], "%u", adcValues[0]);
            sprintf(adcStr[1], "%u", adcValues[1]);
            printf("AIN1=%4s AIN2=%4s \r\n", adcStr[0], adcStr[1]);
        } 
        else if (commandLength == 3 && strncmp(commandBuffer, "PWM", 3) == 0) {
            sprintf(PWMsetStr[0], "%u", PWMset[0]);
            sprintf(PWMsetStr[1], "%u", PWMset[1]);
            printf("PWM1=%4s PWM2=%4s\r\n", PWMsetStr[0], PWMsetStr[1]); 
        }
        else if (commandLength == 3 && strncmp(commandBuffer, "LOG", 3) == 0) {
            if (GPR_GPR1 & (1 << LOG_cmd)) {
                GPR_GPR1 &= ~(1 << LOG_cmd);
                printf("LOG OFF\r\n");
            } else {
                GPR_GPR1 |= (1 << LOG_cmd);
                printf("LOG ON\r\n");
            }
        }
        else if (commandLength == 2 && strncmp(commandBuffer, "AL", 2) == 0) {
            if (GPR_GPR1 & (1 << adc_Log_cmd)) {
                GPR_GPR1 &= ~(1 << adc_Log_cmd);
                printf("AL OFF\r\n");
            } else {
                GPR_GPR1 |= (1 << adc_Log_cmd);
                printf("AL ON\r\n");
            }
        }
        //---------------------------------------------------------        
        else if (strncmp(commandBuffer, "TIME", 4) == 0) {
            if (commandLength == 4) {
                printf("CPU Time is %02u:%02u:%02u.\r\n", RTC_hh, RTC_mm, RTC_ss);
            }
            else if (commandBuffer[4] == ' ' && commandBuffer[7] == ' ' && commandBuffer[10] == ' ') {
                RTC_hh = atoi(&commandBuffer[5]);
                RTC_mm = atoi(&commandBuffer[8]);
                RTC_ss = atoi(&commandBuffer[11]);
                printf("CPU Time set to %02u:%02u:%02u.\r\n", RTC_hh, RTC_mm, RTC_ss);
            }
            else {
                printf("?\r\n");
            }
        }

        else if (commandLength >= 4 && strncmp(commandBuffer, "MINI", 4) == 0) {
            // Si el comando es solo "MINI", se muestra el valor actual.
            if (commandLength == 4) {
                printf("MINI = %u\r\n", Min_i_var);
            }
            // Si el comando tiene el formato "MINI=x", se actualiza el valor.
            else if (commandBuffer[4] == '=') {
                uint16_t new_val = (uint16_t)atoi(&commandBuffer[5]);
                Min_i_var = new_val;
                printf("MINI set to %u\r\n", Min_i_var);
            }
            else {
                printf("?\r\n");
            }
        }
        else if (strncmp(commandBuffer, "TON", 3) == 0) {
            if (commandLength == 3) {
                printf("Total time ON:  %02u hours %02u min\r\n", PC_ON_hh, PC_ON_mm);
            }        
}
        
        
//---------------------------------------------------------    
        
//---------------------------------------------------------
// Agregando la nueva condición para el comando "ON"
else if (commandLength == 2 && strncmp(commandBuffer, "ON", 2) == 0) {
    OFF_SMPS_SetLow();  // Llama a la función cuando el comando "ON" es recibido
    printf("SMPS Power ON.\r\n");  // Notificación al usuario
}
//---------------------------------------------------------
//---------------------------------------------------------
// Agregando la nueva condición para el comando "OFF"
else if (commandLength == 3 && strncmp(commandBuffer, "OFF", 3) == 0) {
    OFF_SMPS_SetHigh();  // PIN  = 0
    printf("SMPS Power OFF.\r\n");  // Notificación al usuario
}
// Buzzer test
else if (commandLength == 3 && strncmp(commandBuffer, "BUZ", 3) == 0) {
        Buzzer_Toggle();
}
//---------------------------------------------------------
       
        else {
            printf("?\r\n");
        }
        commandLength = 0;
    }
}





BatteryEvent check_event(uint16_t new_adc_value) {
    static uint8_t charger_counter = 0;  // Hacerlo estático para mantener el estado entre llamadas
    
    // Actualizar el búfer circular y la suma de los voltajes
    voltage_sum -= voltage_buffer[buffer_index];
    voltage_buffer[buffer_index] = new_adc_value;
    voltage_sum += new_adc_value;
    
    // Actualizar el índice del búfer
    buffer_index = (buffer_index + 1) % BUFFER_SIZE;
    
    // Calcular el voltaje medio
    uint16_t avg_voltage = voltage_sum / BUFFER_SIZE;
    
    // Verificar eventos basados en la desviación del promedio
    int16_t delta = new_adc_value - avg_voltage;
    if (delta > VOLTAGE_THRESHOLD) {
        charger_counter++;
    } else {
        charger_counter = 0;  // Reiniciar el contador si la condición no se cumple
    }
    
    // Decidir si ha ocurrido el evento de conexión del cargador
    if (charger_counter >= MIN_EVENT_DURATION) {
        charger_counter = 0;  // Reiniciar el contador después de detectar el evento
        return CHARGER_CONNECTED;
    }
    
    // Ningún evento de conexión del cargador detectado
    return NO_EVENT;
}
