/***************************************************************************//**
 @copyright © Glue Home Ltd, 2018. All rights reserved
 @author    A Villarreal
 @date      23/04/19
 @file      board.h
 @brief     Provides board definitions
 ******************************************************************************/

#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include "err_codes.h"

#ifdef __cplusplus
extern "C" {
#endif

// DEFINITIONS ****************************************************************/

/** GPIO definitions */
#define BOARD_BTN_1_PIN			(13)
#define BOARD_BTN_2_PIN			(14)
#define BOARD_BTN_3_PIN			(15)
#define BOARD_BTN_4_PIN			(16)

#define BOARD_LED_1_PIN			(17)
#define BOARD_LED_2_PIN			(18)
#define BOARD_LED_3_PIN			(19)
#define BOARD_LED_4_PIN			(20)

// DECLARATIONS ***************************************************************/

/** Strongly-typed board event callback */
// typedef void (*board_event_callback_t)(uint8_t board_pin);

/**
 * Initialise the board pins
 * @retval  ERR_SUCCESS: Success
 * @retval  ERR_ERROR: Failure
 */
AppResult board_init();

#ifdef __cplusplus
}
#endif

#endif  // BOARD_H
