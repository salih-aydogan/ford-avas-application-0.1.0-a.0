// PRQA S 3108 EOF # Comments with links are required for ensure traceability.
/**
 * @file bsp_i2c.h
 * @author Furkan Yildirim (furkan.yildirim@daiichi.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-23
 * 
 * @copyright Copyright (c) 2023
 * 
 * https://codebeamer.daiichi.com/issue/178498
 * https://codebeamer.daiichi.com/issue/177749
 * 
 */

#ifndef BSP_I2C_H
#define BSP_I2C_H

#include "daiichi_types.h"

/*******************************************************************************
 * Macros
 ******************************************************************************/

/**
 * @brief This macro should be used to indicate the dedicated I2C port of 
 *        amplifier module 1.
 * 
 * https://codebeamer.daiichi.com/issue/179888
 * 
 */
#define BSP_I2C_AMP_1_PORT 0U

/**
 * @brief This macro should be used to indicate the dedicated I2C port of 
 *        amplifier module 2.
 * 
 * https://codebeamer.daiichi.com/issue/179889
 * 
 */
#define BSP_I2C_AMP_2_PORT 1U

/*******************************************************************************
 * Enumerations
 ******************************************************************************/

/**
 * @brief List of errors
 * 
 * https://codebeamer.daiichi.com/issue/179891
 * https://codebeamer.daiichi.com/issue/179890
 * 
 */
typedef enum bsp_i2c_err_e
{
    BSP_I2C_STATUS_ERR_GENERIC = 0U,
    BSP_I2C_STATUS_ERR_RECEIVED_NACK = 1U,
    BSP_I2C_STATUS_ERR_TX_UNDERRUN = 2U,
    BSP_I2C_STATUS_ERR_RX_OVERRUN = 3U,
    BSP_I2C_STATUS_ERR_ARBITRATION_LOST = 4U,
    BSP_I2C_STATUS_ERR_ABORTED = 5U,
    BSP_I2C_STATUS_ERR_BUS_BUSY = 6U,
    
} bsp_i2c_err_t;

/*******************************************************************************
 * Type Definitions
 ******************************************************************************/

/**
 * @brief Function pointer type for error function specified when registering to 
 *        the I2C via i2c_register_cb
 * 
 * https://codebeamer.daiichi.com/issue/179893
 * 
 */
typedef void (*bsp_i2c_err_cb_t)(bsp_i2c_err_t error);

/*******************************************************************************
 * Public Functions  
 ******************************************************************************/

/**
 * @brief This function initializes all I2C ports.
 * 
 * https://codebeamer.daiichi.com/issue/180103
 * 
 * @return true  Success.
 * @return false Error.
 * 
 */
bool bsp_i2c_init(void);

/**
 * @brief This function deinitializes all I2C ports.
 * 
 * https://codebeamer.daiichi.com/issue/179883
 *  
 * @return true  Success.
 * @return false Error.
 * 
 */
bool bsp_i2c_deinit(void);

/**
 * @brief This function writes data to target device with chosen I2C port.
 * 
 * https://codebeamer.daiichi.com/issue/179884
 * 
 * @param port The I2C ports. It SHOULD be one of the following at a time:
 *                - BSP_I2C_AMP_1_PORT
 *                - BSP_I2C_AMP_2_PORT
 * @param[in] p_data Pointer to write data buffer. It SHOULD NOT be NULL.
 * @param[in] size   Data size to be written in bytes. It SHOULD NOT be zero.
 * 
 * @return true  Success.
 * @return false Error.
 * 
 */
bool bsp_i2c_write(uint32_t port, const uint8_t* p_data, size_t size);

/**
 * @brief This function reads data from target device with chosen I2C port.
 * 
 * https://codebeamer.daiichi.com/issue/179885
 * 
 * @param port The I2C ports. It SHOULD be one of the following at a time:
 *                - BSP_I2C_AMP_1_PORT
 *                - BSP_I2C_AMP_2_PORT
 * @param[out] p_data Pointer to read data buffer. It SHOULD NOT be NULL.
 * @param[in]  size   Data size to read in bytes. It SHOULD NOT be zero.
 * 
 * @return true  Success.
 * @return false Error.
 * 
 */
bool bsp_i2c_read(uint32_t port, uint8_t* p_data, size_t size);

/**
 * @brief This function writes data to target device and after reads data from
 * target device with chosen I2C port. 
 * 
 * https://codebeamer.daiichi.com/issue/179886
 * 
 * @param port The I2C ports. It SHOULD be one of the following at time:
 *                - BSP_I2C_AMP_1_PORT
 *                - BSP_I2C_AMP_2_PORT
 * @param[out] p_rx_data Pointer to read data buffer. It SHOULD NOT be NULL.
 * @param[in]  rx_size   Data size to read in bytes. It SHOULD NOT be zero.
 * @param[in]  p_tx_data Pointer to write data buffer. It SHOULD NOT be NULL.
 * @param[in]  tx_size   Data size to be written in bytes. It SHOULD NOT be zero
 * 
 * @return true  Success.
 * @return false Error.
 * 
 */
bool bsp_i2c_write_read(uint32_t port, 
                        uint8_t* p_rx_data,
                        size_t rx_size, 
                        const uint8_t* p_tx_data,
                        size_t tx_size);

/**
 * @brief This function sets callback function for logging I2C errors.
 * 
 * https://codebeamer.daiichi.com/issue/179887
 * 
 * @param[in] port The I2C ports. It SHOULD be one of the following at time:
 *                    - BSP_I2C_AMP_1_PORT
 *                    - BSP_I2C_AMP_2_PORT 
 * @param[in] callback Pointer to the callback function. It SHOULD NOT be NULL.
 * 
 * @return true  Success.
 * @return false Error.
 * 
 */
void bsp_i2c_cb_set(uint32_t port, bsp_i2c_err_cb_t callback);

#endif /* BSP_I2C_H */

// end of file
