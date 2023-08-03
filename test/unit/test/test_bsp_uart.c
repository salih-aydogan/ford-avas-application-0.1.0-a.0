/**
 * @file test_bsp_uart.c
 * @author Muhammed Salih AYDOGAN
 * @brief These tests verify the units of the UART module.
 * 
 * @version 0.1
 * 
 * @date 09/01/2023
 * 
 * @copyright Copyright (c) 2023
 * 
 * https://codebeamer.daiichi.com/issue/182870
 * 
 */

/*******************************************************************************
 * Includes 
 ******************************************************************************/

//First - include test framework headers
#include "unity.h"
#include "support.h"

// Second - include the module to be tested
#include "bsp_uart.h"

// Third - include the mocks of required modules
#include "mock_lpuart_driver.h"
#include "mock_pins_driver.h"
#include "mock_edma_driver.h"

#include "mock_log.h"

/*******************************************************************************
 * Test Files
 ******************************************************************************/

TEST_FILE("bsp_uart.c");

/*******************************************************************************             
 * Defines
 ******************************************************************************/

/**
 * @brief This macro specifies eDMA channel-0 number.
 * 
 */
#define TEST_EDMA_CH_0                   (0U)

/**
 * @brief This macro specifies UART tx pin number.
 * 
 */
#define TEST_UART_TX_PIN_NO              (3U)

/**
 * @brief This macro specifies UART pin count.
 * 
 */
#define TEST_UART_PIN_COUNT              (1U)

/**
 * @brief This macro specifies which uart unit to use.
 * 
 */
#define TEST_UART_INSTANCE               (0U)

/*******************************************************************************
 * Private Variables
 ******************************************************************************/
/**
 * @brief Runtime state variable of the LPUART driver.
 * 
 */
static lpuart_state_t m_test_lpuart_state;

/**
 * @brief Enumeration specifying errors that occur during transmission.
 * 
 */
static bsp_uart_err_t m_test_uart_err = BSP_UART_ERR_NONE;

/**
 * @brief Runtime state variable of the LPUART driver.
 */
static lpuart_state_t * m_test_p_lpuart_state = &m_test_lpuart_state;

/**
 * @brief The input data to be sent with uart.
 */
static uint8_t m_test_buff[1U] = { 'T' };

/*******************************************************************************
 * Global Variables
 ******************************************************************************/
/**
 * @brief This variable is used to change the Peripheral PORTC base address.
 * 
 */
uint8_t allocated_area[PERIPH_MEM_SIZE];

/*******************************************************************************
 * Private Functions Prototype Declarations
 ******************************************************************************/

/**
 * @brief This function is an callback function that is used in UART 
 * transmission error.
 * 
 */
static void uart_set_cb(bsp_uart_err_t m_test_uart_err);

/**
 * @brief This helper function initializes the uart init function successfully.
 * 
 */
static void helper_bsp_uart_init(void);

/**
 * @brief This variable is to get the callback function address.
 * 
 */
static uart_callback_t m_uart_write_cb = NULL;

/**
 * @brief This stub function is to get the callback function address. 
 * 
 */
static uart_callback_t uart_install_stub(uint32_t instance, 
                                         uart_callback_t function, 
                                         void* callbackParam, 
                                         int cmock_num_calls);

/**
 * @brief This callback function for edma user configuration parameters check. 
 * 
 */
static status_t test_edma_init_cb(edma_chn_state_t* edmaChannelState, 
                                  const edma_channel_config_t* edmaChannelConfig, 
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
 * @brief The test verifies the bsp_uart_init() function by calling the 
 * PINS_DRV_Init, EDMA_DRV_ChannelInit, and LPUART_DRV_Init functions with a 
 * successful call.
 * 
 * https://codebeamer.daiichi.com/issue/182876
 *  
 */
void test_bsp_uart_init__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    pin_settings_config_t const gpio_cfg_tbl[TEST_UART_PIN_COUNT] = {
    
        {
            .base = PORTC,
            .pinPortIdx = TEST_UART_TX_PIN_NO,
            .pullConfig = PORT_INTERNAL_PULL_NOT_ENABLED,
            .driveSelect = PORT_LOW_DRIVE_STRENGTH,
            .passiveFilter = false,
            .mux = PORT_MUX_ALT2,
            .pinLock = false,
            .intConfig = PORT_DMA_INT_DISABLED,
            .clearIntFlag = false,
            .gpioBase = NULL,
            .digitalFilter = false
        },
    }; 
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    PINS_DRV_Init_ExpectWithArrayAndReturn((uint32_t)TEST_UART_PIN_COUNT,  
                                            gpio_cfg_tbl,
                                            1U,
                                            STATUS_SUCCESS);

    EDMA_DRV_ChannelInit_StubWithCallback(test_edma_init_cb);

    lpuart_user_config_t m_test_lpuart_user_cfg = {0U};
    m_test_lpuart_user_cfg.transferType = LPUART_USING_DMA;
    m_test_lpuart_user_cfg.baudRate = BSP_UART_CFG_BAUDRATE;
    m_test_lpuart_user_cfg.parityMode = LPUART_PARITY_DISABLED;
    m_test_lpuart_user_cfg.stopBitCount = LPUART_ONE_STOP_BIT;
    m_test_lpuart_user_cfg.bitCountPerChar = LPUART_8_BITS_PER_CHAR;
    m_test_lpuart_user_cfg.txDMAChannel = TEST_EDMA_CH_0;

    LPUART_DRV_Init_ExpectAndReturn((uint32_t)TEST_UART_INSTANCE,
                                            &m_test_lpuart_state,
                                            &m_test_lpuart_user_cfg,
                                            STATUS_SUCCESS);

    LPUART_DRV_Init_IgnoreArg_lpuartStatePtr();

    LPUART_DRV_InstallTxCallback_ExpectAndReturn((uint32_t)TEST_UART_INSTANCE, 
                                                NULL, 
                                                NULL, 
                                                STATUS_SUCCESS);

    LPUART_DRV_InstallTxCallback_IgnoreArg_function();                                           

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_uart_init();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief The test verifies the bsp_uart_init() function by calling the 
 * PINS_DRV_Init function with a failed call.
 * 
 * https://codebeamer.daiichi.com/issue/182878
 *  
 */
void test_bsp_uart_init__pin_return_status__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_uart_init();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the bsp_uart_init() function by calling the 
 * PINS_DRV_Init function with a successful call but the EDMA_DRV_ChannelInit 
 * function with a failed call
 * 
 * https://codebeamer.daiichi.com/issue/182877
 *  
 */
void test_bsp_uart_init__edma_return_status__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    EDMA_DRV_ChannelInit_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_uart_init();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the bsp_uart_init() function by calling the 
 * PINS_DRV_Init, and EDMA_DRV_ChannelInit functions with a successful call but 
 * the LPUART_DRV_Init function with a failed call.
 * 
 * https://codebeamer.daiichi.com/issue/182879
 *  
 */
void test_bsp_uart_init__lpuart_return_status__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    EDMA_DRV_ChannelInit_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    LPUART_DRV_Init_ExpectAnyArgsAndReturn(STATUS_ERROR);

    LPUART_DRV_InstallTxCallback_ExpectAnyArgsAndReturn(
        (uart_callback_t)STATUS_SUCCESS);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_uart_init();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the bsp_deuart_init() function by calling the 
 * EDMA_DRV_ReleaseChannel, and LPUART_DRV_Deinit functions with a successful 
 * call.
 * 
 * https://codebeamer.daiichi.com/issue/182924
 *  
 */
void test_bsp_uart_deinit__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_bsp_uart_init();

    // ************************** Expected Calls **************************** //

    EDMA_DRV_ReleaseChannel_ExpectAndReturn(TEST_EDMA_CH_0, STATUS_SUCCESS);

    LPUART_DRV_Deinit_ExpectAndReturn(TEST_UART_INSTANCE, STATUS_SUCCESS);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_uart_deinit();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief The test verifies the bsp_uart_deinit() function reset the callback 
 * variable by calling the EDMA_DRV_ReleaseChannel, and LPUART_DRV_Deinit 
 * functions with a successful call.
 * 
 * https://codebeamer.daiichi.com/item/186459
 *  
 */
void test_bsp_uart_deinit__usual_run__callback_assignment_removed(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_bsp_uart_init();
    bsp_uart_cb_set(uart_set_cb);

    // ************************** Expected Calls **************************** //

    EDMA_DRV_ReleaseChannel_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    LPUART_DRV_Deinit_ExpectAndReturn(TEST_UART_INSTANCE, STATUS_SUCCESS);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_uart_deinit();

    TEST_STEP(2);
    m_uart_write_cb(m_test_p_lpuart_state, UART_EVENT_END_TRANSFER, NULL);

    TEST_STEP(4);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief The test verifies the bsp_deuart_init() function by calling the 
 * EDMA_DRV_ReleaseChannel function with a failed call.
 * 
 * https://codebeamer.daiichi.com/issue/182925
 *  
 */
void test_bsp_uart_deinit__edma_return_status__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_bsp_uart_init();

    // ************************** Expected Calls **************************** //

    EDMA_DRV_ReleaseChannel_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_uart_deinit();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the bsp_deuart_init() function by calling the 
 * EDMA_DRV_ReleaseChannel function with a successful call and the 
 * LPUART_DRV_Deinit function with a failed call.
 * 
 * https://codebeamer.daiichi.com/issue/182926
 *  
 */
void test_bsp_uart_deinit__lpuart_return_status__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_bsp_uart_init();

    // ************************** Expected Calls **************************** //

    EDMA_DRV_ReleaseChannel_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    LPUART_DRV_Deinit_ExpectAndReturn(TEST_UART_INSTANCE, STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_uart_deinit();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the bsp_uart_async_write() function by calling the 
 * LPUART_DRV_GetTransmitStatus, and LPUART_DRV_SendData functions with a 
 * successful call.
 * 
 * https://codebeamer.daiichi.com/issue/182945
 *  
 */
void test_bsp_uart_async_write__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_bsp_uart_init();

    // ************************** Expected Calls **************************** //
    
    LPUART_DRV_GetTransmitStatus_ExpectAndReturn((uint32_t)TEST_UART_INSTANCE,
                                                NULL, 
                                                STATUS_SUCCESS);

    LPUART_DRV_SendData_ExpectAndReturn((uint32_t)TEST_UART_INSTANCE,
                                        m_test_buff,
                                        (uint32_t)1U,
                                        STATUS_SUCCESS);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_uart_async_write(m_test_buff, 1);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief The test verifies the bsp_uart_async_write() function with a false 
 * parameter.
 * 
 * https://codebeamer.daiichi.com/issue/182946
 *  
 */
void test_bsp_uart_async_write__wrong_buffer_param__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    
    // **************************** Pre-Actions ***************************** //

    helper_bsp_uart_init();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_uart_async_write(NULL, 1U);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the bsp_uart_async_write() function with a false 
 * parameter.
 * 
 * https://codebeamer.daiichi.com/issue/182947
 *  
 */
void test_bsp_uart_async_write__wrong_size_param__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_bsp_uart_init();
    
    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_uart_async_write(m_test_buff, 0U);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the bsp_uart_async_write() function by calling the 
 * LPUART_DRV_GetTransmitStatus function with a failed call.
 * 
 * https://codebeamer.daiichi.com/issue/182948
 *  
 */
void test_bsp_uart_async_write__get_function_return_status__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_bsp_uart_init();

    // ************************** Expected Calls **************************** //

    LPUART_DRV_GetTransmitStatus_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_uart_async_write(m_test_buff, 1U);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the bsp_uart_async_write() function by calling the 
 * LPUART_DRV_GetTransmitStatus function with a successful call and 
 * LPUART_DRV_SendData function with a failed call.
 * 
 * https://codebeamer.daiichi.com/issue/182949
 *  
 */
void test_bsp_uart_async_write__send_function_return_status__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_bsp_uart_init();

    // ************************** Expected Calls **************************** //

    LPUART_DRV_GetTransmitStatus_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    LPUART_DRV_SendData_ExpectAnyArgsAndReturn(STATUS_ERROR);
    
    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_uart_async_write(m_test_buff, 1U);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the callback function that is used in UART transmit 
 * operations and the bsp_uart_async_write() function.
 * 
 * https://codebeamer.daiichi.com/issue/183225
 *  
 */
void test_bsp_uart_async_write__install_cb_event__dma_error(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_bsp_uart_init();

    bsp_uart_cb_set(uart_set_cb);

    // ************************** Expected Calls **************************** //

    LPUART_DRV_GetTransmitStatus_ExpectAnyArgsAndReturn(STATUS_ERROR);
    
    // **************************** Test Steps ****************************** //

    TEST_STEP(1);

    bool ret = bsp_uart_async_write(m_test_buff, 1U);

    TEST_STEP(2);
    
    TEST_ASSERT_EQUAL(false, ret);

    TEST_STEP(3);
    m_uart_write_cb(m_test_p_lpuart_state, UART_EVENT_ERROR, NULL);

    TEST_STEP(4);
    TEST_ASSERT_EQUAL(BSP_UART_ERR_DMA, m_test_uart_err);
}

/**
 * @brief The test verifies the callback function that is used in UART transmit 
 * operations and the bsp_uart_async_write() function.
 * 
 * https://codebeamer.daiichi.com/issue/183261
 *  
 */
void test_bsp_uart_async_write__install_cb_event__empty_error(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_bsp_uart_init();

    bsp_uart_cb_set(uart_set_cb);

    // ************************** Expected Calls **************************** //

    LPUART_DRV_GetTransmitStatus_ExpectAnyArgsAndReturn(STATUS_ERROR);
    
    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_uart_async_write(m_test_buff, 1U);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);

    TEST_STEP(3);
    m_uart_write_cb(m_test_p_lpuart_state, UART_EVENT_TX_EMPTY, NULL);

    TEST_STEP(4);
    TEST_ASSERT_EQUAL(BSP_UART_ERR_TX_EMPTY, m_test_uart_err);
}

/**
 * @brief The test verifies the callback function that is used in UART transmit 
 * operations and the bsp_uart_async_write() function.
 * 
 * https://codebeamer.daiichi.com/issue/183271
 *  
 */
void test_bsp_uart_async_write__install_cb_event__aborted_error(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_bsp_uart_init();

    bsp_uart_cb_set(uart_set_cb);

    m_test_p_lpuart_state->transmitStatus = STATUS_UART_ABORTED;

    // ************************** Expected Calls **************************** //

    LPUART_DRV_GetTransmitStatus_ExpectAnyArgsAndReturn(STATUS_UART_ABORTED);
    
    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_uart_async_write(m_test_buff, 1U);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);

    TEST_STEP(3);
    m_uart_write_cb(m_test_p_lpuart_state, UART_EVENT_END_TRANSFER, NULL);

    TEST_STEP(4);
    TEST_ASSERT_EQUAL(BSP_UART_ERR_ABORTED, m_test_uart_err);

    TEST_STEP(5);
    m_test_p_lpuart_state->transmitStatus = STATUS_UART_TX_UNDERRUN;
    m_uart_write_cb(m_test_p_lpuart_state, UART_EVENT_END_TRANSFER, NULL);

    TEST_STEP(6);
    TEST_ASSERT_EQUAL(BSP_UART_ERR_NONE, m_test_uart_err);
}

/**
 * @brief The test verifies the callback function that is used in UART transmit 
 * operations and the bsp_uart_async_write() function.
 * 
 * https://codebeamer.daiichi.com/issue/183276
 *  
 */
void test_bsp_uart_async_write__install_cb_event__none_error(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_bsp_uart_init();

    bsp_uart_cb_set(uart_set_cb);

    // ************************** Expected Calls **************************** //

    LPUART_DRV_GetTransmitStatus_ExpectAnyArgsAndReturn(STATUS_ERROR);
    
    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_uart_async_write(m_test_buff, 1U);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);

    TEST_STEP(3);
    m_uart_write_cb(m_test_p_lpuart_state, UART_EVENT_RX_FULL, NULL);

    TEST_STEP(4);
    TEST_ASSERT_EQUAL(BSP_UART_ERR_NONE, m_test_uart_err);
}

/**
 * @brief The test verifies the bsp_uart_cb_set() function call with the correct
 *  parameter.
 * 
 * https://codebeamer.daiichi.com/issue/183085
 *  
 */
void test_bsp_uart_cb_set__successful_set__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_bsp_uart_init();
    
    // ************************** Expected Calls **************************** //
    
    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_uart_cb_set(uart_set_cb);
}

/**
 * @brief The test verifies the bsp_uart_cb_set() function call with an 
 * incorrect parameter.
 * 
 * https://codebeamer.daiichi.com/issue/182953
 *  
 */
void test_bsp_uart_cb_set__wrong_cb_param__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //
    
    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_uart_cb_set(NULL);
}

/*******************************************************************************
 * Private Functions Definitions
 ******************************************************************************/

static void helper_bsp_uart_init(){
    
    // ************************** Expected Calls **************************** //

    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    EDMA_DRV_ChannelInit_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    LPUART_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    LPUART_DRV_InstallTxCallback_IgnoreAndReturn(
        (uart_callback_t)STATUS_SUCCESS);
    
    LPUART_DRV_InstallTxCallback_StubWithCallback(uart_install_stub);

    // ********************************************************************** //
    
    (void)bsp_uart_init();
};

static uart_callback_t uart_install_stub(uint32_t instance, 
                                         uart_callback_t function, 
                                         void* callbackParam, 
                                         int cmock_num_calls)
{
    (void) instance;
    (void) callbackParam;
    (void) cmock_num_calls;

    m_uart_write_cb = function;

    return (uart_callback_t)STATUS_SUCCESS;
}

static void uart_set_cb(bsp_uart_err_t error)
{
    m_test_uart_err = error;
}

static status_t test_edma_init_cb(edma_chn_state_t* edmaChannelState, 
                                  const edma_channel_config_t* edmaChannelConfig, 
                                  int cmock_num_calls)
{ 
    (void)cmock_num_calls;

    TEST_ASSERT_NOT_NULL(edmaChannelState);

    const edma_channel_config_t edma_ch_cfg = {

        .channelPriority = EDMA_CHN_PRIORITY_0,
        .virtChnConfig = TEST_EDMA_CH_0,
        .source = EDMA_REQ_LPUART0_TX,
        .callback = NULL,
        // PRQA S 2983 ++ # Suppressed, It is used within the nxp library. False positive warning. 
        .enableTrigger = false, 
        // PRQA S 2983 --
        .callbackParam = NULL,
    };
    
    TEST_ASSERT_EQUAL(edma_ch_cfg.channelPriority, edmaChannelConfig->channelPriority);
    TEST_ASSERT_EQUAL(edma_ch_cfg.virtChnConfig, edmaChannelConfig->virtChnConfig);
    TEST_ASSERT_EQUAL(edma_ch_cfg.source, edmaChannelConfig->source);
    TEST_ASSERT_EQUAL(edma_ch_cfg.callback, edmaChannelConfig->callback);
    TEST_ASSERT_EQUAL(edma_ch_cfg.enableTrigger, edmaChannelConfig->enableTrigger);
    TEST_ASSERT_EQUAL(edma_ch_cfg.callbackParam, edmaChannelConfig->callbackParam);

    return STATUS_SUCCESS;
}

// end of file