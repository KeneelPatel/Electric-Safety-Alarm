 .cdecls "stdint.h", "stdbool.h", "stdio.h", "inc/hw_memmap.h", "driverlib/pin_map.h", "driverlib/gpio.h", "driverlib/sysctl.h", "driverlib/timer.h", "launchpad.h", "ranger.h"
					.text
TIMER_PERIPH		.field	SYSCTL_PERIPH_WTIMER0
GPIOC_PERIPH		.field	SYSCTL_PERIPH_GPIOC
TIMER_BASE			.field 	WTIMER0_BASE
GPIOC_BASE			.field 	GPIO_PORTC_BASE
GPIO_TO_WTIMER		.field 	GPIO_PC4_WT0CCP0
GPIOC_PIN     		.equ	GPIO_PIN_4

;to initialize the ranger sensor
initRanger		PUSH 	{LR}
				;   Call SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER0)
                LDR   r0, TIMER_PERIPH
                BL    SysCtlPeripheralEnable

				;   Call  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC)
                LDR   r0, GPIOC_PERIPH
                BL    SysCtlPeripheralEnable


                ;   Call TimerConfigure(WTIMER0_BASE, (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_TIME_UP))
                LDR 	r0, TIMER_BASE
               	MOV 	r1, #TIMER_CFG_SPLIT_PAIR
				ORR 	r1, #TIMER_CFG_A_CAP_TIME_UP
                BL    TimerConfigure

				;TimerControlEvent(WTIMER0_BASE, TIMER_A, TIMER_EVENT_BOTH_EDGES)
				LDR		r0, TIMER_BASE
				MOV		r1, #TIMER_A
				MOV 	r2, #TIMER_EVENT_BOTH_EDGES
				BL    	TimerControlEvent

				;TimerEnable(WTIMER0_BASE, TIMER_A);
				LDR		r0, TIMER_BASE
				MOV		r1, #TIMER_A
				BL		TimerEnable

                POP   {PC}

getRangerValue	PUSH 	{LR}

				;   GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4)
                LDR   	r0, GPIOC_BASE
				MOV		r1, #GPIOC_PIN
                BL    	GPIOPinTypeGPIOOutput

				;    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0) // set to low
                LDR   	r0, GPIOC_BASE
				MOV		r1, #GPIOC_PIN
				MOV		r2, #0
                BL    	GPIOPinWrite

				MOV 	r0, #2
				BL		waitUs

				;    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0xFF); //set to high?
                LDR   	r0, GPIOC_BASE
				MOV		r1, #GPIOC_PIN
				MOV		r2, #0xFF
                BL    	GPIOPinWrite

				MOV 	r0, #5
				BL		waitUs

				;    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0)
                LDR   	r0, GPIOC_BASE
				MOV		r1, #GPIOC_PIN
				MOV		r2, #0
                BL    	GPIOPinWrite

				;    GPIOPinTypeTimer(GPIO_PORTC_BASE, GPIO_PIN_4);
                LDR   	r0, GPIOC_BASE
				MOV		r1, #GPIOC_PIN
                BL    	GPIOPinTypeTimer

				;    GPIOPinConfigure(GPIO_PC4_WT0CCP0); // route pin 4 to wide timer 0, A
                LDR   	r0, GPIO_TO_WTIMER
                BL    	GPIOPinConfigure

				;  TimerIntClear(WTIMER0_BASE, TIMER_CAPA_EVENT)
                LDR   	r0, TIMER_BASE
				MOV		r1, #TIMER_CAPA_EVENT
                BL    	TimerIntClear

				;while(!(TimerIntStatus(WTIMER0_BASE, 0)))
loop			LDR		r0, TIMER_BASE
				MOV		r1, #0
				BL		TimerIntStatus
				CMP 	r0, #0
				BEQ		loop

				;risingtime = TimerValueGet(WTIMER0_BASE, TIMER_A)
				LDR		r0, TIMER_BASE
				MOV 	r1, #TIMER_A
				BL		TimerValueGet
				PUSH	{r0}			;push risingtime

				;   TimerIntClear(WTIMER0_BASE, TIMER_CAPA_EVENT);
                LDR   	r0, TIMER_BASE
				MOV		r1, #TIMER_CAPA_EVENT
                BL    	TimerIntClear


				;while(!(TimerIntStatus(WTIMER0_BASE, 0)))
loop2			LDR		r0, TIMER_BASE
				MOV		r1, #0
				BL		TimerIntStatus
				CMP 	r0, #0
				BEQ		loop2

				;fallingTime = TimerValueGet(WTIMER0_BASE, TIMER_A)
				LDR		r0, TIMER_BASE
				MOV 	r1, #TIMER_A
				BL		TimerValueGet
				PUSH	{r0}			;push fallingTime

				;   TimerIntClear(WTIMER0_BASE, TIMER_CAPA_EVENT);
                LDR   	r0, TIMER_BASE
				MOV		r1, #TIMER_CAPA_EVENT
                BL    	TimerIntClear

				POP		{r1, r0}

				SUB 	r0, r0, r1

				POP		{pc}


