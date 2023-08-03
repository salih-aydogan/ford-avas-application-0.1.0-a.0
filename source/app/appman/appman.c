// PRQA S 3108 EOF # Comments with links are required for ensure traceability.

/**
 * @file appman.c
 * @author berke yalcinkaya (berke.yalcinkaya@daiichi.com)
 * @brief This module handles application, also timers and communication used by
 *        applications.
 * @version 0.1
 * @date 2023-03-15
 *
 * @copyright Copyright (c) 2023
 *
 * https://codebeamer.daiichi.com/issue/186078
 *
 */

#include "appman.h"

#include "bsp_isr.h"
#include "bsp_mcu.h"

#include "log.h"

LOG_MODULE_NAME("APPMAN");

/*******************************************************************************
 * Enumerations
 ******************************************************************************/

/**
 * @brief List of process states.
 *
 * https://codebeamer.daiichi.com/issue/186157
 *
 */
// PRQA S 3213 ++ # process_state_e cannot remove.
typedef enum process_state_e
{
    // PRQA S 3213 --
    PROCESS_STATE_APP_INIT = 0U,
    PROCESS_STATE_APP_RUN = 1U,

} process_state_t;

/*******************************************************************************
 * Structs
 ******************************************************************************/

/**
 * @brief Data structure corresponds to the application's message pool.
 *
 * https://codebeamer.daiichi.com/issue/186156
 *
 */
typedef struct msg_pool_s
{
    appman_msg_id_t msg_id;
    void *p_data;
    size_t size;
    struct msg_pool_s *p_next;

} msg_pool_t;

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/**
 * @brief Holds process state.
 *
 * https://codebeamer.daiichi.com/issue/186160
 *
 */
static process_state_t m_process_state = PROCESS_STATE_APP_INIT;

/**
 * @brief Holds the address of the created application structures.
 *
 * https://codebeamer.daiichi.com/issue/186161
 *
 */
static appman_app_t *mp_app_list = NULL;

/**
 * @brief Holds the address of the created application structures. Provides to
 *        travel of application list.
 *
 * https://codebeamer.daiichi.com/issue/187068
 *
 */
static appman_app_t *mp_app_list_idx = NULL;

/**
 * @brief Holds the address of the created timer structures.
 *
 * https://codebeamer.daiichi.com/issue/186163
 *
 */
static appman_timer_t *mp_timer_list = NULL;

/**
 * @brief Holds the current application to be processed.
 *
 * https://codebeamer.daiichi.com/issue/193336
 *
 */
static appman_app_id_t m_curr_app;

/*******************************************************************************
 * Private Function Declarations
 ******************************************************************************/

/**
 * @brief Compares the id of the applications created with the given id.
 *
 * @param[in]  app_id   Application id.
 * @param[out] p_status Function status.
 *
 * @return appman_app_t* Address of application.
 *
 * https://codebeamer.daiichi.com/issue/186090
 *
 */
static appman_app_t *app_get(appman_app_id_t app_id,
                             appman_status_t *p_status);

/**
 * @brief It finds the timer address given as an argument from the timer list
 *        and returns the found address.
 *
 * @param[in]  p_timer  Created timer address.
 * @param[out] p_status Function status.
 *
 * @return appman_timer_t* Address of timer.
 *
 * https://codebeamer.daiichi.com/issue/186084
 *
 */
static appman_timer_t *timer_get(const appman_timer_t *p_timer,
                                 appman_status_t *p_status);

/**
 * @brief Adds the given application to the end of the application list.
 *
 * @param[in] p_item Application address.
 *
 * https://codebeamer.daiichi.com/issue/186085
 *
 */
static void app_add(appman_app_t *p_item);

/**
 * @brief Adds the given timer to the end of the timer list.
 *
 * @param[in] p_item Timer address.
 *
 * https://codebeamer.daiichi.com/issue/186097
 *
 */
static void timer_add(appman_timer_t *p_item);

/**
 * @brief Finds the free message pool of the given applicaton.
 *
 * @param[in]  p_app    Application structure address.
 * @param[out] p_status Function status.
 *
 * @return msg_pool_t* Address of free message pool field.
 *
 * https://codebeamer.daiichi.com/issue/186082
 *
 */
static msg_pool_t *msg_pool_get(appman_app_t *p_app,
                                appman_status_t *p_status);

/**
 * @brief Finds the message in the message inbox of the given application.
 *
 * @param[in]  p_app   Application structure address.
 * @param[out] p_inbox Application message inbox pointer address.
 *
 * @return appman_status_t Function status.
 *
 * @retval APPMAN_STATUS_SUCCESS     Success.
 * @retval APPMAN_STATUS_MSG_Q_EMPTY There is no message in message inbox.
 *
 * https://codebeamer.daiichi.com/issue/186094
 *
 */
static appman_status_t msg_inbox_get(appman_app_t *p_app,
                                     msg_pool_t **pp_inbox);

/**
 * @brief Adds the given message to the message inbox of the given application.
 *
 * @param[in]  p_msg The address taken from the free space and filled in.
 * @param[out] p_app Application structure address.
 *
 * https://codebeamer.daiichi.com/issue/186091
 *
 */
static void msg_inbox_add(msg_pool_t *p_msg,
                          appman_app_t *p_app);

/**
 * @brief Gives back the message part used in the inbox to the free message
 *        pool.
 *
 * @param[in]  p_msg Message inbox address to be give back.
 * @param[out] p_app Application structure address.
 *
 * https://codebeamer.daiichi.com/issue/186355
 *
 */
static void msg_pool_give(msg_pool_t *p_msg,
                          appman_app_t *p_app);

/**
 * @brief This function connects application message pool sections.
 *
 * https://codebeamer.daiichi.com/issue/186095
 *
 */
static void msg_pool_init(appman_app_t *p_app, appman_status_t *p_status);

/**
 * @brief Sends a message by one application to all applications.
 *
 * @param[in]  p_msg    The address of the message structure to be sent.
 * @param[out] p_status Function status.
 *
 * https://codebeamer.daiichi.com/issue/186098
 *
 */
static void all_app_msg_send(const appman_msg_t *p_msg,
                             appman_status_t *p_status);

/**
 * @brief This function updates the timeout counter of the timers.
 *
 * @param[out] p_timer Timer address.
 *
 * @return appman_status_t Indicates the success or unsuccess of the timer
 *                         counter update.
 *
 * @retval APPMAN_STATUS_SUCCESS     Success.
 * @retval APPMAN_STATUS_ERROR_PARAM Mode variable incorrect.
 *
 * https://codebeamer.daiichi.com/issue/186354
 *
 */
static appman_status_t timer_counter_update(appman_timer_t *p_timer);

/**
 * @brief Manages the timers created sequantially.
 *
 * https://codebeamer.daiichi.com/issue/186080
 *
 */
static void timer_exec(void);

/**
 * @brief Calls the handler of the timer if the timeout condition is met.
 *
 * https://codebeamer.daiichi.com/issue/186088
 *
 */
static void timer_cb(void);

/**
 * @brief This function receives a message in the message inbox of the relevant
 *        application.
 *
 * https://codebeamer.daiichi.com/issue/186081
 *
 */
static void msg_exec(void);

/**
 * @brief Manages aplications, timers and messaging.
 *
 * https://codebeamer.daiichi.com/issue/186086
 *
 */
static void app_exec(void);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/186089
 *
 */
void appman_app_create(appman_app_t *p_app, appman_status_t *p_status)
{
    appman_status_t status = APPMAN_STATUS_SUCCESS;

    if ((NULL == p_app) ||
        (NULL == p_app->p_name) ||
        (NULL == p_app->init) ||
        (NULL == p_app->run) ||
        (NULL == p_app->deinit))
    {
        status = APPMAN_STATUS_ERROR_PARAM;

        LOG_ERROR("Invalid argument in appman_app_create");
    }
    else
    {
        app_add(p_app);

        msg_pool_init(p_app, &status);

        LOG_INFO("%s app created; id:%u", 
                 p_app->p_name, 
                 (uint32_t)p_app->app_id);
    }

    if (NULL != p_status)
    {
        *p_status = status;
    } 
}

/**
 * @brief https://codebeamer.daiichi.com/issue/186092
 *
 */
void appman_timer_create(appman_timer_t *p_timer, appman_status_t *p_status)
{
    appman_status_t status = APPMAN_STATUS_SUCCESS;

    if ((NULL == p_timer) ||
        (NULL == p_timer->handler) ||
        (0U == p_timer->interval_ms))
    {
        status = APPMAN_STATUS_ERROR_PARAM;

        LOG_ERROR("Invalid argument in appman_timer_create");
    }
    else
    {
        timer_add(p_timer);

        LOG_INFO("Timer created; handler: %u, interval:%u",
                 (uint32_t)p_timer->handler,
                 p_timer->interval_ms);

        if (APPMAN_TIMER_STATE_RUN == p_timer->state)
        {
            status = timer_counter_update(p_timer);

            LOG_INFO("Timer started; handler: %u", (uint32_t)p_timer->handler);
        }
        else
        {
            p_timer->state = APPMAN_TIMER_STATE_INIT;
        }
    }

    if (NULL != p_status)
    {
        *p_status = status;
    }
}

// PRQA S 3673 ++ # p_timer can not be const because the state element of p_timer changed. A warning appears because timers are not created from main.
/**
 * @brief https://codebeamer.daiichi.com/issue/186087
 *
 */
void appman_timer_start(appman_timer_t *p_timer, appman_status_t *p_status)
{
    // PRQA S 3673 --
    appman_status_t status = APPMAN_STATUS_ERROR;

    if (NULL == p_timer)
    {
        status = APPMAN_STATUS_ERROR_PARAM;

        LOG_ERROR("Invalid argument in appman_timer_start");
    }
    else
    {
        appman_timer_t *p_tmp_timer = timer_get(p_timer, &status);

        if ((APPMAN_STATUS_SUCCESS == status) &&
            (APPMAN_TIMER_STATE_RUN != p_tmp_timer->state))
        {
            status = timer_counter_update(p_tmp_timer);

            p_tmp_timer->state = APPMAN_TIMER_STATE_RUN;

            LOG_DEBUG("Timer started; handler: %u", (uint32_t)p_timer->handler);
        }
    }

    if (NULL != p_status)
    {
        *p_status = status;
    }
}

// PRQA S 3673 ++ # p_timer can not be const because the state element of p_timer changed. A warning appears because timers are not created from main.
/**
 * @brief https://codebeamer.daiichi.com/issue/186093
 *
 */
void appman_timer_stop(appman_timer_t *p_timer, appman_status_t *p_status)
{
    // PRQA S 3673 --
    appman_status_t status = APPMAN_STATUS_ERROR;

    if (NULL == p_timer)
    {
        status = APPMAN_STATUS_ERROR_PARAM;

        LOG_ERROR("Invalid argument in appman_timer_stop");
    }
    else
    {
        appman_timer_t *p_tmp_timer = timer_get(p_timer, &status);

        if (APPMAN_STATUS_SUCCESS == status)
        {
            p_tmp_timer->state = APPMAN_TIMER_STATE_STOP;

            LOG_DEBUG("Timer stopped");
        }
    }

    if (NULL != p_status)
    {
        *p_status = status;
    }
}

/**
 * @brief https://codebeamer.daiichi.com/issue/186079
 *
 */
void appman_msg_send(const appman_msg_t *p_msg, appman_status_t *p_status)
{
    appman_status_t status = APPMAN_STATUS_ERROR;

    if ((NULL == p_msg) ||
        (NULL == p_msg->p_data))
    {
        status = APPMAN_STATUS_ERROR_PARAM;

        if (NULL != p_status)
        {
            *p_status = status;
        }
        else
        {
            LOG_ERROR("Invalid argument in appman_msg_send");
        }

        return;
    }

    appman_app_id_t app_id = p_msg->dest;

    if (APPMAN_APP_ID_ALL == app_id)
    {
        LOG_DEBUG("Msg broadcast; id:%u", p_msg->msg_id);

        all_app_msg_send(p_msg, &status);

        if (NULL != p_status)
        {
            *p_status = status;
        }

        return;
    }

    appman_app_t *p_app = app_get(app_id, &status);

    if (APPMAN_STATUS_SUCCESS == status)
    {
        msg_pool_t *p_free_msg_section = msg_pool_get(p_app, &status);

        if (APPMAN_STATUS_NO_MSG_POOL != status)
        {
            p_free_msg_section->msg_id = p_msg->msg_id;
            p_free_msg_section->p_data = p_msg->p_data;
            p_free_msg_section->size = p_msg->size;
            p_free_msg_section->p_next = NULL;

            msg_inbox_add(p_free_msg_section, p_app);

            LOG_DEBUG("Message send; app:%s, id:%u", 
                      p_app->p_name,  
                      p_msg->msg_id);
        }
    }
    else
    {
        LOG_ERROR("Unknown app_id received in appman_msg_send");
    }

    if (NULL != p_status)
    {
        *p_status = status;
    }
}

/**
 * @brief https://codebeamer.daiichi.com/issue/186083
 *
 */
void appman_init(void)
{
    bsp_isr_cb_set(BSP_ISR_SYSTICK, timer_cb);

    m_process_state = PROCESS_STATE_APP_INIT;

    mp_app_list = NULL;

    mp_app_list_idx = NULL;

    mp_timer_list = NULL;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/186096
 *
 */
void appman_process(void)
{
    app_exec();

    msg_exec();

    timer_exec();
}

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/186090
 *
 */
static appman_app_t *app_get(appman_app_id_t app_id,
                             appman_status_t *p_status)
{
    appman_app_t *p_tmp_app = mp_app_list;

    *p_status = APPMAN_STATUS_APP_NOT_CREATED;

    while (NULL != p_tmp_app)
    {
        if (app_id == p_tmp_app->app_id)
        {
            *p_status = APPMAN_STATUS_SUCCESS;

            break;
        }

        p_tmp_app = p_tmp_app->p_next;
    }

    return p_tmp_app;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/186084
 *
 */
static appman_timer_t *timer_get(const appman_timer_t *p_timer,
                                 appman_status_t *p_status)
{
    appman_timer_t *p_tmp_timer = mp_timer_list;

    *p_status = APPMAN_STATUS_TIMER_NOT_CREATED;

    while (NULL != p_tmp_timer)
    {
        if (p_timer == p_tmp_timer)
        {
            *p_status = APPMAN_STATUS_SUCCESS;

            break;
        }

        p_tmp_timer = p_tmp_timer->p_next;
    }

    return p_tmp_timer;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/186085
 *
 */
static void app_add(appman_app_t *p_item)
{
    if (NULL == mp_app_list)
    {
        mp_app_list = p_item;

        mp_app_list->p_next = NULL;

        mp_app_list_idx = mp_app_list;

        return;
    }

    appman_app_t *p_tmp_app = mp_app_list;

    while (NULL != p_tmp_app->p_next)
    {
        p_tmp_app = p_tmp_app->p_next;
    }

    p_tmp_app->p_next = p_item;

    p_item->p_next = NULL;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/186097
 *
 */
static void timer_add(appman_timer_t *p_item)
{
    if (NULL == mp_timer_list)
    {
        mp_timer_list = p_item;

        mp_timer_list->p_next = NULL;

        return;
    }

    appman_timer_t *p_tmp_timer = mp_timer_list;

    while (NULL != p_tmp_timer->p_next)
    {
        p_tmp_timer = p_tmp_timer->p_next;
    }

    p_tmp_timer->p_next = p_item;

    p_item->p_next = NULL;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/186082
 *
 */
static msg_pool_t *msg_pool_get(appman_app_t *p_app,
                                appman_status_t *p_status)
{
    msg_pool_t *p_tmp_msg_pool = (msg_pool_t *)p_app->p_msg_pool;

    if (NULL == p_tmp_msg_pool)
    {
        *p_status = APPMAN_STATUS_NO_MSG_POOL;

        LOG_ERROR("There is no available MSG POOL");

        return NULL;
    }

    p_app->p_msg_pool = p_tmp_msg_pool->p_next;

    return p_tmp_msg_pool;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/186094
 *
 */
static appman_status_t msg_inbox_get(appman_app_t *p_app,
                                     msg_pool_t **pp_inbox)
{
    if (NULL == p_app->p_msg_inbox)
    {
        return APPMAN_STATUS_NO_MSG_INBOX;
    }

    *pp_inbox = p_app->p_msg_inbox;

    p_app->p_msg_inbox = (*pp_inbox)->p_next;

    return APPMAN_STATUS_SUCCESS;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/186091
 *
 */
static void msg_inbox_add(msg_pool_t *p_msg,
                          appman_app_t *p_app)
{
    msg_pool_t *p_tmp_msg_inbox = (msg_pool_t *)p_app->p_msg_inbox;

    if (NULL == p_tmp_msg_inbox)
    {
        p_app->p_msg_inbox = p_msg;

        p_msg->p_next = NULL;

        return;
    }

    while (NULL != p_tmp_msg_inbox->p_next)
    {
        p_tmp_msg_inbox = p_tmp_msg_inbox->p_next;
    }

    p_tmp_msg_inbox->p_next = p_msg;

    p_msg->p_next = NULL;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/186355
 *
 */
static void msg_pool_give(msg_pool_t *p_msg,
                          appman_app_t *p_app)
{
    msg_pool_t *p_tmp_msg_pool = (msg_pool_t *)p_app->p_msg_pool;

    LOG_DEBUG("Section added to %s message pool", p_app->p_name);

    if (NULL == p_tmp_msg_pool)
    {
        p_app->p_msg_pool = p_msg;
        p_msg->p_next = NULL;

        return;
    }

    while (NULL != p_tmp_msg_pool->p_next)
    {
        p_tmp_msg_pool = p_tmp_msg_pool->p_next;
    }

    p_tmp_msg_pool->p_next = p_msg;

    p_msg->p_next = NULL;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/186095
 *
 */
static void msg_pool_init(appman_app_t *p_app, appman_status_t *p_status)
{
    if (NULL == p_app->p_msg_pool)
    {
        *p_status = APPMAN_STATUS_APP_MSG_POOL_ERROR;

        LOG_ERROR("There is no available MSG POOL");

        return;
    }

    if (0U != (p_app->msg_pool_size % sizeof(msg_pool_t)))
    {
        *p_status = APPMAN_STATUS_MSG_POOL_INVALID_SIZE;

        LOG_ERROR("Invalid MSG POOL size");

        return;
    }

    p_app->p_msg_inbox = NULL;

    msg_pool_t *p_tmp_msg_pool = (msg_pool_t *)p_app->p_msg_pool;

    msg_pool_t *p_tmp_msg = (msg_pool_t *)p_app->p_msg_pool;

    uint32_t msg_idx = 1U;

    size_t pool_section_count =
        (size_t)((p_app->msg_pool_size) / sizeof(msg_pool_t));

    while (pool_section_count > msg_idx)
    {
        p_tmp_msg->p_next = &p_tmp_msg_pool[msg_idx];

        p_tmp_msg = p_tmp_msg->p_next;

        msg_idx++;
    }

    p_tmp_msg->p_next = NULL;

    *p_status = APPMAN_STATUS_SUCCESS;

    LOG_INFO("%s message pool initialized; size:%u", 
              p_app->p_name,
              pool_section_count);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/186098
 *
 */
static void all_app_msg_send(const appman_msg_t *p_msg,
                             appman_status_t *p_status)
{
    appman_app_t *p_tmp_app = mp_app_list;

    while (NULL != p_tmp_app)
    {
        msg_pool_t *p_msg_pool = msg_pool_get(p_tmp_app,
                                              p_status);

        if (APPMAN_STATUS_NO_MSG_POOL != *p_status)
        {
            p_msg_pool->msg_id = p_msg->msg_id;
            p_msg_pool->p_data = p_msg->p_data;
            p_msg_pool->size = p_msg->size;
            p_msg_pool->p_next = NULL;

            msg_inbox_add(p_msg_pool,
                          p_tmp_app);

            *p_status = APPMAN_STATUS_SUCCESS;

            LOG_DEBUG("Message sent to %s; id:%u", 
                      p_tmp_app->p_name, 
                      p_msg_pool->msg_id);
        }

        p_tmp_app = p_tmp_app->p_next;
    }
}

/**
 * @brief https://codebeamer.daiichi.com/issue/186354
 *
 */
static appman_status_t timer_counter_update(appman_timer_t *p_timer)
{
    appman_timer_mode_t mode = p_timer->mode;

    appman_status_t status = APPMAN_STATUS_SUCCESS;

    bsp_isr_enter_critical();

    switch (mode)
    {
    case APPMAN_TIMER_MODE_SEQUANTIAL:
    {
        p_timer->remain_ms = p_timer->interval_ms + bsp_mcu_tick_get();
    }
    break;

    case APPMAN_TIMER_MODE_STRICT:
    {
        p_timer->remain_ms = p_timer->interval_ms;
    }
    break;

    default:
    {
        status = APPMAN_STATUS_ERROR_PARAM;
    }
    break;
    }

    bsp_isr_exit_critical();

    return status;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/186080
 *
 */
static void timer_exec(void)
{
    appman_timer_t *p_tmp_timer = mp_timer_list;

    while (NULL != p_tmp_timer)
    {
        uint32_t curr_tick = bsp_mcu_tick_get();

        if ((p_tmp_timer->remain_ms <= curr_tick) &&
            (APPMAN_TIMER_MODE_SEQUANTIAL == p_tmp_timer->mode) &&
            (APPMAN_TIMER_STATE_RUN == p_tmp_timer->state))
        {
            LOG_VERBOSE("Timeout occurred");

            p_tmp_timer->handler(p_tmp_timer->p_arg);

            (void)timer_counter_update(p_tmp_timer);
        }

        p_tmp_timer = p_tmp_timer->p_next;
    }
}

/**
 * @brief https://codebeamer.daiichi.com/issue/186088
 *
 */
static void timer_cb(void)
{
    appman_timer_t *p_tmp_timer = mp_timer_list;

    while (NULL != p_tmp_timer)
    {
        if ((APPMAN_TIMER_MODE_STRICT == p_tmp_timer->mode) &&
            (APPMAN_TIMER_STATE_RUN == p_tmp_timer->state))
        {
            p_tmp_timer->remain_ms = p_tmp_timer->remain_ms - 1U;

            if (0U == p_tmp_timer->remain_ms)
            {
                LOG_VERBOSE("Timeout occurred");

                p_tmp_timer->handler(p_tmp_timer->p_arg);

                p_tmp_timer->remain_ms = p_tmp_timer->interval_ms;
            }
        }

        p_tmp_timer = p_tmp_timer->p_next;
    }
}

/**
 * @brief https://codebeamer.daiichi.com/issue/186081
 *
 */
static void msg_exec(void)
{
    appman_msg_t msg = {

        .dest = mp_app_list_idx->app_id,
    };

    msg_pool_t *p_tmp_msg = NULL;

    while (APPMAN_STATUS_NO_MSG_INBOX !=
           msg_inbox_get(mp_app_list_idx, &p_tmp_msg))
    {
        msg.msg_id = p_tmp_msg->msg_id;
        msg.p_data = p_tmp_msg->p_data;
        msg.size = p_tmp_msg->size;

        mp_app_list_idx->msg_handler(&msg);

        msg_pool_give(p_tmp_msg, mp_app_list_idx);
    }
}

/**
 * @brief https://codebeamer.daiichi.com/issue/186086
 *
 */
static void app_exec(void)
{
    m_curr_app = mp_app_list_idx->app_id;

    if (PROCESS_STATE_APP_INIT == m_process_state)
    {
        LOG_INFO("Calling init func; app:%s, id:%u", 
                  mp_app_list_idx->p_name,
                  mp_app_list_idx->app_id);

        mp_app_list_idx->init();
    }

    if (PROCESS_STATE_APP_RUN == m_process_state)
    {
        mp_app_list_idx->run();
    }

    mp_app_list_idx = mp_app_list_idx->p_next;

    if (NULL == mp_app_list_idx)
    {
        if (PROCESS_STATE_APP_INIT == m_process_state)
        {
            m_process_state = PROCESS_STATE_APP_RUN;

            LOG_INFO("Appman state changed to RUN");
        }

        mp_app_list_idx = mp_app_list;
    }
}

// end of file
