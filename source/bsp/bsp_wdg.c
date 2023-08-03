// PRQA S 3108 EOF # Suppressed, Comments with links are required for ensure traceability.
// PRQA S 380 EOF # Suppressed, Too many macros in the driver provided by NXP causes this error.
/**
 * @file bsp_wdg.c
 * @author berke yalcinkaya (berke.yalcinkaya@daiichi.com)
 * @brief Implementation of the WDG component.
 * @version 0.1
 * @date 2023-01-27
 *
 * @copyright Copyright (c) 2021
 *
 * https://codebeamer.daiichi.com/issue/180255
 *
 */

#include "bsp_wdg.h"

#include "wdog_driver.h"
#include "interrupt_manager.h"

#include "log.h"

LOG_MODULE_NAME("BSP_WDG");

/*******************************************************************************
 * Macros
 ******************************************************************************/

/**
 * @brief This macro specifies which wdg unit to use.
 *
 * https://codebeamer.daiichi.com/issue/180875
 *
 */
#define WDG_INSTANCE                (0U)

/**
 * @brief This macro is used in wdg timeout calculation.
 *
 * https://codebeamer.daiichi.com/issue/180877
 *
 */
#define WDG_CFG_TIMEOUT_FACTOR      (2U)

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/**
 * @brief WDG initialization flag.
 *
 * https://codebeamer.daiichi.com/issue/180876
 *
 */
static bool m_wdg_init = false;

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/180810
 *
 */
bool bsp_wdg_init(bsp_wdg_timeout_cb_t callback)
{
    if (NULL == callback)
    {
        LOG_ERROR("Invalid argument in bsp_wdg_init");
        
        return false;
    }

    uint16_t wdg_timeout_val = (uint16_t)(BSP_WDG_CFG_TIMEOUT_MSEC /
                                          WDG_CFG_TIMEOUT_FACTOR);

    wdog_user_config_t wdg_user_cfg = 
    {
        .clkSource = WDOG_LPO_CLOCK,
        .opMode = {

            .debug = false,
            .stop = false,
            .wait = false,
        },
        .intEnable = true,
        .winEnable = false,
        .windowValue = 0U,
        .timeoutValue = wdg_timeout_val,
        .prescalerEnable = true,
    };

    INT_SYS_InstallHandler(WDOG_IRQn,
                           (isr_t)callback,
                           (isr_t *)0U);

    status_t status_ret = WDOG_DRV_Init((uint32_t)WDG_INSTANCE,
                                        &wdg_user_cfg);

    LOG_DEBUG("WDOG Init; instance:%u, timeout:%u, debug:%u, stop:%u, wait:%u, status:%u", 
              WDG_INSTANCE,
              wdg_user_cfg.timeoutValue,
              wdg_user_cfg.opMode.debug,
              wdg_user_cfg.opMode.stop,
              wdg_user_cfg.opMode.wait,
              status_ret);

    m_wdg_init = (STATUS_SUCCESS == status_ret);

    if (true == m_wdg_init)
    {
        /*
            Enabling Reset Control Module(RCM) with delay on watchdog interrupt.
            Without RCM enabled, wdg interrupt last less than 1ms before
            CPU performs an internal force reset.
            With rcm_srie_delay=3, wdg interrupt callback lasts 3.5ms, tested.
            See S32K1xx Reference Manual for more info.
        */
        // PRQA S 306 ++ # Suppressed, Defined by nxp library to cast register data. False positive warning.
        RCM->SRIE = RCM_SRIE_DELAY(3) | RCM_SRIE_WDOG(1);
        // PRQA S 306 --

        LOG_INFO("Initialized");
    }
    else
    {
        LOG_ERROR("Init Fail");
    }

    return m_wdg_init;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/180920
 *
 */
void bsp_wdg_kick(void)
{
    if (false != m_wdg_init)
    {
        WDOG_DRV_Trigger(WDG_INSTANCE);
    }
}

// end of file
