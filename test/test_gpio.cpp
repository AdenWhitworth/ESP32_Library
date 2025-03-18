#include <unity.h>
#include "gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

using namespace GPIO;

void setUp(void) {
    // Set up before each test
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

void RUN_UNITY_TESTS() {
    UNITY_BEGIN();
    
    RUN_TEST(test_gpio_input);
    RUN_TEST(test_gpio_output);
    RUN_TEST(test_gpio_active_low);
    
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