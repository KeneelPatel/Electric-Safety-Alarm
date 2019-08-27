/*
 * main.c for Lab 8, spring 2018
 *
 * Created by Zhao Zhang
 */

#include <pwmbuzzer.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <inc/hw_memmap.h>
#include <inc/hw_ints.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include <driverlib/interrupt.h>
#include <driverlib/systick.h>
#include <driverlib/timer.h>
#include "launchpad.h"
#include "seg7.h"
#include "seg7digit.h"
#include "ranger.h"



static enum {Run, Pause} reverseState = Pause;
static enum {letter, number} displayState = number;
static enum {Active, Almost, Deactive} alarmState = Deactive;
//static enum {On, Off} redFlash = Off;

/*
 * PWM table for gradual changes.
 */
/*typedef struct {
    pwm_t pwm;              // PWM parameters
    uint16_t duration;      // Duration of this PWM segment
} pwmSeg_t;*/


//pulse width will change frequency, width - period will change volume, lower number = low volume
//duration is how long to wait before the next chord is called
/*pwmSeg_t pwnBuzzTable [] = {
    C{{191570, 1500}, 440},
    C{{191570, 1500}, 440},
    G{{127551, 1500}, 440},
    G{{127551, 1500}, 440},
    A{{113636, 1500}, 440},
    A{{113636, 1500}, 440},
   GG{{127551, 1500}, 440},
    F{{142857, 1500}, 880},
    F{{142857, 1500}, 440},
    E{{151515, 1500}, 440},
    E{{151515, 1500}, 440},
    D{{170068, 1500}, 440},
    D{{170068, 1500}, 440},
   CC{{191570, 1500}, 440},
    G{{127551, 1500}, 880},
    G{{127551, 1500}, 440},
    F{{142857, 1500}, 440},
    F{{142857, 1500}, 440},
    E{{151515, 1500}, 440},
    E{{151515, 1500}, 440},
   DD{{170068, 1500}, 440},
    G{{127551, 1500}, 880},
    G{{127551, 1500}, 440},
    F{{142857, 1500}, 440},
    F{{142857, 1500}, 440},
    E{{151515, 1500}, 440},
    E{{151515, 1500}, 440},
   DD{{127551, 1500}, 440},
    SILENT{{100, 1}, 20},
    Alarm {{80000, 6000}, 50}

};*/

/*pwmSeg_t pwmTable [] = {
    {{1, 200}, 100},
    {{2, 200}, 100},
    {{3, 200}, 100},
    {{4, 200}, 100},
    {{5, 200}, 100},
    {{6, 200}, 100},
    {{7, 200}, 100},
    {{8, 200}, 100},
    {{9, 200}, 100},
    {{10, 200}, 200},
    {{9, 200}, 100},
    {{8, 200}, 100},
    {{7, 200}, 100},
    {{6, 200}, 100},
    {{5, 200}, 100},
    {{4, 200}, 100},
    {{3, 200}, 100},
    {{2, 200}, 100},
    {{1, 200}, 100}
};*/

//#define LEGS    (sizeof(pwmTable)/sizeof(pwmTable[0]));
//#define ARMS    ((sizeof(pwnBuzzTable)/sizeof(pwnBuzzTable[0])) - 1); //minus is there so that the last one will hold a space for alarm sound

/*
 * Check the push botton. SW1 is the RUN key, SW2 is the PAUSE key
 */
void
checkPushButton(uint32_t time)
{
    int code = pbRead();
    uint32_t delay;

    switch (code) {
    case 1:             // SW1: Run the system
        if (reverseState == Run)
        {
            reverseState = Pause;
        }
        else if (reverseState == Pause)
        {
            reverseState = Run;
        }
        delay = 250;
        break;

    /*case 2:
        if (displayState == letter)
        {
            displayState = number;
        }
        else if (displayState == number)
        {
            displayState = letter;
        }
        delay = 250;
        break;*/

    default:
        delay = 10;
    }

    schdCallback(checkPushButton, time + delay);
}

/*
 * Update buzzer status
 */
void
pwmbuzzUpdate(uint32_t time)
{
    //led update
    if (reverseState == Run && alarmState == Active) {
       //(redFlash == On) ? ledTurnOnOff(1,0,0) : ledTurnOnOff(0,0,0);
       //redFlash = (redFlash == On) ? Off : On;
       //uprintf("hello\n");
        ledTurnOnOff(1,1,0);

       }
       else if(reverseState == Run && alarmState == Deactive)
       {
           ledTurnOnOff(1,1,1); //green
       }
       else if (reverseState == Run && alarmState == Almost)
       {
           ledTurnOnOff(1,0,1); //yellow
       }
       else {
           // Check again after 10 ms
           ledTurnOnOff(0,0,0); //yellow
       }
    schdCallback(pwmbuzzUpdate, time + 100);
    /*static int segbuzz = 0;                                                 // Current segment of PWM settings
    if (reverseState == Run && alarmState == Active) {
            pwnBuzzerSetKey(pwnBuzzTable[segbuzz].pwm);
            segbuzz = (segbuzz + 1) % ARMS;
            schdCallback(pwmbuzzUpdate, time + pwnBuzzTable[segbuzz].duration);
        }
        else if (reverseState == Run && alarmState == Almost)
        {
            pwm_t buzzer = {100000, 1500};
            pwnBuzzerSetKey(buzzer);
            schdCallback(pwmbuzzUpdate, time + 100);
        }
        else if (reverseState == Run && alarmState == Deactive)
        {
            pwm_t buzzer = {0, 0};
                        pwnBuzzerSetKey(buzzer);
                        schdCallback(pwmbuzzUpdate, time + 100);
            //pwnBuzzerSetKey(pwnBuzzTable[29].pwm);
            //schdCallback(pwmbuzzUpdate, time + pwnBuzzTable[29].duration);
        }
        else {
            // Check again after 10 ms
            pwnBuzzerSetKey(pwnBuzzTable[28].pwm); //silence
            schdCallback(pwmbuzzUpdate, time + 10);
        }*/
   /* if (reverseState == Run && alarmState == Deactive) {
        pwnBuzzerSetKey(pwnBuzzTable[segbuzz].pwm);
        segbuzz = (segbuzz + 1) % ARMS;
        schdCallback(pwmbuzzUpdate, time + pwnBuzzTable[segbuzz].duration);
    }
    else if (reverseState == Run && alarmState == Almost)
    {
        pwm_t buzzer = {100000, 1500};
        pwnBuzzerSetKey(buzzer);
        schdCallback(pwmbuzzUpdate, time + 100);
    }
    else if (reverseState == Run && alarmState == Active)
    {
        pwnBuzzerSetKey(pwnBuzzTable[29].pwm);
        schdCallback(pwmbuzzUpdate, time + pwnBuzzTable[29].duration);
    }
    else {
        // Check again after 10 ms
        pwnBuzzerSetKey(pwnBuzzTable[28].pwm); //silence
        schdCallback(pwmbuzzUpdate, time + 10);
    }*/
}


seg7Display_t seg7Display = {
    0, 0, 0, 0, 0
};

void displayRangerValue(uint32_t time){

    seg7Display.colon = 0;
    uint32_t clockValue = getRangerValue();

    //340/100000. it takes 50Mhz to traveled 340 mm. /2 to get distance
    uint32_t mmValue = clockValue * 340 /100000;  // distance in millimeter
    uint32_t inches = mmValue * 0.0393701; //distance in inches
    if (inches >= 50){
        inches = 50;
    }

    if(inches <= 5)    {
        //alarmState = Deactive;
       alarmState = Deactive;
    }
    else if(inches <=10 && inches > 5)    {
        alarmState = Almost;
    }
    else if(inches > 10){
        //alarmState = Active;
        uprintf("hello\n");
        alarmState = Active;
    }

    if (displayState == number && reverseState == Run){
        seg7Display.d1 = inches % 10;//right most
        if (inches < 10)
        {
            seg7Display.d2 = 10;
            seg7Display.d3 = 10;
            seg7Display.d4 = 10;
        }
        else if (inches < 100)
        {
            seg7Display.d2 = (inches % 100)/10;
            seg7Display.d3 = 10;
            seg7Display.d4 = 10;
        }
        else if (inches < 1000)
        {
            seg7Display.d2 = (inches % 100)/10;
            seg7Display.d3 = (inches % 1000)/100;
            seg7Display.d4 = 10;
        }
        else
        {
            seg7Display.d2 = (inches % 100)/10;
            seg7Display.d3 = (inches % 1000)/100;
            seg7Display.d4 = inches / 1000;//left most
        }
    }
   /* else if (displayState == letter && reverseState == Run)
    {
        if (alarmState == Active)
        {
            seg7Display.d1 = 14;
            seg7Display.d2 = 13;
            seg7Display.d3 = 12;
            seg7Display.d4 = 11;
        }
        else if(alarmState == Almost)
        {
            seg7Display.d1 = 18;
            seg7Display.d2 = 17;
            seg7Display.d3 = 16;
            seg7Display.d4 = 15;
        }
        else if(alarmState == Deactive)
        {
            seg7Display.d1 = 22;
            seg7Display.d2 = 21;
            seg7Display.d3 = 20;
            seg7Display.d4 = 19;
        }
    }*/
    else
    {
        seg7Display.d1 = 10;
        seg7Display.d2 = 10;
        seg7Display.d3 = 10;
        seg7Display.d4 = 10;
    }

    seg7DigitUpdate(&seg7Display);
    //schdCallback(pwmbuzzUpdate, 250);
    schdCallback(displayRangerValue, time + 1000);
}



void main(void)
{
    lpInit();
    initRanger();
    seg7Init();
    pwmBuzzerInit();
    uprintf("%s\n\r", "Final Project: Backup Sensor");

    seg7DigitUpdate(&seg7Display);
    schdCallback(checkPushButton, 1005);
    schdCallback(displayRangerValue, 1010);
    schdCallback(pwmbuzzUpdate, 1011);

    // Loop forever
    while (true) {
        schdExecute();
    }
}
