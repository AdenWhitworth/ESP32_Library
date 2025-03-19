#include "gpio.h"

namespace GPIO {
    /*================================= GpioInput ==============================*/
    
    bool GpioInput::_interrupt_service_installed{false};
    portMUX_TYPE GpioInput::_eventChangeMutex = portMUX_INITIALIZER_UNLOCKED;

    ESP_EVENT_DEFINE_BASE(INPUT_EVENTS);

    void IRAM_ATTR GpioInput::gpio_isr_callback(void *args){
        auto* typed_args = reinterpret_cast<interrupt_args*>(args);
        if (typed_args->type_tag != 0x47504941) {
            return;
        }
        int32_t pin = typed_args->_pin;
        bool custom_event_handler_set = typed_args->_custom_event_handler_set;
        bool event_handler_set = typed_args->_event_handler_set;
        bool queue_enabled = typed_args->_queue_enabled;
        esp_event_loop_handle_t custom_event_loop_handle = typed_args->_custom_event_loop_handle;
        QueueHandle_t queue_handle = typed_args->_queue_handle;

        if(queue_enabled){
            xQueueSendFromISR(queue_handle, &pin, NULL);
        } else if (custom_event_handler_set){
            esp_event_isr_post_to(custom_event_loop_handle, INPUT_EVENTS, pin, nullptr, 0, nullptr);
        } else if (event_handler_set){
            esp_event_isr_post(INPUT_EVENTS, pin, nullptr, 0, nullptr);
        }
    }

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

    /**
     * @brief Enables internal pull-up resistor for the GPIO input pin
     * 
     * @return esp_err_t Status of the operation (ESP_OK on success)
     */
    esp_err_t GpioInput::enablePullup(void){
        return gpio_set_pull_mode(_pin, GPIO_PULLUP_ONLY);
    }

    /**
     * @brief Disables internal pull-up resistor for the GPIO input pin
     * 
     * @return esp_err_t Status of the operation (ESP_OK on success)
     */
    esp_err_t GpioInput::disablePullup(void){
        return gpio_set_pull_mode(_pin, GPIO_FLOATING);
    }

    /**
     * @brief Enables internal pull-down resistor for the GPIO input pin
     * 
     * @return esp_err_t Status of the operation (ESP_OK on success)
     */
    esp_err_t GpioInput::enablePulldown(void){
        return gpio_set_pull_mode(_pin, GPIO_PULLDOWN_ONLY);
    }

    /**
     * @brief Disables internal pull-down resistor for the GPIO input pin
     * 
     * @return esp_err_t Status of the operation (ESP_OK on success)
     */
    esp_err_t GpioInput::disablePulldown(void){
        return gpio_set_pull_mode(_pin, GPIO_FLOATING);
    }

    /**
     * @brief Enables both internal pull-up and pull-down resistors for the GPIO input pin
     * 
     * @return esp_err_t Status of the operation (ESP_OK on success)
     */
    esp_err_t GpioInput::enablePullupPulldown(void){
        return gpio_set_pull_mode(_pin, GPIO_PULLUP_PULLDOWN);
    }

    /**
     * @brief Disables both internal pull-up and pull-down resistors for the GPIO input pin
     * 
     * @return esp_err_t Status of the operation (ESP_OK on success)
     */
    esp_err_t GpioInput::disablePullupPulldown(void){
        return gpio_set_pull_mode(_pin, GPIO_FLOATING);
    }

    /**
     * @brief Enables interrupt functionality for the GPIO input pin
     * 
     * Configures the interrupt type and installs the interrupt service if not already installed.
     * For active-low inputs, the interrupt type is automatically inverted.
     * 
     * @param int_type The type of interrupt to enable (e.g., GPIO_INTR_POSEDGE)
     * @return esp_err_t Status of the operation (ESP_OK on success)
     */
    esp_err_t GpioInput::enableInterrupt(gpio_int_type_t int_type){
        esp_err_t status{ESP_OK};

        if(_active_low){
            switch (int_type)
            {
            case GPIO_INTR_POSEDGE:
                int_type = GPIO_INTR_NEGEDGE;
                break;
            
            case GPIO_INTR_NEGEDGE:
                int_type = GPIO_INTR_POSEDGE;
                break;

            case GPIO_INTR_LOW_LEVEL:
                int_type = GPIO_INTR_HIGH_LEVEL;
                break;

            case GPIO_INTR_HIGH_LEVEL:
                int_type = GPIO_INTR_LOW_LEVEL;
                break;
            
            default:
                break;
            }
        }

        if (!_interrupt_service_installed) {
            status = gpio_install_isr_service(0);

            if(status == ESP_OK){
                _interrupt_service_installed = true;
            }
        }

        if (status == ESP_OK){
            status = gpio_set_intr_type(_pin, int_type);
        }

        if (status == ESP_OK){
            status = gpio_isr_handler_add(_pin, gpio_isr_callback, (void *)_pin);
        }

        return status;
    }

    esp_err_t GpioInput::setEventHandler(esp_event_handler_t Gpio_e_h){
        esp_err_t status{ESP_OK};

        taskENTER_CRITICAL(&_eventChangeMutex);

        status = _clearEventHandlers();

        status = esp_event_handler_instance_register(INPUT_EVENTS, _interrupt_args._pin, Gpio_e_h, 0, nullptr);

        if(status == ESP_OK){
            _interrupt_args._event_handler_set = true;
        }

        taskEXIT_CRITICAL(&_eventChangeMutex);

        return status;
    }

    esp_err_t GpioInput::setEventHandler(esp_event_loop_handle_t Gpio_e_l, esp_event_handler_t Gpio_e_h){
        esp_err_t status{ESP_OK};

        taskENTER_CRITICAL(&_eventChangeMutex);

        status = _clearEventHandlers();

        status = esp_event_handler_instance_register_with(Gpio_e_l, INPUT_EVENTS, _interrupt_args._pin, Gpio_e_h, 0, nullptr);

        if(status == ESP_OK){
            _event_handle = Gpio_e_h;
            _interrupt_args._custom_event_loop_handle = Gpio_e_l;
            _interrupt_args._event_handler_set = true;
        }

        taskEXIT_CRITICAL(&_eventChangeMutex);

        return status;
    }

    void GpioInput::setQueueHandle(QueueHandle_t Gpio_e_q){
        taskENTER_CRITICAL(&_eventChangeMutex);
        _clearEventHandlers();
        _interrupt_args._queue_handle = Gpio_e_q;
        _interrupt_args._queue_enabled = true;
        taskEXIT_CRITICAL(&_eventChangeMutex);
    }

    esp_err_t GpioInput::_clearEventHandlers(){
        esp_err_t status{ESP_OK};

        if(_interrupt_args._custom_event_handler_set){
            esp_event_handler_unregister_with(_interrupt_args._custom_event_loop_handle, INPUT_EVENTS, _interrupt_args._pin, _event_handle);
            _interrupt_args._custom_event_handler_set = false;
        } else if (_interrupt_args._event_handler_set){
            esp_event_handler_instance_unregister(INPUT_EVENTS, _interrupt_args._pin, nullptr);
            _interrupt_args._event_handler_set = false;
        }

        _interrupt_args._queue_handle = nullptr;
        _interrupt_args._queue_enabled = false;

        return status;
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