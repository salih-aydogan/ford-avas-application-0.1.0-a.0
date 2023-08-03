// PRQA S 3108 EOF # Comments with links are required for ensure traceability.
/**
 * @file bsp_eeprom.h
 * @author berke yalcinkaya (berke.yalcinkaya@daiichi.com)
 * @brief Interface of the EEPROM.
 * @version 0.1
 * @date 2023-04-12
 * 
 * @copyright Copyright (c) 2023
 *
 * https://codebeamer.daiichi.com/issue/187390
 * https://codebeamer.daiichi.com/issue/187685
 * 
 */

#ifndef BSP_EEPROM_H
#define BSP_EEPROM_H

#include "daiichi_types.h"

/*******************************************************************************
 * Macros
 ******************************************************************************/

/**
 * @brief This macro holds to FlexRAM size for emulated EEPROM.
 * 
 * https://codebeamer.daiichi.com/issue/188637
 * 
 */
#define BSP_EEPROM_SIZE            (8U * 256U) - (128U)

/*******************************************************************************
 * Enumerations
 ******************************************************************************/

/**
 * @brief List of EEPROM operations status.
 * 
 * https://codebeamer.daiichi.com/issue/188564
 * 
 */
typedef enum bsp_eeprom_status_e
{
    BSP_EEPROM_STATUS_SUCCESS = 0U,
    BSP_EEPROM_STATUS_NOT_RDY = 1U,
    BSP_EEPROM_STATUS_ERR = 2U,
    BSP_EEPROM_STATUS_ERR_PARAM = 3U,
    BSP_EEPROM_STATUS_ERR_ADDR_SIZE = 4U,

} bsp_eeprom_status_t;

/*******************************************************************************
 * Public Functions 
 ******************************************************************************/

/**
 * @brief This function directly accesses the FlexRAM address and obtains the 
 *        data. This function reaches the desired data by shifting the start 
 *        address of FlexRAM by the addr parameter.
 * 
 * @param addr[in]    The address of the data in the EEPROM.
 * @param p_data[out] The address of the variable where read data is written.
 * @param size[in]    Data size.
 * 
 * @return bsp_eeprom_status_t 
 * 
 * @retval BSP_EEPROM_STATUS_SUCCESS       Read operation successfully done
 * @retval BSP_EEPROM_STATUS_ERR_PARAM     The p_data parameter has a NULL value
 * @retval BSP_EEPROM_STATUS_NOT_RDY       FlexRAM is not ready.
 * @retval BSP_EEPROM_STATUS_ERR_ADDR_SIZE The sum of the given address and data 
 *                                         size exceeds the FlexRAM size
 * 
 * https://codebeamer.daiichi.com/issue/188102
 * 
 */
bsp_eeprom_status_t bsp_eeprom_read(uint32_t addr,
                                    uint8_t *p_data,
                                    size_t size);

/**
 * @brief This function writes the requested data to the EEPROM. This function 
 *        reaches the desired data by shifting the start address of FlexRAM by 
 *        the addr parameter.
 * 
 * @param addr[in]   The address of the data in the EEPROM.
 * @param p_data[in] The address of the variable where write data.
 * @param size[in]   Data size.
 * 
 * @return bsp_eeprom_status_t 
 * 
 * @retval BSP_EEPROM_STATUS_SUCCESS       Read operation successfully done
 * @retval BSP_EEPROM_STATUS_ERR_PARAM     The p_data parameter has a NULL value
 * @retval BSP_EEPROM_STATUS_NOT_RDY       FlexRAM is not ready.
 * @retval BSP_EEPROM_STATUS_ERR_ADDR_SIZE The sum of the given address and data 
 *                                         size exceeds the FlexRAM size
 * @retval BSP_EEPROM_STATUS_ERR           The EEPROM write generic error
 * 
 * https://codebeamer.daiichi.com/issue/188103
 * 
 */
bsp_eeprom_status_t bsp_eeprom_write(uint32_t addr,
                                     const uint8_t *p_data,
                                     size_t size);

#endif /* BSP_EEPROM_H  */

// end of file
