/*

MIT License

Copyright (c) John Blaiklock 2022 BlueBridge

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#ifndef SETTINGS_H
#define SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

/***************
*** INCLUDES ***
***************/

#include <stdint.h>

/**************
*** DEFINES ***
**************/

/************
*** TYPES ***
************/

/*************************
*** EXTERNAL VARIABLES ***
*************************/

/***************************
*** FUNCTIONS PROTOTYPES ***
***************************/

/**
 * Initialize the settings driver. If settings not previosuly saved this sets non-volatile settings to defaults and saves them.
 * If previously saved this loads the non volatile settings. In both cases volatile settings are set to defaults. Call once
 * at startup before using other functions.
 * 
 * @note Subsequent calls are ignored
 */
void settings_init(void);

/**
 * Reset all settings to defaults
 */
void settings_reset(void);

/**
 * Serialize non-volatile settings to flash 
 */
void settings_save(void);

/**
 * Read device address non-volatile setting from memory copy
 *
 * @return The setting's value
 */
uint8_t settings_get_device_address(void);

/**
 * Save device address non-volatile setting in memory copy.
 *
 * @param device_address New value of the setting
 * @note This does not save the new setting in flash memory
 */
void settings_set_device_address(uint8_t device_address);

#ifdef __cplusplus
}
#endif

#endif   