
#ifndef PWMBUZZER_H_
#define PWMBUZZER_H_

#include <stdint.h>

// The PWM parameters
typedef struct {
    uint32_t pulse_width;       // PWM pulse width
    uint32_t period;            // PWM period in cycles
} pwm_t;

void pwmBuzzerInit();
//void pwnBuzzerSetKey(pwm_t key);


#endif
