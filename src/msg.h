#ifndef MSG_SRC
#define MSG_SRC

#include "../driver/uart/uart.h"
#include "../driver/timer/timer.h"
#include "../src/adc_src.h"
#include "../src/HCSR04.h"
#include <stdint.h>

/* State */
typedef enum{
    Idle,
    START_DISTANCE,
    STOP_DISTANCE,
    START_ADC,
    STOP_ADC,
    LED_ON,
    LED_OFF,
    RAMP_START,
    RAMP_OFF,
    FADE_START,
    FADE_OFF
}Msg_state_t;

void MSG_set_state(Msg_state_t state);
void MSG_src(void);

#endif