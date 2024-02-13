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
 * @file        azo_ki_main.cpp                                               *
 * @brief       Class constuctors, main application loop,                     *
 *              serial communication handling                                 *
 * @author      Hennie van der Westhuizen - Azoteq (Pty) Ltd                  *
 * @version     v0.0.2                                                        *
 * @date        2023                                                          *
 *****************************************************************************/
#include "azo_ki.hpp"

// HW control register addresses
uint32_t* gpio_input               = (uint32_t*)0xd0000004;
uint32_t* gpio_output              = (uint32_t*)0xd0000010;
uint32_t* gpio_output_set          = (uint32_t*)0xd0000014;
uint32_t* gpio_output_clear        = (uint32_t*)0xd0000018;
uint32_t* gpio_output_enable       = (uint32_t*)0xd0000020;
uint32_t* gpio_output_enable_set   = (uint32_t*)0xd0000024;
uint32_t* gpio_output_enable_clear = (uint32_t*)0xd0000028;

// Default serial return values
uint8_t return_arr[4] = {0xFF, 0xFF, 0xFF, 0xFF};

namespace AZO_KEYBOARD_INTERFACE
{
    /**
    * @name   KeyboardInterface
    * @brief  Specialised constructor for the KeyboardInterface class
    *         Will use the pin_settings_param instance to define the GPIO selection
    * @param  pin_settings_param -> Define GPIO selection for Serial baudrate, I2C clock speed, 
    *                               I2C pin selection, key scan pin selection
    * @retval None
    */
    KeyboardInterface::KeyboardInterface(pin_settings_t pin_settings_param)
    {
        this->pin_settings = pin_settings_param;
    }

    /**
    * @name   KeyboardInterface
    * @brief  Default constructor for the KeyboardInterface class
    *         Will use default_pin_settings instance to define GPIO selection
    * @param  None
    * @retval None
    */
    KeyboardInterface::KeyboardInterface()
    {
        this->pin_settings = default_pin_settings;
    }

    /**
    * @name   comms_setup
    * @brief  Setup serial and I2C communications
    * @param  None
    * @retval None
    */
    void KeyboardInterface::comms_setup()
    {
        Serial.begin(this->pin_settings.serial_baud_rate);
        Wire.setSDA(this->pin_settings.pin_sda_0);
        Wire.setSCL(this->pin_settings.pin_scl_0);
        Wire.begin();
        Wire.setClock(this->pin_settings.i2c_clk);
    }

    /**
    * @name   device_setup
    * @brief  Setup the number of devices in the device matrix and 
    *         configure the selected GPIO pins for a given device
    * @param  device        -> Device type selection (IQS9320, IQS7220A, IQS7320A)
    * @param  num_columns   -> Number of column in the device matrix
    * @param  num_rows      -> Number of rows in the device matrix
    * @retval None
    */
    void KeyboardInterface::device_setup(device_e device, uint8_t num_columns, uint8_t num_rows)
    {
        this->device        = device;
        this->num_columns   = num_columns;
        this->num_rows      = num_rows;

        // Configure the GPIO pins for given device
        switch (device)
        {
        case device_e::dev_iqs7220a:
            this->iqs7220a_gpio_setup();
            break;
        
        case device_e::dev_iqs7320a:
            this->iqs7320a_gpio_setup();
            break;

        case device_e::dev_iqs9320_ks:
            this->iqs9320_gpio_setup();
            break;
        
        default:
            break;
        }

        // Set the flag to enable other commands to execute
        this->setup_complete = true;
    }

    /**
    * @name   get_device_row
    * @brief  Returns the row of the device index that is given
    * @param  device_select -> The index of the device
    * @retval Returns the row number of the device given as parameter
    */
    uint8_t KeyboardInterface::get_device_row(uint8_t device_select)
    {
        return device_select%this->num_rows;
    }

    /**
    * @name   get_device_column
    * @brief  Returns the column of the device index that is given
    * @param  device_select -> The index of the device
    * @retval Returns the column number of the device given as parameter
    */
    uint8_t KeyboardInterface::get_device_column(uint8_t device_select)
    {
        return device_select > 0 ? (uint8_t)(device_select/this->num_rows) : 0;
    }

    /**
    * @name   do_comms
    * @brief  Only function required in main loop of the application.
    *         Will first read the serial buffer to verify if any serial communications
    *         need to be processed. If the serial buffer is empty the byte array containing
    *         serial data will be analyzed to verify if a valid packet has been received.
    *         If a valid packet has been received the device will execute the given command.
    *         If a valid packet has not been received, or if no packet data is available,
    *         the device will attempt to execute a streaming function that will periodically
    *         send data over serial. Multiple streaming configurations can be used to stream 
    *         data from the 3 supported devices.
    * @param  None
    * @retval None
    */
    void KeyboardInterface::do_comms()
    {
        // Only execute code when not receiving serial communication
        if (! this->read_serial())
        {
            // If a serial packet was received execute the instruction
            if (this->test_for_packet())
            {
                this->do_command();
            }

            // If no serial packet was received then stream data
            else
            {
                // Do not stream data when device setup has not been completed
                if (!this->setup_complete) return;

                // Only consider streaming when no serial bytes (partial packets) have been received
                if (serial_input_index == 0)
                {
                    // Return if not enough milliseconds have passed since previous sample
                    if (millis() - this->stream_control.timestamp < this->stream_control.sample_interval) return;
                    this->stream_control.timestamp = millis();

                    switch (this->stream_control.state)
                    {
                        case stream_disabled:
                            break;

                        case stream_iqs7220a_ks:
                            this->iqs7220a_scan_keys_all();
                            break;

                        case stream_iqs7220a_i2c:
                            // Stream from all devices in matrix
                            if (this->stream_control.device_select == 0xFF)
                            {
                                for (uint8_t i = 0; i < this->stream_control.num_registers; i++)
                                {
                                    this->i2c_control.register_addr_lsb = this->stream_control.addr[i];
                                    this->i2c_control.data_len = this->stream_control.len[i];
                                    this->iqs7220a_i2c_read_multi();
                                }
                            }
                            else
                            // Stream from specific device only
                            {
                                this->i2c_control.device_select = this->stream_control.device_select;
                                for (uint8_t i = 0; i < this->stream_control.num_registers; i++)
                                {
                                    this->i2c_control.register_addr_lsb = this->stream_control.addr[i];
                                    this->i2c_control.data_len = this->stream_control.len[i];
                                    this->iqs7220a_i2c_read_single();
                                }
                            }
                            break;

                        case stream_iqs7320a_ks:
                            this->iqs7320a_scan_keys_all();
                            break;

                        case stream_iqs7320a_i2c:
                            // Stream from all devices in matrix
                            if (this->stream_control.device_select == 0xFF)
                            {
                                for (uint8_t i = 0; i < this->stream_control.num_registers; i++)
                                {
                                    this->i2c_control.register_addr_lsb = this->stream_control.addr[i];
                                    this->i2c_control.data_len = this->stream_control.len[i];
                                    this->iqs7320a_i2c_read_multi();
                                }
                            }
                            else
                            // Stream from specific device only
                            {
                                this->i2c_control.device_select = this->stream_control.device_select;
                                for (uint8_t i = 0; i < this->stream_control.num_registers; i++)
                                {
                                    this->i2c_control.register_addr_lsb = this->stream_control.addr[i];
                                    this->i2c_control.data_len = this->stream_control.len[i];
                                    this->iqs7320a_i2c_read_single();
                                }
                            }
                            break;

                        case stream_iqs9320_i2c:
                            for (uint8_t i = 0; i < this->stream_control.num_registers; i++)
                            {
                                for (uint8_t j = 0; j < this->stream_control.num_devices; j++)
                                {
                                    this->i2c_control.device_addr = this->stream_control.device_addr[j];
                                    this->i2c_control.register_addr_lsb = this->stream_control.addr[(2*i)];
                                    this->i2c_control.register_addr_msb = this->stream_control.addr[(2*i)+1];
                                    this->i2c_control.data_len = this->stream_control.len[i];
                                    this->iqs9320_i2c_read_fp();
                                }
                            }
                            break;

                        case stream_iqs9320_ks:
                            this->iqs9320_scan_keys_all(this->stream_control.num_channels);
                            break;

                        case stream_iqs9320_ks_i2c:
                            // Stream from all devices in device matrix
                            if (this->stream_control.device_select == 0xFF)
                            {
                                for (uint8_t i = 0; i < this->stream_control.num_registers; i++)
                                {
                                    this->i2c_control.register_addr_lsb = this->stream_control.addr[(2*i)];
                                    this->i2c_control.register_addr_msb = this->stream_control.addr[(2*i)+1];
                                    this->i2c_control.data_len = this->stream_control.len[i];
                                    for (uint8_t j = 0; j < num_columns*num_rows; j++)
                                    {
                                        this->i2c_control.device_select = j;
                                        this->iqs9320_i2c_read_ks();
                                    }
                                }
                            }
                            else
                            // Stream from specific device only
                            {
                                this->i2c_control.device_select = this->stream_control.device_select;
                                for (uint8_t i = 0; i < this->stream_control.num_registers; i++)
                                {
                                    this->i2c_control.register_addr_lsb = this->stream_control.addr[(2*i)];
                                    this->i2c_control.register_addr_msb = this->stream_control.addr[(2*i)+1];
                                    this->i2c_control.data_len = this->stream_control.len[i];
                                    this->iqs9320_i2c_read_ks();
                                }
                            }
                            break;
                    }
                }
            }
        }
    }

    /**
    * @name   do_command
    * @brief  Execute commands that have been received over serial.
    *         The second byte of the serial_packet_data array contains
    *         the command variable which is passed to a switch statement.
    *         All following bytes are used as command parameters.
    * @param  None
    * @retval None
    */
    void KeyboardInterface::do_command()
    {
        switch(this->serial_packet_data[1])
        {
            // ---------------------------------------------------------
            // General Commands
            // ---------------------------------------------------------
            case cmd_setup:
                this->device_setup((device_e)this->serial_packet_data[2], this->serial_packet_data[3], this->serial_packet_data[4]);
                break;

            case cmd_stop_streaming:
                this->stream_control.state = stream_disabled;
                break;

            case cmd_stop_comms:
                this->serial_comms_state = false;
                break;

            // ---------------------------------------------------------
            // IQS7220A
            // ---------------------------------------------------------
            case cmd_iqs7220a_block_ks:
                if (!this->setup_complete) return;
                this->iqs7220a_scan_keys_all();
                break;

            case cmd_iqs7220a_block_i2c_read_single:
                if (!this->setup_complete) return;
                this->i2c_control.device_select     = this->serial_packet_data[2];
                this->i2c_control.device_addr       = this->serial_packet_data[3];
                this->i2c_control.register_addr_lsb = this->serial_packet_data[4];
                this->i2c_control.data_len          = this->serial_packet_data[5];
                this->iqs7220a_i2c_read_single();
                break;

            case cmd_iqs7220a_block_i2c_write_single:
                if (!this->setup_complete) return;
                this->i2c_control.device_select     = this->serial_packet_data[2];
                this->i2c_control.device_addr       = this->serial_packet_data[3];
                this->i2c_control.register_addr_lsb = this->serial_packet_data[4];
                this->i2c_control.data_len          = this->serial_packet_data[5];
                memcpy(this->i2c_control.output_data, &(this->serial_packet_data[6]), this->i2c_control.data_len);
                this->iqs7220a_i2c_write_single();
                Serial.write(return_arr, 4);
                break;

            case cmd_iqs7220a_block_i2c_read_multi:
                if (!this->setup_complete) return;
                this->i2c_control.device_addr       = this->serial_packet_data[2];
                this->i2c_control.register_addr_lsb = this->serial_packet_data[3];
                this->i2c_control.data_len          = this->serial_packet_data[4];
                this->iqs7320a_i2c_read_multi();
                break;

            case cmd_iqs7220a_block_i2c_write_multi:
                if (!this->setup_complete) return;
                this->i2c_control.device_addr       = this->serial_packet_data[2];
                this->i2c_control.register_addr_lsb = this->serial_packet_data[3];
                this->i2c_control.data_len          = this->serial_packet_data[4];
                memcpy(this->i2c_control.output_data, &(this->serial_packet_data[5]), this->i2c_control.data_len);
                this->iqs7220a_i2c_write_multi();
                Serial.write(return_arr, 4);
                break;
            
            case cmd_iqs7220a_stream_ks:
                if (!this->setup_complete) return;
                this->stream_control.sample_interval    = this->serial_packet_data[2];
                this->stream_control.timestamp          = millis();
                this->stream_control.state              = stream_iqs7220a_ks;
                Serial.write(return_arr, 4);
                break;

            case cmd_iqs7220a_stream_i2c_read_single:
                if (!this->setup_complete) return;
                this->stream_control.sample_interval    = this->serial_packet_data[2];
                this->stream_control.device_select      = this->serial_packet_data[3];
                this->i2c_control.device_addr           = this->serial_packet_data[4];
                this->stream_control.num_registers      = this->serial_packet_data[5];
                memcpy(this->stream_control.addr, &(this->serial_packet_data[6]), this->stream_control.num_registers);
                memcpy(this->stream_control.len, &(this->serial_packet_data[6 + this->stream_control.num_registers]), this->stream_control.num_registers);
                this->stream_control.timestamp          = millis();
                this->stream_control.state              = stream_iqs7220a_i2c;
                Serial.write(return_arr, 4);
                break;

            case cmd_iqs7220a_stream_i2c_read_multi:
                if (!this->setup_complete) return;
                this->stream_control.sample_interval    = this->serial_packet_data[2];
                this->stream_control.device_select      = 0xFF;
                this->i2c_control.device_addr           = this->serial_packet_data[3];
                this->stream_control.num_registers      = this->serial_packet_data[4];
                memcpy(this->stream_control.addr, &(this->serial_packet_data[5]), this->stream_control.num_registers);
                memcpy(this->stream_control.len, &(this->serial_packet_data[5 + this->stream_control.num_registers]), this->stream_control.num_registers);
                this->stream_control.timestamp          = millis();
                this->stream_control.state              = stream_iqs7220a_i2c;
                Serial.write(return_arr, 4);
                break;

            // ---------------------------------------------------------
            // IQS7320A
            // ---------------------------------------------------------
            case cmd_iqs7320a_block_ks:
                if (!this->setup_complete) return;
                this->iqs7220a_scan_keys_all();
                break;

            case cmd_iqs7320a_block_i2c_read_single:
                if (!this->setup_complete) return;
                this->i2c_control.device_select     = this->serial_packet_data[2];
                this->i2c_control.device_addr       = this->serial_packet_data[3];
                this->i2c_control.register_addr_lsb = this->serial_packet_data[4];
                this->i2c_control.data_len          = this->serial_packet_data[5];
                this->iqs7220a_i2c_read_single();
                break;

            case cmd_iqs7320a_block_i2c_write_single:
                if (!this->setup_complete) return;
                this->i2c_control.device_select     = this->serial_packet_data[2];
                this->i2c_control.device_addr       = this->serial_packet_data[3];
                this->i2c_control.register_addr_lsb = this->serial_packet_data[4];
                this->i2c_control.data_len          = this->serial_packet_data[5];
                memcpy(this->i2c_control.output_data, &(this->serial_packet_data[6]), this->i2c_control.data_len);
                this->iqs7220a_i2c_write_single();
                Serial.write(return_arr, 4);
                break;

            case cmd_iqs7320a_block_i2c_read_multi:
                if (!this->setup_complete) return;
                this->i2c_control.device_addr       = this->serial_packet_data[2];
                this->i2c_control.register_addr_lsb = this->serial_packet_data[3];
                this->i2c_control.data_len          = this->serial_packet_data[4];
                this->iqs7320a_i2c_read_multi();
                break;

            case cmd_iqs7320a_block_i2c_write_multi:
                if (!this->setup_complete) return;
                this->i2c_control.device_addr       = this->serial_packet_data[2];
                this->i2c_control.register_addr_lsb = this->serial_packet_data[3];
                this->i2c_control.data_len          = this->serial_packet_data[4];
                memcpy(this->i2c_control.output_data, &(this->serial_packet_data[5]), this->i2c_control.data_len);
                this->iqs7220a_i2c_write_multi();
                Serial.write(return_arr, 4);
                break;

            case cmd_iqs7320a_block_autonomous:
                if (!this->setup_complete) return;
                if (this->serial_packet_data[2] == 1)
                {
                    iqs7320a_autonomous_exit();
                }
                else if (this->serial_packet_data[2] == 2)
                {
                    iqs7320a_autonomous_enter();
                }
                Serial.write(return_arr, 4);
                break;

            case cmd_iqs7320a_block_standby:
                if (!this->setup_complete) return;
                if (this->serial_packet_data[2] == 1)
                {
                    iqs7320a_standby_exit();
                }
                else if (this->serial_packet_data[2] == 2)
                {
                    iqs7320a_standby_enter();
                }
                Serial.write(return_arr, 4);
                break;
            
            case cmd_iqs7320a_stream_ks:
                if (!this->setup_complete) return;
                this->stream_control.sample_interval    = this->serial_packet_data[2];
                this->stream_control.timestamp          = millis();
                this->stream_control.state              = stream_iqs7220a_ks;
                Serial.write(return_arr, 4);
                break;

            case cmd_iqs7320a_stream_i2c_read_single:
                if (!this->setup_complete) return;
                this->stream_control.sample_interval    = this->serial_packet_data[2];
                this->stream_control.device_select      = this->serial_packet_data[3];
                this->i2c_control.device_addr           = this->serial_packet_data[4];
                this->stream_control.num_registers      = this->serial_packet_data[5];
                memcpy(this->stream_control.addr, &(this->serial_packet_data[6]), this->stream_control.num_registers);
                memcpy(this->stream_control.len, &(this->serial_packet_data[6 + this->stream_control.num_registers]), this->stream_control.num_registers);
                this->stream_control.timestamp          = millis();
                this->stream_control.state              = stream_iqs7220a_i2c;
                Serial.write(return_arr, 4);
                break;

            case cmd_iqs7320a_stream_i2c_read_multi:
                if (!this->setup_complete) return;
                this->stream_control.sample_interval    = this->serial_packet_data[2];
                this->i2c_control.device_addr           = this->serial_packet_data[3];
                this->stream_control.num_registers      = this->serial_packet_data[4];
                this->stream_control.device_select      = 0xFF;
                memcpy(this->stream_control.addr, &(this->serial_packet_data[5]), this->stream_control.num_registers);
                memcpy(this->stream_control.len, &(this->serial_packet_data[5 + this->stream_control.num_registers]), this->stream_control.num_registers);
                this->stream_control.timestamp          = millis();
                this->stream_control.state              = stream_iqs7220a_i2c;
                Serial.write(return_arr, 4);
                break;

            // ---------------------------------------------------------
            // IQS9320 I2C
            // ---------------------------------------------------------
            case cmd_iqs9320_block_i2c_read_single:
                if (!this->setup_complete) return;
                this->i2c_control.device_addr       = this->serial_packet_data[2];
                this->i2c_control.register_addr_lsb = this->serial_packet_data[3];
                this->i2c_control.register_addr_msb = this->serial_packet_data[4];
                this->i2c_control.data_len          = this->serial_packet_data[5];
                this->iqs9320_i2c_read_fp();
                break;

            case cmd_iqs9320_block_i2c_write_single:
                if (!this->setup_complete) return;
                this->i2c_control.device_addr       = this->serial_packet_data[2];
                this->i2c_control.register_addr_lsb = this->serial_packet_data[3];
                this->i2c_control.register_addr_msb = this->serial_packet_data[4];
                this->i2c_control.data_len          = this->serial_packet_data[5];
                memcpy(this->i2c_control.output_data, &(this->serial_packet_data[6]), this->i2c_control.data_len);
                this->iqs9320_i2c_write_fp();
                Serial.write(return_arr, 4);
                break;

            case cmd_iqs9320_block_i2c_read_multi:
                {
                    if (!this->setup_complete) return;
                    uint8_t number_devices = this->serial_packet_data[2];
                    for (uint8_t i = 0; i < number_devices; i++)
                    {
                        this->i2c_control.device_addr       = this->serial_packet_data[3+i];
                        this->i2c_control.register_addr_lsb = this->serial_packet_data[3 + number_devices];
                        this->i2c_control.register_addr_msb = this->serial_packet_data[4 + number_devices];
                        this->i2c_control.data_len          = this->serial_packet_data[5 + number_devices];
                        this->iqs9320_i2c_read_fp();
                    }
                    break;
                }

            case cmd_iqs9320_block_i2c_write_multi:
                {
                    if (!this->setup_complete) return;
                    uint8_t number_devices = this->serial_packet_data[2];
                    for (uint8_t i = 0; i < number_devices; i++)
                    {
                        this->i2c_control.device_addr       = this->serial_packet_data[3+i];
                        this->i2c_control.register_addr_lsb = this->serial_packet_data[3 + number_devices];
                        this->i2c_control.register_addr_msb = this->serial_packet_data[4 + number_devices];
                        this->i2c_control.data_len          = this->serial_packet_data[5 + number_devices];
                        memcpy(this->i2c_control.output_data, &(this->serial_packet_data[6 + number_devices]), this->i2c_control.data_len);
                        this->iqs9320_i2c_write_fp();
                    }
                    Serial.write(return_arr, 4);
                    break;
                }

            case cmd_iqs9320_stream_i2c_read_single:
                if (!this->setup_complete) return;
                this->stream_control.num_devices        = 1;
                this->stream_control.sample_interval    = this->serial_packet_data[2];
                this->stream_control.device_addr[0]     = this->serial_packet_data[3];
                this->stream_control.num_registers      = this->serial_packet_data[4];
                memcpy(this->stream_control.addr, &(this->serial_packet_data[5]), this->stream_control.num_registers*2);
                memcpy(this->stream_control.len, &(this->serial_packet_data[5 + this->stream_control.num_registers*2]), this->stream_control.num_registers);
                this->stream_control.timestamp          = millis();
                this->stream_control.state              = stream_iqs9320_i2c;
                Serial.write(return_arr, 4);
                break;

            case cmd_iqs9320_stream_i2c_read_multi:
                if (!this->setup_complete) return;
                this->stream_control.sample_interval    = this->serial_packet_data[2];
                this->stream_control.num_devices        = this->serial_packet_data[3];
                memcpy(this->stream_control.device_addr, &(this->serial_packet_data[4]), this->stream_control.num_devices);
                this->stream_control.num_registers      = this->serial_packet_data[4 + this->stream_control.num_devices];
                memcpy(this->stream_control.addr, &(this->serial_packet_data[5 + this->stream_control.num_devices]), this->stream_control.num_registers*2);
                memcpy(this->stream_control.len, &(this->serial_packet_data[5 + this->stream_control.num_devices + this->stream_control.num_registers*2]), this->stream_control.num_registers);
                this->stream_control.timestamp          = millis();
                this->stream_control.state              = stream_iqs9320_i2c;
                Serial.write(return_arr, 4);
                break;

            // ---------------------------------------------------------
            // IQS9320 Key Scan
            // ---------------------------------------------------------
            case cmd_iqs9320_block_ks:
                if (!this->setup_complete) return;
                this->iqs9320_scan_keys_all(this->serial_packet_data[2]);
                break;

            case cmd_iqs9320_block_ks_i2c_read_single:
                if (!this->setup_complete) return;
                this->i2c_control.device_select     = this->serial_packet_data[2];
                this->i2c_control.device_addr       = this->serial_packet_data[3];
                this->i2c_control.register_addr_lsb = this->serial_packet_data[4];
                this->i2c_control.register_addr_msb = this->serial_packet_data[5];
                this->i2c_control.data_len          = this->serial_packet_data[6];
                this->iqs9320_i2c_read_ks();
                break;

            case cmd_iqs9320_block_ks_i2c_write_single:
                if (!this->setup_complete) return;
                this->i2c_control.device_select     = this->serial_packet_data[2];
                this->i2c_control.device_addr       = this->serial_packet_data[3];
                this->i2c_control.register_addr_lsb = this->serial_packet_data[4];
                this->i2c_control.register_addr_msb = this->serial_packet_data[5];
                this->i2c_control.data_len          = this->serial_packet_data[6];
                memcpy(this->i2c_control.output_data, &(this->serial_packet_data[7]), this->i2c_control.data_len);
                this->iqs9320_i2c_write_ks();
                Serial.write(return_arr, 4);
                break;

            case cmd_iqs9320_block_ks_i2c_read_multi:
                if (!this->setup_complete) return;
                this->i2c_control.device_addr       = this->serial_packet_data[2];
                this->i2c_control.register_addr_lsb = this->serial_packet_data[3];
                this->i2c_control.register_addr_msb = this->serial_packet_data[4];
                this->i2c_control.data_len          = this->serial_packet_data[5];
                for (uint8_t i = 0; i < this->num_columns*this->num_rows; i++)
                {
                    this->i2c_control.device_select = i;
                    this->iqs9320_i2c_read_ks();
                }
                break;

            case cmd_iqs9320_block_ks_i2c_write_multi:
                if (!this->setup_complete) return;
                this->i2c_control.device_addr       = this->serial_packet_data[2];
                this->i2c_control.register_addr_lsb = this->serial_packet_data[3];
                this->i2c_control.register_addr_msb = this->serial_packet_data[4];
                this->i2c_control.data_len          = this->serial_packet_data[5];
                memcpy(this->i2c_control.output_data, &(this->serial_packet_data[6]), this->i2c_control.data_len);
                for (uint8_t i = 0; i < this->num_columns*this->num_rows; i++)
                {
                    this->i2c_control.device_select = i;
                    this->iqs9320_i2c_write_ks();
                }
                Serial.write(return_arr, 4);
                break;

            case cmd_iqs9320_block_ks_standby:
                if (!this->setup_complete) return;
                if (this->serial_packet_data[2] == 1)
                {
                    this->iqs9320_standby_exit();
                }
                else if (this->serial_packet_data[2] == 2)
                {
                    this->iqs9320_standby_enter();
                }
                Serial.write(return_arr, 4);
                break;

            case cmd_iqs9320_stream_ks:
                if (!this->setup_complete) return;
                this->stream_control.sample_interval    = this->serial_packet_data[2];
                this->stream_control.timestamp          = millis();
                this->stream_control.num_channels       = this->serial_packet_data[3];
                this->stream_control.state              = stream_iqs9320_ks;
                Serial.write(return_arr, 4);
                break;

            case cmd_iqs9320_stream_ks_i2c_read_single:
                if (!this->setup_complete) return;
                this->stream_control.sample_interval    = this->serial_packet_data[2];
                this->stream_control.device_select      = this->serial_packet_data[3];
                this->i2c_control.device_addr           = this->serial_packet_data[4];
                this->stream_control.num_registers      = this->serial_packet_data[5];
                memcpy(this->stream_control.addr, &(this->serial_packet_data[6]), this->stream_control.num_registers*2);
                memcpy(this->stream_control.len, &(this->serial_packet_data[6 + this->stream_control.num_registers*2]), this->stream_control.num_registers);
                this->stream_control.timestamp          = millis();
                this->stream_control.state              = stream_iqs9320_ks_i2c;
                Serial.write(return_arr, 4);
                break;

            case cmd_iqs9320_stream_ks_i2c_read_multi:
                if (!this->setup_complete) return;
                this->stream_control.device_select      = 0xFF;
                this->stream_control.sample_interval    = this->serial_packet_data[2];
                this->i2c_control.device_addr           = this->serial_packet_data[3];
                this->stream_control.num_registers      = this->serial_packet_data[4];
                memcpy(this->stream_control.addr, &(this->serial_packet_data[5]), this->stream_control.num_registers*2);
                memcpy(this->stream_control.len, &(this->serial_packet_data[5 + this->stream_control.num_registers*2]), this->stream_control.num_registers);
                this->stream_control.timestamp          = millis();
                this->stream_control.state              = stream_iqs9320_ks_i2c;
                Serial.write(return_arr, 4);
                break;
        }
    }
}