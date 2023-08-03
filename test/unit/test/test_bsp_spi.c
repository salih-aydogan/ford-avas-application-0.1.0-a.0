/**
 * @file test_bsp_spi.c
 * @author Muhammed Salih AYDOGAN
 * @brief These tests verify the units of the SPI module.
 * 
 * @version 0.1
 * 
 * @date 23/02/2023
 * 
 * @copyright Copyright (c) 2023
 * 
 * https://codebeamer.daiichi.com/issue/184355
 * 
 */

/*******************************************************************************
 * Includes 
 ******************************************************************************/

//First - include test framework headers
#include "unity.h"
#include "support.h"

// Second - include the module to be tested
#include "bsp_spi.h"

// Third - include the mocks of required modules
#include "mock_lpspi_master_driver.h"
#include "mock_pins_driver.h"

#include "mock_log.h"

/*******************************************************************************
 * Test Files
 ******************************************************************************/

TEST_FILE("bsp_spi.c");

/*******************************************************************************             
 * Defines
 ******************************************************************************/

/**
 * @brief This macro specifies the timeout values used in blocking transmission.
 * 
 * https://codebeamer.daiichi.com/issue/183175
 * 
 */
#define TEST_BLOCKING_TIMEOUT_MS          (100U)

/**
 * @brief This macro specifies which LPSPI unit to use.
 * 
 * https://codebeamer.daiichi.com/issue/183180
 * 
 */
#define TEST_LPSPI_INSTANCE                (0U)

/**
 * @brief This macro specifies SPI pin count.
 * 
 * https://codebeamer.daiichi.com/issue/183181
 * 
 */
#define TEST_PIN_COUNT                     (3U)

/*******************************************************************************
 * Extern Variables
 ******************************************************************************/

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/**
 * @brief This variable create from runtime state structure of LPSPI driver.
 * 
 * https://codebeamer.daiichi.com/issue/183184
 * 
 */
static lpspi_state_t m_test_lpspi_state;

/**
 * @brief This data structure containing information about a device on the SPI 
 * bus.
 * 
 */
static const lpspi_master_config_t m_test_lpspi_cfg = {

    .bitsPerSec = 1000000UL,
    .whichPcs = LPSPI_PCS0,
    .pcsPolarity = LPSPI_ACTIVE_LOW,
    .isPcsContinuous = true,
    .bitcount = 8U,
    .lpspiSrcClk = 8000000UL,
    .clkPhase = LPSPI_CLOCK_PHASE_1ST_EDGE,
    .clkPolarity = LPSPI_SCK_ACTIVE_HIGH,
    .lsbFirst = false,
    .transferType = LPSPI_USING_INTERRUPTS,
    .rxDMAChannel = 0U,
    .txDMAChannel = 0U,
    .callback = NULL,
    .callbackParam = NULL
};

/**
 * @brief This data array is used for the argument in read and write operations.
 * 
 */
static uint8_t m_test_array[] = { 1U };

/**
 * @brief This variable is used for the argument in read and write operations.
 * 
 */
static size_t m_test_size = 1U;

/**
 * @brief This variable is used in transmit error callback.
 * 
 */
//static status_t m_test_transmit_status;

/**
 * @brief This variable is to change the state of the lpspi driver.
 * 
 */
static lpspi_state_t* m_test_driver_state;

/*******************************************************************************
 * Global Variables
 ******************************************************************************/
/**
 * @brief This variable is used to change the Peripheral PTE base address.
 * 
 */
uint8_t allocated_area[PERIPH_MEM_SIZE];

/*******************************************************************************
 * Private Functions Prototype Declarations
 ******************************************************************************/
/**
 * @brief This function pointer used for get flash callback.
 * 
 */
static spi_callback_t m_test_flash_cb = NULL;

/**
 * @brief Helper of spi success init without stub.
 *  
 */
static void helper_bsp_spi_init(void);

/**
 * @brief Helper of spi success init with stub function.
 *  
 */
static void helper_bsp_spi_init_with_stub(void);

/**
 * @brief Stub function for lpspi driver fonction.
 *  
 */
static status_t lpspi_drv_master_init_stub(
    uint32_t instance, 
    lpspi_state_t* lpspiState, 
    const lpspi_master_config_t* spiConfig, 
    int cmock_num_calls);

/**
 * @brief This callback function tests for errors that occur during the receive 
 * and send operation.
 * 
 */
static void spi_flash_cb(bsp_spi_err_t spi_err);

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
 * @brief The test verifies the bsp_spi_init() function by calling the 
 * PINS_DRV_Init, FLEXIO_DRV_InitDevice, FLEXIO_SPI_DRV_MasterInit, 
 * LPSPI_DRV_MasterInit functions with a successful call.
 * 
 * https://codebeamer.daiichi.com/issue/184368
 *  
 */
void test_bsp_spi_init__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //


    const pin_settings_config_t m_test_gpio_user_cfg_tbl[TEST_PIN_COUNT] = {

        {
            //PRQA S 306 ++ # Defined by nxp library to hold register base address. False positive warning.
            .base = PORTB, // MOSI
            .pinPortIdx = 4U,
            .pullConfig =PORT_INTERNAL_PULL_NOT_ENABLED,
            .driveSelect = PORT_LOW_DRIVE_STRENGTH,
            .passiveFilter = false,
            .mux = PORT_MUX_ALT3,
            .pinLock = false,
            .intConfig = PORT_DMA_INT_DISABLED,
            .clearIntFlag = false,
            .digitalFilter = false,
            .gpioBase = NULL,
        },
        {
            .base = PORTD, // SCK
            .pinPortIdx = 15U,
            .pullConfig =PORT_INTERNAL_PULL_NOT_ENABLED,
            .driveSelect = PORT_LOW_DRIVE_STRENGTH,
            .passiveFilter = false,
            .mux = PORT_MUX_ALT4,
            .pinLock = false,
            .intConfig = PORT_DMA_INT_DISABLED,
            .clearIntFlag = false,
            .digitalFilter = false,
            .gpioBase = NULL,
        },
        { // MISO
            .base = PORTD,
            .pinPortIdx = 16U,
            .pullConfig =PORT_INTERNAL_PULL_NOT_ENABLED,
            .driveSelect = PORT_LOW_DRIVE_STRENGTH,
            .passiveFilter = false,
            .mux = PORT_MUX_ALT4,
            .pinLock = false,
            .intConfig = PORT_DMA_INT_DISABLED,
            .clearIntFlag = false,
            .digitalFilter = false,
            .gpioBase = NULL,
        },
            //PRQA S 306 --
    };    

    PINS_DRV_Init_ExpectWithArrayAndReturn(TEST_PIN_COUNT, 
                                           m_test_gpio_user_cfg_tbl,
                                           TEST_PIN_COUNT, 
                                           STATUS_SUCCESS);


    LPSPI_DRV_MasterInit_AddCallback(lpspi_drv_master_init_stub);
    LPSPI_DRV_MasterInit_ExpectWithArrayAndReturn(
        (uint32_t)TEST_LPSPI_INSTANCE,
        &m_test_lpspi_state,
        1U,
        &m_test_lpspi_cfg,
        1U,
        STATUS_SUCCESS);
    LPSPI_DRV_MasterInit_IgnoreArg_spiConfig();

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_spi_init();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief The test verifies the bsp_spi_init() function by calling the 
 * PINS_DRV_Init function with a failed call.
 * 
 * https://codebeamer.daiichi.com/issue/184370
 *  
 */
void test_bsp_spi_init__gpio_initialization_error__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //
    
    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_spi_init();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the bsp_spi_init() function by calling the 
 * PINS_DRV_Init function with a successful call and the LPSPI_DRV_MasterInit 
 * function with a failed call.
 * 
 * https://codebeamer.daiichi.com/issue/184373
 *  
 */
void test_bsp_spi_init__lpspi_initialization_error__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //
    
    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    LPSPI_DRV_MasterInit_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_spi_init();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the bsp_spi_deinit() function by calling the 
 * LPSPI_DRV_MasterDeinit function with a successful call.
 * 
 * https://codebeamer.daiichi.com/issue/184379
 *  
 */
void test_bsp_spi_deinit__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //
    
    helper_bsp_spi_init();

    // ************************** Expected Calls **************************** //
    
    LPSPI_DRV_MasterDeinit_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_spi_deinit();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief The test validates bsp_spi_deinit() against the initialization status 
 * of bsp_spi_init().
 * 
 * https://codebeamer.daiichi.com/issue/184382
 *  
 */
void test_bsp_spi_deinit__without_init__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_ERROR);
    (void)bsp_spi_init();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_spi_deinit();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the bsp_spi_deinit() function by calling the 
 * LPSPI_DRV_MasterDeinit function with a failed call.
 * 
 * https://codebeamer.daiichi.com/item/184384
 *  
 */
void test_bsp_spi_deinit__lpspi_deinit_status__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_bsp_spi_init();

    // ************************** Expected Calls **************************** //

    LPSPI_DRV_MasterDeinit_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_spi_deinit();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies that the bsp_spi_deinit() function assigns a 
 * NULL value to the callback variable by calling the LPSPI_DRV_MasterDeinit 
 * function with a successful call.
 * 
 * https://codebeamer.daiichi.com/item/186137
 *  
 */
void test_bsp_spi_deinit__usual_run__callback_assignment_removed(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //
    
    helper_bsp_spi_init_with_stub();
    (void)bsp_spi_cb_set(&spi_flash_cb);

    // ************************** Expected Calls **************************** //
    
    LPSPI_DRV_MasterDeinit_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_spi_deinit();

    TEST_STEP(2);
    m_test_driver_state->status = LPSPI_TRANSMIT_FAIL;
    m_test_flash_cb(m_test_driver_state, SPI_EVENT_END_TRANSFER, NULL);
    m_test_driver_state->status = LPSPI_TRANSFER_OK;

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief The test verifies the bsp_spi_read() function by calling the 
 * LPSPI_DRV_MasterTransferBlocking functions with a successful call.
 * 
 * https://codebeamer.daiichi.com/issue/184388
 *  
 */
void test_bsp_spi_read__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    LPSPI_DRV_MasterTransferBlocking_ExpectAndReturn(
        (uint32_t)TEST_LPSPI_INSTANCE,
        NULL,
        m_test_array,
        (uint16_t)m_test_size,
        TEST_BLOCKING_TIMEOUT_MS,
        STATUS_SUCCESS);

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_spi_read(m_test_array, m_test_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief The test verifies the bsp_spi_read() function with a wrong size 
 * parameter.

 * https://codebeamer.daiichi.com/issue/184395
 *  
 */
void test_bsp_spi_read__wrong_size_param__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_spi_read(m_test_array, 0U);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the bsp_spi_read() function with a wrong data 
 * parameter.
 * 
 * https://codebeamer.daiichi.com/issue/184396
 *  
 */
void test_bsp_spi_read__wrong_data_param__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_spi_read(NULL, m_test_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the bsp_spi_read() function by calling the 
 * LPSPI_DRV_MasterTransferBlocking functions with a failed call.
 * 
 * https://codebeamer.daiichi.com/issue/184397
 *  
 */
void test_bsp_spi_read__lspi_read_status__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //
    helper_bsp_spi_init_with_stub();

    (void)bsp_spi_cb_set(&spi_flash_cb);

    m_test_driver_state->status = LPSPI_RECEIVE_FAIL;
    // ************************** Expected Calls **************************** //

    LPSPI_DRV_MasterTransferBlocking_IgnoreAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_spi_read(m_test_array, m_test_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);

    m_test_driver_state->status = LPSPI_TRANSFER_OK;
}

/**
 * @brief This test verifies that the bsp_spi_read function fails and the 
 * callback function is called when called with the 
 * LPSPI_DRV_MasterTransferBlocking function with a failed call.
 * 
 * https://codebeamer.daiichi.com/item/184659
 *  
 */
void test_bsp_spi_read__when_receive_fail__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //
    
    helper_bsp_spi_init_with_stub();

    (void)bsp_spi_cb_set(&spi_flash_cb);

    // ************************** Expected Calls **************************** //
    
    LPSPI_DRV_MasterTransferBlocking_IgnoreAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_spi_read(m_test_array, m_test_size);

    TEST_STEP(2);
    m_test_driver_state->status = LPSPI_RECEIVE_FAIL;
    m_test_flash_cb(m_test_driver_state, SPI_EVENT_END_TRANSFER, NULL);
    m_test_driver_state->status = LPSPI_TRANSFER_OK;

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief This test verifies that the bsp_spi_write function successfully 
 * executes.
 * 
 * https://codebeamer.daiichi.com/issue/184408
 *  
 */
void test_bsp_spi_write__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //
    LPSPI_DRV_MasterTransferBlocking_ExpectAndReturn(            
        (uint32_t)TEST_LPSPI_INSTANCE,
        m_test_array,
        NULL,
        (uint16_t)m_test_size,
        TEST_BLOCKING_TIMEOUT_MS,
        STATUS_SUCCESS);

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_spi_write(m_test_array, m_test_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief This test verifies that the bsp_spi_async_write function successfully 
 * executes.
 * 
 * https://codebeamer.daiichi.com/issue/198432
 *  
 */
void test_bsp_spi_async_write__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    const uint8_t p_data = 10U;

    uint8_t p_data_size = 2U;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    LPSPI_DRV_MasterGetTransferStatus_ExpectAndReturn(TEST_LPSPI_INSTANCE,
                                                      NULL,
                                                      STATUS_SUCCESS);

    LPSPI_DRV_MasterTransfer_ExpectAndReturn(TEST_LPSPI_INSTANCE,
                                             &p_data,
                                             NULL,
                                             (uint16_t)p_data_size,
                                             STATUS_SUCCESS);

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_spi_async_write(&p_data, p_data_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief This test verifies that the bsp_spi_is_busy function when the status
 * success.
 * 
 * https://codebeamer.daiichi.com/item/198436
 *  
 */
void test_bsp_spi_is_busy__status__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    LPSPI_DRV_MasterGetTransferStatus_ExpectAndReturn(TEST_LPSPI_INSTANCE,
                                                      NULL,
                                                      STATUS_SUCCESS);

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_spi_is_busy();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief This test verifies that the bsp_spi_is_busy function when status busy.
 * 
 * https://codebeamer.daiichi.com/issue/198437
 *  
 */
void test_bsp_spi_is_busy__status__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    LPSPI_DRV_MasterGetTransferStatus_ExpectAndReturn(TEST_LPSPI_INSTANCE,
                                                      NULL,
                                                      STATUS_BUSY);

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_spi_is_busy();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief This test verifies that the bsp_spi_async_write function when transfer 
 * status fail.
 * 
 * https://codebeamer.daiichi.com/item/198444
 *  
 */
void test_bsp_spi_async_write__transfer_status__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    const uint8_t p_data = 10U;

    uint8_t p_data_size = 2U;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    LPSPI_DRV_MasterGetTransferStatus_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_spi_async_write(&p_data, p_data_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief This test verifies that the bsp_spi_async_write function when transfer 
 * error.
 * 
 * https://codebeamer.daiichi.com/item/198446
 *  
 */
void test_bsp_spi_async_write__transfer_err__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    const uint8_t p_data = 10U;

    uint8_t p_data_size = 2U;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    LPSPI_DRV_MasterGetTransferStatus_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    LPSPI_DRV_MasterTransfer_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_spi_async_write(&p_data, p_data_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief This test verifies that the bsp_spi_async_write function when called 
 * with invalid data parameter.
 * 
 * https://codebeamer.daiichi.com/item/198447
 *  
 */
void test_bsp_spi_async_write__invalid_data__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint8_t p_data_size = 2U;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_spi_async_write(NULL, p_data_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief This test verifies that the bsp_spi_async_write function when called 
 * with invalid size parameter.
 * 
 * https://codebeamer.daiichi.com/item/198448
 *  
 */
void test_bsp_spi_async_write__invalid_size__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    const uint8_t data = 10U;

    uint8_t data_size = 0U;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_spi_async_write(&data, data_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief This test verifies that the bsp_spi_write function failed executes 
 * when the LPSPI_DRV_MasterTransferBlocking function failed call.
 * 
 * https://codebeamer.daiichi.com/issue/184412
 *  
 */
void test_bsp_spi_write__lspi_state__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_bsp_spi_init_with_stub();

    (void)bsp_spi_cb_set(&spi_flash_cb);

    m_test_driver_state->status = LPSPI_TRANSMIT_FAIL;
    
    // ************************** Expected Calls **************************** //

    LPSPI_DRV_MasterTransferBlocking_IgnoreAndReturn(STATUS_ERROR);
    
    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_spi_write(m_test_array, 
                             m_test_size);
    
    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);

    m_test_driver_state->status = LPSPI_TRANSFER_OK;
}

/**
 * @brief This test verifies that the bsp_spi_write function fails and the 
 * callback function is called when called with the 
 * LPSPI_DRV_MasterTransferBlocking function with a failed call.
 * 
 * https://codebeamer.daiichi.com/issue/184662
 *  
 */
void test_bsp_spi_write__when_transmit_fail__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //
    
    helper_bsp_spi_init_with_stub();

    (void)bsp_spi_cb_set(&spi_flash_cb);

    // ************************** Expected Calls **************************** //
    
    LPSPI_DRV_MasterTransferBlocking_IgnoreAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_spi_write(m_test_array, 
                             m_test_size);

    TEST_STEP(2);
    m_test_driver_state->status = LPSPI_TRANSMIT_FAIL;
    m_test_flash_cb(m_test_driver_state, SPI_EVENT_END_TRANSFER, NULL);
    m_test_driver_state->status = LPSPI_TRANSFER_OK;

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief This test verifies that the function will fail if zero is entered in 
 * the size argument.
 * 
 * https://codebeamer.daiichi.com/issue/184421
 *  
 */
void test_bsp_spi_write__invalid_size_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_spi_write(m_test_array, 0U);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief This test verifies that the function will fail if zero is entered in 
 * the data argument.
 * 
 * https://codebeamer.daiichi.com/issue/184422
 *  
 */
void test_bsp_spi_write__invalid_data_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_spi_write(NULL, m_test_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief This test verifies that the bsp_spi_async_read function 
 * successfully executes when called with LPSPI_DRV_MasterGetTransferStatus 
 * and LPSPI_DRV_MasterTransfer functions successfully.
 * 
 * https://codebeamer.daiichi.com/issue/184533
 *  
 */
void test_bsp_spi_async_read__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    LPSPI_DRV_MasterGetTransferStatus_ExpectAndReturn(TEST_LPSPI_INSTANCE,
                                                      NULL,
                                                      STATUS_SUCCESS);
    LPSPI_DRV_MasterTransfer_ExpectAndReturn(TEST_LPSPI_INSTANCE,
                                             NULL,
                                             m_test_array,
                                             (uint16_t)m_test_size,
                                             STATUS_SUCCESS);

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_spi_async_read(m_test_array, m_test_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief This test verifies that the bsp_spi_async_read function failed 
 * executes when called with the LPSPI_DRV_MasterGetTransferStatus function 
 * failed.
 * 
 * https://codebeamer.daiichi.com/issue/184534
 *  
 */
void test_bsp_spi_async_read__transfer_status_error__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    LPSPI_DRV_MasterGetTransferStatus_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_spi_async_read(m_test_array, m_test_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief This test verifies that the bsp_spi_async_read function failed 
 * executes when called with the LPSPI_DRV_MasterTransfer function failed.
 * 
 * https://codebeamer.daiichi.com/issue/184535
 *  
 */
void test_bsp_spi_async_read__flash_reading_error__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    LPSPI_DRV_MasterGetTransferStatus_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    LPSPI_DRV_MasterTransfer_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_spi_async_read(m_test_array, m_test_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief This test verifies that the bsp_spi_async_read function failed 
 * executes when called with an invalid buffer address argument.
 * 
 * https://codebeamer.daiichi.com/item/184538
 *  
 */
void test_bsp_spi_async_read__invalid_buffer_address__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_spi_async_read(NULL, m_test_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief This test verifies that the bsp_spi_async_read function failed 
 * executes when called with an invalid size argument.
 * 
 * https://codebeamer.daiichi.com/issue/184539
 *  
 */
void test_bsp_spi_async_read__invalid_size_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_spi_async_read(m_test_array, 0U);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief This test verifies that the bsp_spi_cb_set function successfully 
 * executes when called with correct arguments.
 * 
 * https://codebeamer.daiichi.com/issue/184541
 *  
 */
void test_bsp_spi_cb_set__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_bsp_spi_init();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_spi_cb_set(&spi_flash_cb);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief This test verifies that the bsp_spi_cb_set function failed executes 
 * when called with an invalid callback argument.
 * 
 * https://codebeamer.daiichi.com/issue/184548
 *  
 */
void test_bsp_spi_cb_set__invalid_callback_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_bsp_spi_init();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_spi_cb_set(NULL);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief This test verifies that the bsp_spi_cb_set function failed executes 
 * when called without spi initialization.
 * 
 * https://codebeamer.daiichi.com/issue/184543
 *  
 */
void test_bsp_spi_cb_set__without_initialization__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_ERROR);
    (void)bsp_spi_init();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_spi_cb_set(&spi_flash_cb);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/*******************************************************************************
 * Private Functions Definitions
 ******************************************************************************/
static status_t lpspi_drv_master_init_stub(uint32_t instance, 
                                      lpspi_state_t* lpspiState, 
                                      const lpspi_master_config_t* spiConfig, 
                                      int cmock_num_calls)
{
    (void)instance;
    (void)lpspiState;
    (void)cmock_num_calls;

    const lpspi_master_config_t* test_spiConfig = spiConfig;

    TEST_ASSERT_EQUAL(m_test_lpspi_cfg.bitsPerSec, test_spiConfig->bitsPerSec);
    TEST_ASSERT_EQUAL(m_test_lpspi_cfg.whichPcs, test_spiConfig->whichPcs);
    TEST_ASSERT_EQUAL(m_test_lpspi_cfg.pcsPolarity, test_spiConfig->pcsPolarity);
    TEST_ASSERT_EQUAL(m_test_lpspi_cfg.isPcsContinuous, test_spiConfig->isPcsContinuous);
    TEST_ASSERT_EQUAL(m_test_lpspi_cfg.bitcount, test_spiConfig->bitcount);
    TEST_ASSERT_EQUAL(m_test_lpspi_cfg.lpspiSrcClk, test_spiConfig->lpspiSrcClk);
    TEST_ASSERT_EQUAL(m_test_lpspi_cfg.clkPhase, test_spiConfig->clkPhase);
    TEST_ASSERT_EQUAL(m_test_lpspi_cfg.clkPolarity, test_spiConfig->clkPolarity);
    TEST_ASSERT_EQUAL(m_test_lpspi_cfg.lsbFirst, test_spiConfig->lsbFirst);
    TEST_ASSERT_EQUAL(m_test_lpspi_cfg.transferType, test_spiConfig->transferType);
    TEST_ASSERT_EQUAL(m_test_lpspi_cfg.rxDMAChannel, test_spiConfig->rxDMAChannel);
    TEST_ASSERT_EQUAL(m_test_lpspi_cfg.txDMAChannel, test_spiConfig->txDMAChannel);
    TEST_ASSERT_EQUAL(m_test_lpspi_cfg.callbackParam, test_spiConfig->callbackParam);

    m_test_flash_cb = test_spiConfig->callback;
    m_test_driver_state = lpspiState;

    return STATUS_SUCCESS;
}

static void helper_bsp_spi_init()
{
    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    LPSPI_DRV_MasterInit_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    (void)bsp_spi_init();
}

static void helper_bsp_spi_init_with_stub()
{
    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    LPSPI_DRV_MasterInit_StubWithCallback(lpspi_drv_master_init_stub);

    (void)bsp_spi_init();
}

static void spi_flash_cb(bsp_spi_err_t spi_err)
{
    if(BSP_SPI_ERR_RX_OVERRUN == spi_err)
    {
        TEST_ASSERT_EQUAL(BSP_SPI_ERR_RX_OVERRUN, spi_err);
    }
    else if(BSP_SPI_ERR_TX_UNDERRUN == spi_err)
    {
        TEST_ASSERT_EQUAL(BSP_SPI_ERR_TX_UNDERRUN, spi_err);
    }
}

// end of file
