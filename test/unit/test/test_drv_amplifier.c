/**
 * @file test_drv_amplifier.c
 * @author Muhammed Salih AYDOGAN
 * @brief These tests verify the units of the Amplifier driver module.
 * 
 * @version 0.1
 * 
 * @date 27/05/2023
 * 
 * @copyright Copyright (c) 2023
 * 
 * https://codebeamer.daiichi.com/item/194192
 * 
 */

/*******************************************************************************
 * Includes 
 ******************************************************************************/

//First - include test framework headers
#include "unity.h"
#include "support.h"

// Second - include the module to be tested
#include "drv_amplifier.h"

// Third - include the mocks of required modules
#include "mock_bsp_mcu.h"
#include "mock_bsp_i2c.h"
#include "mock_bsp_gpio.h"

#include "mock_log.h"

/*******************************************************************************
 * Test Files
 ******************************************************************************/

TEST_FILE("drv_amplifier.c");

/*******************************************************************************             
 * Defines
 ******************************************************************************/
/**
 * @brief https://codebeamer.daiichi.com/issue/192997
 * 
 */
#define TEST_IB11_ADDR                       0x0BU

/**
 * @brief https://codebeamer.daiichi.com/issue/192998
 * 
 */
#define TEST_IB0_ADDR_SIZE                   1U

/**
 * @brief https://codebeamer.daiichi.com/issue/192999
 * 
 */
#define TEST_MUTE_UNMUTE_DATA_SIZE           2U

/**
 * @brief https://codebeamer.daiichi.com/issue/193001
 * 
 */
#define TEST_DB_SIZE                         11U

/**
 * @brief https://codebeamer.daiichi.com/issue/193002
 * 
 */
#define TEST_IB_SIZE                         23U

/**
 * @brief https://codebeamer.daiichi.com/issue/193003
 * 
 */
#define TEST_AMPLIFIER_FDA903S_MUTE_UNMUTE   (0x01U << 0U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193004
 * 
 */
#define TEST_AMPLIFIER_DIAG_START_BIT_1      (0x01U << 1U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193005
 * 
 */
#define TEST_AMPLIFIER_DIAG_START_BIT_2      (0x01U << 2U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193006
 * 
 */
#define TEST_DIAG_INPUT_OFFSET_POS           (0x01U << 7U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193007
 * 
 */
#define TEST_DIAG_OVERCURR_PROTECT_POS       (0x01U << 6U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193008
 * 
 */
#define TEST_DIAG_CLIPPING_DETECT_POS        (0x01U << 3U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193009
 * 
 */
#define TEST_DIAG_THERMAL_SD_POS             (0x01U << 0U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193010
 * 
 */
#define TEST_DIAG_ANALOG_MUTE_POS            (0x01U << 5U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193011
 * 
 */
#define TEST_DIAG_VOV_SD_POS                 (0x01U << 3U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193012
 * 
 */
#define TEST_DIAG_OUTCURR_OFFSET_POS         (0x01U << 7U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193013
 * 
 */
#define TEST_DIAG_DATA_VALID_POS             (0x01U << 2U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193014
 * 
 */
#define TEST_DIAG_PLAY_POS                   (0x01U << 0U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193015
 * 
 */
#define TEST_ERROR_SHORT_TO_VCC_POS          (0x01U << 5U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193016
 * 
 */
#define TEST_ERROR_SHORT_TO_GND_POS          (0x01U << 4U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193017
 * 
 */
#define TEST_ERROR_WDG_POS                   (0x01U << 2U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193018
 * 
 */
#define TEST_ERROR_FRAME_CHECK_POS           (0x01U << 1U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193019
 * 
 */
#define TEST_ERROR_PLL_LOCKED_POS            (0x01U << 0U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193020
 * 
 */
#define TEST_ERROR_TW_1_POS                  (0x01U << 4U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193021
 * 
 */
#define TEST_ERROR_TW_2_POS                  (0x01U << 3U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193022
 * 
 */
#define TEST_ERROR_TW_4_POS                  (0x01U << 2U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193023
 * 
 */
#define TEST_ERROR_UVLO_POS                  (0x01U << 2U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193024
 * 
 */
#define TEST_ERROR_LOW_BATT_POS              (0x01U << 1U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193025
 * 
 */
#define TEST_ERROR_HIGH_BATT_POS             (0x01U << 0U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193026
 * 
 */
#define TEST_ERROR_SHORT_LOAD_POS            (0x01U << 4U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193027
 * 
 */
#define TEST_ERROR_OPEN_LOAD_POS             (0x01U << 3U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193051
 * 
 */
#define TEST_DEFAULT_VALUE_OF_IB20           0x11U

/**
 * @brief https://codebeamer.daiichi.com/issue/193053
 * 
 */
#define TEST_DEFAULT_VALUE_OF_IB22           0x00U

/**
 * @brief https://codebeamer.daiichi.com/issue/193153
 * 
 */
#define TEST_DC_DIAG_IN_PLAY_FOR_IB20        0xB8U

/**
 * @brief https://codebeamer.daiichi.com/issue/193154
 * 
 */
#define TEST_DC_DIAG_IN_PLAY_FOR_IB22        0x02U

/**
 * @brief https://codebeamer.daiichi.com/issue/179888
 * 
 */
#define TEST_BSP_I2C_AMP_1_PORT 0U

/**
 * @brief https://codebeamer.daiichi.com/issue/179889
 * 
 */
#define TEST_BSP_I2C_AMP_2_PORT 1U

/*******************************************************************************
 * Private Variables
 ******************************************************************************/
/**
 * @brief This variable holds the number of function calls.
 * 
 */
static uint8_t m_test_func_cnt;

/**
 * @brief Variable to run tests while amplifier muted.
 * 
 */
static bool m_test_amplifier_is_mute;

/**
 * @brief Variable for thermal control.
 * 
 */
static uint8_t m_test_thermal_warning_control;

/**
 * @brief This function is used for to i2c port1 error callback function.
 * 
 */
static bsp_i2c_err_cb_t m_test_fda903s_i2c_port_1_error_cb;

/**
 * @brief This function is used for to i2c port2 error callback function.
 * 
 */
static bsp_i2c_err_cb_t m_test_fda903s_i2c_port_2_error_cb;

/**
 * @brief This variable holds the values to be sent to the amplifiers.
 * 
 */
static uint8_t m_test_instruction_bytes[TEST_IB0_ADDR_SIZE + TEST_IB_SIZE];

/*******************************************************************************
 * Private Functions Prototype Declarations
 ******************************************************************************/
/**
 * @brief Helper function for successful amplifier initialization.
 * 
 */
static void helper_drv_amplifier_init(void);

/**
 * @brief Helper function for successful amplifier mute initialization.
 * 
 */
static void helper_drv_amplifier_mute(void);

/**
 * @brief Stub function to receive data from bsp_i2c_write function.
 * 
 */
static bool test_bsp_i2c_write_stub(uint32_t port, 
                                    const uint8_t* p_data, 
                                    size_t size, 
                                    int cmock_num_calls);

/**
 * @brief Stub function to receive data from bsp_i2c_write_read function.
 * 
 */
static bool test_bsp_i2c_write_read_stub(uint32_t port, 
                                         uint8_t* p_rx_data, 
                                         size_t rx_size, 
                                         const uint8_t* p_tx_data, 
                                         size_t tx_size, 
                                         int cmock_num_calls);

/**
 * @brief Stub function to receive data from bsp_i2c_write function.
 * 
 */
static bool test_bsp_i2c_write_mute_stub(uint32_t port, 
                                         const uint8_t* p_data, 
                                         size_t size, 
                                         int cmock_num_calls);


/**
 * @brief Stub function to receive data from bsp_i2c_write function.
 * 
 */
static bool test_bsp_i2c_write_unmute_stub(uint32_t port, 
                                           const uint8_t* p_data, 
                                           size_t size, 
                                           int cmock_num_calls);

/**
 * @brief Stub function for set rx data. 
 * 
 */
static bool test_bsp_i2c_write_read_cb_stub(uint32_t port, 
                                            uint8_t* p_rx_data, 
                                            size_t rx_size, 
                                            const uint8_t* p_tx_data, 
                                            size_t tx_size, 
                                            int cmock_num_calls);

/**
 * @brief Callback function to receive data from bsp_i2c_cb_set function.
 * 
 */
static void test_fda903s_i2c_port_error_cb(uint32_t port, 
                                             bsp_i2c_err_cb_t callback, 
                                             int cmock_num_calls);

/**
 * @brief Stub function for thermal warning 4 state.
 * 
 */
static bool test_bsp_i2c_write_read_cb_tw_stub(uint32_t port, 
                                               uint8_t* p_rx_data, 
                                               size_t rx_size, 
                                               const uint8_t* p_tx_data, 
                                               size_t tx_size, 
                                               int cmock_num_calls);

/**
 * @brief Callback function for testing the set function.
 * 
 */
static void test_drv_amplifier_diag_cb(drv_amplifier_diag_t diagnostic);

/**
 * @brief Callback function for the test of status of being 
 * DRV_AMPLIFIER_DIAG_ERR_TW_3.
 * 
 */
static void test_drv_amplifier_diag_tw_cb(drv_amplifier_diag_t diagnostic);

/**
 * @brief Emty callback funtion for test of set function. 
 * 
 */
static void test_drv_amplifier_diag_emty_cb(drv_amplifier_diag_t diagnostic);
                                         
/*******************************************************************************
 * Setup & Teardown 
 ******************************************************************************/

void setUp()
{
    log_print_Ignore();

    m_test_func_cnt = 0U;
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
 * @brief Verifies drv_amplifier_init function when usual run.
 * 
 * https://codebeamer.daiichi.com/item/193858
 *  
 */
void test_drv_amplifier_init__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //
    
    // ************************** Expected Calls **************************** //

    bsp_i2c_cb_set_StubWithCallback(test_fda903s_i2c_port_error_cb);

    uint8_t m_test_instruction_bytes[TEST_IB0_ADDR_SIZE + TEST_IB_SIZE];
    bsp_i2c_write_ExpectAndReturn(TEST_BSP_I2C_AMP_1_PORT, 
                                  m_test_instruction_bytes, 
                                  (TEST_IB0_ADDR_SIZE + TEST_IB_SIZE), 
                                  true);
    bsp_i2c_write_IgnoreArg_p_data();

    bsp_i2c_write_ExpectAndReturn(TEST_BSP_I2C_AMP_2_PORT, 
                                  m_test_instruction_bytes, 
                                  (TEST_IB0_ADDR_SIZE + TEST_IB_SIZE), 
                                  true);
    bsp_i2c_write_IgnoreArg_p_data();

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = drv_amplifier_init();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);

    TEST_STEP(3);
    m_test_fda903s_i2c_port_1_error_cb(BSP_I2C_STATUS_ERR_GENERIC);

    TEST_STEP(4);
    m_test_fda903s_i2c_port_1_error_cb(BSP_I2C_STATUS_ERR_RECEIVED_NACK);

    TEST_STEP(5);
    m_test_fda903s_i2c_port_1_error_cb(BSP_I2C_STATUS_ERR_TX_UNDERRUN);

    TEST_STEP(6);
    m_test_fda903s_i2c_port_1_error_cb(BSP_I2C_STATUS_ERR_RX_OVERRUN);

    TEST_STEP(7);
    m_test_fda903s_i2c_port_1_error_cb(BSP_I2C_STATUS_ERR_ARBITRATION_LOST);

    TEST_STEP(8);
    m_test_fda903s_i2c_port_1_error_cb(BSP_I2C_STATUS_ERR_ABORTED);

    TEST_STEP(9);
    m_test_fda903s_i2c_port_1_error_cb(BSP_I2C_STATUS_ERR_BUS_BUSY);

    TEST_STEP(10);
    m_test_fda903s_i2c_port_2_error_cb(BSP_I2C_STATUS_ERR_GENERIC);

    TEST_STEP(11);
    m_test_fda903s_i2c_port_2_error_cb(BSP_I2C_STATUS_ERR_RECEIVED_NACK);

    TEST_STEP(12);
    m_test_fda903s_i2c_port_2_error_cb(BSP_I2C_STATUS_ERR_TX_UNDERRUN);

    TEST_STEP(13);
    m_test_fda903s_i2c_port_2_error_cb(BSP_I2C_STATUS_ERR_RX_OVERRUN);

    TEST_STEP(14);
    m_test_fda903s_i2c_port_2_error_cb(BSP_I2C_STATUS_ERR_ARBITRATION_LOST);

    TEST_STEP(15);
    m_test_fda903s_i2c_port_2_error_cb(BSP_I2C_STATUS_ERR_ABORTED);

    TEST_STEP(16);
    m_test_fda903s_i2c_port_2_error_cb(BSP_I2C_STATUS_ERR_BUS_BUSY);
}


/**
 * @brief Verifies drv_amplifier_init function when the amplifier is not 
 * initialized properly for port 1.
 * 
 * https://codebeamer.daiichi.com/item/193863
 *  
 */
void test_drv_amplifier_init__port_1_write_process_err__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //
    
    // ************************** Expected Calls **************************** //

    bsp_i2c_cb_set_ExpectAnyArgs();

    bsp_i2c_cb_set_ExpectAnyArgs();

    bsp_i2c_write_ExpectAnyArgsAndReturn(false);

    bsp_mcu_delay_Expect(10U);

    bsp_i2c_write_ExpectAnyArgsAndReturn(false);

    bsp_mcu_delay_Expect(10U);

    bsp_i2c_write_ExpectAnyArgsAndReturn(false);

    bsp_mcu_delay_Expect(10U);

    bsp_i2c_write_ExpectAnyArgsAndReturn(false);

    bsp_mcu_delay_Expect(10U);

    bsp_i2c_write_ExpectAnyArgsAndReturn(false);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = drv_amplifier_init();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies drv_amplifier_init function when the amplifier is not 
 * initialized properly for port 2.
 * 
 * https://codebeamer.daiichi.com/item/193864
 *  
 */
void test_drv_amplifier_init__port_2_write_process_err__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //
    
    // ************************** Expected Calls **************************** //

    bsp_i2c_cb_set_ExpectAnyArgs();
    
    bsp_i2c_cb_set_ExpectAnyArgs();
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(true);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(false);
    
    bsp_mcu_delay_Expect(10U);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(false);
    
    bsp_mcu_delay_Expect(10U);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(false);
    
    bsp_mcu_delay_Expect(10U);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(false);
    
    bsp_mcu_delay_Expect(10U);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(false);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = drv_amplifier_init();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies drv_amplifier_mute function successfully.
 * 
 * https://codebeamer.daiichi.com/item/193865
 *  
 */
void test_drv_amplifier_mute__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_drv_amplifier_init();
    
    // ************************** Expected Calls **************************** //

    bsp_i2c_write_StubWithCallback(test_bsp_i2c_write_mute_stub);
    
    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = drv_amplifier_mute();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief Verifies drv_amplifier_mute function successfully when mute has been 
 * done before.
 * 
 * https://codebeamer.daiichi.com/item/193947
 *  
 */
void test_drv_amplifier_mute__when_amp_is_mute__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_drv_amplifier_init();
    
    helper_drv_amplifier_mute();
    
    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = drv_amplifier_mute();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief Verifies drv_amplifier_mute function when the write process is fail 
 * and it is seen that the function tries four times.
 * 
 * https://codebeamer.daiichi.com/item/193948
 *  
 */
void test_drv_amplifier_mute__when_amp_1_mute_fail__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_drv_amplifier_init();
    
    // ************************** Expected Calls **************************** //

    bsp_i2c_write_ExpectAnyArgsAndReturn(false);
    
    bsp_mcu_delay_Expect(10U);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(false);
    
    bsp_mcu_delay_Expect(10U);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(false);
    
    bsp_mcu_delay_Expect(10U);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(false);
    
    bsp_mcu_delay_Expect(10U);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(false);
    
    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = drv_amplifier_mute();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies drv_amplifier_mute function when the write process is fail 
 * and it is seen that the function tries four times.
 * 
 * https://codebeamer.daiichi.com/item/193950
 *  
 */
void test_drv_amplifier_mute__when_amp_2_mute_fail__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_drv_amplifier_init();
    
    // ************************** Expected Calls **************************** //

    bsp_i2c_write_ExpectAnyArgsAndReturn(true);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(false);
    
    bsp_mcu_delay_Expect(10U);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(false);
    
    bsp_mcu_delay_Expect(10U);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(false);
    
    bsp_mcu_delay_Expect(10U);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(false);
    
    bsp_mcu_delay_Expect(10U);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(false);
    
    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = drv_amplifier_mute();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies drv_amplifier_unmute function successfully.
 * 
 * https://codebeamer.daiichi.com/item/193952
 *  
 */
void test_drv_amplifier_unmute__when_amp_is_unmute__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_drv_amplifier_init();
    
    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = drv_amplifier_unmute();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief Verifies drv_amplifier_unmute function when the write process is fail 
 * and it is seen that the function tries four times.
 * 
 * https://codebeamer.daiichi.com/item/193954
 *  
 */
void test_drv_amplifier_unmute__when_amp_1_unmute_fail__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_drv_amplifier_init();
    
    helper_drv_amplifier_mute();
    
    // ************************** Expected Calls **************************** //

    bsp_i2c_write_ExpectAnyArgsAndReturn(false);
    
    bsp_mcu_delay_Expect(10U);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(false);
    
    bsp_mcu_delay_Expect(10U);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(false);
    
    bsp_mcu_delay_Expect(10U);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(false);
    
    bsp_mcu_delay_Expect(10U);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(false);
    
    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = drv_amplifier_unmute();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies drv_amplifier_unmute function when the write process is fail 
 * and it is seen that the function tries four times.
 * 
 * https://codebeamer.daiichi.com/item/193957
 *  
 */
void test_drv_amplifier_unmute__when_amp_2_unmute_fail__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_drv_amplifier_init();
    
    helper_drv_amplifier_mute();
    
    // ************************** Expected Calls **************************** //

    bsp_i2c_write_ExpectAnyArgsAndReturn(true);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(false);
    
    bsp_mcu_delay_Expect(10U);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(false);
    
    bsp_mcu_delay_Expect(10U);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(false);
    
    bsp_mcu_delay_Expect(10U);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(false);
    
    bsp_mcu_delay_Expect(10U);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(false);
    
    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = drv_amplifier_unmute();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies drv_amplifier_unmute function successfully.
 * 
 * https://codebeamer.daiichi.com/item/193958
 *  
 */
void test_drv_amplifier_unmute__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_drv_amplifier_init();
    
    helper_drv_amplifier_mute();
    
    // ************************** Expected Calls **************************** //

    bsp_i2c_write_StubWithCallback(test_bsp_i2c_write_unmute_stub);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = drv_amplifier_unmute();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief Verifies drv_amplifier_diag_cb_set function successfully.
 * 
 * https://codebeamer.daiichi.com/item/193966
 *  
 */
void test_drv_amplifier_diag_cb_set__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_drv_amplifier_init();
    
    // ************************** Expected Calls **************************** //

    
    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    drv_amplifier_diag_cb_set(test_drv_amplifier_diag_cb);
}

/**
 * @brief Verifies drv_amplifier_diag_cb_set function with an invalid argument.
 * 
 * https://codebeamer.daiichi.com/item/193967
 *  
 */
void test_drv_amplifier_diag_cb_set__when_use_invalid_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_drv_amplifier_init();
    
    // ************************** Expected Calls **************************** //

    
    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    drv_amplifier_diag_cb_set(NULL);
}

/**
 * @brief Verifies drv_amplifier_process function with the successful read-write 
 * process. The arguments passed to the function are checked.
 * 
 * https://codebeamer.daiichi.com/item/193969
 *  
 */
void test_drv_amplifier_process__write_read_process__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_drv_amplifier_init();
    
    // ************************** Expected Calls **************************** //

    // set mcu tick value according to WAIT_TIME_FOR_DIAG macro
    uint16_t mcu_tick = 400U;

    bsp_mcu_tick_get_ExpectAndReturn(mcu_tick);

    bsp_i2c_write_read_StubWithCallback(test_bsp_i2c_write_read_stub);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = drv_amplifier_process();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief The drv_amplifier_process function is verified by checking the calls 
 * to the error callback functions.
 * 
 * https://codebeamer.daiichi.com/issue/194002
 *  
 */
void test_drv_amplifier_process__call_error_callback__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_drv_amplifier_init();

    (void)drv_amplifier_diag_cb_set(test_drv_amplifier_diag_cb);
    
    // ************************** Expected Calls **************************** //

    uint32_t mcu_tick = 100U;

    bsp_mcu_tick_get_ExpectAndReturn(mcu_tick);

    bsp_i2c_write_read_StubWithCallback(test_bsp_i2c_write_read_cb_stub);

    bsp_i2c_write_StubWithCallback(test_bsp_i2c_write_stub);

    bsp_mcu_tick_get_ExpectAndReturn(mcu_tick);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = drv_amplifier_process();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief The drv_amplifier_process function is verified when the thermal 
 * warning 4.
 * 
 * https://codebeamer.daiichi.com/item/194173
 *  
 */
void test_drv_amplifier_process__thermal_warning_4_state__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_drv_amplifier_init();

    (void)drv_amplifier_diag_cb_set(test_drv_amplifier_diag_tw_cb);
    
    // ************************** Expected Calls **************************** //

    uint32_t mcu_tick = 100U;

    bsp_mcu_tick_get_ExpectAndReturn(mcu_tick);

    bsp_i2c_write_read_StubWithCallback(test_bsp_i2c_write_read_cb_tw_stub);

    bsp_i2c_write_ExpectAnyArgsAndReturn(true);

    bsp_i2c_write_ExpectAnyArgsAndReturn(true);

    bsp_mcu_tick_get_ExpectAndReturn(mcu_tick);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = drv_amplifier_process();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief The drv_amplifier_process function is verified according to the wait 
 * time for diagnostic. 
 * 
 * https://codebeamer.daiichi.com/item/194150
 *  
 */
void test_drv_amplifier_process__elapsed_time_state__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_drv_amplifier_init();
    
    // ************************** Expected Calls **************************** //

    uint32_t mcu_tick = 0U;
    bsp_mcu_tick_get_ExpectAndReturn(mcu_tick);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = drv_amplifier_process();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief The drv_amplifier_process function is verified based on amplifier 
 * read-write success.
 * 
 * https://codebeamer.daiichi.com/item/194151
 *  
 */
void test_drv_amplifier_process__amp_1_read_write_error__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_drv_amplifier_init();
    
    // ************************** Expected Calls **************************** //

    uint32_t mcu_tick = 350U;

    bsp_mcu_tick_get_ExpectAndReturn(mcu_tick);

    bsp_i2c_write_read_ExpectAnyArgsAndReturn(false);

    bsp_mcu_delay_Expect(10U);

    bsp_i2c_write_read_ExpectAnyArgsAndReturn(false);

    bsp_mcu_delay_Expect(10U);

    bsp_i2c_write_read_ExpectAnyArgsAndReturn(false);

    bsp_mcu_delay_Expect(10U);

    bsp_i2c_write_read_ExpectAnyArgsAndReturn(false);

    bsp_mcu_delay_Expect(10U);

    bsp_i2c_write_read_ExpectAnyArgsAndReturn(false);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = drv_amplifier_process();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The drv_amplifier_process function is verified based on amplifier 
 * read-write success.
 * 
 * https://codebeamer.daiichi.com/issue/194154
 *  
 */
void test_drv_amplifier_process__amp_2_read_write_error__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_drv_amplifier_init();
    
    // ************************** Expected Calls **************************** //

    uint32_t mcu_tick = 350U;

    bsp_mcu_tick_get_ExpectAndReturn(mcu_tick);

    bsp_i2c_write_read_ExpectAnyArgsAndReturn(true);

    bsp_i2c_write_read_ExpectAnyArgsAndReturn(false);

    bsp_mcu_delay_Expect(10U);

    bsp_i2c_write_read_ExpectAnyArgsAndReturn(false);

    bsp_mcu_delay_Expect(10U);

    bsp_i2c_write_read_ExpectAnyArgsAndReturn(false);

    bsp_mcu_delay_Expect(10U);

    bsp_i2c_write_read_ExpectAnyArgsAndReturn(false);

    bsp_mcu_delay_Expect(10U);

    bsp_i2c_write_read_ExpectAnyArgsAndReturn(false);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = drv_amplifier_process();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The drv_amplifier_process function is verified based on diagnostic 
 * start function success.
 * 
 * https://codebeamer.daiichi.com/item/194155
 *  
 */
void test_drv_amplifier_process__when_diag_start_amp_1_error__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_drv_amplifier_init();
    
    (void)drv_amplifier_diag_cb_set(test_drv_amplifier_diag_emty_cb);
    
    // ************************** Expected Calls **************************** //

    uint32_t mcu_tick = 350U;
    bsp_mcu_tick_get_ExpectAndReturn(mcu_tick);
    
    bsp_i2c_write_read_StubWithCallback(test_bsp_i2c_write_read_cb_stub);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(false);
    
    bsp_mcu_delay_Expect(10U);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(false);
    
    bsp_mcu_delay_Expect(10U);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(false);
    
    bsp_mcu_delay_Expect(10U);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(false);
    
    bsp_mcu_delay_Expect(10U);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(false);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = drv_amplifier_process();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The drv_amplifier_process function is verified based on diagnostic 
 * start function success.
 * 
 * https://codebeamer.daiichi.com/item/194159
 *  
 */
void test_drv_amplifier_process__when_diag_start_amp_2_error__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_drv_amplifier_init();
    
    (void)drv_amplifier_diag_cb_set(test_drv_amplifier_diag_emty_cb);
    
    // ************************** Expected Calls **************************** //

    uint32_t mcu_tick = 350U;

    bsp_mcu_tick_get_ExpectAndReturn(mcu_tick);
    
    bsp_i2c_write_read_StubWithCallback(test_bsp_i2c_write_read_cb_stub);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(true);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(false);
    
    bsp_mcu_delay_Expect(10U);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(false);
    
    bsp_mcu_delay_Expect(10U);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(false);
    
    bsp_mcu_delay_Expect(10U);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(false);
    
    bsp_mcu_delay_Expect(10U);
    
    bsp_i2c_write_ExpectAnyArgsAndReturn(false);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = drv_amplifier_process();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The drv_amplifier_process function is verified when the amplifier is 
 * mute. 
 * 
 * https://codebeamer.daiichi.com/item/194161
 *  
 */
void test_drv_amplifier_process__when_amplifier_is_mute__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_drv_amplifier_init();
    
    helper_drv_amplifier_mute();
    
    (void)drv_amplifier_diag_cb_set(test_drv_amplifier_diag_emty_cb);
    
    // ************************** Expected Calls **************************** //

    uint32_t mcu_tick = 100U;
    bsp_mcu_tick_get_ExpectAndReturn(mcu_tick);
    
    bsp_i2c_write_read_StubWithCallback(test_bsp_i2c_write_read_cb_stub);
    
    m_test_amplifier_is_mute = true;
    bsp_i2c_write_StubWithCallback(test_bsp_i2c_write_stub);
    
    bsp_mcu_tick_get_ExpectAndReturn(mcu_tick);
    

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);   
    bool ret = drv_amplifier_process();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
    m_test_amplifier_is_mute = false;
}

/*******************************************************************************
 * Private Functions Definitions
 ******************************************************************************/
static bool test_bsp_i2c_write_read_cb_tw_stub(uint32_t port, 
                                              uint8_t* p_rx_data, 
                                              size_t rx_size, 
                                              const uint8_t* p_tx_data, 
                                              size_t tx_size, 
                                              int cmock_num_calls)
{
    (void)port;
    (void)rx_size;
    (void)tx_size;
    (void)p_tx_data;

    if (0 == cmock_num_calls)
    {
        p_rx_data[0U] |= TEST_DIAG_INPUT_OFFSET_POS;
        p_rx_data[0U] |= TEST_DIAG_OVERCURR_PROTECT_POS;
        p_rx_data[0U] |= TEST_DIAG_CLIPPING_DETECT_POS;
        p_rx_data[1U] |= TEST_DIAG_THERMAL_SD_POS;
        p_rx_data[2U] |= TEST_DIAG_ANALOG_MUTE_POS;
        p_rx_data[2U] |= TEST_DIAG_VOV_SD_POS;
        p_rx_data[3U] |= TEST_DIAG_OUTCURR_OFFSET_POS;
        p_rx_data[3U] |= TEST_DIAG_DATA_VALID_POS;
        p_rx_data[3U] |= TEST_DIAG_PLAY_POS;

        p_rx_data[0U] |= TEST_ERROR_SHORT_TO_VCC_POS;
        p_rx_data[0U] |= TEST_ERROR_SHORT_TO_GND_POS;
        p_rx_data[0U] |= TEST_ERROR_WDG_POS;
        p_rx_data[0U] |= TEST_ERROR_FRAME_CHECK_POS;

        p_rx_data[1U] |= TEST_ERROR_TW_1_POS;
        p_rx_data[1U] &= ~TEST_ERROR_TW_4_POS;

        p_rx_data[2U] |= TEST_ERROR_UVLO_POS;
        p_rx_data[2U] |= TEST_ERROR_LOW_BATT_POS;
        p_rx_data[2U] |= TEST_ERROR_HIGH_BATT_POS;
        p_rx_data[3U] |= TEST_ERROR_SHORT_LOAD_POS;
        p_rx_data[3U] |= TEST_ERROR_OPEN_LOAD_POS;

    }
    else if(1 == cmock_num_calls)
    {
        m_test_thermal_warning_control = true;
        
        p_rx_data[0U] &= ~TEST_DIAG_INPUT_OFFSET_POS;
        p_rx_data[0U] &= ~TEST_DIAG_OVERCURR_PROTECT_POS;
        p_rx_data[0U] &= ~TEST_DIAG_CLIPPING_DETECT_POS;
        p_rx_data[1U] &= ~TEST_DIAG_THERMAL_SD_POS;
        p_rx_data[2U] &= ~TEST_DIAG_ANALOG_MUTE_POS;
        p_rx_data[2U] &= ~TEST_DIAG_VOV_SD_POS;
        p_rx_data[3U] &= ~TEST_DIAG_OUTCURR_OFFSET_POS;
        p_rx_data[3U] |=  TEST_DIAG_DATA_VALID_POS;
        p_rx_data[3U] &= ~TEST_DIAG_PLAY_POS;
        
        p_rx_data[0U] &= ~TEST_ERROR_SHORT_TO_VCC_POS;
        p_rx_data[0U] &= ~TEST_ERROR_SHORT_TO_GND_POS;
        p_rx_data[0U] &= ~TEST_ERROR_WDG_POS;
        p_rx_data[0U] &= ~TEST_ERROR_FRAME_CHECK_POS;
        p_rx_data[0U] |= TEST_ERROR_PLL_LOCKED_POS;

        // set rx_data for thermal warning 4 state
        p_rx_data[1U] &= ~TEST_ERROR_TW_1_POS;
        p_rx_data[1U] |= TEST_ERROR_TW_4_POS;
        p_rx_data[1U] &= ~TEST_ERROR_TW_2_POS;

        p_rx_data[2U] &= ~TEST_ERROR_UVLO_POS;
        p_rx_data[2U] &= ~TEST_ERROR_LOW_BATT_POS;
        p_rx_data[2U] &= ~TEST_ERROR_HIGH_BATT_POS;
        p_rx_data[3U] &= ~TEST_ERROR_SHORT_LOAD_POS;
        p_rx_data[3U] &= ~TEST_ERROR_OPEN_LOAD_POS;
    }
    else
    {
        return false;
    }

    return true;
}

static void test_fda903s_i2c_port_error_cb(uint32_t port, 
                                           bsp_i2c_err_cb_t callback, 
                                           int cmock_num_calls)
{
    (void)cmock_num_calls;

    if (0 == cmock_num_calls)
    {  
        TEST_ASSERT_EQUAL(TEST_BSP_I2C_AMP_1_PORT, port);

        m_test_fda903s_i2c_port_1_error_cb = callback;
    }
    else if (1 == cmock_num_calls)
    {
        TEST_ASSERT_EQUAL(TEST_BSP_I2C_AMP_2_PORT, port);

        m_test_fda903s_i2c_port_2_error_cb = callback;
    }
    else
    {
        return;
    }

    return;
}

static bool test_bsp_i2c_write_read_cb_stub(uint32_t port, 
                                            uint8_t* p_rx_data, 
                                            size_t rx_size, 
                                            const uint8_t* p_tx_data, 
                                            size_t tx_size, 
                                            int cmock_num_calls)
{
    (void)port;
    (void)rx_size;
    (void)tx_size;
    (void)p_tx_data;

    if (0 == cmock_num_calls)
    {
        // set rx_data for amplifier_diag_cb function
        p_rx_data[0U] |= TEST_DIAG_INPUT_OFFSET_POS;
        p_rx_data[0U] |= TEST_DIAG_OVERCURR_PROTECT_POS;
        p_rx_data[0U] |= TEST_DIAG_CLIPPING_DETECT_POS;
        p_rx_data[1U] |= TEST_DIAG_THERMAL_SD_POS;
        p_rx_data[2U] |= TEST_DIAG_ANALOG_MUTE_POS;
        p_rx_data[2U] |= TEST_DIAG_VOV_SD_POS;
        p_rx_data[3U] |= TEST_DIAG_OUTCURR_OFFSET_POS;
        p_rx_data[3U] |= TEST_DIAG_DATA_VALID_POS;
        p_rx_data[3U] |= TEST_DIAG_PLAY_POS;

        // set rx_data for amplifier_error_cb function 
        p_rx_data[0U] |= TEST_ERROR_SHORT_TO_VCC_POS;
        p_rx_data[0U] |= TEST_ERROR_SHORT_TO_GND_POS;
        p_rx_data[0U] |= TEST_ERROR_WDG_POS;
        p_rx_data[0U] |= TEST_ERROR_FRAME_CHECK_POS;

        // set rx_data for thermal_warning_control function
        p_rx_data[1U] |= TEST_ERROR_TW_1_POS;
        p_rx_data[1U] &= ~TEST_ERROR_TW_4_POS;

        p_rx_data[2U] |= TEST_ERROR_UVLO_POS;
        p_rx_data[2U] |= TEST_ERROR_LOW_BATT_POS;
        p_rx_data[2U] |= TEST_ERROR_HIGH_BATT_POS;
        p_rx_data[3U] |= TEST_ERROR_SHORT_LOAD_POS;
        p_rx_data[3U] |= TEST_ERROR_OPEN_LOAD_POS;

    }
    else if(1 == cmock_num_calls)
    {
        m_test_thermal_warning_control = true;
        
        // set rx_data for amplifier_diag_cb function
        p_rx_data[0U] &= ~TEST_DIAG_INPUT_OFFSET_POS;
        p_rx_data[0U] &= ~TEST_DIAG_OVERCURR_PROTECT_POS;
        p_rx_data[0U] &= ~TEST_DIAG_CLIPPING_DETECT_POS;
        p_rx_data[1U] &= ~TEST_DIAG_THERMAL_SD_POS;
        p_rx_data[2U] &= ~TEST_DIAG_ANALOG_MUTE_POS;
        p_rx_data[2U] &= ~TEST_DIAG_VOV_SD_POS;
        p_rx_data[3U] &= ~TEST_DIAG_OUTCURR_OFFSET_POS;
        p_rx_data[3U] |=  TEST_DIAG_DATA_VALID_POS;
        p_rx_data[3U] &= ~TEST_DIAG_PLAY_POS;
        

        // set rx_data for amplifier_error_cb function 
        p_rx_data[0U] &= ~TEST_ERROR_SHORT_TO_VCC_POS;
        p_rx_data[0U] &= ~TEST_ERROR_SHORT_TO_GND_POS;
        p_rx_data[0U] &= ~TEST_ERROR_WDG_POS;
        p_rx_data[0U] &= ~TEST_ERROR_FRAME_CHECK_POS;
        p_rx_data[0U] |= TEST_ERROR_PLL_LOCKED_POS;

        // set rx_data for thermal_warning_control function
        p_rx_data[1U] &= ~TEST_ERROR_TW_1_POS;
        p_rx_data[1U] |= TEST_ERROR_TW_4_POS;
        p_rx_data[1U] |= TEST_ERROR_TW_2_POS;

        p_rx_data[2U] &= ~TEST_ERROR_UVLO_POS;
        p_rx_data[2U] &= ~TEST_ERROR_LOW_BATT_POS;
        p_rx_data[2U] &= ~TEST_ERROR_HIGH_BATT_POS;
        p_rx_data[3U] &= ~TEST_ERROR_SHORT_LOAD_POS;
        p_rx_data[3U] &= ~TEST_ERROR_OPEN_LOAD_POS;
    }
    else
    {
        return false;
    }

    return true;
}

static bool test_bsp_i2c_write_stub(uint32_t port, 
                                    const uint8_t* p_data, 
                                    size_t size, 
                                    int cmock_num_calls)
{
    (void)size;

    if(true == m_test_amplifier_is_mute)
    {
        m_test_instruction_bytes[TEST_IB11_ADDR] |= 
            (uint8_t)TEST_AMPLIFIER_DIAG_START_BIT_1;

        m_test_instruction_bytes[TEST_IB11_ADDR] &= 
            (uint8_t)~TEST_AMPLIFIER_DIAG_START_BIT_2;

        m_test_instruction_bytes[20U] = TEST_DEFAULT_VALUE_OF_IB20;
        m_test_instruction_bytes[22U] = TEST_DEFAULT_VALUE_OF_IB22;
    }
    if(false == m_test_amplifier_is_mute)
    {
        m_test_instruction_bytes[TEST_IB11_ADDR] &= 
            (uint8_t)~TEST_AMPLIFIER_DIAG_START_BIT_1;

        m_test_instruction_bytes[TEST_IB11_ADDR] |= 
            (uint8_t)TEST_AMPLIFIER_DIAG_START_BIT_2;

        m_test_instruction_bytes[20U] = (uint8_t)TEST_DC_DIAG_IN_PLAY_FOR_IB20;
        m_test_instruction_bytes[22U] = (uint8_t)TEST_DC_DIAG_IN_PLAY_FOR_IB22;
    }

    if (0 == cmock_num_calls)
    {
        TEST_ASSERT_EQUAL(BSP_I2C_AMP_1_PORT, port);
        TEST_ASSERT_EQUAL(m_test_instruction_bytes[TEST_IB11_ADDR] & 
            (0x01U << 1U), p_data[TEST_IB11_ADDR] & (0x01U << 1U));

        TEST_ASSERT_EQUAL(m_test_instruction_bytes[TEST_IB11_ADDR] & 
            (0x01U << 2U), p_data[TEST_IB11_ADDR] & (0x01U << 2U));

        TEST_ASSERT_EQUAL(m_test_instruction_bytes[20U], 
                          p_data[20U]);
        TEST_ASSERT_EQUAL(m_test_instruction_bytes[22U], 
                          p_data[22U]);
    }
    else if(1 == cmock_num_calls)
    {
        TEST_ASSERT_EQUAL(BSP_I2C_AMP_2_PORT, port);
        TEST_ASSERT_EQUAL(m_test_instruction_bytes[TEST_IB11_ADDR] & 
            (0x01U << 1U), p_data[TEST_IB11_ADDR] & (0x01U << 1U));

        TEST_ASSERT_EQUAL(m_test_instruction_bytes[TEST_IB11_ADDR] & 
            (0x01U << 2U), p_data[TEST_IB11_ADDR] & (0x01U << 2U));

        TEST_ASSERT_EQUAL(m_test_instruction_bytes[20U], 
                          p_data[20U]);
        TEST_ASSERT_EQUAL(m_test_instruction_bytes[22U], 
                          p_data[22U]);
    }
    else
    {
        return false;
    }

    return true;
}

static bool test_bsp_i2c_write_read_stub(uint32_t port, 
                                         uint8_t* p_rx_data, 
                                         size_t rx_size, 
                                         const uint8_t* p_tx_data, 
                                         size_t tx_size, 
                                         int cmock_num_calls)
{
    if (0 == cmock_num_calls)
    {
        TEST_ASSERT_EQUAL(BSP_I2C_AMP_1_PORT, port);
        TEST_ASSERT_EQUAL(0U, p_rx_data[0U]);
        TEST_ASSERT_EQUAL(TEST_DB_SIZE, rx_size);
        TEST_ASSERT_EQUAL(0xA0, *p_tx_data);
        TEST_ASSERT_EQUAL(1U, tx_size);
    }
    else if(1 == cmock_num_calls)
    {
        TEST_ASSERT_EQUAL(BSP_I2C_AMP_2_PORT, port);
        TEST_ASSERT_EQUAL(0U, p_rx_data[0U]);
        TEST_ASSERT_EQUAL(TEST_DB_SIZE, rx_size);
        TEST_ASSERT_EQUAL(0xA0, *p_tx_data);
        TEST_ASSERT_EQUAL(1U, tx_size);
    }
    else
    {
        return false;
    }

    return true;
}

static void helper_drv_amplifier_init(void)
{
    bsp_i2c_cb_set_ExpectAnyArgs();

    bsp_i2c_cb_set_ExpectAnyArgs();

    bsp_i2c_write_ExpectAnyArgsAndReturn(true);

    bsp_i2c_write_ExpectAnyArgsAndReturn(true);
  
    (void)drv_amplifier_init();
}

static void helper_drv_amplifier_mute(void)
{
    bsp_i2c_write_ExpectAnyArgsAndReturn(true);
    bsp_i2c_write_ExpectAnyArgsAndReturn(true); 
    (void)drv_amplifier_mute();
}

static bool test_bsp_i2c_write_mute_stub(uint32_t port, 
                                           const uint8_t* p_data, 
                                           size_t size, 
                                           int cmock_num_calls)
{
    m_test_instruction_bytes[TEST_IB11_ADDR] &= 
        (uint8_t)~TEST_AMPLIFIER_FDA903S_MUTE_UNMUTE;

    uint8_t m_mute_data[2U] = {TEST_IB11_ADDR, 0U};

    m_mute_data[1U] = m_test_instruction_bytes[TEST_IB11_ADDR];

    if (0 == cmock_num_calls)
    {
        TEST_ASSERT_EQUAL(BSP_I2C_AMP_1_PORT, port);
        TEST_ASSERT_EQUAL(m_mute_data[0U], p_data[0U]);
        TEST_ASSERT_EQUAL(m_mute_data[1U] & 0x01U, (p_data[1U] & 0x01U));
        TEST_ASSERT_EQUAL(TEST_MUTE_UNMUTE_DATA_SIZE, size);
    }
    else if(1 == cmock_num_calls)
    {
        TEST_ASSERT_EQUAL(BSP_I2C_AMP_2_PORT, port);
        TEST_ASSERT_EQUAL(m_mute_data[0U], p_data[0U]);
        TEST_ASSERT_EQUAL(m_mute_data[1U] & 0x01U, (p_data[1U] & 0x01U));
        TEST_ASSERT_EQUAL(TEST_MUTE_UNMUTE_DATA_SIZE, size);
    }
    else
    {
        return false;
    }

    return true;
}

static void test_drv_amplifier_diag_emty_cb(drv_amplifier_diag_t diagnostic)
{
    (void)diagnostic;
}

static void test_drv_amplifier_diag_tw_cb(drv_amplifier_diag_t diagnostic)
{
    m_test_func_cnt++;

    if(21U == m_test_func_cnt)
    {
        TEST_ASSERT_EQUAL(DRV_AMPLIFIER_DIAG_ERR_TW_3, diagnostic);
    }       
}

static void test_drv_amplifier_diag_cb(drv_amplifier_diag_t diagnostic)
{
    m_test_func_cnt++;

    if(1U == m_test_func_cnt)
    {
        TEST_ASSERT_EQUAL(DRV_AMPLIFIER_DIAG_INPUT_OFFSET, diagnostic);
    }
    else if(2U == m_test_func_cnt)
    {
        TEST_ASSERT_EQUAL(DRV_AMPLIFIER_DIAG_OVERCURR_PROTECT, diagnostic);
    }
    else if(3U == m_test_func_cnt)
    {
        TEST_ASSERT_EQUAL(DRV_AMPLIFIER_DIAG_CLIPPING_DETECT, diagnostic);
    }
    else if(4U == m_test_func_cnt)
    {
        TEST_ASSERT_EQUAL(DRV_AMPLIFIER_DIAG_THERMAL_SD, diagnostic);
    }
    else if(5U == m_test_func_cnt)
    {
        TEST_ASSERT_EQUAL(DRV_AMPLIFIER_DIAG_ANALOG_MUTE, diagnostic);
    }
    else if(6U == m_test_func_cnt)
    {
        TEST_ASSERT_EQUAL(DRV_AMPLIFIER_DIAG_VOV_SD, diagnostic);
    }
    else if(7U == m_test_func_cnt)
    {
        TEST_ASSERT_EQUAL(DRV_AMPLIFIER_DIAG_OUTCURR_OFFSET, diagnostic);
    }
    else if(8U == m_test_func_cnt)
    {
        TEST_ASSERT_EQUAL(DRV_AMPLIFIER_DIAG_DATA_VALID, diagnostic);
    }
    else if(9U == m_test_func_cnt)
    {
        TEST_ASSERT_EQUAL(DRV_AMPLIFIER_DIAG_PLAY, diagnostic);
    }
    else if(10U == m_test_func_cnt)
    {
        TEST_ASSERT_EQUAL(DRV_AMPLIFIER_DIAG_ERR_SHORT_TO_VCC, diagnostic);
    }
    else if(11U == m_test_func_cnt)
    {
        TEST_ASSERT_EQUAL(DRV_AMPLIFIER_DIAG_ERR_SHORT_TO_GND, diagnostic);
    }
    else if(12U == m_test_func_cnt)
    {
        TEST_ASSERT_EQUAL(DRV_AMPLIFIER_DIAG_ERR_COMMUNICATION, diagnostic);
    }
    else if(13U == m_test_func_cnt)
    {
        TEST_ASSERT_EQUAL(DRV_AMPLIFIER_DIAG_ERR_TW_1, diagnostic);
    }
    else if(14U == m_test_func_cnt)
    {
        TEST_ASSERT_EQUAL(DRV_AMPLIFIER_DIAG_ERR_TW_4, diagnostic);
    }
    else if(15U == m_test_func_cnt)
    {
        TEST_ASSERT_EQUAL(DRV_AMPLIFIER_DIAG_ERR_UVLO, diagnostic);
    }
    else if(16U == m_test_func_cnt)
    {
        TEST_ASSERT_EQUAL(DRV_AMPLIFIER_DIAG_ERR_LOW_BATT, diagnostic);
    }
    else if(17U == m_test_func_cnt)
    {
        TEST_ASSERT_EQUAL(DRV_AMPLIFIER_DIAG_ERR_HIGH_BATT, diagnostic);
    }
    else if(18U == m_test_func_cnt)
    {
        TEST_ASSERT_EQUAL(DRV_AMPLIFIER_DIAG_ERR_SHORT_LOAD, diagnostic);
    }
    else if(19U == m_test_func_cnt)
    {
        TEST_ASSERT_EQUAL(DRV_AMPLIFIER_DIAG_ERR_OPEN_LOAD, diagnostic);
    }
    else if (true == m_test_thermal_warning_control)
    {
        if(20U == m_test_func_cnt)
        {
            TEST_ASSERT_EQUAL(DRV_AMPLIFIER_DIAG_DATA_VALID, diagnostic);
        }
        else if(21U == m_test_func_cnt)
        {
            TEST_ASSERT_EQUAL(DRV_AMPLIFIER_DIAG_ERR_TW_2, diagnostic);
        }       
    }
}

static bool test_bsp_i2c_write_unmute_stub(uint32_t port, 
                                           const uint8_t* p_data, 
                                           size_t size, 
                                           int cmock_num_calls)
{
    m_test_instruction_bytes[TEST_IB11_ADDR] |= 
        TEST_AMPLIFIER_FDA903S_MUTE_UNMUTE;

    uint8_t m_unmute_data [2U] = {TEST_IB11_ADDR, 0U};

    m_unmute_data[1U] = m_test_instruction_bytes[TEST_IB11_ADDR];

    if (0 == cmock_num_calls)
    {
        TEST_ASSERT_EQUAL(BSP_I2C_AMP_1_PORT, port);
        TEST_ASSERT_EQUAL(m_unmute_data[0U], p_data[0U]);
        TEST_ASSERT_EQUAL(m_unmute_data[1U] & 0x01U, (p_data[1U] & 0x01U));
        TEST_ASSERT_EQUAL(TEST_MUTE_UNMUTE_DATA_SIZE, size);

    }
    else if(1 == cmock_num_calls)
    {
        TEST_ASSERT_EQUAL(BSP_I2C_AMP_2_PORT, port);
        TEST_ASSERT_EQUAL(m_unmute_data[0U], p_data[0U]);
        TEST_ASSERT_EQUAL(m_unmute_data[1U] & 0x01U, (p_data[1U] & 0x01U));
        TEST_ASSERT_EQUAL(TEST_MUTE_UNMUTE_DATA_SIZE, size);
    }
    else
    {
        return false;
    }

    return true;
}

// end of file
