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

#include "app/app.h"

// DEFINITIONS ****************************************************************/

// DECLARATIONS ***************************************************************/

// IMPLEMENTATION *************************************************************/

void app_run(void)
{
	uint8_t hex_data[] = { 0x31, 0x32, 0x33, 0xBA, 0x45 };

 	ret_code_t nrf_result = NRF_LOG_INIT(NULL);
	APP_ERROR_CHECK(nrf_result);
	NRF_LOG_DEFAULT_BACKENDS_INIT();

	NRF_LOG_INFO("[APP] Info");
	NRF_LOG_DEBUG("[APP] Debug");
	NRF_LOG_WARNING("[APP] Warning");
	NRF_LOG_ERROR("[APP] Error");
	NRF_LOG_HEXDUMP_DEBUG(hex_data, sizeof(hex_data));
	NRF_LOG_RAW_INFO("[APP] Raw Info\n");
	NRF_LOG_RAW_HEXDUMP_INFO("[APP] Raw Info", 14);

	NRF_LOG_FLUSH();	

	while (1)
	{
		(void) NRF_LOG_PROCESS();
	}
}

// HANDLERS *******************************************************************/

// CALLBACKS ******************************************************************/