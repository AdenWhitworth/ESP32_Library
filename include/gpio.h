#ifndef GPIO_H
#define GPIO_H

#include "driver/gpio.h"
#include "esp_event.h"

namespace GPIO {

    ESP_EVENT_DECLARE_BASE(INPUT_EVENTS);

    /**
     * @brief Enumeration for GPIO output levels.
     */
    enum class GpioLevel {
        LOW = 0,  ///< Represents a low output level (0).
        HIGH = 1  ///< Represents a high output level (1).
    };

    /**
     * @brief Base class for GPIO control.
     * 
     * This class provides the basic attributes for GPIO pin management,
     * including the pin number and active low configuration.
     */
    class GpioBase {
        protected:
            gpio_num_t _pin;        ///< GPIO pin number.
            bool _active_low;       ///< Indicates if the pin is active low.
    };

    /**
     * @brief Class for GPIO input control.
     * 
     * This class allows for reading the state of a GPIO pin and handling input events.
     */
    class GpioInput : public GpioBase {
        private:
            /**
             * @brief Initializes the GPIO input.
             * 
             * @param pin The GPIO pin number to initialize.
             * @param activeLow Indicates if the pin is active low.
             * @return esp_err_t Status of the initialization (ESP_OK on success).
             */
            esp_err_t _init(const gpio_num_t pin, const bool activeLow);
            bool _event_handler_set = false;  ///< Flag indicating if an event handler is registered
            static bool _interrupt_service_installed;  ///< Flag indicating if the interrupt service is installed
            
        public:
            /**
             * @brief Constructor with pin and active low configuration.
             * 
             * @param pin The GPIO pin number to initialize.
             * @param activeLow Indicates if the pin is active low.
             */
            GpioInput(const gpio_num_t pin, const bool activeLow);
            
            /**
             * @brief Constructor with pin, default active low to false.
             * 
             * @param pin The GPIO pin number to initialize.
             */
            GpioInput(const gpio_num_t pin);
            
            /** @brief Default constructor. */
            GpioInput(void);
            
            /**
             * @brief Initializes the GPIO input.
             * 
             * @param pin The GPIO pin number to initialize.
             * @param activeLow Indicates if the pin is active low.
             * @return esp_err_t Status of the initialization (ESP_OK on success).
             */
            esp_err_t init(const gpio_num_t pin, const bool activeLow);
            
            /**
             * @brief Initializes the GPIO input with default active low.
             * 
             * @param pin The GPIO pin number to initialize.
             * @return esp_err_t Status of the initialization (ESP_OK on success).
             */
            esp_err_t init(const gpio_num_t pin);
            
            /**
             * @brief Reads the current state of the GPIO pin.
             * 
             * @return int The state of the GPIO pin (0 or 1).
             */
            int read(void);

            /**
             * @brief Enables the internal pull-up resistor for the GPIO pin.
             * 
             * @return esp_err_t Status of the operation (ESP_OK on success).
             */
            esp_err_t enablePullup(void);

            /**
             * @brief Disables the internal pull-up resistor for the GPIO pin.
             * 
             * @return esp_err_t Status of the operation (ESP_OK on success).
             */
            esp_err_t disablePullup(void);

            /**
             * @brief Enables the internal pull-down resistor for the GPIO pin.
             * 
             * @return esp_err_t Status of the operation (ESP_OK on success).
             */
            esp_err_t enablePulldown(void);

            /**
             * @brief Disables the internal pull-down resistor for the GPIO pin.
             * 
             * @return esp_err_t Status of the operation (ESP_OK on success).
             */
            esp_err_t disablePulldown(void);

            /**
             * @brief Enables both internal pull-up and pull-down resistors for the GPIO pin.
             * 
             * @return esp_err_t Status of the operation (ESP_OK on success).
             */
            esp_err_t enablePullupPulldown(void);

            /**
             * @brief Disables both internal pull-up and pull-down resistors for the GPIO pin.
             * 
             * @return esp_err_t Status of the operation (ESP_OK on success).
             */
            esp_err_t disablePullupPulldown(void);

            /**
             * @brief Enables interrupt functionality for the GPIO pin.
             * 
             * @param int_type The type of interrupt to enable.
             * @return esp_err_t Status of the operation (ESP_OK on success).
             */
            esp_err_t enableInterrupt(gpio_int_type_t int_type);

            /**
             * @brief Sets an event handler for GPIO input events.
             * 
             * @param Gpio_e_h Pointer to the event handler function.
             * @return esp_err_t Status of the operation (ESP_OK on success).
             */
            esp_err_t setEventHandler(esp_event_handler_t Gpio_e_h);

            /**
             * @brief Static callback function for GPIO interrupts.
             * 
             * This function is called when a GPIO interrupt occurs.
             * 
             * @param arg Pointer to the pin number that triggered the interrupt.
             */
            static void IRAM_ATTR gpio_isr_callback(void* arg);
    };

    /**
     * @brief Class for GPIO output control.
     * 
     * This class allows for controlling the state of a GPIO pin.
     */
    class GpioOutput: public GpioBase {
        private: 
            GpioLevel _level = GpioLevel::LOW; ///< Current level of the GPIO output.
            
            /**
             * @brief Initializes the GPIO output.
             * 
             * @param pin The GPIO pin number to initialize.
             * @param activeLow Indicates if the pin is active low.
             * @return esp_err_t Status of the initialization (ESP_OK on success).
             */
            esp_err_t _init(const gpio_num_t pin, const bool activeLow);

        public:
            /**
             * @brief Constructor with pin and active low configuration.
             * 
             * @param pin The GPIO pin number to initialize.
             * @param activeLow Indicates if the pin is active low.
             */
            GpioOutput(const gpio_num_t pin, const bool activeLow);
            
            /**
             * @brief Constructor with pin, default active low to false.
             * 
             * @param pin The GPIO pin number to initialize.
             */
            GpioOutput(const gpio_num_t pin);
            
            /** @brief Default constructor. */
            GpioOutput(void);
            
            /**
             * @brief Initializes the GPIO output.
             * 
             * @param pin The GPIO pin number to initialize.
             * @param activeLow Indicates if the pin is active low.
             * @return esp_err_t Status of the initialization (ESP_OK on success).
             */
            esp_err_t init(const gpio_num_t pin, const bool activeLow);
            
            /**
             * @brief Initializes the GPIO output with default active low.
             * 
             * @param pin The GPIO pin number to initialize.
             * @return esp_err_t Status of the initialization (ESP_OK on success).
             */
            esp_err_t init(const gpio_num_t pin);
            
            /**
             * @brief Turns the GPIO output on.
             * 
             * @return esp_err_t Status of the operation (ESP_OK on success).
             */
            esp_err_t on(void);
            
            /**
             * @brief Turns the GPIO output off.
             * 
             * @return esp_err_t Status of the operation (ESP_OK on success).
             */
            esp_err_t off(void);
            
            /**
             * @brief Toggles the GPIO output state.
             * 
             * @return esp_err_t Status of the operation (ESP_OK on success).
             */
            esp_err_t toggle(void);
            
            /**
             * @brief Sets the GPIO output to a specific level.
             * 
             * @param level The level to set (GpioLevel::LOW or GpioLevel::HIGH).
             * @return esp_err_t Status of the operation (ESP_OK on success).
             */
            esp_err_t setLevel(GpioLevel level);
    };

}

#endif