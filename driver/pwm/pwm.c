/* PWM Driver TIMER1 & TIMER2 */
/* MCU = ATmega328P */

    /* PIN MAPPING */
/* pwm_CH1A ----> PB1 */
/* pwm_CH1B ----> PB2 */
/* pwm_CH2A ----> PB3 (USED AS TOP CAN NOT OUTPUT PWM BEHAVIOUR) */ 
/* pwm_CH2B ----> PD3 */

        /* APIs */
/* pwm_init(freq, timer) -----> Initialize PWM with desired fequency and Timer (TIMER1 / TIMER2) */
/* pwm_set(channel, duty_percent) -----> PWM channel (pin) and duty (0 - 100) */

#define F_CPU 16000000UL

#include "pwm.h"

static uint8_t ch1a_enb = 0;          /* TIMER1 channle A enable flag */
static uint8_t ch1b_enb = 0;         /* TIMER1 channle B enable flag */
static uint8_t ch2b_enb = 0;        /* TIMER2 channle B enable flag */

static uint16_t pwm_top;   /*  ICR1 (TIMER1 TOP value) */
static uint8_t pwm_top2;  /* OCRA (TIMER2 TOP vlaue) */


/* TIMER1 Prescaler table */
typedef struct {
    uint16_t prescaler_val;
    uint8_t cs_bits;
} prescaler_t;

const prescaler_t prescalers[] = {
    {1, (1 << CS10)},
    {8, (1 << CS11)},
    {64, (1 << CS11) | (1 << CS10)},
    {256, (1 << CS12)},
    {1024, (1 << CS12) | (1 << CS10)}
};

/* TIMER2 (8 bits) prescaler table */
const prescaler_t prescalers2[] = {
    {1,(1 << CS20)},
    {8, (1 << CS21)},
    {32, (1 << CS21) | (1 << CS20)},
    {64, (1 << CS22)},
    {128, (1 << CS22) | (1 << CS20)},
    {256, (1 << CS22) | (1 << CS21)},
    {1024, (1 << CS22) | (1 << CS21) | (1 << CS20)}
};

/* Computation fucntion for TIMER1 Prescaler */
uint8_t pwm1_compute(uint32_t freq, uint16_t *top, uint8_t *cs_bits){

    /* Selection of minimum prescaler */
    for(uint8_t i = 0; i < 5; i++){
        uint32_t temp = (F_CPU / (prescalers[i].prescaler_val * freq)) - 1;

        if(temp <= 65535){         
            *top = (uint16_t)temp;      // Type cast from 32bit to 16bit to prevent compiler warning
            *cs_bits = prescalers[i].cs_bits;        // Assigning prescaler bits 
            return 1;    // Prescaler found and frequency achieved
        }
    }
    return 0;    // Frequency not achievable
}

/* TIMER2 pwm compute */
uint8_t pwm2_compute(uint32_t freq, uint8_t *top2, uint8_t *cs_bits2){
    for(uint8_t i = 0; i < 7; i++){
        uint32_t temp = (F_CPU / (prescalers2[i].prescaler_val * freq)) - 1;

        if(temp <= 255){
            *top2 = (uint8_t)temp;
            *cs_bits2 = prescalers2[i].cs_bits;
            return 1;  // Success
        }
    }
    return 0; // Failed
}

/* TIMER1 configuration for PWM (fast pwm mode) */
void timer1_pwm_init(uint32_t freq){

    // Clearing channel1 flag on initialization
    ch1a_enb = 0;
    ch1b_enb = 0;

    uint8_t cs_bits;  // Computed TIMER1 Prescaler bits

    if(!pwm1_compute(freq, &pwm_top, &cs_bits)){
        return;
    }
    TCCR1A = (1 << WGM11);
    TCCR1B = (1 << WGM12) | (1 << WGM13) | cs_bits;
    ICR1 = pwm_top;
}

/* TIMER2 configuration for pwm (fast pwm mode) */
void timer2_pwm_init(uint32_t freq){

    // Clearing channel2 flag on initialization
    ch2b_enb = 0;

    uint8_t cs_bits2;         // Computed TIMER2 Prescaler bits
    
    if(!pwm2_compute(freq, &pwm_top2, &cs_bits2)){
        return;
    }
    TCCR2A = (1 << WGM20) | (1 << WGM21);
    TCCR2B = (1 << WGM22) | cs_bits2;
    OCR2A = pwm_top2;
}

/* TIMER1 PWM channel (pin) selection */
void pwm_enb_channel1(pwm_channel_t channel){

    if(channel == pwm_CH1A){
        if(!ch1a_enb){
            DDRB |= (1 << PB1);
            TCCR1A |= (1 << COM1A1);
            ch1a_enb = 1;
        }
    }
    else if (channel == pwm_CH1B)
    {
        if(!ch1b_enb){
            DDRB |= (1 << PB2);
            TCCR1A |= (1 << COM1B1);
            ch1b_enb = 1;
        }
    }
}

/* TIMER2 PWM channel (pin) selection */
void pwm_enb_channel2(pwm_channel_t chnl2){

    if (chnl2 == pwm_CH2B){
        if(!ch2b_enb){
            DDRD |= ( 1<< PD3);
            TCCR2A |= (1 << COM2B1);
            ch2b_enb = 1;
        }
    }
}

/* Seperate API for enabling channels */
void pwm_enb_channel(pwm_channel_t chn){

    if(chn == pwm_CH1A || chn == pwm_CH1B){
        pwm_enb_channel1(chn);
    }
    
    else if (chn == pwm_CH2B){
        pwm_enb_channel2(chn);
    }
}

/* TIMER1 PWM dutycycle assignment */
void timer1_pwm_set(pwm_channel_t ch, uint8_t duty_percent){
    
    pwm_enb_channel1(ch);
    
    if(duty_percent > 100) duty_percent = 100; 
    
    uint16_t duty = ((uint32_t)pwm_top * duty_percent) / 100;    /* Duty percentage to OCR1x value conversion */
    
    if(ch == pwm_CH1A){
        OCR1A = duty;
    }
    else if (ch == pwm_CH1B){
        OCR1B =  duty;
    }
}

/* TIMER2 PWM dutycycle assignment */
void timer2_pwm_set(pwm_channel_t ch, uint8_t duty_percent){

    pwm_enb_channel2(ch);

    if(duty_percent > 100) duty_percent = 100;

    uint8_t duty = ((uint32_t)pwm_top2 * duty_percent) / 100;

    OCR2B = duty;   // Only CH2B Supported in variable frequency mode
}

/* PWM Initialisation and TOP (ICR1) calculation based on passed frequency */
void pwm_init(uint32_t freq, pwm_timer_t timer){
    
    if(freq == 0){               /* Frequency edge case */
        return;
    }
    if(timer == pwm_TIMER1){
        timer1_pwm_init(freq);
    }
    else if(timer == pwm_TIMER2){
        timer2_pwm_init(freq);
    }

}

/* Dutycycle (percent) assignment according to channel */
void pwm_set(pwm_channel_t ch, uint32_t duty_percent){
    
    if(ch == pwm_CH1A || ch == pwm_CH1B){
        timer1_pwm_set(ch, duty_percent);
    }
    else if (ch == pwm_CH2B){
        timer2_pwm_set(ch, duty_percent);
    }
}

/* PWM stop function */
void pwm_disable(pwm_channel_t ch){
    
    if(ch == pwm_CH1A){
        TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0));
        PORTB &= ~(1 << PB1);
    }
    else if(ch == pwm_CH1B){
        TCCR1A &= ~((1 << COM1B1) | (1 << COM1B0));
        PORTB &= ~(1 << PB2);
    }
    else if(ch == pwm_CH2B){
        TCCR2A &= ~((1 << COM2B1) | (1 << COM2B0));
        PORTD &= ~(1 << PD3);
    }
}

/* PWM start function */
void pwm_start(pwm_channel_t ch){

    if(ch == pwm_CH1A){
        DDRB |= (1 << PB1);

        TCCR1A &= ~(1 << COM1A0);      // Non inverting
        TCCR1A |= (1 << COM1A1);      // re enable pwm output
    }
    else if(ch == pwm_CH1B){
        DDRB|= (1 << PB2);

        TCCR1A &= ~(1 << COM1B0);
        TCCR1A |= (1 << COM1B1);

    }
    else if(ch == pwm_CH2B){
        DDRD |= (1 << PD3);

        TCCR2A &= ~(1 << COM2B0);
        TCCR2A |= (1 << COM2B1);
    }
}