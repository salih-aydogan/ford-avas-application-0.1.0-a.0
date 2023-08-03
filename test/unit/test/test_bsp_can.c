/**
 * @file test_bsp_can.c
 * @author Furkan Yildirim (furkan.yildirim@daiichi.com)
 * 
 * @brief These tests verify the units of the bsp_can module.
 * 
 * @version 0.1
 * @date 2023-03-01
 * 
 * @copyright Copyright (c) 2023
 * 
 * https://codebeamer.daiichi.com/issue/185180
 * 
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

//First - include test framework headers
#include "unity.h"
#include "support.h"

// Second - include the module to be tested
#include "bsp_can.h"

// Third - include the mocks of required modules
#include "mock_pins_driver.h"
#include "mock_flexcan_driver.h"

#include "mock_log.h"

/*******************************************************************************
 * Test Files
 ******************************************************************************/

TEST_FILE("bsp_can.c");

/*******************************************************************************
 * Defines
 ******************************************************************************/

/**
 * @brief This macro holds instance number of CAN-BUS peripheral.
 *
 */
#define TEST_CAN_INSTANCE 0U

/**
 * @brief This macro holds count of CAN-BUS pins.
 *
 */
#define TEST_CAN_PINS_MAX 2U

/**
 * @brief This macro holds speed level count of CAN-BUS communication.
 *
 */
#define TEST_CAN_SPEED_MAX 3U

/*******************************************************************************
 * Structs
 ******************************************************************************/

/**
 * @brief This struct is used to configure time segmentation settings for
 *        CAN-BUS bitrate.
 *
 */
typedef struct test_can_arbitration_phase_s
{
    uint32_t bitrate_kbps;
    uint32_t prog_segment;
    uint32_t phase_segment_1;
    uint32_t phase_segment_2;
    uint32_t pre_divider;
    uint32_t rjump_width;

} test_can_arbitration_phase_t;

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/**
 * @brief This variable holds the address of the static can_err_cb function 
 *        in the bsp_can module.
 * 
 */
static flexcan_error_callback_t m_test_err_callback_addr;

/**
 * @brief This variable holds the address of the static can_callback function 
 *        in the bsp_can module.
 * 
 */
static flexcan_callback_t m_test_event_callback_addr;

/**
 * @brief This variable holds the adress of callback functions in the bsp_can
 *         module.
 * 
 */
static bsp_can_cb_t m_test_callback_addr;

/**
 * @brief This variable represents an example 29-bit message ID.
 * 
 */
static uint32_t m_test_tx_msg_id = 0x18FF8503U;

/**
 * @brief This flexcan data structure type variable holds message buffer 
 *        information for test process. 
 * 
 */
static flexcan_msgbuff_t m_test_recv_buff = {0U};

/**
 * @brief This data structure type variable holds message buffer information
 *        for test process.
 * 
 */
static bsp_can_data_t m_test_data;

/**
 * @brief This variables holds adress of tx error counter.
 * 
 */
static uint8_t m_test_tx_cnt;

/**
 * @brief This variables holds adress of tx error counter.
 * 
 */
static uint8_t m_test_rx_cnt;

/*******************************************************************************
 * Global Variables
 ******************************************************************************/

uint8_t allocated_area[PERIPH_MEM_SIZE];

/*******************************************************************************
 * Private Functions Prototype Declarations
 ******************************************************************************/

/**
 * @brief Stub function of PINS_DRV_Init function in the pins_driver module.
 *
 */
static status_t stub_pins_drv_init(uint32_t pinCount,
                                   const pin_settings_config_t *config,
                                   int cmock_num_calls);

/**
 * @brief Stub function of FLEXCAN_DRV_Init function in the flexcan_driver
 *        module.
 *
 */
static status_t stub_flexcan_drv_init(uint8_t instance,
                                      flexcan_state_t *state,
                                      const flexcan_user_config_t *data,
                                      int cmock_num_calls);

/**
 * @brief This function returns the numbers of the filter ID based on the
 *        numbers of the filtered ID
 *
 */
static flexcan_rx_fifo_id_filter_num_t m_can_filter_num_get(uint32_t msg_count);

/**
 * @brief Stub function of FLEXCAN_DRV_InstallEventCallback function in the
 *        flexcan_driver module.
 *
 */
static void stub_flexcan_event_cb_install(uint8_t instance,
                                          flexcan_callback_t callback,
                                          void *callbackParam,
                                          int cmock_num_calls);

/**
 * @brief Stub function of FLEXCAN_DRV_InstallErrorCallback function in the
 *        flexcan_driver module.
 *
 */
static void stub_flexcan_drv_install(uint8_t instance,
                                     flexcan_error_callback_t callback,
                                     void *callbackParam,
                                     int cmock_num_calls);

/**
 * @brief This function provides initializes to pins and flexcan module, and
 *        sets callback function.
 *
 */
static void helper_pins_and_flexcan_init_callback_set(void);

/**
 * @brief The purpose of the this function test error status according to error
 *        variable.
 *
 */
static void m_test_err_function(bsp_can_err_status_t error);

/**
 * @brief The purpose of this function is to test the status of notifying to
 *        upper layer of the ID of the transmitted transfer when the transfer
 *        operation is successful.
 *
 */
static void m_test_tx_function(const bsp_can_data_t *p_tx_data);

/**
 * @brief The purpose of this function is to test the status of notifying to
 *        upper layer of the data structure of the received data transfer when
 *        the transfer receiving operation is successful.
 *
 */
static void m_test_rx_function(bsp_can_data_t *p_rx_data);

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
 * @brief The test verifies the bsp_can_init function by successfully calling
 *        PINS_DRV_Init()and FLEXCAN_DRV_Init() functions.
 *
 * @link https://codebeamer.daiichi.com/issue/185188
 *
 */
void test_bsp_can_init__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // can_pin_init function in bsp_can_init
    PINS_DRV_Init_AddCallback(stub_pins_drv_init);

    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    // can_drv_init function in bsp_can_init
    FLEXCAN_DRV_Init_AddCallback(stub_flexcan_drv_init);

    FLEXCAN_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    FLEXCAN_DRV_InstallEventCallback_Expect(TEST_CAN_INSTANCE, NULL, NULL);
    FLEXCAN_DRV_InstallEventCallback_IgnoreArg_callback();
    FLEXCAN_DRV_InstallEventCallback_IgnoreArg_callbackParam();

    FLEXCAN_DRV_InstallErrorCallback_Expect(TEST_CAN_INSTANCE, NULL, NULL);
    FLEXCAN_DRV_InstallErrorCallback_IgnoreArg_callback();
    FLEXCAN_DRV_InstallErrorCallback_IgnoreArg_callbackParam();

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_can_init();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief The test verifies the bsp_can_init function by unsuccessfully calling
 *        PINS_DRV_Init() function.
 *
 * @link https://codebeamer.daiichi.com/issue/185422
 *
 */
void test_bsp_can_init__can_pin_init_error__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // can_pin_init function in bsp_can_init
    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_can_init();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the bsp_can_init function by successfully calling
 *        PINS_DRV_Init() function and unsuccessfully calling FLEXCAN_DRV_Init
 *        function.
 *
 * @link https://codebeamer.daiichi.com/issue/185423
 *
 */
void test_bsp_can_init__flexcan_drv_init_error__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // can_pin_init function in bsp_can_init
    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    // can_drv_init function in bsp_can_init
    FLEXCAN_DRV_Init_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_can_init();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the bsp_can_deinit function by successfully calling
 *        FLEXCAN_DRV_Deinit() function.
 *
 * @link https://codebeamer.daiichi.com/issue/185189
 *
 */
void test_bsp_can_deinit__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_pins_and_flexcan_init_callback_set();

    // ************************** Expected Calls **************************** //

    FLEXCAN_DRV_Deinit_ExpectAndReturn(TEST_CAN_INSTANCE, STATUS_SUCCESS);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_can_deinit();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief The test verifies the bsp_can_deinit function by unsuccessfully
 *        calling PINS_DRV_Deinit() function.
 *
 * @link https://codebeamer.daiichi.com/issue/185427
 *
 */
void test_bsp_can_deinit__flexcan_drv_deinit_error__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    FLEXCAN_DRV_Deinit_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_can_deinit();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the bsp_can_filter function by successfully calling
 *        FLEXCAN_DRV_ConfigRxFifo() and FLEXCAN_DRV_RxFifo() functions.
 *
 * @link https://codebeamer.daiichi.com/issue/185190
 *
 */
void test_bsp_can_filter__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    const uint32_t msg_ids[32U] = {0U};

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    FLEXCAN_DRV_ConfigRxFifo_Expect(TEST_CAN_INSTANCE,
                                    FLEXCAN_RX_FIFO_ID_FORMAT_A,
                                    NULL);

    FLEXCAN_DRV_ConfigRxFifo_IgnoreArg_id_filter_table();

    FLEXCAN_DRV_RxFifo_ExpectAndReturn(TEST_CAN_INSTANCE, NULL, STATUS_SUCCESS);
    FLEXCAN_DRV_RxFifo_IgnoreArg_data();

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_can_filter(msg_ids);
}

/**
 * @brief The test verifies the bsp_can_async_write function with the address of
 *        a variable of type const bsp_can_data_t holding 0U value, the argument
 *        must not be NULL.
 *
 * @link https://codebeamer.daiichi.com/issue/185192
 *
 */
void test_bsp_can_async_write__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // can_pin_init function in bsp_can_init
    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_ERROR);

    (void)bsp_can_init();

    // ************************** Expected Calls **************************** //

    for (uint8_t i = 0; i < 32U; i++)
    {
        FLEXCAN_DRV_Send_ExpectAndReturn(TEST_CAN_INSTANCE,
                                         0U,
                                         NULL,
                                         m_test_tx_msg_id,
                                         NULL,
                                         STATUS_SUCCESS);

        FLEXCAN_DRV_Send_IgnoreArg_mb_idx();
        FLEXCAN_DRV_Send_IgnoreArg_tx_info();
        FLEXCAN_DRV_Send_IgnoreArg_msg_id();
        FLEXCAN_DRV_Send_IgnoreArg_mb_data();
    }

    bsp_can_status_t ret = BSP_CAN_STATUS_ERROR;

    // **************************** Test Steps ****************************** //

    for (uint8_t i = 0U; i < 64U; i += 2U)
    {

        TEST_STEP(i);
        ret = bsp_can_async_write(&(const bsp_can_data_t){
            .msg_id = m_test_tx_msg_id,
            .payload = {0U},
            .size = 0U});

        TEST_STEP(i + 1U);
        TEST_ASSERT_EQUAL(BSP_CAN_STATUS_OK, ret);
    }
}

/**
 * @brief The test verifies the bsp_can_async_write function with NULL argument.
 *
 * @link https://codebeamer.daiichi.com/issue/185432
 *
 */
void test_bsp_can_async_write__null_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_can_status_t ret = bsp_can_async_write(NULL);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(BSP_CAN_STATUS_ERROR, ret);
}

/**
 * @brief This test verifies transmitted ID.
 *
 * @link https://codebeamer.daiichi.com/issue/190972
 *
 */
void test_bsp_can_async_write__id_transmission__succcess(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    flexcan_state_t flexcanState;

    // **************************** Pre-Actions ***************************** //

    helper_pins_and_flexcan_init_callback_set();

    // ************************** Expected Calls **************************** //

    FLEXCAN_DRV_Send_ExpectAndReturn(TEST_CAN_INSTANCE,
                                     0U,
                                     NULL,
                                     m_test_tx_msg_id,
                                     NULL,
                                     STATUS_SUCCESS);

    FLEXCAN_DRV_Send_IgnoreArg_mb_idx();
    FLEXCAN_DRV_Send_IgnoreArg_tx_info();
    FLEXCAN_DRV_Send_IgnoreArg_mb_data();

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_can_status_t ret = bsp_can_async_write(&(const bsp_can_data_t){
        .msg_id = m_test_tx_msg_id,
        .payload = {0U},
        .size = 0U});

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(BSP_CAN_STATUS_OK, ret);

    TEST_STEP(3);
    m_test_event_callback_addr(0U, FLEXCAN_EVENT_TX_COMPLETE, 0U, &flexcanState);
}

/**
 * @brief The test verifies the bsp_can_async_write function with the address of
 *        a variable of type const bsp_can_data_t holding 0U value, the argument
 *        must not be NULL.
 *
 * @link https://codebeamer.daiichi.com/issue/185433
 *
 */
void test_bsp_can_async_write__hw_error__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_pins_and_flexcan_init_callback_set();

    // ************************** Expected Calls **************************** //
    
    // can_callback
    flexcan_state_t flexcanState;

    FLEXCAN_DRV_GetErrorStatus_ExpectAndReturn(TEST_CAN_INSTANCE, 0x00U);

    m_test_err_callback_addr(0U,
                             FLEXCAN_EVENT_ERROR,
                             &flexcanState);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_can_status_t ret = bsp_can_async_write(&(const bsp_can_data_t){
        .msg_id = 0U,
        .payload = {0U},
        .size = 0U});

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(BSP_CAN_STATUS_HW_ERROR, ret);
}

/**
 * @brief The test verifies the bsp_can_async_write function with the address of
 *        a variable of type const bsp_can_data_t holding 0U value, the argument
 *        must not be NULL.
 *
 * @link https://codebeamer.daiichi.com/issue/185434
 *
 */
void test_bsp_can_async_write__flexcan_drv_send_status_busy__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // can_pin_init function in bsp_can_init
    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_ERROR);

    (void)bsp_can_init();

    // ************************** Expected Calls **************************** //

    FLEXCAN_DRV_Send_ExpectAndReturn(TEST_CAN_INSTANCE,
                                     0U,
                                     NULL,
                                     0U,
                                     NULL,
                                     STATUS_BUSY);

    FLEXCAN_DRV_Send_IgnoreArg_mb_idx();
    FLEXCAN_DRV_Send_IgnoreArg_tx_info();
    FLEXCAN_DRV_Send_IgnoreArg_msg_id();
    FLEXCAN_DRV_Send_IgnoreArg_mb_data();

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_can_status_t ret = bsp_can_async_write(&(const bsp_can_data_t){
        .msg_id = 0U,
        .payload = {0U},
        .size = 0U});

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(BSP_CAN_STATUS_BUSY, ret);
}

/**
 * @brief The test verifies the bsp_can_async_write function with the address of
 *        a variable of type const bsp_can_data_t holding 0U value, the argument
 *        must not be NULL.
 *
 * @link https://codebeamer.daiichi.com/issue/185435
 *
 */
void test_bsp_can_async_write__flexcan_drv_send_status_error__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // can_pin_init function in bsp_can_init
    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_ERROR);

    (void)bsp_can_init();

    // ************************** Expected Calls **************************** //

    FLEXCAN_DRV_Send_ExpectAndReturn(TEST_CAN_INSTANCE,
                                     0U,
                                     NULL,
                                     0U,
                                     NULL,
                                     STATUS_ERROR);

    FLEXCAN_DRV_Send_IgnoreArg_mb_idx();
    FLEXCAN_DRV_Send_IgnoreArg_tx_info();
    FLEXCAN_DRV_Send_IgnoreArg_msg_id();
    FLEXCAN_DRV_Send_IgnoreArg_mb_data();

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_can_status_t ret = bsp_can_async_write(&(const bsp_can_data_t){
        .msg_id = 0U,
        .payload = {0U},
        .size = 0U});

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(BSP_CAN_STATUS_ERROR, ret);
}

/**
 * @brief The test verifies the bsp_can_cb_set function with the address of a
 *        static function, the argument must not be NULL.
 *
 * @link https://codebeamer.daiichi.com/issue/185196
 *
 */
void test_bsp_can_cb_set__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_can_status_t ret = bsp_can_cb_set(&m_test_callback_addr);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(BSP_CAN_STATUS_OK, ret);
}

/**
 * @brief The test verifies the bsp_can_cb_set function with a NULL argument.
 *
 * @link https://codebeamer.daiichi.com/issue/185436
 *
 */
void test_bsp_can_cb_set__null_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_can_status_t ret = bsp_can_cb_set(NULL);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(BSP_CAN_STATUS_ERROR, ret);
}

/**
 * @brief This test verifies the bsp_can_cb_set with assigned all callback
 * function pointer to NULL value.
 *
 * @link https://codebeamer.daiichi.com/issue/190975
 *
 */
void test_bsp_can_cb_set__null_arguments__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // can_pin_init function in bsp_can_init
    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    // can_drv_init function in bsp_can_init
    FLEXCAN_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    FLEXCAN_DRV_InstallEventCallback_AddCallback(stub_flexcan_event_cb_install);
    FLEXCAN_DRV_InstallEventCallback_ExpectAnyArgs();

    FLEXCAN_DRV_InstallErrorCallback_AddCallback(stub_flexcan_drv_install);
    FLEXCAN_DRV_InstallErrorCallback_ExpectAnyArgs();

    (void)bsp_can_init();

    m_test_callback_addr.err_cb = NULL;
    m_test_callback_addr.rx_cb = NULL;
    m_test_callback_addr.tx_cb = NULL;

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_can_status_t ret = bsp_can_cb_set(&m_test_callback_addr);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(BSP_CAN_STATUS_ERROR, ret);
}
/**
 * @brief https://codebeamer.daiichi.com/issue/191214
 * 
 */
void test_bsp_can_cb_set__null_arguments_err_callback__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    flexcan_state_t flexcanState;

    // **************************** Pre-Actions ***************************** //

    // can_pin_init function in bsp_can_init
    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    // can_drv_init function in bsp_can_init
    FLEXCAN_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    FLEXCAN_DRV_InstallEventCallback_AddCallback(stub_flexcan_event_cb_install);
    FLEXCAN_DRV_InstallEventCallback_ExpectAnyArgs();

    FLEXCAN_DRV_InstallErrorCallback_AddCallback(stub_flexcan_drv_install);
    FLEXCAN_DRV_InstallErrorCallback_ExpectAnyArgs();

    (void)bsp_can_init();

    FLEXCAN_DRV_Deinit_ExpectAndReturn(TEST_CAN_INSTANCE, STATUS_SUCCESS);

    (void)bsp_can_deinit();

    m_test_callback_addr.err_cb = NULL;
    m_test_callback_addr.rx_cb = (bsp_can_rx_cb_t)m_test_rx_function;
    m_test_callback_addr.tx_cb = (bsp_can_tx_cb_t)m_test_tx_function;

    bsp_can_cb_set(&m_test_callback_addr);

    // ************************** Expected Calls **************************** //

    // FLEXCAN_DRV_GetErrorStatus_ExpectAndReturn(TEST_CAN_INSTANCE, 0U);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    m_test_err_callback_addr(TEST_CAN_INSTANCE,
                             FLEXCAN_EVENT_ERROR, 
                             &flexcanState);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/191240
 * 
 */
void test_bsp_can_cb_set__null_arguments_event_callback__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    flexcan_state_t flexcanState;

    // **************************** Pre-Actions ***************************** //

    // can_pin_init function in bsp_can_init
    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    // can_drv_init function in bsp_can_init
    FLEXCAN_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    FLEXCAN_DRV_InstallEventCallback_AddCallback(stub_flexcan_event_cb_install);
    FLEXCAN_DRV_InstallEventCallback_ExpectAnyArgs();

    FLEXCAN_DRV_InstallErrorCallback_AddCallback(stub_flexcan_drv_install);
    FLEXCAN_DRV_InstallErrorCallback_ExpectAnyArgs();

    (void)bsp_can_init();

    FLEXCAN_DRV_Deinit_ExpectAndReturn(TEST_CAN_INSTANCE, STATUS_SUCCESS);

    (void)bsp_can_deinit();

    m_test_callback_addr.err_cb = NULL;
    m_test_callback_addr.rx_cb = NULL;
    m_test_callback_addr.tx_cb = NULL;

    bsp_can_cb_set(&m_test_callback_addr);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1); 
    m_test_event_callback_addr(0U, FLEXCAN_EVENT_TX_COMPLETE, 0U, &flexcanState);
}


/**
 * @brief This test verifies bsp_can_cb_set function with only registered rx
 *        callback function.
 * 
 * @link https://codebeamer.daiichi.com/issue/191014
 * 
 */
void test_bsp_can_cb_set__without_rx_cb_value__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // can_pin_init function in bsp_can_init
    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    // can_drv_init function in bsp_can_init
    FLEXCAN_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    FLEXCAN_DRV_InstallEventCallback_AddCallback(stub_flexcan_event_cb_install);
    FLEXCAN_DRV_InstallEventCallback_ExpectAnyArgs();

    FLEXCAN_DRV_InstallErrorCallback_AddCallback(stub_flexcan_drv_install);
    FLEXCAN_DRV_InstallErrorCallback_ExpectAnyArgs();

    (void)bsp_can_init();

    m_test_callback_addr.err_cb = NULL;
    m_test_callback_addr.rx_cb = m_test_rx_function;
    m_test_callback_addr.tx_cb = NULL;

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_can_status_t ret = bsp_can_cb_set(&m_test_callback_addr);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(BSP_CAN_STATUS_OK, ret);
}
/**
 * @brief This test verifies bsp_can_cb_set function with only registered tx
 *        callback function.
 * 
 * @link https://codebeamer.daiichi.com/issue/191015
 * 
 */
void test_bsp_can_cb_set__without_tx_cb_value__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // can_pin_init function in bsp_can_init
    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    // can_drv_init function in bsp_can_init
    FLEXCAN_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    FLEXCAN_DRV_InstallEventCallback_AddCallback(stub_flexcan_event_cb_install);
    FLEXCAN_DRV_InstallEventCallback_ExpectAnyArgs();

    FLEXCAN_DRV_InstallErrorCallback_AddCallback(stub_flexcan_drv_install);
    FLEXCAN_DRV_InstallErrorCallback_ExpectAnyArgs();

    (void)bsp_can_init();

    m_test_callback_addr.err_cb = NULL;
    m_test_callback_addr.rx_cb = NULL;
    m_test_callback_addr.tx_cb = (bsp_can_tx_cb_t)m_test_tx_function;

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_can_status_t ret = bsp_can_cb_set(&m_test_callback_addr);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(BSP_CAN_STATUS_OK, ret);
}
/**
 * @brief This test verifies bsp_can_cb_set function with only registered error
 *        callback function.
 * 
 * @link https://codebeamer.daiichi.com/issue/191016
 * 
 */
void test_bsp_can_cb_set__without_err_cb_value__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // can_pin_init function in bsp_can_init
    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    // can_drv_init function in bsp_can_init
    FLEXCAN_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    FLEXCAN_DRV_InstallEventCallback_AddCallback(stub_flexcan_event_cb_install);
    FLEXCAN_DRV_InstallEventCallback_ExpectAnyArgs();

    FLEXCAN_DRV_InstallErrorCallback_AddCallback(stub_flexcan_drv_install);
    FLEXCAN_DRV_InstallErrorCallback_ExpectAnyArgs();

    (void)bsp_can_init();

    m_test_callback_addr.err_cb = m_test_err_function;
    m_test_callback_addr.rx_cb = NULL;
    m_test_callback_addr.tx_cb = NULL;

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_can_status_t ret = bsp_can_cb_set(&m_test_callback_addr);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(BSP_CAN_STATUS_OK, ret);
}

/**
 * @brief The test verifies the bsp_can_cb_set function with uses the
 *        can_callback() function in the bsp_can module.
 *
 * @link https://codebeamer.daiichi.com/issue/190653
 *
 */
void test_bsp_can_cb_set__callback_rx_fifo_complete__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    flexcan_state_t flexcanState;

    // **************************** Pre-Actions ***************************** //

    helper_pins_and_flexcan_init_callback_set();

    // ************************** Expected Calls **************************** //

    uint8_t test_payload[8U] = {0U};

    m_test_recv_buff.msgId = m_test_tx_msg_id;

    m_test_recv_buff.dataLen = 8U;

    for (size_t i = 0U; i < 8U; i++)
    {
        m_test_recv_buff.data[i] = test_payload[i];
    }

    m_test_data.msg_id = m_test_tx_msg_id;

    for (size_t i = 0U; i < 8U; i++)
    {
        m_test_data.payload[i] = test_payload[i];
    }

    FLEXCAN_DRV_RxFifo_ExpectAndReturn(TEST_CAN_INSTANCE, NULL, STATUS_SUCCESS);

    FLEXCAN_DRV_RxFifo_IgnoreArg_data();

    FLEXCAN_DRV_RxFifo_ReturnThruPtr_data(&m_test_recv_buff);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    m_test_event_callback_addr(0U,
                               FLEXCAN_EVENT_RXFIFO_COMPLETE, 
                               0U, 
                               &flexcanState);
}

/**
 * @brief The test verifies failure status the bsp_can_cb_set function with uses
 *        the can_callback() function in the bsp_can module.
 *
 * @link https://codebeamer.daiichi.com/issue/190976
 *
 */
void test_bsp_can_cb_set__callback_rx_fifo_complete__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    flexcan_state_t flexcanState;

    // **************************** Pre-Actions ***************************** //

    helper_pins_and_flexcan_init_callback_set();

    // ************************** Expected Calls **************************** //

    uint8_t test_payload[8U] = {0U};

    m_test_recv_buff.msgId = m_test_tx_msg_id;

    m_test_recv_buff.dataLen = 8U;

    for (size_t i = 0U; i < 8U; i++)
    {
        m_test_recv_buff.data[i] = test_payload[i];
    }

    m_test_data.msg_id = m_test_tx_msg_id;

    for (size_t i = 0U; i < 8U; i++)
    {
        m_test_data.payload[i] = test_payload[i];
    }

    FLEXCAN_DRV_RxFifo_ExpectAndReturn(TEST_CAN_INSTANCE, NULL, STATUS_ERROR);

    FLEXCAN_DRV_RxFifo_IgnoreArg_data();

    FLEXCAN_DRV_RxFifo_ReturnThruPtr_data(&m_test_recv_buff);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    m_test_event_callback_addr(0U,
                               FLEXCAN_EVENT_RXFIFO_COMPLETE, 
                               0U, 
                               &flexcanState);
}

/**
 * @brief This test verifies the bsp_can_cb_set function with the adress of
 *        static function can_callback, TX complete status.
 *
 * @link https://codebeamer.daiichi.com/issue/190655
 *
 */
void test_bsp_can_cb_set__callback_tx_complete__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    flexcan_state_t flexcanState;

    // **************************** Pre-Actions ***************************** //

    helper_pins_and_flexcan_init_callback_set();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    m_test_event_callback_addr(0U,
                               FLEXCAN_EVENT_TX_COMPLETE, 
                               0U, 
                               &flexcanState);
}

/**
 * @brief This test verifies the bsp_can_cb_set function with bit0 error status.
 *
 * @link https://codebeamer.daiichi.com/issue/190656
 *
 */
void test_bsp_can_cb_set__error_callback_bit0_error__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    flexcan_state_t flexcanState;

    // **************************** Pre-Actions ***************************** //

    helper_pins_and_flexcan_init_callback_set();

    // ************************** Expected Calls **************************** //
    
    uint32_t bit1_error = 0x8000U; 

    FLEXCAN_DRV_GetErrorStatus_ExpectAndReturn(TEST_CAN_INSTANCE, bit1_error);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    m_test_err_callback_addr(TEST_CAN_INSTANCE,
                             FLEXCAN_EVENT_ERROR, 
                             &flexcanState);
}

/**
 * @brief This test verifies the bsp_can_cb_set function with bit1 error status.
 *
 * @link https://codebeamer.daiichi.com/issue/190659
 *
 */
void test_bsp_can_cb_set__error_callback_bit1_error__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    flexcan_state_t flexcanState;

    // **************************** Pre-Actions ***************************** //

    helper_pins_and_flexcan_init_callback_set();

    // ************************** Expected Calls **************************** //

    uint32_t bit1_error = 0x4000U;

    FLEXCAN_DRV_GetErrorStatus_ExpectAndReturn(TEST_CAN_INSTANCE, bit1_error);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    m_test_err_callback_addr(TEST_CAN_INSTANCE,
                             FLEXCAN_EVENT_ERROR, 
                             &flexcanState);
}

/**
 * @brief This test verifies the bsp_can_cb_set function with ACK error status.
 *
 * @link https://codebeamer.daiichi.com/issue/190662
 *
 */
void test_bsp_can_cb_set__error_callback_ack_error__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    flexcan_state_t flexcanState;

    // **************************** Pre-Actions ***************************** //

    helper_pins_and_flexcan_init_callback_set();

    // ************************** Expected Calls **************************** //

    uint32_t bit1_error = 0x2000U;

    FLEXCAN_DRV_GetErrorStatus_ExpectAndReturn(TEST_CAN_INSTANCE, bit1_error);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    m_test_err_callback_addr(TEST_CAN_INSTANCE,
                             FLEXCAN_EVENT_ERROR, 
                             &flexcanState);
}

/**
 * @brief This test verifies the bsp_can_cb_set function with FORM error status.
 *
 * @link https://codebeamer.daiichi.com/issue/190663
 *
 */
void test_bsp_can_cb_set__error_callback_form_error__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    flexcan_state_t flexcanState;

    // **************************** Pre-Actions ***************************** //

    helper_pins_and_flexcan_init_callback_set();

    // ************************** Expected Calls **************************** //

    uint32_t bit1_error = 0x800U;

    FLEXCAN_DRV_GetErrorStatus_ExpectAndReturn(TEST_CAN_INSTANCE, bit1_error);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    m_test_err_callback_addr(TEST_CAN_INSTANCE,
                             FLEXCAN_EVENT_ERROR, 
                             &flexcanState);
}

/**
 * @brief This test verifies the bsp_can_cb_set function with stuff error 
 *        status.
 *
 * @link https://codebeamer.daiichi.com/issue/190664
 *
 */
void test_bsp_can_cb_set__error_callback_stuff_error__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    flexcan_state_t flexcanState;

    // **************************** Pre-Actions ***************************** //

    helper_pins_and_flexcan_init_callback_set();

    // ************************** Expected Calls **************************** //
    
    uint32_t bit1_error = 0x400U;

    FLEXCAN_DRV_GetErrorStatus_ExpectAndReturn(TEST_CAN_INSTANCE, bit1_error);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    m_test_err_callback_addr(TEST_CAN_INSTANCE,
                             FLEXCAN_EVENT_ERROR, 
                             &flexcanState);
}

/**
 * @brief This test verifies the bsp_can_cb_set function with CRC error status.
 *
 * @link https://codebeamer.daiichi.com/issue/190665
 *
 */
void test_bsp_can_cb_set__error_callback_crc_error__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    flexcan_state_t flexcanState;

    // **************************** Pre-Actions ***************************** //

    helper_pins_and_flexcan_init_callback_set();

    // ************************** Expected Calls **************************** //
    
    uint32_t bit1_error = 0x1000U;

    FLEXCAN_DRV_GetErrorStatus_ExpectAndReturn(TEST_CAN_INSTANCE, bit1_error);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    m_test_err_callback_addr(TEST_CAN_INSTANCE,
                             FLEXCAN_EVENT_ERROR, 
                             &flexcanState);
}

/**
 * @brief This test verifies the bsp_can_cb_set function with BUS OFF error
 *        status.
 *
 * @link https://codebeamer.daiichi.com/issue/190666
 *
 */
void test_bsp_can_cb_set__error_callback_busoff_error__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    flexcan_state_t flexcanState;

    // **************************** Pre-Actions ***************************** //

    helper_pins_and_flexcan_init_callback_set();

    // ************************** Expected Calls **************************** //

    uint32_t bit1_error = 0x20U;

    FLEXCAN_DRV_GetErrorStatus_ExpectAndReturn(TEST_CAN_INSTANCE, bit1_error);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    m_test_err_callback_addr(TEST_CAN_INSTANCE,
                             FLEXCAN_EVENT_ERROR,
                             &flexcanState);
}

/**
 * @brief This test verifies the bsp_can_cb_set function with active error
 *        status.
 *
 * @link https://codebeamer.daiichi.com/issue/190668
 *
 */
void test_bsp_can_cb_set__error_callback_active_error__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    flexcan_state_t flexcanState;

    // **************************** Pre-Actions ***************************** //

    helper_pins_and_flexcan_init_callback_set();

    // ************************** Expected Calls **************************** //

    uint32_t bit1_error = 0x00U;

    FLEXCAN_DRV_GetErrorStatus_ExpectAndReturn(TEST_CAN_INSTANCE, bit1_error);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    m_test_err_callback_addr(TEST_CAN_INSTANCE,
                             FLEXCAN_EVENT_ERROR,
                             &flexcanState);
}

/**
 * @brief This test verifies the bsp_can_cb_set function with passive error
 *        status.
 *
 * @link https://codebeamer.daiichi.com/issue/190667
 *
 */
void test_bsp_can_cb_set__error_callback_passive_error__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    flexcan_state_t flexcanState;

    // **************************** Pre-Actions ***************************** //

    helper_pins_and_flexcan_init_callback_set();

    // ************************** Expected Calls **************************** //

    uint32_t bit1_error = 0x10U;

    FLEXCAN_DRV_GetErrorStatus_ExpectAndReturn(TEST_CAN_INSTANCE, bit1_error);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    m_test_err_callback_addr(TEST_CAN_INSTANCE,
                             FLEXCAN_EVENT_ERROR, 
                             &flexcanState);
}

/**
 * @brief This test verifies bsp_can_fault_state_get function with valid
 *        argument
 *
 * @link https://codebeamer.daiichi.com/issue/190978
 *
 */
void test_bsp_can_fault_state_get__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    bsp_can_fault_state_t fault_state;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(2);
    bsp_can_status_t ret = bsp_can_fault_state_get(&fault_state);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(BSP_CAN_STATUS_OK, ret);
}

/**
 * @brief This test verifies bsp_can_fault_state_get function with valid
 *        argument.
 *
 * @link https://codebeamer.daiichi.com/issue/190979
 *
 */
void test_bsp_can_fault_state_get__null_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_can_status_t ret = bsp_can_fault_state_get(NULL);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(BSP_CAN_STATUS_ERROR, ret);
}

/**
 * @brief This test simulates the error callback with the masking test bit,
 *        confirming that the active value is returned.
 * 
 * @link https://codebeamer.daiichi.com/issue/192221
 * 
 */
void test_bsp_can_fault_state_get__active_state__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    
    flexcan_state_t flexcanState;

    bsp_can_fault_state_t test_state;

    // **************************** Pre-Actions ***************************** //

    // can_pin_init function in bsp_can_init
    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    // can_drv_init function in bsp_can_init
    FLEXCAN_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    FLEXCAN_DRV_InstallEventCallback_AddCallback(stub_flexcan_event_cb_install);

    FLEXCAN_DRV_InstallEventCallback_ExpectAnyArgs();

    FLEXCAN_DRV_InstallErrorCallback_AddCallback(stub_flexcan_drv_install);

    FLEXCAN_DRV_InstallErrorCallback_ExpectAnyArgs();

    (void)bsp_can_init();

    m_test_callback_addr.err_cb = (bsp_can_err_cb_t)m_test_err_function;
    m_test_callback_addr.rx_cb = (bsp_can_rx_cb_t)m_test_rx_function;
    m_test_callback_addr.tx_cb = (bsp_can_tx_cb_t)m_test_tx_function;

    bsp_can_cb_set(&m_test_callback_addr);

    // ************************** Expected Calls **************************** //

    uint32_t bit1_error = 0x00U;

    FLEXCAN_DRV_GetErrorStatus_ExpectAndReturn(TEST_CAN_INSTANCE, bit1_error);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    m_test_err_callback_addr(TEST_CAN_INSTANCE,
                             FLEXCAN_EVENT_ERROR, 
                             &flexcanState);

    TEST_STEP(2);
    bsp_can_status_t ret = bsp_can_fault_state_get(&test_state);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(BSP_CAN_STATUS_OK, ret);

    TEST_STEP(4);
    TEST_ASSERT_EQUAL(BSP_CAN_FAULT_STATE_ACTIVE, test_state);
}

/**
 * @brief This test simulates the error callback with the masking test bit,
 *        confirming that the passive value is returned.
 * 
 * @link https://codebeamer.daiichi.com/issue/192222
 * 
 */
void test_bsp_can_fault_state_get__passive_state__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    
    flexcan_state_t flexcanState;

    bsp_can_fault_state_t test_state;

    // **************************** Pre-Actions ***************************** //

    // can_pin_init function in bsp_can_init
    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    // can_drv_init function in bsp_can_init
    FLEXCAN_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    FLEXCAN_DRV_InstallEventCallback_AddCallback(stub_flexcan_event_cb_install);

    FLEXCAN_DRV_InstallEventCallback_ExpectAnyArgs();

    FLEXCAN_DRV_InstallErrorCallback_AddCallback(stub_flexcan_drv_install);

    FLEXCAN_DRV_InstallErrorCallback_ExpectAnyArgs();

    (void)bsp_can_init();

    m_test_callback_addr.err_cb = m_test_err_function;
    m_test_callback_addr.rx_cb = m_test_rx_function;
    m_test_callback_addr.tx_cb = m_test_tx_function;

    bsp_can_cb_set(&m_test_callback_addr);

    // ************************** Expected Calls **************************** //

    uint32_t bit1_error = 0x10U;

    FLEXCAN_DRV_GetErrorStatus_ExpectAndReturn(TEST_CAN_INSTANCE, bit1_error);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    m_test_err_callback_addr(TEST_CAN_INSTANCE,
                             FLEXCAN_EVENT_ERROR, 
                             &flexcanState);

    TEST_STEP(2);
    bsp_can_status_t ret = bsp_can_fault_state_get(&test_state);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(BSP_CAN_STATUS_OK, ret);

    TEST_STEP(4);
    TEST_ASSERT_EQUAL(BSP_CAN_FAULT_STATE_PASSIVE, test_state);
}

/**
 * @brief This test simulates the error callback with the masking test bit,
 *        confirming that the bus-off value is returned.
 * 
 * @link https://codebeamer.daiichi.com/issue/192223
 * 
 */
void test_bsp_can_fault_state_get__busoff_state__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    
    flexcan_state_t flexcanState;

    bsp_can_fault_state_t test_state;

    // **************************** Pre-Actions ***************************** //

    // can_pin_init function in bsp_can_init
    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    // can_drv_init function in bsp_can_init
    FLEXCAN_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    FLEXCAN_DRV_InstallEventCallback_AddCallback(stub_flexcan_event_cb_install);

    FLEXCAN_DRV_InstallEventCallback_ExpectAnyArgs();

    FLEXCAN_DRV_InstallErrorCallback_AddCallback(stub_flexcan_drv_install);

    FLEXCAN_DRV_InstallErrorCallback_ExpectAnyArgs();

    (void)bsp_can_init();

    m_test_callback_addr.err_cb = m_test_err_function;
    m_test_callback_addr.rx_cb = m_test_rx_function;
    m_test_callback_addr.tx_cb = m_test_tx_function;

    bsp_can_cb_set(&m_test_callback_addr);

    // ************************** Expected Calls **************************** //

    uint32_t bit1_error = 0x20U;

    FLEXCAN_DRV_GetErrorStatus_ExpectAndReturn(TEST_CAN_INSTANCE, bit1_error);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    m_test_err_callback_addr(TEST_CAN_INSTANCE,
                             FLEXCAN_EVENT_ERROR, 
                             &flexcanState);

    TEST_STEP(2);
    bsp_can_status_t ret = bsp_can_fault_state_get(&test_state);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(BSP_CAN_STATUS_OK, ret);

    TEST_STEP(4);
    TEST_ASSERT_EQUAL(BSP_CAN_FAULT_STATE_BUSOFF, test_state);
}

/**
 * @brief This test verifies bsp_can_rx_err_cnt_get function with a test
 *        argument.
 *
 * @link https://codebeamer.daiichi.com/issue/192215
 *
 */
void test_bsp_can_rx_err_cnt_get__test_counter_val__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    
    static CAN_Type * const m_flexcan_base[] = CAN_BASE_PTRS; 

    CAN_Type* base = m_flexcan_base[0U];

    // **************************** Pre-Actions ***************************** //
    
    base->ECR =  0xA34U;

    uint32_t err_cnt_reg = base->ECR;

    uint8_t test = (uint8_t)(err_cnt_reg >> 8U);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_can_status_t ret = bsp_can_rx_err_cnt_get(&m_test_rx_cnt);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(BSP_CAN_STATUS_OK, ret);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(test, m_test_rx_cnt);
}

/**
 * @brief This test verifies bsp_can_rx_err_cnt_get function with a valid
 *        argument.
 *
 * @link https://codebeamer.daiichi.com/issue/190983
 *
 */
void test_bsp_can_rx_err_cnt_get__null_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_can_status_t ret = bsp_can_rx_err_cnt_get(NULL);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(BSP_CAN_STATUS_ERROR, ret);
}

/**
 * @brief This test verifies bsp_can_tx_err_cnt_get function with a valid
 *        argument.
 *
 * @link https://codebeamer.daiichi.com/issue/190984
 *
 */
void test_bsp_can_tx_err_cnt_get__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_can_status_t ret = bsp_can_tx_err_cnt_get(&m_test_tx_cnt);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(BSP_CAN_STATUS_OK, ret);
}

/**
 * @brief This test verifies bsp_can_tx_err_cnt function with test value 0xA34U.
 * 
 * @link https://codebeamer.daiichi.com/issue/192216
 * 
 */
void test_bsp_can_tx_err_cnt_get__test_counter_val__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    
    static CAN_Type * const m_flexcan_base[] = CAN_BASE_PTRS; 

    CAN_Type* base = m_flexcan_base[0U];

    base->ECR =  0xA34U;

    uint8_t test = (uint8_t)(base->ECR);

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_can_status_t ret = bsp_can_tx_err_cnt_get(&m_test_tx_cnt);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(BSP_CAN_STATUS_OK, ret);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(test, m_test_tx_cnt);
}

/**
 * @brief This test verifies bsp_can_tx_err_cnt_get function with a NULL
 *        argument.
 *
 * @link https://codebeamer.daiichi.com/issue/190985
 *
 */
void test_bsp_can_tx_err_cnt_get__null_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_can_status_t ret = bsp_can_tx_err_cnt_get(NULL);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(BSP_CAN_STATUS_ERROR, ret);
}

/*******************************************************************************
 * Private Functions Definitions
 ******************************************************************************/

static void helper_pins_and_flexcan_init_callback_set(void)
{
    // can_pin_init function in bsp_can_init
    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    // can_drv_init function in bsp_can_init
    FLEXCAN_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    FLEXCAN_DRV_InstallEventCallback_AddCallback(stub_flexcan_event_cb_install);

    FLEXCAN_DRV_InstallEventCallback_ExpectAnyArgs();

    FLEXCAN_DRV_InstallErrorCallback_AddCallback(stub_flexcan_drv_install);

    FLEXCAN_DRV_InstallErrorCallback_ExpectAnyArgs();

    (void)bsp_can_init();

    m_test_callback_addr.err_cb = m_test_err_function;
    m_test_callback_addr.rx_cb = m_test_rx_function;
    m_test_callback_addr.tx_cb = m_test_tx_function;

    bsp_can_cb_set(&m_test_callback_addr);
}

static status_t stub_pins_drv_init(uint32_t pinCount,
                                   const pin_settings_config_t *config,
                                   int cmock_num_calls)
{
    (void)cmock_num_calls;

    TEST_ASSERT_EQUAL(TEST_CAN_PINS_MAX, pinCount);

    const pin_settings_config_t m_can_pins_cfg[TEST_CAN_PINS_MAX] =
        {
            {
                // PRQA S 306 ++
                .base = PORTC,
                // PRQA S 306 --
                .pinPortIdx = 2,
                .pullConfig = PORT_INTERNAL_PULL_NOT_ENABLED,
                .driveSelect = PORT_LOW_DRIVE_STRENGTH,
                .passiveFilter = false,
                .mux = PORT_MUX_ALT3,
                .pinLock = false,
                .intConfig = PORT_DMA_INT_DISABLED,
                .clearIntFlag = false,
                .gpioBase = NULL,
                .digitalFilter = false,
            },
            {
                // PRQA S 306 ++
                .base = PORTC,
                // PRQA S 306 --
                .pinPortIdx = 3,
                .pullConfig = PORT_INTERNAL_PULL_NOT_ENABLED,
                .driveSelect = PORT_LOW_DRIVE_STRENGTH,
                .passiveFilter = false,
                .mux = PORT_MUX_ALT3,
                .pinLock = false,
                .intConfig = PORT_DMA_INT_DISABLED,
                .clearIntFlag = false,
                .gpioBase = NULL,
                .digitalFilter = false,
            },
        };

    for (uint8_t i = 0U; i < TEST_CAN_PINS_MAX; ++i)
    {
        TEST_ASSERT_EQUAL(m_can_pins_cfg[i].base,
                          config[i].base);
        TEST_ASSERT_EQUAL(m_can_pins_cfg[i].clearIntFlag,
                          config[i].clearIntFlag);
        TEST_ASSERT_EQUAL(m_can_pins_cfg[i].digitalFilter,
                          config[i].digitalFilter);
        TEST_ASSERT_EQUAL(m_can_pins_cfg[i].direction,
                          config[i].direction);
        TEST_ASSERT_EQUAL(m_can_pins_cfg[i].driveSelect,
                          config[i].driveSelect);
        TEST_ASSERT_EQUAL(m_can_pins_cfg[i].gpioBase,
                          config[i].gpioBase);
        TEST_ASSERT_EQUAL(m_can_pins_cfg[i].initValue,
                          config[i].initValue);
        TEST_ASSERT_EQUAL(m_can_pins_cfg[i].intConfig,
                          config[i].intConfig);
        TEST_ASSERT_EQUAL(m_can_pins_cfg[i].mux,
                          config[i].mux);
        TEST_ASSERT_EQUAL(m_can_pins_cfg[i].passiveFilter,
                          config[i].passiveFilter);
        TEST_ASSERT_EQUAL(m_can_pins_cfg[i].pinLock,
                          config[i].pinLock);
        TEST_ASSERT_EQUAL(m_can_pins_cfg[i].pinPortIdx,
                          config[i].pinPortIdx);
        TEST_ASSERT_EQUAL(m_can_pins_cfg[i].pullConfig,
                          config[i].pullConfig);
    }

    return STATUS_SUCCESS;
}

static status_t stub_flexcan_drv_init(uint8_t instance,
                                      flexcan_state_t *state,
                                      const flexcan_user_config_t *data,
                                      int cmock_num_calls)
{
    (void)cmock_num_calls;
    (void)state;

    TEST_ASSERT_EQUAL(TEST_CAN_INSTANCE, instance);

    const test_can_arbitration_phase_t speed_tbl[TEST_CAN_SPEED_MAX] =
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

    flexcan_user_config_t can_cfg;

    can_cfg.bitrate.propSeg = 0U;
    can_cfg.bitrate.phaseSeg1 = 0U;
    can_cfg.bitrate.phaseSeg2 = 0U;
    can_cfg.bitrate.preDivider = 0U;
    can_cfg.bitrate.rJumpwidth = 0U;

    bool found = false;

    for (uint32_t cnt = 0; (cnt < TEST_CAN_SPEED_MAX) && (true != found); cnt++)
    {
        if (500000U == speed_tbl[cnt].bitrate_kbps)
        {
            can_cfg.bitrate.propSeg = speed_tbl[cnt].prog_segment;
            can_cfg.bitrate.phaseSeg1 = speed_tbl[cnt].phase_segment_1;
            can_cfg.bitrate.phaseSeg2 = speed_tbl[cnt].phase_segment_2;
            can_cfg.bitrate.preDivider = speed_tbl[cnt].pre_divider;
            can_cfg.bitrate.rJumpwidth = speed_tbl[cnt].rjump_width;

            found = true;
        }
    }

    can_cfg.flexcanMode = FLEXCAN_NORMAL_MODE;
    can_cfg.is_rx_fifo_needed = true;
    can_cfg.max_num_mb = 32U;
    can_cfg.num_id_filters = m_can_filter_num_get(32U);
    can_cfg.transfer_type = FLEXCAN_RXFIFO_USING_INTERRUPTS;

    TEST_ASSERT_EQUAL(can_cfg.bitrate.propSeg, (*data).bitrate.propSeg);
    TEST_ASSERT_EQUAL(can_cfg.bitrate.phaseSeg1, (*data).bitrate.phaseSeg1);
    TEST_ASSERT_EQUAL(can_cfg.bitrate.phaseSeg2, (*data).bitrate.phaseSeg2);
    TEST_ASSERT_EQUAL(can_cfg.bitrate.preDivider, (*data).bitrate.preDivider);
    TEST_ASSERT_EQUAL(can_cfg.bitrate.rJumpwidth, (*data).bitrate.rJumpwidth);
    TEST_ASSERT_EQUAL(can_cfg.flexcanMode, (*data).flexcanMode);
    TEST_ASSERT_EQUAL(can_cfg.is_rx_fifo_needed, (*data).is_rx_fifo_needed);
    TEST_ASSERT_EQUAL(can_cfg.max_num_mb, (*data).max_num_mb);
    TEST_ASSERT_EQUAL(can_cfg.num_id_filters, (*data).num_id_filters);
    TEST_ASSERT_EQUAL(can_cfg.transfer_type, (*data).transfer_type);

    return STATUS_SUCCESS;
}

static flexcan_rx_fifo_id_filter_num_t m_can_filter_num_get(uint32_t msg_count)
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

static void stub_flexcan_drv_install(uint8_t instance,
                                     flexcan_error_callback_t callback,
                                     void *callbackParam,
                                     int cmock_num_calls)
{
    (void)instance;
    (void)cmock_num_calls;
    (void)callbackParam;

    m_test_err_callback_addr = callback;
}

static void stub_flexcan_event_cb_install(uint8_t instance,
                                          flexcan_callback_t callback,
                                          void *callbackParam,
                                          int cmock_num_calls)
{
    (void)instance;
    (void)cmock_num_calls;
    (void)callbackParam;

    m_test_event_callback_addr = callback;
}

static void m_test_err_function(bsp_can_err_status_t error)
{
    bsp_can_fault_state_t error_state;

    switch (error)
    {
    case BSP_CAN_ERR_STATUS_BIT_MONITORING1:

        TEST_ASSERT_EQUAL(BSP_CAN_ERR_STATUS_BIT_MONITORING1, error);

        break;

    case BSP_CAN_ERR_STATUS_BIT_MONITORING2:

        TEST_ASSERT_EQUAL(BSP_CAN_ERR_STATUS_BIT_MONITORING2, error);

        break;

    case BSP_CAN_ERR_STATUS_CHECK_ACK_FAILED:

        TEST_ASSERT_EQUAL(BSP_CAN_ERR_STATUS_CHECK_ACK_FAILED, error);

        break;
        
    case BSP_CAN_ERR_STATUS_CHECK_FORM_FAILED:

        TEST_ASSERT_EQUAL(BSP_CAN_ERR_STATUS_CHECK_FORM_FAILED, error);

        break;

    case BSP_CAN_ERR_STATUS_CHECK_STUFFING_FAILED:

        TEST_ASSERT_EQUAL(BSP_CAN_ERR_STATUS_CHECK_STUFFING_FAILED, error);

        break;

    case BSP_CAN_ERR_STATUS_CHECK_CRC_FAILED:

        TEST_ASSERT_EQUAL(BSP_CAN_ERR_STATUS_CHECK_CRC_FAILED, error);

        break;

    default:
        //
        break;

        TEST_ASSERT_EQUAL(BSP_CAN_FAULT_STATE_ACTIVE, error_state);
    }
}

static void m_test_tx_function(const bsp_can_data_t *p_tx_data)
{
    if (p_tx_data->msg_id != 0U)
    {
        TEST_ASSERT_EQUAL(p_tx_data->msg_id, m_test_tx_msg_id);
    }

    return;
}

static void m_test_rx_function(bsp_can_data_t *p_rx_data)
{
    TEST_ASSERT_EQUAL(p_rx_data->msg_id, m_test_recv_buff.msgId);

    TEST_ASSERT_EQUAL(p_rx_data->size, m_test_recv_buff.dataLen);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(p_rx_data->payload, m_test_recv_buff.data, 8U);
}

// end of file
