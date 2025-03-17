# CPPGPIO - C++ Wrapper for ESP32 GPIO

A C++ wrapper library for ESP32 GPIO functionality that provides an intuitive, object-oriented interface for GPIO operations.

## Features

- Object-oriented GPIO control
- Support for both input and output pins
- Active-low configuration option
- Simple API for pin state management

## Installation

### PlatformIO

1. Add this library to your `platformio.ini`:

```ini
lib_deps = 
    https://github.com/yourusername/CPPGPIO.git
```

2. Or install through the PlatformIO Library Manager.

## Usage

### Include the library

```cpp
#include "cppgpio.h"
```

### GPIO Output Example

```cpp
#include "cppgpio.h"

// Create an output pin (GPIO 22)
CPPGPIO::GpioOutput led(GPIO_NUM_22);

void setup() {
    // Turn on the LED
    led.on();
    
    // Wait a bit
    vTaskDelay(pdMS_TO_TICKS(1000));
    
    // Turn off the LED
    led.off();
    
    // Toggle the LED
    led.toggle();
    
    // Set to specific level
    led.setLevel(CPPGPIO::GpioLevel::HIGH);
}
```

### GPIO Input Example

```cpp
#include "cppgpio.h"

// Create an input pin (GPIO 21)
CPPGPIO::GpioInput button(GPIO_NUM_21);

void setup() {
    // Read the button state
    int state = button.read();
    
    // If the button is pressed
    if (state) {
        // Do something
    }
}
```

### Active Low Configuration

```cpp
// Create an active-low output pin (common for LEDs)
CPPGPIO::GpioOutput led(GPIO_NUM_22, true);

// Create an active-low input pin (common for buttons with pull-ups)
CPPGPIO::GpioInput button(GPIO_NUM_21, true);
```

## License

This library is licensed under the MIT License - see the LICENSE file for details. 