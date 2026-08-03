#ifndef HCSR04_H
#define HCSR04_H

#include "../driver/icu/icu.h"
#include "../driver/timer/timer.h"
#include "../driver/uart/uart.h"
#include <util/delay.h>

typedef enum{
    IDLE,
    START,
    STOP
}measurement_state_t;

void HCSR04_init(void);                                      /* HCSR04 hardware initialization function */
void HCSR04_trigger(void);                                  /* PULSE TRIGGER */
void HCSR04_update(void);                                  /* UPDATE RESULT */
uint8_t HCSR04_done(void);                                /* Measurement done flag */
uint16_t HCSR04_get_distance(void);                      /* Convert pulse duration into distance (in cm) and returns distance */
void HCSR04_set_state(measurement_state_t m_state);     /* Set state */
void HCSR04_src(void);                                 /* operation */

#endif