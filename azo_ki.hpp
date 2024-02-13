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
 * @file        azo_ki.hpp                                                    *
 * @brief       Header file for entire Azoteq Keyboard Interface project.     *
 *              Contains KeyboardInterface class declaration, global          *
 *              definitions, data structure declarations.                     *
 * @author      Hennie van der Westhuizen - Azoteq (Pty) Ltd                  *
 * @version     v0.0.2                                                        *
 * @date        2023                                                          *
 *****************************************************************************/
#pragma once

#include "Arduino.h"
#include "Wire.h"

// Serial
#define SERIAL_HEADER_A             0xCC
#define SERIAL_HEADER_B             0xEF
#define PACKET_LEN                  128
#define MAX_STREAM                  20
#define SCAN_DELAY                  20
#define AZQ700_KS_OUTPUT_PARAMS     5
#define AZQ701_KS_OUTPUT_PARAMS     22

// GPIO hardware control registers
extern uint32_t* gpio_input;
extern uint32_t* gpio_output;
extern uint32_t* gpio_output_set;
extern uint32_t* gpio_output_clear;
extern uint32_t* gpio_output_enable;
extern uint32_t* gpio_output_enable_set;
extern uint32_t* gpio_output_enable_clear;

namespace AZO_KEYBOARD_INTERFACE
{
    enum commands_e
    {
        // General Commands
        cmd_setup                               = 0x00,
        cmd_stop_streaming                      = 0x01,
        cmd_stop_comms                          = 0x02,

        // IQS7220A Commands
        cmd_iqs7220a_block_ks                   = 0x10,
        cmd_iqs7220a_block_i2c_read_single      = 0x11,
        cmd_iqs7220a_block_i2c_write_single     = 0x12,
        cmd_iqs7220a_block_i2c_read_multi       = 0x13,
        cmd_iqs7220a_block_i2c_write_multi      = 0x14,
        cmd_iqs7220a_stream_ks                  = 0x15,
        cmd_iqs7220a_stream_i2c_read_single     = 0x16,
        cmd_iqs7220a_stream_i2c_read_multi      = 0x17,

        // IQS7320A Commands
        cmd_iqs7320a_block_ks                   = 0x20,
        cmd_iqs7320a_block_i2c_read_single      = 0x21,
        cmd_iqs7320a_block_i2c_write_single     = 0x22,
        cmd_iqs7320a_block_i2c_read_multi       = 0x23,
        cmd_iqs7320a_block_i2c_write_multi      = 0x24,
        cmd_iqs7320a_block_autonomous           = 0x25,
        cmd_iqs7320a_block_standby              = 0x26,
        cmd_iqs7320a_stream_ks                  = 0x27,
        cmd_iqs7320a_stream_i2c_read_single     = 0x28,
        cmd_iqs7320a_stream_i2c_read_multi      = 0x29,

        // IQS9320 - I2C Interface
        cmd_iqs9320_block_i2c_read_single       = 0x30,
        cmd_iqs9320_block_i2c_write_single      = 0x31,
        cmd_iqs9320_block_i2c_read_multi        = 0x32,
        cmd_iqs9320_block_i2c_write_multi       = 0x33,
        cmd_iqs9320_stream_i2c_read_single      = 0x34,
        cmd_iqs9320_stream_i2c_read_multi       = 0x35,

        // IQS9320 - Key Scan Interface
        cmd_iqs9320_block_ks                    = 0x40,
        cmd_iqs9320_block_ks_i2c_read_single    = 0x41,
        cmd_iqs9320_block_ks_i2c_write_single   = 0x42,
        cmd_iqs9320_block_ks_i2c_read_multi     = 0x43,
        cmd_iqs9320_block_ks_i2c_write_multi    = 0x44,
        cmd_iqs9320_block_ks_standby            = 0x45,
        cmd_iqs9320_stream_ks                   = 0x46,
        cmd_iqs9320_stream_ks_i2c_read_single   = 0x47,
        cmd_iqs9320_stream_ks_i2c_read_multi    = 0x48
    };

    struct pin_settings_t
    {
        uint32_t serial_baud_rate;
        uint32_t i2c_clk;
        uint8_t pin_sda_0;
        uint8_t pin_scl_0;
        uint32_t *c0_msk;
        uint32_t *r0_msk;
        uint32_t *r1_msk;
        uint32_t *r2_msk;
        uint32_t *r3_msk;
        uint32_t *s0_msk;
        uint32_t *s1_msk;
        uint32_t *d0_msk;
        uint32_t *d1_msk;
        uint32_t c0_all;
        uint32_t r0_all;
        uint32_t r1_all;
        uint32_t r2_all;
        uint32_t r3_all;
        uint32_t s0_all;
        uint32_t s1_all;
        uint32_t d0_all;
        uint32_t d1_all;
    };

    struct stream_control_t
    {
        uint8_t     state;
        uint8_t     device_addr[20];
        uint8_t     device_select;  // for KS only
        uint8_t     num_devices;    // for FP only
        uint8_t     num_registers;
        uint8_t     addr[20];
        uint8_t     len[20];
        uint8_t     sample_interval;
        uint32_t    timestamp;
        uint8_t     num_channels; // for 701 KS only
    };

    struct i2c_control_t
    {
        uint8_t  device_select;
        uint8_t  device_addr;
        uint8_t  register_addr_lsb;
        uint8_t  register_addr_msb;
        uint8_t  data_len;
        uint8_t  input_data[PACKET_LEN];
        uint8_t  input_index;
        uint8_t  output_data[PACKET_LEN];
        uint8_t  output_index;
    };

    enum device_e
    {
        dev_iqs7220a    = 0,
        dev_iqs7320a    = 1,
        dev_iqs9320_i2c = 2,
        dev_iqs9320_ks  = 3
    };

    enum stream_states_e
    {
        stream_disabled         = 0x00,
        stream_iqs7220a_ks      = 0x10,
        stream_iqs7220a_i2c     = 0x11,
        stream_iqs7320a_ks      = 0x20,
        stream_iqs7320a_i2c     = 0x21,
        stream_iqs9320_i2c      = 0x30,
        stream_iqs9320_ks       = 0x31,
        stream_iqs9320_ks_i2c   = 0x32

    };

    extern pin_settings_t default_pin_settings;
    extern uint8_t serial_data_byte;

    class KeyboardInterface
    {
        private:
            pin_settings_t      pin_settings;
            stream_control_t    stream_control;
            i2c_control_t       i2c_control;
            bool                setup_complete;
            uint8_t             device;
            uint8_t             num_columns;
            uint8_t             num_rows;
            

            // Serial
            uint8_t serial_packet_data[PACKET_LEN];
            uint8_t serial_output_data[PACKET_LEN+6];
            uint8_t serial_input_data[PACKET_LEN+6];
            uint8_t serial_input_index;
            uint8_t serial_output_index;
            uint8_t serial_packet_index;
            uint8_t serial_packet_len;

        public:
            // Constructors
            KeyboardInterface();
            KeyboardInterface(pin_settings_t pin_settings);

            // Main
            bool                serial_comms_state;
            void                comms_setup();
            void                device_setup(device_e platform, uint8_t num_columns, uint8_t num_rows);
            uint8_t             get_device_row(uint8_t device_select);
            uint8_t             get_device_column(uint8_t device_select);
            void                do_comms();
            void                do_command();

            // Serial
            bool                read_serial();
            bool                test_for_packet();
            void                send_packet_response();
            uint16_t            get_crc(uint8_t data[], uint8_t data_len);


            // IQS7220A
            bool iqs7220a_key_scan_results[6][6][5];
            void iqs7220a_gpio_setup();
            void iqs7220a_scan_keys_column(uint8_t column_select);
            void iqs7220a_scan_keys_all();
            void iqs7220a_config_enter_column(uint8_t column_select);
            void iqs7220a_config_enter_row(uint8_t row_select);
            void iqs7220a_config_exit_row(uint8_t row_select);
            void iqs7220a_i2c_read_single();
            void iqs7220a_i2c_write_single();
            void iqs7220a_i2c_read_multi();
            void iqs7220a_i2c_write_multi();
            
            // IQS7320A
            bool iqs7320a_key_scan_results[6][6][5];
            void iqs7320a_gpio_setup();
            void iqs7320a_scan_keys_column(uint8_t column_select);
            void iqs7320a_scan_keys_all();
            void iqs7320a_config_enter_column(uint8_t column_select);
            void iqs7320a_config_enter_row(uint8_t row_select);
            void iqs7320a_config_exit_row(uint8_t row_select);
            void iqs7320a_autonomous_enter();
            void iqs7320a_autonomous_exit();
            void iqs7320a_standby_enter();
            void iqs7320a_standby_exit();
            void iqs7320a_i2c_read_single();
            void iqs7320a_i2c_write_single();
            void iqs7320a_i2c_read_multi();
            void iqs7320a_i2c_write_multi();

            // IQS9320
            bool iqs9320_key_scan_results[6][6][22];
            void iqs9320_gpio_setup();
            void iqs9320_scan_keys_column(uint8_t column_select, uint8_t num_channels);
            void iqs9320_scan_keys_all(uint8_t num_channels);
            void iqs9320_config_enter(uint8_t column_select, uint8_t row_select);
            void iqs9320_config_exit(uint8_t row_select);
            void iqs9320_standby_enter();
            void iqs9320_standby_exit();
            void iqs9320_i2c_read_fp();
            void iqs9320_i2c_write_fp();
            void iqs9320_i2c_read_ks();
            void iqs9320_i2c_write_ks();
    };
}
