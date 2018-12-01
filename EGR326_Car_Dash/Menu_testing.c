//**********************************************************************************
// Name(s): Nicklas Koeller && Mason Cordes
// Date: October 5, 2018
// Professor: Dr. Krug
// Description: This program is testing LCD for final Project Trying to get a main menu with highlighting different options
// Source(s):   The ST7735 Library, and code from Prof. Jim Valvano at Univ. of Texas. , and
//              code from Dr. Krug's Lecture 9 for EGR326-01 for the Fall 2018 Semester was used/modified
//**********************************************************************************
#include "driverlib.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "ST7735.h"
#include "Images.h"
#include "RTC.h"
#include "speed.h"
#include "ultraSonic.h"
#include "alarms.h"
#include "ADC.h"

#define writeIdleScreen     0
#define writeMainMenu       1
#define writeSetTimeSubMenu 2
#define writeDateSetSubMenu 3
#define writeErrorLogMenu   4
#define writeTempAlarm      5
#define writeSpeedAlarm     6

uint32_t SMCLKfreq,MCLKfreq; //Variable to store the clock frequencies
uint8_t nextDirection = 0; //no selections
uint8_t contact1 = 0, contact2 = 0, direction = 0;              //Rotary encoder vars
uint8_t hallEffectMagnetCounts = 0 , speed = 0;                 //variable for counting how many times the magnet passes the hall effect
volatile float normalizedADCRes, normalizedADCResBat;
static uint16_t resultsBuffer[2];
uint16_t noInputCounter = 0;                 //variabls used for detecting idle

//UltraSonic Vars
uint32_t ultraSonicRead1, ultraSonicRead2;    //Store timerA read for ultrasonic capcture
uint8_t triggerFinished = 0;                     //starts trigger for ultra sonic

//Alarm variables
uint8_t alarmData[20];

void clockInit48MHzXTL(void); //Function to set the clk to 48MHz external
void PORT1_IRQHandler(void);  //for both on board pushbuttons
void push_btn_init(void);


int main(void)
{
    MAP_WDT_A_holdTimer(); // Stop Watchdog
    clockInit48MHzXTL(); //Sets MCLK and SMCLK to the external HFXT crystal
    MAP_CS_initClockSignal(CS_SMCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_4);
    SMCLKfreq=MAP_CS_getSMCLK(); // get SMCLK value to verify it was set to 12 MHz
    MCLKfreq=MAP_CS_getMCLK(); // get MCLK value and verify it also
    Systick_Init(); //Initilaizes SysTick Timer
    ST7735_InitR(INITR_REDTAB); //Initializes LCD

    //variabls
    uint8_t direction = 0;
    int bumperDistance = 0;
    uint8_t state  = 0, nextState = 0;//controls the state
    uint8_t userSelection = 0;
    uint8_t timeArray[7]      = {1,2,5,6,0,0,0};       //send to all print functions to print time to screen, should be updated by RTC read atleast once a minute.
    uint8_t writeTimeToRTC[7];                         //for writing time data to RTC.
    float RTCtemp = 22.3;                              //used to take temperature from RTC and send to different functions
    uint8_t tempAlarmCheck = 0, speedAlarmCheck=0;                        //used so that only one alarm is triggered once

    uint8_t noInputReset = 0 , noInputState = 0;                          //used to reset LCD when user goes Idle

    //inits
    push_btn_init();
    rotaryPinInit();                   //init rotary encoder
    initRTC();                         //starts RTC on P1.6 and P1.7
    initMotorPWM();           //init PWM for motor
    initTimer32For100us();    //init Timer32 for 100us interrupt rate
    initHallEffectPins();     //init just read the name
    initSpeedometer();        //init just read name
    initBatteryometer();
    ADCBacklightInit();       //init ADC module and Backlight PWM
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P7, GPIO_PIN6);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P7, GPIO_PIN6);

    //First Time ultraSonic
    timerA2_init();           //init Timer A2 for ultrasonic
    ultraSonicPinInit();      //init trig pins and LEDs for ultrasonic
    MAP_Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_CONTINUOUS_MODE);
    triggerUltraSonic();    //trig pin for 10us

    //for testing speed read
    changeMotorPWMspeed(2);            //Sets to mid speed


    readFullRTC(timeArray);            //Gets init time from RTC
//      FOR HARD SETTING DATE
//    timeArray[6] = 0x18;               //the year does not seem to be staying
//    timeArray[5] = 0x11;               //month
//    timeArray[4] = 0x27;               //date
//    writeFullRTC(timeArray);           //Just for setting up the RTC properly


    MAP_Interrupt_enableMaster();           //enable interrupts

    //Print Splash Screen
    buddyCorp();
    systick_delay_ms(1000);                 //delay to see splash screen for 1s

    while(1)
    {

        switch (state)
        {
            case writeIdleScreen:
                userSelection = idleScreen(direction, RTCtemp, speed ,timeArray);

                if(userSelection)
                     nextState = writeMainMenu;
                else nextState = writeIdleScreen;

            break;
            case writeMainMenu:
                userSelection = writeMenu(direction , noInputReset);

                if(userSelection == 0)
                    nextState = writeMainMenu;
                else if(userSelection == 1)
                    nextState = writeSetTimeSubMenu;//next state = write set time
                else if(userSelection == 2)
                    nextState = writeDateSetSubMenu;//nextState = write the date to RTC
                else if(userSelection == 3)
                    nextState = writeErrorLogMenu;//nextState = something
                else if(userSelection == 4)
                    nextState = writeIdleScreen;//nextState = something
                else
                    nextState = writeIdleScreen;

                break;


            case writeSetTimeSubMenu:
                userSelection = setTimeSubMenu(direction, writeTimeToRTC, timeArray, noInputReset);

                if (userSelection)
                {
                    nextState = writeIdleScreen;
                    writeTimeOnly(writeTimeToRTC);
                    readTimeIn(timeArray);          //reads in RTC right after write becasue first read is garbage
                    //Send time to write to RTC here
                }
                break;

            case writeDateSetSubMenu:
                userSelection = setDateSubMenu(direction, writeTimeToRTC);

                if(userSelection == 1)
                {
                    nextState = writeIdleScreen;
                    //write new date to RTC
                }

                break;

            case writeErrorLogMenu:
                userSelection = writeErrorMenu(direction);

                if(userSelection == 1)
                    nextState = writeTempAlarm;//Write Temp error log
                if(userSelection == 2)
                    nextState = writeSpeedAlarm;//write Speed error logs

                break;

            case writeTempAlarm:
                userSelection = writeTempAlarmLog(direction);

                if(userSelection == 1)
                    nextState = writeIdleScreen;

                break;

            case writeSpeedAlarm:
                userSelection = writeSpeedAlarmLog(direction);

                if(userSelection == 1)
                    nextState = writeIdleScreen;

                break;
            default:
                ;//do nothing
        }

        //Stuff that needs to happen as often as possible without being super timing critical
        //aka:watchdog?

        //Setting flags for when user goes idle
        noInputReset = 0;
        if(nextState == noInputState)
        {
            noInputReset = 1;
            noInputState = 99;
        }
        if(direction != 0)
            noInputCounter=0;
        if(noInputCounter > 100)
        {
            noInputCounter = 0;     //restart counter
            //noInputReset = 1;       //flag for reseting LCD screens
            noInputState = nextState;//keep track of state that it happened in
            nextState = writeIdleScreen;
        }



        //encoder direction setting
        direction = nextDirection;
        nextDirection = 0;
        //every time the encoder nextdirection is not 0 reset a timer to count down from 1min
        //if direction = 0 for a minute reset next state to the idle screen

        //Read time and temp from RTC and set according vars
        readTimeIn(timeArray);
        RTCtemp = readRTCtemp(RTCtemp);

        //controlling next state
        state = nextState;
        //Print any updates to top banner
        if(state != writeIdleScreen)
        {
            topBannerPrint(RTCtemp,speed,timeArray);
        }
        //Change speedometer
        driveMotor(speed);


        //Ultra Sonic
        if(triggerFinished)
        {
            bumperDistance =  ((ultraSonicRead2*10)/12) /58;
            //Changes LED
            if(bumperDistance >= 200) {MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);proximityBannerAlarm(0);}
            if(bumperDistance < 200)  {MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);proximityBannerAlarm(1);}

            triggerUltraSonic();    //trig pin for 10us
            triggerFinished = 0;
        }


        //changed alarm threshold to 24 for testing purposes
        if(RTCtemp <= 24)//used so that only one alarm is written when the temp is over 43C       could also trigger buzzer here?
            tempAlarmCheck=1;
        if(RTCtemp > 24 && tempAlarmCheck)
        {
            //send alarm to save in flash
            sprintf(alarmData,"%02x/%02x/%02x at %02x:%02x", timeArray[5],timeArray[4],timeArray[6],  timeArray[2],timeArray[1]);
            saveToFlash(alarmData, 1 );        //write a temp alarm to flash
            tempAlarmCheck = 0;                 //used so that the alarm is not set continuously if temp stays high
        }


        if(speed <= 85)//used so that only one alarm is written when the temp is over 43C       could also trigger buzzer here?
            speedAlarmCheck=1;
        if(speed > 85 && speedAlarmCheck)
        {
            //send alarm to save in flash
            sprintf(alarmData,"%02x/%02x/%02x at %02x:%02x", timeArray[5], timeArray[4], timeArray[6], timeArray[2], timeArray[1]);
            saveToFlash(alarmData, 2 );        //write a temp alarm to flash
            speedAlarmCheck = 0;                 //used so that the alarm is not set continuously if temp stays high
        }


        updateBacklight(normalizedADCRes, normalizedADCResBat);



    }
}

/******************************
Name:         clockInit48MHzXTL
Description:  Sets up MSP to run off the external 48MHz Clock Source
Input:        none
Output:       none
Source(s):    Some of this code was taken from Dr. Krug's Lecture 7 for EGR 326-01 Fall 2018.
*******************************/
void clockInit48MHzXTL(void)
{  // sets the clock module to use the external 48 MHz crystal
    /* Configuring pins for peripheral/crystal usage */
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ,GPIO_PIN3 | GPIO_PIN2,GPIO_PRIMARY_MODULE_FUNCTION);
    CS_setExternalClockSourceFrequency(32000, 48000000); // enables getMCLK, getSMCLK to know externally set frequencies
    /* Starting HFXT in non-bypass mode without a timeout. Before we start
     * we have to change VCORE to 1 to support the 48MHz frequency */
    MAP_PCM_setCoreVoltageLevel(PCM_VCORE1);
    MAP_FlashCtl_setWaitState(FLASH_BANK0, 2);
    MAP_FlashCtl_setWaitState(FLASH_BANK1, 2);
    CS_startHFXT(false); // false means that there are no timeouts set,will return when stable
    /* Initializing MCLK to HFXT (effectively 48MHz) */
    MAP_CS_initClockSignal(CS_MCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
}


void push_btn_init(void)
{
    //push btn p1.1
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);
    MAP_Interrupt_enableInterrupt(INT_PORT1);

    //push btn p1.4
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN4);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN4);
    MAP_Interrupt_enableInterrupt(INT_PORT1);
}



/*-------------------------------------------------------------INTERRUPTS-------------------------------------------------------------*/



void PORT1_IRQHandler(void)
{
    uint32_t status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    if(status & GPIO_PIN1){
        nextDirection = 1;      //forward
    }
    if(status & GPIO_PIN4){
        nextDirection = 3;      //backwards
    }

}

/******************************
 Name:         PORT6IRQ_Handler
 Description:  Handler for the Port 4 Interrupt
 Input:        none
 Output:       none
 Source(s):    Some of this code was taken from Texas Instrument Example Code.
 *******************************/
void PORT6_IRQHandler(void)
{
    uint32_t status;   //Variable to hold the interrupt status

    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P6);  //Saves the interrupt status to status variable
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P6, status);  //Clears the interrupt flag

    //Rotary Encoder
    if (status & GPIO_PIN0) //If P4.0 caused the interrupt
    {
        if(contact1 == 1){
            contact1 = 0;
            contact2 = 0;
        }
        if ((contact2) == 0)
        {
            contact1 = 1;
        }
        if (contact2 == 1)
        {
            contact1 = 2;
        }
    }

    if (status & GPIO_PIN1) //If P4.1 caused the interrupt
        {
            if(contact1 == 2)
            {
                contact1 = 0;
                contact2 = 0;
            }
            if ((contact1) == 0)
            {
                contact2 = 1;
            }

            if (contact1 == 1)
            {
                contact2 = 2;
            }
        }

    if (contact2 == 2 || contact1 == 2)
    {
        nextDirection = contact1;
        contact1 = 0;
        contact2 = 0;
    }

    if (status & GPIO_PIN2) //If P4.2 caused the interrupt
    {
        nextDirection = 3;      //if the btn is pushed
    }

    //Hall Effect
    if (status & GPIO_PIN3)
    {
        hallEffectMagnetCounts++;
    }
}


void T32_INT1_IRQHandler(void)
{
    //This acts as a 100ms Timer interrupt
    static int blinkerCount = 5;


    MAP_Timer32_clearInterruptFlag(TIMER32_BASE);
    speed = hallEffectMagnetCounts;
    hallEffectMagnetCounts = 0;
    MAP_Timer32_setCount(TIMER32_BASE,4800000);
    MAP_Timer32_startTimer(TIMER32_BASE, true);

    if(!blinkerCount)
        {
        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P7, GPIO_PIN6);
        blinkerCount = 6;
        }
    blinkerCount--;

    //used to kick users out of an entry screen to return them to idle
    //after 1minute
    noInputCounter++;

}



//Interrupts
void TA2_N_IRQHandler(void)
{
    int rising = 0;
    if (TIMER_A2->CCTL[1] & BIT0) // Timer A2.1 on p5.6 was the cause. This is setup as a capture
        MAP_Timer_A_clearCaptureCompareInterrupt(TIMER_A2_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_1);// clear timer_A interrupt flag on 2.1

    if(P5->IN&BIT6) rising=1; else rising=0; // check for rising or falling edge on input

    if(rising)
    {
        //ultraSonicRead1 = MAP_Timer_A_getCaptureCompareCount(TIMER_A2_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_1); // read timer_A value
        //this value multiplied by 166.7nS gives time but dont forget the time is double because it travels there and back
        ultraSonicRead1 = 0;
        MAP_Timer_A_stopTimer(TIMER_A2_BASE);
        TA2R = 0;
        MAP_Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_CONTINUOUS_MODE);

    }
    if(!rising)
    {
        ultraSonicRead2 = MAP_Timer_A_getCaptureCompareCount(TIMER_A2_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_1); // read timer_A value
        if(ultraSonicRead2 < ultraSonicRead1) ultraSonicRead2 =+ 65536;             //change this to check overflow bit and reset it
        triggerFinished = 1;          //Tells main to calculate difference and find distance
    }
}

void ADC14_IRQHandler(void)
{
    uint64_t status = MAP_ADC14_getEnabledInterruptStatus();


    MAP_ADC14_clearInterruptFlag(status);

    if(status & ADC_INT1)
        {
            MAP_ADC14_getMultiSequenceResult(resultsBuffer);
        }
        normalizedADCRes = (resultsBuffer[0]) / 16384.0;
        normalizedADCResBat = ((resultsBuffer[1]) / 16384.0)*(3.3*1.065*4.108);

}
