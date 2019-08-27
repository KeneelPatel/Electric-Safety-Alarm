#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <inc/hw_memmap.h>
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/timer.h>
#include "pwmbuzzer.h"

void
pwmBuzzerInit(){

    SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    GPIOPinTypeTimer(GPIO_PORTC_BASE, GPIO_PIN_7);
    GPIOPinConfigure(GPIO_PC7_WT1CCP1);

    TimerConfigure(WTIMER1_BASE, (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_PWM));

    TimerLoadSet(WTIMER1_BASE, TIMER_B, 1500);
    TimerMatchSet(WTIMER1_BASE, TIMER_B, 1500-1);

    TimerEnable(WTIMER1_BASE, TIMER_B);

}

void
pwnBuzzerSetKey(pwm_t key){

    TimerLoadSet(WTIMER1_BASE, TIMER_B, key.pulse_width);
    TimerMatchSet(WTIMER1_BASE, TIMER_B, key.pulse_width - key.period);
}
