// PRQA S 3108 EOF # Supressed, Comments with links are required for ensure traceability.
// PRQA S 380 EOF # Suppressed, Too many macros in the driver provided by NXP causes this error.
/**
 * @file bsp_can.c
 * @author Utku Cecen (utku.cecen@daiichi.com)
 * @brief
 * @version 0.1
 * @date 2023-02-02
 *
 * @copyright Copyright (c) 2023
 *
 * https://codebeamer.daiichi.com/issue/177714
 * https://codebeamer.daiichi.com/issue/177731
 *
 */

#include "bsp_can.h"

#include "flexcan_driver.h"
#include "pins_driver.h"

#include "log.h"

LOG_MODULE_NAME("BSP_CAN");

/*******************************************************************************
 * Macros
 ******************************************************************************/
/**
 * @brief Number of CAN instance
 *
 */
#define CAN_INSTANCE 0U

/**
 * @brief Count of the relevant CAN pins.
 *
 */
#define CAN_PINS_MAX 2U

/**
 * @brief CAN bitrate options.
 *
 */
#define CAN_SPEED_MAX 3U

/**
 * @brief https://codebeamer.daiichi.com/issue/182342
 *
 */
#define CAN_TX_INITIAL_MBOX ((2U * (BSP_CAN_CFG_FILTER_IDS_TBL_MAX / 8U)) + 8U)

/*******************************************************************************
 * Structs
 ******************************************************************************/

/**
 * @brief CAN bitrate structures.
 *
 * https://codebeamer.daiichi.com/issue/182685
 *
 */
// PRQA S 3213 ++ # Supressed, The tag 'can_arbitration_phase_s' is not used and could be removed.
typedef struct can_arbitration_phase_s
{
    uint32_t bitrate_kbps;
    uint32_t prog_segment;
    uint32_t phase_segment_1;
    uint32_t phase_segment_2;
    uint32_t pre_divider;
    uint32_t rjump_width;

} can_arbitration_phase_t;

// PRQA S 3213 --

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/188386
 *
 */
static bsp_can_rx_cb_t m_can_rx_cb;

/**
 * @brief https://codebeamer.daiichi.com/issue/188387
 *
 */
static bsp_can_tx_cb_t m_can_tx_cb;

/**
 * @brief https://codebeamer.daiichi.com/issue/188388
 *
 */
static bsp_can_err_cb_t m_can_err_cb;

/**
 * @brief https://codebeamer.daiichi.com/issue/182418
 *
 */
static flexcan_msgbuff_t m_recv_buff;

/**
 * @brief https://codebeamer.daiichi.com/issue/183493
 * 
 */
static uint8_t m_tx_mailbox_cnt = 0U;

/**
 * @brief https://codebeamer.daiichi.com/issue/182687
 *
 */
static volatile bool m_can_has_hw_err = false;

// PRQA S 306 ++ # Defined by nxp library to hold register base address.
/**
 * @brief https://codebeamer.daiichi.com/issue/188389
 *
 */
static CAN_Type * const m_flexcan_base[] = CAN_BASE_PTRS;
// PRQA S 306 --

/**
 * @brief https://codebeamer.daiichi.com/issue/188394
 *
 */
static volatile bsp_can_fault_state_t m_fault_state = BSP_CAN_FAULT_STATE_NONE;

/**
 * @brief https://codebeamer.daiichi.com/issue/188608
 *
 */
static uint32_t m_tx_msg_id = 0U;

/*******************************************************************************
 * Private Function Declarations
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/183016
 *
 */
static void can_callback(uint8_t instance,
                         flexcan_event_type_t eventType,
                         uint32_t buffIdx,
                         flexcan_state_t *flexcanState);

/**
 * @brief https://codebeamer.daiichi.com/issue/188575
 *
 */
static void can_err_cb(uint8_t instance,
                       flexcan_event_type_t eventType,
                       flexcan_state_t *flexcanState);

/**
 * @brief https://codebeamer.daiichi.com/issue/183017
 *
 */
static bool can_pin_init(void);

/**
 * @brief https://codebeamer.daiichi.com/issue/183019
 *
 */
static bool can_drv_init(void);

/**
 * @brief Set to number of filter id according to count of filtered ID.
 *
 */
static flexcan_rx_fifo_id_filter_num_t can_filter_num_get(uint32_t msg_count);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/181183
 *
 */
bool bsp_can_init(void)
{
    bool ret = can_pin_init();

    m_can_has_hw_err = false;

    if (false == ret)
    {
        LOG_ERROR("Init Fail");

        return ret;
    }

    ret = can_drv_init();

    if (true == ret)
    {
        LOG_INFO("Initialized");
    }
    else
    {
        LOG_ERROR("Init Fail");
    }

    return ret;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/181181
 * 
 */
bool bsp_can_deinit(void)
{
    status_t ret = FLEXCAN_DRV_Deinit(CAN_INSTANCE);

    LOG_DEBUG("CAN DeInit; instance:%u, status:%u", 
              CAN_INSTANCE,
              ret);

    if (STATUS_SUCCESS == ret)
    {
        m_can_rx_cb = NULL;
        m_can_tx_cb = NULL;
        m_can_err_cb = NULL;

        LOG_INFO("Initialized");
    
        return true;
    }
    else
    {
        LOG_ERROR("Deinit Fail");
    }

    return false;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/181185
 *
 */
bsp_can_status_t bsp_can_async_write(const bsp_can_data_t *p_tx)
{
    if (NULL == p_tx)
    {
        LOG_ERROR("Invalid argument in bsp_can_async_write");

        return BSP_CAN_STATUS_ERROR;
    }

    if (true == m_can_has_hw_err)
    {
        return BSP_CAN_STATUS_HW_ERROR;
    }

    uint8_t tx_mbox = (((uint8_t)CAN_TX_INITIAL_MBOX) + m_tx_mailbox_cnt);

    flexcan_data_info_t data_info =
    {
        .data_length = p_tx->size,
        .is_remote = false,
        .msg_id_type = FLEXCAN_MSG_ID_STD,
    };

    m_tx_msg_id = (uint32_t)p_tx->msg_id;

    status_t ret = FLEXCAN_DRV_Send(CAN_INSTANCE,
                                    tx_mbox,
                                    &data_info,
                                    (uint32_t)p_tx->msg_id,
                                    p_tx->payload);

    LOG_TX(p_tx->payload, p_tx->size);

    if (STATUS_SUCCESS == ret)
    {
        if (tx_mbox >= 31U)
        {
            m_tx_mailbox_cnt = 0U;
        }
        else
        {
            m_tx_mailbox_cnt++;
        }
    }

    if (STATUS_BUSY == ret)
    {
        return BSP_CAN_STATUS_BUSY;
    }

    if (STATUS_ERROR == ret)
    {
        return BSP_CAN_STATUS_ERROR;
    }

    return BSP_CAN_STATUS_OK;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/181182
 *
 */
void bsp_can_filter(const uint32_t *p_msg_ids)
{
    flexcan_id_table_t filter_id_table[32U] = {0U};

    for (uint16_t cnt = 0U; cnt < BSP_CAN_CFG_FILTER_IDS_TBL_MAX; cnt++)
    {
        filter_id_table[cnt].isRemoteFrame = false;
        filter_id_table[cnt].isExtendedFrame = true;
        filter_id_table[cnt].id = p_msg_ids[cnt];

        LOG_DEBUG("Filter added; id:%u", filter_id_table[cnt].id);
    }

    FLEXCAN_DRV_ConfigRxFifo(CAN_INSTANCE,
                             FLEXCAN_RX_FIFO_ID_FORMAT_A,
                             filter_id_table);

    (void)FLEXCAN_DRV_RxFifo(CAN_INSTANCE, &m_recv_buff);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/188576
 * 
 */
bsp_can_status_t bsp_can_cb_set(const bsp_can_cb_t *p_cb)
{
    if ((NULL == p_cb) || 
       ((NULL == p_cb->err_cb) && 
        (NULL == p_cb->rx_cb) && 
        (NULL == p_cb->tx_cb)))
    {
        LOG_ERROR("Invalid argument in bsp_can_cb_set");
        
        return BSP_CAN_STATUS_ERROR;
    }

    if (NULL != p_cb->err_cb) 
    {
        m_can_err_cb = p_cb->err_cb;
    }

    if (NULL != p_cb->rx_cb) 
    {
        m_can_rx_cb = p_cb->rx_cb;
    }

    if (NULL != p_cb->tx_cb)
    {
        m_can_tx_cb = p_cb->tx_cb;
    }

    return BSP_CAN_STATUS_OK;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/188318
 * 
 */
bsp_can_status_t bsp_can_fault_state_get(bsp_can_fault_state_t *p_state)
{
    if (NULL == p_state)
    {
        LOG_ERROR("Invalid argument in bsp_can_fault_state_get");

        return BSP_CAN_STATUS_ERROR;
    }

    *p_state = m_fault_state;
    
    return BSP_CAN_STATUS_OK;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/188319
 * 
 */
bsp_can_status_t bsp_can_rx_err_cnt_get(uint8_t *p_err_cnt)
{
    if (NULL == p_err_cnt)
    {
        LOG_ERROR("Invalid argument in bsp_can_rx_err_cnt_get");

        return BSP_CAN_STATUS_ERROR; 
    }

    CAN_Type const* base = m_flexcan_base[CAN_INSTANCE];

    uint32_t err_cnt_reg = base->ECR;

    *p_err_cnt = (uint8_t)(err_cnt_reg >> 8U);

    return BSP_CAN_STATUS_OK;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/188317
 * 
 */
bsp_can_status_t bsp_can_tx_err_cnt_get(uint8_t *p_err_cnt)
{
    if (NULL == p_err_cnt)
    {
        LOG_ERROR("Invalid argument in bsp_can_tx_err_cnt_get");

        return BSP_CAN_STATUS_ERROR; 
    }

    CAN_Type const* base = m_flexcan_base[CAN_INSTANCE];
    
    uint32_t err_cnt_reg = base->ECR;

    *p_err_cnt = (uint8_t)err_cnt_reg;

    return BSP_CAN_STATUS_OK;
}

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/183017
 *
 */
static bool can_pin_init(void)
{
    const pin_settings_config_t can_pins_cfg[CAN_PINS_MAX] =
    {
        {
            // PRQA S 306 ++
            .base = PORTE,
            // PRQA S 306 --
            .pinPortIdx = 4,
            .pullConfig = PORT_INTERNAL_PULL_NOT_ENABLED,
            .driveSelect = PORT_LOW_DRIVE_STRENGTH,
            .passiveFilter = false,
            .mux = PORT_MUX_ALT5,
            .pinLock = false,
            .intConfig = PORT_DMA_INT_DISABLED,
            .clearIntFlag = false,
            .gpioBase = NULL,
            .digitalFilter = false,
        },
        {
            // PRQA S 306 ++
            .base = PORTE,
            // PRQA S 306 --
            .pinPortIdx = 5,
            .pullConfig = PORT_INTERNAL_PULL_NOT_ENABLED,
            .driveSelect = PORT_LOW_DRIVE_STRENGTH,
            .passiveFilter = false,
            .mux = PORT_MUX_ALT5,
            .pinLock = false,
            .intConfig = PORT_DMA_INT_DISABLED,
            .clearIntFlag = false,
            .gpioBase = NULL,
            .digitalFilter = false,
        },
    };

    status_t ret = PINS_DRV_Init(CAN_PINS_MAX, can_pins_cfg);

    for (uint8_t cnt = 0; cnt < CAN_PINS_MAX; cnt++)
    {
        LOG_DEBUG("Pin init; base:%u, pin:%u, dir:%u, pull:%u, status:%u", 
                  can_pins_cfg[cnt].base, 
                  can_pins_cfg[cnt].pinPortIdx,
                  can_pins_cfg[cnt].direction,
                  can_pins_cfg[cnt].pullConfig,
                  ret);
    }

    if (STATUS_SUCCESS != ret)
    {
        return false;
    }

    return true;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/183019
 *
 */
static bool can_drv_init(void)
{
    static flexcan_state_t m_can_state;

    const can_arbitration_phase_t speed_tbl[CAN_SPEED_MAX] =
    {
        {
            .bitrate_kbps = 500000U,
            .prog_segment = 7U,
            .phase_segment_1 = 4U,
            .phase_segment_2 = 1U,
            .pre_divider = 4U,
            .rjump_width = 1U,
        },
        {
            .bitrate_kbps = 250000U,
            .prog_segment = 7U,
            .phase_segment_1 = 4U,
            .phase_segment_2 = 1U,
            .pre_divider = 9U,
            .rjump_width = 1U,
        },
        {
            .bitrate_kbps = 100000U,
            .prog_segment = 7U,
            .phase_segment_1 = 4U,
            .phase_segment_2 = 1U,
            .pre_divider = 24U,
            .rjump_width = 1U,
        },
    };

    flexcan_user_config_t can_cfg = {0};

    bool found = false;

    for (uint32_t cnt = 0; (cnt < CAN_SPEED_MAX) && (true != found); cnt++)
    {
        if (BSP_CAN_CFG_BITRATE_BIT == speed_tbl[cnt].bitrate_kbps)
        {
            can_cfg.bitrate.propSeg = speed_tbl[cnt].prog_segment;
            can_cfg.bitrate.phaseSeg1 = speed_tbl[cnt].phase_segment_1;
            can_cfg.bitrate.phaseSeg2 = speed_tbl[cnt].phase_segment_2;
            can_cfg.bitrate.preDivider = speed_tbl[cnt].pre_divider;
            can_cfg.bitrate.rJumpwidth = speed_tbl[cnt].rjump_width;

            found = true;
        }
    }

    if (false == found)
    {
        LOG_ERROR("BSP_CAN_CFG_BITRATE_BIT not found");

        return false;
    }

    can_cfg.flexcanMode = FLEXCAN_NORMAL_MODE;
    can_cfg.is_rx_fifo_needed = true;
    can_cfg.max_num_mb = 32U;
    can_cfg.num_id_filters = can_filter_num_get(BSP_CAN_CFG_FILTER_IDS_TBL_MAX);
    can_cfg.transfer_type = FLEXCAN_RXFIFO_USING_INTERRUPTS;

    status_t ret = FLEXCAN_DRV_Init(CAN_INSTANCE, &m_can_state, &can_cfg);

    LOG_DEBUG("CAN Init; instance:%u, bitrate:%u, progSeg:%u, phSeg1:%u, phSeg2:%u, preDiv:%u, rJump:%u, status:%u", 
              CAN_INSTANCE,
              (uint32_t)BSP_CAN_CFG_BITRATE_BIT,
              can_cfg.bitrate.propSeg,
              can_cfg.bitrate.phaseSeg1,
              can_cfg.bitrate.phaseSeg2,
              can_cfg.bitrate.preDivider,
              can_cfg.bitrate.rJumpwidth,
              ret);

    if (STATUS_SUCCESS != ret)
    {
        return false;
    }

    FLEXCAN_DRV_InstallEventCallback(CAN_INSTANCE, can_callback, NULL);

    FLEXCAN_DRV_InstallErrorCallback(CAN_INSTANCE, can_err_cb, NULL);
    
    return true;
}

/**
 * @brief Holds the calculated number of filter id according to count 
 * of filtered IDs.
 *
 */
static flexcan_rx_fifo_id_filter_num_t can_filter_num_get(uint32_t msg_count)
{
    flexcan_rx_fifo_id_filter_num_t rx_fifo_filter_num;

    uint32_t calculated_filter_num = ((msg_count - 1U) / 8U);

    if (0x0U == calculated_filter_num)
    {
        rx_fifo_filter_num = FLEXCAN_RX_FIFO_ID_FILTERS_8;
    }
    else if (0x1U == calculated_filter_num)
    {
        rx_fifo_filter_num = FLEXCAN_RX_FIFO_ID_FILTERS_16;
    }
    else if (0x2U == calculated_filter_num)
    {
        rx_fifo_filter_num = FLEXCAN_RX_FIFO_ID_FILTERS_24;
    }
    else
    {
        rx_fifo_filter_num = FLEXCAN_RX_FIFO_ID_FILTERS_32;
    }

    return rx_fifo_filter_num;
}

// PRQA S 3673 ++ # No const arguments were used in the prototype defined in the nxp library. (The relevant item is for the 4th parameter of the callback function.)
/**
 * @brief https://codebeamer.daiichi.com/issue/183016
 *
 */
static void can_callback(uint8_t instance,
                         flexcan_event_type_t eventType,
                         uint32_t buffIdx,
                         flexcan_state_t *flexcanState)
{
    (void)buffIdx;
    (void)flexcanState;

    bsp_can_data_t data;

    if ((NULL != m_can_rx_cb) && (FLEXCAN_EVENT_RXFIFO_COMPLETE == eventType))
    {
            data.msg_id = m_recv_buff.msgId;

            data.size = m_recv_buff.dataLen;
            
            (void)strncpy((char *)data.payload, 
                          (char *)m_recv_buff.data, 
                           m_recv_buff.dataLen);

            (m_can_rx_cb)(&data);

            LOG_RX(data.payload, data.size);
            
            status_t ret = FLEXCAN_DRV_RxFifo(instance, &m_recv_buff);

            if (STATUS_SUCCESS != ret)
            {
                LOG_ERROR("CB FAILED");
            }
    }
    else if ((NULL != m_can_tx_cb) && (FLEXCAN_EVENT_TX_COMPLETE == eventType))
    {   
        data.msg_id = m_tx_msg_id;

        (m_can_tx_cb)(&data);
    }
    else
    {
        return;
    }
}

/**
 * @brief https://codebeamer.daiichi.com/issue/188575
 *
 */
static void can_err_cb(uint8_t instance,
                       flexcan_event_type_t eventType,
                       flexcan_state_t *flexcanState)
{
    (void)eventType;
    (void)flexcanState;

    if (NULL == m_can_err_cb)
    {
        return;
    }

    m_can_has_hw_err = true;

    uint32_t status_reg = FLEXCAN_DRV_GetErrorStatus(instance);

    if ((1U & (status_reg >> CAN_ESR1_BIT1ERR_SHIFT)) != 0U) 
    {
        (m_can_err_cb)(BSP_CAN_ERR_STATUS_BIT_MONITORING1);
    }

    if ((1U & (status_reg >> CAN_ESR1_BIT0ERR_SHIFT)) != 0U)
    {
        (m_can_err_cb)(BSP_CAN_ERR_STATUS_BIT_MONITORING2);
    }

    if ((1U & (status_reg >> CAN_ESR1_ACKERR_SHIFT)) != 0U)
    {
        (m_can_err_cb)(BSP_CAN_ERR_STATUS_CHECK_ACK_FAILED);
    }

    if ((1U & (status_reg >> CAN_ESR1_FRMERR_SHIFT)) != 0U)
    {
        (m_can_err_cb)(BSP_CAN_ERR_STATUS_CHECK_FORM_FAILED);
    }

    if ((1U & (status_reg >> CAN_ESR1_STFERR_SHIFT)) != 0U)
    {
        (m_can_err_cb)(BSP_CAN_ERR_STATUS_CHECK_STUFFING_FAILED);
    }

    if ((1U & (status_reg >> CAN_ESR1_CRCERR_SHIFT)) != 0U)
    {
        (m_can_err_cb)(BSP_CAN_ERR_STATUS_CHECK_CRC_FAILED);
    }
   
    if ((status_reg & CAN_ESR1_FLTCONF_MASK) == 0U)
    {
        m_fault_state = BSP_CAN_FAULT_STATE_ACTIVE;
    }
    else if ((status_reg & CAN_ESR1_FLTCONF_MASK) == 0x10U)
    {
        m_fault_state = BSP_CAN_FAULT_STATE_PASSIVE;
    }
    else
    {
        m_fault_state = BSP_CAN_FAULT_STATE_BUSOFF;
    }
}

// end of file
