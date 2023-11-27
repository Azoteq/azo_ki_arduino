/******************************************************************************
 *                                                                            *
 *                                Copyright by                                *
 *                                                                            *
 *                              Azoteq (Pty) Ltd                              *
 *                          Republic of South Africa                          *
 *                                                                            *
 *                           Tel: +27(0)21 863 0033                           *
 *                           E-mail: info@azoteq.com                          *
 *                                                                            *
 * ========================================================================== *
 * @file        iqs7220a.cpp                                                  *
 * @brief       Functions for interaction with the IQS7220A device            *
 * @author      Hennie van der Westhuizen - Azoteq (Pty) Ltd                  *
 * @version     v0.0.1                                                        *
 * @date        2023                                                          *
 *****************************************************************************/
#include "azo_ki.hpp"

namespace AZO_KEYBOARD_INTERFACE
{
    /**
    * @name   iqs7220a_gpio_setup
    * @brief  Configures the GPIO pins for a keyboard device matrix.
    *         The pin_settings instance of the KeyboardInterface class defines
    *         the GPIO pins which should be configured. All pins are configured
    *         as software controlled inputs with internal pull-up resistors enabled.
    * @param  None
    * @retval None
    */
    void KeyboardInterface::iqs7220a_gpio_setup(){
        uint32_t value;
        uint32_t mask;
        uint32_t address;

        for (uint8_t i = 0; i < (this->num_columns); i++)
        {
            // Set all S0 pins as software controlled GPIO
            mask = this->pin_settings.s0_msk[i];
            address = 0x40014004 + log2(mask)*8;
            value = *(uint32_t*)(address);
            value &= 0xFFFFFFE0;
            value |= 5;
            *(uint32_t*)(address) = value;
            // Enable internal pullup resistors for all S0 pins
            address = 0x4001C004 + log2(mask)*4;
            value = *(uint32_t*)(address);
            value |= (1 << 3);
            *(uint32_t*)(address) = value;

            // Set all S1 pins as software controlled GPIO
            mask = this->pin_settings.s1_msk[i];
            address = 0x40014004 + log2(mask)*8;
            value = *(uint32_t*)(address);
            value &= 0xFFFFFFE0;
            value |= 5;
            // Enable internal pullup resistors for all S1 pins
            *(uint32_t*)(address) = value;
            address = 0x4001C004 + log2(mask)*4;
            value = *(uint32_t*)(address);
            value |= (1 << 3);
            *(uint32_t*)(address) = value;
        }

        for (uint8_t i = 0; i < (this->num_rows); i++)
        {
            // Set all D0 pins as software controlled GPIO
            mask = this->pin_settings.d0_msk[i];
            address = 0x40014004 + log2(mask)*8;
            value = *(uint32_t*)(address);
            value &= 0xFFFFFFE0;
            value |= 5;
            // Enable internal pullup resistors for all D0 pins
            *(uint32_t*)(address) = value;
            address = 0x4001C004 + log2(mask)*4;
            value = *(uint32_t*)(address);
            value |= (1 << 3);
            *(uint32_t*)(address) = value;

            // Set all D1 pins as software controlled GPIO
            mask = this->pin_settings.d1_msk[i];
            address = 0x40014004 + log2(mask)*8;
            value = *(uint32_t*)(address);
            value &= 0xFFFFFFE0;
            value |= 5;
            // Enable internal pullup resistors for all D1 pins
            *(uint32_t*)(address) = value;
            address = 0x4001C004 + log2(mask)*4;
            value = *(uint32_t*)(address);
            value |= (1 << 3);
            *(uint32_t*)(address) = value;
        }

        this->pin_settings.s0_all = 0;
        this->pin_settings.s1_all = 0;
        this->pin_settings.d0_all = 0;
        this->pin_settings.d1_all = 0;

        for (uint8_t i = 0; i < num_columns; i++)
        {
            this->pin_settings.s0_all |= this->pin_settings.s0_msk[i];
            this->pin_settings.s1_all |= this->pin_settings.s1_msk[i];
        }

        for (uint8_t i = 0; i < num_rows; i++)
        {
            this->pin_settings.d0_all |= this->pin_settings.d0_msk[i];
            this->pin_settings.d1_all |= this->pin_settings.d1_msk[i];
        }

        // Set all pins LOW
        *gpio_output_clear =    this->pin_settings.s0_all |
                                this->pin_settings.s1_all |
                                this->pin_settings.d0_all |
                                this->pin_settings.d1_all;

        // Set all pins as input
        *gpio_output_enable_clear = this->pin_settings.s0_all |
                                    this->pin_settings.s1_all |
                                    this->pin_settings.d0_all |
                                    this->pin_settings.d1_all;
    }

    /**
    * @name   iqs7220a_scan_keys_column
    * @brief  Scan channel and device states for a single column of devices
    *         in the device matrix. Populate the iqs7220a_key_scan_results instance
    *         of the KeyboardInterface class with the sampled results.
    * @param  colunm_select -> The index of the column which must be sampled.
    * @retval None
    */
    void KeyboardInterface::iqs7220a_scan_keys_column(uint8_t column_select){
        // Set S0 and S1 LOW
        *gpio_output_enable_set = this->pin_settings.s0_msk[column_select] | this->pin_settings.s1_msk[column_select];
        delayMicroseconds(SCAN_DELAY);

        // Read device reset state
        for (uint8_t i = 0; i < num_rows; i++)
        {  
            this->iqs7220a_key_scan_results[column_select][i][0] = *gpio_input & this->pin_settings.d0_msk[i];
        }

        // Set S0 HIGH
        *gpio_output_enable_clear = this->pin_settings.s0_msk[column_select];
        delayMicroseconds(SCAN_DELAY);

        // Read CH0&1 states
        for (uint8_t i = 0; i < num_rows; i++)
        {
            this->iqs7220a_key_scan_results[column_select][i][1] = *gpio_input & this->pin_settings.d0_msk[i];
            this->iqs7220a_key_scan_results[column_select][i][2] = *gpio_input & this->pin_settings.d1_msk[i];
        }

        // Set S1 HIGH, S0 LOW
        *gpio_output_enable_set = this->pin_settings.s0_msk[column_select];
        *gpio_output_enable_clear = this->pin_settings.s1_msk[column_select];
        delayMicroseconds(SCAN_DELAY);

        // Read CH2&3 states
        for (uint8_t i = 0; i < num_rows; i++)
        {
            this->iqs7220a_key_scan_results[column_select][i][3] = *gpio_input & this->pin_settings.d0_msk[i];
            this->iqs7220a_key_scan_results[column_select][i][4] = *gpio_input & this->pin_settings.d1_msk[i];
        }

        // Set S0 HIGH
        *gpio_output_enable_clear = this->pin_settings.s0_msk[column_select];
        delayMicroseconds(SCAN_DELAY);
    }

    /**
    * @name   iqs7220a_scan_keys_all
    * @brief  Scan channel and device states for all columns in the device
    *         matrix. Populate the iqs7220a_key_scan_results instance
    *         of the KeyboardInterface class with the sampled results.
    *         Communicate device results over serial.
    * @param  None
    * @retval None
    */
    void KeyboardInterface::iqs7220a_scan_keys_all(){
        uint8_t i,j,k,device_result;
        
        // Scan each column
        for (i = 0; i < this->num_columns; i++)
        {
            this->iqs7220a_scan_keys_column(i);
        }

        // Send byte value for each device
        for (i = 0; i < this->num_columns; i++)
        {
            for (j = 0; j < this->num_rows; j++)
            {
                device_result = 0;
                for (k = 0; k < AZQ700_KS_OUTPUT_PARAMS; k++)
                {
                    device_result |= (this->iqs7220a_key_scan_results[i][j][k] << k);
                }
                Serial.write(device_result);
            }
        }
    }

    /**
    * @name   iqs7220a_config_enter_column
    * @brief  Place the selected column of devices in the device matrix in
    *         the I2C configuration state where all devices will await a D1 rising edge.
    * @param  column_select -> Index of the column which must be placed in the configuration state.
    * @retval None
    */
    void KeyboardInterface::iqs7220a_config_enter_column(uint8_t column_select){
        // Set S0 and S1 LOW
        *gpio_output_enable_set = this->pin_settings.s0_msk[column_select] | this->pin_settings.s1_msk[column_select];
        delayMicroseconds(SCAN_DELAY);

        // Set S0 and S1 HIGH
        *gpio_output_enable_clear = this->pin_settings.s0_msk[column_select] | this->pin_settings.s1_msk[column_select];
        delayMicroseconds(SCAN_DELAY);
    }

    /**
    * @name   iqs7220a_config_enter_row
    * @brief  Place a single device in a column in the configuration state after
    *         the column has already been placed in the configuration state and is
    *         awaiting the D1 rising edge.
    * @param  row_select -> Index of the row which must be placed in the configuration state.
    * @retval None
    */
    void KeyboardInterface::iqs7220a_config_enter_row(uint8_t row_select){
        // Set D1 LOW
        *gpio_output_enable_set = this->pin_settings.d1_msk[row_select];
        delayMicroseconds(SCAN_DELAY);

        // Set D1 HIGH
        *gpio_output_enable_clear = this->pin_settings.d1_msk[row_select];
        delayMicroseconds(SCAN_DELAY);

        // Await D0 LOW
        for (uint8_t i = 0; i < 50; i++)
        {
            if (!(*gpio_input & this->pin_settings.d0_msk[row_select])) break;
            delayMicroseconds(20);
        }
    }

    /**
    * @name   iqs7220a_config_exit_row
    * @brief  Exit the configuration state of a single device in a column that has already
    *         been placed in the configuration state.
    * @param  row_select -> Index of the row which must exit the configuration state.
    * @retval None
    */
    void KeyboardInterface::iqs7220a_config_exit_row(uint8_t row_select){
        // Set D1 LOW
        *gpio_output_enable_set = this->pin_settings.d1_msk[row_select];
        delayMicroseconds(SCAN_DELAY);

        // Await D0 HIGH
        for (uint8_t i = 0; i < 50; i++)
        {
            if (*gpio_input & this->pin_settings.d0_msk[row_select]) break;
            delayMicroseconds(20);
        }

        // Set D1 HIGH
        *gpio_output_enable_clear = this->pin_settings.d1_msk[row_select];
        delayMicroseconds(SCAN_DELAY);
    }

    /**
    * @name   iqs7220a_i2c_read_single
    * @brief  I2C read operation on a single device in the device matrix.
    *         Parameters are defined in the do_command() function in the azo_ki_main.cpp file.
    *         Serial response containing I2C data.
    * @param  None
    * @retval None
    */
    void KeyboardInterface::iqs7220a_i2c_read_single(){
        // Enable I2C on IQS device
        this->iqs7220a_config_enter_column(this->get_device_column(this->i2c_control.device_select));
        this->iqs7220a_config_enter_row(this->get_device_row(this->i2c_control.device_select));

        this->i2c_control.input_index = 0;

        // Transmit I2C register that must be read from
        Wire.beginTransmission(this->i2c_control.device_addr);
        Wire.write(this->i2c_control.register_addr_lsb);
        Wire.endTransmission(false);

        // Receive I2C data
        Wire.requestFrom(this->i2c_control.device_addr, this->i2c_control.data_len);
        while (Wire.available())
        {
            this->i2c_control.input_data[this->i2c_control.input_index] = Wire.read();
            this->i2c_control.input_index++;
            if (this->i2c_control.input_index >= this->i2c_control.data_len) break;
        }

        // Disable I2C on IQS device
        this->iqs7220a_config_exit_row(get_device_row(this->i2c_control.device_select));

        Serial.write(this->i2c_control.input_data, this->i2c_control.data_len);
        memset(this->i2c_control.input_data, 0, this->i2c_control.data_len);
    }

    /**
    * @name   iqs7220a_i2c_write_single
    * @brief  I2C write operation to a single device in the device matrix.
    *         Parameters are defined in the do_command() function in the azo_ki_main.cpp file.
    * @param  None
    * @retval None
    */
    void KeyboardInterface::iqs7220a_i2c_write_single(){
        // Enable I2C on IQS device
        this->iqs7220a_config_enter_column(this->get_device_column(this->i2c_control.device_select));
        this->iqs7220a_config_enter_row(this->get_device_row(this->i2c_control.device_select));

        // I2C Comms
        Wire.beginTransmission(this->i2c_control.device_addr);
        Wire.write(this->i2c_control.register_addr_lsb);
        Wire.write(this->i2c_control.output_data, this->i2c_control.data_len);
        Wire.endTransmission(true);

        // Disable I2C on IQS device
        this->iqs7220a_config_exit_row(get_device_row(this->i2c_control.device_select));
    }

    /**
    * @name   iqs7220a_i2c_read_multi
    * @brief  I2C read operation on all devices in the device matrix.
    *         Parameters are defined in the do_command function in the azo_ki_main.cpp file.
    * @param  None
    * @retval None
    */
    void KeyboardInterface::iqs7220a_i2c_read_multi(){
        for (uint8_t i = 0; i < this->num_columns; i++)
        {
            this->iqs7220a_config_enter_column(i);
            for (uint8_t j = 0; j < this->num_rows; j++)
            {
                this->iqs7220a_config_enter_row(j);
                this->i2c_control.input_index = 0;
                
                // Transmit I2C register that must be read from
                Wire.beginTransmission(this->i2c_control.device_addr);
                Wire.write(this->i2c_control.register_addr_lsb);
                Wire.endTransmission(false);
                // Receive I2C data
                Wire.requestFrom(this->i2c_control.device_addr, this->i2c_control.data_len);
                while (Wire.available())
                {
                    this->i2c_control.input_data[this->i2c_control.input_index] = Wire.read();
                    this->i2c_control.input_index++;
                    if (this->i2c_control.input_index >= this->i2c_control.data_len) break;
                }

                Serial.write(this->i2c_control.input_data, this->i2c_control.data_len);
                memset(this->i2c_control.input_data, 0, this->i2c_control.data_len);
                this->iqs7220a_config_exit_row(get_device_row(j));
            }
        }
    }

    /**
    * @name   iqs7220a_config_enter_column
    * @brief  I2C write operation to all devices in the device matrix.
    *         Parameters are defined in the do_command() function in the azo_ki_main.cpp file.
    * @param  None
    * @retval None
    */
    void KeyboardInterface::iqs7220a_i2c_write_multi(){
        for (uint8_t i = 0; i < this->num_columns; i++)
        {
            this->iqs7220a_config_enter_column(i);
            for (uint8_t j = 0; j < this->num_rows; j++)
            {
                this->iqs7220a_config_enter_row(j);

                // I2C Comms
                Wire.beginTransmission(this->i2c_control.device_addr);
                Wire.write(this->i2c_control.register_addr_lsb);
                Wire.write(this->i2c_control.output_data, this->i2c_control.data_len);
                Wire.endTransmission(true);

                this->iqs7220a_config_exit_row(j);
            }
        }
    }

}