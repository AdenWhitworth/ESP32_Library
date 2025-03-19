#include <unity.h>
#include "i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

using namespace I2C;

void setUp(void) {
    // Set up before each test
}

void tearDown(void) {
    // Clean up after each test
}

void test_i2c_initialization() {
    I2c i2c(I2C_NUM_0, 0, 0, 0);
    esp_err_t result = i2c.InitMaster(21, 22, 100000, true, true, 0);
    TEST_ASSERT_EQUAL(ESP_OK, result);
}

void test_i2c_read_write() {
    I2c i2c(I2C_NUM_0, 0, 0, 0);
    TEST_ASSERT_EQUAL(ESP_OK, i2c.InitMaster(21, 22, 100000, true, true, 0));
    
    // Test device address (0x36 for STEMMA soil sensor)
    const uint8_t dev_addr = 0x36;
    
    // Test register read
    uint8_t value = i2c.ReadRegister(dev_addr, 0x0F);
    TEST_ASSERT_TRUE(value >= 0 && value <= 255);
    
    // Test register write
    TEST_ASSERT_EQUAL(ESP_OK, i2c.WriteRegister(dev_addr, 0x0F, 0x00));
}

void test_i2c_multiple_bytes() {
    I2c i2c(I2C_NUM_0, 0, 0, 0);
    TEST_ASSERT_EQUAL(ESP_OK, i2c.InitMaster(21, 22, 100000, true, true, 0));
    
    const uint8_t dev_addr = 0x36;
    uint8_t rx_data[2];
    
    // Test reading multiple bytes
    TEST_ASSERT_EQUAL(ESP_OK, i2c.ReadRegisterMultipleBytes(dev_addr, 0x00, rx_data, 2));
    
    // Test writing multiple bytes
    uint8_t tx_data[2] = {0x00, 0x00};
    TEST_ASSERT_EQUAL(ESP_OK, i2c.WriteRegisterMultipleBytes(dev_addr, 0x00, tx_data, 2));
}

void RUN_UNITY_TESTS() {
    UNITY_BEGIN();
    
    RUN_TEST(test_i2c_initialization);
    RUN_TEST(test_i2c_read_write);
    RUN_TEST(test_i2c_multiple_bytes);
    
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