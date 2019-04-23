/***************************************************************************//**
 @author    A Villarreal
 @date      18/04/19
 @file      build_config.h
 @brief     Provides build configuration

 NOTE: Do not move this file, it is modified by Jenkins during the build process
 ******************************************************************************/

#ifndef BUILD_CONFIG_H
#define BUILD_CONFIG_H

#include "sdk_config.h"

#ifdef __cplusplus
extern "C" {
#endif

// DEFINITIONS ****************************************************************/

/**
 * CONFIG: Select environment:
 * 0 = DEV
 * 1 = TEST
 * 2 = PROD
 */
#define CONFIG_ENV                   (0)
/**
 * CONFIG: Select build type
 * 0 = Build for App 0 or App 1
 * 1 = Build for test
 */
#define CONFIG_BUILD_TESTS           (0)

// DECLARATIONS ***************************************************************/

#ifdef __cplusplus
}
#endif

#endif // BUILD_CONFIG_H
