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
 * @file        azo_ki_serial.cpp                                             *
 * @brief       Handling of Rx serial communication                           *
 * @author      Hennie van der Westhuizen - Azoteq (Pty) Ltd                  *
 * @version     v0.0.2                                                        *
 * @date        2023                                                          *
 *****************************************************************************/
#include "azo_ki.hpp"

namespace AZO_KEYBOARD_INTERFACE
{
    uint8_t     serial_data_byte;
    bool        header_a_received, header_b_received;

    /**
    * @name   read_serial
    * @brief  Verify that serial data is available and place the serial data
    *         in byte arrays after parsing packet headers.
    * @param  None
    * @retval Returns a boolean value to indicate if serial data is available 
    *         in the serial buffer.
    */
    bool KeyboardInterface::read_serial()
    {
        if (Serial.available())
        {
            serial_data_byte = (uint8_t)Serial.read();

            if (header_a_received)
            {
                // Start receiving package if header bytes have been received
                if (header_b_received)
                {
                    this->serial_comms_state = true;
                    this->serial_input_data[this->serial_input_index] = serial_data_byte;
                    this->serial_input_index++;
                }
                // Await header byte B
                else if ((uint8_t)serial_data_byte == (uint8_t)SERIAL_HEADER_B)
                {
                    header_b_received = true;
                }
                // Reset if second byte is not header byte B
                else
                {
                    header_a_received = false;
                    header_b_received = false;
                }   
            }
            // Await header byte A
            else if ((uint8_t)serial_data_byte == (uint8_t)SERIAL_HEADER_A)
            {
                header_a_received = true;
            }
            return true;
        }
        return false;
    }
    
    /**
    * @name   get_crc
    * @brief  Returns the CRC16 value for a given array of byte values.
    * @param  data -> Random byte array
    * @param  data_len -> Length of data parameter
    * @retval Returns a uint16_t containing calculated CRC16 value.
    */
    uint16_t KeyboardInterface::get_crc(uint8_t data[], uint8_t data_len)
    {
        uint16_t crc = 0xFFFF;
        uint8_t j;

        for (uint8_t i = 0; i < data_len; i++)
        {
            crc = crc ^ (((uint16_t)data[i]) << 8);
            j = 0;
            while (j++ < 8)
            {
                if (crc & 0x8000)
                    crc = crc << 1 ^ 0x1021;
                else
                    crc = crc << 1;
            }
        }

        return crc;
    }

    /**
    * @name   test_for_packet
    * @brief  Test if a valid packet has been received after verifying that
    *         no serial data is available. The device will send a serial response
    *         if a valid serial packet has been received.
    * @param  None
    * @retval Returns a boolean to indicate whether a valid packet has been received.
    */
    bool KeyboardInterface::test_for_packet()
    {   
        // Verify that data has been received
        if (this->serial_packet_len > 0)
        {
            // Parse data if number if bytes received is greater that expected (excluding end of package)
            if (this->serial_input_index > (this->serial_packet_len + 3))
            {
                // Verify that package is complete
                if( this->serial_input_data[this->serial_packet_len+4] == (uint8_t)SERIAL_HEADER_B && 
                    this->serial_input_data[this->serial_packet_len+3] == (uint8_t)SERIAL_HEADER_A
                )
                {
                    uint16_t crc_result = this->serial_input_data[this->serial_packet_len+1] + 
                                            (this->serial_input_data[this->serial_packet_len+2] << 8);

                    if (this->get_crc(&(this->serial_input_data[1]), this->serial_packet_len) == crc_result)
                    {
                        // Copy in to packet array
                        memcpy(this->serial_packet_data, &(this->serial_input_data[1]), this->serial_packet_len);

                        // Clear serial input array
                        this->serial_input_index = 0;
                        this->serial_packet_len = 0;
                        header_a_received = false;
                        header_b_received = false;

                        // Send response back to PC
                        this->send_packet_response();

                        return true;
                    }
                }
            }
        }
        // Receive expected number of bytes
        else if (this->serial_input_index > 0)
        {
            this->serial_packet_len = this->serial_input_data[0];
        }
        return false;
    }

    /**
    * @name   send_packet_response
    * @brief  Sends a response over serial to indicate that a valid packet has been received.
    * @param  None
    * @retval None
    */
    void KeyboardInterface::send_packet_response()
    {
        this->serial_output_data[0] = SERIAL_HEADER_A;
        this->serial_output_data[1] = SERIAL_HEADER_B;
        this->serial_output_data[2] = serial_packet_data[0];
        this->serial_output_data[3] = serial_packet_data[1];
        this->serial_output_data[4] = SERIAL_HEADER_A;
        this->serial_output_data[5] = SERIAL_HEADER_B;

        Serial.write(this->serial_output_data, 6);
    }
}