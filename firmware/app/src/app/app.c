/*******************************************************************************
 @author    A Villarreal
 @date      18/04/19
 @file      app.c
 @brief     Provides the main application
 ******************************************************************************/

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "app_error.h"
#include "app_timer.h"
#include "hal/board.h"
#include "ble/app_ble.h"
#include "app/app.h"

// DEFINITIONS ****************************************************************/

// DECLARATIONS ***************************************************************/

// IMPLEMENTATION *************************************************************/

void app_run(void)
{
	ret_code_t nrf_result;
	AppResult app_result;

 	nrf_result = NRF_LOG_INIT(NULL);
	APP_ERROR_CHECK(nrf_result);
	NRF_LOG_DEFAULT_BACKENDS_INIT();

    NRF_LOG_INFO("********************************************************************************");
    NRF_LOG_INFO("Copyright (c) Glue Home Ltd, 2019. All rights reserved");
    NRF_LOG_INFO("********************************************************************************");
    NRF_LOG_FLUSH();

    // Initialise NRF modules
    nrf_result = app_timer_init();
    APP_ERROR_CHECK(nrf_result);

    app_result = board_init();
    APP_ERROR_CHECK((ret_code_t)app_result);
	app_result = app_ble_init();
	APP_ERROR_CHECK((ret_code_t)app_result);

	app_ble_advertising_start();

	while (1)
	{
		(void) NRF_LOG_PROCESS();
	}
}

// HANDLERS *******************************************************************/

// CALLBACKS ******************************************************************/
