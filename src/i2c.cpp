#include "i2c.h"

namespace I2C {
    /**
     * @brief Construct a new I2c object
     * 
     * Initializes the I2C object with the specified port and buffer configurations.
     * The object is not ready for use until InitMaster() is called.
     * 
     * @param port I2C port number (I2C_NUM_0, I2C_NUM_1, etc.)
     * @param slv_rx_buf_len Slave receive buffer length
     * @param slv_tx_buf_len Slave transmit buffer length
     * @param intr_alloc_flags Interrupt allocation flags
     */
    I2c::I2c(i2c_port_t port, size_t slv_rx_buf_len, size_t slv_tx_buf_len, int intr_alloc_flags){
        _port = port;
        _slv_rx_buf_len = slv_rx_buf_len;
        _slv_tx_buf_len = slv_tx_buf_len;
        _intr_alloc_flags = intr_alloc_flags;
    }

    /**
     * @brief Destroy the I2c object
     * 
     * Cleans up the I2C driver and frees resources.
     */
    I2c::~I2c(){
        i2c_driver_delete(_port);
    }

    /**
     * @brief Initialize I2C in master mode
     * 
     * Configures the I2C driver for master mode operation with the specified
     * GPIO pins and clock settings. This must be called before using any
     * read or write operations.
     * 
     * @param sda_io_num GPIO number for SDA pin
     * @param scl_io_num GPIO number for SCL pin
     * @param clk_speed I2C clock speed in Hz
     * @param sda_pullup_en Enable internal pullup for SDA pin
     * @param scl_pullup_en Enable internal pullup for SCL pin
     * @param clk_flags I2C clock flags
     * @return esp_err_t ESP_OK on success, error code otherwise
     */
    esp_err_t I2c::InitMaster(int sda_io_num, int scl_io_num, uint32_t clk_speed, bool sda_pullup_en, bool scl_pullup_en, uint32_t clk_flags){
        esp_err_t status{ESP_OK};
        i2c_config_t _config{};
        _mode = I2C_MODE_MASTER;

        _config.mode = I2C_MODE_MASTER;
        _config.sda_io_num = sda_io_num;
        _config.scl_io_num = scl_io_num;
        _config.master.clk_speed = clk_speed;
        _config.sda_pullup_en = sda_pullup_en;
        _config.scl_pullup_en = scl_pullup_en;
        _config.clk_flags = clk_flags;

        status |= i2c_param_config(_port, &_config);
        status |= i2c_driver_install(_port, _mode, _slv_rx_buf_len, _slv_tx_buf_len, 0);
        return status;
    }
            
    /**
     * @brief Read a single byte from an I2C register
     * 
     * Performs a read operation from a single register on the I2C device.
     * Uses a 1-second timeout for the operation.
     * 
     * @param dev_addr I2C device address
     * @param reg_addr Register address to read from
     * @return uint8_t Value read from the register
     */
    uint8_t I2c::ReadRegister(uint8_t dev_addr, uint8_t reg_addr){
        uint8_t rxBuf{};
        i2c_master_write_read_device(_port, dev_addr, &reg_addr, 1, &rxBuf, 1, pdMS_TO_TICKS(1000));
        return rxBuf;
    }
    
    /**
     * @brief Write a single byte to an I2C register
     * 
     * Performs a write operation to a single register on the I2C device.
     * Uses a 1-second timeout for the operation.
     * 
     * @param dev_addr I2C device address
     * @param reg_addr Register address to write to
     * @param txData Data to write to the register
     * @return esp_err_t ESP_OK on success, error code otherwise
     */
    esp_err_t I2c::WriteRegister(uint8_t dev_addr, uint8_t reg_addr, uint8_t txData){
        const uint8_t txBuf[2] {reg_addr, txData};
        return i2c_master_write_to_device(_port, dev_addr, txBuf, 2, pdMS_TO_TICKS(1000));
    }

    /**
     * @brief Read multiple bytes from an I2C register
     * 
     * Performs a read operation from multiple consecutive registers on the I2C device.
     * Uses a 1-second timeout for the operation.
     * 
     * @param dev_addr I2C device address
     * @param reg_addr Starting register address to read from
     * @param rx_data Buffer to store the read data
     * @param length Number of bytes to read
     * @return esp_err_t ESP_OK on success, error code otherwise
     */
    esp_err_t I2c::ReadRegisterMultipleBytes(uint8_t dev_addr, uint8_t reg_addr, uint8_t *rx_data, int length){
        return i2c_master_write_read_device(_port, dev_addr, &reg_addr, 1, rx_data, length, pdMS_TO_TICKS(1000));
    }
    
    /**
     * @brief Write multiple bytes to an I2C register
     * 
     * Performs a write operation to multiple consecutive registers on the I2C device.
     * Uses a static command link for better performance and a 1-second timeout.
     * 
     * @param dev_addr I2C device address
     * @param reg_addr Starting register address to write to
     * @param tx_data Buffer containing data to write
     * @param length Number of bytes to write
     * @return esp_err_t ESP_OK on success, error code otherwise
     */
    esp_err_t I2c::WriteRegisterMultipleBytes(uint8_t dev_addr, uint8_t reg_addr, uint8_t *tx_data, int length){
        esp_err_t status{ESP_OK};
        uint8_t buffer[I2C_LINK_RECOMMENDED_SIZE(3)] = { 0 };
        i2c_cmd_handle_t _handle = i2c_cmd_link_create_static(buffer, sizeof(buffer));
        
        status |= i2c_master_start(_handle);
        status |= i2c_master_write_byte(_handle, (dev_addr << 1) | I2C_MASTER_WRITE, true);
        status |= i2c_master_write_byte(_handle, reg_addr, true);
        status |= i2c_master_write(_handle, tx_data, length, true);
        status |= i2c_master_stop(_handle);
        status |= i2c_master_cmd_begin(_port, _handle, pdMS_TO_TICKS(1000));
        i2c_cmd_link_delete_static(_handle);
        return status;
    }
}