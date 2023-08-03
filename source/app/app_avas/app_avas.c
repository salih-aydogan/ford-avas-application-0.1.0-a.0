// PRQA S 3108 EOF # Comments with links are required for ensure traceability.
/**
 * @file app_avas.c
 * @author Berke Yalcinkaya (berke.yalcinkaya@daiichi.com)
 * @brief The implementation of the AVAS Application interface.
 * @version 0.1
 * @date 2023-06-16
 *
 * @copyright Copyright (c) 2023
 *
 * https://codebeamer.daiichi.com/issue/196215
 *
 */

#include "app_avas.h"

#include "api_audio.h"
#include "appman.h"
#include "bsp_mcu.h"

#include "log.h"

LOG_MODULE_NAME("APP_AVAS");

/*******************************************************************************
 * Macros
 ******************************************************************************/

/**
 * @brief This macro holds the value that indicates the maximum number of times
 *        the function will be tried to call when a function fails.
 *
 * https://codebeamer.daiichi.com/issue/196261
 *
 */
#define AVAS_ERROR_ATTEMPTS (5U)

/**
 * @brief This macro holds the value that indicates the timeout time in
 *        milliseconds.
 *
 * https://codebeamer.daiichi.com/issue/196262
 *
 */
#define AVAS_ERROR_TIMEOUT_MS (100U)

/**
 * @brief This macro holds the information that timeout has not occurred.
 *
 * https://codebeamer.daiichi.com/issue/196263
 *
 */
#define TIMEOUT_NOT_OCCURED (0U)

/**
 * @brief This macro holds the information that the timeout occurred.
 *
 * https://codebeamer.daiichi.com/issue/196264
 *
 */
#define TIMEOUT_OCCURED (1U)

/**
 * @brief This macro holds the information that the audio file name size.
 *
 * https://codebeamer.daiichi.com/issue/196998
 *
 */
#define AUDIO_FILE_NAME_SIZE (33U)

/**
 * @brief This macro holds the information that the audio file version size.
 *
 * https://codebeamer.daiichi.com/issue/196999
 *
 */
#define AUDIO_FILE_VERSION_SIZE (5U)

/**
 * @brief This macro holds the information that the audio file calibration
 *        version size.
 *
 * https://codebeamer.daiichi.com/issue/197000
 *
 */
#define AUDIO_FILE_CALIB_VER_SIZE (5U)

/**
 * @brief This macro holds the information that the audio file calibration tool
 *        version size.
 *
 * https://codebeamer.daiichi.com/issue/197001
 *
 */
#define AUDIO_FILE_CALIB_TOOL_VER_SIZE (5U)

/**
 * @brief This macro holds the information that the audio file calibration date
 *        size.
 *
 * https://codebeamer.daiichi.com/issue/197002
 *
 */
#define AUDIO_FILE_CALIB_DATE_SIZE (17U)

/*******************************************************************************
 * Enumerations
 ******************************************************************************/

/**
 * @brief List of AVAS application's functions return status.
 *
 * https://codebeamer.daiichi.com/issue/198276
 * https://codebeamer.daiichi.com/issue/198277
 *
 */
// PRQA S 3213 ++ # The tag can not removed. False positive warning.
typedef enum avas_return_e
{
    AVAS_RETURN_ERROR = 0U,
    AVAS_RETURN_SUCCESS = 1U,
    AVAS_RETURN_BUSY = 2U,

} avas_return_t;

/**
 * @brief  List of AVAS application's process states.
 *
 * https://codebeamer.daiichi.com/issue/196265
 * https://codebeamer.daiichi.com/issue/196270
 *
 */
typedef enum avas_state_e
{
    AVAS_STATE_INIT = 0U,
    AVAS_STATE_DEINIT = 1U,
    AVAS_STATE_MUTE = 2U,
    AVAS_STATE_RUN = 3U,

} avas_state_t;

/*******************************************************************************
 * Structs
 ******************************************************************************/

/**
 * @brief The data structure corresponds to the AVAS application's message pool.
 *
 * https://codebeamer.daiichi.com/issue/196266
 * https://codebeamer.daiichi.com/issue/196271
 *
 */
typedef struct avas_msg_pool_s
// PRQA S 3213 --
{
    appman_msg_id_t msg_id;
    appman_app_id_t app_id;
    void *p_data;
    size_t size;

} avas_msg_pool_t;

/*******************************************************************************
 * Type Definitions
 ******************************************************************************/

/**
 * @brief This function pointer uses for registering the function that will be
 *        handled when an error occurs in the AVAS application's functions.
 *
 * https://codebeamer.daiichi.com/issue/196268
 *
 */
typedef avas_return_t (*generic_cb_t)(void);

/*******************************************************************************
 * Public Variables
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/197642
 *
 */
uint8_t g_app_avas_msg_pool[APP_AVAS_MSG_POOL_SIZE] = {0};

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/**
 * @brief Holds the AVAS module initialize flag.
 *
 * https://codebeamer.daiichi.com/issue/196272
 *
 */
static bool m_avas_is_init = false;

/**
 * @brief Holds the AVAS module mute flag.
 *
 * https://codebeamer.daiichi.com/issue/196273
 *
 */
static bool m_avas_is_mute = true;

/**
 * @brief Holds the AVAS module timeout flag.
 *
 * https://codebeamer.daiichi.com/issue/196274
 *
 */
static uint8_t m_timeout = TIMEOUT_NOT_OCCURED;

/**
 * @brief Holds the configuration table printing status.
 *
 * https://codebeamer.daiichi.com/issue/196993
 *
 */
static bool m_cfg_is_print = false;

/**
 * @brief Holds the AVAS module error timer structure.
 *
 * https://codebeamer.daiichi.com/issue/196275
 *
 */
static appman_timer_t m_err_timer = {

    .mode = APPMAN_TIMER_MODE_STRICT,
    .state = APPMAN_TIMER_STATE_INIT,
    .interval_ms = AVAS_ERROR_TIMEOUT_MS,
    .p_arg = &m_timeout,
};

/**
 * @brief Holds the AVAS application's current process state.
 *
 * https://codebeamer.daiichi.com/issue/198279
 *
 */
static avas_state_t m_avas_state = AVAS_STATE_DEINIT;

/*******************************************************************************
 * Private Function Declarations
 ******************************************************************************/

/**
 * @brief This function provides to take action when a timeout occurs.
 *
 * @param p_arg[out] Specifies the timeout flag.
 *
 * https://codebeamer.daiichi.com/issue/196280
 *
 */
static void timer_err_cb(void *p_arg);

/**
 * @brief This function provides to call failed functions as many times as the
 *        number of attempts.
 *
 * @param callback[in]    Function pointer.
 * @param attempt_cnt[in] Specifies the number of attempts.
 *
 * @return bool Function status.
 *
 * @retval true  If the attempted function is successful.
 * @retval false If the attempted function fails or callback argument is NULL
 *
 * https://codebeamer.daiichi.com/issue/196852
 *
 */
static avas_return_t avas_error_handler(generic_cb_t callback,
                                        uint32_t attempt_cnt);

/**
 * @brief This function provides to call failed functions as many times as the
 *        number of attempts. It waits until the timeout occurs.
 *
 * @param callback[in]    Function pointer.
 * @param attempt_cnt[in] Specifies the number of attempts.
 *
 * @return bool Function status.
 *
 * @retval true  If the attempted function is successful.
 * @retval false If the attempted function fails or callback argument is NULL
 *
 * https://codebeamer.daiichi.com/issue/196221
 *
 */
static bool avas_error_handler_blocking(generic_cb_t callback,
                                        uint32_t attempt_cnt);

/**
 * @brief This function provides inits audio with api_audio_init and starts
 *        audio playback with api_audio_play.
 *
 * @return avas_return_t Function status.
 *
 * @retval AVAS_RETURN_ERROR   Audio init or audio play fail.
 * @retval AVAS_RETURN_SUCCESS Audio init and audio play success.
 *
 * https://codebeamer.daiichi.com/issue/196220
 *
 */
static avas_return_t audio_init(void);

/**
 * @brief This function provides mute audio.
 *
 * @return avas_return_t Specifies the function status.
 *
 * @retval AVAS_RETURN_ERROR   Specifies the error state.
 * @retval AVAS_RETURN_SUCCESS Specifies the success state.
 *
 * https://codebeamer.daiichi.com/issue/196281
 *
 */
static avas_return_t audio_mute(void);

/**
 * @brief This function provides unmute audio.
 *
 * @return avas_return_t Specifies the function status.
 *
 * @retval AVAS_RETURN_ERROR   Specifies the error state.
 * @retval AVAS_RETURN_SUCCESS Specifies the success state.
 *
 * https://codebeamer.daiichi.com/issue/196282
 *
 */
static avas_return_t audio_unmute(void);

/**
 * @brief This function provides to check the AVAS module initialization flag.
 *
 * @return bool Function status.
 *
 * @retval true  Initialize successful.
 * @retval false Initialize fail.
 *
 * https://codebeamer.daiichi.com/issue/196853
 *
 */
static inline avas_return_t avas_init_check(void);

/**
 * @brief This function provides to check the AVAS module mute flag when the
 *        AVAS application is in the run state.
 *
 * @return bool Function status.
 *
 * @retval true  Initialize successful.
 * @retval false Initialize fail.
 *
 * https://codebeamer.daiichi.com/issue/196851
 *
 */
static inline avas_return_t avas_mute_check(void);

/**
 * @brief This function provides to check the AVAS vehicle speed.
 *
 * @param vehicle_speed[in] Vehicle speed.
 *
 * https://codebeamer.daiichi.com/issue/196285
 *
 */
static inline void avas_vehicle_speed_check(uint16_t vehicle_speed);

/**
 * @brief This function provides to print configuration table parameters related
 *        to audio files.
 *
 * https://codebeamer.daiichi.com/issue/197003
 *
 */
static inline void cfg_param_print(void);

/**
 * @brief
 *
 *
 *
 */
static inline void avas_state_manager(void);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/196218
 *
 */
void app_avas_init(void)
{
    appman_status_t status = APPMAN_STATUS_ERROR;

    m_avas_state = AVAS_STATE_INIT;

    m_err_timer.handler = timer_err_cb;

    appman_timer_create(&m_err_timer, &status);

    if (APPMAN_STATUS_SUCCESS != status)
    {
        LOG_ERROR("APPMAN Timer could not be created");
    }

    avas_return_t init_status = audio_init();

    if (AVAS_RETURN_ERROR == init_status)
    {
        m_avas_is_init = avas_error_handler_blocking(audio_init,
                                                     AVAS_ERROR_ATTEMPTS);

        if (false == m_avas_is_init)
        {
            appman_timer_start(&m_err_timer, &status);

            LOG_FATAL("Audio could not be initialized");
        }
    }
    else
    {
        m_avas_state = AVAS_STATE_RUN;

        avas_return_t mute_status = audio_unmute();

        m_avas_is_mute = false;

        if (AVAS_RETURN_ERROR == mute_status)
        {
            m_avas_is_mute = true;

            appman_timer_start(&m_err_timer, &status);

            LOG_ERROR("Audio unmute fail");
        }
    }
}

/**
 * @brief https://codebeamer.daiichi.com/issue/196219
 *
 */
void app_avas_deinit(void)
{
    api_audio_deinit();

    appman_timer_stop(&m_err_timer, NULL);

    m_timeout = TIMEOUT_NOT_OCCURED;

    m_avas_is_init = false;

    m_avas_is_mute = true;

    avas_return_t status = audio_mute();

    if (AVAS_RETURN_ERROR == status)
    {
        bool ret = avas_error_handler_blocking(audio_mute, AVAS_ERROR_ATTEMPTS);

        if (false == ret)
        {
            m_avas_is_mute = false;

            LOG_ERROR("Failed to mute after %u attempts", AVAS_ERROR_ATTEMPTS);
        }
    }

    m_avas_state = AVAS_STATE_DEINIT;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/196217
 *
 */
void app_avas_run(void)
{
    cfg_param_print();

    avas_state_manager();
}

/**
 * @brief https://codebeamer.daiichi.com/issue/196216
 *
 */
// PRQA S 3673 ++ # Since the function will be saved in appman, its p_arg argument cannot be made const.
void app_avas_msg_handler(void *p_arg)
// PRQA S 3673 --
{
    const avas_msg_pool_t *p_msg = (const avas_msg_pool_t *)p_arg;

    appman_msg_id_t msg_id = p_msg->msg_id;

    if (APPMAN_MSG_ID_VEHICLE_SPEED == msg_id)
    {
        const uint16_t *p_data = (const uint16_t *)p_msg->p_data;

        avas_vehicle_speed_check(*p_data);
    }

    if (APPMAN_MSG_ID_CURR_AUDIO == msg_id)
    {
        const uint8_t *p_data = (const uint8_t *)p_msg->p_data;

        bool ret = api_audio_switch(*p_data);

        if (false == ret)
        {
            LOG_ERROR("The audio could not be changed.");
        }

        m_cfg_is_print = false;
    }

    if (APPMAN_MSG_ID_AUDIO_MUTE_STATUS == msg_id)
    {
        uint8_t tmp = *((uint8_t *)p_msg->p_data);

        m_avas_is_mute = (1U == tmp);

        if (m_avas_is_mute)
        {
            m_avas_state = AVAS_STATE_MUTE;
        }
    }
}

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/196280
 *
 */
static void timer_err_cb(void *p_arg)
{
    if (NULL != p_arg)
    {
        uint8_t *p_tmp = (uint8_t *)p_arg;

        *p_tmp = TIMEOUT_OCCURED;
    }
}

/**
 * @brief https://codebeamer.daiichi.com/issue/196852
 *
 */
static avas_return_t avas_error_handler(generic_cb_t callback,
                                        uint32_t attempt_cnt)
{
    static uint8_t attempt = 0U;

    if (attempt_cnt <= attempt)
    {
        attempt = 0U;

        appman_timer_stop(&m_err_timer, NULL);

        return AVAS_RETURN_ERROR;
    }

    if (TIMEOUT_NOT_OCCURED == m_timeout)
    {
        return AVAS_RETURN_BUSY;
    }

    avas_return_t status = callback();

    attempt++;

    if (AVAS_RETURN_ERROR != status)
    {
        attempt = 0U;

        appman_timer_stop(&m_err_timer, NULL);
    }

    m_timeout = TIMEOUT_NOT_OCCURED;

    return status;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/196221
 *
 */
static bool avas_error_handler_blocking(generic_cb_t callback,
                                        uint32_t attempt_cnt)
{
    uint32_t cnt = 0U;

    bool loop_flag = false;

    appman_status_t timer_status = APPMAN_STATUS_ERROR;

    appman_timer_start(&m_err_timer, &timer_status);

    if (APPMAN_STATUS_SUCCESS != timer_status)
    {
        return false;
    }

    while ((cnt < attempt_cnt) &&
           (false == loop_flag))
    {
        if (TIMEOUT_OCCURED == m_timeout)
        {
            avas_return_t status = callback();

            if (AVAS_RETURN_SUCCESS == status)
            {
                loop_flag = true;
            }

            m_timeout = TIMEOUT_NOT_OCCURED;

            cnt++;
        }
    }

    appman_timer_stop(&m_err_timer, NULL);

    return loop_flag;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/196220
 *
 */
static avas_return_t audio_init(void)
{
    bool ret = api_audio_init();

    if (false == ret)
    {
        return AVAS_RETURN_ERROR;
    }

    if (false == api_audio_status_get())
    {
        ret = api_audio_play();

        if (false == ret)
        {
            LOG_ERROR("AUDIO buffers are not ready");
        }
    }

    return AVAS_RETURN_SUCCESS;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/196281
 *
 */
static avas_return_t audio_mute(void)
{
    bool ret = api_audio_mute();

    if (false == ret)
    {
        return AVAS_RETURN_ERROR;
    }

    return AVAS_RETURN_SUCCESS;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/196282
 *
 */
static avas_return_t audio_unmute(void)
{
    bool ret = api_audio_unmute();

    if (false == ret)
    {
        return AVAS_RETURN_ERROR;
    }

    return AVAS_RETURN_SUCCESS;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/196853
 *
 */
static inline avas_return_t avas_init_check(void)
{
    avas_return_t status = AVAS_RETURN_SUCCESS;

    if (false == m_avas_is_init)
    {
        status = avas_error_handler(audio_init,
                                    AVAS_ERROR_ATTEMPTS);

        m_avas_is_init = (AVAS_RETURN_SUCCESS == status) ? true : false;
    }

    return status;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/196851
 *
 */
static inline avas_return_t avas_mute_check(void)
{
    avas_return_t status = AVAS_RETURN_SUCCESS;

    if (true == m_avas_is_mute)
    {
        status = avas_error_handler(audio_unmute,
                                    AVAS_ERROR_ATTEMPTS);

        m_avas_is_mute = (AVAS_RETURN_SUCCESS == status) ? false : true;
    }

    return status;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/196285
 *
 */
static inline void avas_vehicle_speed_check(uint16_t vehicle_speed)
{
    static uint16_t speed = 0U;

    if (speed != vehicle_speed)
    {
        api_audio_param_update(vehicle_speed);

        LOG_DEBUG("Vehicle speed: %u", vehicle_speed);

        speed = vehicle_speed;

        uint16_t tmp = 0U;

        bool ret = api_audio_gain_get(&tmp, vehicle_speed);

        if (true == ret)
        {
            LOG_DEBUG("Audio Gain: %u", tmp);
        }

        ret = api_audio_freq_get(&tmp, vehicle_speed);

        if (true == ret)
        {
            LOG_DEBUG("Audio Frequency: %u", tmp);
        }
    }
}

/**
 * @brief https://codebeamer.daiichi.com/issue/197003
 *
 */
static inline void cfg_param_print(void)
{
    if (true == m_cfg_is_print)
    {
        return;
    }

    char_t audio_file_name[AUDIO_FILE_NAME_SIZE];

    bool ret = api_audio_name_get(audio_file_name);

    if (false == ret)
    {
        return;
    }

    LOG_INFO("Audio name: %s", audio_file_name);

    char_t audio_file_ver[AUDIO_FILE_VERSION_SIZE] = {0};
    char_t audio_file_calib_ver[AUDIO_FILE_CALIB_VER_SIZE] = {0};
    char_t audio_file_calib_tool_ver[AUDIO_FILE_CALIB_TOOL_VER_SIZE] = {0};
    char_t audio_file_calib_date[AUDIO_FILE_CALIB_DATE_SIZE] = {0};

    (void)api_audio_ver_get(audio_file_ver);

    LOG_INFO("Audio file version: %s", audio_file_ver);

    (void)api_audio_calib_ver_get(audio_file_calib_ver);

    LOG_INFO("Audio calibration version: %s", audio_file_calib_ver);

    (void)api_audio_calib_tool_get(audio_file_calib_tool_ver);

    LOG_INFO("Audio calibration tool version: %s", audio_file_calib_tool_ver);

    (void)api_audio_calib_date_get(audio_file_calib_date);

    LOG_INFO("Calibration date: %s", audio_file_calib_date);

    m_cfg_is_print = true;
}

/**
 * @brief Handles App Avas States.
 *
 *        In Init state, if init has failed, retries init operation.
 *        In Mute state, performs and retries mute operation.
 *        In Run state, executes normal api audio process and unmutes if needed.
 *
 * https://codebeamer.daiichi.com/issue/198317
 *
 */
static inline void avas_state_manager(void)
{
    static avas_state_t log_state = 0xFFU;

    if (log_state != m_avas_state)
    {
        LOG_DEBUG("App Avas Process, Prev state: %u, New state: %u", 
                   log_state,
                   m_avas_state);

        log_state = m_avas_state;
    }

    switch (m_avas_state)
    {
    case AVAS_STATE_INIT:
    {
        avas_return_t init_status = avas_init_check();

        if (AVAS_RETURN_ERROR == init_status)
        {
            LOG_NOW(LOG_LEVEL_FATAL,
                    "Failed to init avas after %u attempts. MCU will be reset",
                    AVAS_ERROR_ATTEMPTS);

            bsp_mcu_reset();
        }
        else
        {
            m_avas_state = AVAS_STATE_RUN;
        }
    }
    break;

    case AVAS_STATE_MUTE:
    {
        if (false == m_avas_is_mute)
        {
            avas_return_t mute_status = avas_error_handler(audio_mute,
                                                           AVAS_ERROR_ATTEMPTS);

            if (AVAS_RETURN_SUCCESS == mute_status)
            {
                m_avas_is_mute = true;

                m_avas_state =
                    (false == m_avas_is_init) ? AVAS_STATE_INIT : AVAS_STATE_RUN;
            }
            else
            {
                m_avas_state = AVAS_STATE_MUTE;
            }
        }
    }
    break;

    case AVAS_STATE_RUN:
    {
        avas_return_t mute_status = avas_mute_check();

        if (AVAS_RETURN_ERROR == mute_status)
        {
            LOG_ERROR("Failed to init avas after %u attempts.",
                      AVAS_ERROR_ATTEMPTS);
        }

        api_audio_process();
    }
    break;

    default:
    {
        LOG_ERROR("AVAS state in deinit");
    }
    break;
    }
}

// end of line
