// PRQA S 3108 EOF # Suppressed, Comments with links are required for ensure traceability.
/**
 * @file bsp_timer.h
 * @author berke yalcinkaya (berke.yalcinkaya@daiichi.com)
 * @brief Interface of the timer.
 * @version 0.1
 * @date 2023-02-14
 *
 * @copyright Copyright (c) 2023
 *
 * https://codebeamer.daiichi.com/issue/177712
 * https://codebeamer.daiichi.com/issue/180399
 *
 */

#ifndef BSP_TIMER_H
#define BSP_TIMER_H

#include "daiichi_types.h"

/*******************************************************************************
 * Macros
 ******************************************************************************/

/**
 * @brief This macro should be used to indicate the dedicated timer port for the
 *        DAC.
 *
 * https://codebeamer.daiichi.com/issue/180564
 *
 */
#define BSP_TIMER_DAC_PORT      (0U)

/*******************************************************************************
 * Function Type
 ******************************************************************************/

/**
 * @brief Type definition for timeout operations.
 *
 */
typedef void (*mcu_timeout_cb_t)(void);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Initialize a low-power timer instance with values from an input
 *        configuration structure and enables an interrupt.
 *
 * https://codebeamer.daiichi.com/issue/180557
 *
 */
void bsp_timer_init(void);

/**
 * @brief De-initialize the timer, stop the counter and reset all registers to
 *        the default value.
 *
 * https://codebeamer.daiichi.com/issue/180558
 *
 */
void bsp_timer_deinit(void);

/**
 * @brief This function enables to start of the counter.
 *
 * https://codebeamer.daiichi.com/issue/180559
 *
 * @param[in] port The port parameter specifies the unit of interest.
 */
void bsp_timer_start(uint32_t port);

/**
 * @brief This function enables to stop of the counter.
 *
 * https://codebeamer.daiichi.com/issue/180560
 *
 * @param[in] port The port parameter specifies the unit of interest.
 */
void bsp_timer_stop(uint32_t port);

/**
 * @brief This function set the compare value in counter tick units, for a
 *        low-power timer instance according to the port.
 *
 * https://codebeamer.daiichi.com/issue/180561
 *
 * @param[in] port      The port parameter specifies the unit of interest.
 * @param[in] period_hz Specifies the timeout value in terms of frequency.
 *
 * @return bool Registration success or error.
 *
 * @retval true  Success.
 * @retval false Error.
 */
bool bsp_timer_period_set(uint32_t port, uint16_t period_hz);

/**
 * @brief This function is used to register the user defined callback function.
 *
 * https://codebeamer.daiichi.com/issue/180751
 *
 * @param[in] port      The port parameter specifies the unit of interest.
 * @param[in] callback  Holds the timeout callback function.
 *
 * @return bool Registration success or error.
 *
 * @retval true  Success.
 * @retval false Error.
 */
bool bsp_timer_cb_set(uint32_t port, mcu_timeout_cb_t callback);

#endif /* BSP_TIMER_H  */

// end of file
