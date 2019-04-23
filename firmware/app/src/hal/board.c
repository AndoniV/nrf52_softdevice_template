/***************************************************************************//**
 @copyright © Glue Home Ltd, 2018. All rights reserved
 @author    A Villarreal
 @date      13/03/19
 @file      board.c
 @brief     Provides generic board functions
 ******************************************************************************/

#include "nrf_log.h"
#include "nrf_gpio.h"
#include "hal/board.h"

// DEFINITIONS ****************************************************************/

// DECLARATIONS ***************************************************************/

/** Whether the board is initialised */
bool s_board_initialised = false;

// IMPLEMENTATION *************************************************************/

/**
 * Initialise the board pins
 */
AppResult board_init()
{
	AppResult result = ERR_ERROR;

	if (!s_board_initialised)
	{
		// Initialise device-common GPIO
		nrf_gpio_cfg_output(BOARD_LED_1_PIN);
		nrf_gpio_cfg_output(BOARD_LED_2_PIN);
		nrf_gpio_cfg_output(BOARD_LED_3_PIN);
		nrf_gpio_cfg_output(BOARD_LED_4_PIN);

		nrf_gpio_pin_set(BOARD_LED_1_PIN);
		nrf_gpio_pin_set(BOARD_LED_2_PIN);
		nrf_gpio_pin_set(BOARD_LED_3_PIN);
		nrf_gpio_pin_set(BOARD_LED_4_PIN);

		NRF_LOG_INFO("[BOARD] Initialised")
		s_board_initialised = true;
		result = ERR_SUCCESS;
	}

	return result;
}
