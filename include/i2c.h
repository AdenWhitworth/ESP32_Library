#ifndef I2C_H
#define I2C_H

#include "driver/i2c.h"
#include "esp_intr_alloc.h"

namespace I2C {
    /**
     * @brief Class for handling I2C communication on ESP32
     * 
     * This class provides a high-level interface for I2C communication,
     * supporting both master and slave modes. It wraps the ESP-IDF I2C driver
     * functions to provide a more user-friendly API.
     */
    class I2c {
        private:
            uint16_t _slaveAddr{};        ///< I2C slave address
            i2c_port_t _port{};           ///< I2C port number (I2C_NUM_0, I2C_NUM_1, etc.)
            i2c_mode_t _mode{};           ///< I2C mode (master or slave)
            size_t _slv_rx_buf_len{};     ///< Slave receive buffer length
            size_t _slv_tx_buf_len{};     ///< Slave transmit buffer length
            int _intr_alloc_flags{};      ///< Interrupt allocation flags
        
        public:
            /**
             * @brief Construct a new I2c object
             * 
             * @param port I2C port number (I2C_NUM_0, I2C_NUM_1, etc.)
             * @param slv_rx_buf_len Slave receive buffer length (default: 0)
             * @param slv_tx_buf_len Slave transmit buffer length (default: 0)
             * @param intr_alloc_flags Interrupt allocation flags (default: 0)
             */
            I2c(i2c_port_t port, size_t slv_rx_buf_len = 0, size_t slv_tx_buf_len = 0, int intr_alloc_flags = 0);

            /**
             * @brief Destroy the I2c object and clean up I2C driver
             */
            ~I2c();

            /**
             * @brief Initialize I2C in master mode
             * 
             * @param sda_io_num GPIO number for SDA pin
             * @param scl_io_num GPIO number for SCL pin
             * @param clk_speed I2C clock speed in Hz
             * @param sda_pullup_en Enable internal pullup for SDA pin (default: false)
             * @param scl_pullup_en Enable internal pullup for SCL pin (default: false)
             * @param clk_flags I2C clock flags (default: I2C_SCLK_SRC_FLAG_FOR_NOMAL)
             * @return esp_err_t ESP_OK on success, error code otherwise
             */
            esp_err_t InitMaster(int sda_io_num,
                             int scl_io_num,
                             uint32_t clk_speed,
                             bool sda_pullup_en = false,
                             bool scl_pullup_en = false,
                             uint32_t clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL);
            
            /**
             * @brief Read a single byte from an I2C register
             * 
             * @param dev_addr I2C device address
             * @param reg_addr Register address to read from
             * @return uint8_t Value read from the register
             */
            uint8_t ReadRegister(uint8_t dev_addr, uint8_t reg_addr);

            /**
             * @brief Write a single byte to an I2C register
             * 
             * @param dev_addr I2C device address
             * @param reg_addr Register address to write to
             * @param txData Data to write to the register
             * @return esp_err_t ESP_OK on success, error code otherwise
             */
            esp_err_t WriteRegister(uint8_t dev_addr, uint8_t reg_addr, uint8_t txData);

            /**
             * @brief Read multiple bytes from an I2C register
             * 
             * @param dev_addr I2C device address
             * @param reg_addr Register address to read from
             * @param rx_data Buffer to store the read data
             * @param length Number of bytes to read
             * @return esp_err_t ESP_OK on success, error code otherwise
             */
            esp_err_t ReadRegisterMultipleBytes(uint8_t dev_addr, uint8_t reg_addr, uint8_t *rx_data, int length);

            /**
             * @brief Write multiple bytes to an I2C register
             * 
             * @param dev_addr I2C device address
             * @param reg_addr Register address to write to
             * @param tx_data Buffer containing data to write
             * @param length Number of bytes to write
             * @return esp_err_t ESP_OK on success, error code otherwise
             */
            esp_err_t WriteRegisterMultipleBytes(uint8_t dev_addr, uint8_t reg_addr, uint8_t *tx_data, int length);
    };
}

#endif