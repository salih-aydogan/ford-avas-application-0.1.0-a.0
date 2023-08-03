// PRQA S 3108 EOF # Allowed to ensure traceability.
/**
 * @file bsp_spi.h
 * @author Berke Yalcinkaya (berke.yalcinkaya@daiichi.com)
 * @brief Interface of the spi.
 * @version 0.1
 * @date 2023-02-08
 *
 * @copyright Copyright (c) 2023
 *
 * https://codebeamer.daiichi.com/issue/178048
 *
 */

#ifndef BSP_SPI_H
#define BSP_SPI_H

#include "daiichi_types.h"

/*******************************************************************************
 * Enumerations
 ******************************************************************************/

/**
 * @brief Enumeration specifying errors that occur during transmission.
 *
 * https://codebeamer.daiichi.com/issue/178552
 *
 */
typedef enum bsp_spi_err_e
{
    BSP_SPI_ERR_NONE = (0U),
    BSP_SPI_ERR_TX_UNDERRUN = (1U),
    BSP_SPI_ERR_RX_OVERRUN = (2U),
    BSP_SPI_ERR_ABORTED = (3U),

} bsp_spi_err_t;

/*******************************************************************************
 * Function Type
 ******************************************************************************/

/**
 * @brief This function pointer specified as error function uses in
 *        communication errors. It provides function definition to
 *        the upper layers in case of communication errors.
 *
 * https://codebeamer.daiichi.com/issue/178570
 *
 */
typedef void (*bsp_spi_err_cb_t)(bsp_spi_err_t err);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief SPI module initialization function. Also this function
 *        configure driver modules such as gpio, flexio and lpspi.
 *
 * https://codebeamer.daiichi.com/issue/178546
 *
 * @return bool initialization success or error.
 *
 * @retval true  Success.
 * @retval false Error.
 */
bool bsp_spi_init(void);

/**
 * @brief SPI module deinitialization function. Deinitialize flexio
 *        and lpspi driver modules.
 *
 * https://codebeamer.daiichi.com/issue/178547
 *
 * @return bool deinitialization success or error.
 *
 * @retval true  Success.
 * @retval false Error.
 */
bool bsp_spi_deinit(void);

/**
 * @brief SPI module write function. This function work with blocking mode.
 *        The function sends the given data synchronously. The function does not
 *        exit until the send is complete or an error occurs.
 *
 * https://codebeamer.daiichi.com/issue/178549
 *
 * @param[in] p_data The data to be sent with spi.
 * @param[in] size   The size of the data to be sent.
 *
 * @return bool write operation success or error.
 *
 * @retval true  Success.
 * @retval false Error.
 */
bool bsp_spi_write(const uint8_t *p_data, size_t size);

/**
 * @brief Write data to the FLASH asynchronously. If there is no error,
 *        BSP_SPI_ERR_NONE is triggered when data writing to flash is
 *        finished.
 *
 * https://codebeamer.daiichi.com/issue/197613
 *
 * @param[out] p_data The variable to hold the write data.
 * @param[in] size    Data size to write.
 *
 * @return bool read operation success or error.
 *
 * @retval true  Success.
 * @retval false Error.
 */
bool bsp_spi_async_write(const uint8_t *p_data, size_t size);

/**
 * @brief Returns if the SPI peripheral is busy with another read/write
 *        transmission or not.
 *
 * https://codebeamer.daiichi.com/issue/197614
 *
 * @return bool Busy status of the SPI peripheral.
 *
 * @retval true  SPI is busy. New transmission cannot be made until the
 *               present transmission is completed.
 * @retval false SPI is ready. New transmission can be started.
 */
bool bsp_spi_is_busy(void);

/**
 * @brief SPI module read function. This function work with blocking mode.
 *        The function receives the incoming data synchronously. The function
 *        does not exit until the receive is complete or an error occurs.
 *
 * https://codebeamer.daiichi.com/issue/178549
 *
 * @param[out] p_data The data to be receive with spi.
 * @param[in] size    The size of the data to be sent.
 *
 * @return bool write operation success or error.
 *
 * @retval true  Success.
 * @retval false Error.
 */
bool bsp_spi_read(uint8_t *p_data, size_t size);

/**
 * @brief Read data from the FLASH. If there is no error, BSP_SPI_ERR_NONE is
 *        triggered when data reading from the flash to receive buffer is
 *        finished.
 *
 * https://codebeamer.daiichi.com/issue/180583
 *
 * @param[out] p_data The variable to hold the read data.
 * @param[in] size    Data size to read.
 *
 * @return bool read operation success or error.
 *
 * @retval true  Success.
 * @retval false Error.
 */
bool bsp_spi_async_read(uint8_t *p_data, size_t size);

/**
 * @brief SPI module transmit callback. This function allows to use the
 *        user callback function that will be provided from upper layer.
 *        The user callback function is called in case of errors that
 *        occur during transmission or when the transmission is
 *        completed.
 *
 * https://codebeamer.daiichi.com/issue/178571
 *
 * @param[in] callback User callcack function provided from upper layer.
 *
 *
 * @return bool operation success or error.
 *
 * @retval true  Success.
 * @retval false Error.
 */
bool bsp_spi_cb_set(bsp_spi_err_cb_t callback);

#endif /* BSP_SPI_H  */

// end of file
