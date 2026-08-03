/* Command Line Interface */

#include "CLI.h"

#define CLI_buffer_size 32

static char CLI_buffer[CLI_buffer_size];
static uint8_t CLI_index = 0;

/* Build string from received character and pass it to execute function */
void CLI_process(void){
    while(USART_rx_avail()){
        char c = USART_get_data();

        if(c ==  '\n' || c == '\r'){
            CLI_buffer[CLI_index] = '\0';         // Terminate string
            CLI_execute(CLI_buffer);             // Passing string to execute function
            CLI_index = 0;   // Buffer reset
        }
        else{
            if(CLI_index < CLI_buffer_size - 1){
                CLI_buffer[CLI_index++] = c;
            }
        }
    }
}

/* Execute function for triggering actions based on user commands */
void CLI_execute(char *cmd){

    /* LED on/off */
    if(strcmp(cmd, "led on") == 0){
        led_set_state(state_led_on);
        MSG_set_state(LED_ON);
    }
    else if(strcmp(cmd, "led off") == 0){
        led_set_state(state_led_off);
        MSG_set_state(LED_OFF);
    }

    /* LED ramp and fade */
    else if(strcmp(cmd, "start led ramp") == 0){
        pwm_set_state(state_led_on);
        MSG_set_state(RAMP_START);
    }
    else if(strcmp(cmd, "stop led ramp") == 0){
        pwm_set_state(state_led_off);
        MSG_set_state(RAMP_OFF);
    }
    else if(strcmp(cmd, "start led fade") == 0){
        pwm_set_state(state_led_fade);
        MSG_set_state(FADE_START);
    }

    /* GAS detection */
    else if(strcmp(cmd, "adc detect") == 0){
        adc_set_state(GAS_start_detect);
        MSG_set_state(START_ADC);
    }
    else if(strcmp(cmd, "stop adc detect") == 0){
        adc_set_state(GAS_stop_detect);
        MSG_set_state(STOP_ADC);
    }

    /* Distance measurement */
    else if(strcmp(cmd,"measure distance") == 0){
        HCSR04_set_state(START);
        MSG_set_state(START_DISTANCE);
    }
    else {
        USART_print("Invalid command\n");
    }
}
