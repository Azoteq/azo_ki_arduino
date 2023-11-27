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
 * @brief       Default pin definitions                                       *
 * @author      Hennie van der Westhuizen - Azoteq (Pty) Ltd                  *
 * @version     v0.0.1                                                        *
 * @date        2023                                                          *
 *****************************************************************************/
#include "azo_ki.hpp"

namespace AZO_KEYBOARD_INTERFACE
{
    uint32_t default_r0_msk[] = {
        0x1,        // GPIO 0
        0x10,       // GPIO 4
        0x100,      // GPIO 8
        0x1000,     // GPIO 12
    };

    uint32_t default_r1_msk[] = {
        0x2,        // GPIO 1
        0x20,       // GPIO 5
        0x200,      // GPIO 9
        0x2000,     // GPIO 13
    };

    uint32_t default_r2_msk[] = {
        0x4,        // GPIO 2
        0x40,       // GPIO 6
        0x400,      // GPIO 10
        0x4000,     // GPIO 14
    };

    uint32_t default_r3_msk[] = {
        0x8,        // GPIO 3
        0x80,       // GPIO 7
        0x800,      // GPIO 11
        0x8000,     // GPIO 15
    };

    uint32_t default_c0_msk[] = {
        0x10000,    // GPIO 16
        0x20000,    // GPIO 17
        0x400000,   // GPIO 22
        0x4000000,  // GPIO 26
        0x8000000   // GPIO 27
    };

    uint32_t default_d0_msk[] = {
        0x1,        // GPIO 0
        0x4,        // GPIO 2
        0x10,       // GPIO 4
        0x40,       // GPIO 6
        0x100,      // GPIO 8
        0x400       // GPIO 10
    };

    uint32_t default_d1_msk[] = {
        0x2,        // GPIO 1
        0x8,        // GPIO 3
        0x20,       // GPIO 5
        0x80,       // GPIO 7
        0x200,      // GPIO 9
        0x800       // GPIO 11
    };

    uint32_t default_s0_msk[] = {
        0x1000,     // GPIO 12
        0x4000,     // GPIO 14
        0x10000,    // GPIO 16
        0x400000,   // GPIO 22
    };

    uint32_t default_s1_msk[] = {
        0x2000,     // GPIO 13
        0x8000,     // GPIO 15
        0x20000,    // GPIO 17
        0x4000000,  // GPIO 26
    };

    pin_settings_t default_pin_settings = {
        .serial_baud_rate   = 115200,
        .i2c_clk            = 1000000,
        .pin_sda_0          = 20,
        .pin_scl_0          = 21,
        .pin_sda_1          = 18,
        .pin_scl_1          = 19,
        .c0_msk             = default_c0_msk,
        .r0_msk             = default_r0_msk,
        .r1_msk             = default_r1_msk,
        .r2_msk             = default_r2_msk,
        .r3_msk             = default_r3_msk,
        .s0_msk             = default_s0_msk,
        .s1_msk             = default_s1_msk,
        .d0_msk             = default_d0_msk,
        .d1_msk             = default_d1_msk
    };
}