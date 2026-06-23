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
        USART_print("LED ON\n");
    }
    else if(strcmp(cmd, "led off") == 0){
        led_set_state(state_led_off);
        USART_print("LED OFF\n");
    }

    /* LED ramp and fade */
    else if(strcmp(cmd, "start led ramp") == 0){
        pwm_set_state(state_led_on);
        USART_print("LED RAMP STARTED\n");
    }
    else if(strcmp(cmd, "stop led ramp") == 0){
        pwm_set_state(state_led_off);
        USART_print("LED RAMP STOPPED\n");
    }
    else if(strcmp(cmd, "start led fade") == 0){
        pwm_set_state(state_led_fade);
        USART_print("LED FADE STARTED\n");
    }

    /* GAS detection */
    else if(strcmp(cmd, "gas detect") == 0){
        adc_set_state(GAS_start_detect);
        
        USART_print("DETECTION STARTED!\n");
        USART_print("ENTER COMMAND AGAIN TO PRINT\n");
        
        // Print gas status (HIGH or LOW)
        if(gas_status() == GAS_high){
            USART_print("GAS: ");
            USART_printIN(gas_result());
            USART_print(" HIGH\n");
        }
        else if(gas_status() == GAS_low){
            USART_print("GAS: ");
            USART_printIN(gas_result());
            USART_print(" LOW\n");
        }
    
    }
    else if(strcmp(cmd, "stop gas detect") == 0){
        adc_set_state(GAS_stop_detect);
        USART_print("DETECTION STOPPED\n");
    }
    else {
        USART_print("Invalid command\n");
    }
}
