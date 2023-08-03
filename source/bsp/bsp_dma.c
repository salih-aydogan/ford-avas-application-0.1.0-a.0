// PRQA S 3108 EOF # Comments with links are required for ensure traceability.
/**
 * @file bsp_dma.c
 * @author berke yalcinkaya (berke.yalcinkaya@daiichi.com)
 * @brief The interface of the DMA.
 * @version 0.1
 * @date 2023-05-22
 *
 * @copyright Copyright (c) 2023
 *
 * https://codebeamer.daiichi.com/issue/192951
 *
 */

#include "bsp_dma.h"

#include "edma_driver.h"

#include "log.h"

LOG_MODULE_NAME("BSP_DMA");

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/**
 * @brief Holds run-time state of DMA driver.
 *
 * https://codebeamer.daiichi.com/issue/192960
 *
 */
static edma_state_t m_edma_state;

/**
 * @brief Holds initialization status of DMA unit.
 *
 * https://codebeamer.daiichi.com/issue/193109
 *
 */
static bool m_dma_init = false;

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/193196
 *
 */
bool bsp_dma_init(void)
{
    const edma_user_config_t edma_user_cfg = {
        .chnArbitration = EDMA_ARBITRATION_ROUND_ROBIN,
        .haltOnError = false,

    };

    if (false == m_dma_init)
    {
        status_t status = EDMA_DRV_Init(&m_edma_state,
                                        &edma_user_cfg,
                                        NULL,
                                        NULL,
                                        0U);

        m_dma_init = (STATUS_SUCCESS == status);

        LOG_DEBUG("EDMA Init; status:%u", status);
    }

    if (true == m_dma_init)
    {
        LOG_INFO("Initialized");
    }
    else
    {
        LOG_ERROR("Init Fail");
    }

    return m_dma_init;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/192963
 *
 */
bool bsp_dma_deinit(void)
{
    if (true == m_dma_init)
    {
        (void)EDMA_DRV_Deinit();

        m_dma_init = false;

        LOG_INFO("Denitialized");

        return true;
    }

    return false;
}

// end of file
