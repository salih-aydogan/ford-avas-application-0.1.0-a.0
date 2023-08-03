// PRQA S 3108 EOF # Allowed to ensure traceability.
/**
 * @file bsp_uart.h
 * @author Berke Yalcinkaya (berke.yalcinkaya@daiichi.com)
 * @brief Interface of the uart.
 * @version 0.1
 * @date 2023-01-27
 *
 * @copyright Copyright (c) 2023
 *
 * https://codebeamer.daiichi.com/issue/178047
 *
 */

#ifndef BSP_UART_H
#define BSP_UART_H

#include "daiichi_types.h"

/*******************************************************************************
 * Macros
 ******************************************************************************/

/**
 * @brief Macro that defines the uart communication speed.
 *
 * https://codebeamer.daiichi.com/issue/178530
 *
 */
#define BSP_UART_CFG_BAUDRATE       (115200U)

/*******************************************************************************
 * Enumerations
 ******************************************************************************/

/**
 * @brief Enumeration specifying errors that occur during transmission.
 *
 * https://codebeamer.daiichi.com/issue/178533
 *
 */
typedef enum bsp_uart_err_e
{
    BSP_UART_ERR_NONE = (0U),
    BSP_UART_ERR_TX_EMPTY = (1U),
    BSP_UART_ERR_DMA = (2U),
    BSP_UART_ERR_ABORTED = (3U),

} bsp_uart_err_t;

/*******************************************************************************
 * Function Type
 ******************************************************************************/

/**
 * @brief This function pointer specified as error function uses in
 *        communication errors. It provides function definition to
 *        the upper layers in case of communication errors.
 *
 * https://codebeamer.daiichi.com/issue/178531
 *
 */
typedef void (*bsp_uart_err_cb_t)(bsp_uart_err_t err);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief UART module initialization function. Also this function
 *        configure driver modules such as gpio, edma and uart.
 *
 * https://codebeamer.daiichi.com/issue/178534
 *
 * @return bool initialization success or error.
 *
 * @retval true  Success.
 * @retval false Error.
 */
bool bsp_uart_init(void);

/**
 * @brief UART module deinitialization function. Deinitialize edma
 *        and uart driver modules.
 *
 * https://codebeamer.daiichi.com/issue/178535
 *
 * @return bool deinitialization success or error.
 *
 * @retval true  Success.
 * @retval false Error.
 */
bool bsp_uart_deinit(void);

/**
 * @brief UART module write function. This function work with dma mode.
 *        Function terminated asynchronous quickly and dma sends the
 *        given data. It checks transmitter status, if it is not
 *        busy the function executed.
 *
 * https://codebeamer.daiichi.com/issue/178536
 *
 * @param[in] p_data The data to be sent with uart.
 * @param[in] size   The size of the data to be sent.
 *
 * @return true bool write operation success or error.
 *
 * @retval true  Success.
 * @retval false Error.
 */
bool bsp_uart_async_write(const uint8_t *p_data, size_t size);

/**
 * @brief TODO
 * 
 * @param p_data 
 * @param size 
 * @return true 
 * @return false 
 */
bool bsp_uart_write(const uint8_t *p_data, size_t size);

/**
 * @brief UART module transmit callback. This function allows to use the
 *        user callback function that will be provided from upper layer.
 *        The user callback function is called in case of errors that
 *        occur during transmission or when the transmission is
 *        completed.
 *
 * https://codebeamer.daiichi.com/issue/178537
 *
 * @param[in] callback User callcack function provided from upper layer.
 */
void bsp_uart_cb_set(bsp_uart_err_cb_t callback);

#endif /* BSP_UART_H  */

// end of file
