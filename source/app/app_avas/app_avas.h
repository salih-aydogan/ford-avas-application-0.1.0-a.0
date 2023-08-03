// PRQA S 3108 EOF # Comments with links are required for ensure traceability.
/**
 * @file app_avas.h
 * @author Berke Yalcinkaya (berke.yalcinkaya@daiichi.com)
 * @brief The interface of the app_avas provides to manage audio operations.
 * @version 0.1
 * @date 2023-06-16
 *
 * @copyright Copyright (c) 2023
 *
 * https://codebeamer.daiichi.com/issue/195389
 * https://codebeamer.daiichi.com/issue/177739
 *
 */

#ifndef APP_AVAS_H
#define APP_AVAS_H

#include "daiichi_types.h"

/*******************************************************************************
 * Macros
 ******************************************************************************/

/**
 * @brief This macro indicates the total bytes of the message pool. A message
 *        takes up sixteen bytes of space. If you want to create a 5-element
 *        message pool, it should be set to 80. (16 * 5 = 80)
 *
 * https://codebeamer.daiichi.com/issue/195396
 *
 */
#define APP_AVAS_MSG_POOL_SIZE (80U)

/*******************************************************************************
 * Public Variables
 ******************************************************************************/

/**
 * @brief This variable indicates the message pool of the Avas application. It
 *        registers in appman when the AVAS application will create.
 *        AVAS application never accesses this message pool.
 *
 * https://codebeamer.daiichi.com/issue/197642
 *
 */
extern uint8_t g_app_avas_msg_pool[APP_AVAS_MSG_POOL_SIZE];

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief This function provides to initialize the api_audio component.
 *
 * https://codebeamer.daiichi.com/issue/195397
 *
 */
void app_avas_init(void);

/**
 * @brief This function provides to stop all audio module functions and mutes
 *        the amplifier.
 *
 * https://codebeamer.daiichi.com/issue/195399
 *
 */
void app_avas_deinit(void);

/**
 * @brief This function is responsible for managing audio files by handling
 *        operations such as frequency shifting, gain calculating,
 *        configuration table management, and managing buffers for
 *        audio samples.
 *
 * https://codebeamer.daiichi.com/issue/195398
 *
 */
void app_avas_run(void);

/**
 * @brief This function provides to takes messages to be sent from other
 *        applications.
 *
 * @param[out] p_arg Callback argument.
 *
 * https://codebeamer.daiichi.com/issue/195400
 *
 */
void app_avas_msg_handler(void *p_arg);

#endif /* PRIVATE_COMPONENT_A_MODULE_A_H */

// end of file
