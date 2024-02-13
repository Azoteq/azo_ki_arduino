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
 * @file        azo_ki.ino                                                    *
 * @brief       Main Arduino file for the Azoteq Keyboard Interface           *
 * @author      Hennie van der Westhuizen - Azoteq (Pty) Ltd                  *
 * @version     v0.0.2                                                        *
 * @date        2023                                                          *
 *****************************************************************************/
#include "azo_ki.hpp"

AZO_KEYBOARD_INTERFACE::KeyboardInterface kb_obj = AZO_KEYBOARD_INTERFACE::KeyboardInterface();

void setup()
{
    // Setup serial and I2C communications to receive instructions
    kb_obj.comms_setup();
}

void loop()
{
    // Check if any serial communications have been received
    // Respond to serial communications if any have been received
    // Sample and communicate data if streaming has been configured
    kb_obj.do_comms();
}