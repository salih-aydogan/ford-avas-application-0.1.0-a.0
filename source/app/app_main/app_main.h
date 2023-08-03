/**
 * @file app_main.h
 * @author Burak Utku Cecen (utku.cecen@daiichi.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-02
 * 
 * @copyright Copyright (c) 2023
 * 
 * https://codebeamer.daiichi.com/issue/180314
 * https://codebeamer.daiichi.com/issue/185052
 * 
 */

#ifndef APP_MAIN_H
#define APP_MAIN_H

#include "daiichi_types.h"

/*******************************************************************************
 * Macros
 ******************************************************************************/

/**
 * @brief Specifies to receiving message size.
 * 
 * https://codebeamer.daiichi.com/issue/187452
 * 
 */
#define APP_MAIN_MSG_SIZE                12U

/**
 * @brief Specifies maximum message count.
 * 
 * https://codebeamer.daiichi.com/issue/187451
 * 
 */
#define APP_MAIN_MSG_POOL_SIZE           4U

/*******************************************************************************
 * Public Variables
 ******************************************************************************/

/**
 * @brief Specifies app_main message pool.
 * 
 * https://codebeamer.daiichi.com/issue/187449
 * 
 */
extern uint8_t g_app_main_msg_pool[APP_MAIN_MSG_SIZE * APP_MAIN_MSG_POOL_SIZE];

/*******************************************************************************
 * Public Functions 
 ******************************************************************************/

/**
 * @brief Initializes the AVAS module.
 * 
 * https://codebeamer.daiichi.com/issue/185684,
 * 
 */
void app_main_init(void);

/**
 * @brief De-initializes the AVAS module
 * 
 * https://codebeamer.daiichi.com/issue/185685
 * 
 */
void app_main_deinit(void);

/**
 * @brief Starts to WDG timer counter and log process..
 * 
 * https://codebeamer.daiichi.com/issue/185686
 * 
 */
void app_main_run(void);

/**
 * @brief Getting message from other application.
 * 
 * https://codebeamer.daiichi.com/issue/185688
 * 
 */
void app_main_msg(void* p_msg);

#endif /* APP_MAIN_H */

// end of file
