/**
 * @file test_canif_transceiver.c
 * @author Muhammed Salih AYDOGAN
 * @brief These tests verify the units of the CANIF TRANSCEIVER module.
 * 
 * @version 0.1
 * 
 * @date 28/06/2023
 * 
 * @copyright Copyright (c) 2023
 * 
 * https://codebeamer.daiichi.com/issue/197441
 * 
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

//First - include test framework headers
#include "unity.h"
#include "support.h"

// Second - include the module to be tested
#include "canif.h"
#include "canif_cfg.c"

// Third - include the mocks of required modules
#include "mock_bsp_gpio.h"
#include "mock_bsp_can.h"

#include "mock_log.h"

/*******************************************************************************
 * Test Files
 ******************************************************************************/

TEST_FILE("canif_transceiver.c");

/*******************************************************************************
 * Defines
 ******************************************************************************/
/**
 * @brief This macro holds port number and pin number info of relevant pin.
 * 
 * https://codebeamer.daiichi.com/issue/180119
 * 
 */
#define TEST_BSP_GPIO_CAN_EN_OUT_PIN_NO          316U

/**
 * @brief This macro holds port number and pin number info of relevant pin.
 * 
 * https://codebeamer.daiichi.com/issue/180118
 * 
 */
#define TEST_BSP_GPIO_CAN_STB_OUT_PIN_NO         45U

/**
 * @brief This macro indicates the local wake-up input of the CAN transceiver.
 * 
 * https://codebeamer.daiichi.com/issue/190354
 * 
 */
#define TEST_BSP_GPIO_CAN_WAKE_OUT_PIN_NO        58U

/*******************************************************************************
 * Private Functions Prototype Declarations
 ******************************************************************************/
/**
 * @brief This helper function initialize canif module.
 * 
 */
static void helper_canif_init(void);

/**
 * @brief This helper function deinitialize canif module.
 * 
 */
static void helper_canif_deinit(void);

/**
 * @brief This helper function sets canif module to standby.
 * 
 */
static void helper_standby_mode(void);

/**
 * @brief This helper function sets canif module to listenonly.
 * 
 */
static void helper_listenonly_mode(void);

/**
 * @brief This helper function sets canif module to normal.
 * 
 */
static void helper_normal_mode(void);

/**
 * @brief This helper function sets canif module to gotosleep.
 * 
 */
static void helper_gotosleep_mode(void);

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
 * @brief Verifies the canif_trcv_mode_set function when the transceiver mode 
 * request has been accepted.
 * 
 * https://codebeamer.daiichi.com/issue/197478
 *  
 */
void test_canif_trcv_mode_set__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    can_trcv_mode_t trcv_mode_req = CAN_TRCV_MODE_NORMAL;

    // **************************** Pre-Actions ***************************** //

    // Transceiver current mode is CAN_TRCV_MODE_SLEEP
    helper_canif_init();

    // ************************** Expected Calls **************************** //

    bsp_gpio_set_Expect(TEST_BSP_GPIO_CAN_STB_OUT_PIN_NO);

    bsp_gpio_set_Expect(TEST_BSP_GPIO_CAN_EN_OUT_PIN_NO);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_trcv_mode_set(trcv_mode_req);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_OK, ret);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(CAN_TRCV_MODE_NORMAL, g_transceiver_cfg.trcv_current_mode);
}

/**
 * @brief Verifies the canif_trcv_mode_set function when the current transceiver 
 * mode is SLEEP request mode is STANDBY.
 * 
 * https://codebeamer.daiichi.com/item/197479
 *  
 */
void test_canif_trcv_mode_set__sleep_to_standby__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    can_trcv_mode_t trcv_mode_req = CAN_TRCV_MODE_STANDBY;

    // **************************** Pre-Actions ***************************** //

    // Transceiver current mode is CAN_TRCV_MODE_SLEEP
    helper_canif_init();

    // ************************** Expected Calls **************************** //

    bsp_gpio_reset_Expect(TEST_BSP_GPIO_CAN_STB_OUT_PIN_NO);

    bsp_gpio_set_Expect(TEST_BSP_GPIO_CAN_WAKE_OUT_PIN_NO);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_trcv_mode_set(trcv_mode_req);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_NOT_OK, ret);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(CAN_TRCV_MODE_STANDBY, g_transceiver_cfg.trcv_current_mode);
}

/**
 * @brief Verifies the canif_trcv_mode_set function when the current transceiver 
 * mode is STANDBY, the request mode is SLEEP.
 * 
 * https://codebeamer.daiichi.com/item/197480
 *  
 */
void test_canif_trcv_mode_set__sleep_mode__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    can_trcv_mode_t trcv_mode_req = CAN_TRCV_MODE_SLEEP;

    // **************************** Pre-Actions ***************************** //

    // Transceiver current mode is CAN_TRCV_MODE_SLEEP
    helper_canif_init();

    // Transceiver current mode changes to CAN_TRCV_MODE_STANDBY
    g_transceiver_cfg.trcv_current_mode = CAN_TRCV_MODE_STANDBY;

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_trcv_mode_set(trcv_mode_req);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_NOT_OK, ret);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(CAN_TRCV_MODE_STANDBY, g_transceiver_cfg.trcv_current_mode);
}

/**
 * @brief Verifies the canif_trcv_mode_set function when the current transceiver 
 * mode is LISTEN ONLY, the request mode is SLEEP.
 * 
 * https://codebeamer.daiichi.com/item/197481
 *  
 */
void test_canif_trcv_mode_set__listen_only__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    can_trcv_mode_t trcv_mode_req = CAN_TRCV_MODE_SLEEP;

    // **************************** Pre-Actions ***************************** //

    // Transceiver current mode is CAN_TRCV_MODE_SLEEP
    helper_canif_init();

    // Transceiver current mode changes to CAN_TRCV_MODE_LISTENONLY
    g_transceiver_cfg.trcv_current_mode = CAN_TRCV_MODE_LISTENONLY;

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_trcv_mode_set(trcv_mode_req);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_NOT_OK, ret);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(CAN_TRCV_MODE_LISTENONLY, 
                      g_transceiver_cfg.trcv_current_mode);
}

/**
 * @brief Verifies the canif_trcv_mode_set function when the current transceiver 
 * mode is NORMAL, request mode is SLEEP.
 * 
 * https://codebeamer.daiichi.com/item/197482
 *  
 */
void test_canif_trcv_mode_set__normal_mode__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    can_trcv_mode_t trcv_mode_req = CAN_TRCV_MODE_SLEEP;

    // **************************** Pre-Actions ***************************** //

    // Transceiver current mode is CAN_TRCV_MODE_SLEEP
    helper_canif_init();

    // Transceiver current mode changes to CAN_TRCV_MODE_NORMAL
    g_transceiver_cfg.trcv_current_mode = CAN_TRCV_MODE_NORMAL;

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_trcv_mode_set(trcv_mode_req);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_NOT_OK, ret);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(CAN_TRCV_MODE_NORMAL, 
                      g_transceiver_cfg.trcv_current_mode);
}

/**
 * @brief Verifies the canif_trcv_mode_set function when the current transceiver 
 * mode is GOTOSLEEP, and the request mode is SLEEP.
 * 
 * https://codebeamer.daiichi.com/item/197487
 *  
 */
void test_canif_trcv_mode_set__gotosleep_mode__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    can_trcv_mode_t trcv_mode_req = CAN_TRCV_MODE_SLEEP;

    // **************************** Pre-Actions ***************************** //

    // Transceiver current mode is CAN_TRCV_MODE_SLEEP
    helper_canif_init();

    // Transceiver current mode changes to CAN_TRCV_MODE_GOTOSLEEP
    g_transceiver_cfg.trcv_current_mode = CAN_TRCV_MODE_GOTOSLEEP;

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_trcv_mode_set(trcv_mode_req);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_NOT_OK, ret);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(CAN_TRCV_MODE_GOTOSLEEP, 
                      g_transceiver_cfg.trcv_current_mode);
}

/**
 * @brief Verifies the canif_trcv_mode_set function when the current transceiver 
 * mode is LISTENONLY, the request mode is NORMAL.
 * 
 * https://codebeamer.daiichi.com/issue/197492
 *  
 */
void test_canif_trcv_mode_set__normal_mode__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    can_trcv_mode_t trcv_mode_req = CAN_TRCV_MODE_NORMAL;

    // **************************** Pre-Actions ***************************** //

    // Transceiver current mode is CAN_TRCV_MODE_SLEEP
    helper_canif_init();

    // Transceiver current mode changes to CAN_TRCV_MODE_LISTENONLY
    g_transceiver_cfg.trcv_current_mode = CAN_TRCV_MODE_LISTENONLY;

    // ************************** Expected Calls **************************** //

    bsp_gpio_set_Ignore();

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_trcv_mode_set(trcv_mode_req);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_OK, ret);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(CAN_TRCV_MODE_NORMAL, 
                      g_transceiver_cfg.trcv_current_mode);
}

/**
 * @brief Verifies the canif_trcv_mode_set function when the current transceiver 
 * mode is NORMAL, the request mode is NORMAL.
 * 
 * https://codebeamer.daiichi.com/item/197493
 *  
 */
void test_canif_trcv_mode_set__normal_modes__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    can_trcv_mode_t trcv_mode_req = CAN_TRCV_MODE_NORMAL;

    // **************************** Pre-Actions ***************************** //

    // Transceiver current mode is CAN_TRCV_MODE_SLEEP
    helper_canif_init();

    // Transceiver current mode changes to CAN_TRCV_MODE_NORMAL
    g_transceiver_cfg.trcv_current_mode = CAN_TRCV_MODE_NORMAL;

    // ************************** Expected Calls **************************** //

    bsp_gpio_set_Ignore();

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_trcv_mode_set(trcv_mode_req);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_NOT_OK, ret);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(CAN_TRCV_MODE_NORMAL, 
                      g_transceiver_cfg.trcv_current_mode);
}

/**
 * @brief Verifies the canif_trcv_mode_set function when the current transceiver 
 * mode is LISTENONLY, the request mode is LISTENONLY.
 * 
 * https://codebeamer.daiichi.com/item/197496
 *  
 */
void test_canif_trcv_mode_set__listenonly_modes__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    can_trcv_mode_t trcv_mode_req = CAN_TRCV_MODE_LISTENONLY;

    // **************************** Pre-Actions ***************************** //

    // Transceiver current mode is CAN_TRCV_MODE_SLEEP
    helper_canif_init();

    // Transceiver current mode changes to CAN_TRCV_MODE_LISTENONLY
    g_transceiver_cfg.trcv_current_mode = CAN_TRCV_MODE_LISTENONLY;

    // ************************** Expected Calls **************************** //

    bsp_gpio_set_Ignore();

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_trcv_mode_set(trcv_mode_req);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_NOT_OK, ret);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(CAN_TRCV_MODE_LISTENONLY, 
                      g_transceiver_cfg.trcv_current_mode);
}

/**
 * @brief Verifies the canif_trcv_mode_set function when the current transceiver 
 * mode is NORMAL, request mode is LISTENONLY.
 * 
 * https://codebeamer.daiichi.com/item/197500
 *  
 */
void test_canif_trcv_mode_set__listenonly_mode__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    can_trcv_mode_t trcv_mode_req = CAN_TRCV_MODE_LISTENONLY;

    // **************************** Pre-Actions ***************************** //

    // Transceiver current mode is CAN_TRCV_MODE_SLEEP
    helper_canif_init();

    // Transceiver current mode changes to CAN_TRCV_MODE_NORMAL
    g_transceiver_cfg.trcv_current_mode = CAN_TRCV_MODE_NORMAL;

    // ************************** Expected Calls **************************** //

    bsp_gpio_set_Expect(TEST_BSP_GPIO_CAN_STB_OUT_PIN_NO);

    bsp_gpio_reset_Expect(TEST_BSP_GPIO_CAN_EN_OUT_PIN_NO);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_trcv_mode_set(trcv_mode_req);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_OK, ret);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(CAN_TRCV_MODE_LISTENONLY, 
        g_transceiver_cfg.trcv_current_mode);
}

/**
 * @brief Verifies the canif_trcv_mode_set function when the current transceiver 
 * mode is STANDBY, request mode is GOTOSLEEP.
 * 
 * https://codebeamer.daiichi.com/item/197504
 *  
 */
void test_canif_trcv_mode_set__gotosleep_mode_and_standby_mode__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    can_trcv_mode_t trcv_mode_req = CAN_TRCV_MODE_GOTOSLEEP;

    // **************************** Pre-Actions ***************************** //

    // Transceiver current mode is CAN_TRCV_MODE_SLEEP
    helper_canif_init();

    // Transceiver current mode changes to CAN_TRCV_MODE_STANDBY
    g_transceiver_cfg.trcv_current_mode = CAN_TRCV_MODE_STANDBY;

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_trcv_mode_set(trcv_mode_req);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_NOT_OK, ret);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(CAN_TRCV_MODE_STANDBY, 
        g_transceiver_cfg.trcv_current_mode);
}

/**
 * @brief Verifies the canif_trcv_mode_set function when the current transceiver 
 * mode is SLEEP, the request mode is GOTOSLEEP.
 * 
 * https://codebeamer.daiichi.com/item/197505
 *  
 */
void test_canif_trcv_mode_set__gotosleep_mode_and_sleep_mode__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    can_trcv_mode_t trcv_mode_req = CAN_TRCV_MODE_GOTOSLEEP;

    // **************************** Pre-Actions ***************************** //

    // Transceiver current mode is CAN_TRCV_MODE_SLEEP
    helper_canif_init();

    // Transceiver current mode changes to CAN_TRCV_MODE_SLEEP
    g_transceiver_cfg.trcv_current_mode = CAN_TRCV_MODE_SLEEP;

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_trcv_mode_set(trcv_mode_req);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_NOT_OK, ret);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(CAN_TRCV_MODE_SLEEP, g_transceiver_cfg.trcv_current_mode);
}

/**
 * @brief Verifies the canif_trcv_mode_set function when the current transceiver 
 * mode is NORMAL, the request mode is GOTOSLEEP.
 * 
 * https://codebeamer.daiichi.com/item/197508
 *  
 */
void test_canif_trcv_mode_set__gotosleep_mode_and_normal_mode__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    can_trcv_mode_t trcv_mode_req = CAN_TRCV_MODE_GOTOSLEEP;

    // **************************** Pre-Actions ***************************** //

    // Transceiver current mode is CAN_TRCV_MODE_SLEEP
    helper_canif_init();

    // Transceiver current mode changes to CAN_TRCV_MODE_NORMAL
    g_transceiver_cfg.trcv_current_mode = CAN_TRCV_MODE_NORMAL;

    // ************************** Expected Calls **************************** //

    bsp_gpio_reset_Expect(TEST_BSP_GPIO_CAN_STB_OUT_PIN_NO);

    bsp_gpio_set_Expect(TEST_BSP_GPIO_CAN_EN_OUT_PIN_NO);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_trcv_mode_set(trcv_mode_req);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_OK, ret);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(CAN_TRCV_MODE_GOTOSLEEP, 
        g_transceiver_cfg.trcv_current_mode);
}

/**
 * @brief Verifies the canif_trcv_mode_set function when the current transceiver 
 * mode is NORMAL, the request mode is STANDBY.
 * 
 * https://codebeamer.daiichi.com/item/197512
 *  
 */
void test_canif_trcv_mode_set__standby_mode__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    can_trcv_mode_t trcv_mode_req = CAN_TRCV_MODE_STANDBY;

    // **************************** Pre-Actions ***************************** //

    // Transceiver current mode is CAN_TRCV_MODE_SLEEP
    helper_canif_init();

    // Transceiver current mode changes to CAN_TRCV_MODE_NORMAL
    g_transceiver_cfg.trcv_current_mode = CAN_TRCV_MODE_NORMAL;

    // ************************** Expected Calls **************************** //

    bsp_gpio_reset_Expect(TEST_BSP_GPIO_CAN_STB_OUT_PIN_NO);

    bsp_gpio_reset_Expect(TEST_BSP_GPIO_CAN_EN_OUT_PIN_NO);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_trcv_mode_set(trcv_mode_req);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_OK, ret);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(CAN_TRCV_MODE_STANDBY, 
        g_transceiver_cfg.trcv_current_mode);
}

/**
 * @brief Verifies the canif_trcv_mode_set function when the current transceiver 
 * mode is STANDBY, the request mode is STANDBY.
 * 
 * https://codebeamer.daiichi.com/item/197513
 *  
 */
void test_canif_trcv_mode_set__standby_to_standby__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    can_trcv_mode_t trcv_mode_req = CAN_TRCV_MODE_STANDBY;

    // **************************** Pre-Actions ***************************** //

    // Transceiver current mode is CAN_TRCV_MODE_SLEEP
    helper_canif_init();

    // Transceiver current mode changes to CAN_TRCV_MODE_STANDBY
    g_transceiver_cfg.trcv_current_mode = CAN_TRCV_MODE_STANDBY;

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_trcv_mode_set(trcv_mode_req);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_NOT_OK, ret);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(CAN_TRCV_MODE_STANDBY, 
        g_transceiver_cfg.trcv_current_mode);
}

/**
 * @brief Verifies the canif_trcv_mode_set function according to the canif 
 * initialize state. 
 * 
 * https://codebeamer.daiichi.com/item/197514
 *  
 */
void test_canif_trcv_mode_set__init_state__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    can_trcv_mode_t trcv_mode_req = CAN_TRCV_MODE_STANDBY;

    // **************************** Pre-Actions ***************************** //

    helper_canif_deinit();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_trcv_mode_set(trcv_mode_req);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_NOT_OK, ret);
}

/**
 * @brief Verifies canif_trcv_mode_get when the transceiver mode request has not 
 * been accepted. 
 * 
 * https://codebeamer.daiichi.com/item/197515
 *  
 */
void test_canif_trcv_mode_get__init_state__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    can_trcv_mode_t p_current_trcv_mode = CAN_TRCV_MODE_STANDBY;

    // **************************** Pre-Actions ***************************** //

    helper_canif_deinit();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_trcv_mode_get(&p_current_trcv_mode);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_NOT_OK, ret);
}

/**
 * @brief Verifies canif_trcv_mode_get when the transceiver mode request has 
 * been accepted.
 * 
 * https://codebeamer.daiichi.com/item/197516
 *  
 */
void test_canif_trcv_mode_get__trcv_mode__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    can_trcv_mode_t p_current_trcv_mode = CAN_TRCV_MODE_STANDBY;

    // **************************** Pre-Actions ***************************** //

    helper_canif_init();

    g_transceiver_cfg.trcv_current_mode = CAN_TRCV_MODE_NORMAL;

    helper_standby_mode();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_trcv_mode_get(&p_current_trcv_mode);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_OK, ret);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(CAN_TRCV_MODE_STANDBY, p_current_trcv_mode);
}

/**
 * @brief Verifies the canif_trcv_wakeup_reason_get function when transceiver 
 * current mode is CAN_TRCV_MODE_LISTENONLY.
 * 
 * https://codebeamer.daiichi.com/item/197517
 *  
 */
void test_canif_trcv_wakeup_reason_get__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    can_trcv_wakeup_reason_t p_trcv_wu_reason = CANTRCV_WU_NOT_SUPPORTED;

    // **************************** Pre-Actions ***************************** //

    helper_canif_init();

    g_transceiver_cfg.trcv_current_mode = CAN_TRCV_MODE_NORMAL;

    helper_listenonly_mode();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_trcv_wakeup_reason_get(&p_trcv_wu_reason);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_OK, ret);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(CANTRCV_WU_BY_PIN, p_trcv_wu_reason);
}
    
/**
 * @brief Verifies the canif_trcv_wakeup_reason_get function when transceiver 
 * current mode is CAN_TRCV_MODE_STANDBY. 
 * 
 * https://codebeamer.daiichi.com/item/197698
 *  
 */
void test_canif_trcv_wakeup_reason_get__trcv_mode_standby__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    can_trcv_wakeup_reason_t p_trcv_wu_reason = CANTRCV_WU_NOT_SUPPORTED;

    // **************************** Pre-Actions ***************************** //

    helper_canif_init();

    g_transceiver_cfg.trcv_current_mode = CAN_TRCV_MODE_NORMAL;

    helper_standby_mode();

    // ************************** Expected Calls **************************** //

    bsp_gpio_status_get_ExpectAndReturn(BSP_GPIO_CAN_ERROR_IN_PIN_NO, 
                                        BSP_GPIO_STATUS_RESET);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_trcv_wakeup_reason_get(&p_trcv_wu_reason);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_OK, ret);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(CANTRCV_WU_INTERNALLY, p_trcv_wu_reason);

    TEST_STEP(4);
    TEST_ASSERT_EQUAL(CAN_TRCV_INTERNAL_FLAG_SET, 
                      g_transceiver_cfg.trcv_flag_status);
}

/**
 * @brief Verifies the canif_trcv_wakeup_reason_get function when transceiver 
 * current mode is CAN_TRCV_MODE_STANDBY. 
 * 
 * https://codebeamer.daiichi.com/item/197700
 *  
 */
void test_canif_trcv_wakeup_reason_get__trcv_wu_by_bus__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    can_trcv_wakeup_reason_t p_trcv_wu_reason = CANTRCV_WU_NOT_SUPPORTED;

    // **************************** Pre-Actions ***************************** //

    helper_canif_init();

    g_transceiver_cfg.trcv_current_mode = CAN_TRCV_MODE_NORMAL;

    helper_standby_mode();

    // ************************** Expected Calls **************************** //

    bsp_gpio_status_get_ExpectAndReturn(BSP_GPIO_CAN_ERROR_IN_PIN_NO, 
                                        BSP_GPIO_STATUS_SET);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_trcv_wakeup_reason_get(&p_trcv_wu_reason);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_OK, ret);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(CANTRCV_WU_BY_BUS, p_trcv_wu_reason);

    TEST_STEP(4);
    TEST_ASSERT_EQUAL(CAN_TRCV_INTERNAL_FLAG_CLEARED, 
                      g_transceiver_cfg.trcv_flag_status);
}

/**
 * @brief Verifies the canif_trcv_wakeup_reason_get function when transceiver 
 * current mode is CAN_TRCV_MODE_GOTOSLEEP.
 * 
 * https://codebeamer.daiichi.com/item/197701
 *  
 */
void test_canif_trcv_wakeup_reason_get__trcv_wu_err__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    can_trcv_wakeup_reason_t p_trcv_wu_reason = CANTRCV_WU_NOT_SUPPORTED;

    // **************************** Pre-Actions ***************************** //

    helper_canif_init();

    g_transceiver_cfg.trcv_current_mode = CAN_TRCV_MODE_NORMAL;

    helper_gotosleep_mode();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    std_return_t ret = canif_trcv_wakeup_reason_get(&p_trcv_wu_reason);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(E_OK, ret);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(CANTRCV_WU_ERROR, p_trcv_wu_reason);
}

/**
 * @brief Verifies the canif_trcv_main_func_diagnostics function when transceiver 
 * current mode is CAN_TRCV_MODE_GOTOSLEEP.
 * 
 * Note: Error flag is equal to CAN_TRCV_WAKEUP_FLAG in this test.
 * 
 * https://codebeamer.daiichi.com/item/197703
 *  
 */
void test_canif_trcv_main_func_diagnostics__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_canif_init();

    g_transceiver_cfg.trcv_current_mode = CAN_TRCV_MODE_NORMAL;

    helper_gotosleep_mode();

    // ************************** Expected Calls **************************** //

    bsp_gpio_status_get_ExpectAndReturn(BSP_GPIO_CAN_ERROR_IN_PIN_NO, 
                                        BSP_GPIO_STATUS_RESET);

    // set CAN fault state  
    bsp_can_fault_state_t status = BSP_CAN_FAULT_STATE_ACTIVE;

    bsp_can_fault_state_get_ExpectAnyArgsAndReturn(BSP_CAN_STATUS_OK);
    bsp_can_fault_state_get_ReturnThruPtr_p_state(&status);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    canif_trcv_main_func_diagnostics();
}

/**
 * @brief Verifies the canif_trcv_main_func_diagnostics function when error flag
 * not readable. 
 * 
 * https://codebeamer.daiichi.com/item/197704
 *  
 */
void test_canif_trcv_main_func_diagnostics__trcv_wu_err__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_canif_init();

    g_transceiver_cfg.trcv_current_mode = CAN_TRCV_MODE_NORMAL;

    helper_standby_mode();

    // ************************** Expected Calls **************************** //

    bsp_gpio_status_get_ExpectAndReturn(BSP_GPIO_CAN_ERROR_IN_PIN_NO, 
                                        BSP_GPIO_STATUS_SET);

    // set CAN fault state  
    bsp_can_fault_state_t status = BSP_CAN_FAULT_STATE_ACTIVE;

    bsp_can_fault_state_get_ExpectAnyArgsAndReturn(BSP_CAN_STATUS_OK);
    bsp_can_fault_state_get_ReturnThruPtr_p_state(&status);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    canif_trcv_main_func_diagnostics();
}

/**
 * @brief Verifies the canif_trcv_main_func_diagnostics function when tranceiver 
 * current mode is CAN_TRCV_MODE_NORMAL.
 * 
 * Note: Error flag is equal to CAN_TRCV_BUS_FAILURE_FLAG in this test.
 * 
 * https://codebeamer.daiichi.com/item/197913
 *  
 */
void test_canif_trcv_main_func_diagnostics__trcv_normal_mode__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_canif_init();

    g_transceiver_cfg.trcv_current_mode = CAN_TRCV_MODE_LISTENONLY;

    helper_normal_mode();

    // ************************** Expected Calls **************************** //

    bsp_gpio_status_get_ExpectAndReturn(BSP_GPIO_CAN_ERROR_IN_PIN_NO, 
                                        BSP_GPIO_STATUS_RESET);

    bsp_can_fault_state_t status = BSP_CAN_FAULT_STATE_ACTIVE;

    bsp_can_fault_state_get_ExpectAnyArgsAndReturn(BSP_CAN_STATUS_OK);

    bsp_can_fault_state_get_ReturnThruPtr_p_state(&status);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    canif_trcv_main_func_diagnostics();
}

/**
 * @brief Verifies the canif_trcv_main_func_diagnostics function when tranceiver 
 * current mode is CAN_TRCV_MODE_LISTENONLY.
 * 
 * Note: Error flag is equal to CAN_TRCV_BUS_FAILURE_FLAG in this test.
 * 
 * https://codebeamer.daiichi.com/item/197914
 *  
 */
void test_canif_trcv_main_func_diagnostics__trcv_listenonly_mode__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_canif_init();

    g_transceiver_cfg.trcv_current_mode = CAN_TRCV_MODE_NORMAL;

    helper_listenonly_mode();

    // ************************** Expected Calls **************************** //

    bsp_gpio_status_get_ExpectAndReturn(BSP_GPIO_CAN_ERROR_IN_PIN_NO, 
                                        BSP_GPIO_STATUS_RESET);

    bsp_can_fault_state_t status = BSP_CAN_FAULT_STATE_ACTIVE;

    bsp_can_fault_state_get_ExpectAnyArgsAndReturn(BSP_CAN_STATUS_OK);

    bsp_can_fault_state_get_ReturnThruPtr_p_state(&status);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    canif_trcv_main_func_diagnostics();
}

/**
 * @brief Verifies the canif_trcv_main_func_diagnostics function when tranceiver 
 * old mode is CAN_TRCV_MODE_GOTOSLEEP.
 * 
 * Note: Error flag is equal to CAN_TRCV_PWON_FLAG in this test.
 * 
 * https://codebeamer.daiichi.com/item/197915
 *  
 */
void test_canif_trcv_main_func_diagnostics__trcv_old_mode__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_canif_init();

    g_transceiver_cfg.trcv_current_mode = CAN_TRCV_MODE_GOTOSLEEP;

    helper_gotosleep_mode();

    // ************************** Expected Calls **************************** //

    bsp_gpio_status_get_ExpectAndReturn(BSP_GPIO_CAN_ERROR_IN_PIN_NO, 
                                        BSP_GPIO_STATUS_RESET);

    bsp_can_fault_state_t status = BSP_CAN_FAULT_STATE_ACTIVE;

    bsp_can_fault_state_get_ExpectAnyArgsAndReturn(BSP_CAN_STATUS_OK);
    
    bsp_can_fault_state_get_ReturnThruPtr_p_state(&status);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    canif_trcv_main_func_diagnostics();
}

/**
 * @brief Verifies the canif_trcv_main_func_diagnostics function when the pin 
 * state equals a value out of the logic level.
 * 
 * https://codebeamer.daiichi.com/item/197916
 *  
 */
void test_canif_trcv_main_func_diagnostics__pin_state__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_canif_init();

    // ************************** Expected Calls **************************** //

    // set pin state to error
    bsp_gpio_status_get_ExpectAndReturn(BSP_GPIO_CAN_ERROR_IN_PIN_NO, 
                                        BSP_GPIO_STATUS_ERR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    canif_trcv_main_func_diagnostics();
}

/*******************************************************************************
 * Private Functions Definitions
 ******************************************************************************/

static void helper_canif_init(void)
{
    canif_tx_pdu_cfg_t test_tx_config;

    static canif_config_t config;
    config.rx_configuration = &canif_rx_pdu_cfg[0U];
    config.tx_configuration = &test_tx_config;
 
    bsp_can_init_ExpectAndReturn(true);
    bsp_gpio_set_Ignore();
    bsp_can_cb_set_ExpectAnyArgsAndReturn(BSP_CAN_STATUS_OK);

    (void)canif_init(&config);
}

static void helper_canif_deinit(void)
{
    bsp_can_deinit_ExpectAndReturn(true);

    (void)canif_deinit();
}

static void helper_standby_mode(void)
{
    can_trcv_mode_t trcv_mode_req = CAN_TRCV_MODE_STANDBY;

    bsp_gpio_reset_Expect(TEST_BSP_GPIO_CAN_STB_OUT_PIN_NO);

    bsp_gpio_reset_Expect(TEST_BSP_GPIO_CAN_EN_OUT_PIN_NO);

    // Transceiver current mode changes to CAN_TRCV_MODE_STANDBY
    (void)canif_trcv_mode_set(trcv_mode_req);
}

static void helper_listenonly_mode(void)
{
    can_trcv_mode_t trcv_mode_req = CAN_TRCV_MODE_LISTENONLY;

    bsp_gpio_set_Expect(TEST_BSP_GPIO_CAN_STB_OUT_PIN_NO);

    bsp_gpio_reset_Expect(TEST_BSP_GPIO_CAN_EN_OUT_PIN_NO);

    // Transceiver current mode changes to CAN_TRCV_MODE_LISTENONLY
    (void)canif_trcv_mode_set(trcv_mode_req);
}

static void helper_normal_mode(void)
{
    can_trcv_mode_t trcv_mode_req = CAN_TRCV_MODE_NORMAL;

    bsp_gpio_set_Ignore();

    // Transceiver current mode changes to CAN_TRCV_MODE_NORMAL
    (void)canif_trcv_mode_set(trcv_mode_req);
}

static void helper_gotosleep_mode(void)
{
    can_trcv_mode_t trcv_mode_req = CAN_TRCV_MODE_GOTOSLEEP;

    bsp_gpio_reset_Expect(TEST_BSP_GPIO_CAN_STB_OUT_PIN_NO);

    bsp_gpio_set_Expect(TEST_BSP_GPIO_CAN_EN_OUT_PIN_NO);

    // Transceiver current mode changes to CAN_TRCV_MODE_GOTOSLEEP
    (void)canif_trcv_mode_set(trcv_mode_req);
}
// end of file
