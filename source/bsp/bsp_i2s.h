/**
 * @file bsp_i2s.h
 * @author Burak Utku Cecen (utku.cecen@daiichi.com)
 * @brief The interface of the I2S module.
 * @version 0.1
 * @date 2023-04-12
 * 
 * @copyright Copyright (c) 2023
 * 
 * https://codebeamer.daiichi.com/issue/188710
 * https://codebeamer.daiichi.com/issue/187392
 * 
 */

#ifndef BSP_I2S_H
#define BSP_I2S_H

#include "daiichi_types.h"

/*******************************************************************************
 * Enumerations
 ******************************************************************************/

/**
 * @brief eDMA channel status and event of I2S callback.
 * 
 * https://codebeamer.daiichi.com/issue/188738
 * 
 */
typedef enum bsp_i2s_status_e
{
    BSP_I2S_RX_FULL = 0U,
    BSP_I2S_TX_EMPTY = 1U,
    BSP_I2S_END_TRANSFER = 2U, 
    BSP_I2S_ERROR = 3U,

} bsp_i2s_status_t;

/*******************************************************************************
 * Type Definitions
 ******************************************************************************/

/**
 * @brief Defines function type pointer is used for the I2S callback 
 * notification.
 * 
 * https://codebeamer.daiichi.com/issue/188743
 * 
 */
typedef void (*bsp_i2s_cb_t)(bsp_i2s_status_t status);

/*******************************************************************************
 * Public Functions 
 ******************************************************************************/

/**
 * @brief Initializes the I2S interface.
 * 
 * https://codebeamer.daiichi.com/issue/188734
 * 
 * @return true   Initialization successful.
 * @return false  Initialization failed.
 * 
 */
bool bsp_i2s_init(void);

/**
 * @brief De-initializes the I2S interface.
 * 
 * https://codebeamer.daiichi.com/issue/188735
 * 
 * @return true  De-initialization successful.
 * @return false De-initialization failed.
 * 
 */
bool bsp_i2s_deinit(void);

/**
 * @brief This functions provides request transmission of I2S data.
 * 
 * https://codebeamer.daiichi.com/issue/188736
 * 
 * @param[in] data A pointer to transmitted data.
 * @param size Transmitted data size.
 * 
 * @return true  Transmission request successful.
 * @return false Transmission request failed.
 */
bool bsp_i2s_write(const uint8_t* data, size_t size);

/**
 * @brief This function provides registers the callback functions.
 * 
 * @param callback Function pointer type variable used for register of 
 * callback functions.
 * 
 * @return true   Register process successful.
 * @return false  Register process failed.
 */
bool bsp_i2s_cb_set(bsp_i2s_cb_t callback);

#endif /*BSP_I2S_H*/

// end of file
