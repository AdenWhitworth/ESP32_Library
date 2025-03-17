/**
 * @file cppgpio.cpp
 * @brief Implementation of the CPPGPIO library for ESP32 GPIO control.
 * 
 * This file provides the implementation for GPIO input and output control classes,
 * designed to simplify the interface to the ESP32's GPIO functionality.
 */

#include "cppgpio.h"

namespace CPPGPIO {
    /*================================= GpioInput ==============================*/
    
    /**
     * @brief Initializes the GPIO input pin with specified configuration.
     * 
     * This private method configures the GPIO pin for input mode with the specified active low setting.
     * 
     * @param pin The GPIO pin number to initialize.
     * @param activeLow If true, the input is considered active when the pin is low.
     * @return esp_err_t Status of the initialization (ESP_OK on success).
     */
    esp_err_t GpioInput::_init(const gpio_num_t pin, const bool activeLow) {
        esp_err_t status{ESP_OK};
        _active_low = activeLow;
        _pin = pin;
        
        gpio_config_t cfg;
        cfg.pin_bit_mask = 1ULL <<pin;
        cfg.mode = GPIO_MODE_INPUT;
        cfg.pull_up_en = GPIO_PULLUP_DISABLE;
        cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
        cfg.intr_type = GPIO_INTR_POSEDGE;

        status |= gpio_config(&cfg);
        return status;
    }

    /**
     * @brief Constructs a GpioInput object with specified pin and active low setting.
     * 
     * @param pin The GPIO pin number to initialize.
     * @param activeLow If true, the input is considered active when the pin is low.
     */
    GpioInput::GpioInput(const gpio_num_t pin, const bool activeLow){
        _init(pin, activeLow);
    }

    /**
     * @brief Constructs a GpioInput object with specified pin and default active low setting (false).
     * 
     * @param pin The GPIO pin number to initialize.
     */
    GpioInput::GpioInput(const gpio_num_t pin){
        _init(pin, false);
    }

    /**
     * @brief Default constructor for GpioInput.
     * 
     * Creates an uninitialized GpioInput object. The init() method must be called before use.
     */
    GpioInput::GpioInput(void){
    }

    /**
     * @brief Initializes the GPIO input pin with specified configuration.
     * 
     * @param pin The GPIO pin number to initialize.
     * @param activeLow If true, the input is considered active when the pin is low.
     * @return esp_err_t Status of the initialization (ESP_OK on success).
     */
    esp_err_t GpioInput::init(const gpio_num_t pin, const bool activeLow) {
        return _init(pin, activeLow);
    }

    /**
     * @brief Initializes the GPIO input pin with specified pin and default active low setting (false).
     * 
     * @param pin The GPIO pin number to initialize.
     * @return esp_err_t Status of the initialization (ESP_OK on success).
     */
    esp_err_t GpioInput::init(const gpio_num_t pin) {
        return _init(pin, false);
    }

    /**
     * @brief Reads the current state of the GPIO input pin.
     * 
     * If the pin is configured as active low, the logic level is inverted.
     * 
     * @return esp_err_t The logical state of the input (0 or 1).
     */
    esp_err_t GpioInput::read() {
        return _active_low ? !gpio_get_level(_pin) : gpio_get_level(_pin);
    }

    /*================================= GpioOutput ==============================*/

    /**
     * @brief Initializes the GPIO output pin with specified configuration.
     * 
     * This private method configures the GPIO pin for output mode with the specified active low setting.
     * 
     * @param pin The GPIO pin number to initialize.
     * @param activeLow If true, the output is considered active when the pin is low.
     * @return esp_err_t Status of the initialization (ESP_OK on success).
     */
    esp_err_t GpioOutput::_init(const gpio_num_t pin, const bool activeLow){
        esp_err_t status{ESP_OK};
        _active_low = activeLow;
        _pin = pin;

        gpio_config_t cfg;
        cfg.pin_bit_mask = 1ULL << pin;
        cfg.mode = GPIO_MODE_OUTPUT;
        cfg.pull_up_en = GPIO_PULLUP_DISABLE;
        cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
        cfg.intr_type = GPIO_INTR_DISABLE;

        status |= gpio_config(&cfg);
        return status;
    }

    /**
     * @brief Constructs a GpioOutput object with specified pin and active low setting.
     * 
     * @param pin The GPIO pin number to initialize.
     * @param activeLow If true, the output is considered active when the pin is low.
     */
    GpioOutput::GpioOutput(const gpio_num_t pin, const bool activeLow){
       _init(pin, activeLow);
    }

    /**
     * @brief Constructs a GpioOutput object with specified pin and default active low setting (false).
     * 
     * @param pin The GPIO pin number to initialize.
     */
    GpioOutput::GpioOutput(const gpio_num_t pin){
        _init(pin, false);
    }

    /**
     * @brief Default constructor for GpioOutput.
     * 
     * Creates an uninitialized GpioOutput object. The init() method must be called before use.
     */
    GpioOutput::GpioOutput(void){        
    }

    /**
     * @brief Initializes the GPIO output pin with specified configuration.
     * 
     * @param pin The GPIO pin number to initialize.
     * @param activeLow If true, the output is considered active when the pin is low.
     * @return esp_err_t Status of the initialization (ESP_OK on success).
     */
    esp_err_t GpioOutput::init(const gpio_num_t pin, const bool activeLow){
        return _init(pin, activeLow);
    }

    /**
     * @brief Initializes the GPIO output pin with specified pin and default active low setting (false).
     * 
     * @param pin The GPIO pin number to initialize.
     * @return esp_err_t Status of the initialization (ESP_OK on success).
     */
    esp_err_t GpioOutput::init(const gpio_num_t pin){
        return _init(pin, false);
    }

    /**
     * @brief Turns the GPIO output on.
     * 
     * Sets the output to its active state. If active_low is true, this will set the pin to a low level.
     * Otherwise, it will set the pin to a high level.
     * 
     * @return esp_err_t Status of the operation (ESP_OK on success).
     */
    esp_err_t GpioOutput::on(void) {
        return setLevel(GpioLevel::HIGH);
    }

    /**
     * @brief Turns the GPIO output off.
     * 
     * Sets the output to its inactive state. If active_low is true, this will set the pin to a high level.
     * Otherwise, it will set the pin to a low level.
     * 
     * @return esp_err_t Status of the operation (ESP_OK on success).
     */
    esp_err_t GpioOutput::off(void) {
        return setLevel(GpioLevel::LOW);
    }

    /**
     * @brief Toggles the GPIO output state.
     * 
     * Switches the output between its active and inactive states.
     * 
     * @return esp_err_t Status of the operation (ESP_OK on success).
     */
    esp_err_t GpioOutput::toggle(void) {
        _level = (_level == GpioLevel::HIGH) ? GpioLevel::LOW : GpioLevel::HIGH;
        return gpio_set_level(_pin, _active_low ? static_cast<int>(_level == GpioLevel::LOW) : static_cast<int>(_level));
    }

    /**
     * @brief Sets the GPIO output to a specific level.
     * 
     * Sets the output to the specified level. The active_low setting is taken into account
     * when determining the actual pin level to set.
     * 
     * @param level The logical level to set (GpioLevel::LOW or GpioLevel::HIGH).
     * @return esp_err_t Status of the operation (ESP_OK on success).
     */
    esp_err_t GpioOutput::setLevel(GpioLevel level) {
        _level = level;
        return gpio_set_level(_pin, _active_low ? static_cast<int>(level == GpioLevel::LOW) : static_cast<int>(level));
    }
}