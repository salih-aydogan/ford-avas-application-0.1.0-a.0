/**
 * @file test_bsp_isr.c
 * @author Furkan Yildirim (furkan.yildirim@daiichi.com)
 * 
 * @brief These tests verify the units of the bsp_isr module.
 * 
 * @version 0.1
 * @date 2023-02-23
 * 
 * @copyright Copyright (c) 2023
 * 
 * https://codebeamer.daiichi.com/issue/184889
 * 
 */

/*******************************************************************************
 * Includes 
 ******************************************************************************/

#include "unity.h"
#include "support.h"

#include "bsp_isr.h"

#include "mock_interrupt_manager.h"

#include "mock_log.h"

/*******************************************************************************
 * Test Files
 ******************************************************************************/

TEST_FILE("bsp_isr.c");

/*******************************************************************************
 * Typedef and Definations
 ******************************************************************************/

/**
 * @brief This macro uses for giving an invalid argument value to 
 *        bsp_isr_cb_set.
 * 
 */
#define TEST_INVALID_ISR_ARGUMENT_VALUE 3U

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/**
 * @brief This array variable holds static isr function addresses.
 *        isr_function_addr[0U]: hardfault_isr
 *        isr_function_addr[1U]: systick_isr
 * 
 */
isr_t m_test_isr_function_addr[2U] = {0U};

/**
 * @brief This variable holds the callback flag. This flag must be true when 
 *        the static function (m_callback_function_1) is called. This indicates 
 *        that the static functions in the bsp_isr module can or cannot be 
 *        called via the stub function.
 * 
 */
bool m_test_callback_flag = false;

/*******************************************************************************
 * Private Functions Prototype Declarations
 ******************************************************************************/

/**
 * @brief This function inits bsp isr component in the pre-actions.
 * 
 */
static void helper_bsp_isr_init(void);

/**
 * @brief This function is the stub function of the INT_SYS_InstallHandler 
 *        function in the bsp_isr_init function. 
 * 
 */
static void stub_INT_SYS_InstallHandler(IRQn_Type irqNumber,
                                        const isr_t newHandler,
                                        isr_t* const oldHandler,
                                        int cmock_num_calls);

/**
 * @brief This function runs like a registered callback function via 
 *        bsp_isr_cb_set. This function don't set callback_flag function.
 * 
 */
static void m_callback_function_with_not_flag_set(void);

/**
 * @brief This function runs like a registered callback function via 
 *        bsp_isr_cb_set. It is qualified as 1 to represent the first function 
 *        used in successive calls.
 * 
 */
static void m_callback_function_1(void);

/**
 * @brief This function runs like a registered callback function via 
 *        bsp_isr_cb_set. It is qualified as 2 to represent the second function 
 *        used in successive calls.
 * 
 */
static void m_callback_function_2(void);

/**
 * @brief This function runs like a registered callback function via 
 *        bsp_isr_cb_set. It is qualified as 3 to represent the third function 
 *        used in successive calls.
 * 
 */
static void m_callback_function_3(void);

/**
 * @brief This function runs like a registered callback function via 
 *        bsp_isr_cb_set. It is qualified as 4 to represent the fourth function 
 *        used in successive calls.
 * 
 */
static void m_callback_function_4(void);

/**
 * @brief This function runs like a registered callback function via 
 *        bsp_isr_cb_set. It is qualified as 5 to represent the fifth function 
 *        used in successive calls.
 * 
 */
static void m_callback_function_5(void);

/**
 * @brief This function runs like a registered callback function via 
 *        bsp_isr_cb_set. It is qualified as 5 to represent the sixth function 
 *        used in successive calls.
 * 
 */
static void m_callback_function_6(void);

/**
 * @brief This function runs like a nonregistered old callback function via 
 *        bsp_isr_cb_set.
 * 
 */
static void m_old_callback_function(void);

/*******************************************************************************
 * Setup & Teardown 
 ******************************************************************************/

void setUp()
{
    log_print_Ignore();

    m_test_callback_flag = false;

    m_test_isr_function_addr[BSP_ISR_HARDFAULT] = 0U;
    m_test_isr_function_addr[BSP_ISR_SYSTICK] = 0U;
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
 * @brief The test verifies the function bsp_isr_init by successfully calling 
 *        INT_SYS_InstallHandler(), INT_SYS_SetPriority() functions.
 * 
 * @link https://codebeamer.daiichi.com/issue/184721
 *   
 */
void test_bsp_isr_init__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //
    
    INT_SYS_InstallHandler_Expect(HardFault_IRQn, NULL, NULL); 
    INT_SYS_InstallHandler_IgnoreArg_newHandler();
    INT_SYS_InstallHandler_IgnoreArg_oldHandler();

    INT_SYS_InstallHandler_Expect(SysTick_IRQn,
                                  NULL,
                                  NULL);
    INT_SYS_InstallHandler_IgnoreArg_newHandler();
    INT_SYS_InstallHandler_IgnoreArg_oldHandler();
    
    INT_SYS_SetPriority_Expect(LPTMR0_IRQn,                 0U);
    INT_SYS_SetPriority_Expect(WDOG_IRQn,                   0U);
    INT_SYS_SetPriority_Expect(PORT_IRQn,                   1U);
    INT_SYS_SetPriority_Expect(CAN0_ORed_Err_Wakeup_IRQn,   1U);
    INT_SYS_SetPriority_Expect(CAN0_ORed_0_31_MB_IRQn,      1U);
    INT_SYS_SetPriority_Expect(LPI2C0_Master_Slave_IRQn,    2U);
    INT_SYS_SetPriority_Expect(FLEXIO_IRQn,                 2U);
    INT_SYS_SetPriority_Expect(LPSPI0_IRQn,                 3U);
    INT_SYS_SetPriority_Expect(LPUART0_RxTx_IRQn,           3U);
    INT_SYS_SetPriority_Expect(DMA0_IRQn,                   3U);
    INT_SYS_SetPriority_Expect(DMA_Error_IRQn,              3U);
    INT_SYS_SetPriority_Expect(SysTick_IRQn,                0U);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);    
    bsp_isr_init();
}

/**
 * @brief This test verifies that the bsp_isr_init function resets the callback 
 * values on successful initialization.
 *  
 * @link https://codebeamer.daiichi.com/item/186625
 *   
 */
void test_bsp_isr_init__usual_run__callback_assignment_removed(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //
    
    //bsp_isr_init
    INT_SYS_InstallHandler_AddCallback(stub_INT_SYS_InstallHandler);

    INT_SYS_InstallHandler_Expect(HardFault_IRQn, NULL, NULL);
    INT_SYS_InstallHandler_IgnoreArg_newHandler();
    INT_SYS_InstallHandler_IgnoreArg_oldHandler();
    
    INT_SYS_InstallHandler_Expect(SysTick_IRQn, NULL, NULL);
    INT_SYS_InstallHandler_IgnoreArg_newHandler();
    INT_SYS_InstallHandler_IgnoreArg_oldHandler();

    INT_SYS_SetPriority_Ignore();
    

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);    
    bsp_isr_init();

    TEST_STEP(2);
    m_test_isr_function_addr[BSP_ISR_SYSTICK]();

    TEST_STEP(3);
    m_test_isr_function_addr[BSP_ISR_HARDFAULT](); 
}

/**
 * @brief The test verifies the function bsp_isr_enter_critical by successfully 
 *        calling INT_SYS_DisableIRQGlobal_Epect() function.
 * 
 * @link https://codebeamer.daiichi.com/issue/184722
 *   
 */
void test_bsp_isr_enter_critical__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    INT_SYS_DisableIRQGlobal_Expect();

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_isr_enter_critical();
}

/**
 * @brief The test verifies the function bsp_isr_exit_critical by successfully 
 *        calling INT_SYS_EnableIRQGlobal_Expect() function.
 * 
 * @link https://codebeamer.daiichi.com/issue/184723
 *  
 */
void test_bsp_isr_exit_critical__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    INT_SYS_EnableIRQGlobal_Expect();

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_isr_exit_critical();
}

/**
 * @brief The test verifies the bsp_isr_cb_set function by unsuccessfully 
 *        calling hardfault_isr function. 
 * 
 * @link https://codebeamer.daiichi.com/issue/184727
 * 
 */
void test_bsp_isr_cb_set__null_callback_argument_hardfault_callback__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_bsp_isr_init();

    bsp_isr_cb_set(BSP_ISR_HARDFAULT, NULL); 

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    m_test_isr_function_addr[BSP_ISR_HARDFAULT]();  

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, m_test_callback_flag);
}

/**
 * @brief The test verifies the bsp_isr_cb_set function by unsuccessfully 
 *        calling systick_isr function. 
 * 
 * @link https://codebeamer.daiichi.com/issue/184724
 * 
 */
void test_bsp_isr_cb_set__null_callback_argument_systick_callback__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_bsp_isr_init();

    for (uint32_t i = 0U; i < 5U; i++)
    {
        bsp_isr_cb_set(BSP_ISR_SYSTICK, NULL);
    }

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);    
    m_test_isr_function_addr[BSP_ISR_SYSTICK]();
    
    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, m_test_callback_flag);
}

/**
 * @brief The test verifies the bsp_isr_cb_set function by calling 
 *        bsp_isr_cb_set function with invalid argument value. 
 * 
 * @link https://codebeamer.daiichi.com/issue/184730
 * 
 */
void test_bsp_isr_cb_set__invalid_isr_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_bsp_isr_init();

    bsp_isr_cb_set(TEST_INVALID_ISR_ARGUMENT_VALUE, m_callback_function_1);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);    
    m_test_isr_function_addr[BSP_ISR_HARDFAULT]();
    
    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, m_test_callback_flag);

    TEST_STEP(3);
    m_test_isr_function_addr[BSP_ISR_SYSTICK]();

    TEST_STEP(4);
    TEST_ASSERT_EQUAL(false, m_test_callback_flag);
}

/**
 * @brief The test verifies the bsp_isr_cb_set function by successfully calling 
 *        hardfault_isr function.
 * 
 * @link https://codebeamer.daiichi.com/issue/184797
 * 
 */
void test_bsp_isr_cb_set__hardfault_callback_usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_bsp_isr_init();

    bsp_isr_cb_set(BSP_ISR_HARDFAULT, m_callback_function_1);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);    
    m_test_isr_function_addr[BSP_ISR_HARDFAULT]();
    
    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, m_test_callback_flag);
}

/**
 * @brief The test verifies the bsp_isr_cb_set function by successfully calling 
 *        systick_isr function.
 * 
 * @link https://codebeamer.daiichi.com/issue/184882
 * 
 */
void test_bsp_isr_cb_set__systick_callback_usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_bsp_isr_init();

    bsp_isr_cb_set(BSP_ISR_SYSTICK, m_callback_function_1);
    bsp_isr_cb_set(BSP_ISR_SYSTICK, m_callback_function_2);
    bsp_isr_cb_set(BSP_ISR_SYSTICK, m_callback_function_3);
    bsp_isr_cb_set(BSP_ISR_SYSTICK, m_callback_function_4);
    bsp_isr_cb_set(BSP_ISR_SYSTICK, m_callback_function_5);    

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);    
    m_test_isr_function_addr[BSP_ISR_SYSTICK]();
    
    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, m_test_callback_flag);
}

/**
 * @brief The test verifies the bsp_isr_cb_set function by unsuccessfully 
 *        calling systick_isr function
 * 
 * @link https://codebeamer.daiichi.com/issue/185218
 * 
 */
void test_bsp_isr_cb_set__systick_callback_overflow__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_bsp_isr_init();

    bsp_isr_cb_set(BSP_ISR_SYSTICK, m_callback_function_with_not_flag_set);
    bsp_isr_cb_set(BSP_ISR_SYSTICK, m_callback_function_2);
    bsp_isr_cb_set(BSP_ISR_SYSTICK, m_callback_function_3);
    bsp_isr_cb_set(BSP_ISR_SYSTICK, m_callback_function_4);
    bsp_isr_cb_set(BSP_ISR_SYSTICK, m_callback_function_5); 
    bsp_isr_cb_set(BSP_ISR_SYSTICK, m_callback_function_6);   

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);    
    m_test_isr_function_addr[BSP_ISR_SYSTICK]();
    
    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, m_test_callback_flag);
}

/*******************************************************************************
 * Private Functions Definitions
 ******************************************************************************/

static void helper_bsp_isr_init(void)
{
    //bsp_isr_init
    INT_SYS_InstallHandler_AddCallback(stub_INT_SYS_InstallHandler);

    INT_SYS_InstallHandler_Expect(HardFault_IRQn, NULL, NULL);
    INT_SYS_InstallHandler_IgnoreArg_newHandler();
    INT_SYS_InstallHandler_IgnoreArg_oldHandler();
    
    INT_SYS_InstallHandler_Expect(SysTick_IRQn, NULL, NULL);
    INT_SYS_InstallHandler_IgnoreArg_newHandler();
    INT_SYS_InstallHandler_IgnoreArg_oldHandler();

    INT_SYS_SetPriority_Ignore();

    bsp_isr_init();
}

static void stub_INT_SYS_InstallHandler(IRQn_Type irqNumber,
                                        const isr_t newHandler,
                                        isr_t* const oldHandler,
                                        int cmock_num_calls)
{
    (void)irqNumber;

    if (cmock_num_calls < 2)
    {
        m_test_isr_function_addr[cmock_num_calls] = newHandler;
    }

    if (BSP_ISR_SYSTICK == cmock_num_calls)
    {
        *oldHandler = m_old_callback_function;
    }
    else
    {
        (void)oldHandler;
    }
}

static void m_callback_function_with_not_flag_set(void)
{
    TEST_MESSAGE("MY CALLBACK FUNCTION 1 CALLED");
}

static void m_callback_function_1(void)
{
    TEST_MESSAGE("MY CALLBACK FUNCTION 1 CALLED");

    m_test_callback_flag = true;
}

static void m_callback_function_2(void)
{
    TEST_MESSAGE("MY CALLBACK FUNCTION 2 CALLED");
}

static void m_callback_function_3(void)
{
    TEST_MESSAGE("MY CALLBACK FUNCTION 3 CALLED");
}

static void m_callback_function_4(void)
{
    TEST_MESSAGE("MY CALLBACK FUNCTION 4 CALLED");
}

static void m_callback_function_5(void)
{
    TEST_MESSAGE("MY CALLBACK FUNCTION 5 CALLED");
}

static void m_callback_function_6(void)
{
    TEST_MESSAGE("MY CALLBACK FUNCTION 6 CALLED");

    m_test_callback_flag = true;
}

static void m_old_callback_function(void)
{
    TEST_MESSAGE("MY OLD CALLBACK FUNCTION CALLED");
}

// end of file 
