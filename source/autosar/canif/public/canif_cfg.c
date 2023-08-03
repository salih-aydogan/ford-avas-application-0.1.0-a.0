#include "canif.h"
#include "log.h"


LOG_MODULE_NAME("CANIF_CFG");

/*******************************************************************************
 *                          CALLOUT TEST FUNCTION DEC
*******************************************************************************/

 void rx_j1939_indication_callout_test(can_id_t can_id, 
                                      const pdu_info_t* pdu_info);

 void rx_indication_callout_test(can_id_t can_id,
                                const pdu_info_t* pdu_info);
                                      
 void tx_j1939_confirmation_callout_test(can_id_t can_id,
                                        std_return_t status);
                                        
 void tx_confirmation_callout_test(can_id_t can_id,
                                  std_return_t status);

 void trcv_mode_callout_test(can_trcv_mode_t current_mode); 

 void wake_flag_callout_test(void);

 void bus_off_callout_test(void);

 void error_callout_test(canif_err_status_t error);

/*******************************************************************************
 *                          CANIF CONFIGURATIONS
*******************************************************************************/

const canif_tx_pdu_cfg_t canif_tx_pdu_cfg[] = 
{
    //todo
};

const canif_rx_pdu_cfg_t canif_rx_pdu_cfg[] = 
{
    [0U] =
    {
        /*ETC2 for TransCurrentGear*/
        .canif_rx_pdu2tp_indication = J1939TP_INDICATION,
        .canif_rx_pdu_can_id = 0x18F00503, 
        .canif_rx_pdu_can_id_mask = 0xFF,
        .canif_rx_pdu_data_length = 8U,
        .canif_rx_pdu_data_length_check = false,
        .canif_rx_pdu_id = 0U,
        .canif_rx_upper_pdu_id = 0U,

    },
    [1U] =
    {
        /*TCO1_TACHOGRAPH for VehicleSpeed*/
        .canif_rx_pdu2tp_indication = J1939TP_INDICATION,
        .canif_rx_pdu_can_id = 0xCFE6CEE, 
        .canif_rx_pdu_can_id_mask = 0xFF,
        .canif_rx_pdu_data_length = 8U,
        .canif_rx_pdu_data_length_check = true,
        .canif_rx_pdu_id = 1U,
        .canif_rx_upper_pdu_id = 1U,

    },
    [2U] =
    {   /*VCU_HMI_Info for EV_Ready*/
        .canif_rx_pdu2tp_indication = J1939TP_INDICATION,
        .canif_rx_pdu_can_id = 0x18FF7B00, 
        .canif_rx_pdu_can_id_mask = 0xFF,
        .canif_rx_pdu_data_length = 8U,
        .canif_rx_pdu_data_length_check = true,
        .canif_rx_pdu_id = 2U,
        .canif_rx_upper_pdu_id = 2U,
    },
};

const canif_callout_t g_canif_generic_callouts[] = 
{
    [0U] =
    {
        .canif_bus_off_callout = bus_off_callout_test,
        .canif_error_callout = error_callout_test,
        .canif_rx_indication_callout = rx_indication_callout_test,
        .canif_rx_j1939_indication_callout = rx_j1939_indication_callout_test,
        .canif_trcv_mode_callout = trcv_mode_callout_test,
        .canif_tx_confirmation_callout = tx_confirmation_callout_test,
        .canif_tx_j1939_confirmation_callout = tx_j1939_confirmation_callout_test,
        .canif_wake_flag_callout = wake_flag_callout_test,
    },
};

const canif_config_t g_canif_config =
{
    .rx_configuration = canif_rx_pdu_cfg,
    .tx_configuration = canif_tx_pdu_cfg,
};

//todo silinecek 
/*******************************************************************************
 *                             CALLOUT TEST FUNCTION DEF
*******************************************************************************/

void rx_j1939_indication_callout_test(can_id_t can_id,
                                      const pdu_info_t* pdu_info)
{
    (void)can_id;
    (void)pdu_info;

    LOG_DEBUG("rx_j1939_indication_callout");
}

void tx_j1939_confirmation_callout_test(can_id_t can_id,
                                        std_return_t status)
{
    (void)can_id;
    (void)status;

    LOG_DEBUG("tx_j1939_confirmation_callout");
}

void rx_indication_callout_test(can_id_t can_id,
                                const pdu_info_t* pdu_info)
{
    (void)can_id;
    (void)pdu_info;

    LOG_DEBUG("rx_indication_callout - canTp");
}

void tx_confirmation_callout_test(can_id_t can_id,
                                  std_return_t status)
{
    (void)can_id;
    (void)status;

    LOG_DEBUG("tx_confirmation_callout - canTp");
}

void wake_flag_callout_test(void)
{
    LOG_DEBUG("wake_flag_callout");
}

void trcv_mode_callout_test(can_trcv_mode_t current_mode)
{
    (void)current_mode;

    LOG_DEBUG("trcv_mode_callout");
}

void bus_off_callout_test(void)
{
    LOG_ERROR("bus_off_callout");
}

void error_callout_test(canif_err_status_t error)
{
    (void)error;

    LOG_ERROR("error_callout");
}

// end of file
