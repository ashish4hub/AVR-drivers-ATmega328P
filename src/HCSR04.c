/* HCSR04 */

#include "HCSR04.h"
#include <stdint.h>
#include <avr/io.h>

static volatile uint16_t start_time;             // store pulse start time
static volatile uint16_t end_time;              // store pulse end time
static volatile uint16_t duration;             // Pulse duration
static volatile uint16_t distance;            // store calulated distance
static volatile uint8_t measurement_done;    // measuremnt done flag
static uint32_t trig_wait = 0;

/* State */
typedef enum{
    HCSR04_wait_rising,
    HCSR04_wait_falling
} HCSR04_state_t;

HCSR04_state_t state;
measurement_state_t m_state = IDLE;

/* HCSR04 initialization */
void HCSR04_init(void){

    /* ICU configuration */
    ICU_config_t icu_config = {
        .edge = ICU_rising,
        .noise = ICU_noise_on,
        .prescaler = ICU_ps_8,
    };

    ICU_init(&icu_config);

    DDRD |= (1 << PD7);         // TRIG pin set as output
    state = HCSR04_wait_rising;
    ICU_set_edge(ICU_rising);
}

/* Trigger pulse */
void HCSR04_trigger(void){

    PORTD |= (1 << PD7);
    _delay_us(10);
    PORTD &= ~(1 << PD7);
}

/* Distance update */
void HCSR04_update(void){
    if(ICU_done()){

        if(state == HCSR04_wait_rising){
            start_time = ICU_get_capture();
            ICU_clear();
            ICU_set_edge(ICU_falling);
            state = HCSR04_wait_falling;
        }
        else {
            end_time = ICU_get_capture();
            duration = end_time- start_time;
            distance = duration / 116;
            measurement_done = 1;
            ICU_set_edge(ICU_rising);
            state = HCSR04_wait_rising;
        }
        ICU_clear();
    }
}

/* Done API */
uint8_t HCSR04_done(void){
    return measurement_done;
}

/* Distance API */
uint16_t HCSR04_get_distance(void){
    measurement_done = 0;
    return distance;
}

/* Set state */
void HCSR04_set_state(measurement_state_t cur_state){

    m_state = cur_state;                 // set current state
}

/* State based operation */
void HCSR04_src(void){

    switch (m_state)
    {
    case IDLE:
    break;

    case START:
        if(nb_wait_ms(&trig_wait,60)){
            HCSR04_trigger();
        }
        HCSR04_update();
        break;

    case STOP:
        ICU_stop();

    default:
        break;
    }
}