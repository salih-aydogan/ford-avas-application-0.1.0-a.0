/**
 * @file test_canif.c
 * @author Muhammed Salih AYDOGAN
 * @brief These tests verify the units of the CANIF module.
 * 
 * @version 0.1
 * 
 * @date 03/05/2023
 * 
 * @copyright Copyright (c) 2023
 * 
 * https://codebeamer.daiichi.com/issue/191261
 * 
 */

/*******************************************************************************
 * Includes 
 ******************************************************************************/

//First - include test framework headers
#include "unity.h"
#include "support.h"

// Second - include the module to be tested
#include "canif_cfg.c"

// Third - include the mocks of required modules
#include "mock_bsp_can.h"

#include "mock_log.h"

/*******************************************************************************
 * Test Files
 ******************************************************************************/

TEST_FILE("canif.c");

/*******************************************************************************             
 * Defines
 ******************************************************************************/

/**
 * @brief Number of TX message count.
 *
 */
#define TEST_CANIF_TX_MSG_CNT               2U

/**
 * @brief Number of RX message count.
 * 
 */
#define TEST_CANIF_RX_MSG_CNT               5U

/**
 * @brief Specified TX and RX configuration table invalid index value.
 * 
 */
#define TEST_E_INVALID_INDEX                0xFF

/*******************************************************************************
 * Extern Variables
 ******************************************************************************/

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/**
 * @brief This variable is a function pointer create for rx callback.
 *
 */
static bsp_can_rx_cb_t m_test_can_rx_cb;

/**
 * @brief This variable is a function pointer create for tx callback.
 *
 */
static bsp_can_tx_cb_t m_test_can_tx_cb;

/**
 * @brief This variable is a function pointer create for error callback.
 *
 */
static bsp_can_err_cb_t m_test_can_err_cb;

/**
 * @brief This variable is used to compare the parameters entered into the 
 * function.
 *
 */
static bsp_can_data_t m_test_can_data;

/*******************************************************************************
 * Global Variables
 ******************************************************************************/

/*******************************************************************************
 * Private Functions Prototype Declarations
 ******************************************************************************/

/**
 * @brief Helper of canif init function uses for success initialization.
 * 
 */
static void helper_canif_init(void);

/**
 * @brief Helper of canif init function uses for tx configuration.
 * 
 */
static void helper_canif_init_with_tx_config(uint8_t number);

/**
 * @brief Helper of canif init function uses for rx configuration.
 * 
 */
static void helper_canif_init_with_rx_config(uint8_t number);

/**
 * @brief Callback of bsp_can_cb_set function uses for get address of callback 
 * functions
 * 
 */
static bsp_can_status_t test_cb_bsp_can_cb_set(const bsp_can_cb_t* p_cb, 
                                               int cmock_num_calls);

/**
 * @brief This function is used instead of bsp_can_async_write function.
 * 
 */
static bsp_can_status_t test_bsp_can_async_write(const bsp_can_data_t* p_tx, 
                                                 int cmock_num_calls);

/*******************************************************************************
 * Setup & Teardown 
 ******************************************************************************/

void setUp()
{
    log_print_Ignore();
}

void tearDown()
{
}

void suiteSetUp(void)
{
}

int suiteTearDown(int num_failures)
{
    (void)num_failures;
    return 0;
}

/*******************************************************************************
 * Test Functions
 ******************************************************************************/

/**
 * @brief The test verifies the canif_init() function by calling the 
 * bsp_can_init() and bsp_can_cb_set() functions successfully.
 * 
 * https://codebeamer.daiichi.com/issue/191287
 *  
 */
void test_canif_init__usual_run__success(void)
{
    TEST_CASE();

    //**************************** Input Params **************************** //

    canif_rx_pdu_cfg_t test_rx_config;
    canif_tx_pdu_cfg_t test_tx_config;

    canif_config_t config;
    config.rx_configuration = &test_rx_config;
    config.tx_configuration = &test_tx_config;
 
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    bsp_can_init_ExpectAndReturn(true);
    bsp_can_cb_set_ExpectAnyArgsAndReturn(BSP_CAN_STATUS_OK);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = canif_init(&config);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(true, g_canif_init);

    TEST_STEP(4);
    TEST_ASSERT_EQUAL(CAN_TRCV_MODE_SLEEP, g_transceiver_cfg.trcv_current_mode);

    TEST_STEP(5);
    TEST_ASSERT_EQUAL(CAN_TRCV_INTERNAL_FLAG_CLEARED, 
                      g_transceiver_cfg.trcv_flag_status);

    TEST_STEP(6);
    TEST_ASSERT_EQUAL(CAN_TRCV_MODE_SLEEP, g_transceiver_cfg.trcv_old_mode);

    TEST_STEP(7);
    TEST_ASSERT_EQUAL(CAN_TRCV_ERR_N_FLAG_NONE, g_transceiver_cfg.errn_flag);
}

/**
 * @brief The test verifies the canif_init() function by calling the 
 * bsp_can_init() function failed.
 * 
 * https://codebeamer.daiichi.com/issue/191386
 *  
 */
void test_canif_init__can_init_state__fail(void)
{
    TEST_CASE();
    

    //**************************** Input Params **************************** //

    canif_rx_pdu_cfg_t test_rx_config;
    canif_tx_pdu_cfg_t test_tx_config;

    canif_config_t config;
    config.rx_configuration = &test_rx_config;
    config.tx_configuration = &test_tx_config;
 
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    bsp_can_init_ExpectAndReturn(false);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = canif_init(&config);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the canif_init() function by calling the 
 * bsp_can_cb_set() function failed.
 * 
 * https://codebeamer.daiichi.com/item/191388
 *  
 */
void test_canif_init__can_cb_set_state__fail(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //

    canif_rx_pdu_cfg_t test_rx_config;
    canif_tx_pdu_cfg_t test_tx_config;

    canif_config_t config;
    config.rx_configuration = &test_rx_config;
    config.tx_configuration = &test_tx_config;
 
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    bsp_can_init_ExpectAndReturn(true);
    bsp_can_cb_set_ExpectAnyArgsAndReturn(false);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = canif_init(&config);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief The test verifies the canif_init() function failed with the NULL 
 * argument.
 * 
 * https://codebeamer.daiichi.com/item/191389
 *  
 */
void test_canif_init__when_using_null_argument__fail(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = canif_init(NULL);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the canif_init() function failed with the invalid 
 * RX configuration argument.
 * 
 * https://codebeamer.daiichi.com/issue/191396
 *  
 */
void test_canif_init__invalid_rx_config__fail(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    canif_tx_pdu_cfg_t test_tx_config;

    canif_config_t config;
    config.rx_configuration = NULL;
    config.tx_configuration = &test_tx_config;
 
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = canif_init(&config);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the canif_init() function failed with the invalid TX 
 * configuration argument.
 * 
 * https://codebeamer.daiichi.com/item/191393
 *  
 */
void test_canif_init__invalid_tx_config__fail(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    canif_rx_pdu_cfg_t test_rx_config;

    canif_config_t config;
    config.rx_configuration = &test_rx_config;
    config.tx_configuration = NULL;
 
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = canif_init(&config);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the canif_deinit() function by calling the 
 * bsp_can_deinit() function.
 * 
 * https://codebeamer.daiichi.com/item/191288
 *  
 */
void test_canif_deinit__usual_run__success(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_canif_init();

    // ************************** Expected Calls **************************** //

    bsp_can_deinit_ExpectAndReturn(true);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = canif_deinit();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(false, g_canif_init);
}

/**
 * @brief The test verifies the canif_deinit() function by calling the
 * bsp_can_deinit() function with failed.
 * 
 * https://codebeamer.daiichi.com/item/191464
 *  
 */
void test_canif_deinit__can_deinit_state__fail(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_canif_init();

    // ************************** Expected Calls **************************** //

    bsp_can_deinit_ExpectAndReturn(false);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = canif_deinit();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(true, g_canif_init);
}

/**
 * @brief The test verifies the canif_controller_error_state_get() function by 
 * calling the bsp_can_fault_state_get() function.
 * 
 * https://codebeamer.daiichi.com/issue/191503
 *  
 */
void test_canif_controller_error_state_get__usual_run__success(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    canif_fault_state_t test_state = CAN_ERRORSTATE_NONE;
    canif_fault_state_t *p_error_state = &test_state;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // set CAN fault state  
    bsp_can_fault_state_t status = BSP_CAN_FAULT_STATE_ACTIVE;

    bsp_can_fault_state_get_ExpectAnyArgsAndReturn(BSP_CAN_STATUS_OK);
    bsp_can_fault_state_get_ReturnThruPtr_p_state(&status);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_controller_error_state_get(p_error_state);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_OK, ret);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(BSP_CAN_FAULT_STATE_ACTIVE, status);
}

/**
 * @brief The test verifies the canif_controller_error_state_get() function by 
 * calling the bsp_can_fault_state_get() function when the fault state is equal 
 * to "BSP_CAN_FAULT_STATE_PASSIVE".
 * 
 * https://codebeamer.daiichi.com/item/191504
 *  
 */
void test_canif_controller_error_state_get__passive_error_state__success(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    canif_fault_state_t test_state = CAN_ERRORSTATE_NONE;
    canif_fault_state_t *p_error_state = &test_state;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // set CAN fault state
    bsp_can_fault_state_t status = BSP_CAN_FAULT_STATE_PASSIVE;

    bsp_can_fault_state_get_ExpectAnyArgsAndReturn(BSP_CAN_STATUS_OK);
    bsp_can_fault_state_get_ReturnThruPtr_p_state(&status);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_controller_error_state_get(p_error_state);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_OK, ret);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(BSP_CAN_FAULT_STATE_PASSIVE, status);
}

/**
 * @brief The test verifies the canif_controller_error_state_get() function by 
 * calling the bsp_can_fault_state_get() function when the fault state is equal 
 * to "BSP_CAN_FAULT_STATE_BUSOFF".
 * 
 * https://codebeamer.daiichi.com/item/191505
 *  
 */
void test_canif_controller_error_state_get__busoff_error_state__success(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    canif_fault_state_t test_state = CAN_ERRORSTATE_NONE;
    canif_fault_state_t *p_error_state = &test_state;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // set CAN fault state
    bsp_can_fault_state_t status = BSP_CAN_FAULT_STATE_BUSOFF;

    bsp_can_fault_state_get_ExpectAnyArgsAndReturn(BSP_CAN_STATUS_OK);
    bsp_can_fault_state_get_ReturnThruPtr_p_state(&status);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_controller_error_state_get(p_error_state);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_OK, ret);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(BSP_CAN_FAULT_STATE_BUSOFF, status);
}

/**
 * @brief The test verifies the canif_controller_error_state_get() function by 
 * calling the bsp_can_fault_state_get() function when the fault state is equal 
 * to "BSP_CAN_FAULT_STATE_NONE".
 * 
 * https://codebeamer.daiichi.com/item/191506
 *  
 */
void test_canif_controller_error_state_get__none_error_state__fail(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    canif_fault_state_t test_state = CAN_ERRORSTATE_NONE;
    canif_fault_state_t *p_error_state = &test_state;


    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // set CAN fault state
    bsp_can_fault_state_t status = BSP_CAN_FAULT_STATE_NONE;

    bsp_can_fault_state_get_ExpectAnyArgsAndReturn(BSP_CAN_STATUS_OK);
    bsp_can_fault_state_get_ReturnThruPtr_p_state(&status);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_controller_error_state_get(p_error_state);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_NOT_OK, ret);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(BSP_CAN_FAULT_STATE_NONE, status);
}

/**
 * @brief The test verifies the canif_controller_error_state_get() function by 
 * calling the bsp_can_fault_state_get() function with fail.
 * 
 * https://codebeamer.daiichi.com/item/191507
 *  
 */
void test_canif_controller_error_state_get__bsp_can_fault_state__fail(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    canif_fault_state_t test_state = CAN_ERRORSTATE_NONE;
    canif_fault_state_t *p_error_state = &test_state;


    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    bsp_can_fault_state_get_ExpectAnyArgsAndReturn(BSP_CAN_STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_controller_error_state_get(p_error_state);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_NOT_OK, ret);
}

/**
 * @brief The test verifies the canif_controller_error_state_get() function with 
 * an invalid argument.
 * 
 * https://codebeamer.daiichi.com/item/191508
 *  
 */
void test_canif_controller_error_state_get__use_invalid_argument__fail(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_controller_error_state_get(NULL);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_NOT_OK, ret);
}


///********************************************************
/// buraya canif_transmit() için başarılı tstler yazılacak.
///********************************************************

/**
 * @brief The test verifies the canif_transmit() function with used invalid 
 * PDU ID argument. 
 * 
 * https://codebeamer.daiichi.com/item/191521
 *  
 */
void test_canif_transmit__use_invalid_pdu_id_argument__fail(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    pdu_info_t test_info;
    const pdu_info_t *p_pdu_info = &test_info;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //


    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_transmit((pdu_id_t)0U, p_pdu_info);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_NOT_OK, ret);
}

/**
 * @brief The test verifies the canif_transmit() function with used invalid PDU 
 * argument.
 * 
 * https://codebeamer.daiichi.com/item/191522
 *  
 */
void test_canif_transmit__use_invalid_pdu_argument__fail(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    pdu_id_t tx_pdu_id = 1U;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //


    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_transmit(tx_pdu_id, NULL);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_NOT_OK, ret);
}

/**
 * @brief The test verifies the canif_transmit() function with used invalid meta 
 * argument.
 * 
 * https://codebeamer.daiichi.com/item/191523
 *  
 */
void test_canif_transmit__use_invalid_meta_argument__fail(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    pdu_id_t tx_pdu_id = 1U;
    pdu_info_t test_pdu_info = {(uint8_t *)1U, NULL, 1U};
    const pdu_info_t *p_pdu_info = &test_pdu_info;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //


    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_transmit(tx_pdu_id, p_pdu_info);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_NOT_OK, ret);
}

/**
 * @brief The test verifies the canif_transmit() function with used invalid SDU 
 * argument.
 * 
 * https://codebeamer.daiichi.com/item/191524
 *  
 */
void test_canif_transmit__use_invalid_sdu_argument__fail(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    pdu_id_t tx_pdu_id = 1U;
    pdu_info_t test_pdu_info = {NULL, (uint8_t *)1U, 1U};
    const pdu_info_t *p_pdu_info = &test_pdu_info;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //


    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_transmit(tx_pdu_id, p_pdu_info);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_NOT_OK, ret);
}

/**
 * @brief The test verifies the canif_controller_rx_error_cnt_get() function by 
 * calling the bsp_can_rx_err_cnt_get() function.
 * 
 * https://codebeamer.daiichi.com/issue/191527
 *  
 */
void test_canif_controller_rx_error_cnt_get__usual_run__success(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    uint8_t test_val = 10U;
    uint8_t *test_rx_error_counter = &test_val;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //
    bsp_can_rx_err_cnt_get_ExpectAnyArgsAndReturn(BSP_CAN_STATUS_OK);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_controller_rx_error_cnt_get(test_rx_error_counter);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_OK, ret);
}

/**
 * @brief The test verifies the canif_controller_rx_error_cnt_get() function by 
 * calling the bsp_can_rx_err_cnt_get() function failed.
 * 
 * https://codebeamer.daiichi.com/item/191528
 *  
 */
void test_canif_controller_rx_error_cnt_get__rx_err_cnt_state__fail(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    uint8_t test_val = 10U;
    uint8_t *test_rx_error_counter = &test_val;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //
    bsp_can_rx_err_cnt_get_ExpectAnyArgsAndReturn(BSP_CAN_STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_controller_rx_error_cnt_get(test_rx_error_counter);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_NOT_OK, ret);
}

/**
 * @brief The test verifies the canif_controller_rx_error_cnt_get() function 
 * with an invalid argument.
 * 
 * https://codebeamer.daiichi.com/item/191529
 *  
 */
void test_canif_controller_rx_error_cnt_get__invalid_argument__fail(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //
    
    // ************************** Expected Calls **************************** //

    bsp_can_rx_err_cnt_get_ExpectAndReturn(NULL, BSP_CAN_STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_controller_rx_error_cnt_get(NULL);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_NOT_OK, ret);
}

/**
 * @brief The test verifies the canif_controller_tx_error_cnt_get() function by 
 * calling the bsp_can_tx_err_cnt_get() function.
 * 
 * https://codebeamer.daiichi.com/issue/191712
 *  
 */
void test_canif_controller_tx_error_cnt_get__usual_run__success(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    uint8_t test_cnt = 0U;
    uint8_t *test_tx_err_cnt = &test_cnt;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    bsp_can_tx_err_cnt_get_ExpectAndReturn(test_tx_err_cnt, BSP_CAN_STATUS_OK);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_controller_tx_error_cnt_get(test_tx_err_cnt);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_OK, ret); 
}

/**
 * @brief The test verifies the canif_controller_tx_error_cnt_get() function by 
 * calling the bsp_can_tx_err_cnt_get() function fail.
 * 
 * https://codebeamer.daiichi.com/item/191713
 *  
 */
void test_canif_controller_tx_error_cnt_get__err_cnt_state__success(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    uint8_t test_cnt = 0U;
    uint8_t *test_tx_err_cnt = &test_cnt;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    bsp_can_tx_err_cnt_get_ExpectAndReturn(test_tx_err_cnt, 
                                           BSP_CAN_STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_controller_tx_error_cnt_get(test_tx_err_cnt);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_NOT_OK, ret); 
}

/**
 * @brief The test verifies the canif_controller_tx_error_cnt_get() function by 
 * calling the bsp_can_tx_err_cnt_get() function when using an invalid argument.
 * 
 * https://codebeamer.daiichi.com/item/191903
 *  
 */
void test_canif_controller_tx_error_cnt_get__invalid_argument__success(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    bsp_can_tx_err_cnt_get_ExpectAnyArgsAndReturn(BSP_CAN_STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_controller_tx_error_cnt_get(NULL);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_NOT_OK, ret);
}

/**
 * @brief The test verifies the canif_tx_confirmation() function with an invalid 
 * CAN ID value.
 * 
 * https://codebeamer.daiichi.com/issue/191904
 *  
 */
void test_canif_tx_confirmation__when_use_invalid_index__success(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    can_id_t can_id;
    can_id = 0U;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //


    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    canif_tx_confirmation(can_id);
}

/**
 * @brief The test verifies the canif_tx_confirmation() function for j1939 
 * confirmation with a valid CAN ID value.
 * 
 * https://codebeamer.daiichi.com/item/191905
 *  
 */
void test_canif_tx_confirmation__use_j1939_confirmation__fail(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    can_id_t can_id;
    can_id = 0x18FED922U;

    // **************************** Pre-Actions ***************************** //
    helper_canif_init_with_tx_config(0U);

    // ************************** Expected Calls **************************** //


    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    canif_tx_confirmation(can_id);
}

/**
 * @brief The test verifies the canif_tx_confirmation() function for cantp 
 * confirmation with a valid CAN ID value.
 * 
 * https://codebeamer.daiichi.com/item/191908
 *  
 */
void test_canif_tx_confirmation__use_cantp_confirmation__fail(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    can_id_t can_id;
    can_id = 0x18FED922U;

    // **************************** Pre-Actions ***************************** //

    helper_canif_init_with_tx_config(2U);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    canif_tx_confirmation(can_id);
}

/**
 * @brief The test verifies the canif_tx_confirmation() function for invalid 
 * confirmation with a valid CAN ID value.
 * 
 * https://codebeamer.daiichi.com/item/191909
 *  
 */
void test_canif_tx_confirmation__use_invalid_confirmation__fail(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    can_id_t can_id;
    can_id = 0x18FED922U;

    // **************************** Pre-Actions ***************************** //
    helper_canif_init_with_tx_config(99U);

    // ************************** Expected Calls **************************** //


    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    canif_tx_confirmation(can_id);
}

/**
 * @brief The test verifies the canif_rx_indication() function with an invalid 
 * CAN ID value.
 * 
 * https://codebeamer.daiichi.com/issue/191964
 *  
 */
void test_canif_rx_indication__invalid_can_id__fail(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    can_id_t can_id;
    can_id = 0U;
    pdu_info_t pdu_info_obj;
    const pdu_info_t* p_pdu_info = &pdu_info_obj;

    // **************************** Pre-Actions ***************************** //


    // ************************** Expected Calls **************************** //


    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    canif_rx_indication(can_id, p_pdu_info);
}

/**
 * @brief The test verifies the canif_rx_indication() function with a valid 
 * j1939tp indication parameter. 
 * 
 * https://codebeamer.daiichi.com/issue/191965
 *  
 */
void test_canif_rx_indication__when_use_j1939_tp_indication__success(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    can_id_t can_id;
    can_id = 0x18FED921;
    pdu_info_t pdu_info_obj;
    const pdu_info_t* p_pdu_info = &pdu_info_obj;

    // **************************** Pre-Actions ***************************** //
    helper_canif_init_with_rx_config(0U);

    // ************************** Expected Calls **************************** //


    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    canif_rx_indication(can_id, p_pdu_info);
}

/**
 * @brief The test verifies the canif_rx_indication() function with a valid 
 * cantp indication parameter. 
 * 
 * https://codebeamer.daiichi.com/issue/191966
 *  
 */
void test_canif_rx_indication__when_use_can_tp_indication__success(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    can_id_t can_id;
    can_id = 0x18FED921;
    pdu_info_t pdu_info_obj;
    const pdu_info_t* p_pdu_info = &pdu_info_obj;

    // **************************** Pre-Actions ***************************** //
    helper_canif_init_with_rx_config(2U);

    // ************************** Expected Calls **************************** //


    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    canif_rx_indication(can_id, p_pdu_info);
}

/**
 * @brief The test verifies the canif_rx_indication() function with a invalid
 * indication parameter.
 * 
 * https://codebeamer.daiichi.com/item/191967
 *  
 */
void test_canif_rx_indication__when_use_invalid_indication__success(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    can_id_t can_id;
    can_id = 0x18FED921;
    pdu_info_t pdu_info_obj;
    const pdu_info_t* p_pdu_info = &pdu_info_obj;

    // **************************** Pre-Actions ***************************** //
    helper_canif_init_with_rx_config(99U);

    // ************************** Expected Calls **************************** //


    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    canif_rx_indication(can_id, p_pdu_info);
}

/**
 * @brief The test verifies the canif_rx_indication() function with an invalid 
 * PDU parameter. 
 * 
 * https://codebeamer.daiichi.com/item/191972
 *  
 */
void test_canif_rx_indication__when_use_invalid_pdu_info__success(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    can_id_t can_id;
    can_id = 0x18FED921;

    // **************************** Pre-Actions ***************************** //
    helper_canif_init_with_rx_config(1U);

    // ************************** Expected Calls **************************** //


    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    canif_rx_indication(can_id, NULL);
}

/**
 * @brief The test verifies the canif_rx_indication() function successfully. 
 * 
 * https://codebeamer.daiichi.com/item/192229
 *  
 */
void test_canif_bus_off_state__usual_run__success(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //


    // ************************** Expected Calls **************************** //


    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    canif_bus_off_state();
}

/**
 * @brief Verifies that the canif_error_notification() function can be called 
 * by the error callback set in canif_init().
 * 
 * https://codebeamer.daiichi.com/issue/192230
 *  
 */
void test_canif_error_notification__when_error_occured__success(void)
{
    TEST_CASE();
    
    //**************************** Input Params ****************************  //

    // **************************** Pre-Actions ***************************** //

    helper_canif_init();

    // ************************** Expected Calls **************************** //


    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    m_test_can_err_cb(BSP_CAN_ERR_STATUS_BIT_MONITORING1);

    TEST_STEP(2);
    m_test_can_err_cb(BSP_CAN_ERR_STATUS_BIT_MONITORING2);

    TEST_STEP(3);
    m_test_can_err_cb(BSP_CAN_ERR_STATUS_CHECK_ACK_FAILED);

    TEST_STEP(4);
    m_test_can_err_cb(BSP_CAN_ERR_STATUS_CHECK_FORM_FAILED);

    TEST_STEP(5);
    m_test_can_err_cb(BSP_CAN_ERR_STATUS_CHECK_STUFFING_FAILED);

    TEST_STEP(6);
    m_test_can_err_cb(BSP_CAN_ERR_STATUS_CHECK_CRC_FAILED);

    TEST_STEP(7);
    m_test_can_err_cb((bsp_can_err_status_t)99U);
}

/**
 * @brief Verifies that the canif_rx_indication() function can be called 
 * by the canif_rx_cb() set in canif_init().
 * 
 * https://codebeamer.daiichi.com/item/192258
 *  
 */
void test_canif_init__set_rx_cb__success(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //

    static bsp_can_data_t data;
   
    data.msg_id = 0x18FED921;
    data.size = 8U;
    for (uint8_t i = 0U; i < data.size; i++) {
        data.payload[i] = i;
    }

    canif_tx_pdu_cfg_t test_tx_config;

    static canif_config_t config;
    config.tx_configuration = &test_tx_config;

    config.rx_configuration = &canif_rx_pdu_cfg[0U];

    // **************************** Pre-Actions ***************************** //
    
    // ************************** Expected Calls **************************** //

    bsp_can_init_ExpectAndReturn(true);

    bsp_can_cb_set_AddCallback(test_cb_bsp_can_cb_set);

    bsp_can_cb_set_ExpectAnyArgsAndReturn(BSP_CAN_STATUS_OK);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    (void)canif_init(&config);

    TEST_STEP(2);
    m_test_can_rx_cb(&data);
}

/**
 * @brief Verifies that canif_rx_indication() can only be called with the 
 * correct values by canif_rx_cb() set in canif_init().
 * 
 * https://codebeamer.daiichi.com/item/192259
 *  
 */
void test_canif_rx_indication__not_called_state__success(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //
    
    helper_canif_init_with_rx_config(0U);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    m_test_can_rx_cb(NULL);
}

/**
 * @brief Verifies that canif_rx_indication() can only be called with the 
 * correct values by canif_rx_cb() set in canif_init(). Data size is incorrect. 
 * 
 * https://codebeamer.daiichi.com/item/192260
 *  
 */
void test_canif_rx_indication__with_use_invalid_size__success(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    static bsp_can_data_t data;
   
    data.msg_id = 0x18FED921;
    data.size = 7U;
    for (uint8_t i = 0U; i < data.size; i++) {
        data.payload[i] = i;
    }

    // **************************** Pre-Actions ***************************** //
    
    helper_canif_init_with_rx_config(1U);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    m_test_can_rx_cb(&data);
}

/**
 * @brief Verifies that canif_rx_indication() can only be called with the 
 * correct values by canif_rx_cb() set in canif_init(). Data size is correct. 
 * 
 * https://codebeamer.daiichi.com/item/192261
 *  
 */
void test_canif_rx_indication__with_use_valid_size__success(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    static bsp_can_data_t data;
   
    data.msg_id = 0x18FED921;
    data.size = 8U;
    for (uint8_t i = 0U; i < data.size; i++) {
        data.payload[i] = i;
    }

    // **************************** Pre-Actions ***************************** //
    
    helper_canif_init_with_rx_config(1U);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    m_test_can_rx_cb(&data);
}

/**
 * @brief Verifies that canif_rx_indication() can only be called with the 
 * correct values by canif_rx_cb() set in canif_init(). Id value is not correct. 
 * 
 * https://codebeamer.daiichi.com/item/192265
 *  
 */
void test_canif_rx_indication__when_not_founded_index__success(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    static bsp_can_data_t data;
    
    // set invalid id value for test 
    data.msg_id = 0x999999; 
    data.size = 8U;
    for (uint8_t i = 0U; i < data.size; i++) {
        data.payload[i] = i;
    }

    // **************************** Pre-Actions ***************************** //
    
    helper_canif_init_with_rx_config(0U);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    m_test_can_rx_cb(&data);
}

/**
 * @brief Verifies that canif_tx_indication() can only be called with the 
 * correct values by canif_tx_cb() set in canif_init(). 
 * 
 * https://codebeamer.daiichi.com/item/192288
 *  
 */
void test_canif_tx_confirmation__when_use_j1939_tp_confirmation__success(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    static bsp_can_data_t data;
    
    data.msg_id = 0x18FED922U; 
    data.size = 8U;
    for (uint8_t i = 0U; i < data.size; i++) {
        data.payload[i] = i;
    }

    // **************************** Pre-Actions ***************************** //
    
    helper_canif_init_with_tx_config(1U);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    m_test_can_tx_cb(&data);
}

/**
 * @brief Verifies that canif_tx_indication() can only be called with the 
 * correct values by canif_tx_cb() set in canif_init(). 
 * 
 * https://codebeamer.daiichi.com/item/192289
 *  
 */
void test_canif_tx_confirmation__when_use_cantp_confirmation__success(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    static bsp_can_data_t data;
    
    data.msg_id = 0x18FED552U; 
    data.size = 8U;
    for (uint8_t i = 0U; i < data.size; i++) {
        data.payload[i] = i;
    }

    // **************************** Pre-Actions ***************************** //
    
    helper_canif_init_with_tx_config(2U);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    m_test_can_tx_cb(&data);
}

/**
 * @brief Verifies that canif_tx_indication() with the incorrect argument.
 * 
 * https://codebeamer.daiichi.com/item/192290
 *  
 */
void test_canif_tx_confirmation__when_use_invalid_argument__success(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //
    
    helper_canif_init_with_tx_config(2U);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    m_test_can_tx_cb(NULL);
}

/**
 * @brief Verifies that canif_tx_indication() can only be called with the 
 * correct values by canif_tx_cb() set in canif_init(). 
 * 
 * https://codebeamer.daiichi.com/item/192293
 *  
 */
void test_canif_tx_confirmation__when_use_invalid_index__fail(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    static bsp_can_data_t data;
    
    data.msg_id = 0x18FED922U; 
    data.size = 8U;
    for (uint8_t i = 0U; i < data.size; i++) {
        data.payload[i] = i;
    }

    // **************************** Pre-Actions ***************************** //
    
    helper_canif_init_with_tx_config(99U);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    m_test_can_tx_cb(&data);
}

/**
 * @brief Verifies that canif_tx_indication() can only be called with the 
 * correct values by canif_tx_cb() set in canif_init(). 
 * 
 * https://codebeamer.daiichi.com/item/192295
 *  
 */
void test_canif_tx_confirmation__when_use_invalid_length__fail(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    static bsp_can_data_t data;
    
    data.msg_id = 0x18FED922U; 
    data.size = 7U;
    for (uint8_t i = 0U; i < data.size; i++) {
        data.payload[i] = i;
    }

    // **************************** Pre-Actions ***************************** //
    
    helper_canif_init_with_tx_config(1U);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    m_test_can_tx_cb(&data);
}

/**
 * @brief The test verifies the canif_transmit() function by calling the 
 * bsp_can_async_write() function succesfully.
 * 
 * https://codebeamer.daiichi.com/item/192332
 *  
 */
void test_canif_transmit__usual_run__success(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    pdu_id_t tx_pdu_id = 1U;

    static uint8_t test_p_sdu_data = 1U;
    static uint8_t test_p_meta_data = 1U;
    static pdu_length_t sdu_length = 8U;

    pdu_info_t test_pdu_info = {&test_p_sdu_data, &test_p_meta_data, sdu_length};

    const pdu_info_t *p_pdu_info = &test_pdu_info;

    // **************************** Pre-Actions ***************************** //

    // Initialize cainf with tx config
    canif_rx_pdu_cfg_t test_rx_config;

    static canif_config_t config;
    config.rx_configuration = &test_rx_config;
    config.tx_configuration = &canif_tx_pdu_cfg[0U];

    bsp_can_init_ExpectAndReturn(true);
    bsp_can_cb_set_ExpectAnyArgsAndReturn(BSP_CAN_STATUS_OK);

    (void)canif_init(&config);

    // ************************** Expected Calls **************************** //

    m_test_can_data.msg_id = 
        (uint32_t)config.tx_configuration->canif_tx_pdu_can_id;

    uint8_t *payload_addr =  m_test_can_data.payload;

    (void)memcpy(payload_addr, p_pdu_info->p_sdu_data, p_pdu_info->sdu_length);

    m_test_can_data.size = config.tx_configuration->canif_tx_pdu_data_length;

    // Callback function for argument compare process
    bsp_can_async_write_AddCallback(test_bsp_can_async_write);

    bsp_can_async_write_ExpectAnyArgsAndReturn(BSP_CAN_STATUS_OK);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_transmit(tx_pdu_id, p_pdu_info);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_OK, ret);
}

/**
 * @brief The test verifies the canif_transmit() function by calling the 
 * bsp_can_async_write() function successfully. 
 * 
 * https://codebeamer.daiichi.com/item/192334
 *  
 */
void test_canif_transmit__when_use_correct_length__success(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    pdu_id_t tx_pdu_id = 1U;

    static uint8_t test_p_sdu_data = 1U;
    static uint8_t test_p_meta_data = 1U;

    // set correct length value
    static pdu_length_t sdu_length = 8U;

    pdu_info_t test_pdu_info = {&test_p_sdu_data, &test_p_meta_data, sdu_length};

    const pdu_info_t *p_pdu_info = &test_pdu_info;

    // **************************** Pre-Actions ***************************** //

    helper_canif_init_with_tx_config(1U);

    // ************************** Expected Calls **************************** //

    bsp_can_async_write_ExpectAnyArgsAndReturn(BSP_CAN_STATUS_OK);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_transmit(tx_pdu_id, p_pdu_info);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_OK, ret);
}

/**
 * @brief The test verifies the canif_transmit() function with an invalid length 
 * argument.  
 * 
 * https://codebeamer.daiichi.com/item/192335
 *  
 */
void test_canif_transmit__when_use_incorrect_length__fail(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    pdu_id_t tx_pdu_id = 1U;

    static uint8_t test_p_sdu_data = 1U;
    static uint8_t test_p_meta_data = 1U;

    // set correct length value
    static pdu_length_t sdu_length = 99U;

    pdu_info_t test_pdu_info = {&test_p_sdu_data, 
        &test_p_meta_data, sdu_length};

    const pdu_info_t *p_pdu_info = &test_pdu_info;

    // **************************** Pre-Actions ***************************** //

    helper_canif_init_with_tx_config(1U);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_transmit(tx_pdu_id, p_pdu_info);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_NOT_OK, ret);
}

/**
 * @brief The test verifies the canif_transmit() function according to the 
 * bsp_can_async_wirte() function return state.  
 * 
 * https://codebeamer.daiichi.com/item/192336
 *  
 */
void test_canif_transmit__write_func_state__fail(void)
{
    TEST_CASE();
    
    //**************************** Input Params **************************** //
    pdu_id_t tx_pdu_id = 1U;

    static uint8_t test_p_sdu_data = 1U;
    static uint8_t test_p_meta_data = 1U;
    static pdu_length_t sdu_length = 8U;

    pdu_info_t test_pdu_info = {&test_p_sdu_data, 
                                &test_p_meta_data, 
                                sdu_length};

    const pdu_info_t *p_pdu_info = &test_pdu_info;

    // **************************** Pre-Actions ***************************** //

    helper_canif_init_with_tx_config(0U);

    // ************************** Expected Calls **************************** //

    bsp_can_async_write_ExpectAnyArgsAndReturn(BSP_CAN_STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_transmit(tx_pdu_id, p_pdu_info);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_NOT_OK, ret);
}

/*******************************************************************************
 * Private Functions Definitions
 ******************************************************************************/
static void helper_canif_init_with_rx_config(uint8_t number)
{
    canif_tx_pdu_cfg_t test_tx_config;

    static canif_config_t config;
    config.tx_configuration = &test_tx_config;

    if(99U != number)
    {
        config.rx_configuration = &canif_rx_pdu_cfg[number];
    }
    else
    {
        const canif_rx_pdu_cfg_t canif_rx_pdu_cfg[] = 
        {
            [0U] =
            {
                .canif_rx_pdu2tp_indication = 3U,
                .canif_rx_pdu_can_id = 0x18FED921, //AUXIO1
                .canif_rx_pdu_can_id_mask = 0xFF,
                .canif_rx_pdu_data_length = 8U,
                .canif_rx_pdu_data_length_check = true,
                .canif_rx_pdu_id = 1U,
                .canif_rx_upper_pdu_id = 1U
            },
        };

        // set invalid parameter
        config.rx_configuration = canif_rx_pdu_cfg;
    }
    bsp_can_init_ExpectAndReturn(true);
    bsp_can_cb_set_AddCallback(test_cb_bsp_can_cb_set);
    bsp_can_cb_set_ExpectAnyArgsAndReturn(BSP_CAN_STATUS_OK);

    (void)canif_init(&config);
}

static void helper_canif_init(void)
{
    canif_tx_pdu_cfg_t test_tx_config;

    static canif_config_t config;
    config.rx_configuration = &canif_rx_pdu_cfg[0U];
    config.tx_configuration = &test_tx_config;
 
    bsp_can_init_ExpectAndReturn(true);
    bsp_can_cb_set_AddCallback(test_cb_bsp_can_cb_set);
    bsp_can_cb_set_ExpectAnyArgsAndReturn(BSP_CAN_STATUS_OK);

    (void)canif_init(&config);
}

static bsp_can_status_t test_cb_bsp_can_cb_set(const bsp_can_cb_t* p_cb, 
                                               int cmock_num_calls)
{
    (void)cmock_num_calls;

    if (NULL != p_cb->err_cb) 
    {
        m_test_can_err_cb = p_cb->err_cb;
    }

    if (NULL != p_cb->rx_cb) 
    {
        m_test_can_rx_cb = p_cb->rx_cb;
    }

    if (NULL != p_cb->tx_cb)
    {
        m_test_can_tx_cb = p_cb->tx_cb;
    }

    return BSP_CAN_STATUS_OK;
}

static void helper_canif_init_with_tx_config(uint8_t number)
{
    canif_rx_pdu_cfg_t test_rx_config;

    static canif_config_t config;
    config.rx_configuration = &test_rx_config;
    if(99U != number)
    {
        config.tx_configuration = &canif_tx_pdu_cfg[number];
    }
    else
    {
        const canif_tx_pdu_cfg_t canif_tx_pdu_cfg[] = 
        {
            [0U] =
            {
                .canif_tx_pdu2tp_confirmation = J1939TP_CONFIRMATION,
                .canif_tx_pdu_can_id = 0x18FED922U,
                .canif_tx_pdu_data_length = 8U,
                .canif_tx_pdu_data_length_check = false,
                .canif_tx_pdu_id = 0U,
                .canif_tx_upper_pdu_id = 0U
            },
        };

        // set invalid confirmation parameter
        config.tx_configuration = canif_tx_pdu_cfg;
    }

    bsp_can_init_ExpectAndReturn(true);
    bsp_can_cb_set_ExpectAnyArgsAndReturn(BSP_CAN_STATUS_OK);

    (void)canif_init(&config);
}

static bsp_can_status_t test_bsp_can_async_write(const bsp_can_data_t* p_tx, 
                                                 int cmock_num_calls)
{
    (void)p_tx;
    (void)cmock_num_calls;

    TEST_ASSERT_EQUAL(m_test_can_data.msg_id, p_tx->msg_id);
    TEST_ASSERT_EQUAL(m_test_can_data.size, p_tx->size);

    return BSP_CAN_STATUS_OK;
}

// end of file 
