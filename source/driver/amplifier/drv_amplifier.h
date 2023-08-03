// PRQA S 3108 EOF # Comments with links are required for ensure traceability.

/**
 * @file drv_amplifier.h
 * @author Furkan Yildirim (furkan.yildirim@daiichi.com)
 * @brief
 * @version 0.1
 * @date 2023-05-15
 *
 * @copyright Copyright (c) 2023
 *
 * https://codebeamer.daiichi.com/issue/177726
 * https://codebeamer.daiichi.com/issue/190525
 *
 */

#ifndef DRV_AMPLIFIER_H
#define DRV_AMPLIFIER_H

#include "daiichi_types.h"

/*******************************************************************************
 * Enumerations
 ******************************************************************************/

/**
 * @brief List of error status of the amplifier.
 *
 * https://codebeamer.daiichi.com/issue/192721
 * https://codebeamer.daiichi.com/issue/192722
 *
 */
typedef enum drv_amplifier_diag_err_e
{
    DRV_AMPLIFIER_DIAG_INPUT_OFFSET = 0U,
    DRV_AMPLIFIER_DIAG_OVERCURR_PROTECT,
    DRV_AMPLIFIER_DIAG_CLIPPING_DETECT,
    DRV_AMPLIFIER_DIAG_THERMAL_SD,
    DRV_AMPLIFIER_DIAG_ANALOG_MUTE,
    DRV_AMPLIFIER_DIAG_VOV_SD,
    DRV_AMPLIFIER_DIAG_OUTCURR_OFFSET,
    DRV_AMPLIFIER_DIAG_DATA_VALID,
    DRV_AMPLIFIER_DIAG_PLAY,
    DRV_AMPLIFIER_DIAG_ERR_SHORT_TO_VCC,
    DRV_AMPLIFIER_DIAG_ERR_SHORT_TO_GND,
    DRV_AMPLIFIER_DIAG_ERR_COMMUNICATION,
    DRV_AMPLIFIER_DIAG_ERR_TW_1,
    DRV_AMPLIFIER_DIAG_ERR_TW_2,
    DRV_AMPLIFIER_DIAG_ERR_TW_3,
    DRV_AMPLIFIER_DIAG_ERR_TW_4,
    DRV_AMPLIFIER_DIAG_ERR_UVLO,
    DRV_AMPLIFIER_DIAG_ERR_LOW_BATT,
    DRV_AMPLIFIER_DIAG_ERR_HIGH_BATT,
    DRV_AMPLIFIER_DIAG_ERR_SHORT_LOAD,
    DRV_AMPLIFIER_DIAG_ERR_OPEN_LOAD,

} drv_amplifier_diag_t;

/**
 * @brief List of amplifier channels.
 *
 * https://codebeamer.daiichi.com/issue/191801
 * https://codebeamer.daiichi.com/issue/191799
 *
 */
typedef enum drv_amplifier_channel_e
{
    DRV_AMPLIFIER_CHANNEL_1 = 0U,
    DRV_AMPLIFIER_CHANNEL_2 = 1U,

} drv_amplifier_channel_t;

/*******************************************************************************
 * Type Definitions
 ******************************************************************************/

/**
 * @brief Function pointer type for error function specified when registering
 *        to the drv_amplifier via drv_amplifier_diag_cb_set.
 *
 * https://codebeamer.daiichi.com/issue/192737
 *
 */
typedef void (*drv_amplifier_diag_cb_t)(drv_amplifier_diag_t diagnostic);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Implementation of the drv_amplifier_init.
 *
 * https://codebeamer.daiichi.com/issue/191794
 *
 * @return bool Success or fail.
 *
 * @retval true  Successful.
 * @retval false Possible errors:
 *               -A communication error.
 *               -The bsp_i2c component is not initialized properly.
 *               -The amplifier is not initialized properly.
 *
 */
bool drv_amplifier_init(void);

/**
 * @brief Implementation of the drv_amplifier_mute.
 *
 * https://codebeamer.daiichi.com/issue/191795
 *
 *
 * @return bool Success or fail.
 *
 * @retval true  Successful.
 * @retval false Possible errors:
 *               -A communication error.
 *
 */
bool drv_amplifier_mute(void);

/**
 * @brief Implementation of the drv_amplifier_unmute.
 *
 * https://codebeamer.daiichi.com/issue/191796
 *
 * @return bool Success or fail.
 *
 * @retval true  Successful.
 * @retval false Possible errors:
 *               -A communication error.
 *
 */
bool drv_amplifier_unmute(void);

/**
 * @brief Implementatiton of drv_amplifier_process.
 *
 * https://codebeamer.daiichi.com/issue/191797
 *
 * @return bool Success or fail.
 *
 * @retval true  Successful.
 * @retval false Possible errors:
 *               -A communication error.
 *
 */
bool drv_amplifier_process(void);

/**
 * @brief Implementatiton of drv_amplifier_diag_cb_set.
 *
 * https://codebeamer.daiichi.com/issue/192723
 *
 * @param[in] callback
 *
 * @return bool Success or fail.
 *
 * @retval true Successful.
 * @retval false Possible errors:
 *               -Invalid argument.
 *
 */
void drv_amplifier_diag_cb_set(drv_amplifier_diag_cb_t callback);

#endif /* DRV_AMPLIFIER_H */

// end of file
