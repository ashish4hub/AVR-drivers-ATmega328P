# ATmega328P HAL
A modular bare-metal firmware framework for the **ATmega328P**, developed in C using register-level peripheral drivers and a layered hardware abstraction architecture.
The project is built to understand and implement MCU peripherals below the Arduino abstraction layer, while keeping application code separated from low-level hardware access.
---
## Overview

This project provides a collection of reusable drivers and higher-level modules for the ATmega328P.
Instead of directly manipulating MCU registers throughout the application, peripheral access is organized into dedicated drivers.
The general architecture is:

``` Application Layer --> Service Layer --> Peripheral Drivers --> Hardware Registers ```

The project also contains a UART-based command-line interface for interacting with the firmware during development and hardware testing.

---
## Goals
The main goals of this project are:
- Understand the ATmega328P at register level
- Develop reusable peripheral drivers
- Separate hardware access from application logic
- Work with interrupts and timing-sensitive peripherals
- Build higher-level modules on top of low-level drivers
- Develop firmware without depending on Arduino's high-level APIs
- Practice embedded C, cross-compilation, and hardware debugging
---
## Target Hardware
### Microcontroller
- **ATmega328P**
- AVR 8-bit architecture
- 16 MHz system clock
-  Current firmware configuration is based on:
```MCU : ATmega328P | F_CPU : 16000000UL```
---
# Drivers
The project currently contains drivers for several ATmega328P peripherals.
| ADC | Analog-to-Digital Converter | Analog signal acquisition | ICU | Input Capture Unit | Precise measurement of external timing signals | Digital output / LED control || PWM | Timer peripherals | PWM signal generation || SPI | SPI peripheral | Synchronous serial communication || Timer | Timer/Counter | Timing and periodic operations || UART | USART | Serial communication |
---
# ADC Driver
The ADC driver provides an interface to the ATmega328P's **10-bit ADC**.
The driver handles ADC configuration, channel selection, conversion triggering, and result retrieval.
## Public API
The ADC interface provides functionality for:
```ADC_init() | ADC_start() | ADC_done() | ADC_get_result() | ADC_eanble() | ADC_disable()```

### `ADC_init()`
Initialize ADC hardware with desired `Reference Voltage` , `Prescaler` and `Mode [Interrupt/Polling]`

### `ADC_start()`
Starts an `ADC Conversion` on the selected `Channel`. It selects the requested ADC channel and `Triggers` a conversion.

### `ADC_done()`
Checks whether the current ADC conversion has completed by monitoring the ADC start conversion (`ADSC`) bit. `ADSC` remains set while conversion is in progress and clears when conversion completes.

**Returns:**
- 1 - When conversion complete
- 0 - Conversion still in progress

### `ADC_get_result()`
Reads the completed ADC conversion result from the `ADCL` and `ADCH` register.

**Returns:** 10-bit ADC conversion result (0-1023).

### `ADC_enable()`
Enables the ADC peripheral by setting the ADC Enable (`ADEN`) bit in the `ADCSRA` register.

### `ADC_disable()`
Disables the ADC peripheral by clearing the ADC Enable (`ADEN`) bit in the `ADCSRA` register.

### Typical usage
---
```
ADC_init(&config);
ADC_start(ADC_CH0);
while (!ADC_done());
uint16_t value = ADC_get_result();
```

### ADC configuration
---
```
The driver supports configuration of parameters such as:
- Voltage reference
- ADC prescaler
- Conversion mode

The driver is designed so that application code does not need to directly manipulate ADC control registers for normal operation.
```
### ADC Channel Mapping
---
| ADC Channel | ATmega38P Pin | Arduino UNO Pin |
|-------------|---------------|-----------------|
|ADC_CH0      | PC0 / Pin 23  | A0              |
|ADC_CH1      | PC1 / Pin 24  | A1              |
|ADC_CH2      | PC2 / Pin 25  | A2              |
|ADC_CH3      | PC3 / Pin 26  | A3              |
|ADC_CH4      | PC4 / Pin 27  | A4              |
|ADC_CH5      | PC5 / Pin 28  | A5              |

---

# PWM Driver
The PWM driver provides configurable PWM generation using the ATmega328P timer peripherals.
It is intended for applications such as:
- Motor control
- LED brightness control
- Servo-related signals
- General-purpose waveform generation
The driver supports PWM configuration through the hardware timers rather than implementing PWM entirely in software.
---
## Public APIs
`pwm_init()` `pwm_set()` `pwm_disable()` `pwm_start()`
---
### `pwm_init()`
```C
void pwm_init(uint32_t freq, pwm_timer_t timer);
```
Initializes the selected `Timer` for `PWM` at the requested `Frequency`.
**Working:** The `driver`selects suitable `Prescaler` and calculate the timer's `TOP` value so that the requested frequency can be generated. `TIMER1` uses `ICR1` as TOP, while `TIMER2` uses `OCR2A`.

**Parameters:**
| Parameter | Description |
|-----------|-------------|
| `freq`    | Desired PWM frequency in hz |
| `timer`   | `pwm_timer1` or `pwm_timer2` |
**Typical usage:**
``` C
pwm_init(1000, pwm_TIMER1)
```
This configures `Timer1` for a 1kHz `PWM` signal.

### `pwm_set()`
```C
void pwm_set(pwm_channel_t ch, uint32_t duty_percent);
```
Sets duty cycle for the selected `PWM` channel.
**Working:** The requested percentage is converted into the corresponding Output Compare Register (`OCR`) value using the Timers's calculated `TOP` value. *Values over 100% are clamped to 100%*.
**Parameters:**
| Parameter | Description |
|-----------|-------------|
|`ch`       | PWM output channel |
| `duty_percent` | Duty cycle from 0 to 100% |
**Typical usage:**
```C
pwm_set(pwm_CH1A,50);
```
This produces approx a 50% duty-cycle on `PB1 / OC1A`.

### `pwm_disable()`
```C
void pwm_disable(pwm_channel_t ch);
```
Disables PWM output on the selected channel and drives the corresponding pin `low`.

### `pwm_start()`
```C
void pwm_start(pwm_channel_t ch);
```
Starts or re-enables PWM output on the selected channel.

**PWM Channel Mapping:**
| PWM Channel | ATmega328P |
|-------------|------------|
| `pwm_CH1A1` | `PB1`      |
| `pwm_CH1B`  | `PB2`      |
| `pwm_CH2B`  | `PD3`      |
---


# Timer Driver

The `Timer` driver provides a `1 ms` system time base using `Timer0` in `CTC` mode.
`Timer0` is configured with a `64x` prescaler and `OCR0A = 249`, producing a compare match interrupt every 1 ms at a `16 MHz` CPU clock. Each interrupt increments a global `tick` counter.

## APIs
### `timer_init();`
Initialize `Timer0` in `CTC` mode.
```c
timer_init();
```

### `get_ticks();`
Returns current `ticks (Time count)`. `1 tick = 1 MS`.
```C
uint32_t time = get_ticks();
```

### `nb_wait_ms()`;
Checks whether the specified time interval has elapsed without blocking the program execution.
**Parameters:**
| Parameter | Type | Description |
|-----------|------|------------|
| `*prv_time` | uint32_t * | Pointer to the timestamp of the previous execution. Updated when interval expires. |
| `time`    | uint32_t | Required time interval in milliseconds. |
**Returns:**
```
- 1  --> When requested interval has elapsed.
- 0  --> Interval has not elapsed yet.
```
**Typical usage:**
```C
if(nb_wait_ms(&current_time,500)){
    ...
}
```
This creates a `Non-blocking` delay of `500 MS` without blocking the `CPU`.

---
# UART Driver
The UART driver provides serial communication using the ATmega328P USART peripheral.
The driver is designed around interrupt-driven communication and buffering rather than relying only on blocking polling operations.
The UART implementation includes:
- TX buffering
- RX buffering
- Interrupt-based transmission/reception
- Configurable baud rate
- Serial data handling Conceptually:
```Application --> UART API --> TX/RX Buffers --> USART Peripheral --> Serial Interface```
This allows the UART to be used by higher-level components such as the CLI.
## APIs
| APIs | Purpose |
|------|---------|
| `USART_init()` | Initializes `USART` hardware with requested `baud rate`. |
| `USART_print()` | Transmits a null-terminated string. |
| `USART_printIN` | Transmit an integer |
| `USART_rx_avil()` | Checks if `received` data is available |
| `USART_get_data()` | Retrieves one received character |
| `USART_TX_byte` | Adss one byte to the TX buffer for transmission |
### `USART_init()`
Calculates the USART baud-rate register value from `F_CPU` and the requested baud rate, enables the transmitter and receiver, and enables the interrupt. The driver configures the USART for `8-bit` character transmission.
```C
void USART_init(uint32_t BAUD);
```
**Parameter:**
|Parameter | Type | Description |
|----------|------|-------------|
| `BAUD`   | uint32_t | Desired communication baud rate. |
**Typical usage:**
```C
USART_init(9600);
```
### `USART_print()`
Transmits a `null-terminated` string through `USART`.
```C
void USART_print(const char *str);
```
**Parameter:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `str`     |`const char *`| Pointer to the null-terminated string. |
**Typical usage:**
```C
USART_print("Hello world\n");
```
**`USART_print` does not directly manipulate the USART hardware for every character. It uses the lower level `USART_TX_byte()` API.**
### `USART_printIN()`
```C
void USART_printIN(int num);
```
Converts an integer into its decimal character representation and transmits it through the USART.
Handles zero and negative values, converts the individual decimal digits into characters then sends them through `USART_TX_byte`.
**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| num       | int  | Integer value to transmit |
**Typical usage:**
```C
int distance = 50;
USART_ptintIN(distance);
```
### `USART_rx_avil()`
Checks whether unread data is avilable in the `RX buffer`. compares the RX `head` and `tail` indexes of the `circular buffer`.
**Returns:**
- 1  --> Data is available.
- 2  --> RX buffer is empty.
**Typical usage:**
```C
if(USART_rx_avil())
{
    char data = USART_get_data();
}
```
This function allows the application to check for incoming data without blocking while waiting for a character.

### `USAR_get_data()`
```C
char USART_get_data(void);
```
Retrives the next unread character from the RX buffer. reads the character at the current RX tail position and advances the tail index.
**Returns:**
- Received character when data is availble
- 0 when RX buffer is empty

### `USART_TX_byte()`
```C
void USART_TX_byte(uint8_t data);
```
Queues a single byte for transmission through the USART.
The byte is placed into TX circular buffer. The USART Data register empty interrupt is then enabled so that the `ISR` can transmit buffered bytes as the USART becomes ready. If transmission was idle, the function starts transmission immediately.
**Parameter:**
| Parameter | Type | Description |
|-----------|------|-------------|
| data      | uint8_t | Byte to transmit |
**Typical usage:**
```C
USART_TX_byte('A');
```
## Interrupt-Driven Reception
*When a byte arrives the USART RX interrupt is triggered. The `ISR` stores the received byte in the RX buffer and advances the buffer head. If the buffer is full the incoming byte is discarded rather than overwriting the unread data.*

## Interrupt-Driven Transmission
*When the TX buffer becomes empty, the driver disables the Data register Empty interrupt. When data is avilable, the ISR continues loading bytes into `UDR0`.*

## Buffer Configuration
*The driver defines:*
```C
#define Tx_buffer_size 64
#define Rx_buffer_size 64
#define line_size 32
```
## Example:
```C
USART_init(9600);
USART_print("Hello world\n");
USART_printIN(123);

while(1)
{
    if(USART_rx_avail())
    {
        char data = USART_get_data();
    }
     // Process received character
}
```

---
# SPI Driver
*The `SPI` driver provides a hardware abstraction for ATmega328P's `Serial Peripheral Interface in Master mode.*
## SPI Pin Mapping
| SPI signal | ATmega328P | Direction |
|------------|------------|-----------|
| `MOSI`     | PB3        | Output    |
|`MISO`      | PB4        | Input |
| `SS`       | PB2        | Output |
|`SCK`       | PB5        | Output |

*These Pin assignment and the current SPI configuration are implemnted dirctly in `spi.c`.*
## Public APIs
|API | Purpose |
|----|---------|
|`SPI_init()` | Initialize the SPI peripheral as master |
| `SPI_transfer()` | Sends and revceives one byte through SPI |

### `SPI_init()`
```C
void_SPI_init(void);
```
- Configures `MOSI`, `SS`, `SCK` as output.
- Configures  `MISO` as an input.
- Enables the SPI peripheral.
- Selects Master mode.
- Configures the SPI clock as `F_CPU/64`.
- Configures SPI mode 0 (`CPOL = 0`, `CPHA = 0`).
*Thses settings are implemented through `DDRB` and `SPCR` registers.*

### `SPI_transfer()`
*Transfers one byte through SPI peripheral and returns the byte received during the same transfer.*
**Parameters**
| Parameter | Type | Description |
|-----------|------|-------------|
| data      | uint8_t | Byte to transmit |
**Returns**
```C
uint8_t
```
*The byte received from SPI slave.*
## Current Limitations
*The current SPI driver is intentionally minimal.*
---
# ICU Driver
The `ICU` driver uses `ATmega328P` `Timer1` `Input Capture Unit` to record the timer value when and external signal edge occures.
This allows the firmware to measure `time intervals` between signal edges which can be used for pulse-widht, frequency and sensor timing measurement.
**Typical Application**
- Ultrasonic sensor echo timing
- Pulse-width measurement
- Frequency measurement
- Period measurement
## Public APIs
| APIs | Purpose |
|------|---------|
| `ICU_init()` | Initializes Timer1 Input Capture. |
| `ICU_set_edge()` | Change the Capture Edge. |
| `ICU_done()` | Checks whether a capture event occured. |
| `ICU_get_capture()` | Returns the captured Timer1 value. |
| `ICU_clear()` | Clears the capture status |
| `ICU_stop()` | Stops ICU capture and disables its interrupt |

### `ICU_init()`
Initailizes the Timer1 Input Capture peripheral using the supplied configuration.
Configures the capture edge, input noise canceler and Timer1 prescaler. It also clears any previous flag and enables the Timer1 Input Capture interrupt.
**Parameter**
| Parameter | Type | Description |
|-----------|------|-------------|
| `config`     | const ICU_config_t * | Pointer to ICU configuration |
**Typical usage**
```C
ICU_config_t config = {
    .edge = ICU_rising,
    .prescaler = ICU_ps_8,
    .noise = ICU_noise_on
};

ICU_init(&config);
```
### `ICU_set_edge()`
Changes the signal edge that triggers an input capture event.
**Parameter**
| Parameter | Type | Description |
|-----------|------|-------------|
| edge      | ICU_edge_t | `ICU_rising` or `ICU_falling`|
**Typical usage**
```C
ICU_set_edge(ICU_falling);
```
*The driver updates the `ICES1` bit in the Timer1 control register to select the required edge.*

### `ICU_done()`
```C
uint8_t ICU_done(void);
```
Checks whether a new input capture event has occured.
**Returns**
- 1  --> New capture value is available
- 0  --> No new capture value
**Typical usage**
```C
if(ICU_done())
{
    uint16_t capture = ICU_get_capture();
}
```
### `ICU_get_capture()`
```C
uint16_t ICU_get_capture(void);
```
Returns the Timer1 value captured during the most recent input capture event.
**Returns**
`16-bit` Timer1 capture value.

### `ICU_clear()`
```C
void ICU_clear(void);
```
*Clears the current ICU capture status.*

### `ICU_done()`
```C
void ICU_stop(void);
```
*Stops Timer1 counting and disables the Timer1 input capture interrupt.*

## Input Capture Interrupt
```C
ISR(TIMER1_CAPT_vect)
{
    icu_capture = ICR1;
    icu_done = 1;
}
```
## Example Implementation
```C
ICU_config_t config = {
    .edge = ICU_rising,
    .prescaler = ICU_ps_8,
    .noise = ICU_noise_on
};

ICU_init(&config);

while(1)
{
    if(ICU_done())
    {
        uint16_t capture = ICU_get_capture();

        ICU_clear();
    }
}
```
---
# Command-Line Interface
The project includes a UART-based command-line interface.
The CLI provides an interactive way to communicate with the firmware from a serial terminal.
The CLI is intended for:
- Peripheral testing
- Hardware experimentation
- Runtime interaction
- Driver verification
- Debugging
---
# Project Structure
---
# Build System
The project uses **GNU Make** and the AVR-GCC toolchain.
## Requirements
Install the AVR development toolchain on Debian/Ubuntu:
```bash
sudo apt update
sudo apt install gcc-avr avr-libc binutils-avr make avrdude
```
Verify the compiler:
```bash
avr-gcc --version
```
---
# Building the Firmware
Clone the repository:
```bash
git clone https://github.com/ashish4hub/atmega328p-hal.git
```
Enter the project:
```bash
cd atmega328p-hal
```
Build:
```bash
make
```
*The build system targets the ATmega328P and uses a 16 MHz clock configuration.*
---
# Firmware Development Workflow
---
# Testing and Verification
Driver development is performed through hardware experiments and peripheral-specific tests.
Examples include:
### UART
Communication can be verified using a serial terminal.
### PWM
PWM output can be observed using measurement equipment such as a logic analyzer or oscilloscope.
### ADC
ADC readings can be compared against known input voltages.
### Timer / ICU
Timer and input-capture functionality can be verified by measuring signal timing and captured values.
---
# Design Philosophy
The project intentionally avoids placing peripheral register manipulation directly throughout application code.
For example, application code should be able to request an ADC conversion through the ADC driver instead of manually configuring every ADC register each time.
This makes the firmware:
- Easier to reuse
- Easier to understand
- Easier to extend
- Easier to test
- Less dependent on application-specific register manipulation
---
# Engineering Concepts Practiced
This project focuses on practical embedded firmware concepts including:
- Embedded C
- Pointers and memory-mapped registers
- MCU peripheral configuration
- Interrupts
- Timers and counters
- PWM
- ADC
- UART
- SPI
- Input Capture
- Sensor interfacing
- Hardware abstraction
- Driver architecture
- Modular firmware design
- Cross-compilation
- Make-based build systems
- Hardware debugging
---
# Why Build a HAL From Scratch?
High-level frameworks make microcontroller development easier, but they can hide much of the hardware underneath.
This project is intentionally built closer to the hardware to understand the relationship between firmware and MCU peripherals.
The objective is not to recreate a vendor SDK, but to understand how such abstractions are designed and implemented.
---
# Current Status
The project is an actively evolving firmware framework.
Current areas of development include:
- Peripheral driver implementation
- Higher-level hardware modules
- UART-based CLI
- Hardware testing
- Driver refinement
- Improving modularity and reusability
---
# Roadmap
Planned improvements include:
- [x] ADC driver
- [x] PWM driver
- [x] Timer driver
- [x] UART driver
- [x] SPI driver
- [x] ICU driver
- [x] LED driver
- [x] HC-SR04 module
- [x] UART-based CLI
- [ ] Expand driver documentation
- [ ] Increase hardware verification coverage
- [ ] Improve driver APIs
- [ ] Expand CLI functionality
- [ ] Add additional peripheral/device drivers
- [ ] Expand interrupt-driven implementations
- [ ] Add more systematic testing
---
# Future Direction
The long-term goal is to continue evolving the project toward a more complete and reusable embedded firmware framework.
Potential future areas include:
---
# Author
**ashish4hub**
GitHub: [ashish4hub](https://github.com/ashish4hub)
---
# License
This project is licensed under the MIT License.
See the [LICENSE](LICENSE) file for details.