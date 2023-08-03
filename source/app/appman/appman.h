// PRQA S 3108 EOF # Comments with links are required for ensure traceability.

/**
 * @file appman.h
 * @author berke yalcinkaya (berke.yalcinkaya@daiichi.com)
 * @brief Handles application, also timers and communication used by 
 *        applications.
 * @version 0.1
 * @date 2023-03-15
 * 
 * @copyright Copyright (c) 2023
 *
 * https://codebeamer.daiichi.com/issue/185052
 * https://codebeamer.daiichi.com/issue/177741
 * 
 */

#ifndef APPMAN_H
#define APPMAN_H

#include "daiichi_types.h"

/*******************************************************************************
 * Type Definitions
 ******************************************************************************/

/**
 * @brief This function pointer uses for registering functions of applications.
 * 
 * https://codebeamer.daiichi.com/issue/185062
 * 
 */
typedef void (*appman_app_handler_t)(void);

/**
 * @brief This function pointer uses for registering the function that will be 
 *        handled when timeout occurs in the created timer.
 * 
 * https://codebeamer.daiichi.com/issue/185063
 * 
 */
typedef void (*appman_timer_handler_t)(void *p_arg);

/**
 * @brief This function pointer uses for registering the function that used to 
 *        pass a message to an application.
 * 
 * https://codebeamer.daiichi.com/issue/185484
 * 
 */
typedef appman_timer_handler_t appman_msg_handler_t;

/*******************************************************************************
 * Enumerations
 ******************************************************************************/
/**
 * @brief List of appman status
 * 
 * https://codebeamer.daiichi.com/issue/193295
 * 
 */
typedef enum appman_status_e
{
    APPMAN_STATUS_SUCCESS = 0x00U,
    APPMAN_STATUS_APP_NOT_CREATED = 0x10U,
    APPMAN_STATUS_APP_MSG_POOL_ERROR = 0x11U,
    APPMAN_STATUS_TIMER_NOT_CREATED = 0x20U,
    APPMAN_STATUS_NO_MSG_INBOX = 0x30U,
    APPMAN_STATUS_NO_MSG_POOL = 0x31U,
    APPMAN_STATUS_MSG_POOL_INVALID_SIZE = 0x32U,
    APPMAN_STATUS_ERROR = 0x40U,
    APPMAN_STATUS_ERROR_PARAM = 0x41U,

} appman_status_t;

/**
 * @brief List of possible timer modes.
 * 
 * https://codebeamer.daiichi.com/issue/185067
 * 
 */
typedef enum appman_timer_mode_e
{
    APPMAN_TIMER_MODE_STRICT = 0U,
    APPMAN_TIMER_MODE_SEQUANTIAL = 1U,

} appman_timer_mode_t;

/**
 * @brief List of possible timer states.
 * 
 * https://codebeamer.daiichi.com/issue/185068
 * 
 */
typedef enum appman_timer_state_e
{
    APPMAN_TIMER_STATE_INIT = 0U,
    APPMAN_TIMER_STATE_RUN = 1U,
    APPMAN_TIMER_STATE_STOP = 2U,

} appman_timer_state_t;

/**
 * @brief List of possible application id.
 * 
 * https://codebeamer.daiichi.com/issue/185069
 * 
 */
typedef enum appman_app_id_e
{
    APPMAN_APP_ID_MAIN = 0U,
    APPMAN_APP_ID_AVAS = 1U,
    APPMAN_APP_ID_DIAG = 2U,
    APPMAN_APP_ID_CAN = 3U,
    APPMAN_APP_ID_ALL = 99U,

} appman_app_id_t;

/**
 * @brief List of possible message ids between applications.
 * 
 * https://codebeamer.daiichi.com/issue/193253
 * 
 */
typedef enum appman_msg_id_e
{
    APPMAN_MSG_ID_SYSTEM_STATUS = 0U,
    APPMAN_MSG_ID_VEHICLE_SPEED = 1U,
    APPMAN_MSG_ID_TRANS_GEAR = 2U,
    APPMAN_MSG_ID_GEAR = 3U,    
    APPMAN_MSG_ID_CURR_AUDIO = 4U,
    APPMAN_MSG_ID_AUDIO_MUTE_STATUS = 5U,

    APPMAN_MSG_ID_SPEAKER_1_OPEN_CIRCUIT = 100U,
    APPMAN_MSG_ID_SPEAKER_1_SHORT_TO_PWR = 101U,
    APPMAN_MSG_ID_SPEAKER_1_SHORT_TO_GND = 102U,
    APPMAN_MSG_ID_SPEAKER_2_OPEN_CIRCUIT = 103U,
    APPMAN_MSG_ID_SPEAKER_2_SHORT_TO_PWR = 104U,
    APPMAN_MSG_ID_SPEAKER_2_SHORT_TO_GND = 105U,
    APPMAN_MSG_ID_CAN_COMM_ERR_VCU = 106U,
    APPMAN_MSG_ID_INVALID_DATA_FROM_VCU = 107U,
    APPMAN_MSG_ID_CTRL_MODULE_CHECKSUM_FAIL = 108U,
    APPMAN_MSG_ID_CTRL_MODULE_MEM_FAIL = 109U,
    APPMAN_MSG_ID_CTRL_MODULE_COMP_FAIL = 110U,
    APPMAN_MSG_ID_BAT_LOW = 111U,
    APPMAN_MSG_ID_BAT_HIGH = 112U,

} appman_msg_id_t;

/*******************************************************************************
 * Structs
 ******************************************************************************/

/**
 * @brief Data structure to holds application attributes. The p_msg_pool pointer
 *        must point to space which is a multiple of the structure size with two
 *        pointers, one enum, and one variable of type size_t. In this MCU, the 
 *        specified structure occupies 16 bytes from memory. The msg_pool_size 
 *        variable indicates the total number of bytes of the message pool. 
 *        For example, if a 5 element pool is created, msg_pool_size takes
 *        the value 5 * 16.
 * 
 * https://codebeamer.daiichi.com/issue/193278
 * 
 */
typedef struct appman_app_s
{
    const char *p_name;

    appman_app_id_t app_id;

    appman_app_handler_t init;
    appman_app_handler_t run;
    appman_app_handler_t deinit;

    appman_msg_handler_t msg_handler;

    void *p_msg_pool;
    void *p_msg_inbox;           ///< It cannot be modified.
    size_t msg_pool_size;

    struct appman_app_s *p_next; ///< It cannot be modified.

} appman_app_t;

/**
 * @brief Data structure to holds timer attributes.
 * 
 * https://codebeamer.daiichi.com/issue/185075
 * 
 */
typedef struct appman_timer_s
{
    appman_timer_handler_t handler;
    appman_timer_mode_t mode;
    appman_timer_state_t state;
    uint32_t interval_ms;
    volatile uint32_t remain_ms;   ///< It cannot be modified.
    void *p_arg;
    struct appman_timer_s *p_next; ///< It cannot be modified.

} appman_timer_t;

/**
 * @brief Data structure to holds message attributes.
 * 
 * https://codebeamer.daiichi.com/issue/193257
 * 
 */
typedef struct appman_msg_s
{
    appman_app_id_t dest;
    appman_msg_id_t msg_id;
    void *p_data;
    size_t size;

} appman_msg_t;

/*******************************************************************************
 * Public Variables
 ******************************************************************************/

/**
 * @brief This function allows to create an application in appman.
 * 
 * @param[in] p_app     Created application structure.
 * @param[out] p_status State of application creation.
 * 
 * https://codebeamer.daiichi.com/issue/185091
 * 
 */
void appman_app_create(appman_app_t *p_app, appman_status_t *p_status);

/**
 * @brief This function creates a timer in appman.
 * 
 * @param[in] p_timer   Created timer structure.
 * @param[out] p_status State of application creation.
 * 
 * https://codebeamer.daiichi.com/issue/185092
 * 
 */
void appman_timer_create(appman_timer_t *p_timer, appman_status_t *p_status);

/**
 * @brief This function starts the timer created by the application in the 
 *        appman.
 * 
 * @param[in] p_timer   Timer structure to be started.
 * @param[out] p_status State of application creation.
 * 
 * https://codebeamer.daiichi.com/issue/185093
 * 
 */
void appman_timer_start(appman_timer_t *p_timer, appman_status_t *p_status);

/**
 * @brief This function stops the timer created by the application in the 
 *        appman.
 * 
 * @param[in] p_timer   Timer structure to be stopped.
 * @param[out] p_status State of application creation.
 * 
 * https://codebeamer.daiichi.com/issue/185094
 * 
 */
void appman_timer_stop(appman_timer_t *p_timer, appman_status_t *p_status);

/**
 * @brief This function uses to send a message from one application to another 
 *        application or to all applications.
 * 
 * @param[in] p_msg     Sent message structure.
 * @param[out] p_status State of message sending.
 * 
 * https://codebeamer.daiichi.com/issue/185095
 * 
 */
void appman_msg_send(const appman_msg_t *p_msg, appman_status_t *p_status);

/**
 * @brief This function allows the necessary initial settings to be made.
 * 
 * https://codebeamer.daiichi.com/issue/185097
 * 
 */
void appman_init(void);

/**
 * @brief This function manages the created applications.
 * 
 * https://codebeamer.daiichi.com/issue/185098
 * 
 */
void appman_process(void);

#endif /* APPMAN_H */

// end of file
