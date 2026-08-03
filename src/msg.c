#include "msg.h"


Msg_state_t current_state = Idle;

uint8_t dist_once = 1;              // Flag for printing distance initial message once
uint8_t dist_value = 1;            // Flag for printing distance value continiously
uint8_t adc_once = 1;             // flag for printing ADC initial message once
uint8_t adc_value = 1;           // Flag for printing ADC value continiously
uint8_t idle_once = 1;          // Flag for printing IDLE state message once
static uint8_t led_on = 1;
static uint8_t led_off = 1;
static uint8_t ramp_on = 1;
static uint8_t ramp_off = 1;
uint8_t fade_on = 1;
uint8_t fade_off = 1;

uint32_t dist_wait = 0;            // Flag for printing distance values in fixed time intervals
uint32_t adc_wait = 0;            // Flag for prinitng moisture values in fixed time intervals

uint32_t dist = 0;

/* Set state function */
void MSG_set_state(Msg_state_t state){
    current_state = state;
}

/* State based operation */
void MSG_src(void){
    switch (current_state)
    {
    
    case Idle:
    if(idle_once == 1){
        USART_print("System in IDLE state!\n");
        idle_once = 0;
    }
    break;
    
    case START_DISTANCE:
    if(dist_once == 1){
        USART_print("Distance mesurement started\n");
        dist_once = 0;
    }
    if(dist_value == 1){
        if(nb_wait_ms(&dist_wait,500)){
            if(HCSR04_done()){
                dist = HCSR04_get_distance();
                USART_print("Obstacle at: ");
                USART_printIN(dist);
                USART_print(" CM\n"); 
            }
        }
    }
    break;

    case STOP_DISTANCE:
    USART_print("Distance measurement stopped\n");
    break;

    case START_ADC:
    if(adc_once == 1){
        USART_print("Moisture detection started\n");
        adc_once = 0;
    }
    if(adc_value == 1){
        if(nb_wait_ms(&adc_wait,500)){
            USART_print("Moisture value: ");
            USART_printIN(ADC_get_result());
            USART_print("\n");
        }
    }
    break;

    case STOP_ADC:
    USART_print("Moisture detection stopped\n");
    break;

    case LED_ON:
    if(led_on == 1){
        USART_print("LED ON\n");
        led_on = 0;
    }
    break;

    case LED_OFF:
    if(led_off == 1){
        USART_print("LED OFF\n");
        led_off = 0;
    }
    break;

    case RAMP_START:
    if(ramp_on == 1){
        USART_print("LED ramp started\n");
        ramp_on = 0;
    }
    break;

    case RAMP_OFF:
    if(ramp_off == 1){
        USART_print("LED ramp stopped\n");
        ramp_off = 0;
    }
    break;

    case FADE_START:
    if(fade_on == 1){
        USART_print("LED fade started\n");
        fade_on = 0;
    }
    break;

    case FADE_OFF:
    if(fade_off == 1){
        USART_print("LED fade stopped\n");
        fade_off = 0;
    }
    break;

    default:
        break;
    }
}