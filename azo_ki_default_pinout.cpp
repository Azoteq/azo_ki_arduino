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
 * @version     v0.0.2                                                        *
 * @date        2023                                                          *
 *****************************************************************************/
#include "azo_ki.hpp"

namespace AZO_KEYBOARD_INTERFACE
{
    uint32_t default_r0_msk[] = {
        1 << 18,
        1 << 14,
        1 << 7,
        1 << 28
    };

    uint32_t default_r1_msk[] = {
        1 << 17,
        1 << 13,
        1 << 8,
        1 << 27
    };

    uint32_t default_r2_msk[] = {
        1 << 16,
        1 << 12,
        1 << 9,
        1 << 26,
    };

    uint32_t default_r3_msk[] = {
        1 << 3,
        1 << 11,
        1 << 10,
        1 << 22
    };

    uint32_t default_c0_msk[] = {
        1 << 19,
        1 << 15,
        1 << 6,
        1 << 1
    };

    uint32_t default_d0_msk[] = {
        1 << 16,
        1 << 13,
        1 << 8,
        1 << 10,
        1 << 27,
        1 << 22
    };

    uint32_t default_d1_msk[] = {
        1 << 3,
        1 << 12,
        1 << 9,
        1 << 11,
        1 << 26,
        1 << 21
    };

    uint32_t default_s0_msk[] = {
        1 << 18,
        1 << 15,
        1 << 6,
        1 << 1
    };

    uint32_t default_s1_msk[] = {
        1 << 17,
        1 << 14,
        1 << 7,
        1 << 28
    };

    pin_settings_t default_pin_settings = {
        .serial_baud_rate   = 115200,
        .i2c_clk            = 1000000,
        .pin_sda_0          = 4,
        .pin_scl_0          = 5,
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