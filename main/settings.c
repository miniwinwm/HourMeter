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

/***************
*** INCLUDES ***
***************/

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "settings.h"
#include "flash.h"

/**************
*** DEFINES ***
**************/

#define WAIT_FOREVER       									portMAX_DELAY  			///< Redefinition of FreeRTOS wait forever definition
#define SIGNATURE											0xDEEDBEEFUL			///< Settings signature in flash to determine settings have been written previously
#define SETTINGS_DEFAULT_CAN_DEVICE_ADDRESS					22U						///< Default NMEA2000 network address 

/************
*** TYPES ***
************/

/**
 * Settings data structure in flash
 */
typedef struct 
{
    uint32_t signature;																///< Signature to determine if settings have been written previously
    uint8_t device_address;															///< NMEA2000 device address
} settings_non_volatile_t;

/********************************
*** LOCAL FUNCTION PROTOTYPES ***
********************************/

/**********************
*** LOCAL VARIABLES ***
**********************/

static settings_non_volatile_t settings_non_volatile;								///< Memory copy of non-volatile settings read from flash
static SemaphoreHandle_t settings_mutex_handle;										///< Mutex handle to ensure settings access thread safety
static bool settings_initialized = false;											///< If the settings driver has been initialised

/***********************
*** GLOBAL VARIABLES ***
***********************/

/****************
*** CONSTANTS ***
****************/

/**********************
*** LOCAL FUNCTIONS ***
**********************/

/***********************
*** GLOBAL FUNCTIONS ***
***********************/

void settings_reset(void)
{
	(void)memset(&settings_non_volatile, 0, sizeof(settings_non_volatile_t));
	settings_non_volatile.signature = SIGNATURE;
	settings_non_volatile.device_address = SETTINGS_DEFAULT_CAN_DEVICE_ADDRESS;
	flash_store_data((const uint8_t *)&settings_non_volatile, sizeof(settings_non_volatile_t));  	
}

void settings_init(void)
{
	if (settings_initialized)
	{
		return;
	}
	
	settings_initialized = true;
	settings_mutex_handle = xSemaphoreCreateMutex();	
	flash_load_data((uint8_t *)&settings_non_volatile, sizeof(settings_non_volatile_t));
    if (settings_non_volatile.signature != SIGNATURE)
    {
		settings_reset();
    }
}

uint8_t settings_get_device_address(void)
{
	return settings_non_volatile.device_address;
}

void settings_set_device_address(uint8_t device_address)
{
	xSemaphoreTake(settings_mutex_handle, WAIT_FOREVER);	
	settings_non_volatile.device_address = device_address;
	xSemaphoreGive(settings_mutex_handle);	
}

void settings_save(void)
{
	xSemaphoreTake(settings_mutex_handle, WAIT_FOREVER);	
	flash_store_data((const uint8_t *)&settings_non_volatile, sizeof(settings_non_volatile_t));	
	xSemaphoreGive(settings_mutex_handle);	
}

