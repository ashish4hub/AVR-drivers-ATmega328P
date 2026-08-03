MCU    = atmega328p
F_CPU  = 16000000UL
CC     = avr-gcc
OBJCOPY = avr-objcopy
CFLAGS = -mmcu=$(MCU) -Os -Wall -DF_CPU=$(F_CPU)

TARGET = main
SRC    = main.c driver/uart/uart.c driver/timer/timer.c driver/pwm/pwm.c driver/adc/adc.c driver/spi/spi.c driver/led/led.c driver/icu/icu.c CLI/CLI.c src/led_src.c src/pwm_src.c src/adc_src.c src/HCSR04.c src/msg.c
# This converts the .c list into a .o list
OBJ    = $(SRC:.c=.o)

all: $(TARGET).hex

# Link objects to create the ELF
$(TARGET).elf: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Convert ELF to HEX for flashing
$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

# Compile C files into Object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET).elf $(TARGET).hex

.PHONY: all clean