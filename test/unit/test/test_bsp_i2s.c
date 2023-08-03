/**
 * @file test_bsp_i2s.c
 * @author Muhammed Salih AYDOGAN
 * @brief These tests verify the units of the I2S module.
 * 
 * @version 0.1
 * 
 * @date 25/05/2023
 * 
 * @copyright Copyright (c) 2023
 * 
 * https://codebeamer.daiichi.com/item/193599
 * 
 */

/*******************************************************************************
 * Includes 
 ******************************************************************************/

//First - include test framework headers
#include "unity.h"
#include "support.h"

// Second - include the module to be tested
#include "bsp_i2s.h"

// Third - include the mocks of required modules
#include "mock_edma_driver.h"
#include "mock_flexio_common.h"
#include "mock_flexio.h"
#include "mock_pins_driver.h"
#include "mock_flexio_i2s_driver.h"

#include "mock_log.h"

/*******************************************************************************
 * Test Files
 ******************************************************************************/

TEST_FILE("bsp_i2s.c");

/*******************************************************************************             
 * Defines
 ******************************************************************************/
/**
 * @brief https://codebeamer.daiichi.com/issue/189282
 *
 */
#define TEST_I2S_INSTANCE       0U

/**
 * @brief https://codebeamer.daiichi.com/issue/189284
 *
 */
#define TEST_I2S_PINS_COUNT     4U

/**
 * @brief https://codebeamer.daiichi.com/issue/189287
 *
 */
#define TEST_EDMA_CH_1          1U

/**
 * @brief https://codebeamer.daiichi.com/issue/189289
 *
 */
#define TEST_EDMA_CH_2          2U

/*******************************************************************************
 * Extern Variables
 ******************************************************************************/

/*******************************************************************************
 * Private Variables
 ******************************************************************************/
/**
 * @brief This variable holds i2s status value. 
*/
static bsp_i2s_status_t m_test_status;

/**
 * @brief This variable holds i2s event callback address. 
*/
static i2s_callback_t m_test_i2s_event_cb = NULL;

/**
 * @brief This variable contain EDMA channel0 configuration. 
*/
static const edma_channel_config_t m_test_dma_controller_ch0_cfg =
{
    .channelPriority = EDMA_CHN_PRIORITY_3, 
    .virtChnConfig = TEST_EDMA_CH_1,
    .source = EDMA_REQ_DISABLED,
    .callback = NULL,
    .callbackParam = NULL,
    .enableTrigger = false,
};

/**
 * @brief This variable contain EDMA channel1 configuration. 
*/
static const edma_channel_config_t m_test_dma_controller_ch1_cfg = 
{
    .channelPriority = EDMA_CHN_PRIORITY_3, 
    .virtChnConfig = TEST_EDMA_CH_2,
    .source = EDMA_REQ_DISABLED,
    .callback = NULL,
    .callbackParam = NULL,
    .enableTrigger = false,
};

/*******************************************************************************
 * Global Variables
 ******************************************************************************/

uint8_t allocated_area[PERIPH_MEM_SIZE];

/*******************************************************************************
 * Private Functions Prototype Declarations
 ******************************************************************************/
/**
 * @brief This callback function is called when an I2S event occurs.
*/
static void test_callback(bsp_i2s_status_t status);

/**
 * @brief This stub function uses to take the callback function address.
*/
static status_t test_flexio_drv_init_device_stub(
    uint32_t instance, 
    const flexio_i2s_master_user_config_t* userConfigPtr, 
    flexio_i2s_master_state_t* master,
    int cmock_num_calls);

/**
 * @brief The helper function starts bsp_i2s_init() successfully.
*/
static void helper_bsp_i2s_init(void);

/**
 * @brief The helper function starts bsp_i2s_deinit() successfully.
*/
static void helper_bsp_i2s_deinit(void);

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
 * @brief Verifies bsp_i2s_deinit function when deinitialization is successful.
 * 
 * https://codebeamer.daiichi.com/item/193621
 *  
 */
void test_bsp_i2s_deinit__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //
    
    // ************************** Expected Calls **************************** //

    flexio_i2s_master_state_t i2s_master_state = {0U};

    FLEXIO_I2S_DRV_MasterDeinit_ExpectAndReturn(&i2s_master_state, 
                                                STATUS_SUCCESS);
    FLEXIO_DRV_DeinitDevice_ExpectAndReturn(TEST_I2S_INSTANCE, STATUS_SUCCESS);
    EDMA_DRV_ReleaseChannel_ExpectAndReturn(TEST_EDMA_CH_1, STATUS_SUCCESS);
    EDMA_DRV_ReleaseChannel_ExpectAndReturn(TEST_EDMA_CH_2, STATUS_SUCCESS);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = bsp_i2s_deinit();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief Verifies bsp_i2s_deinit function when deinitialization is failed with 
 * channel1 state.
 * 
 * https://codebeamer.daiichi.com/item/193787
 *  
 */
void test_bsp_i2s_deinit__channel_1_state__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //
    
    // ************************** Expected Calls **************************** //

    FLEXIO_I2S_DRV_MasterDeinit_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    FLEXIO_DRV_DeinitDevice_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    EDMA_DRV_ReleaseChannel_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = bsp_i2s_deinit();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies bsp_i2s_deinit function when deinitialization is failed with 
 * channel2 state.
 * 
 * https://codebeamer.daiichi.com/item/193789
 *  
 */
void test_bsp_i2s_deinit__channel_2_state__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //
    
    // ************************** Expected Calls **************************** //

    FLEXIO_I2S_DRV_MasterDeinit_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    FLEXIO_DRV_DeinitDevice_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    EDMA_DRV_ReleaseChannel_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    EDMA_DRV_ReleaseChannel_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = bsp_i2s_deinit();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies bsp_i2s_deinit function when FLEXIO_DRV_DeinitDevice function 
 * is fail. 
 * 
 * https://codebeamer.daiichi.com/item/193622
 *  
 */
void test_bsp_i2s_deinit__when_flexio_drive_error_occur__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //
    
    // ************************** Expected Calls **************************** //

    FLEXIO_I2S_DRV_MasterDeinit_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    FLEXIO_DRV_DeinitDevice_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = bsp_i2s_deinit();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies bsp_i2s_deinit function when FLEXIO_I2S_DRV_MasterDeinit 
 * function is fail.
 * 
 * https://codebeamer.daiichi.com/item/193623
 *  
 */
void test_bsp_i2s_deinit__when_busy_error_occur__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //
    
    // ************************** Expected Calls **************************** //

    FLEXIO_I2S_DRV_MasterDeinit_ExpectAnyArgsAndReturn(STATUS_BUSY);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = bsp_i2s_deinit();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies bsp_i2s_write function when use invalid data argument.  
 * 
 * https://codebeamer.daiichi.com/item/193624
 *  
 */
void test_bsp_i2s_write__when_use_invalid_data__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //
    
    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = bsp_i2s_write(NULL, (size_t)10U);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies bsp_i2s_write function when use invalid size argument. 
 * 
 * https://codebeamer.daiichi.com/item/193625
 *  
 */
void test_bsp_i2s_write__when_use_invalid_size__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    const uint8_t *data;
    uint8_t test_data = 10U;
    data = &test_data;
    // **************************** Pre-Actions ***************************** //
    
    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = bsp_i2s_write(data, (size_t)0U);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies bsp_i2s_write function with valid arguments.
 * 
 * https://codebeamer.daiichi.com/item/193626
 *  
 */
void test_bsp_i2s_write__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    const uint8_t *data;
    uint8_t test_data = 10U;
    data = &test_data;

    size_t size = sizeof(test_data);
    // **************************** Pre-Actions ***************************** //
    
    // ************************** Expected Calls **************************** //
    flexio_i2s_master_state_t test_i2s_master_state = {0U};

    FLEXIO_I2S_DRV_MasterSendData_ExpectAndReturn(&test_i2s_master_state, 
                                                  data, 
                                                  size, 
                                                  STATUS_SUCCESS);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = bsp_i2s_write(data, size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief Verifies bsp_i2s_cb_set function with invalid callback argument.
 * 
 * https://codebeamer.daiichi.com/item/193634
 *  
 */
void test_bsp_i2s_cb_set__when_use_invalid_callback__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //
    
    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = bsp_i2s_cb_set(NULL);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies bsp_i2s_cb_set function with valid callback argument.
 * 
 * https://codebeamer.daiichi.com/issue/193635
 *  
 */
void test_bsp_i2s_cb_set__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //
    
    helper_bsp_i2s_init();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = bsp_i2s_cb_set(test_callback);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);

    TEST_STEP(3);
    m_test_i2s_event_cb(I2S_EVENT_RX_FULL, NULL);

    TEST_STEP(4);
    TEST_ASSERT_EQUAL(BSP_I2S_RX_FULL, m_test_status);
}

/**
 * @brief The callback function set with the bsp_i2s_cb_set function is verified 
 * to be called when the transfer ends.
 * 
 * https://codebeamer.daiichi.com/item/193704
 *  
 */
void test_bsp_i2s_cb_set__when_transfer_end__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //
    
    helper_bsp_i2s_init();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = bsp_i2s_cb_set(test_callback);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);

    TEST_STEP(3);
    m_test_i2s_event_cb(I2S_EVENT_END_TRANSFER, NULL);

    TEST_STEP(4);
    TEST_ASSERT_EQUAL(BSP_I2S_END_TRANSFER, m_test_status);
}

/**
 * @brief The callback function set with the bsp_i2s_cb_set function is verified 
 * to be called when the error occurs.
 * 
 * https://codebeamer.daiichi.com/item/193705
 *  
 */
void test_bsp_i2s_cb_set__when_error_occur__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //
    
    helper_bsp_i2s_init();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = bsp_i2s_cb_set(test_callback);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);

    TEST_STEP(3);
    m_test_i2s_event_cb(I2S_EVENT_ERROR, NULL);

    TEST_STEP(4);
    TEST_ASSERT_EQUAL(BSP_I2S_ERROR, m_test_status);
}

/**
 * @brief The bsp_i2s_write function is verified when tx emty error occurs.
 * 
 * https://codebeamer.daiichi.com/item/194904
 *  
 */
void test_bsp_i2s_write__when_tx_emty_err_occur__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    const uint8_t *data;
    uint8_t test_data = 10U;
    data = &test_data;

    size_t size = sizeof(test_data);
    // **************************** Pre-Actions ***************************** //
    
    helper_bsp_i2s_init();
    (void)bsp_i2s_cb_set(test_callback);

    // ************************** Expected Calls **************************** //

    flexio_i2s_master_state_t test_i2s_master_state = {0U};
    FLEXIO_I2S_DRV_MasterSetTxBuffer_ExpectAndReturn(&test_i2s_master_state,
                                                     data,
                                                     size,
                                                     STATUS_SUCCESS);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    m_test_i2s_event_cb(I2S_EVENT_TX_EMPTY, NULL);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(BSP_I2S_TX_EMPTY, m_test_status);

    TEST_STEP(3);   
    bool ret = bsp_i2s_write(data, size);

    TEST_STEP(4);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief Verifies bsp_i2s_write function when tx buffer error.
 * 
 * https://codebeamer.daiichi.com/item/193790
 *  
 */
void test_bsp_i2s_write__tx_buffer_error__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    const uint8_t *data;
    uint8_t test_data = 10U;
    data = &test_data;

    size_t size = sizeof(test_data);
    // **************************** Pre-Actions ***************************** //
    
    helper_bsp_i2s_init();
    (void)bsp_i2s_cb_set(test_callback);

    // ************************** Expected Calls **************************** //
    
    FLEXIO_I2S_DRV_MasterSetTxBuffer_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    m_test_i2s_event_cb(I2S_EVENT_TX_EMPTY, NULL);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(BSP_I2S_TX_EMPTY, m_test_status);

    TEST_STEP(3);
    bool ret = bsp_i2s_write(data, size);

    TEST_STEP(4);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies bsp_i2s_write function when transaction error. 
 * 
 * https://codebeamer.daiichi.com/item/193791
 *  
 */
void test_bsp_i2s_write__when_drv_transaction_error__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    const uint8_t *data;
    uint8_t test_data = 10U;
    data = &test_data;

    size_t size = sizeof(test_data);
    // **************************** Pre-Actions ***************************** //
    
    helper_bsp_i2s_deinit();
    helper_bsp_i2s_init();

    // ************************** Expected Calls **************************** //

    FLEXIO_I2S_DRV_MasterSendData_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = bsp_i2s_write(data, size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies bsp_i2s_init function when initialization is successful.
 * 
 * https://codebeamer.daiichi.com/item/193665
 *  
 */
void test_bsp_i2s_init__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //
    const pin_settings_config_t test_pin_mux_init_cfg[TEST_I2S_PINS_COUNT] = 
    {
        {
            .base            = PORTD,
            .pinPortIdx      = 0U,
            .pullConfig      = PORT_INTERNAL_PULL_NOT_ENABLED,
            .driveSelect     = PORT_LOW_DRIVE_STRENGTH,
            .passiveFilter   = false,
            .mux             = PORT_MUX_ALT6,
            .pinLock         = false,
            .intConfig       = PORT_DMA_INT_DISABLED,
            .clearIntFlag    = false,
            .gpioBase        = NULL,
            .digitalFilter   = false,
        },
        {
            .base            = PORTD,
            .pinPortIdx      = 1U,
            .pullConfig      = PORT_INTERNAL_PULL_NOT_ENABLED,
            .driveSelect     = PORT_LOW_DRIVE_STRENGTH,
            .passiveFilter   = false,
            .mux             = PORT_MUX_ALT6,
            .pinLock         = false,
            .intConfig       = PORT_DMA_INT_DISABLED,
            .clearIntFlag    = false,
            .gpioBase        = NULL,
            .digitalFilter   = false,
        },
        {
            .base            = PORTD,
            .pinPortIdx      = 3U,
            .pullConfig      = PORT_INTERNAL_PULL_NOT_ENABLED,
            .driveSelect     = PORT_LOW_DRIVE_STRENGTH,
            .passiveFilter   = false,
            .mux             = PORT_MUX_ALT4,
            .pinLock         = false,
            .intConfig       = PORT_DMA_INT_DISABLED,   
            .clearIntFlag    = false,
            .gpioBase        = NULL,
            .digitalFilter   = false,
        },
        {
            .base            = PORTD,
            .pinPortIdx      = 2U,
            .pullConfig      = PORT_INTERNAL_PULL_NOT_ENABLED,
            .driveSelect     = PORT_LOW_DRIVE_STRENGTH,
            .passiveFilter   = false,
            .mux             = PORT_MUX_ALT4,
            .pinLock         = false,
            .intConfig       = PORT_DMA_INT_DISABLED,
            .clearIntFlag    = false,
            .gpioBase        = NULL,
            .digitalFilter   = false,
        }    
    };
    
    // ************************** Expected Calls **************************** //

    PINS_DRV_Init_ExpectAndReturn(TEST_I2S_PINS_COUNT, 
                                  test_pin_mux_init_cfg, 
                                  STATUS_SUCCESS);


    edma_chn_state_t test_dma_controller_ch0_state;
    edma_chn_state_t test_dma_controller_ch1_state;

    EDMA_DRV_ChannelInit_ExpectAndReturn(&test_dma_controller_ch0_state,
                                         &m_test_dma_controller_ch0_cfg,
                                         STATUS_SUCCESS);
    EDMA_DRV_ChannelInit_IgnoreArg_edmaChannelState();

    EDMA_DRV_ChannelInit_ExpectAndReturn(&test_dma_controller_ch1_state,
                                         &m_test_dma_controller_ch1_cfg,
                                         STATUS_SUCCESS);
    EDMA_DRV_ChannelInit_IgnoreArg_edmaChannelState();

    flexio_device_state_t test_i2s_device_init_cfg = {0U};

    FLEXIO_DRV_InitDevice_ExpectAndReturn(TEST_I2S_INSTANCE, 
                                          &test_i2s_device_init_cfg, 
                                          STATUS_SUCCESS);

    FLEXIO_I2S_DRV_MasterInit_StubWithCallback(test_flexio_drv_init_device_stub);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = bsp_i2s_init();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief Verifies bsp_i2s_init function when pin configuration function failed.
 * 
 * https://codebeamer.daiichi.com/item/193669
 *  
 */
void test_bsp_i2s_init__when_i2s_pin_cfg_fail__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //
    
    // ************************** Expected Calls **************************** //

    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = bsp_i2s_init();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies bsp_i2s_init function when the first EDMA init process 
 * function failed.
 * 
 * https://codebeamer.daiichi.com/item/193670
 *  
 */
void test_bsp_i2s_init__when_edma_init_fail__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //
    
    // ************************** Expected Calls **************************** //

    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    EDMA_DRV_ChannelInit_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = bsp_i2s_init();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies bsp_i2s_init function when the second EDMA init process 
 * function failed.
 * 
 * https://codebeamer.daiichi.com/item/193680
 *  
 */
void test_bsp_i2s_init__when_second_edma_init_fail__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //
    
    // ************************** Expected Calls **************************** //

    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    EDMA_DRV_ChannelInit_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    EDMA_DRV_ChannelInit_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = bsp_i2s_init();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies bsp_i2s_init function when the FLEXIO_DRV_InitDevice() 
 * function failed. 
 * 
 * https://codebeamer.daiichi.com/item/193686
 * 
 */
void test_bsp_i2s_init__when_flexio_init_fail__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //
    
    // ************************** Expected Calls **************************** //

    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    EDMA_DRV_ChannelInit_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    EDMA_DRV_ChannelInit_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    FLEXIO_DRV_InitDevice_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = bsp_i2s_init();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies bsp_i2s_init function when the i2s init configuration 
 * function failed.
 *  
 * https://codebeamer.daiichi.com/item/193687
 *  
 */
void test_bsp_i2s_init__when_i2s_master_init_fail__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    EDMA_DRV_ChannelInit_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    EDMA_DRV_ChannelInit_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    FLEXIO_DRV_InitDevice_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    FLEXIO_I2S_DRV_MasterInit_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = bsp_i2s_init();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}
/*******************************************************************************
 * Private Functions Definitions
 ******************************************************************************/

static void helper_bsp_i2s_init()
{
    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    EDMA_DRV_ChannelInit_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    EDMA_DRV_ChannelInit_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    FLEXIO_DRV_InitDevice_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    FLEXIO_I2S_DRV_MasterInit_StubWithCallback(test_flexio_drv_init_device_stub);

    (void)bsp_i2s_init();
}

static void helper_bsp_i2s_deinit()
{
    flexio_i2s_master_state_t i2s_master_state = {0U};

    FLEXIO_I2S_DRV_MasterDeinit_ExpectAndReturn(&i2s_master_state, 
                                                STATUS_SUCCESS);
    FLEXIO_DRV_DeinitDevice_ExpectAndReturn(TEST_I2S_INSTANCE, STATUS_SUCCESS);
    EDMA_DRV_ReleaseChannel_ExpectAndReturn(TEST_EDMA_CH_1, STATUS_SUCCESS);
    EDMA_DRV_ReleaseChannel_ExpectAndReturn(TEST_EDMA_CH_2, STATUS_SUCCESS);

    (void)bsp_i2s_deinit();
}

static void test_callback(bsp_i2s_status_t status)
{
    m_test_status = status;
}

static status_t test_flexio_drv_init_device_stub(
    uint32_t instance, 
    const flexio_i2s_master_user_config_t* userConfigPtr, 
    flexio_i2s_master_state_t* master,
    int cmock_num_calls)
{   
    (void)cmock_num_calls;
    (void)master;

    const flexio_i2s_master_user_config_t test_i2s_master_cfg =
    {
        .baudRate = 3072000,
        .bitsWidth = 32U,
        .callback = NULL,
        .callbackParam = NULL,
        .driverType = FLEXIO_DRIVER_TYPE_DMA,
        .rxDMAChannel = TEST_EDMA_CH_2,
        .txDMAChannel = TEST_EDMA_CH_1,
        .rxPin = 5U,
        .sckPin = 1U,
        .txPin = 0U,
        .wsPin = 4U,
    };

    TEST_ASSERT_EQUAL(TEST_I2S_INSTANCE, instance);
    TEST_ASSERT_EQUAL(test_i2s_master_cfg.baudRate, userConfigPtr->baudRate);
    TEST_ASSERT_EQUAL(test_i2s_master_cfg.bitsWidth, userConfigPtr->bitsWidth);
    TEST_ASSERT_EQUAL(test_i2s_master_cfg.callbackParam, userConfigPtr->callbackParam);
    TEST_ASSERT_EQUAL(test_i2s_master_cfg.driverType, userConfigPtr->driverType);
    TEST_ASSERT_EQUAL(test_i2s_master_cfg.rxDMAChannel, userConfigPtr->rxDMAChannel);
    TEST_ASSERT_EQUAL(test_i2s_master_cfg.txDMAChannel, userConfigPtr->txDMAChannel);
    TEST_ASSERT_EQUAL(test_i2s_master_cfg.rxPin, userConfigPtr->rxPin);
    TEST_ASSERT_EQUAL(test_i2s_master_cfg.sckPin, userConfigPtr->sckPin);
    TEST_ASSERT_EQUAL(test_i2s_master_cfg.txPin, userConfigPtr->txPin);
    TEST_ASSERT_EQUAL(test_i2s_master_cfg.wsPin, userConfigPtr->wsPin);

    m_test_i2s_event_cb = userConfigPtr->callback;

    return STATUS_SUCCESS;
}

// end of file
