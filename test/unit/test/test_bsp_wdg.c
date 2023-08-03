/**
 * @file test_bsp_wdg.c
 * @author Muhammed Salih AYDOGAN
 * @brief These tests verify the units of the WDG module.
 * 
 * @version 0.1
 * 
 * @date 03/01/2023
 * 
 * @copyright Copyright (c) 2023
 * 
 * https://codebeamer.daiichi.com/issue/182279
 * 
 */

/*******************************************************************************
 * Includes 
 ******************************************************************************/

//First - include test framework headers
#include "unity.h"
#include "support.h"

// Second - include the module to be tested
#include "bsp_wdg.h"

// Third - include the mocks of required modules
#include "mock_interrupt_manager.h"
#include "mock_wdog_driver.h"

#include "mock_log.h"

/*******************************************************************************
 * Test Files
 ******************************************************************************/

TEST_FILE("bsp_wdg.c");

/*******************************************************************************             
 * Defines
 ******************************************************************************/
/**
 * @brief This macro specifies which wdg unit to use.
 * 
 */
#define TEST_WDG_INSTANCE				(0U)

/**
 * @brief This macro is used in wdg timeout calculation. 
 * 
 */
#define TEST_WDG_CFG_TIMEOUT_FACTOR		(2U)

/*******************************************************************************
 * Extern Variables
 ******************************************************************************/

/*******************************************************************************
 * Global Variables
 ******************************************************************************/

uint8_t allocated_area[PERIPH_MEM_SIZE];

/*******************************************************************************
 * Private Functions Prototype Declarations
 ******************************************************************************/
/**
 * @brief Callback for init function.
 * 
 */
static void test_cb(void);

/**
 * @brief Helper of wdg success init.
 * 
 */
static void helper_bsp_wdg_init_success(void);

/**
 * @brief Helper of wdg fail init.
 * 
 */
static void helper_bsp_wdg_init_fail(void);

/**
 * @brief This function tests as stub. 
 * 
 */
static void test_handler_stub(IRQn_Type irqNumber, 
                              const isr_t newHandler, 
                              isr_t* const oldHandler, 
                              int cmock_num_calls);

/*******************************************************************************
 * Private Variables
 ******************************************************************************/
/**
 * @brief WDG user configuration structure variable.
 * 
 */
static wdog_user_config_t m_test_wdg_user_cfg;

/**
 * @brief This function pointer uses new interrupt handler.
 * 
 */
static bsp_wdg_timeout_cb_t m_test_cb = test_cb;

/**
 * @brief This variable is used to get the new interrupt handler routine address 
 * pointer
 * 
 */
static isr_t m_test_new_handler;

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
 * @brief The test verifies the bsp_wdg_init() function by calling the 
 * INT_SYS_InstallHandler() and WDOG_DRV_Init() functions.
 * 
 * https://codebeamer.daiichi.com/issue/182465
 *  
 */
void test_bsp_wdg_init__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    uint16_t wdg_timeout_val = (uint16_t)(BSP_WDG_CFG_TIMEOUT_MSEC / 
                                          TEST_WDG_CFG_TIMEOUT_FACTOR);

    m_test_wdg_user_cfg.clkSource = WDOG_LPO_CLOCK;
    m_test_wdg_user_cfg.opMode.wait = false;
    m_test_wdg_user_cfg.opMode.stop = false;
    m_test_wdg_user_cfg.opMode.debug = false;
    m_test_wdg_user_cfg.intEnable = true;
    m_test_wdg_user_cfg.winEnable = false;
    m_test_wdg_user_cfg.windowValue = 0U;
    m_test_wdg_user_cfg.timeoutValue = wdg_timeout_val;
    m_test_wdg_user_cfg.prescalerEnable = true;

    INT_SYS_InstallHandler_Stub(test_handler_stub);

    WDOG_DRV_Init_ExpectWithArrayAndReturn((uint32_t)TEST_WDG_INSTANCE, 
                                            &m_test_wdg_user_cfg, 
                                            1U, 
                                            STATUS_SUCCESS);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_wdg_init(m_test_cb);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(m_test_cb, m_test_new_handler);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(true, ret);

    TEST_STEP(4);
    TEST_ASSERT_EQUAL(35U, RCM->SRIE);
}

/**
 * @brief The test verifies the bsp_wdg_init() function by using the wrong 
 * parameter.
 * 
 * https://codebeamer.daiichi.com/issue/182497
 *  
 */
void test_bsp_wdg_init__call_with_wrong_param__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    bsp_wdg_timeout_cb_t m_test_cb = NULL;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_wdg_init(m_test_cb);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the bsp_wdg_init() function by calling the 
 * WDOG_DRV_Init() function with a failed call.
 * 
 * https://codebeamer.daiichi.com/issue/182502
 *  
 */
void test_bsp_wdg_init__init_return_status__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    INT_SYS_InstallHandler_Expect(WDOG_IRQn, 
                                 (isr_t)m_test_cb, 
                                 (isr_t *)0U);

    WDOG_DRV_Init_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_wdg_init(m_test_cb);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the bsp_wdg_kick() function by calling the 
 * WDOG_DRV_Trigger() function.
 * 
 * https://codebeamer.daiichi.com/issue/182512
 *  
 */
void test_bsp_wdg_kick__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //
    
    helper_bsp_wdg_init_success();

    // ************************** Expected Calls **************************** //
    
    WDOG_DRV_Trigger_Expect(TEST_WDG_INSTANCE);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_wdg_kick();
}

/**
 * @brief The test verifies the bsp_wdg_kick() function when initializing 
 * bsp_wdg_init() with fail.
 * 
 * https://codebeamer.daiichi.com/issue/182516
 *  
 */
void test_bsp_wdg_kick__without_init__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_bsp_wdg_init_fail();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_wdg_kick();
}

/*******************************************************************************
 * Private Functions Definitions
 ******************************************************************************/
static void test_cb(void)
{
    TEST_MESSAGE("Callback function called!");
}

static void helper_bsp_wdg_init_success(void)
{
    // **************************** Input Params **************************** //

    // ************************** Expected Calls **************************** //

    uint16_t wdg_timeout_val = (uint16_t)(BSP_WDG_CFG_TIMEOUT_MSEC / 
                                          TEST_WDG_CFG_TIMEOUT_FACTOR);

    m_test_wdg_user_cfg.clkSource = WDOG_LPO_CLOCK;
    m_test_wdg_user_cfg.opMode.wait = false;
    m_test_wdg_user_cfg.opMode.stop = false;
    m_test_wdg_user_cfg.opMode.debug = false;
    m_test_wdg_user_cfg.intEnable = true;
    m_test_wdg_user_cfg.winEnable = false;
    m_test_wdg_user_cfg.windowValue = 0U;
    m_test_wdg_user_cfg.timeoutValue = wdg_timeout_val;
    m_test_wdg_user_cfg.prescalerEnable = true;

    INT_SYS_InstallHandler_Expect(WDOG_IRQn, 
                                 (isr_t)m_test_cb, 
                                 (isr_t *)0U);

    WDOG_DRV_Init_ExpectWithArrayAndReturn((uint32_t)TEST_WDG_INSTANCE, 
                                            &m_test_wdg_user_cfg, 
                                            1U, 
                                            STATUS_SUCCESS);

    // **************************** Test Steps ****************************** //

    (void)bsp_wdg_init(m_test_cb);
}

static void helper_bsp_wdg_init_fail(void)
{
    // **************************** Input Params **************************** //

    // ************************** Expected Calls **************************** //

    INT_SYS_InstallHandler_ExpectAnyArgs();
    WDOG_DRV_Init_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    (void)bsp_wdg_init(m_test_cb);
}

static void test_handler_stub(IRQn_Type irqNumber, 
                              const isr_t newHandler, 
                              isr_t* const oldHandler, 
                              int cmock_num_calls)
{
    TEST_ASSERT_EQUAL(WDOG_IRQn, irqNumber);
    m_test_new_handler = newHandler;
    TEST_ASSERT_EQUAL((isr_t *)0U, oldHandler);
    (void) cmock_num_calls;
}

// end of file 
