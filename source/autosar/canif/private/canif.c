// PRQA S 3108 EOF # Comments with links are required for ensure traceability.
/**
 * @file canif.c
 * @author Burak Utku Cecen (utku.cecen@daiichi.com)
 * @brief
 * @version 0.1
 * @date 2023-03-30
 *
 * @copyright Copyright (c) 2023
 *
 * https://codebeamer.daiichi.com/issue/187418
 *
 */

#include "private/canif.h"

#include "bsp_can.h"

#include "log.h"

LOG_MODULE_NAME("CANIF");

/*******************************************************************************
 * Macros
 ******************************************************************************/

/**
 * @brief Number of TX message count.
 *
 * https://codebeamer.daiichi.com/issue/187943
 *
 */
#define CANIF_TX_MSG_CNT 2U

/**
 * @brief Number of RX message count.
 *
 * https://codebeamer.daiichi.com/issue/187944
 *
 */
#define CANIF_RX_MSG_CNT 3U

/**
 * @brief Specified TX and RX configuration table invalid index value.
 * 
 * https://codebeamer.daiichi.com/issue/195503
 * 
 */
#define CANIF_E_INVALID_INDEX 0xFF

/*******************************************************************************
 * Enumerations
 ******************************************************************************/

/**
 * @brief Canif transmission type for finding relevant index on the
 * configuration tables.
 *
 * https://codebeamer.daiichi.com/issue/187945
 *
 */
// PRQA S 3213 ++ # Supressed,'canif_transmission_type_e' and 'canif_id_type_e' cannot remove.
// PRQA S 1535 ++ # Supressed, 'canif_transmission_type_t' and  'canif_id_type_t' used this project.
typedef enum canif_transmission_type_e
{
    CANIF_TRANSMISSION_TYPE_TX = 0U,
    CANIF_TRANSMISSION_TYPE_RX = 1U,

} canif_transmission_type_t;

/**
 * @brief Canif id types for finding relevant index on the configuration table
 *
 * https://codebeamer.daiichi.com/issue/187946
 *
 */
typedef enum canif_id_type_e
{
    CANIF_ID_TYPE_MSG = 0U,
    CANIF_ID_TYPE_PDU = 1U,

} canif_id_type_t;

/*******************************************************************************
 * Public Variables
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/191397
 * 
 */
bool g_canif_init = false;

/**
 * @brief https://codebeamer.daiichi.com/issue/191398
 * 
 */
can_trcv_info_t g_transceiver_cfg;


// PRQA S 3213 --
// PRQA S 1535 --

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/187938
 *
 */
static const canif_config_t *mp_canif_config = NULL;

/**
 * @brief https://codebeamer.daiichi.com/issue/187939
 *
 */
static pdu_info_t m_canif_data_info = {0U};

/**
 * @brief This array used for dynamic memory allocation process.
 * 
 * https://codebeamer.daiichi.com/issue/187941
 * 
 */
static uint8_t m_can_id_array[4U];

/*******************************************************************************
 * Private Function Declarations
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/188056
 *
 * @param err This variable specified error status from bsp-can to canif.
 */
static void canif_err_cb(bsp_can_err_status_t error);

/**
 * @brief https://codebeamer.daiichi.com/issue/188062
 *
 * @param p_rx_data Holds received data information from bsp-can.
 */
static void canif_rx_cb(bsp_can_data_t *p_rx_data);

/**
 * @brief https://codebeamer.daiichi.com/issue/188063
 *
 * @param p_tx_data Holds transmitted data information from bsp-can.
 */
static void canif_tx_cb(const bsp_can_data_t *p_tx_data);

/**
 * @brief https://codebeamer.daiichi.com/issue/188132
 *
 * @param id_value Specified message ID.(PDU ID or CAN ID.)
 * @param transmit_type Specified RX or TX transmission. 
 * @param id_type Specified message ID format.
 * 
 * @return uint8_t If relevant message ID not found this function return error 
 * or transmit type out of the scope this function return error.
 * If ID founded, returned the relevant ID index.
 */
static uint8_t canif_id_idx_get(uint32_t id_value,
                                canif_transmission_type_t transmit_type,
                                canif_id_type_t id_type);

/**
 * @brief https://codebeamer.daiichi.com/issue/188496
 * 
 * @param id_value Specified message ID.(PDU ID or CAN ID.)
 * @param id_type Specified message ID format.
 * 
 * @return * uint8_t If relevant message ID not found this function return error.
 * If ID founded, returned the relevant ID index.
 */
static uint8_t canif_rx_msg_idx_get(uint32_t id_value, canif_id_type_t id_type);

/**
 * @brief https://codebeamer.daiichi.com/issue/188497
 * 
 * @param id_value Specified message ID.(PDU ID or CAN ID.)
 * @param id_type Specified message ID format.
 * 
 * @return uint8_t If relevant message ID not found this function return error.
 * If ID founded, returned the relevant ID index.
 * 
 */
static uint8_t canif_tx_msg_idx_get(uint32_t id_value, canif_id_type_t id_type);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/188061
 *
 */
bool canif_init(const canif_config_t *p_config)
{
    if (NULL == p_config)
    {
        return false;
    }

    if (NULL == p_config->rx_configuration)
    {
        return false;
    }

    if (NULL == p_config->tx_configuration)
    {
        return false;
    }

    mp_canif_config = p_config;

    g_canif_init = true;

    bsp_can_cb_t set_canif_cb =
        {
            .err_cb = canif_err_cb,
            .rx_cb = canif_rx_cb,
            .tx_cb = canif_tx_cb,
        };

    (void)bsp_can_cb_set(&set_canif_cb);

    g_transceiver_cfg.errn_flag = CAN_TRCV_ERR_N_FLAG_NONE;
    g_transceiver_cfg.trcv_current_mode = CAN_TRCV_MODE_STANDBY;
    g_transceiver_cfg.trcv_flag_status = CAN_TRCV_INTERNAL_FLAG_CLEARED;
    g_transceiver_cfg.trcv_old_mode = CAN_TRCV_MODE_SLEEP;

    return true;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/188057
 *
 */
bool canif_deinit(void)
{
    bool ret = bsp_can_deinit();

    if (true != ret)
    {
        return false;
    }

    g_canif_init = false;

    return true;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/188065
 *
 */
std_return_t canif_controller_error_state_get(
    canif_fault_state_t *p_error_state)
{
    if (NULL == p_error_state)
    {
        return E_NOT_OK;
    }

    bsp_can_fault_state_t status;

    bsp_can_status_t ret = bsp_can_fault_state_get(&status);

    if (BSP_CAN_STATUS_ERROR == ret)
    {
        return E_NOT_OK;
    }

    if (BSP_CAN_FAULT_STATE_ACTIVE == status)
    {
        *p_error_state = CAN_ERRORSTATE_ACTIVE;
    }
    else if (BSP_CAN_FAULT_STATE_PASSIVE == status)
    {
        *p_error_state = CAN_ERRORSTATE_PASSIVE;
    }
    else if (BSP_CAN_FAULT_STATE_BUSOFF == status)
    {
        *p_error_state = CAN_ERRORSTATE_BUSOFF;

        canif_bus_off_state();
    }
    else
    {
        return E_NOT_OK;
    }

    return E_OK;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/188058
 *
 */
std_return_t canif_transmit(pdu_id_t tx_pdu_id, const pdu_info_t *p_pdu_info)
{
    if (NULL == p_pdu_info)
    {
        return E_NOT_OK;
    }

    if (NULL == p_pdu_info->p_meta_data)
    {
        return E_NOT_OK;
    }

    if (NULL == p_pdu_info->p_sdu_data)
    {
        return E_NOT_OK;
    }

    uint8_t founded_idx = canif_id_idx_get((uint32_t)tx_pdu_id,
                                     CANIF_TRANSMISSION_TYPE_TX,
                                     CANIF_ID_TYPE_PDU);
    
    if ((uint8_t)CANIF_E_INVALID_INDEX == founded_idx)
    {
        LOG_ERROR("CANIF INVALID INDEX");
        
        return E_NOT_OK;
    }
    
    const canif_tx_pdu_cfg_t* p_current_tx_pdu_cfg =
         &(mp_canif_config->tx_configuration[founded_idx]);

    if (true == p_current_tx_pdu_cfg->canif_tx_pdu_data_length_check)
    {
        if (p_pdu_info->sdu_length != 
            p_current_tx_pdu_cfg->canif_tx_pdu_data_length)
        {
            return E_NOT_OK;
        }
    }

    bsp_can_data_t can_data;

    can_data.msg_id = (uint32_t)p_current_tx_pdu_cfg->canif_tx_pdu_can_id;

    uint8_t *p_payload_addr =  can_data.payload;

    (void)memcpy(p_payload_addr, p_pdu_info->p_sdu_data, p_pdu_info->sdu_length);

    can_data.size = p_current_tx_pdu_cfg->canif_tx_pdu_data_length;

    bsp_can_status_t ret = bsp_can_async_write(&can_data);

    if (BSP_CAN_STATUS_OK != ret)
    {
        return E_NOT_OK;
    }

    return E_OK;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/188059
 *
 */
std_return_t canif_controller_rx_error_cnt_get(uint8_t *p_rx_error_counter)
{
    bsp_can_status_t ret = bsp_can_rx_err_cnt_get(p_rx_error_counter);

    if (BSP_CAN_STATUS_OK != ret)
    {
        return E_NOT_OK;
    }

    return E_OK;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/188066
 *
 */
std_return_t canif_controller_tx_error_cnt_get(uint8_t *p_tx_error_counter)
{
    bsp_can_status_t ret = bsp_can_tx_err_cnt_get(p_tx_error_counter);

    if (BSP_CAN_STATUS_OK != ret)
    {
        return E_NOT_OK;
    }

    return E_OK;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/188064
 *
 */
void canif_tx_confirmation(can_id_t can_id)
{
    uint8_t founded_idx = canif_id_idx_get((uint32_t)can_id,
                                             CANIF_TRANSMISSION_TYPE_TX, 
                                             CANIF_ID_TYPE_MSG);

    if ((uint8_t)CANIF_E_INVALID_INDEX == founded_idx)
    {
        LOG_ERROR("CANIF INVALID INDEX");
        return;
    }
    
    const canif_tx_pdu_cfg_t* p_current_tx_pdu_cfg =
         &(mp_canif_config->tx_configuration[founded_idx]);

    pdu_id_t tx_pdu_id = p_current_tx_pdu_cfg->canif_tx_upper_pdu_id;

    if (CAN_TP_CONFIRMATION ==
        p_current_tx_pdu_cfg->canif_tx_pdu2tp_confirmation)
    {
        g_canif_generic_callouts->canif_tx_confirmation_callout(tx_pdu_id, E_OK);
    }
    else if (J1939TP_CONFIRMATION == 
             p_current_tx_pdu_cfg->canif_tx_pdu2tp_confirmation)
    {
        g_canif_generic_callouts->canif_tx_j1939_confirmation_callout(tx_pdu_id,
                                                                     E_OK);
    }
    else
    {
        return;
    }
}

/**
 * @brief https://codebeamer.daiichi.com/issue/188060
 *
 */
void canif_rx_indication(can_id_t can_id,
                         const pdu_info_t *p_pdu_info)
{
    if (NULL == p_pdu_info)
    {
        return;
    }

    uint8_t founded_idx = canif_id_idx_get((uint32_t)can_id,
                                            CANIF_TRANSMISSION_TYPE_RX,
                                            CANIF_ID_TYPE_MSG);

    if ((uint8_t)CANIF_E_INVALID_INDEX == founded_idx)
    {
        LOG_ERROR("CANIF INVALID INDEX");

        return;
    }
    
    const canif_rx_pdu_cfg_t* p_current_rx_pdu_cfg =
         &(mp_canif_config->rx_configuration[founded_idx]);

    pdu_id_t rx_pdu_id = p_current_rx_pdu_cfg->canif_rx_upper_pdu_id;

    if ((CAN_TP_INDICATION) == p_current_rx_pdu_cfg->canif_rx_pdu2tp_indication)
    {
        g_canif_generic_callouts->canif_rx_indication_callout(rx_pdu_id,
                                                             p_pdu_info);
    }
    else if ((J1939TP_INDICATION) == 
            p_current_rx_pdu_cfg->canif_rx_pdu2tp_indication)
    {
        g_canif_generic_callouts->canif_rx_j1939_indication_callout(rx_pdu_id, 
                                                                   p_pdu_info);
    }
    else
    {
        return;
    }
}

/**
 * @brief https://codebeamer.daiichi.com/issue/188092
 *
 */
void canif_bus_off_state(void)
{
    g_canif_generic_callouts->canif_bus_off_callout();
}

/**
 * @brief https://codebeamer.daiichi.com/issue/188211
 *
 */
void canif_error_notification(canif_err_status_t error_type)
{
    g_canif_generic_callouts->canif_error_callout(error_type);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/188056
 *
 */
static void canif_err_cb(bsp_can_err_status_t error)
{
    switch (error)
    {
    case BSP_CAN_ERR_STATUS_BIT_MONITORING1:
    {
        canif_error_notification(CANIF_ERR_STATUS_BIT_MONITORING1);
    }
    break;

    case BSP_CAN_ERR_STATUS_BIT_MONITORING2:
    {
        canif_error_notification(CANIF_ERR_STATUS_BIT_MONITORING2);
    }
    break;

    case BSP_CAN_ERR_STATUS_CHECK_ACK_FAILED:
    {
        canif_error_notification(CANIF_ERR_STATUS_CHECK_ACK_FAILED);
    }
    break;

    case BSP_CAN_ERR_STATUS_CHECK_FORM_FAILED:
    {
        canif_error_notification(CANIF_ERR_STATUS_CHECK_FORM_FAILED);
    }
    break;

    case BSP_CAN_ERR_STATUS_CHECK_STUFFING_FAILED:
    {
        canif_error_notification(CANIF_ERR_STATUS_CHECK_STUFFING_FAILED);
    }
    break;

    case BSP_CAN_ERR_STATUS_CHECK_CRC_FAILED:
    {
        canif_error_notification(CANIF_ERR_STATUS_CHECK_CRC_FAILED);
    }
    break;

    default:
    {
        canif_error_notification(CANIF_ERR_STATUS_NONE);
    }
    break;
    }
}

/**
 * @brief https://codebeamer.daiichi.com/issue/188496
 * 
 */
static uint8_t canif_rx_msg_idx_get(uint32_t id_value, canif_id_type_t id_type)
{
    bool found = false;

    uint8_t index = 0U;

    for (uint8_t cnt = 0U; (true != found) && (cnt < CANIF_RX_MSG_CNT); cnt++)
    {
        if (CANIF_ID_TYPE_MSG == id_type)
        {
            if ((mp_canif_config->rx_configuration[cnt].
                 canif_rx_pdu_can_id_mask & id_value) == 
                (mp_canif_config->rx_configuration[cnt].
                 canif_rx_pdu_can_id_mask &
                 mp_canif_config->rx_configuration[cnt].canif_rx_pdu_can_id))
            {
                found = true;
                index = cnt;
            }
        }
        else if (CANIF_ID_TYPE_PDU == id_type)
        {
            if (mp_canif_config->rx_configuration[cnt].canif_rx_pdu_id ==
                id_value)
            {
                found = true;
                index = cnt;
            }
        }
        else
        {
            return (uint8_t)CANIF_E_INVALID_INDEX;
        }
    }

    if (found == false)
    {
        return (uint8_t)CANIF_E_INVALID_INDEX;
    }

    return index;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/188497
 * 
 */
static uint8_t canif_tx_msg_idx_get(uint32_t id_value, canif_id_type_t id_type)
{    
    bool found = false;

    uint8_t index = 0U;

    for (uint8_t cnt = 0U; (true != found) && (cnt < CANIF_TX_MSG_CNT); cnt++)
    {
        if (CANIF_ID_TYPE_MSG == id_type)
        {
            if (mp_canif_config->tx_configuration[cnt].canif_tx_pdu_can_id ==
                id_value)
            {
                found = true;
                index = cnt;
            }
        }
        else if (CANIF_ID_TYPE_PDU == id_type)
        {
            if (mp_canif_config->tx_configuration[cnt].canif_tx_pdu_id ==
                id_value)
            {
                found = true;
                index = cnt;
            }
        }
        else
        {
            return (uint8_t)CANIF_E_INVALID_INDEX;
        }
    }

    if (found == false)
    {
        return (uint8_t)CANIF_E_INVALID_INDEX;
    }

    return index;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/188132
 *
 */
static uint8_t canif_id_idx_get(uint32_t id_value,
                                canif_transmission_type_t transmit_type,
                                canif_id_type_t id_type)
{
    if (0U == id_value)
    {
        return (uint8_t)CANIF_E_INVALID_INDEX;
    }

    uint8_t index = 0U;

    if (CANIF_TRANSMISSION_TYPE_RX == transmit_type)
    {
        index = canif_rx_msg_idx_get(id_value, id_type);
    }
    else if (CANIF_TRANSMISSION_TYPE_TX == transmit_type) 
    {   
        index = canif_tx_msg_idx_get(id_value, id_type);
    }
    else
    {
        return (uint8_t)CANIF_E_INVALID_INDEX;
    }

    if ((uint8_t)CANIF_E_INVALID_INDEX == index)
    {
        return (uint8_t)CANIF_E_INVALID_INDEX;
    }

    return index;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/188062
 *
 */
static void canif_rx_cb(bsp_can_data_t *p_rx_data)
{
    if (NULL == p_rx_data)
    {
        return;
    }

    uint8_t founded_idx = canif_id_idx_get(p_rx_data->msg_id,
                                           CANIF_TRANSMISSION_TYPE_RX,
                                           CANIF_ID_TYPE_MSG);

    if ((uint8_t)CANIF_E_INVALID_INDEX == founded_idx)
    {
        LOG_ERROR("CANIF INVALID INDEX");

        return;
    }

    const canif_rx_pdu_cfg_t* p_current_rx_pdu_cfg =
         &(mp_canif_config->rx_configuration[founded_idx]);

    if (true == p_current_rx_pdu_cfg->canif_rx_pdu_data_length_check)
    {
        if (p_rx_data->size != p_current_rx_pdu_cfg->canif_rx_pdu_data_length)
        {
            return;
        }
    }

    j1939_if_meta_data_t meta_data;

    meta_data.can_id = (uint32_t)p_rx_data->msg_id;

    m_canif_data_info.p_meta_data = m_can_id_array;

    (void)memcpy(m_canif_data_info.p_meta_data,
                 (uint8_t *)&meta_data.can_id,
                 sizeof(uint8_t));

    m_canif_data_info.p_sdu_data = p_rx_data->payload;

    (void)memcpy(m_canif_data_info.p_sdu_data,
                 p_rx_data->payload,
                 p_rx_data->size);

    m_canif_data_info.sdu_length = (uint16_t)p_rx_data->size;

    canif_rx_indication((can_id_t)p_rx_data->msg_id, &m_canif_data_info);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/188063
 *
 */
static void canif_tx_cb(const bsp_can_data_t *p_tx_data)
{
    if (NULL == p_tx_data)
    {
        return;
    }

    uint8_t founded_idx = canif_id_idx_get(p_tx_data->msg_id,
                                     CANIF_TRANSMISSION_TYPE_TX,
                                     CANIF_ID_TYPE_MSG);
    
    if ((uint8_t)CANIF_E_INVALID_INDEX == founded_idx)
    {
        LOG_ERROR("CANIF INVALID INDEX");

        return;
    }
    
    const canif_tx_pdu_cfg_t* p_current_tx_pdu_cfg =
         &(mp_canif_config->tx_configuration[founded_idx]);

    if (true == p_current_tx_pdu_cfg->canif_tx_pdu_data_length_check)
    {
        if (p_tx_data->size != p_current_tx_pdu_cfg->canif_tx_pdu_data_length)
        {
            return;
        }
    }
    
    canif_tx_confirmation((can_id_t)p_tx_data->msg_id);
}

// end of file
