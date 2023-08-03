/**
 * @file canif.h
 * @author Burak Utku Cecen (utku.cecen@daiichi.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-30
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef PUBLIC_DRIVER_CANIF_H
#define PUBLIC_DRIVER_CANIF_H

#include "comstack_types.h"

/*******************************************************************************
 * Enumerations
 ******************************************************************************/

/**
 * @brief List of error states.
 * 
 * https://codebeamer.daiichi.com/issue/188212
 * 
 */
typedef enum canif_err_status_e
{
    CANIF_ERR_STATUS_NONE = 0x0U,
    CANIF_ERR_STATUS_BIT_MONITORING1 = 0x01U,
    CANIF_ERR_STATUS_BIT_MONITORING2 = 0x02U,
    CANIF_ERR_STATUS_CHECK_ACK_FAILED = 0x03U,
    CANIF_ERR_STATUS_CHECK_FORM_FAILED = 0x04U,
    CANIF_ERR_STATUS_CHECK_STUFFING_FAILED = 0x05U,
    CANIF_ERR_STATUS_CHECK_CRC_FAILED = 0x06U,
    
} canif_err_status_t;

/**
 * @brief List of fault states.
 * 
 * https://codebeamer.daiichi.com/issue/187541
 * 
 */
typedef enum canif_fault_state_e
{
    CAN_ERRORSTATE_ACTIVE = 0,
    CAN_ERRORSTATE_PASSIVE,
    CAN_ERRORSTATE_BUSOFF,
    CAN_ERRORSTATE_NONE,

} canif_fault_state_t;

/**
 * @brief List of wake up reasons.
 * 
 * https://codebeamer.daiichi.com/issue/186404
 * 
 */
typedef enum can_trcv_wakeup_reason_e
{
    CANTRCV_WU_ERROR = 0x00U, 
    CANTRCV_WU_NOT_SUPPORTED = 0x01U, 
    CANTRCV_WU_BY_BUS,
    CANTRCV_WU_INTERNALLY, 
    CANTRCV_WU_RESET, 
    CANTRCV_WU_POWER_ON,
    CANTRCV_WU_BY_PIN,
    CANTRCV_WU_BY_SYSERR,
    
} can_trcv_wakeup_reason_t;

/**
 * @brief List of TJA1043 CAN Transceiver's internal flags.
 * 
 * https://codebeamer.daiichi.com/issue/186947
 * 
 */
typedef enum can_trcv_errn_flag_e
{
    CAN_TRCV_ERR_N_FLAG_NONE,
    CAN_TRCV_BUS_FAILURE_FLAG = 1,
    CAN_TRCV_WAKEUP_SOURCE_FLAG,
    CAN_TRCV_PWON_FLAG,
    CAN_TRCV_LOCAL_FAILURE_FLAG,
    CAN_TRCV_WAKEUP_FLAG,

} can_trcv_errn_flag_t;

/**
 * @brief List of TJA1043 CAN Transceiver's internal flag status.
 * 
 * https://codebeamer.daiichi.com/issue/186991
 * 
 */
typedef enum can_trcv_flag_status_e
{
    CAN_TRCV_INTERNAL_FLAG_CLEARED = 0,
    CAN_TRCV_INTERNAL_FLAG_SET,

} can_trcv_flag_status_t;

/**
 * @brief List of transport layer types for message confirmation.
 * 
 * https://codebeamer.daiichi.com/issue/186403
 * 
 */
typedef enum tp_confirmation_e
{
    CAN_TP_CONFIRMATION,
    J1939TP_CONFIRMATION,

} tp_confirmation_t;

/**
 * @brief List of transport layer types for message indication.
 * 
 * https://codebeamer.daiichi.com/issue/186402
 * 
 */
typedef enum tp_indication_type_e
{
    CAN_TP_INDICATION,
    J1939TP_INDICATION,

} tp_indication_type_t;

/**
 * @brief List of transceiver modes.
 * 
 * https://codebeamer.daiichi.com/issue/186399
 * 
 */
typedef enum can_trcv_mode_e
{
    CAN_TRCV_MODE_NORMAL = 0,
    CAN_TRCV_MODE_STANDBY,
    CAN_TRCV_MODE_SLEEP,
    CAN_TRCV_MODE_LISTENONLY,
    CAN_TRCV_MODE_GOTOSLEEP,

} can_trcv_mode_t;

/*******************************************************************************
 * Type Definitions
 ******************************************************************************/

/**
 * @brief This function pointer type used to define a callback function that
 * will be invoked when a message is received.
 * 
 * https://codebeamer.daiichi.com/issue/186641
 * 
 */
typedef void (*canif_rx_user_indication_co_t)(can_id_t can_id,
                                              const pdu_info_t* pdu_info); 

/**
 * @brief This function pointer type used to define a callback function that
 * will be invoked when a message is transmitted.
 * 
 * https://codebeamer.daiichi.com/issue/186639
 * 
 */
typedef void (*canif_tx_pdu_user_confirm_co_t)(can_id_t can_id,
                                               std_return_t status); 

/**
 * @brief This function pointer type used to define callback function that 
 * will be invoked when any error occurred.
 * 
 * https://codebeamer.daiichi.com/issue/186638
 * 
 */
typedef void (*canif_error_co_t)(canif_err_status_t error);

/**
 * @brief This function pointer type used to define callback function that will
 * be invoked when bus off state occured.
 * 
 * https://codebeamer.daiichi.com/issue/186640
 * 
 */
typedef void (*canif_bus_off_co_t)(void);

/**
 * @brief This function pointer type used to define callback function that will
 * be invoked to when wake flag set.
 * 
 * https://codebeamer.daiichi.com/issue/186644
 * 
 */
typedef void (*canif_trcv_wake_flag_co_t)(void);

/**
 * @brief This function pointer type used to define callback function that will
 * be invoked to when transceiver mode changed.
 * 
 * https://codebeamer.daiichi.com/issue/186642
 * 
 */
typedef void (*canif_trcv_mode_co_t)(can_trcv_mode_t current_mode);

/*******************************************************************************
 * Structs
 ******************************************************************************/

/**
 * @brief Callouts structure contains all callouts.
 * 
 * https://codebeamer.daiichi.com/issue/186725
 * 
 */
typedef struct canif_callout_s
{
    canif_rx_user_indication_co_t canif_rx_indication_callout;

    canif_rx_user_indication_co_t canif_rx_j1939_indication_callout;

    canif_tx_pdu_user_confirm_co_t canif_tx_confirmation_callout;

    canif_tx_pdu_user_confirm_co_t canif_tx_j1939_confirmation_callout;

    canif_error_co_t canif_error_callout;

    canif_bus_off_co_t canif_bus_off_callout;

    canif_trcv_wake_flag_co_t canif_wake_flag_callout;

    canif_trcv_mode_co_t canif_trcv_mode_callout;

} canif_callout_t;

/**
 * @brief Contains TX data configuration parameters.
 * 
 * https://codebeamer.daiichi.com/issue/186392
 * 
 */
typedef struct canif_tx_pdu_cfg_s
{
    pdu_id_t canif_tx_pdu_id;

    can_id_t canif_tx_pdu_can_id; 

    pdu_id_t canif_tx_upper_pdu_id;

    bool canif_tx_pdu_data_length_check;

    tp_confirmation_t canif_tx_pdu2tp_confirmation;

    uint32_t canif_tx_pdu_data_length;

} canif_tx_pdu_cfg_t;

/**
 * @brief This code defines a data structure for configuring CAN receive PDUs in 
 * the CAN Interface
 * 
 * https://codebeamer.daiichi.com/issue/186393
 * 
 */
typedef struct canif_rx_pdu_cfg_s
{
    can_id_t canif_rx_pdu_can_id;

    pdu_id_t canif_rx_upper_pdu_id;

    uint32_t canif_rx_pdu_can_id_mask;

    bool canif_rx_pdu_data_length_check;

    uint8_t canif_rx_pdu_data_length;

    pdu_id_t canif_rx_pdu_id;

    tp_indication_type_t canif_rx_pdu2tp_indication;

} canif_rx_pdu_cfg_t;

/**
 * @brief  Defines a data structure representing the configuration, containing
 *  pointers to the RX and TX protocol data unit (PDU) configurations.
 * 
 * https://codebeamer.daiichi.com/issue/186394
 * 
 */
typedef struct canif_config_s
{
    const canif_rx_pdu_cfg_t* rx_configuration;

    const canif_tx_pdu_cfg_t* tx_configuration;

} canif_config_t;

/**
 * @brief This structure holds various information related to a CAN transceiver. 
 * 
 * https://codebeamer.daiichi.com/issue/186993
 * 
 */
typedef struct can_trcv_info_s
{   
    can_trcv_errn_flag_t errn_flag; // Specified ERR_N pin flags.

    can_trcv_mode_t trcv_current_mode; // Transceiver current mode.

    can_trcv_mode_t trcv_old_mode; // Transceiver mode before changed.

    can_trcv_flag_status_t trcv_flag_status; // flag status cleared - set

} can_trcv_info_t;

/*******************************************************************************
 * Public Variables
 ******************************************************************************/

/**
 * @brief This variable provides notification to upper layers for callouts.
 * This variable provides access to callout functions.
 */
extern const canif_callout_t g_canif_generic_callouts[];

/**
 * @brief This variable used for sets general CANIF configuration.
 * 
 */
extern const canif_config_t g_canif_config;

/**
 * @brief This variable holds transceivers initial informations.
 * 
 */
extern can_trcv_info_t g_transceiver_cfg;

/**
 * @brief This variable specified initialization status.
 * 
 */
extern bool g_canif_init;

/*******************************************************************************
 * Public Functions 
 ******************************************************************************/

/**
 * @brief Initializes the CAN interfaces.
 * 
 * https://codebeamer.daiichi.com/issue/186284
 * 
 * @param p_config 
 * @return true 
 * @return false 
 */
bool canif_init(const canif_config_t* p_config);

/**
 * @brief De initialize the CAN interface
 * 
 * https://codebeamer.daiichi.com/issue/186285
 * 
 * @return true 
 * @return false 
 */
bool canif_deinit(void);

/**
 * @brief Reports the BUS error status. This function calls the corresponding 
 * BSP CAN service for obtaining the error state of the CAN controller.
 * 
 * https://codebeamer.daiichi.com/issue/186286
 * 
 * @param p_error_state Pointer to a memory location, where the error state of 
 * the CAN controller will be stored.
 * 
 * @return std_return_t     E_OK:     Error state request has been accepted.
 *                          E_NOT_OK: Error state request has not been accepted.
 */
std_return_t canif_controller_error_state_get(
    canif_fault_state_t* p_error_state);

/**
 * @brief This function provides request transmission of a PDU.
 * 
 * https://codebeamer.daiichi.com/issue/186287
 * 
 * @param tx_pdu_id The identifier of the transmitted PDU.
 * @param p_pdu_info A pointer to the PDU information associated with the 
 * transmitted.
 * 
 * @return std_return_t E_OK: Transmit request has been accepted.
 *                      E_NOT_OK: Transmit request has not been accepted.
 */
std_return_t canif_transmit(pdu_id_t tx_pdu_id, const pdu_info_t* p_pdu_info);

/**
 * @brief This function provides mode changes of TJA-1043 CAN transceiver.
 * 
 * https://codebeamer.daiichi.com/issue/186289
 * 
 * @param trcv_mode_req Contains the desired operating mode.
 * 
 * @return std_return_t E_OK: The request for transceiver mode change has been 
 *                      accepted.
 *                      E_NOT_OK: The request for transceiver mode change has 
 *                      not been accepted.
 */
std_return_t canif_trcv_mode_set(can_trcv_mode_t trcv_mode_req);

/**
 * @brief This function reports the current transceiver mode.
 * 
 * https://codebeamer.daiichi.com/issue/186290
 * 
 * @param p_current_trcv_mode Pointer to which API call has to be applied.
 * 
 * @return std_return_t E_OK: Operating mode was detected.
 *                      E_NOT_OK: Operation mode was not detected.
 */
std_return_t canif_trcv_mode_get(can_trcv_mode_t* p_current_trcv_mode);

/**
 * @brief This function reports wake up reason of the transceiver.
 * 
 * https://codebeamer.daiichi.com/issue/186291
 * 
 * @param p_trcv_wu_reason Pointer to which API call has to be applied.
 * 
 * @return std_return_t E_OK: wake-up reason request has been accepted.
 *                      E_NOT_OK: wake-up reason request has not been accepted.
 * 
 */
std_return_t canif_trcv_wakeup_reason_get(
    can_trcv_wakeup_reason_t* p_trcv_wu_reason);

/**
 * @brief This function calls the corresponding BSP CAN service for obtaining
 * the RX error counter of the CAN controller.
 * 
 * https://codebeamer.daiichi.com/issue/186293
 * 
 * @param p_rx_error_counter Pointer to a memory location, where the current Rx
 *  error counter of the CAN controller will be stored.
 * 
 * @return std_return_t E_OK: Rx error counter available.
 *                      E_NOT_OK: Rx error counter not available.
 */
std_return_t canif_controller_rx_error_cnt_get(uint8_t* p_rx_error_counter);

/**
 * @brief This function calls the corresponding BSP CAN service for obtaining
 * the TX error counter of the CAN controller.
 * 
 * https://codebeamer.daiichi.com/issue/186294
 * 
 * @param p_tx_error_counter  Pointer to a memory location, where the current TX
 * error counter of the CAN controller will be stored.
 * 
 * @return std_return_t  E_OK: Tx error counter available.
 *                       E_NOT_OK: Tx error counter not available.
 */
std_return_t canif_controller_tx_error_cnt_get(uint8_t* p_tx_error_counter);

/**
 * @brief This function confirms transmission of a CAN message with a spesific
 * identifier.
 *  
 * https://codebeamer.daiichi.com/issue/186295
 * 
 * @param can_id The Identifier of the transmitted CAN message.
 */
void canif_tx_confirmation(can_id_t can_id);

/**
 * @brief This function is called when a CAN message is received, passing the
 * identifier of the received message and a pointer to the PDU (Protocol Data 
 * Unit) information associated with it as parameters.
 * 
 * https://codebeamer.daiichi.com/issue/186296
 * 
 * @param can_id The identifier of the received CAN message 
 * @param p_pdu_info A pointer to the PDU information associated with the 
 * received
 */
void canif_rx_indication(can_id_t can_id,
                         const pdu_info_t* p_pdu_info);

/**
 * @brief This function calls the corresponding callout function for the notify 
 * the bus off state.
 * 
 * https://codebeamer.daiichi.com/issue/186297
 * 
 */
void canif_bus_off_state(void);

/**
 * @brief Notifies the canif module of an error event.
 * 
 * https://codebeamer.daiichi.com/issue/186298
 * 
 * @param error_type The type of error that has occurred, as a value of the 
 * canif_err_status_t enumeration.
 * 
 * @return * void 
 */
void canif_error_notification(canif_err_status_t error_type);

/**
 * @brief This function calls the corresponding callout function when the wake 
 * flag is set.
 * 
 * https://codebeamer.daiichi.com/issue/186300
 * 
 */
void canif_trcv_wake_flag_indication(void);

/**
 * @brief This function calls the corresponding callout function when the 
 * change of transceiver mode.
 * 
 * https://codebeamer.daiichi.com/issue/186301
 * 
 * @param transceiver_mode Specified TJA1043 CAN transceiver mode.
 */
void canif_trcv_mode_indication(can_trcv_mode_t transceiver_mode);

/**
 * @brief This function reports occured diagnostic information according to 
 * transceivers internal flags.
 * 
 * https://codebeamer.daiichi.com/issue/186946
 * 
 */
void canif_trcv_main_func_diagnostics(void);

#endif /* PUBLIC_DRIVER_CANIF_H */

// end of file
