/*

MIT License

Copyright (c) John Blaiklock 2025 HourMeter

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

#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "NMEA2000_CAN.h"  
#include "N2kMessages.h"
#include "esp_log.h"
#include "settings.h"

/**************
*** DEFINES ***
**************/

/************
*** TYPES ***
************/

/********************************
*** LOCAL FUNCTION PROTOTYPES ***
********************************/

static void vTimerCallback8s(TimerHandle_t xTimer);
static void HandleNMEA2000Msg(const tN2kMsg &N2kMsg);
static uint32_t get_time_ms();
static uint32_t get_time_s();

/**********************
*** LOCAL VARIABLES ***
**********************/

static TimerHandle_t xTimers8s;		

/***********************
*** GLOBAL VARIABLES ***
***********************/

/****************
*** CONSTANTS ***
****************/

/**
 * Array of PGN's of NMEA2000 messages that are transmitted 
 */
static const unsigned long n2k_transmit_messages[] = {130310UL, // atmospheric pressure
													  127489UL,	// engine data
													  0UL};
													  
/**
 * Array of PGN's of NMEA2000 messages that are received 
 */													  
static const unsigned long n2k_receive_messages[] = {0};

/**********************
*** LOCAL FUNCTIONS ***
**********************/

/**
 * Callback function when FreeRTOS task fires every 8s
 * 
 * @param xTimer Unused
 */
static void vTimerCallback8s(TimerHandle_t xTimer)
{
	static double engine_hours = 1000.0;
	
	(void)xTimer;
		     
	tN2kMsg N2kMsg;
	SetN2kEngineDynamicParam(N2kMsg, 0U, N2kDoubleNA, N2kDoubleNA, N2kDoubleNA, N2kDoubleNA, N2kDoubleNA, engine_hours * 60.0f * 60.0, N2kDoubleNA, N2kDoubleNA, N2kInt8NA, N2kInt8NA, false);
	NMEA2000.SendMsg(N2kMsg);	
	
	engine_hours += 1.0;
}

/**
 * Handle any incoming NMEA2000 message
 *
 * @param N2kMsg Reference to the incoming message
 */
static void HandleNMEA2000Msg(const tN2kMsg &N2kMsg) 
{	
}

/**
 * Get system up time in milliseconds 
 * 
 * @return Time in milliseconds 
 */
static uint32_t get_time_ms()
{
	return (uint32_t)xTaskGetTickCount() * (1000UL / configTICK_RATE_HZ);
}

/**
 * Get system up time in seconds 
 * 
 * @return Time in seconds 
 */
 uint32_t get_time_s()
{
	return get_time_ms() / 1000UL;
}

/***********************
*** GLOBAL FUNCTIONS ***
***********************/

extern "C" void app_main(void)
{    
    ESP_LOGI(pcTaskGetName(NULL), "Started");

	settings_init();		
	
	(void)get_time_s();		// dummy to prevent unused warning
	
    // set up N2K  
    NMEA2000.SetN2kCANMsgBufSize(16);
    NMEA2000.SetProductInformation("00000001", 1, "HourMeter", "1.0", "HM1.0");		
    NMEA2000.SetDeviceInformation(1, 140, 75, 2040); 
    NMEA2000.SetMode(tNMEA2000::N2km_ListenAndNode, settings_get_device_address());
    NMEA2000.EnableForward(false);      
	NMEA2000.SetN2kCANMsgBufSize(25);
    NMEA2000.ExtendTransmitMessages(n2k_transmit_messages);
	NMEA2000.ExtendReceiveMessages(n2k_receive_messages);
	NMEA2000.SetMsgHandler(HandleNMEA2000Msg);	
    NMEA2000.Open();		
			
	// create 8s timer
	xTimers8s = xTimerCreate("8s timer", (TickType_t)8000, pdTRUE, (void *)0, vTimerCallback8s);				
	(void)xTimerStart(xTimers8s, (TickType_t)0);		
	
	while (true) 
	{ 
        // do N2K routine stuff
		NMEA2000.ParseMessages();
        if (NMEA2000.ReadResetAddressChanged())
        {
			settings_set_device_address(NMEA2000.GetN2kSource());
			settings_save();
        }	
		
		vTaskDelay((TickType_t)10);        		
    }		
}
