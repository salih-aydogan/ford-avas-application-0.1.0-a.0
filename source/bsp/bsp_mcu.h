/**
 * @file mcu.h
 * @author Furkan Yildirim (furkan.yildirim@daiichi.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-26
 * 
 * @copyright Copyright (c) 2023
 * 
 * https://codebeamer.daiichi.com/issue/178497
 * https://codebeamer.daiichi.com/issue/177720
 * 
 */

#ifndef BSP_MCU_H
#define BSP_MCU_H

/*******************************************************************************
 * Includes
 ******************************************************************************/                   
#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
 * Macros
 ******************************************************************************/

/**
 * @brief Purpose of this macro is to configurate the system clock frequency.
 * 
 * https://codebeamer.daiichi.com/issue/180048
 * 
 */
#define BSP_MCU_CFG_FREQ_HZ        40000000U

/*******************************************************************************
 * Type Definitions
 ******************************************************************************/

/**
 * @brief List of the error types may occure when MCU operations.
 * 
 * https://codebeamer.daiichi.com/issue/180088
 * https://codebeamer.daiichi.com/issue/180090
 * 
 */
typedef enum bsp_mcu_status_e
{
    BSP_MCU_STATUS_OK = 0U,
    BSP_MCU_STATUS_TRANSITION_FAILED = 1U,
    BSP_MCU_STATUS_INVALID = 2U,
    BSP_MCU_STATUS_ERROR = 3U

} bsp_mcu_status_t;

/*******************************************************************************
 * Public Functions 
 ******************************************************************************/

/**
 * @brief This function inits the MCU.
 * 
 * https://codebeamer.daiichi.com/issue/180031
 * 
 * @return bsp_mcu_status_t 
 * 
 * @retval BSP_MCU_STATUS_OK                  MCU has been initialied.
 * @retval BSP_MCU_STATUS_TRANSITION_FAILED   MCU transition failed.
 * @retval BSP_MCU_STATUS_INVALID             MCU invalid state.
 * @retval BSP_MCU_STATUS_ERROR               Possible error states:
 *                                                STATUS_ERROR
 *                                                STATUS_BUSY
 *                                                STATUS_TIMEOUT
 *                                                STATUS_UNSUPPORTED
 *                                                STATUS_MCU_GATED_OFF
 *                                                STATUS_MCU_NOTIFY_BEFORE_ERROR
 *                                                STATUS_MCU_NOTIFY_AFTER_ERROR  
 * 
 */
bsp_mcu_status_t bsp_mcu_init(void);

/**
 * @brief This function delays the MCU.
 * 
 * https://codebeamer.daiichi.com/issue/180158
 * 
 * @param[in] delay time in milliseconds. 
 */
void bsp_mcu_delay(uint32_t delay);

/**
 * @brief Initiates a request to reset the MCU.
 * 
 * https://codebeamer.daiichi.com/issue/180159
 * 
 */
void bsp_mcu_reset(void);

/**
 * @brief This function returns the number of miliseconds elapsed since 
 *        starting the internal timer. 
 * 
 * https://codebeamer.daiichi.com/issue/180160
 * 
 * @return uint32_t 
 */
uint32_t bsp_mcu_tick_get(void);

#endif /* BSP_MCU_H */

// end of file
