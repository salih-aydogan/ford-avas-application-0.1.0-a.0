// PRQA S 3108 EOF # Allowed to ensure traceability.
/**
 * @file bsp_wdg.h
 * @author berke yalcinkaya (berke.yalcinkaya@daiichi.com)
 * @brief Interface of the wdg.
 * @version 0.1
 * @date 2023-01-27
 *
 * @copyright Copyright (c) 2021
 *
 * https://codebeamer.daiichi.com/issue/180256
 *
 */

#ifndef BSP_WDG_H
#define BSP_WDG_H

#include "daiichi_types.h"

/*******************************************************************************
 * Macros
 ******************************************************************************/

/**
 * @brief This macro specifies wdg timeout value.
 *
 * https://codebeamer.daiichi.com/issue/180248
 *
 */
#define BSP_WDG_CFG_TIMEOUT_MSEC        (1000U)

/*******************************************************************************
 * Function Type
 ******************************************************************************/

/**
 * @brief This function pointer provides function definition to the upper layers
 *        in case of wdg timeout.
 *
 * https://codebeamer.daiichi.com/issue/180326
 *
 */
typedef void (*bsp_wdg_timeout_cb_t)(void);

/*******************************************************************************
 * Public Variables
 ******************************************************************************/

/**
 * @brief This function uses the NXP driver to initialize WDG unit. It also
 *        registers user callback function.
 *
 * https://codebeamer.daiichi.com/issue/180249
 *
 */
bool bsp_wdg_init(bsp_wdg_timeout_cb_t callback);

/**
 * @brief This function refreshes the WDG timer counter. The wdg unit must be
 *        initialized.
 *
 * https://codebeamer.daiichi.com/issue/180250
 *
 * @param callback User callcack function provided from upper layer.
 *
 */
void bsp_wdg_kick(void);

#endif /* BSP_WDG_H  */

// end of file
