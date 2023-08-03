/**
 * @file test_bsp_timer.c
 * @author Muhammed Salih AYDOGAN
 * @brief These tests verify the units of the TIMER module.
 * 
 * @version 0.1
 * 
 * @date 15/02/2023
 * 
 * @copyright Copyright (c) 2023
 * 
 * https://codebeamer.daiichi.com/issue/183859
 * 
 */

/*******************************************************************************
 * Includes 
 ******************************************************************************/

//First - include test framework headers
#include "unity.h"
#include "support.h"

// Second - include the module to be tested
#include "bsp_timer.h"

// Third - include the mocks of required modules
#include "mock_clock.h"
#include "mock_lptmr_driver.h"
#include "mock_interrupt_manager.h"

#include "mock_log.h"

/*******************************************************************************
 * Test Files
 ******************************************************************************/

TEST_FILE("bsp_timer.c");

/*******************************************************************************             
 * Defines
 ******************************************************************************/
/**
 * @brief This macro specifies which LPTMR unit to use.
 * 
 */
#define TEST_LPTMR_INSTANCE          (0U)

/**
 * @brief This macro gives the number of cycles required to reach the desired 
 *        frequency value. Initially this value should be 1250 to reach 
 *        32khz frequency.
 */
#define TEST_TIMEOUT_VALUE_INIT      (1250U)

/**
 * @brief This macro should be used to indicate the dedicated timer port for the 
 *        DAC.
 */
#define TEST_BSP_TIMER_DAC_PORT      (0U)

/*******************************************************************************
 * Extern Variables
 ******************************************************************************/

/*******************************************************************************
 * Private Variables
 ******************************************************************************/
/**
 * @brief This variable specifies the frequency of the clock source used by the 
 *        LPTMR driver.
 * 
 */
static uint32_t m_test_lptmr_clk_freq = 1U;

/**
 * @brief Defines the configuration structure for LPTMR.
 * 
 */
static const lptmr_config_t m_test_lptmr_cfg = {

    .dmaRequest = false,
    .interruptEnable = true,
    .freeRun = false,
    .workMode = LPTMR_WORKMODE_TIMER,
    .clockSelect = LPTMR_CLOCKSOURCE_PCC,
    .prescaler = LPTMR_PRESCALE_2,
    .bypassPrescaler = true,
    .compareValue = TEST_TIMEOUT_VALUE_INIT,
    .counterUnits = LPTMR_COUNTER_UNITS_TICKS,
    .pinSelect = LPTMR_PINSELECT_TRGMUX,
    .pinPolarity = LPTMR_PINPOLARITY_RISING
};

/**
 * @brief Specifies the timeout value in terms of frequency.
 * 
 */
static uint16_t period_hz = 5U;

/**
 * @brief This variable is used to verify whether the callback function has been 
 * called.
 * 
 */
static bool is_test_state;

/*******************************************************************************
 * Global Variables
 ******************************************************************************/

/*******************************************************************************
 * Private Functions Prototype Declarations
 ******************************************************************************/

/**
 * @brief This callback function controls the configuration parameters.
 * 
 */
static void test_lptmr_drv_init_cb (const uint32_t instance,
                                    const lptmr_config_t * const config,
                                    const bool startCounter,
                                    int cmock_num_calls); 

/**
 * @brief This function starts the timer successfully or unsuccessfully 
 * according to the parameter it receives.
 * 
 */
static void helper_bsp_timer_init(status_t status);

/**
 * @brief This function starts the bsp_timer_stop() successfully or 
 * unsuccessfully according to the parameter it receives.
 * 
 */
static void helper_bsp_timer_stop(void);

/**
 * @brief This function is the handler function that will be called by driver.
 * 
 */
static void test_lptmr_handler(void);

/**
 * @brief The helper call bsp_timer_init() function with a function stub.
 * 
 */
static void helper_bsp_timer_init_with_stub(void);

/**
 * @brief The helper sets bsp_timer_cb_set() function with a successful 
 * call.
 * 
 */
static void helper_bsp_timer_cb_set(void);

/**
 * @brief New handler address pointer.
 * 
 */
static isr_t lptmr_handler = NULL;

/**
 * @brief Stub function to get new handler address.
 * 
 */
static void timer_stub_cb(IRQn_Type irqNumber, 
                          const isr_t newHandler, 
                          isr_t* const oldHandler, 
                          int cmock_num_calls);

/*******************************************************************************
 * Setup & Teardown 
 ******************************************************************************/

void setUp()
{
    log_print_Ignore();

    is_test_state = false;
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
 * @brief The test verifies the bsp_timer_init() function by calling the 
 * CLOCK_DRV_GetFreq function with a successful call and the LPTMR_DRV_Init, 
 * INT_SYS_InstallHandler, and INT_SYS_EnableIRQ function.
 * 
 * https://codebeamer.daiichi.com/issue/183867
 *  
 */
void test_bsp_timer_init__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //
    
    LPTMR_DRV_Init_AddCallback(test_lptmr_drv_init_cb);
    LPTMR_DRV_Init_ExpectWithArray(TEST_LPTMR_INSTANCE, 
                                   &m_test_lptmr_cfg, 
                                   11U, 
                                   true);
    LPTMR_DRV_Init_IgnoreArg_config();

    INT_SYS_InstallHandler_Expect(LPTMR0_IRQn, 
                                  test_lptmr_handler, 
                                  (isr_t *)NULL);
    INT_SYS_InstallHandler_IgnoreArg_newHandler();

    INT_SYS_EnableIRQ_Expect(LPTMR0_IRQn);

    CLOCK_DRV_GetFreq_ExpectAndReturn(SOSCDIV2_CLK, 
                                      &m_test_lptmr_clk_freq, 
                                      STATUS_SUCCESS);
    CLOCK_DRV_GetFreq_IgnoreArg_frequency();

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bsp_timer_init();
}

/**
 * @brief The test verifies the bsp_timer_init() function by calling the 
 * CLOCK_DRV_GetFreq function with a failed call and the LPTMR_DRV_Init, 
 * INT_SYS_InstallHandler, and INT_SYS_EnableIRQ function with a call.
 * 
 * https://codebeamer.daiichi.com/issue/183971
 *  
 */
void test_bsp_timer_init__fail_to_get__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    LPTMR_DRV_Init_ExpectAnyArgs();

    INT_SYS_InstallHandler_ExpectAnyArgs();

    INT_SYS_EnableIRQ_ExpectAnyArgs();

    CLOCK_DRV_GetFreq_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //
    TEST_STEP(1);
    bsp_timer_init();
}

/**
 * @brief The test verifies the bsp_timer_init() function by calling the 
 * CLOCK_DRV_GetFreq function with a failed call and the LPTMR_DRV_Init, 
 * INT_SYS_InstallHandler, and INT_SYS_EnableIRQ function with a call.
 * 
 * https://codebeamer.daiichi.com/issue/183868
 *  
 */
void test_bsp_timer_deinit__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_bsp_timer_init(STATUS_SUCCESS);

    // ************************** Expected Calls **************************** //

    LPTMR_DRV_Deinit_Expect(TEST_LPTMR_INSTANCE);
    INT_SYS_DisableIRQ_Expect(LPTMR0_IRQn);

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bsp_timer_deinit();
}

/**
 * @brief The test verifies that the bsp_timer_deinit() function assigns a NULL 
 * value to the callback variable by calling the LPTMR_DRV_Deinit and 
 * INT_SYS_DisableIRQ functions.
 * 
 * https://codebeamer.daiichi.com/item/186179
 *  
 */
void test_bsp_timer_deinit__usual_run__callback_assignment_removed(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_bsp_timer_init_with_stub();
    helper_bsp_timer_cb_set();

    // ************************** Expected Calls **************************** //

    LPTMR_DRV_Deinit_Expect(TEST_LPTMR_INSTANCE);
    INT_SYS_DisableIRQ_Expect(LPTMR0_IRQn);

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bsp_timer_deinit();

    TEST_STEP(2);
    lptmr_handler();
}

/**
 * @brief The test verifies the bsp_timer_deinit() function according to timer 
 * initialization status.
 * 
 * https://codebeamer.daiichi.com/issue/183869
 *  
 */
void test_bsp_timer_deinit__without_init__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //
    
    helper_bsp_timer_init(STATUS_ERROR);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bsp_timer_deinit();
}

/**
 * @brief The test verifies the bsp_timer_start() function by calling the 
 * LPTMR_DRV_StartCounter function.
 * 
 * https://codebeamer.daiichi.com/issue/183886
 *  
 */
void test_bsp_timer_start__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //
    
    helper_bsp_timer_init(STATUS_SUCCESS);

    // ************************** Expected Calls **************************** //

    LPTMR_DRV_StartCounter_Expect(TEST_LPTMR_INSTANCE);

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bsp_timer_start(TEST_BSP_TIMER_DAC_PORT);
}

/**
 * @brief The test verifies the bsp_timer_start() function according to the 
 * timer initialization status.
 * 
 * https://codebeamer.daiichi.com/issue/183887
 *  
 */
void test_bsp_timer_start__without_init__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //
    
    helper_bsp_timer_init(STATUS_ERROR);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bsp_timer_start(TEST_BSP_TIMER_DAC_PORT);
}


/**
 * @brief The test verifies the bsp_timer_start() function according to the 
 * port parameter.
 * 
 * https://codebeamer.daiichi.com/issue/183888
 *  
 */
void test_bsp_timer_start__call_wrong_param__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //
    
    helper_bsp_timer_init(STATUS_SUCCESS);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bsp_timer_start(STATUS_ERROR);
}

/**
 * @brief The test verifies the bsp_timer_stop() function by calling the 
 * LPTMR_DRV_StopCounter function.
 * 
 * https://codebeamer.daiichi.com/issue/183897
 *  
 */
void test_bsp_timer_stop__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //
    
    helper_bsp_timer_init(STATUS_SUCCESS);

    // ************************** Expected Calls **************************** //

    LPTMR_DRV_StopCounter_Expect(TEST_LPTMR_INSTANCE);

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bsp_timer_stop(TEST_BSP_TIMER_DAC_PORT);
}

/**
 * @brief The test verifies the bsp_timer_stop() function according to the 
 * timer initialization status.
 * 
 * https://codebeamer.daiichi.com/issue/183898
 *  
 */
void test_bsp_timer_stop__without_init_return(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //
    
    helper_bsp_timer_init(STATUS_ERROR);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bsp_timer_stop(TEST_BSP_TIMER_DAC_PORT);
}

/**
 * @brief The test verifies the bsp_timer_stop() function according to the 
 * timer port parameter.
 * 
 * https://codebeamer.daiichi.com/issue/183902
 *  
 */
void test_bsp_timer_stop__call_wrong_param__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //
    
    helper_bsp_timer_init(STATUS_SUCCESS);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bsp_timer_stop(STATUS_ERROR);
}

/**
 * @brief The test verifies the bsp_timer_period_set() function by calling the 
 * LPTMR_DRV_IsRunning and LPTMR_DRV_SetCompareValueByCount functions.
 * 
 * https://codebeamer.daiichi.com/issue/183905
 *  
 */
void test_bsp_timer_period_set__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //
    
    LPTMR_DRV_Init_ExpectWithArray(TEST_LPTMR_INSTANCE, 
                                   &m_test_lptmr_cfg, 
                                   11U, 
                                   true);
    LPTMR_DRV_Init_IgnoreArg_config();

    INT_SYS_InstallHandler_Expect(LPTMR0_IRQn, 
                                 test_lptmr_handler, 
                                 (isr_t *)NULL);
    INT_SYS_InstallHandler_IgnoreArg_newHandler();

    INT_SYS_EnableIRQ_Expect(LPTMR0_IRQn);

    CLOCK_DRV_GetFreq_ExpectAndReturn(SOSCDIV2_CLK, 
                                      &m_test_lptmr_clk_freq, 
                                      STATUS_SUCCESS);
    CLOCK_DRV_GetFreq_IgnoreArg_frequency();
    CLOCK_DRV_GetFreq_ReturnArrayThruPtr_frequency(&m_test_lptmr_clk_freq, 1U);

    bsp_timer_init();

    helper_bsp_timer_stop();

    uint16_t tick_value = (uint16_t)(m_test_lptmr_clk_freq / period_hz);

    // ************************** Expected Calls **************************** //

    LPTMR_DRV_IsRunning_ExpectAndReturn(TEST_LPTMR_INSTANCE, false);
    LPTMR_DRV_SetCompareValueByCount_ExpectAndReturn(TEST_LPTMR_INSTANCE, 
                                                     tick_value, 
                                                     STATUS_SUCCESS);

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_timer_period_set(TEST_BSP_TIMER_DAC_PORT, period_hz);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief The test verifies the bsp_timer_period_set() function according to 
 * the timer initialization status.
 * 
 * https://codebeamer.daiichi.com/issue/183917
 *  
 */
void test_bsp_timer_period_set__without_init__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //   

    // **************************** Pre-Actions ***************************** //
    
    helper_bsp_timer_init(STATUS_ERROR);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_timer_period_set(TEST_BSP_TIMER_DAC_PORT, period_hz);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the bsp_timer_period_set() function according to 
 * the timer initialization status.
 * 
 * https://codebeamer.daiichi.com/issue/183919
 *  
 */
void test_bsp_timer_period_set__invalid_port_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //    

    // **************************** Pre-Actions ***************************** //
    
    helper_bsp_timer_init(STATUS_SUCCESS);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_timer_period_set(period_hz, period_hz);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the bsp_timer_period_set() function according to 
 * the invalid period argument.
 * 
 * https://codebeamer.daiichi.com/item/184490
 *  
 */
void test_bsp_timer_period_set__invalid_period_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //
    
    helper_bsp_timer_init(STATUS_SUCCESS);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_timer_period_set(TEST_BSP_TIMER_DAC_PORT, 0U);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the bsp_timer_period_set() function by calling the 
 * LPTMR_DRV_IsRunning function with a failed call.
 * 
 * https://codebeamer.daiichi.com/issue/183920
 *  
 */
void test_bsp_timer_period_set__timer_state__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //
    
    helper_bsp_timer_init(STATUS_SUCCESS);
    helper_bsp_timer_stop();

    // ************************** Expected Calls **************************** //

    LPTMR_DRV_IsRunning_ExpectAndReturn(TEST_LPTMR_INSTANCE, true);

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_timer_period_set(TEST_BSP_TIMER_DAC_PORT, period_hz);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the bsp_timer_period_set() function by calling the 
 * LPTMR_DRV_SetCompareValueByCount function with a failed call.
 * 
 * https://codebeamer.daiichi.com/issue/183921
 *  
 */
void test_bsp_timer_period_set__compare_value__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //
    
    helper_bsp_timer_init(STATUS_SUCCESS);
    helper_bsp_timer_stop();

    // ************************** Expected Calls **************************** //

    LPTMR_DRV_IsRunning_ExpectAndReturn(TEST_LPTMR_INSTANCE, false);
    LPTMR_DRV_SetCompareValueByCount_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_timer_period_set(TEST_BSP_TIMER_DAC_PORT, period_hz);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the function set by bsp_timer_cb_set() with a 
 * successful call.
 * 
 * https://codebeamer.daiichi.com/issue/184318
 *  
 */
void test_bsp_timer_cb_set__static_function_call__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //
    
    helper_bsp_timer_init_with_stub();
    helper_bsp_timer_cb_set();

    // ************************** Expected Calls **************************** //
    
    LPTMR_DRV_GetCompareFlag_ExpectAndReturn(TEST_LPTMR_INSTANCE, true);
    LPTMR_DRV_ClearCompareFlag_ExpectAnyArgs();

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    lptmr_handler();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, is_test_state);
}

/**
 * @brief  The test validates the function set by bsp_timer_cb_set() with a 
 * failed call at the compare flag step.
 * 
 * https://codebeamer.daiichi.com/item/184319
 *  
 */
void test_bsp_timer_cb_set__flag_status_failed__not_occur_cb_action(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //
    
    helper_bsp_timer_init_with_stub();
    helper_bsp_timer_cb_set();

    // ************************** Expected Calls **************************** //
    
    LPTMR_DRV_GetCompareFlag_ExpectAndReturn(TEST_LPTMR_INSTANCE, false);

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    lptmr_handler();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, is_test_state);
}

/**
 * @brief The test verifies the bsp_timer_cb_set() function with a successful 
 * call.
 * 
 * https://codebeamer.daiichi.com/issue/183925
 *  
 */
void test_bsp_timer_cb_set__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //
    
    helper_bsp_timer_init(STATUS_SUCCESS);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_timer_cb_set(TEST_BSP_TIMER_DAC_PORT, test_lptmr_handler);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief The test verifies the bsp_timer_cb_set() function according to the 
 * timer initialization status.
 * 
 * https://codebeamer.daiichi.com/item/183927
 *  
 */
void test_bsp_timer_cb_set__without_init__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //
    
    helper_bsp_timer_init(STATUS_ERROR);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_timer_cb_set(TEST_BSP_TIMER_DAC_PORT, test_lptmr_handler);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the bsp_timer_cb_set() function according to the 
 * wrong callback parameter.
 * 
 * https://codebeamer.daiichi.com/issue/183928
 *  
 */
void test_bsp_timer_cb_set__call_wrong_cb_param__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //
    
    helper_bsp_timer_init(STATUS_SUCCESS);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_timer_cb_set(TEST_BSP_TIMER_DAC_PORT, NULL);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the bsp_timer_cb_set() function according to the 
 * wrong port parameter.
 * 
 * https://codebeamer.daiichi.com/issue/183933
 *  
 */
void test_bsp_timer_cb_set__call_wrong_port_param__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //
    
    helper_bsp_timer_init(STATUS_SUCCESS);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //
    
    TEST_STEP(1);
    bool ret = bsp_timer_cb_set(1U, test_lptmr_handler);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/*******************************************************************************
 * Private Functions Definitions
 ******************************************************************************/

static void test_lptmr_handler(void)
{
    is_test_state = true;
}

static void test_lptmr_drv_init_cb (const uint32_t instance,
                                    const lptmr_config_t * const config,
                                    const bool startCounter,
                                    int cmock_num_calls)
{
    (void) instance;
    (void) cmock_num_calls;

    TEST_ASSERT_EQUAL(m_test_lptmr_cfg.dmaRequest, config->dmaRequest);
    TEST_ASSERT_EQUAL(m_test_lptmr_cfg.interruptEnable, config->interruptEnable);
    TEST_ASSERT_EQUAL(m_test_lptmr_cfg.freeRun, config->freeRun);
    TEST_ASSERT_EQUAL(m_test_lptmr_cfg.workMode, config->workMode);
    TEST_ASSERT_EQUAL(m_test_lptmr_cfg.clockSelect, config->clockSelect);
    TEST_ASSERT_EQUAL(m_test_lptmr_cfg.prescaler, config->prescaler);
    TEST_ASSERT_EQUAL(m_test_lptmr_cfg.bypassPrescaler, config->bypassPrescaler);
    TEST_ASSERT_EQUAL(m_test_lptmr_cfg.compareValue, config->compareValue);
    TEST_ASSERT_EQUAL(m_test_lptmr_cfg.counterUnits, config->counterUnits);
    TEST_ASSERT_EQUAL(m_test_lptmr_cfg.pinSelect, config->pinSelect);
    TEST_ASSERT_EQUAL(m_test_lptmr_cfg.pinPolarity, config->pinPolarity);
    TEST_ASSERT_EQUAL(startCounter, true);
}

static void helper_bsp_timer_init(status_t status)
{
    LPTMR_DRV_Init_ExpectAnyArgs();

    INT_SYS_InstallHandler_ExpectAnyArgs();

    INT_SYS_EnableIRQ_ExpectAnyArgs();

    CLOCK_DRV_GetFreq_ExpectAnyArgsAndReturn(status);
    
    bsp_timer_init();
}

static void helper_bsp_timer_stop()
{
    LPTMR_DRV_StopCounter_Expect(TEST_LPTMR_INSTANCE);

    bsp_timer_stop(TEST_BSP_TIMER_DAC_PORT);
}


static void helper_bsp_timer_init_with_stub(){
    
    // ************************** Expected Calls **************************** //
    LPTMR_DRV_Init_ExpectAnyArgs();

    INT_SYS_InstallHandler_Ignore();
    INT_SYS_InstallHandler_StubWithCallback(timer_stub_cb);

    INT_SYS_EnableIRQ_ExpectAnyArgs();
    CLOCK_DRV_GetFreq_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    // ********************************************************************** //
    bsp_timer_init();
}

static void helper_bsp_timer_cb_set(){

    (void)bsp_timer_cb_set(TEST_BSP_TIMER_DAC_PORT, test_lptmr_handler);
}

static void timer_stub_cb(IRQn_Type irqNumber, 
                          const isr_t newHandler, 
                          isr_t* const oldHandler, 
                          int cmock_num_calls)
{
    (void)irqNumber;
    (void)oldHandler;
    (void)cmock_num_calls;

    lptmr_handler = newHandler;
}

// end of file
