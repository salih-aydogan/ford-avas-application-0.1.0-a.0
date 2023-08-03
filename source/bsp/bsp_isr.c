// PRQA S 3108 EOF # Supressed, Comments with links are required for ensure traceability.
/**
 * @file bsp_isr.c
 * @author Utku Cecen (utku.cecen@daiichi.com)
 * @brief
 * @version 0.1
 * @date 2023-02-02
 *
 * @copyright Copyright (c) 2023
 *
 * https://codebeamer.daiichi.com/issue/182578
 *
 */

#include "bsp_isr.h"

#include "interrupt_manager.h"

#include "log.h"

LOG_MODULE_NAME("BSP_ISR");

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/182612
 *
 */
static uint8_t m_systick_idx = 0U;

/**
 * @brief https://codebeamer.daiichi.com/issue/182613
 *
 */
static bsp_isr_cb_t m_systick_isr_tbl[BSP_ISR_CFG_SYSTICK_CNT_MAX] = {0U};

/**
 * @brief https://codebeamer.daiichi.com/issue/182614
 *
 */
static bsp_isr_cb_t m_hardfault_isr = NULL;

/**
 * @brief https://codebeamer.daiichi.com/issue/182615
 *
 */
static isr_t m_systick_old_isr;

/*******************************************************************************
 * Private Function Declarations
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/182608
 *
 */
static void hardfault_isr(void);

/**
 * @brief https://codebeamer.daiichi.com/issue/182609
 *
 */
static void systick_isr(void);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/182910
 *
 */
void bsp_isr_init(void)
{
    m_systick_idx = 0U;

    m_hardfault_isr = NULL;

    for (uint32_t idx = 0U; idx < BSP_ISR_CFG_SYSTICK_CNT_MAX; ++idx)
    {
        m_systick_isr_tbl[idx] = NULL;
    }

    INT_SYS_InstallHandler(HardFault_IRQn, hardfault_isr, NULL);

    INT_SYS_InstallHandler(SysTick_IRQn, systick_isr, &m_systick_old_isr);

    INT_SYS_SetPriority(LPTMR0_IRQn, 0);

    INT_SYS_SetPriority(WDOG_IRQn, 0);

    INT_SYS_SetPriority(PORT_IRQn, 1);

    INT_SYS_SetPriority(CAN0_ORed_Err_Wakeup_IRQn, 1);

    INT_SYS_SetPriority(CAN0_ORed_0_31_MB_IRQn, 1);

    INT_SYS_SetPriority(LPI2C0_Master_Slave_IRQn, 2);

    INT_SYS_SetPriority(FLEXIO_IRQn, 2);

    INT_SYS_SetPriority(LPSPI0_IRQn, 3);

    INT_SYS_SetPriority(LPUART0_RxTx_IRQn, 3);

    INT_SYS_SetPriority(DMA0_IRQn, 3);

    INT_SYS_SetPriority(DMA_Error_IRQn, 3);

    INT_SYS_SetPriority(SysTick_IRQn, 0);

    LOG_INFO("Initialized");
}

/**
 * @brief https://codebeamer.daiichi.com/issue/182911
 *
 */
void bsp_isr_enter_critical(void)
{
    LOG_DEBUG("bsp_isr enter critical");

    INT_SYS_DisableIRQGlobal();
}

/**
 * @brief https://codebeamer.daiichi.com/issue/182908
 *
 */
void bsp_isr_exit_critical(void)
{
    LOG_DEBUG("bsp_isr exit critical");

    INT_SYS_EnableIRQGlobal();
}

/**
 * @brief https://codebeamer.daiichi.com/issue/182909
 *
 */
void bsp_isr_cb_set(bsp_isr_t isr, bsp_isr_cb_t callback)
{
    if (NULL == callback)
    {
        LOG_ERROR("Invalid argument in bsp_isr_cb_set");

        return;
    }

    if (isr == BSP_ISR_HARDFAULT)
    {
        m_hardfault_isr = callback;

        LOG_DEBUG("Hardfault cb configured"); 
    }

    if (isr == BSP_ISR_SYSTICK)
    {
        if (m_systick_idx < BSP_ISR_CFG_SYSTICK_CNT_MAX)
        {
            m_systick_isr_tbl[m_systick_idx] = callback;

            m_systick_idx++;

            LOG_DEBUG("Systick cb configured"); 
        }
    }
}

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/182608
 *
 */
static void hardfault_isr(void)
{
    if (NULL != m_hardfault_isr)
    {
        LOG_NOW(LOG_LEVEL_FATAL, "Hardfault ISR occured");

        m_hardfault_isr();        
    }
    else
    {
        LOG_NOW(LOG_LEVEL_FATAL, "Hardfault ISR occured and NULL callback");
    }
}

/**
 * @brief https://codebeamer.daiichi.com/issue/182609
 *
 */
static void systick_isr(void)
{
    (*m_systick_old_isr)();

    for (uint32_t cnt = 0U; cnt < m_systick_idx; ++cnt)
    {
        if (NULL != m_systick_isr_tbl[cnt])
        {
            (*m_systick_isr_tbl[cnt])();
        }
    }
}

// end of file
