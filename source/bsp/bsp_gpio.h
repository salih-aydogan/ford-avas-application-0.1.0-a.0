// PRQA S 3108 EOF # Comments with links are required for ensure traceability.
/**
 * @file bsp_gpio.h
 * @author Furkan Yildirim (furkan.yildirim@daiichi.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-08
 * 
 * @copyright Copyright (c) 2023
 * 
 * https://codebeamer.daiichi.com/issue/178050
 * https://codebeamer.daiichi.com/issue/177718
 * 
 */

#ifndef BSP_GPIO_H
#define BSP_GPIO_H

#include "daiichi_types.h"

/*******************************************************************************
 * Macros
 ******************************************************************************/
 
/**
 * @brief This macro holds port number and pin number info of relevant pin.
 * 
 * https://codebeamer.daiichi.com/issue/180115
 * 
 */
#define BSP_GPIO_AMP_STB_1_OUT_PIN_NO       315U

/**
 * @brief This macro holds port number and pin number info of relevant pin.
 * 
 * https://codebeamer.daiichi.com/issue/180116
 * 
 */
#define BSP_GPIO_AMP_STB_2_OUT_PIN_NO       314U

/**
 * @brief This macro holds port number and pin number info of relevant pin.
 * 
 * https://codebeamer.daiichi.com/issue/180118
 * 
 */
#define BSP_GPIO_CAN_STB_OUT_PIN_NO         45U

/**
 * @brief This macro holds port number and pin number info of relevant pin.
 * 
 * https://codebeamer.daiichi.com/issue/180119
 * 
 */
#define BSP_GPIO_CAN_EN_OUT_PIN_NO          316U

/**
 * @brief This macro holds port number and pin number info of relevant pin.
 * 
 * https://codebeamer.daiichi.com/issue/180121
 * 
 */
#define BSP_GPIO_AMP_MUTE_1_OUT_PIN_NO      23U

/**
 * @brief This macro holds port number and pin number info of relevant pin.
 * 
 * https://codebeamer.daiichi.com/issue/180122
 * 
 */
#define BSP_GPIO_AMP_MUTE_2_OUT_PIN_NO      22U

/**
 * @brief This macro indicates the local wake-up input of the CAN transceiver.
 * 
 * https://codebeamer.daiichi.com/issue/190354
 * 
 */
#define BSP_GPIO_CAN_WAKE_OUT_PIN_NO        58U

/**
 * @brief This macro holds port number and pin number info of relevant pin.
 * 
 * https://codebeamer.daiichi.com/issue/180123
 * 
 */
#define BSP_GPIO_AMP_1_ERROR_IN_PIN_NO      20U

/**
 * @brief This macro holds port number and pin number info of relevant pin.
 * 
 * https://codebeamer.daiichi.com/issue/180125
 * 
 */
#define BSP_GPIO_AMP_2_ERROR_IN_PIN_NO      21U

/**
 * @brief This macro holds port number and pin number info of relevant pin.
 * 
 * https://codebeamer.daiichi.com/issue/180126
 * 
 */
#define BSP_GPIO_CAN_ERROR_IN_PIN_NO        31U

/**
 * @brief This macro indicates the SPI Chip Select output of the External Flash.
 * 
 * https://codebeamer.daiichi.com/issue/197473
 * 
 */
#define BSP_GPIO_SPI_CS_OUT_PIN_NO          25U

// PRQA S 1534 ++ # The purpose of these macros is configuration. It may need to be used and not used as conditions change.

/**
 * @brief This macro holds interrupt ID number.
 * 
 * https://codebeamer.daiichi.com/issue/180128
 * 
 */
#define BSP_GPIO_INT_BOTH_EDGE              1U

/**
 * @brief This macro holds interrupt ID number.
 * 
 * https://codebeamer.daiichi.com/issue/180129
 * 
 */
#define BSP_GPIO_INT_RISING_EDGE            2U

/**
 * @brief This macro holds interrupt ID number.
 * 
 * https://codebeamer.daiichi.com/issue/180130
 * 
 */
#define BSP_GPIO_INT_FALLING_EDGE           4U

// PRQA S 1534 --

/**
 * @brief This macro holds interrupt number of amplifier 1 error pin.
 * 
 * https://codebeamer.daiichi.com/issue/180131
 * 
 */
#define BSP_GPIO_CFG_AMP_1_ERROR_IN_PIN     BSP_GPIO_INT_FALLING_EDGE

/**
 * @brief This macro holds interrupt number of amplifier 2 error pin.
 * 
 * https://codebeamer.daiichi.com/issue/180132
 * 
 */
#define BSP_GPIO_CFG_AMP_2_ERROR_IN_PIN     BSP_GPIO_INT_FALLING_EDGE

/**
 * @brief This macro holds interrupt number of can-bus error pin.
 * 
 * https://codebeamer.daiichi.com/issue/180133
 * 
 */
#define BSP_GPIO_CFG_CAN_ERROR_IN_PIN       BSP_GPIO_INT_RISING_EDGE

/*******************************************************************************
 * Enumerations
 ******************************************************************************/

/**
 * @brief List of pin state types.
 * 
 * https://codebeamer.daiichi.com/issue/180137
 * https://codebeamer.daiichi.com/issue/180138
 * 
 */
typedef enum bsp_gpio_status_e
{
    BSP_GPIO_STATUS_RESET = 0U,
    BSP_GPIO_STATUS_SET   = 1U,
    BSP_GPIO_STATUS_ERR   = 2U,
        
} bsp_gpio_status_t;

/*******************************************************************************
 * Type Definitions
 ******************************************************************************/

/**
 * @brief Function pointer type for ISR function specified when registering to 
 *        the GPIO.
 * 
 * https://codebeamer.daiichi.com/issue/180139
 * 
 */
typedef void (*bsp_gpio_cb_t)(void);

/*******************************************************************************
 * Public Functions 
 ******************************************************************************/

/**
 * @brief This funciton initializes all GPIO ports.
 * 
 * https://codebeamer.daiichi.com/issue/180145
 * 
 * @return true 
 * @return false 
 */
bool bsp_gpio_init(void);

/**
 * @brief This function returns the status of the pin whose pin number is given 
 *        as an argument.
 * 
 * https://codebeamer.daiichi.com/issue/180146
 * 
 * @param[in] pin_num Any of the pin numbers defined as a macro.
 * 
 * @return bsp_gpio_status_t 
 */
bsp_gpio_status_t bsp_gpio_status_get(uint32_t pin_num);

/**
 * @brief This function sets the status of the pin whose pin number is given 
 *        as an argument.
 * 
 * https://codebeamer.daiichi.com/issue/180147
 * 
 * @param[in] pin_num Any of the pin numbers defined as a macro.
 */
void bsp_gpio_set(uint32_t pin_num);

/**
 * @brief This function resets the status of the pin whose pin number is given 
 *        as an argument.
 * 
 * https://codebeamer.daiichi.com/issue/180148
 * 
 * @param[in] pin_num Any of the pin numbers defined as a macro.
 */
void bsp_gpio_reset(uint32_t pin_num);

/**
 * @brief This function sets the callback function of the pin whose pin number 
 *        is given as an argument.
 * 
 * https://codebeamer.daiichi.com/issue/180149
 * 
 * @param[in] pin_num Any of the pin numbers defined as a macro.
 * @param[in] callback Address of the callback function to the relevant pin.
 */
void bsp_gpio_cb_set(uint32_t pin_num, bsp_gpio_cb_t callback);

#endif /* BSP_GPIO_H */

// end of file
