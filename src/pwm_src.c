/* PWM servicce */

#include "pwm_src.h"

pwm_state_t pwm_state = state_ramp_off;

/* Start led ram up */
void run_led_rampup(void){

    static uint32_t duty_wait = 0;
    static uint8_t duty = 0;

    if(duty_wait == 0){
        duty_wait = get_ticks();
    }
    
    if(nb_wait_ms(&duty_wait,50)){

        pwm_set(pwm_CH2B,duty);
        duty++;

        if(duty > 100){
            duty = 0;
        }
    }
}

/* LED fade */
void led_fade(void){

    static uint32_t led_fade_wait = 0;
    static uint8_t duty = 0;
    static int8_t step = 1;

    if(led_fade_wait == 0){
        led_fade_wait = get_ticks();
    }

    if(nb_wait_ms(&led_fade_wait,10)){

        duty += step;

        if(duty >= 100){
            duty = 100;
            step = -1;
        }
        else if(duty == 0){
            step = 1;
        }
        pwm_set(pwm_CH2B,duty);
    }
}

/* State based control */
void pwm_set_state(pwm_state_t state){

    switch (state)
    {
    case state_ramp_on:
        pwm_state = state_ramp_on;
        break;
    case state_ramp_off:
    pwm_state = state_ramp_off;
        break;
    case state_led_fade:
        pwm_state = state_led_fade;
        break;
    }
}

/* PWM SRC*/
void pwm_src(void){
    
    switch (pwm_state)
    {
    case state_ramp_on:
        pwm_start(pwm_CH2B);
        run_led_rampup();
        break;
    
    case state_ramp_off:
        pwm_disable(pwm_CH2B);
        break;
    
    case state_led_fade:
        pwm_start(pwm_CH2B);
        led_fade();
    }
}

