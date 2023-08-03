// PRQA S 3108 EOF # Suppressed, Comments with links are required for ensure traceability.
/**
 * @file bsp_timer.c
 * @author Berke Yalcinkaya (berke.yalcinkaya@daiichi.com)
 * @brief Implementation of timer component.
 * @version 0.1
 * @date 2023-02-14
 *
 * @copyright Copyright (c) 2023
 *
 * https://codebeamer.daiichi.com/issue/183682
 *
 */

#include "bsp_timer.h"

#include "clock.h"
#include "lptmr_driver.h"
#include "interrupt_manager.h"

#include "log.h"

LOG_MODULE_NAME("BSP_TIMER");

// PRQA S 380 ++ # Suppressed, Too many macros in the driver provided by NXP causes this error.
/*******************************************************************************
 * Macros
 ******************************************************************************/

/**
 * @brief This macro specifies which LPTMR unit to use.
 *
 * https://codebeamer.daiichi.com/issue/183712
 *
 */
#define LPTMR_INSTANCE          (0U)

/**
 * @brief This macro gives the number of cycles required to reach the desired
 *        frequency value. Initially this value should be 1250 to reach
 *        32khz frequency.
 *
 * https://codebeamer.daiichi.com/issue/183713
 *
 */
#define TIMEOUT_VALUE_INIT      (1250U)

// PRQA S 380 --

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/**
 * @brief This variable is used to indicate whether the initialize operation was
 *        successfull or not.
 *
 * https://codebeamer.daiichi.com/issue/183714
 *
 */
static bool m_lptmr_init = false;

/**
 * @brief This function pointer variable allows to define the actions to be
 *        taken in case of timeout.
 *
 * https://codebeamer.daiichi.com/issue/183715
 *
 */
static mcu_timeout_cb_t m_timeout_cb;

/**
 * @brief This macro specifies the frequency of the clock source used by the
 *        LPTMR driver.
 *
 * https://codebeamer.daiichi.com/issue/183950
 *
 */
static uint32_t m_lptmr_clk_freq;

/*******************************************************************************
 * Private Function Declarations
 ******************************************************************************/

/**
 * @brief This function is the handler function that will be called by driver
 *        in case of timeout.
 *
 * https://codebeamer.daiichi.com/issue/183692
 *
 */
static void lptmr_handler(void);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/183691
 *
 */
void bsp_timer_init(void)
{
    const lptmr_config_t lptmr_cfg = {

        .dmaRequest = false,
        .interruptEnable = true,
        .freeRun = false,
        .workMode = LPTMR_WORKMODE_TIMER,
        .clockSelect = LPTMR_CLOCKSOURCE_PCC,
        .prescaler = LPTMR_PRESCALE_2,
        .bypassPrescaler = true,
        .compareValue = TIMEOUT_VALUE_INIT,
        .counterUnits = LPTMR_COUNTER_UNITS_TICKS,
        .pinSelect = LPTMR_PINSELECT_TRGMUX,
        .pinPolarity = LPTMR_PINPOLARITY_RISING,
    };

    LPTMR_DRV_Init(LPTMR_INSTANCE, &lptmr_cfg, true);

    INT_SYS_InstallHandler(LPTMR0_IRQn, lptmr_handler, NULL);

    INT_SYS_EnableIRQ(LPTMR0_IRQn);

    status_t status = CLOCK_DRV_GetFreq(SOSCDIV2_CLK, &m_lptmr_clk_freq);

    m_lptmr_init = (STATUS_SUCCESS == status);

    LOG_DEBUG("Timer Init; instance:%u, clock:%u, status:%u", 
              LPTMR_INSTANCE,
              m_lptmr_clk_freq,
              status);

    if (m_lptmr_init)
    {
        LOG_INFO("Initialized");
    }
    else
    {
        LOG_ERROR("Init Fail");
    }
}

/**
 * @brief https://codebeamer.daiichi.com/issue/183690
 *
 */
void bsp_timer_deinit(void)
{
    if (false == m_lptmr_init)
    {
        LOG_ERROR("Invalid argument in bsp_timer_deinit");

        return;
    }

    LPTMR_DRV_Deinit(LPTMR_INSTANCE);

    INT_SYS_DisableIRQ(LPTMR0_IRQn);

    m_lptmr_init = false;

    m_timeout_cb = NULL;

    LOG_INFO("Denitialized");
}

/**
 * @brief https://codebeamer.daiichi.com/issue/183689
 *
 */
bool bsp_timer_period_set(uint32_t port, uint16_t period_hz)
{
    if (false == m_lptmr_init)
    {
        LOG_ERROR("Invalid argument in bsp_timer_period_set");

        return false;
    }

    if (0U == period_hz)
    {
        LOG_ERROR("Invalid argument in bsp_timer_period_set");

        return false;
    }

    status_t status = STATUS_ERROR;

    if (BSP_TIMER_DAC_PORT == port)
    {
        if (false == LPTMR_DRV_IsRunning(LPTMR_INSTANCE))
        {
            uint16_t tick_value = (uint16_t)(m_lptmr_clk_freq / period_hz);

            status = LPTMR_DRV_SetCompareValueByCount(LPTMR_INSTANCE,
                                                      tick_value);

            LOG_DEBUG("Timer Period Set; instance:%u, tick:%u, status:%u", 
                      LPTMR_INSTANCE,
                      tick_value,
                      status);
        }
    }
    else
    {
        LOG_ERROR("Unknown port in bsp_timer_period_set; port:%u", port);
    }

    if (STATUS_SUCCESS != status)
    {
        LOG_ERROR("Timer Period Set Fail");
    }

    return (STATUS_SUCCESS == status);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/183693
 *
 */
void bsp_timer_start(uint32_t port)
{
    if (false == m_lptmr_init)
    {
        return;
    }

    if (BSP_TIMER_DAC_PORT == port)
    {
        LPTMR_DRV_StartCounter(LPTMR_INSTANCE);
    }
    else
    {
        LOG_ERROR("Unknown port in timer_start; port:%u", port);
    }
}

/**
 * @brief https://codebeamer.daiichi.com/issue/183688
 *
 */
void bsp_timer_stop(uint32_t port)
{
    if (false == m_lptmr_init)
    {
        return;
    }

    if (BSP_TIMER_DAC_PORT == port)
    {
        LPTMR_DRV_StopCounter(LPTMR_INSTANCE);
    }
    else
    {
        LOG_ERROR("Unknown port in timer_stop; port:%u", port);
    }
}

/**
 * @brief https://codebeamer.daiichi.com/issue/183694
 *
 */
bool bsp_timer_cb_set(uint32_t port, mcu_timeout_cb_t callback)
{
    if (false == m_lptmr_init)
    {
        LOG_ERROR("Timer not initialized");

        return false;
    }

    if (NULL == callback)
    {
        LOG_ERROR("Invalid argument in bsp_timer_cb_set");
        
        return false;
    }

    bool ret = false;

    if (BSP_TIMER_DAC_PORT == port)
    {
        m_timeout_cb = callback;

        ret = true;
    }
    else
    {
        LOG_ERROR("Unknown port in timer_cb_set; port:%u", port);
    }

    return ret;
}

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/183692
 *
 */
static void lptmr_handler(void)
{
    if (NULL == m_timeout_cb)
    {
        LOG_DEBUG("NULL callback variable");

        return;
    }

    if (true == LPTMR_DRV_GetCompareFlag(LPTMR_INSTANCE))
    {
        m_timeout_cb();

        LPTMR_DRV_ClearCompareFlag(LPTMR_INSTANCE);
    }
}

// end of file
