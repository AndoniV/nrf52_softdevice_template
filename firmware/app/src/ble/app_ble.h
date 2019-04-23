/*******************************************************************************
 @author    A Villarreal
 @date      23/04/19
 @file      ble.h
 @brief     Provides a BLE interface for appication
 ******************************************************************************/

#ifndef APP_BLE_H
#define APP_BLE_H

#include <stdbool.h>
#include <stdint.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "err_codes.h"

#ifdef __cplusplus
extern "C" {
#endif

// DEFINITIONS ****************************************************************/

// DECLARATIONS ***************************************************************/

/**
 *	Initialize the module
 *	@retval ERR_SUCCESS: 	Success
 *  @retval ERR_ERROR: 		Failed to initialise
 */
AppResult app_ble_init(void);

/**
 * Start advertising
 * @retval  ERR_SUCCESS: Success
 * @retval  ERR_ERROR:   Failed to start
 */
AppResult app_ble_advertising_start();

#ifdef __cplusplus
}
#endif

#endif // APP_BLE_H
