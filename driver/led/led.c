/* LED driver */

#include "led.h"

/* LED on function */
void led_on(void){
    PORTB |= (1 << PB5);
}

/* LED off function */
void led_off(void){
    PORTB &= ~(1 << PB5);
}