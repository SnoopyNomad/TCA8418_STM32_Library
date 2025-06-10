# TCA8418 STM32 Library

A lightweight and easy-to-integrate C library for interfacing the Texas Instruments TCA8418 I²C keypad controller with STM32 microcontrollers using the STM32Cube HAL. This library provides a simple interface for reading keypad inputs and generating interrupts.

## Table of Contents
- [Features](#features)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Configuration](#configuration)
- [Usage](#usage)
  - [Initialization](#initialization)
  - [Reading Key Events](#reading-key-events)
  - [Keypad Locking](#keypad-locking)
  - [Interrupt Handling](#interrupt-handling)
- [Error Handling](#error-handling)
- [Contributing](#contributing)
- [License](#license)
- [Acknowledgments](#acknowledgments)

## Features

- Initialize and configure TCA8418 via I²C
- Read key press and release events
- FIFO-based event handling (up to 10 events)
- Interrupt-driven operation
- Compatible with STM32 HAL drivers

## Prerequisites

- STM32 microcontroller
- STM32Cube HAL drivers installed
- I²C peripheral configured in your project
- Power supply and wiring to TCA8418 per datasheet
- Basic understanding of STM32 development

## Installation

1. Clone this repository into your project directory:  
```bash
git clone https://github.com/SnoopyNomad/TCA8418_STM32_Library.git
```

2. Copy the following files into your project:
   - `tca8418.c` → Your project's source folder
   - `tca8418.h` → Your project's include folder

## Configuration

The library is configured with the following default settings:
- Keypad Configuration: ROW0 and COL6:0 (7 keys)
- Interrupts: Enabled for key events
- GPIO Configuration: Unused pins configured as inputs with pull-up

Before using the library, configure any pin assignments and settings in your project headers:
```c
#define TCA8418_INT_Pin        GPIO_PIN_0
#define TCA8418_INT_GPIO_Port  GPIOA
```

## Usage

### Initialization

1. Configure I²C in your STM32 project:
```c
// In your main.c or system initialization
I2C_HandleTypeDef hi2c1;

void SystemClock_Config(void);
static void MX_I2C1_Init(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_I2C1_Init();
    // ... rest of your initialization code
}
```

2. **Include the Library**: Add the library header to your project.
```c
#include "tca8418.h"
```

3. Initialize the TCA8418:
```c
HAL_StatusTypeDef status;

// Basic initialization
status = TCA8418_Init();
if(status != HAL_OK){
    Error_Handler();
}
```

### Reading Key Events

```c
uint8_t keyEvents[10]; // Array to store up to 10 events
uint8_t numEvents; // Number of events actually read

// Read all pending key events
HAL_StatusTypeDef status = TCA8418_ReadKeyEvents(keyEvents, &numEvents);

if(status == HAL_OK){
    // Process each event
    for(int i = 0; i < numEvents; i++){
        uint8_t keyNumber = keyEvents[i] & 0x7F; // Bits 6:0
        uint8_t isPress = (keyEvents[i] & 0x80) ? 1 : 0; // Bit 7
        
        if(isPress){
            // Handle key press
        } else {
            // Handle key release
        }
    }
}
```

### Keypad Locking

```c
// Lock keypad (only POWER key remains active)
status = TCA8418_LockKeypad();
if(status != HAL_OK){
    Error_Handler();
}

// Unlock keypad (all keys active)
status = TCA8418_UnlockKeypad();
if(status != HAL_OK){
    Error_Handler();
}
```

### Interrupt Handling

1. Configure interrupts:
```c
// Interrupts are enabled by default in TCA8418_Init()
```

2. Set up interrupt handler:
```c
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
    if(GPIO_Pin == TCA8418_INT_Pin){
        uint8_t keyEvents[10];
        uint8_t numEvents;
        
        // Read all pending key events
        if(TCA8418_ReadKeyEvents(keyEvents, &numEvents) == HAL_OK)        {
            // Process key events here
            // ...
        }
    }
}
```

## Error Handling

All functions return `HAL_StatusTypeDef`:
- `HAL_OK`: Operation successful
- `HAL_ERROR`: Operation failed
- `HAL_BUSY`: Device is busy
- `HAL_TIMEOUT`: Operation timed out

Common issues:
- I²C communication failure
- Invalid device address
- Sensor unresponsive
- Misconfiguration

## Contributing

Contributions are welcome! Please follow these steps:
1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## License

MIT – see [LICENSE](LICENSE).

## Acknowledgments

- Texas Instruments (TCA8418)
- STMicroelectronics (HAL drivers)
- Community contributors
