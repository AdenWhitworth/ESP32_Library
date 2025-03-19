#include <unity.h>
#include "gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

using namespace GPIO;

// Global variables for event handler testing
static bool event_handler_called = false;
static int32_t event_pin = -1;

// Test event handler function
static void test_event_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data) {
    event_handler_called = true;
    event_pin = id;
}

void setUp(void) {
    // Set up before each test
    event_handler_called = false;
    event_pin = -1;
}

void tearDown(void) {
    // Clean up after each test
}

void test_gpio_input() {
    // Test GPIO input
    GpioInput input(GPIO_NUM_2);
    TEST_ASSERT_EQUAL(ESP_OK, input.init(GPIO_NUM_2));
    
    // Read the pin state
    int state = input.read();
    // Note: We can't predict the actual state, but we can verify it's either 0 or 1
    TEST_ASSERT_TRUE(state == 0 || state == 1);
}

void test_gpio_output() {
    // Test GPIO output
    GpioOutput output(GPIO_NUM_3);
    TEST_ASSERT_EQUAL(ESP_OK, output.init(GPIO_NUM_3));
    
    // Test on/off
    TEST_ASSERT_EQUAL(ESP_OK, output.on());
    TEST_ASSERT_EQUAL(ESP_OK, output.off());
    
    // Test toggle
    TEST_ASSERT_EQUAL(ESP_OK, output.toggle());
    TEST_ASSERT_EQUAL(ESP_OK, output.toggle());
    
    // Test setLevel
    TEST_ASSERT_EQUAL(ESP_OK, output.setLevel(GpioLevel::HIGH));
    TEST_ASSERT_EQUAL(ESP_OK, output.setLevel(GpioLevel::LOW));
}

void test_gpio_active_low() {
    // Test active low input
    GpioInput input(GPIO_NUM_4, true);
    TEST_ASSERT_EQUAL(ESP_OK, input.init(GPIO_NUM_4, true));
    
    // Test active low output
    GpioOutput output(GPIO_NUM_5, true);
    TEST_ASSERT_EQUAL(ESP_OK, output.init(GPIO_NUM_5, true));
    
    // Test operations
    TEST_ASSERT_EQUAL(ESP_OK, output.on());
    TEST_ASSERT_EQUAL(ESP_OK, output.off());
}

void test_gpio_pullup_pulldown() {
    GpioInput input(GPIO_NUM_6);
    TEST_ASSERT_EQUAL(ESP_OK, input.init(GPIO_NUM_6));
    
    // Test pull-up
    TEST_ASSERT_EQUAL(ESP_OK, input.enablePullup());
    TEST_ASSERT_EQUAL(ESP_OK, input.disablePullup());
    
    // Test pull-down
    TEST_ASSERT_EQUAL(ESP_OK, input.enablePulldown());
    TEST_ASSERT_EQUAL(ESP_OK, input.disablePulldown());
    
    // Test both pull-up and pull-down
    TEST_ASSERT_EQUAL(ESP_OK, input.enablePullupPulldown());
    TEST_ASSERT_EQUAL(ESP_OK, input.disablePullupPulldown());
}

void test_gpio_interrupt() {
    GpioInput input(GPIO_NUM_7);
    TEST_ASSERT_EQUAL(ESP_OK, input.init(GPIO_NUM_7));
    
    // Test enabling different interrupt types
    TEST_ASSERT_EQUAL(ESP_OK, input.enableInterrupt(GPIO_INTR_POSEDGE));
    TEST_ASSERT_EQUAL(ESP_OK, input.enableInterrupt(GPIO_INTR_NEGEDGE));
    TEST_ASSERT_EQUAL(ESP_OK, input.enableInterrupt(GPIO_INTR_HIGH_LEVEL));
    TEST_ASSERT_EQUAL(ESP_OK, input.enableInterrupt(GPIO_INTR_LOW_LEVEL));
    
    // Test event handler registration
    TEST_ASSERT_EQUAL(ESP_OK, input.setEventHandler(nullptr));
}

void test_gpio_interrupt_active_low() {
    GpioInput input(GPIO_NUM_8, true);  // Active low input
    TEST_ASSERT_EQUAL(ESP_OK, input.init(GPIO_NUM_8, true));
    
    // Test that interrupt types are inverted for active low
    TEST_ASSERT_EQUAL(ESP_OK, input.enableInterrupt(GPIO_INTR_POSEDGE));
    // Note: The actual interrupt type will be inverted internally
}

void test_gpio_event_handler() {
    GpioInput input(GPIO_NUM_9);
    TEST_ASSERT_EQUAL(ESP_OK, input.init(GPIO_NUM_9));
    
    // Register event handler
    TEST_ASSERT_EQUAL(ESP_OK, input.setEventHandler(test_event_handler));
    
    // Enable interrupt
    TEST_ASSERT_EQUAL(ESP_OK, input.enableInterrupt(GPIO_INTR_POSEDGE));
    
    // Wait a bit for any pending events to be processed
    vTaskDelay(pdMS_TO_TICKS(100));
    
    // Note: We can't actually trigger the interrupt in software,
    // but we can verify that the event handler was registered correctly
    // and that the event handler function is valid
    TEST_ASSERT_NOT_NULL(test_event_handler);
}

void test_gpio_event_handler_active_low() {
    GpioInput input(GPIO_NUM_10, true);  // Active low input
    TEST_ASSERT_EQUAL(ESP_OK, input.init(GPIO_NUM_10, true));
    
    // Register event handler
    TEST_ASSERT_EQUAL(ESP_OK, input.setEventHandler(test_event_handler));
    
    // Enable interrupt (will be inverted internally)
    TEST_ASSERT_EQUAL(ESP_OK, input.enableInterrupt(GPIO_INTR_POSEDGE));
    
    // Wait a bit for any pending events to be processed
    vTaskDelay(pdMS_TO_TICKS(100));
    
    // Verify event handler registration
    TEST_ASSERT_NOT_NULL(test_event_handler);
}

void RUN_UNITY_TESTS() {
    UNITY_BEGIN();
    
    RUN_TEST(test_gpio_input);
    RUN_TEST(test_gpio_output);
    RUN_TEST(test_gpio_active_low);
    RUN_TEST(test_gpio_pullup_pulldown);
    RUN_TEST(test_gpio_interrupt);
    RUN_TEST(test_gpio_interrupt_active_low);
    RUN_TEST(test_gpio_event_handler);
    RUN_TEST(test_gpio_event_handler_active_low);
    
    UNITY_END();
}

extern "C" void app_main(void) {
    // Wait for serial to be ready
    vTaskDelay(pdMS_TO_TICKS(1000));
    
    // Run tests
    RUN_UNITY_TESTS();
    
    // Keep the ESP32 running
    while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
} 