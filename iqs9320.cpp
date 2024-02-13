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
 * @file        iqs9320.cpp                                                   *
 * @brief       Functions for interaction with the IQS9320 device             *
 * @author      Hennie van der Westhuizen - Azoteq (Pty) Ltd                  *
 * @version     v0.0.2                                                        *
 * @date        2023                                                          *
 *****************************************************************************/
#include "azo_ki.hpp"

namespace AZO_KEYBOARD_INTERFACE
{
    /**
    * @name   iqs9320_gpio_setup
    * @brief  Configures the GPIO pins for a keyboard device matrix.
    *         The pin_settings instance of the KeyboardInterface class defines
    *         the GPIO pins which should be configured. All pins are configured
    *         as software controlled inputs with internal pull-up resistors enabled.
    * @param  None
    * @retval None
    */
    void KeyboardInterface::iqs9320_gpio_setup(){
         uint32_t value;
        uint32_t mask;
        uint32_t address;

        for (uint8_t i = 0; i < (this->num_columns); i++)
        {
            // Set all C0 pins as software controlled GPIO
            mask = this->pin_settings.c0_msk[i];
            address = 0x40014004 + log2(mask)*8;
            value = *(uint32_t*)(address);
            value &= 0xFFFFFFE0;
            value |= 5;
            *(uint32_t*)(address) = value;
            // Enable internal pullup resistors for all C0 pins
            address = 0x4001C004 + log2(mask)*4;
            value = *(uint32_t*)(address);
            value |= (1 << 3);
            *(uint32_t*)(address) = value;
        }

        for (uint8_t i = 0; i < (this->num_rows); i++)
        {
            // Set all R0 pins as software controlled GPIO
            mask = this->pin_settings.r0_msk[i];
            address = 0x40014004 + log2(mask)*8;
            value = *(uint32_t*)(address);
            value &= 0xFFFFFFE0;
            value |= 5;
            // Enable internal pullup resistors for all R0 pins
            *(uint32_t*)(address) = value;
            address = 0x4001C004 + log2(mask)*4;
            value = *(uint32_t*)(address);
            value |= (1 << 3);
            *(uint32_t*)(address) = value;

            // Set all R1 pins as software controlled GPIO
            mask = this->pin_settings.r1_msk[i];
            address = 0x40014004 + log2(mask)*8;
            value = *(uint32_t*)(address);
            value &= 0xFFFFFFE0;
            value |= 5;
            // Enable internal pullup resistors for all R1 pins
            *(uint32_t*)(address) = value;
            address = 0x4001C004 + log2(mask)*4;
            value = *(uint32_t*)(address);
            value |= (1 << 3);
            *(uint32_t*)(address) = value;

            // Set all R2 pins as software controlled GPIO
            mask = this->pin_settings.r2_msk[i];
            address = 0x40014004 + log2(mask)*8;
            value = *(uint32_t*)(address);
            value &= 0xFFFFFFE0;
            value |= 5;
            // Enable internal pullup resistors for all R2 pins
            *(uint32_t*)(address) = value;
            address = 0x4001C004 + log2(mask)*4;
            value = *(uint32_t*)(address);
            value |= (1 << 3);
            *(uint32_t*)(address) = value;

            // Set all R3 pins as software controlled GPIO
            mask = this->pin_settings.r3_msk[i];
            address = 0x40014004 + log2(mask)*8;
            value = *(uint32_t*)(address);
            value &= 0xFFFFFFE0;
            value |= 5;
            // Enable internal pullup resistors for all R3 pins
            *(uint32_t*)(address) = value;
            address = 0x4001C004 + log2(mask)*4;
            value = *(uint32_t*)(address);
            value |= (1 << 3);
            *(uint32_t*)(address) = value;
        }

        this->pin_settings.c0_all = 0;
        this->pin_settings.r0_all = 0;
        this->pin_settings.r1_all = 0;
        this->pin_settings.r2_all = 0;
        this->pin_settings.r3_all = 0;
        
        for (uint8_t i = 0; i < num_columns; i++)
        {
            this->pin_settings.c0_all |= this->pin_settings.c0_msk[i];
            
        }

        for (uint8_t i = 0; i < num_rows; i++)
        {
            this->pin_settings.r0_all |= this->pin_settings.r0_msk[i];
            this->pin_settings.r1_all |= this->pin_settings.r1_msk[i];
            this->pin_settings.r2_all |= this->pin_settings.r2_msk[i];
            this->pin_settings.r3_all |= this->pin_settings.r3_msk[i];
        }

        // Set all pins LOW
        *gpio_output_clear =    this->pin_settings.c0_all |
                                this->pin_settings.r0_all |
                                this->pin_settings.r1_all |
                                this->pin_settings.r2_all |
                                this->pin_settings.r3_all;

        // Set all pins as input
        *gpio_output_enable_clear = this->pin_settings.c0_all |
                                    this->pin_settings.r0_all |
                                    this->pin_settings.r1_all |
                                    this->pin_settings.r2_all |
                                    this->pin_settings.r3_all;

    }

    /**
    * @name   iqs9320_scan_keys_column
    * @brief  Scan channel and device states for a single column of devices
    *         in the device matrix. Populate the iqs7320a_key_scan_results instance
    *         of the KeyboardInterface class with the sampled results.
    *         The IQS9320 can produce different number of GPIO responses defined by the
    *         number of channels the device is configured for. 
    * @param  colunm_select -> The index of the column which must be sampled.
    * @param  num_channels  -> The number of channels the device is configured for.
    * @retval None
    */
    void KeyboardInterface::iqs9320_scan_keys_column(uint8_t column_select, uint8_t num_channels){
        // Set C0 LOW
        *gpio_output_enable_set = this->pin_settings.c0_msk[column_select];
        delayMicroseconds(SCAN_DELAY);

        // Read device reset state
        for (uint8_t i = 0; i < this->num_rows; i++)
        {
            iqs9320_key_scan_results[column_select][i][0] = *gpio_input & this->pin_settings.r1_msk[i]; // True when LOW
            iqs9320_key_scan_results[column_select][i][1] = *gpio_input & this->pin_settings.r2_msk[i]; // True when LOW
        }

        uint8_t key_scan_cycles = num_channels/4;
        key_scan_cycles += (num_channels%4) ? 1 : 0;
        bool c0_state = 0;

        for (uint8_t i = 0; i < key_scan_cycles; i++)
        {
            if (c0_state)
            {
                // Set C0 LOW
                *gpio_output_enable_set = this->pin_settings.c0_msk[column_select];
                c0_state = 0;
            }
            else
            {
                // Set C0 HIGH
                *gpio_output_enable_clear = this->pin_settings.c0_msk[column_select];
                c0_state = 1;
            }

            delayMicroseconds(SCAN_DELAY);

            // Read CH0, CH1, CH2
            for (uint8_t j = 0; j < this->num_rows; j++)
            {
                this->iqs9320_key_scan_results[column_select][j][2 + i*4] = *gpio_input & this->pin_settings.r0_msk[j];
                this->iqs9320_key_scan_results[column_select][j][3 + i*4] = *gpio_input & this->pin_settings.r1_msk[j];
                this->iqs9320_key_scan_results[column_select][j][4 + i*4] = *gpio_input & this->pin_settings.r2_msk[j];
                this->iqs9320_key_scan_results[column_select][j][5 + i*4] = *gpio_input & this->pin_settings.r3_msk[j];
            }
        }

        if (c0_state)
        {
            // Set C0 LOW
            *gpio_output_enable_set = this->pin_settings.c0_msk[column_select];
            c0_state = 0;
        }
        else
        {
            // Set C0 HIGH
            *gpio_output_enable_clear = this->pin_settings.c0_msk[column_select];
            c0_state = 1;
        }
        delayMicroseconds(SCAN_DELAY);

        if (!c0_state)
        {
            // Set C0 HIGH
            *gpio_output_enable_clear = this->pin_settings.c0_msk[column_select];
            delayMicroseconds(SCAN_DELAY);
        }
    }

    /**
    * @name   iqs9320_scan_keys_all
    * @brief  Scan channel and device states for all columns in the device
    *         matrix. Populate the iqs7320a_key_scan_results instance
    *         of the KeyboardInterface class with the sampled results.
    *         Communicate device results over serial.
    *         The IQS9320 can produce different number of GPIO responses 
    *         defined by the number of channels the device is configured for.
    * @param  num_channels -> The number of channels the device is configured for.
    * @retval None
    */
    void KeyboardInterface::iqs9320_scan_keys_all(uint8_t num_channels){
         uint8_t i,j,k;
        uint32_t device_result;
        // Scan all keys
        for (i = 0; i < this->num_columns; i++)
        {
            this->iqs9320_scan_keys_column(i, num_channels);
        }
        // Send byte value for each device
        for (i = 0; i < this->num_columns; i++)
        {
            for (j = 0; j < this->num_rows; j++)
            {
                device_result = 0;
                for (k = 0; k < (2+num_channels); k++)
                {
                    device_result |= (this->iqs9320_key_scan_results[i][j][k] << k);
                }
                Serial.write(device_result & 0xFF);
                Serial.write((device_result & 0xFF00) >> 8);
                Serial.write((device_result & 0xFF0000) >> 16);
            }
        }
    }

    /**
    * @name   iqs9320_config_enter
    * @brief  Place a single device in the matrix in the configuration state.
    * @param  column_select -> Index of the column in which the device is that must be placed in
    *                          the configuration mode.
    * @param  row_select    -> Index of the row in which the device is that must be placed in the
    *                          configuration state.
    * @retval None
    */
    void KeyboardInterface::iqs9320_config_enter(uint8_t column_select, uint8_t row_select){
        // R0 LOW for selected row
        *gpio_output_enable_set = this->pin_settings.r0_msk[row_select];

        // R3 LOW for all other rows
        *gpio_output_enable_set = this->pin_settings.r3_all & ~(this->pin_settings.r3_msk[row_select]);

        // C0 LOW
        *gpio_output_enable_set = this->pin_settings.c0_msk[column_select];
        delayMicroseconds(SCAN_DELAY);

        // C0 HIGH
        *gpio_output_enable_clear = this->pin_settings.c0_msk[column_select];
        delayMicroseconds(SCAN_DELAY);

        // R0 HIGH & R3 HIGH for all rows
        *gpio_output_enable_clear = this->pin_settings.r0_all | this->pin_settings.r3_all;
        delayMicroseconds(SCAN_DELAY);

        // Await R1 Falling Edge (1ms timeout)
        for (uint8_t i = 0; i < 50; i++)
        {
            if ((*gpio_input & this->pin_settings.r1_msk[row_select]) == 0) break;
            delayMicroseconds(20);
        }
    }

    /**
    * @name   iqs9320_config_exit
    * @brief  Exit the configuration state for a single device in the matrix.
    *         Only the row index is required to exit the configuration state of the device.
    * @param  row_select -> The row index of the device which must exit the configuration state.
    * @retval None
    */
    void KeyboardInterface::iqs9320_config_exit(uint8_t row_select){
        // R0 LOW
        *gpio_output_enable_set = this->pin_settings.r0_msk[row_select];
        delayMicroseconds(SCAN_DELAY);

        // Await R1 Rising Edge (1ms timeout)
        for (uint8_t i = 0; i < 50; i++)
        {
            if ((*gpio_input & this->pin_settings.r1_msk[row_select]) != 0) break;
            delayMicroseconds(20);
        }

        // R0 HIGH
        *gpio_output_enable_clear = this->pin_settings.r0_msk[row_select];
    }

    /**
    * @name   iqs9320_standby_enter
    * @brief  Enter the standby mode for all devices in the device matrix.
    * @param  None
    * @retval None
    */
    void KeyboardInterface::iqs9320_standby_enter(){
        // R0 LOW
        *gpio_output_enable_set = this->pin_settings.r0_all;
        delayMicroseconds(SCAN_DELAY);

        // C0 LOW
        *gpio_output_enable_set = this->pin_settings.c0_all;
        delayMicroseconds(SCAN_DELAY);

        // R0 HIGH
        *gpio_output_enable_clear = this->pin_settings.r0_all;
        delayMicroseconds(SCAN_DELAY);

        // C0 HIGH
        *gpio_output_enable_clear = this->pin_settings.c0_all;
        delayMicroseconds(SCAN_DELAY);
    }

    /**
    * @name   iqs9320_standby_exit
    * @brief  Exit the standby mode for all device in the device matrix.
    * @param  None
    * @retval None
    */
    void KeyboardInterface::iqs9320_standby_exit(){
        // R0 LOW
        *gpio_output_enable_set = this->pin_settings.r0_all;
        delayMicroseconds(SCAN_DELAY);

        // C0 LOW
        *gpio_output_enable_set = this->pin_settings.c0_all;
        delay(1);

        // C0 HIGH
        *gpio_output_enable_clear = this->pin_settings.c0_all;
        delayMicroseconds(SCAN_DELAY);

        // R0 HIGH
        *gpio_output_enable_clear = this->pin_settings.r0_all;
        delayMicroseconds(SCAN_DELAY);
    }

    /**
    * @name   iqs9320_i2c_read_fp
    * @brief  I2C read operation (full-polling) on a single device.
    *         This function is applicable to IQS9320 devices which are
    *         configured for the I2C communications interface and do not support key scanning.
    *         Parameters are defined in the do_command() function in the azo_ki_main.cpp file.
    * @param  None
    * @retval None
    */
    void KeyboardInterface::iqs9320_i2c_read_fp(){
        this->i2c_control.input_index = 0;

        // Default read condition
        if ((this->i2c_control.register_addr_lsb != 0xFF) && (this->i2c_control.register_addr_msb != 0xFF))
        {
            Wire.beginTransmission(this->i2c_control.device_addr);
            Wire.write(this->i2c_control.register_addr_lsb);
            Wire.write(this->i2c_control.register_addr_msb);
            Wire.endTransmission(false);
        }

        // Read at specific address
        Wire.requestFrom(this->i2c_control.device_addr, this->i2c_control.data_len);
        while(Wire.available())
        {
            this->i2c_control.input_data[this->i2c_control.input_index] = Wire.read();
            this->i2c_control.input_index++;
        }

        Serial.write(this->i2c_control.input_data, this->i2c_control.data_len);
        memset(this->i2c_control.input_data, 0, this->i2c_control.data_len);
    }

    /**
    * @name   iqs9320_i2c_write_fp
    * @brief  I2C write operation (full-polling) to a single device.
    *         This function is applicable to IQS9320 devices which are
    *         configured for the I2C communications interface and do not support key scanning.
    *         Parameters are defined in the do_command() function in the azo_ki_main.cpp file.
    * @param  None
    * @retval None
    */
    void KeyboardInterface::iqs9320_i2c_write_fp(){
        Wire.beginTransmission(this->i2c_control.device_addr);
        Wire.write(this->i2c_control.register_addr_lsb);
        Wire.write(this->i2c_control.register_addr_msb);
        Wire.write(this->i2c_control.output_data, this->i2c_control.data_len);
        Wire.endTransmission();
    }

    /**
    * @name   iqs9320_i2c_read_ks
    * @brief  I2C read operation (full-polling) on a single device in the device matrix.
    *         This function is applicable to IQS9320 devices which are
    *         configured for the key scanning communications interface and do not
    *         support full-polling I2C.
    *         Parameters are defined in the do_command() function in the azo_ki_main.cpp file.
    * @param  None
    * @retval None
    */
    void KeyboardInterface::iqs9320_i2c_read_ks(){
        // Enable I2C on IQS device
        this->iqs9320_config_enter(get_device_column(this->i2c_control.device_select), get_device_row(this->i2c_control.device_select));

        this->i2c_control.input_index = 0;

        // I2C Comms
        Wire.beginTransmission(this->i2c_control.device_addr);
        Wire.write(this->i2c_control.register_addr_lsb);
        Wire.write(this->i2c_control.register_addr_msb);
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
        this->iqs9320_config_exit(get_device_row(this->i2c_control.device_select));

        Serial.write(this->i2c_control.input_data, this->i2c_control.data_len);
        memset(this->i2c_control.input_data, 0, this->i2c_control.data_len);
    }

    /**
    * @name   iqs9320_i2c_write_ks
    * @brief  I2C write operation (full-polling) to a single device in the device matrix.
    *         This function is applicable to IQS9320 devices which are
    *         configured for the key scanning communications interface and do not
    *         support full-polling I2C.
    *         Parameters are defined in the do_command() function in the azo_ki_main.cpp file.
    * @param  None
    * @retval None
    */
    void KeyboardInterface::iqs9320_i2c_write_ks(){
        // Enable I2C on IQS device
        this->iqs9320_config_enter(get_device_column(this->i2c_control.device_select), get_device_row(this->i2c_control.device_select));

        // I2C Comms
        Wire.beginTransmission(this->i2c_control.device_addr);
        Wire.write(this->i2c_control.register_addr_lsb);
        Wire.write(this->i2c_control.register_addr_msb);
        Wire.write(this->i2c_control.output_data, this->i2c_control.data_len);
        Wire.endTransmission(true);

        // Disable I2C on IQS device
        this->iqs9320_config_exit(get_device_row(this->i2c_control.device_select));
    }

}