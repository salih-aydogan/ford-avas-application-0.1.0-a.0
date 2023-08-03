/**
 * @file bsp_can.h
 * @author Utku Cecen (utku.cecen@daiichi.com)
 * @brief
 * @version 0.1
 * @date 2023-02-02
 *
 * @copyright Copyright (c) 2023
 *
 * https://codebeamer.daiichi.com/issue/177714
 *
 */

#ifndef BSP_CAN_H
#define BSP_CAN_H

#include "daiichi_types.h"

/*******************************************************************************
 * Macros
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/179946
 *
 * Indicates the CAN-BUS speed.
 *
 */
#define BSP_CAN_CFG_BITRATE_BIT 500000U

/**
 * @brief https://codebeamer.daiichi.com/issue/180251
 *
 * Indicates the count of the message IDs.
 *
 */
#define BSP_CAN_CFG_FILTER_IDS_TBL_MAX 32U

/**
 * @brief https://codebeamer.daiichi.com/issue/180254
 *
 * Indicates the size of the mailboxes specified in bytes.
 *
 */
#define BSP_CAN_CFG_PAYLOAD_SIZE 8U

/*******************************************************************************
 * Enumerations
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/187395
 *
 * List of CAN-BUS fault confinement states.
 *
 */
typedef enum bsp_can_fault_state_e
{
    BSP_CAN_FAULT_STATE_ACTIVE = 0U,
    BSP_CAN_FAULT_STATE_PASSIVE = 1U,
    BSP_CAN_FAULT_STATE_BUSOFF = 2U,
    BSP_CAN_FAULT_STATE_NONE = 3U

} bsp_can_fault_state_t;

/**
 * @brief https://codebeamer.daiichi.com/issue/179875
 *
 * List of CAN-BUS generic operation status.
 *
 */
typedef enum bsp_can_status_e
{
    BSP_CAN_STATUS_OK = 0U,
    BSP_CAN_STATUS_BUSY = 1U,
    BSP_CAN_STATUS_HW_ERROR = 2U,
    BSP_CAN_STATUS_ERROR = 3U,

} bsp_can_status_t;

/**
 * @brief https://codebeamer.daiichi.com/issue/187332
 *
 * List of CAN-BUS error statuses.
 *
 */
typedef enum bsp_can_err_status_e
{
    BSP_CAN_ERR_STATUS_BIT_MONITORING1 = 0x01U,
    BSP_CAN_ERR_STATUS_BIT_MONITORING2 = 0x02U,
    BSP_CAN_ERR_STATUS_CHECK_ACK_FAILED = 0x03U,
    BSP_CAN_ERR_STATUS_CHECK_FORM_FAILED = 0x04U,
    BSP_CAN_ERR_STATUS_CHECK_STUFFING_FAILED = 0x05U,
    BSP_CAN_ERR_STATUS_CHECK_CRC_FAILED = 0x06U,

} bsp_can_err_status_t;

/*******************************************************************************
 * Structs and Type Definitions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/180366
 *
 * Message buffer information structure.
 *
 */
typedef struct bsp_can_data_s
{
    uint32_t msg_id;
    uint8_t payload[BSP_CAN_CFG_PAYLOAD_SIZE];
    size_t size;

} bsp_can_data_t;

/**
 * @brief https://codebeamer.daiichi.com/issue/187403
 *
 * Function pointer type for CAN-BUS error function specified when registering
 * to the CAN via bsp_can_cb_set.
 *
 */
typedef void (*bsp_can_rx_cb_t)(bsp_can_data_t *p_rx_data);

/**
 * @brief https://codebeamer.daiichi.com/issue/187324
 *
 * Function pointer type for CAN-BUS transmit function specified when
 * registering to the CAN via bsp_can_cb_set.
 *
 */
typedef void (*bsp_can_tx_cb_t)(const bsp_can_data_t *p_tx_data);

/**
 * @brief https://codebeamer.daiichi.com/issue/187325
 *
 * Function pointer type for CAN-BUS error function specified when registering
 * to the CAN via bsp_can_cb_set.
 *
 */
typedef void (*bsp_can_err_cb_t)(bsp_can_err_status_t err);

/**
 * @brief https://codebeamer.daiichi.com/issue/187411
 *
 * Structure of callback function pointers.
 *
 */
typedef struct bsp_can_cb_s
{
    bsp_can_rx_cb_t rx_cb;
    bsp_can_tx_cb_t tx_cb;
    bsp_can_err_cb_t err_cb;

} bsp_can_cb_t;

/*******************************************************************************
 * Public Variables
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/179872
 *
 * Initializes the CAN-BUS and activates the CAN transceiver.
 *
 * @return true Success
 * @return false CAN-BUS not initiated properly
 */
bool bsp_can_init(void);

/**
 * @brief https://codebeamer.daiichi.com/issue/179871
 *
 * De-initializes the CAN-BUS.
 *
 * @return true Success
 * @return false CAN-BUS is not de-initiated properly
 */
bool bsp_can_deinit(void);

/**
 * @brief https://codebeamer.daiichi.com/issue/179869
 *
 * Sends a CAN frame using the specified message buffer.
 *
 * @param[in] p_tx Holds Transmitted message information.
 *
 * @return bsp_can_status_t
 *
 * @retval BSP_CAN_STATUS_OK            	No error.
 * @retval BSP_CAN_STATUS_BUSY              The relevant message buffer used
 *                                          by the transfer.
 * @retval BSP_CAN_STATUS_HW_ERROR          Hardware error
 * @retval BSP_CAN_STATUS_ERROR            	Transmit parameter is NULL value or
 *                                          generic operation failure status
 */
bsp_can_status_t bsp_can_async_write(const bsp_can_data_t *p_tx);

/**
 * @brief https://codebeamer.daiichi.com/issue/179868
 *
 * Configures the ID filter table element, receives a CAN frame using the
 * message FIFO and enables RX FIFO(First in first out) interrupts.
 *
 * @param p_msg_ids Holds filtered ID from user.
 */
void bsp_can_filter(const uint32_t *p_msg_ids);

/**
 * @brief https://codebeamer.daiichi.com/issue/187336
 *
 *  Registers the callback functions for CAN-BUS transfer and error operations.
 *
 * @param[in] p_cb
 * @retval BSP_CAN_STATUS_OK            	No error.
 * @retval BSP_CAN_STATUS_ERROR            	Transmit parameter is NULL value or
 *                                          generic operation failure status
 */
bsp_can_status_t bsp_can_cb_set(const bsp_can_cb_t *p_cb);

/**
 * @brief https://codebeamer.daiichi.com/issue/187343
 *
 *  This function gives the fault confinement state of the CAN-BUS.
 *
 * @param[in] p_err_state
 * @retval BSP_CAN_STATUS_OK            	No error.
 * @retval BSP_CAN_STATUS_ERROR            	Transmit parameter is NULL value or
 *                                          generic operation failure status
 */
bsp_can_status_t bsp_can_fault_state_get(bsp_can_fault_state_t *p_state);

/**
 * @brief https://codebeamer.daiichi.com/issue/187344
 *
 *  This function gives the CAN-BUS Rx error counter value.
 *
 * @param[in] p_err_state
 * @retval BSP_CAN_STATUS_OK            	No error.
 * @retval BSP_CAN_STATUS_ERROR            	Transmit parameter is NULL value or
 *                                          generic operation failure status
 */
bsp_can_status_t bsp_can_rx_err_cnt_get(uint8_t *p_err_cnt);
/**
 * @brief https://codebeamer.daiichi.com/issue/187345
 *
 *  This function gives the CAN-BUS Tx error counter value.
 *
 * @param[in] p_err_state
 * @retval BSP_CAN_STATUS_OK            	No error.
 * @retval BSP_CAN_STATUS_ERROR            	Transmit parameter is NULL value or
 *                                          generic operation failure status
 */
bsp_can_status_t bsp_can_tx_err_cnt_get(uint8_t *p_err_cnt);

#endif /* BSP_CAN_H */

// end of file
