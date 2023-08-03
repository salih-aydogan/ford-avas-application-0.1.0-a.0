/**
 * @file test_bsp_i2c.c
 * @author Berke Yalcinkaya <berke.yalcinkaya@daiichi.com>
 * @brief This tests verifies the bsp_i2c module.
 * 
 * @version 0.1
 * 
 * @date 2023-02-01
 * 
 * @copyright Copyright (c) 2023
 * 
 * https://codebeamer.daiichi.com/item/182085
 * 
 */

/*******************************************************************************
 * Includes 
 ******************************************************************************/

#include "unity.h"
#include "support.h"

#include "bsp_i2c.h"
#include "daiichi_types.h"

#include "mock_flexio_common.h"
#include "mock_flexio.h"
#include "mock_flexio_i2c_driver.h"
#include "mock_lpi2c_driver.h"
#include "mock_pins_driver.h"

#include "mock_log.h"

/*******************************************************************************
 * Test Files
 ******************************************************************************/
TEST_FILE("bsp_i2c.c")

/*******************************************************************************
 * Typedef and Definations
 ******************************************************************************/
#define TEST_PIN_COUNT           (2U)

#define TEST_I2C_AMP_PORT        (2U)

#define TEST_INST_ID_I2C1        (0U)

#define TEST_INST_ID_I2C2        (0U)

#define TEST_BLOCKING_TIMEOUT    (100U)

/*******************************************************************************
 * Extern Variables
 ******************************************************************************/

/*******************************************************************************
 * Private Variables
 ******************************************************************************/
/**
 * @brief https://codebeamer.daiichi.com/issue/193911
 * 
 */
static lpi2c_master_state_t* mp_test_lpi2c_master_state;

static lpi2c_master_state_t m_test_lpi2c_master_state;

const lpi2c_master_user_config_t m_test_lpi2c_master_config = {

    .slaveAddress = 0x00U,
    .is10bitAddr = false,
    .operatingMode = LPI2C_FAST_MODE,
    .baudRate = 400000UL,
    .transferType = LPI2C_USING_INTERRUPTS,
    .dmaChannel = 0,
    .masterCallback = NULL,
    .callbackParam = NULL
};

const flexio_i2c_master_user_config_t m_test_flexio_i2c_config = {

    .slaveAddress = 0x6CU,
    .driverType = FLEXIO_DRIVER_TYPE_POLLING,
    .baudRate = 400000UL,
    .sdaPin = 3U,
    .sclPin = 2U,
    .callback = NULL,
    .callbackParam = NULL
};

const pin_settings_config_t m_test_pin_mux_init_config[] = 
{
    {
        .base            = PORTA,
        .pinPortIdx      = 2U,
        .pullConfig      = PORT_INTERNAL_PULL_NOT_ENABLED,
        .driveSelect     = PORT_LOW_DRIVE_STRENGTH,
        .passiveFilter   = false,
        .mux             = PORT_MUX_ALT3,
        .pinLock         = false,
        .intConfig       = PORT_DMA_INT_DISABLED,
        .clearIntFlag    = false,
        .gpioBase        = NULL,
        .digitalFilter   = false,
    },
    {
        .base            = PORTA,
        .pinPortIdx      = 3U,
        .pullConfig      = PORT_INTERNAL_PULL_NOT_ENABLED,
        .driveSelect     = PORT_LOW_DRIVE_STRENGTH,
        .passiveFilter   = false,
        .mux             = PORT_MUX_ALT3,
        .pinLock         = false,
        .intConfig       = PORT_DMA_INT_DISABLED,
        .clearIntFlag    = false,
        .gpioBase        = NULL,
        .digitalFilter   = false,
    }    
};

bsp_i2c_err_cb_t m_test_error_cb[2U];

static bool m_test_flag;

/*******************************************************************************
 * Global Variables
 ******************************************************************************/

uint8_t allocated_area[PERIPH_MEM_SIZE];

/*******************************************************************************
 * Private Functions Prototype Declarations
 ******************************************************************************/

/**
 * @brief This function provides successfully initialize of bsp_i2c module.
 * 
 */
static void helper_bsp_i2c_init_success(void);

/**
 * @brief This function provides successfully initialize of bsp_i2c module.
 * 
 */
static void helper_bsp_i2c_init_stub(bool state);

/**
 * @brief This function provides successfully deinitialize of bsp_i2c module.
 * 
 */
static void helper_bsp_i2c_deinit_success(void);

/**
 * @brief This function provides emulate the callback function.
 * 
 * @param[in] error Communicaiton errors.
 */
static void helper_cb(bsp_i2c_err_t error);

/**
 * @brief This function provides emulate the callback function.
 * 
 * @param[in] port     Can take BSP_I2C_AMP_1_PORT or BSP_I2C_AMP_2_PORT
 * @param[in] callback Pointer to the callback function.
 */
static void helper_cb_set(uint32_t port, bsp_i2c_err_cb_t callback);

/**
 * @brief This function stubs LPI2C_DRV_MasterInit driver function.
 *  
 */
static status_t test_i2c_master_init_stub(uint32_t instance, 
    const lpi2c_master_user_config_t* userConfigPtr, 
    lpi2c_master_state_t* master, 
    int cmock_num_calls);
    
/*******************************************************************************
 * Setup & Teardown 
 ******************************************************************************/

void setUp()
{
    log_print_Ignore();

    m_test_flag = false;
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
 * @brief https://codebeamer.daiichi.com/issue/182186
 *  
 */
void test_bsp_i2c_init__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    PINS_DRV_Init_ExpectWithArrayAndReturn(TEST_PIN_COUNT, 
                                           m_test_pin_mux_init_config, 
                                           2U,
                                           STATUS_SUCCESS);

    LPI2C_DRV_MasterInit_ExpectWithArrayAndReturn(TEST_INST_ID_I2C2,
                                                  &m_test_lpi2c_master_config,
                                                  1U,
                                                  &m_test_lpi2c_master_state,
                                                  1U,
                                                  STATUS_SUCCESS);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_init();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/182240
 *  
 */
void test_bsp_i2c_init__pins_init_error__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_init();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}


/**
 * @brief https://codebeamer.daiichi.com/issue/182248
 *  
 */
void test_bsp_i2c_init__lpi2c_init_error__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //


    // ************************** Expected Calls **************************** //

    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    LPI2C_DRV_MasterInit_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_init();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/182336
 *  
 */
void test_bsp_i2c_deinit__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_init_success();

    // ************************** Expected Calls **************************** //

    LPI2C_DRV_MasterDeinit_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_deinit();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}


/**
 * @brief https://codebeamer.daiichi.com/issue/182350
 *  
 */
void test_bsp_i2c_deinit__lpi2c_deinit_fail__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_init_success();

    // ************************** Expected Calls **************************** //

    LPI2C_DRV_MasterDeinit_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_deinit();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/182351
 *  
 */
void test_bsp_i2c_read__amplifier_port_1_usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint8_t read_data[1U] = {0U};

    uint32_t rx_size = 1U;

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_init_stub(true);

    // ************************** Expected Calls **************************** //

    LPI2C_DRV_MasterReceiveData_ExpectAndReturn(0U,
                                                read_data,
                                                rx_size,
                                                true,
                                                STATUS_SUCCESS);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_read(BSP_I2C_AMP_1_PORT, read_data, rx_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/182352
 *  
 */
void test_bsp_i2c_read__amplifier_port_2_usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint8_t read_data[1U] = {0U};

    uint32_t rx_size = 1U;

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_init_stub(true); 

    // ************************** Expected Calls **************************** //

    LPI2C_DRV_MasterSetSlaveAddr_Expect(0U,(const uint16_t)0x67U, false);

    LPI2C_DRV_MasterReceiveData_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_read(BSP_I2C_AMP_2_PORT, read_data, rx_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}


/**
 * @brief https://codebeamer.daiichi.com/issue/182355
 *  
 */
void test_bsp_i2c_read__amplifier_port_2_receive_fail__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint8_t read_data[1U] = {0U};

    uint32_t rx_size = 1U;

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_init_success();

    // ************************** Expected Calls **************************** //

    LPI2C_DRV_MasterReceiveData_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_read(BSP_I2C_AMP_2_PORT, read_data, rx_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/182356
 *  
 */
void test_bsp_i2c_read__invalid_data_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint32_t port = BSP_I2C_AMP_1_PORT;
    uint8_t *read_data = NULL;

    uint32_t rx_size = 1U;

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_init_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_read(port, read_data, rx_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/182357
 *  
 */
void test_bsp_i2c_read__invalid_size_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint32_t port = BSP_I2C_AMP_1_PORT;
    uint8_t read_data[1U] = {0U};

    uint32_t rx_size = 0U;

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_init_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_read(port, read_data, rx_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/182397
 *  
 */
void test_bsp_i2c_read__invalid_port_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint8_t read_data[1U] = {0U};
    uint32_t port = TEST_I2C_AMP_PORT;

    uint32_t rx_size = 1U;

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_init_stub(true);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_read(port, read_data, rx_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/182360
 *  
 */
void test_bsp_i2c_write__amplifier_port_1_usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint8_t tx_data[1U] = {0U};

    uint32_t tx_size = 1U;

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_init_stub(true);

    // ************************** Expected Calls **************************** //

    LPI2C_DRV_MasterSetSlaveAddr_Expect(0U,(const uint16_t)0x66U, false);

    LPI2C_DRV_MasterSendData_ExpectAndReturn(0U, 
                                             tx_data, 
                                             tx_size, 
                                             true, 
                                             STATUS_SUCCESS);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_write(BSP_I2C_AMP_1_PORT, tx_data, tx_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/182361
 *  
 */
void test_bsp_i2c_write__amplifier_port_2_usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint8_t tx_data[1U] = {0U};

    uint32_t tx_size = 1U;

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_init_stub(true);

    // ************************** Expected Calls **************************** //

    LPI2C_DRV_MasterSetSlaveAddr_Expect(0U,(const uint16_t)0x67U, false);

    LPI2C_DRV_MasterSendData_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_write(BSP_I2C_AMP_2_PORT, tx_data, tx_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}


/**
 * @brief https://codebeamer.daiichi.com/issue/182363
 *  
 */
void test_bsp_i2c_write__amplifier_port_2_write_error__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint8_t tx_data[1U] = {0U};

    uint32_t tx_size = 1U;

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_init_stub(true);

    // ************************** Expected Calls **************************** //

    LPI2C_DRV_MasterSendData_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_write(BSP_I2C_AMP_2_PORT, tx_data, tx_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/182366
 *  
 */
void test_bsp_i2c_write__invalid_data_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint32_t port = BSP_I2C_AMP_1_PORT;

    uint32_t tx_size = 1U;
    uint8_t *tx_data = NULL;

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_init_stub(true);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_write(port, tx_data, tx_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/182367
 *  
 */
void test_bsp_i2c_write__invalid_size_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint32_t port = BSP_I2C_AMP_1_PORT;
    uint8_t tx_data[1U] = {0U};

    uint32_t tx_size = 0U;

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_init_stub(true);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_write(port, tx_data, tx_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/182396
 *  
 */
void test_bsp_i2c_write__invalid_port_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint32_t port = TEST_I2C_AMP_PORT;
    uint8_t tx_data[1U] = {0U};

    uint32_t tx_size = 0U;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    helper_bsp_i2c_init_stub(true);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_write(port, tx_data, tx_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/182369
 *  
 */
void test_bsp_i2c_write_read__amplifier_port_1_usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint8_t tx_data[1U] = {0U};
    uint8_t rx_data[1U] = {0U};

    uint32_t tx_size = 1U;
    uint32_t rx_size = 1U;

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_init_stub(true);

    // ************************** Expected Calls **************************** //

    LPI2C_DRV_MasterSetSlaveAddr_Expect(0U,(const uint16_t)0x66U, false);

    LPI2C_DRV_MasterSendData_ExpectAndReturn(0U, 
                                             tx_data, 
                                             tx_size, 
                                             false, 
                                             STATUS_SUCCESS);

    LPI2C_DRV_MasterReceiveData_ExpectAndReturn(0U,
                                                rx_data,
                                                rx_size,
                                                true,
                                                STATUS_SUCCESS);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_write_read(BSP_I2C_AMP_1_PORT,
                                  tx_data,
                                  tx_size,
                                  rx_data, 
                                  rx_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/182372
 *  
 */
void test_bsp_i2c_write_read__amplifier_port_2_usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint8_t tx_data[1U] = {0U};
    uint8_t rx_data[1U] = {0U};

    uint32_t tx_size = 1U;
    uint32_t rx_size = 1U;

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_init_stub(true);

    // ************************** Expected Calls **************************** //

    LPI2C_DRV_MasterSetSlaveAddr_Expect(0U,(const uint16_t)0x67U, false);

    LPI2C_DRV_MasterSendData_ExpectAndReturn(0U, 
                                             tx_data, 
                                             tx_size, 
                                             false, 
                                             STATUS_SUCCESS);

    LPI2C_DRV_MasterReceiveData_ExpectAndReturn(0U,
                                                rx_data,
                                                rx_size,
                                                true,
                                                STATUS_SUCCESS);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_write_read(BSP_I2C_AMP_2_PORT,
                                  rx_data,
                                  rx_size,
                                  tx_data, 
                                  tx_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/182373
 *  
 */
void test_bsp_i2c_write_read__amplifier_port_1_write_error__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint8_t tx_data[1U] = {0U};
    uint8_t rx_data[1U] = {0U};

    uint32_t tx_size = 1U;
    uint32_t rx_size = 1U;

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_init_stub(true);

    // ************************** Expected Calls **************************** //

    LPI2C_DRV_MasterSetSlaveAddr_Expect(0U,(const uint16_t)0x66U, false);

    LPI2C_DRV_MasterSendData_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_write_read(BSP_I2C_AMP_1_PORT,
                                  rx_data,
                                  rx_size,
                                  tx_data, 
                                  tx_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/182375
 *  
 */
void test_bsp_i2c_write_read__amplifier_port_1_receive_error__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint8_t tx_data[1U] = {0U};
    uint8_t rx_data[1U] = {0U};

    uint32_t tx_size = 1U;
    uint32_t rx_size = 1U;

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_init_stub(true);

    // ************************** Expected Calls **************************** //

    LPI2C_DRV_MasterSendData_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    LPI2C_DRV_MasterReceiveData_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_write_read(BSP_I2C_AMP_1_PORT,
                                  rx_data,
                                  rx_size,
                                  tx_data, 
                                  tx_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/182376
 *  
 */
void test_bsp_i2c_write_read__amplifier_port_2_write_error__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint8_t tx_data[1U] = {0U};
    uint8_t rx_data[1U] = {0U};

    uint32_t tx_size = 1U;
    uint32_t rx_size = 1U;

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_init_stub(true);

    // ************************** Expected Calls **************************** //

    LPI2C_DRV_MasterSetSlaveAddr_Expect(0U,(const uint16_t)0x67U, false);

    LPI2C_DRV_MasterSendData_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_write_read(BSP_I2C_AMP_2_PORT,
                                  rx_data,
                                  rx_size,
                                  tx_data, 
                                  tx_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/182377
 *  
 */
void test_bsp_i2c_write_read__amplifier_port_2_receive_error__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint8_t tx_data[1U] = {0U};
    uint8_t rx_data[1U] = {0U};

    uint32_t tx_size = 1U;
    uint32_t rx_size = 1U;

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_deinit_success();

    helper_bsp_i2c_init_stub(true);

    // ************************** Expected Calls **************************** //

    LPI2C_DRV_MasterSendData_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    LPI2C_DRV_MasterReceiveData_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_write_read(BSP_I2C_AMP_2_PORT,
                                  rx_data,
                                  rx_size,
                                  tx_data, 
                                  tx_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/182382
 *  
 */
void test_bsp_i2c_write_read__invalid_receive_data_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint8_t tx_data[1U] = {0U};
    uint8_t *rx_data = NULL;

    uint32_t tx_size = 1U;
    uint32_t rx_size = 1U;

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_init_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_write_read(BSP_I2C_AMP_1_PORT,
                                  rx_data,
                                  rx_size,
                                  tx_data, 
                                  tx_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/182386
 *  
 */
void test_bsp_i2c_write_read__invalid_receive_size_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint8_t tx_data[1U] = {0U};
    uint8_t rx_data[1U] = {0U};

    uint32_t tx_size = 1U;
    uint32_t rx_size = 0U;

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_init_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_write_read(BSP_I2C_AMP_1_PORT,
                                  rx_data,
                                  rx_size,
                                  tx_data, 
                                  tx_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/182393
 *  
 */
void test_bsp_i2c_write_read__invalid_transmit_data_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint8_t rx_data[1U] = {0U};
    uint8_t *tx_data = NULL;

    uint32_t tx_size = 1U;
    uint32_t rx_size = 1U;

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_init_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_write_read(BSP_I2C_AMP_1_PORT,
                                  rx_data,
                                  rx_size,
                                  tx_data, 
                                  tx_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/182394
 *  
 */
void test_bsp_i2c_write_read__invalid_transmit_size_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint8_t rx_data[1U] = {0U};
    uint8_t tx_data[1U] = {0U};

    uint32_t rx_size = 1U;
    uint32_t tx_size = 0U;

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_init_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_write_read(BSP_I2C_AMP_1_PORT,
                                  rx_data,
                                  rx_size,
                                  tx_data, 
                                  tx_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/182395
 *  
 */
void test_bsp_i2c_write_read__invalid_port_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint32_t port = TEST_I2C_AMP_PORT;
    uint8_t rx_data[1U] = {0U};
    uint8_t tx_data[1U] = {0U};

    uint32_t rx_size = 1U;
    uint32_t tx_size = 1U;    

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_init_stub(true);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_write_read(port,
                                  rx_data,
                                  rx_size,
                                  tx_data, 
                                  tx_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/184387
 *  
 */
void test_bsp_i2c_write_read__i2c_received_nack_error_cb__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint8_t rx_data[1U] = {0U};
    uint8_t tx_data[1U] = {0U};
    uint32_t port = BSP_I2C_AMP_2_PORT;

    uint32_t rx_size = 1U;
    uint32_t tx_size = 1U;

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_deinit_success();
    helper_bsp_i2c_init_stub(true);
    helper_cb_set(port, helper_cb);

    // ************************** Expected Calls **************************** //

    LPI2C_DRV_MasterSendData_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    LPI2C_DRV_MasterReceiveData_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    mp_test_lpi2c_master_state->status = STATUS_I2C_RECEIVED_NACK;

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_write_read(port,
                                  rx_data,
                                  rx_size,
                                  tx_data,
                                  tx_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/184389
 *  
 */
void test_bsp_i2c_write_read__i2c_tx_underrun_error_cb__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint8_t rx_data[1U] = {0U};
    uint8_t tx_data[1U] = {0U};
    uint32_t port = BSP_I2C_AMP_2_PORT;

    uint32_t rx_size = 1U;
    uint32_t tx_size = 1U;

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_deinit_success();
    helper_bsp_i2c_init_stub(true);
    helper_cb_set(port, helper_cb);

    // ************************** Expected Calls **************************** //

    LPI2C_DRV_MasterSendData_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    LPI2C_DRV_MasterReceiveData_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    mp_test_lpi2c_master_state->status = STATUS_I2C_TX_UNDERRUN;

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_write_read(port,
                                  rx_data,
                                  rx_size,
                                  tx_data, 
                                  tx_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/184390
 *  
 */
void test_bsp_i2c_write_read__i2c_rx_overrrun_error_cb__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint8_t rx_data[1U] = {0U};
    uint8_t tx_data[1U] = {0U};
    uint32_t port = BSP_I2C_AMP_2_PORT;

    uint32_t rx_size = 1U;
    uint32_t tx_size = 1U;

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_deinit_success();
    helper_bsp_i2c_init_stub(true);
    helper_cb_set(port, helper_cb);

    // ************************** Expected Calls **************************** //

    LPI2C_DRV_MasterSendData_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    LPI2C_DRV_MasterReceiveData_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    mp_test_lpi2c_master_state->status = STATUS_I2C_RX_OVERRUN;

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_write_read(port,
                                  rx_data,
                                  rx_size,
                                  tx_data, 
                                  tx_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/184391
 *  
 */
void test_bsp_i2c_write_read__i2c_arbitration_lost_error_cb__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint8_t rx_data[1U] = {0U};
    uint8_t tx_data[1U] = {0U};
    uint32_t port = BSP_I2C_AMP_2_PORT;

    uint32_t rx_size = 1U;
    uint32_t tx_size = 1U;

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_deinit_success();
    helper_bsp_i2c_init_stub(true);
    helper_cb_set(port, helper_cb);

    // ************************** Expected Calls **************************** //

    LPI2C_DRV_MasterSendData_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    LPI2C_DRV_MasterReceiveData_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    mp_test_lpi2c_master_state->status = STATUS_I2C_ARBITRATION_LOST;

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_write_read(port,
                                  rx_data,
                                  rx_size,
                                  tx_data, 
                                  tx_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief https://codebeamer.daiichi.com/item/186629
 *  
 */
void test_bsp_i2c_write_read__without_cb__error_cb_not_called(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint8_t rx_data[1U] = {0U};
    uint8_t tx_data[1U] = {0U};
    uint32_t port = BSP_I2C_AMP_2_PORT;

    uint32_t rx_size = 1U;
    uint32_t tx_size = 1U;

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_init_stub(true);
    helper_cb_set(port, helper_cb);
    helper_bsp_i2c_deinit_success();

    // **************************** Test Steps ****************************** //

    // ************************** Expected Calls **************************** //

    LPI2C_DRV_MasterSendData_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    LPI2C_DRV_MasterReceiveData_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    mp_test_lpi2c_master_state->status = STATUS_I2C_ABORTED;

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_write_read(port,
                                  rx_data,
                                  rx_size,
                                  tx_data, 
                                  tx_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/184392
 *  
 */
void test_bsp_i2c_write_read__i2c_aborted_error_cb__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint8_t rx_data[1U] = {0U};
    uint8_t tx_data[1U] = {0U};
    uint32_t port = BSP_I2C_AMP_2_PORT;

    uint32_t rx_size = 1U;
    uint32_t tx_size = 1U;

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_deinit_success();
    helper_bsp_i2c_init_stub(true);
    helper_cb_set(port, helper_cb);

    // ************************** Expected Calls **************************** //

    LPI2C_DRV_MasterSendData_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    LPI2C_DRV_MasterReceiveData_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    mp_test_lpi2c_master_state->status = STATUS_I2C_ABORTED;

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_write_read(port,
                                  rx_data,
                                  rx_size,
                                  tx_data, 
                                  tx_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/184393
 *  
 */
void test_bsp_i2c_write_read__i2c_bus_busy_error_cb__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint8_t rx_data[1U] = {0U};
    uint8_t tx_data[1U] = {0U};
    uint32_t port = BSP_I2C_AMP_2_PORT;

    uint32_t rx_size = 1U;
    uint32_t tx_size = 1U;

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_deinit_success();
    helper_bsp_i2c_init_stub(true);
    helper_cb_set(port, helper_cb);

    // ************************** Expected Calls **************************** //

    LPI2C_DRV_MasterSendData_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    LPI2C_DRV_MasterReceiveData_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    mp_test_lpi2c_master_state->status = STATUS_I2C_ABORTED;

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_i2c_write_read(port,
                                  rx_data,
                                  rx_size,
                                  tx_data, 
                                  tx_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/183284
 * 
 */
void test_bsp_i2c_cb_set__port_1_usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint32_t port = BSP_I2C_AMP_1_PORT;

    bsp_i2c_err_cb_t callback = helper_cb;

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_init_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_i2c_cb_set(port, callback);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/183289
 * 
 */
void test_bsp_i2c_cb_set__port_2_usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint32_t port = BSP_I2C_AMP_2_PORT;

    bsp_i2c_err_cb_t callback = helper_cb;

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_init_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_i2c_cb_set(port, callback);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/183285
 * 
 */
void test_bsp_i2c_cb_set__invalid_port_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint32_t port = TEST_I2C_AMP_PORT;

    bsp_i2c_err_cb_t callback = helper_cb;

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_init_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_i2c_cb_set(port, callback);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/183286
 * 
 */
void test_bsp_i2c_cb_set__invalid_callback_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint32_t port = BSP_I2C_AMP_1_PORT;

    bsp_i2c_err_cb_t callback = NULL;

    // **************************** Pre-Actions ***************************** //

    helper_bsp_i2c_init_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_i2c_cb_set(port, callback);
}

/*******************************************************************************
 * Private Functions Definitions
 ******************************************************************************/

static void helper_bsp_i2c_init_success(void)
{
    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    LPI2C_DRV_MasterInit_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    (void)bsp_i2c_init();
}

static void helper_bsp_i2c_init_stub(bool state)
{

    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    LPI2C_DRV_MasterInit_StubWithCallback(test_i2c_master_init_stub);

    (void)bsp_i2c_init();

    mp_test_lpi2c_master_state->i2cIdle = state;
}

static void helper_bsp_i2c_deinit_success(void)
{
    LPI2C_DRV_MasterDeinit_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    (void)bsp_i2c_deinit();
}

static void helper_cb(bsp_i2c_err_t error)
{
    if (BSP_I2C_STATUS_ERR_RECEIVED_NACK == error)
    {
        m_test_flag = true;
    }

    if (BSP_I2C_STATUS_ERR_TX_UNDERRUN == error)
    {
        m_test_flag = true;
    }

    if (BSP_I2C_STATUS_ERR_RX_OVERRUN == error)
    {
        m_test_flag = true;
    }

    if (BSP_I2C_STATUS_ERR_ARBITRATION_LOST == error)
    {
        m_test_flag = true;
    }

    if (BSP_I2C_STATUS_ERR_ABORTED == error)
    {
        m_test_flag = true;
    }

    if (BSP_I2C_STATUS_ERR_BUS_BUSY == error)
    {
        m_test_flag = true;
    }  

    TEST_ASSERT_EQUAL(true, m_test_flag);                 
}

static void helper_cb_set(uint32_t port, bsp_i2c_err_cb_t callback)
{
    bsp_i2c_cb_set(port, callback);
}

static status_t test_i2c_master_init_stub(uint32_t instance, 
    const lpi2c_master_user_config_t* userConfigPtr, 
    lpi2c_master_state_t* master, 
    int cmock_num_calls)
{
    (void)instance;
    (void)userConfigPtr;
    (void)cmock_num_calls;

    mp_test_lpi2c_master_state = master;

    return STATUS_SUCCESS;
}

// end of file 
