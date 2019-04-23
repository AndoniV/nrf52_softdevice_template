/*******************************************************************************
 @author    A Villarreal
 @date      18/04/19
 @file      err_codes.h
 @brief     Provides common error codes used across modules
*******************************************************************************/

#ifndef ERR_CODES_H
#define ERR_CODES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// DEFINITIONS ****************************************************************/

#define ERR_SUCCESS                    	(0x00)
#define ERR_INVALID_PARAM				(0x01)

#define ERR_ERROR 						(0xFF)

// DECLARATIONS ***************************************************************/

typedef uint8_t AppResult;

#ifdef __cplusplus
}
#endif

#endif // ERR_CODES_H
