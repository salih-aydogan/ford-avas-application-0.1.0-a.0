/**
 * @file test_appman.c
 * @author Muhammed Salih AYDOGAN
 * @brief These tests verify the units of the appman module.
 * 
 * @version 0.1
 * 
 * @date 28/03/2023
 * 
 * @copyright Copyright (c) 2023
 * 
 * https://codebeamer.daiichi.com/issue/187322
 * 
 */

/*******************************************************************************
 * Includes 
 ******************************************************************************/

//First - include test framework headers
#include "unity.h"
#include "support.h"

// Second - include the module to be tested
#include "appman.h"

// Third - include the mocks of required modules
#include "mock_bsp_mcu.h"
#include "mock_bsp_isr.h"

#include "mock_log.h"

/*******************************************************************************
 * Test Files
 ******************************************************************************/

TEST_FILE("appman.c");

/*******************************************************************************             
 * Defines
 ******************************************************************************/
/**
 * @brief MCU core interrupt handler
*/
#define TEST_APP_ISR_SYSTICK                  1U

/*******************************************************************************
 * Extern Variables
 ******************************************************************************/

/*******************************************************************************
 * Private Variables
 ******************************************************************************/
/**
 * @brief Test message for send process.
*/
static char test_message[12U] = "1234567891";

/**
 * @brief Test message for send process.
*/
unsigned char test_data[] = {0x12, 0x34, 0x56};

/**
 * @brief Test variable for call callback process.
*/
static bool cb_status;

/**
 * @brief Test variable for first timer hadler.
*/
static bool first_handler_status;

/**
 * @brief Test variable for second timer hadler.
*/
static bool second_handler_status;

/**
 * @brief Function pointer for hold callback function adress.
*/
static bsp_isr_cb_t test_timer_cb = NULL;

/*******************************************************************************
 * Global Variables
 ******************************************************************************/

/*******************************************************************************
 * Private Functions Prototype Declarations
 ******************************************************************************/
/**
 * @brief This function for used to common application init handler.
*/
static void test_app_init(void);

/**
 * @brief This function for used to avas application init handler.
*/
static void test_app_avas_init(void);

/**
 * @brief This function for used to diagnostic application init handler.
*/
static void test_app_diag_init(void);

/**
 * @brief This function for used to can application init handler.
*/
static void test_app_can_msg_send_init(void);

/**
 * @brief This function for used to can application init handler.
*/
static void test_app_can_init(void);

/**
 * @brief This function for used to main application init handler.
*/
static void test_app_main_init(void);

/**
 * @brief This function for used to main application init handler with message 
 * send process.
*/
static void test_app_main_msg_send_init(void);

/**
 * @brief This function for used to can application init handler with message 
 * send process.
*/
static void test_app_can_msg_second_send_init(void);

/**
 * @brief This function for used to main application init handler with message 
 * send process.
*/
static void test_app_main_msg_second_send_init(void);

/**
 * @brief This function for used to avas application init handler with message 
 * send process.
*/
static void test_app_avas_msg_send_init(void);

/**
 * @brief This function for used to send message on diagnostic application init 
 * handler.
*/
static void test_app_diag_send_init(void);

/**
 * @brief This function for used to common application run handler.
*/
static void test_app_run(void);

/**
 * @brief This function for used to main application run handler.
*/
static void test_app_main_run(void);

/**
 * @brief This function for used to can application run handler.
*/
static void test_app_can_run(void);

/**
 * @brief This function for used to avas application run handler.
*/
static void test_app_avas_run(void);

/**
 * @brief This function for used to diag application run handler.
*/
static void test_app_diag_run(void);

/**
 * @brief This function for used to diag application run handler.
*/
static void test_app_diag_send_msg_run(void);

/**
 * @brief This function for used to avas application run handler.
*/
static void test_app_avas_send_msg_run(void);

/**
 * @brief This function for used to common application deinit handler.
*/
static void test_app_deinit(void);

/**
 * @brief This function uses when the timeout occurs.
*/
static void test_appman_timer_handler(void *p_arg);

/**
 * @brief This function uses when the timeout occurs for second timer. 
*/
static void test_appman_second_timer_handler(void *p_arg);

/**
 * @brief Message handler function for AVAS application. Check incoming message. 
*/
static void test_appman_msg_avas_handler(void *p_arg);

/**
 * @brief Message handler function for DIAGNOSTIC application. Check incoming 
 * message. 
*/
static void test_appman_msg_diag_handler(void *p_arg);

/**
 * @brief Message handler function for DIAGNOSTIC application. Check incoming 
 * message. 
*/
static void test_appman_msg_diag_second_handler(void *p_arg);

/**
 * @brief Message handler function for MAIN application. Check incoming message. 
*/
static void test_appman_msg_main_handler(void *p_arg);

/**
 * @brief Message handler function for CAN application. Check incoming message. 
*/
static void test_appman_msg_can_handler(void *p_arg);

/**
 * @brief Helper function uses for appman initialize.
*/
static void helper_appman_init(void);

/**
 * @brief Helper function uses for appman initialize with stub function.
*/
static void helper_appman_init_stub(void);

/**
 * @brief Stub function for get callback adress.
*/
static void test_cb_stub(bsp_isr_t isr, 
                         bsp_isr_cb_t callback, 
                         int cmock_num_calls);

/**
 * @brief Stub function for bsp_mcu_tick_get.
*/
static uint32_t test_stub_tick_get(int cmock_num_calls);

/**
 * @brief Helper function for appman create process.
*/
static void helper_appman_app_create(appman_app_t *p_app, 
                                     appman_status_t *p_status);

/**
 * @brief Helper function uses for appman timer create.
*/
static void helper_appman_timer_create(appman_timer_t *p_timer, 
                                       appman_status_t *p_status);                                    

/**
 * @brief Helper function uses for appman timer create.
*/
static void helper_appman_timer_create_with_ignore(appman_timer_t *p_timer, 
                                                   appman_status_t *p_status);

/*******************************************************************************
 * Setup & Teardown 
 ******************************************************************************/

void setUp()
{
    log_print_Ignore();

    cb_status = false;
    second_handler_status = false;
    first_handler_status = false;
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
 * @brief The test verifies the appman_init() function by calling the 
 * bsp_isr_cb_set() function.
 * 
 * https://codebeamer.daiichi.com/issue/187327
 *  
 */
void test_appman_init__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // Callback for timer operations
    bsp_isr_cb_set_Stub(test_cb_stub);
    bsp_isr_cb_set_ExpectAnyArgs();

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_init();    

    TEST_STEP(2);
    test_timer_cb();
}

/**
 * @brief The test verifies the appman_app_create() function with valid 
 * arguments.
 * 
 * https://codebeamer.daiichi.com/item/187429
 *  
 */
void test_appman_app_create__usual_run__success(void)
{
    TEST_CASE();

    uint8_t test_msg_arr[16];

    // **************************** Input Params **************************** //
 
    appman_app_t test_p_app;
    test_p_app.p_name = "TEST_APP";
    test_p_app.app_id = APPMAN_APP_ID_MAIN;
    test_p_app.init = test_app_init;
    test_p_app.run = test_app_run;
    test_p_app.deinit = test_app_deinit;
    test_p_app.p_msg_pool = test_msg_arr;
    test_p_app.msg_pool_size = 16U;
    appman_status_t status = APPMAN_STATUS_APP_NOT_CREATED;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_app_create(&test_p_app, &status);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(APPMAN_STATUS_SUCCESS, status);
}

/**
 * @brief The test verifies with an invalid argument that the 
 * appman_app_create() function sets the state.
 * 
 * https://codebeamer.daiichi.com/item/187434
 *  
 */
void test_appman_app_create__invalid_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    appman_status_t status = APPMAN_STATUS_APP_NOT_CREATED;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_app_create(NULL, &status);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(APPMAN_STATUS_ERROR_PARAM, status);
}

/**
 * @brief The test verifies with an invalid name variable that 
 * appman_app_create() sets the state.
 * 
 * https://codebeamer.daiichi.com/item/187436
 *  
 */
void test_appman_app_create__invalid_name_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    appman_app_t test_p_app;

    // Set name argument to NULL
    test_p_app.p_name = NULL;
    test_p_app.app_id = APPMAN_APP_ID_MAIN;

    appman_status_t status = APPMAN_STATUS_APP_NOT_CREATED;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_app_create(&test_p_app, &status);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(APPMAN_STATUS_ERROR_PARAM, status);
}

/**
 * @brief The test verifies with an invalid init function variable that 
 * appman_app_create() sets the state to "APPMAN_STATUS_ERROR_PARAM".
 * 
 * https://codebeamer.daiichi.com/issue/187437
 *  
 */
void test_appman_app_create__invalid_init_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    appman_app_t test_p_app;
    test_p_app.p_name = "TEST_APP";
    test_p_app.app_id = APPMAN_APP_ID_MAIN;

    // Set init argument to NULL
    test_p_app.init = NULL;
    test_p_app.run = test_app_run;
    test_p_app.deinit = test_app_deinit;

    appman_status_t status = APPMAN_STATUS_APP_NOT_CREATED;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_app_create(&test_p_app, &status);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(APPMAN_STATUS_ERROR_PARAM, status);
}

/**
 * @brief The test verifies with an invalid run function argument that 
 * appman_app_create() sets the fail state.
 * 
 * https://codebeamer.daiichi.com/issue/187438
 *  
 */
void test_appman_app_create__invalid_run_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    appman_app_t test_p_app;
    test_p_app.p_name = "TEST_APP";
    test_p_app.app_id = APPMAN_APP_ID_MAIN;
    test_p_app.init = test_app_init;

    // Set run argument to NULL
    test_p_app.run = NULL;
    test_p_app.deinit = test_app_deinit;

    appman_status_t status = APPMAN_STATUS_APP_NOT_CREATED;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_app_create(&test_p_app, &status);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(APPMAN_STATUS_ERROR_PARAM, status);
}

/**
 * @brief The test verifies with an invalid deinit function argument that 
 * appman_app_create() sets the fail state.
 * 
 * https://codebeamer.daiichi.com/item/187439
 *  
 */
void test_appman_app_create__invalid_deinit_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    appman_app_t test_p_app;
    test_p_app.p_name = "TEST_APP";
    test_p_app.app_id = APPMAN_APP_ID_MAIN;
    test_p_app.init = test_app_init;
    test_p_app.run = test_app_run;
    // Set deinit argument to NULL
    test_p_app.deinit = NULL;

    appman_status_t status = APPMAN_STATUS_APP_NOT_CREATED;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_app_create(&test_p_app, &status);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(APPMAN_STATUS_ERROR_PARAM, status);
}

/**
 * @brief The test verifies with an invalid status argument that 
 * appman_app_create() does not set the state.
 * 
 * https://codebeamer.daiichi.com/item/187440
 *  
 */
void test_appman_app_create__invalid_status_argument__not_set_status(void)
{
    TEST_CASE();

    uint8_t test_msg_arr[12];

    // **************************** Input Params **************************** //

    appman_app_t test_p_app;
    test_p_app.p_name = "TEST_APP";
    test_p_app.app_id = APPMAN_APP_ID_MAIN;
    test_p_app.init = test_app_init;
    test_p_app.run = test_app_run;
    test_p_app.deinit = test_app_deinit;
    test_p_app.p_msg_pool = test_msg_arr;
    test_p_app.msg_pool_size = (size_t)12U;

    // **************************** Pre-Actions ***************************** //

    helper_appman_init();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_app_create(&test_p_app, NULL);
}

/**
 * @brief The test verifies with an invalid size argument that 
 * appman_app_create() set the state to "APPMAN_STATUS_MSG_POOL_INVALID_SIZE".
 * 
 * https://codebeamer.daiichi.com/item/187445
 *  
 */
void test_appman_app_create__invalid_size_argument__size_error_occured(void)
{
    TEST_CASE();

    uint8_t test_msg_arr[12U];

    // **************************** Input Params **************************** //

    appman_app_t test_p_app;
    test_p_app.p_name = "TEST_APP";
    test_p_app.app_id = APPMAN_APP_ID_MAIN;
    test_p_app.init = test_app_init;
    test_p_app.run = test_app_run;
    test_p_app.deinit = test_app_deinit;
    test_p_app.p_msg_pool = test_msg_arr;

    // invalid size argument
    test_p_app.msg_pool_size = (size_t)1U;

    appman_status_t status = APPMAN_STATUS_APP_NOT_CREATED;

    // **************************** Pre-Actions ***************************** //

    helper_appman_init();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_app_create(&test_p_app, &status);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(APPMAN_STATUS_MSG_POOL_INVALID_SIZE, status);
}

/**
 * @brief The test verifies with an invalid message poll argument that 
 * appman_app_create() sets the fail state.
 * 
 * https://codebeamer.daiichi.com/issue/187446
 *  
 */
void test_appman_app_create__invalid_msg_pool_argument__poll_error_occured(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    appman_app_t test_p_app;
    test_p_app.p_name = "TEST_APP";
    test_p_app.app_id = APPMAN_APP_ID_MAIN;
    test_p_app.init = test_app_init;
    test_p_app.run = test_app_run;
    test_p_app.deinit = test_app_deinit;
    test_p_app.msg_pool_size = (size_t)12U;

    // invalid size argument
    test_p_app.p_msg_pool = NULL;

    appman_status_t status = APPMAN_STATUS_APP_NOT_CREATED;

    // **************************** Pre-Actions ***************************** //

    helper_appman_init();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_app_create(&test_p_app, &status);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(APPMAN_STATUS_APP_MSG_POOL_ERROR, status);
}

/**
 * @brief The test verifies the appman_app_create() function with by calling the 
 * msg_pool_init() function.
 * 
 * https://codebeamer.daiichi.com/issue/187447
 *  
 */
void test_appman_app_create__when_stable_pool_section_count__success(void)
{
    TEST_CASE();

    uint8_t test_msg_arr[12U];

    // **************************** Input Params **************************** //

    appman_app_t test_p_app;
    test_p_app.p_name = "TEST_APP";
    test_p_app.app_id = APPMAN_APP_ID_MAIN;
    test_p_app.init = test_app_init;
    test_p_app.run = test_app_run;
    test_p_app.deinit = test_app_deinit;
    test_p_app.p_msg_pool = test_msg_arr;
    test_p_app.msg_pool_size = (size_t)16U;

    appman_status_t status = APPMAN_STATUS_APP_NOT_CREATED;

    // **************************** Pre-Actions ***************************** //

    helper_appman_init();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_app_create(&test_p_app, &status);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(APPMAN_STATUS_SUCCESS, status);
}

/**
 * @brief The test verifies the appman_app_create() function by calling the 
 * msg_pool_init() function.Pools connected by msg_pool_init() in 
 * appman_app_create() checks in this test.
 * 
 * https://codebeamer.daiichi.com/issue/187453
 *  
 */
void test_appman_app_create__increase_section_count__success(void)
{
    TEST_CASE();

    uint8_t test_msg_arr[48U];

    // **************************** Input Params **************************** //

    appman_app_t test_p_app;
    test_p_app.p_name = "TEST_APP";
    test_p_app.app_id = APPMAN_APP_ID_MAIN;
    test_p_app.init = test_app_init;
    test_p_app.run = test_app_run;
    test_p_app.deinit = test_app_deinit;
    test_p_app.p_msg_pool = test_msg_arr;

    // Increase message pool size
    test_p_app.msg_pool_size = (size_t)48U;

    appman_status_t status = APPMAN_STATUS_APP_NOT_CREATED;

    typedef struct msg_pool_s
    {
        appman_msg_id_t msg_id;
        void *p_data;
        size_t size;
        struct msg_pool_s *p_next;

    }msg_pool_t;

    msg_pool_t *test_msg_pool = (msg_pool_t *)test_p_app.p_msg_pool;

    // **************************** Pre-Actions ***************************** //

    helper_appman_init();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_app_create(&test_p_app, &status);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(test_msg_arr, test_msg_pool);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(APPMAN_STATUS_SUCCESS, status);

    TEST_STEP(4);
    TEST_ASSERT_EQUAL(test_msg_arr+16, test_msg_pool->p_next);

    TEST_STEP(5);
    test_msg_pool = test_msg_pool->p_next;
    TEST_ASSERT_EQUAL(test_msg_arr+32, test_msg_pool->p_next);
}

/**
 * @brief The test verifies the appman_timer_create() function with the valid 
 * argument.
 * 
 * https://codebeamer.daiichi.com/item/187455
 *  
 */
void test_appman_timer_create__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    appman_timer_t test_p_timer;
    test_p_timer.handler = test_appman_timer_handler;
    test_p_timer.mode = APPMAN_TIMER_MODE_STRICT;
    test_p_timer.state = APPMAN_TIMER_STATE_INIT;
    test_p_timer.interval_ms = 100U;
    test_p_timer.p_arg = &test_message;

    appman_status_t test_p_status = APPMAN_STATUS_APP_NOT_CREATED;

    // **************************** Pre-Actions ***************************** //

    helper_appman_init();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_timer_create(&test_p_timer, &test_p_status);

    TEST_STEP(2);
    appman_timer_create(&test_p_timer, &test_p_status);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(APPMAN_STATUS_SUCCESS, test_p_status);
}

/**
 * @brief The test verifies the appman_timer_create() function with the invalid 
 * timer structure address.
 * 
 * https://codebeamer.daiichi.com/item/187457
 *  
 */
void test_appman_timer_create__invalid_argument__set_status(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    appman_status_t test_p_status = APPMAN_STATUS_SUCCESS;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_timer_create(NULL, &test_p_status);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(APPMAN_STATUS_ERROR_PARAM, test_p_status);
}

/**
 * @brief The test verifies the appman_timer_create() function with the invalid 
 * function status argument.
 * 
 * https://codebeamer.daiichi.com/item/187458
 *  
 */
void test_appman_timer_create__invalid_status_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    appman_timer_t test_p_timer;
    test_p_timer.handler = test_appman_timer_handler;
    test_p_timer.mode = APPMAN_TIMER_MODE_STRICT;
    test_p_timer.state = APPMAN_TIMER_STATE_INIT;
    test_p_timer.interval_ms = 100U;
    test_p_timer.p_arg = &test_message;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_timer_create(&test_p_timer, NULL);
}

/**
 * @brief The test verifies the appman_timer_create() function with the invalid 
 * timer handler argument.
 * 
 * https://codebeamer.daiichi.com/item/187459
 *  
 */
void test_appman_timer_create__invalid_handler_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    appman_timer_t test_p_timer;

    test_p_timer.handler = NULL;
    test_p_timer.mode = APPMAN_TIMER_MODE_STRICT;
    test_p_timer.state = APPMAN_TIMER_STATE_INIT;
    test_p_timer.interval_ms = 100U;
    test_p_timer.p_arg = &test_message;

    appman_status_t test_p_status = APPMAN_STATUS_SUCCESS;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_timer_create(&test_p_timer, &test_p_status);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(APPMAN_STATUS_ERROR_PARAM, test_p_status);
}

/**
 * @brief The test verifies the appman_timer_create() function is a call 
 * callback function with the timer run state.
 * 
 * https://codebeamer.daiichi.com/item/187460
 *  
 */
void test_appman_timer_create__when_run_state__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    appman_timer_t test_p_timer;

    test_p_timer.handler = test_appman_timer_handler;
    test_p_timer.mode = APPMAN_TIMER_MODE_SEQUANTIAL;

    // Set timer state 
    test_p_timer.state = APPMAN_TIMER_STATE_RUN; 
    test_p_timer.interval_ms = 1U;
    test_p_timer.p_arg = &test_message;
    
    appman_status_t test_p_status = APPMAN_STATUS_ERROR_PARAM;

    // **************************** Pre-Actions ***************************** //

    helper_appman_init_stub();

    // ************************** Expected Calls **************************** //

    bsp_isr_enter_critical_Ignore();
    bsp_mcu_tick_get_ExpectAndReturn(10U);
    bsp_isr_exit_critical_Ignore();

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_timer_create(&test_p_timer, &test_p_status);

    TEST_STEP(2);
    TEST_ASSERT_NOT_EQUAL_INT(1U, test_p_timer.remain_ms);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(APPMAN_STATUS_SUCCESS, test_p_status);
    
    TEST_STEP(4);
    TEST_ASSERT_EQUAL(true, cb_status);
}

/**
 * @brief The test verifies the appman_timer_create() function with the invalid 
 * mode argument.
 * 
 * https://codebeamer.daiichi.com/issue/187867
 *  
 */
void test_appman_timer_create__when_use_invalid_mode__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    appman_timer_t test_p_timer;

    test_p_timer.handler = test_appman_timer_handler;

    // Set timer mode
    test_p_timer.mode = APPMAN_TIMER_MODE_SEQUANTIAL + 1U;
    test_p_timer.state = APPMAN_TIMER_STATE_RUN; 
    test_p_timer.interval_ms = 1U;
    test_p_timer.remain_ms = 99U;
    test_p_timer.p_arg = &test_message;
    
    appman_status_t test_p_status = APPMAN_STATUS_ERROR_PARAM;

    // **************************** Pre-Actions ***************************** //

    helper_appman_init();

    // ************************** Expected Calls **************************** //

    bsp_isr_enter_critical_Ignore();
    bsp_isr_exit_critical_Ignore();

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_timer_create(&test_p_timer, &test_p_status);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(99U, test_p_timer.remain_ms);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(APPMAN_STATUS_ERROR_PARAM, test_p_status);
}


/**
 * @brief The test verifies the appman_timer_create() function call the callback 
 * function with the set timeout value in strict mode.
 * 
 * https://codebeamer.daiichi.com/item/187656
 *  
 */
void test_appman_timer_create__when_set_timeout_period__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    appman_timer_t test_p_timer;

    test_p_timer.handler = test_appman_timer_handler;

    // Set mode argument to strict
    test_p_timer.mode = APPMAN_TIMER_MODE_STRICT;
    test_p_timer.state = APPMAN_TIMER_STATE_RUN; 

    // Set timeot value
    test_p_timer.interval_ms = 5U;
    test_p_timer.p_arg = &test_message;
    
    appman_status_t test_p_status = APPMAN_STATUS_ERROR_PARAM;

    // **************************** Pre-Actions ***************************** //

    helper_appman_init_stub();

    // ************************** Expected Calls **************************** //

    bsp_isr_enter_critical_Ignore();
    bsp_isr_exit_critical_Ignore();

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_timer_create(&test_p_timer, &test_p_status);

    TEST_STEP(2);
    test_timer_cb();

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(4U, test_p_timer.remain_ms);

    TEST_STEP(4);
    test_timer_cb();

    TEST_STEP(5);
    test_timer_cb();

    TEST_STEP(6);
    test_timer_cb();

    TEST_STEP(7);
    test_timer_cb();

    TEST_STEP(8);
    TEST_ASSERT_EQUAL(test_p_timer.remain_ms, test_p_timer.interval_ms);

    TEST_STEP(9);
    TEST_ASSERT_EQUAL(APPMAN_STATUS_SUCCESS, test_p_status);

    TEST_STEP(10);
    TEST_ASSERT_EQUAL(true, cb_status);
}

/**
 * @brief The test verifies whether the counter of each created timer has 
 * changed by creating multiple timers. Likewise, the handler call is tested.
 * 
 * https://codebeamer.daiichi.com/item/188185
 *  
 */
void test_appman_timer_create__when_multiple_set_timeout__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    appman_timer_t test_first_timer;

    test_first_timer.handler = test_appman_timer_handler;

    // Set mode argument to strict
    test_first_timer.mode = APPMAN_TIMER_MODE_STRICT;
    test_first_timer.state = APPMAN_TIMER_STATE_RUN; 

    // Set timeot value
    test_first_timer.interval_ms = 5U;
    test_first_timer.p_arg = &test_message;
    
    appman_status_t test_first_status = APPMAN_STATUS_ERROR_PARAM;

    appman_timer_t test_second_timer;

    test_second_timer.handler = test_appman_second_timer_handler;

    // Set mode argument to strict
    test_second_timer.mode = APPMAN_TIMER_MODE_STRICT;
    test_second_timer.state = APPMAN_TIMER_STATE_RUN; 

    // Set timeot value
    test_second_timer.interval_ms = 5U;
    test_second_timer.p_arg = &test_message;
    
    appman_status_t test_second_status = APPMAN_STATUS_ERROR_PARAM;

    // **************************** Pre-Actions ***************************** //

    helper_appman_init_stub();

    // ************************** Expected Calls **************************** //

    bsp_isr_enter_critical_Ignore();
    bsp_isr_exit_critical_Ignore();

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_timer_create(&test_first_timer, &test_first_status);

    TEST_STEP(2);
    appman_timer_create(&test_second_timer, &test_second_status);

    TEST_STEP(3);
    test_timer_cb();

    TEST_STEP(4);
    TEST_ASSERT_EQUAL(4U, test_first_timer.remain_ms);

    TEST_STEP(5);
    TEST_ASSERT_EQUAL(4U, test_second_timer.remain_ms);

    TEST_STEP(6);
    test_timer_cb();

    TEST_STEP(7);
    test_timer_cb();

    TEST_STEP(8);
    test_timer_cb();

    TEST_STEP(9);
    test_timer_cb();

    TEST_STEP(10);
    TEST_ASSERT_EQUAL(test_first_timer.remain_ms, test_first_timer.interval_ms);

    TEST_STEP(11);
    TEST_ASSERT_EQUAL(test_second_timer.remain_ms, 
                      test_second_timer.interval_ms);

    TEST_STEP(12);
    TEST_ASSERT_EQUAL(APPMAN_STATUS_SUCCESS, test_first_status);

    TEST_STEP(13);
    TEST_ASSERT_EQUAL(APPMAN_STATUS_SUCCESS, test_second_status);

    TEST_STEP(14);
    TEST_ASSERT_EQUAL(true, first_handler_status);

    TEST_STEP(15);
    TEST_ASSERT_EQUAL(true, second_handler_status);
}

/**
 * @brief The test verifies the appman_timer_start() function successfully.
 * 
 * https://codebeamer.daiichi.com/item/187471
 *  
 */
void test_appman_timer_start__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    appman_timer_t test_p_timer;

    test_p_timer.handler = test_appman_timer_handler;
    test_p_timer.mode = APPMAN_TIMER_MODE_SEQUANTIAL;
    test_p_timer.state = APPMAN_TIMER_STATE_INIT; 
    test_p_timer.interval_ms = 1U;
    test_p_timer.p_arg = &test_message;
    
    appman_status_t test_p_status = APPMAN_STATUS_ERROR_PARAM;

    // **************************** Pre-Actions ***************************** //

    helper_appman_init();

    helper_appman_timer_create(&test_p_timer, &test_p_status);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_timer_start(&test_p_timer, &test_p_status);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(APPMAN_STATUS_SUCCESS, test_p_status);
}

/**
 * @brief The test verifies the appman_timer_start() function successfully when 
 * create multiple timers start.
 * 
 * https://codebeamer.daiichi.com/item/187868
 *  
 */
void test_appman_timer_start__create_multipler_timer__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    appman_timer_t test_first_timer;
    test_first_timer.handler = test_appman_timer_handler;
    test_first_timer.mode = APPMAN_TIMER_MODE_SEQUANTIAL;
    test_first_timer.state = APPMAN_TIMER_STATE_INIT; 
    test_first_timer.interval_ms = 1U;
    test_first_timer.p_arg = &test_message;
    
    appman_status_t test_first_status = APPMAN_STATUS_ERROR_PARAM;

    appman_timer_t test_second_timer;
    test_second_timer.handler = test_appman_timer_handler;
    test_second_timer.mode = APPMAN_TIMER_MODE_SEQUANTIAL;
    test_second_timer.state = APPMAN_TIMER_STATE_INIT; 
    test_second_timer.interval_ms = 1U;
    test_second_timer.p_arg = &test_message;
    
    appman_status_t test_second_status = APPMAN_STATUS_ERROR_PARAM;

    // **************************** Pre-Actions ***************************** //

    helper_appman_init();

    helper_appman_timer_create(&test_second_timer, &test_second_status);
    helper_appman_timer_create(&test_first_timer, &test_first_status);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_timer_start(&test_first_timer, &test_first_status);
    
    TEST_STEP(2);
    appman_timer_start(&test_second_timer, &test_second_status);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(APPMAN_STATUS_SUCCESS, test_first_status);

    TEST_STEP(4);
    TEST_ASSERT_EQUAL(APPMAN_STATUS_SUCCESS, test_second_status);
}

/**
 * @brief The test verifies the appman_timer_start() function with invalid timer 
 * structure address argument.
 * 
 * https://codebeamer.daiichi.com/item/187487
 *  
 */
void test_appman_timer_start__invalid_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    appman_status_t test_p_status = APPMAN_STATUS_SUCCESS;

    // **************************** Pre-Actions ***************************** //

    helper_appman_init();

    appman_timer_t test_p_timer;
    test_p_timer.handler = test_appman_timer_handler;
    test_p_timer.mode = APPMAN_TIMER_MODE_SEQUANTIAL;
    test_p_timer.state = APPMAN_TIMER_STATE_INIT; 
    test_p_timer.interval_ms = 0U;
    test_p_timer.remain_ms = 0U;
    test_p_timer.p_arg = &test_message;

    appman_timer_create(&test_p_timer, &test_p_status);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_timer_start(NULL, &test_p_status);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(APPMAN_STATUS_ERROR_PARAM, test_p_status);
}

/**
 * @brief The test verifies that the appman_timer_start() function failed to set
 * the state, an invalid state variable.
 * 
 *  https://codebeamer.daiichi.com/item/187489
 *  
 */
void test_appman_timer_start__invalid_status_argument__not_set_status(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    appman_timer_t test_p_timer;

    test_p_timer.handler = test_appman_timer_handler;
    test_p_timer.mode = APPMAN_TIMER_MODE_SEQUANTIAL;
    test_p_timer.state = APPMAN_TIMER_STATE_INIT; 
    test_p_timer.interval_ms = 1U;
    test_p_timer.remain_ms = 0U;
    test_p_timer.p_arg = &test_message;
    
    // **************************** Pre-Actions ***************************** //

    helper_appman_init();

    appman_status_t test_p_status;
    helper_appman_timer_create(&test_p_timer, &test_p_status);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_timer_start(&test_p_timer, NULL);
}

/**
 * @brief The test verifies the appman_timer_start() function is set to 
 * remain_ms variable and changes timer state to "APPMAN_TIMER_STATE_RUN" 
 * when the timer run state argument. 
 * 
 *  https://codebeamer.daiichi.com/item/187494
 *  
 */
void test_appman_timer_start__when_timer_run_state__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    appman_timer_t test_p_timer;

    test_p_timer.handler = test_appman_timer_handler;
    test_p_timer.mode = APPMAN_TIMER_MODE_SEQUANTIAL;

    // Set timer state to init
    test_p_timer.state = APPMAN_TIMER_STATE_INIT; 
    test_p_timer.interval_ms = 1U;
    test_p_timer.p_arg = &test_message;
    
    appman_status_t test_p_status = APPMAN_STATUS_ERROR_PARAM;

    // **************************** Pre-Actions ***************************** //

    helper_appman_init();

    helper_appman_timer_create(&test_p_timer, &test_p_status);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_timer_start(&test_p_timer, &test_p_status);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(APPMAN_STATUS_SUCCESS, test_p_status);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(11, test_p_timer.remain_ms);

    TEST_STEP(4);
    TEST_ASSERT_EQUAL(APPMAN_TIMER_STATE_RUN, test_p_timer.state);
}

/**
 * @brief The test verifies that the appman_timer_start() function fails when 
 * the timer is not created.
 * 
 *  https://codebeamer.daiichi.com/item/187498
 *  
 */
void test_appman_timer_start__when_timer_address_empty__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    appman_timer_t test_p_timer;
    test_p_timer.handler = test_appman_timer_handler;

    // Set timer mode to strict
    test_p_timer.mode = APPMAN_TIMER_MODE_STRICT;
    
    // Set timer state to run
    test_p_timer.state = APPMAN_TIMER_STATE_RUN; 
    test_p_timer.interval_ms = 0U;
    test_p_timer.remain_ms = 0U;
    test_p_timer.p_arg = &test_message;
    
    appman_status_t test_p_status = APPMAN_STATUS_ERROR_PARAM;

    // **************************** Pre-Actions ***************************** //

    helper_appman_init();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_timer_start(&test_p_timer, &test_p_status);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(APPMAN_STATUS_TIMER_NOT_CREATED, test_p_status);
}

/**
 * @brief The test verifies the appman_timer_stop() function successfully.
 * 
 * https://codebeamer.daiichi.com/item/187510
 *  
 */
void test_appman_timer_stop__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    appman_timer_t test_p_timer;

    test_p_timer.handler = test_appman_timer_handler;
    test_p_timer.mode = APPMAN_TIMER_MODE_SEQUANTIAL;

    // Set timer state to run
    test_p_timer.state = APPMAN_TIMER_STATE_RUN; 
    test_p_timer.interval_ms = 1U;
    test_p_timer.remain_ms = 0U;
    test_p_timer.p_arg = &test_message;
    
    appman_status_t test_p_status = APPMAN_STATUS_ERROR_PARAM;

    // **************************** Pre-Actions ***************************** //

    helper_appman_init();

    helper_appman_timer_create(&test_p_timer, &test_p_status);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_timer_stop(&test_p_timer, &test_p_status);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(APPMAN_STATUS_SUCCESS, test_p_status);
}

/**
 * @brief The test verifies that the appman_timer_stop() function fails when 
 * using an invalid timer state argument.
 * 
 * https://codebeamer.daiichi.com/item/187528
 *  
 */
void test_appman_timer_stop__invalid_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    appman_timer_t test_p_timer;

    test_p_timer.handler = test_appman_timer_handler;
    test_p_timer.mode = APPMAN_TIMER_MODE_SEQUANTIAL;

    // Set timer state to run
    test_p_timer.state = APPMAN_TIMER_STATE_RUN; 
    test_p_timer.interval_ms = 1U;
    test_p_timer.remain_ms = 0U;
    test_p_timer.p_arg = &test_message;
    
    appman_status_t test_p_status = APPMAN_STATUS_SUCCESS;

    // **************************** Pre-Actions ***************************** //

    helper_appman_init();

    helper_appman_timer_create(&test_p_timer, &test_p_status);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_timer_stop(NULL, &test_p_status);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(APPMAN_STATUS_ERROR_PARAM, test_p_status);
}

/**
 * @brief The test verifies that the appman_timer_stop() function fails when 
 * using an invalid status argument.
 * 
 * https://codebeamer.daiichi.com/item/187530
 *  
 */
void test_appman_timer_stop__invalid_status_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    appman_timer_t test_p_timer;

    test_p_timer.handler = test_appman_timer_handler;
    test_p_timer.mode = APPMAN_TIMER_MODE_SEQUANTIAL;

    // Set timer state to run
    test_p_timer.state = APPMAN_TIMER_STATE_RUN; 
    test_p_timer.interval_ms = 1U;
    test_p_timer.remain_ms = 0U;
    test_p_timer.p_arg = &test_message;
    
    appman_status_t test_p_status = APPMAN_STATUS_ERROR_PARAM;

    // **************************** Pre-Actions ***************************** //

    helper_appman_init();

    helper_appman_timer_create(&test_p_timer, &test_p_status);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_timer_stop(&test_p_timer, NULL);
}

/**
 * @brief The test verifies the appman_timer_stop() function is failing, 
 * without the timer not being created.
 * 
 * https://codebeamer.daiichi.com/item/187542
 *  
 */
void test_appman_timer_stop__without_crete_timer__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    appman_timer_t test_p_timer;

    test_p_timer.handler = test_appman_timer_handler;
    test_p_timer.mode = APPMAN_TIMER_MODE_SEQUANTIAL;

    // Set timer state to run
    test_p_timer.state = APPMAN_TIMER_STATE_RUN; 
    test_p_timer.interval_ms = 1U;
    test_p_timer.remain_ms = 0U;
    test_p_timer.p_arg = &test_message;
    
    appman_status_t test_p_status = APPMAN_STATUS_ERROR_PARAM;

    // **************************** Pre-Actions ***************************** //

    helper_appman_init();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_timer_stop(&test_p_timer, &test_p_status);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(APPMAN_STATUS_TIMER_NOT_CREATED, test_p_status);
}

/**
 * @brief The test verifies the appman_msg_send() function successfully.
 * 
 * https://codebeamer.daiichi.com/issue/187578
 *  
 */
void test_appman_msg_send__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    appman_msg_t test_msg;
    test_msg.dest = APPMAN_APP_ID_DIAG;
    test_msg.msg_id = APPMAN_MSG_ID_SYSTEM_STATUS;
    test_msg.p_data = "T";
    test_msg.size = (size_t)1U;

    appman_status_t test_msg_status = APPMAN_STATUS_ERROR;

    // **************************** Pre-Actions ***************************** //

    uint8_t test_msg_arr_avas[16];
    uint8_t test_msg_arr_diag[16];

    appman_app_t test_app_avas;
    test_app_avas.p_name = "TEST_APP_AVAS";
    test_app_avas.app_id = APPMAN_APP_ID_AVAS;
    test_app_avas.init = test_app_init;
    test_app_avas.run = test_app_run;
    test_app_avas.deinit = test_app_deinit;
    test_app_avas.msg_handler = test_appman_msg_avas_handler;
    test_app_avas.p_msg_pool = test_msg_arr_avas;
    test_app_avas.msg_pool_size = (size_t)16U;

    appman_status_t diag_status = APPMAN_STATUS_APP_NOT_CREATED;

    appman_app_t test_app_diag;
    test_app_diag.p_name = "TEST_APP_DIAG";
    test_app_diag.app_id = APPMAN_APP_ID_DIAG;
    test_app_diag.init = test_app_init;
    test_app_diag.run = test_app_run;
    test_app_diag.deinit = test_app_deinit;
    test_app_diag.msg_handler = test_appman_msg_diag_handler;
    test_app_diag.p_msg_pool = test_msg_arr_diag;
    test_app_diag.msg_pool_size = (size_t)16U;

    appman_status_t avas_status = APPMAN_STATUS_APP_NOT_CREATED;

    helper_appman_init();
    helper_appman_app_create(&test_app_diag, &diag_status);
    helper_appman_app_create(&test_app_avas, &avas_status);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_msg_send(&test_msg, &test_msg_status);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(APPMAN_STATUS_SUCCESS, test_msg_status);
}

/**
 * @brief The test verifies the appman_msg_send() function fails with an invalid 
 * message structure address argument.
 * 
 * https://codebeamer.daiichi.com/item/187579
 *  
 */
void test_appman_msg_send__invalid_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    appman_status_t test_msg_status = APPMAN_STATUS_ERROR;

    // **************************** Pre-Actions ***************************** //


    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_msg_send(NULL, &test_msg_status);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(APPMAN_STATUS_ERROR_PARAM, test_msg_status);
}

/**
 * @brief The test verifies the appman_msg_send() function fails with an 
 * invalid function status argument.
 * 
 * https://codebeamer.daiichi.com/item/187580
 *  
 */
void test_appman_msg_send__invalid_status_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    appman_msg_t test_msg;
    test_msg.dest = APPMAN_APP_ID_DIAG;
    test_msg.msg_id = APPMAN_MSG_ID_SYSTEM_STATUS;
    test_msg.p_data = "T";
    test_msg.size = (size_t)1U;

    // **************************** Pre-Actions ***************************** //
    
    uint8_t test_msg_arr_diag[16];

    appman_status_t diag_status = APPMAN_STATUS_APP_NOT_CREATED;

    appman_app_t test_app_diag;
    test_app_diag.p_name = "TEST_APP_DIAG";
    test_app_diag.app_id = APPMAN_APP_ID_AVAS;
    test_app_diag.init = test_app_init;
    test_app_diag.run = test_app_run;
    test_app_diag.deinit = test_app_deinit;
    test_app_diag.msg_handler = test_appman_msg_diag_handler;
    test_app_diag.p_msg_pool = test_msg_arr_diag;
    test_app_diag.msg_pool_size = (size_t)16U;
    
    helper_appman_init();
    helper_appman_app_create(&test_app_diag, &diag_status);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_msg_send(&test_msg, NULL);
}

/**
 * @brief The test verifies the appman_msg_send() function fails with invalid 
 * message argument.
 * 
 * https://codebeamer.daiichi.com/item/187581
 *  
 */
void test_appman_msg_send__invalid_msg_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    appman_msg_t test_msg;
    test_msg.dest = APPMAN_APP_ID_AVAS;
    test_msg.msg_id = APPMAN_MSG_ID_SYSTEM_STATUS;

    // Set data argument to NULL
    test_msg.p_data = NULL;
    test_msg.size = (size_t)16U;

    appman_status_t test_msg_status = APPMAN_STATUS_APP_NOT_CREATED;

    // **************************** Pre-Actions ***************************** //

    uint8_t test_msg_arr_diag[16];

    appman_status_t diag_status = APPMAN_STATUS_APP_NOT_CREATED;

    appman_app_t test_app_diag;
    test_app_diag.p_name = "TEST_APP_DIAG";
    test_app_diag.app_id = APPMAN_APP_ID_DIAG;
    test_app_diag.init = test_app_init;
    test_app_diag.run = test_app_run;
    test_app_diag.deinit = test_app_deinit;
    test_app_diag.msg_handler = test_appman_msg_diag_handler;
    test_app_diag.p_msg_pool = test_msg_arr_diag;
    test_app_diag.msg_pool_size = (size_t)16U;

    helper_appman_init();
    helper_appman_app_create(&test_app_diag, &diag_status);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_msg_send(&test_msg, &test_msg_status);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(APPMAN_STATUS_ERROR_PARAM, test_msg_status);
}

/**
 * @brief The test verifies the appman_msg_send() function fails when the 
 * selected all applications for the message destination and used an invalid 
 * function status argument.
 * 
 * https://codebeamer.daiichi.com/item/187583
 *  
 */
void test_appman_msg_send__invalid_arg_when_all_id_send__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    appman_msg_t test_msg;
    // Set message destination argument
    test_msg.dest = APPMAN_APP_ID_ALL;
    test_msg.p_data = "T";
    test_msg.size = (size_t)1U;

    // **************************** Pre-Actions ***************************** //
    uint8_t test_msg_arr_avas[16];
    uint8_t test_msg_arr_diag[16];

    appman_app_t test_app_avas;
    test_app_avas.p_name = "TEST_APP_AVAS";
    test_app_avas.app_id = APPMAN_APP_ID_AVAS;
    test_app_avas.init = test_app_init;
    test_app_avas.run = test_app_run;
    test_app_avas.deinit = test_app_deinit;
    test_app_avas.msg_handler = test_appman_msg_avas_handler;
    test_app_avas.p_msg_pool = test_msg_arr_avas;
    test_app_avas.msg_pool_size = (size_t)16U;

    appman_status_t diag_status = APPMAN_STATUS_APP_NOT_CREATED;

    appman_app_t test_app_diag;
    test_app_diag.p_name = "TEST_APP_DIAG";
    test_app_diag.app_id = APPMAN_APP_ID_DIAG;
    test_app_diag.init = test_app_init;
    test_app_diag.run = test_app_run;
    test_app_diag.deinit = test_app_deinit;
    test_app_diag.msg_handler = test_appman_msg_diag_handler;
    test_app_diag.p_msg_pool = test_msg_arr_diag;
    test_app_diag.msg_pool_size = (size_t)16U;

    appman_status_t avas_status = APPMAN_STATUS_APP_NOT_CREATED;

    helper_appman_init();
    helper_appman_app_create(&test_app_diag, &diag_status);
    helper_appman_app_create(&test_app_avas, &avas_status);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_msg_send(&test_msg, NULL);
}

/**
 * @brief The test verifies the appman_msg_send() function when set status to 
 * "APPMAN_STATUS_NO_MSG_POOL".
 * 
 * https://codebeamer.daiichi.com/item/187585
 *  
 */
void test_appman_msg_send__when_message_pool_full__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    char test_message[24] = "123456798912345678912345";
    appman_msg_t test_msg;
    test_msg.dest = APPMAN_APP_ID_DIAG;
    test_msg.p_data = &test_message;

    // Increase message size argument
    test_msg.size = (size_t)24U;

    appman_status_t test_msg_status = APPMAN_STATUS_ERROR;

    // **************************** Pre-Actions ***************************** //

    appman_app_t test_app_avas;
    test_app_avas.p_name = "TEST_APP_AVAS";
    test_app_avas.app_id = APPMAN_APP_ID_AVAS;
    test_app_avas.init = test_app_init;
    test_app_avas.run = test_app_run;
    test_app_avas.deinit = test_app_deinit;
    test_app_avas.msg_handler = test_appman_msg_avas_handler;

    // Set message argument to invalid
    test_app_avas.p_msg_pool = NULL;
    test_app_avas.msg_pool_size = (size_t)16U;

    appman_status_t diag_status = APPMAN_STATUS_APP_NOT_CREATED;

    appman_app_t test_app_diag;
    test_app_diag.p_name = "TEST_APP_DIAG";
    test_app_diag.app_id = APPMAN_APP_ID_DIAG;
    test_app_diag.init = test_app_init;
    test_app_diag.run = test_app_run;
    test_app_diag.deinit = test_app_deinit;
    test_app_diag.msg_handler = test_appman_msg_diag_handler;

    // Set message argument to invalid
    test_app_diag.p_msg_pool = NULL;
    test_app_diag.msg_pool_size = (size_t)16U;

    appman_status_t avas_status = APPMAN_STATUS_APP_NOT_CREATED;

    helper_appman_init();
    helper_appman_app_create(&test_app_diag, &diag_status);
    helper_appman_app_create(&test_app_avas, &avas_status);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_msg_send(&test_msg, &test_msg_status);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(APPMAN_STATUS_NO_MSG_POOL, test_msg_status);
}

/**
 * @brief The test verifies the appman_msg_send() function is set function 
 * status to "APPMAN_STATUS_NO_MSG_POOL" when the selected all applications 
 * for the message destination and used an invalid message poll, size argument.
 * 
 * https://codebeamer.daiichi.com/item/187591
 *  
 */
void test_appman_msg_send__when_all_msg_send__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    char test_message[24] = "01234567890123456789012";
    appman_msg_t test_msg;
    // Set message destination argument
    test_msg.dest = APPMAN_APP_ID_ALL;
    test_msg.p_data = &test_message;
    // Increase message size argument
    test_msg.size = (size_t)24U;

    appman_status_t test_msg_status = APPMAN_STATUS_ERROR;

    // **************************** Pre-Actions ***************************** //

    appman_app_t test_app_avas;
    test_app_avas.p_name = "TEST_APP_AVAS";
    test_app_avas.app_id = APPMAN_APP_ID_AVAS;
    test_app_avas.init = test_app_init;
    test_app_avas.run = test_app_run;
    test_app_avas.deinit = test_app_deinit;
    test_app_avas.msg_handler = test_appman_msg_avas_handler;
    
    // Set message argument to invalid
    test_app_avas.p_msg_pool = NULL;
    test_app_avas.msg_pool_size = (size_t)16U;

    appman_status_t diag_status = APPMAN_STATUS_APP_NOT_CREATED;

    appman_app_t test_app_diag;
    test_app_diag.p_name = "TEST_APP_DIAG";
    test_app_diag.app_id = APPMAN_APP_ID_DIAG;
    test_app_diag.init = test_app_init;
    test_app_diag.run = test_app_run;
    test_app_diag.deinit = test_app_deinit;
    test_app_diag.msg_handler = test_appman_msg_diag_handler;

    // Set message argument to invalid
    test_app_diag.p_msg_pool = NULL;
    test_app_diag.msg_pool_size = (size_t)16U;

    appman_status_t avas_status = APPMAN_STATUS_APP_NOT_CREATED;

    helper_appman_init();
    helper_appman_app_create(&test_app_diag, &diag_status);
    helper_appman_app_create(&test_app_avas, &avas_status);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_msg_send(&test_msg, &test_msg_status);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(APPMAN_STATUS_NO_MSG_POOL, test_msg_status);
}

/**
 * @brief The test verifies the appman_process() function calls the handler 
 * when timeout and verifies that appman_app_process receives a message in the 
 * message inbox of the relevant application.
 * 
 * https://codebeamer.daiichi.com/item/187628
 *  
 */
void test_appman_process__usual_run__success(void)
{
    TEST_CASE();
    
    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //
    uint8_t test_msg_arr_avas[32];
    uint8_t test_msg_arr_diag[32];

    appman_status_t diag_status = APPMAN_STATUS_APP_NOT_CREATED;
    appman_status_t avas_status = APPMAN_STATUS_APP_NOT_CREATED;
    appman_status_t test_p_status = APPMAN_STATUS_ERROR_PARAM;

    appman_app_t test_app_avas;
    test_app_avas.p_name = "TEST_APP_AVAS";
    test_app_avas.app_id = APPMAN_APP_ID_AVAS;
    test_app_avas.init = test_app_init;
    test_app_avas.run = test_app_run;
    test_app_avas.deinit = test_app_deinit;
    test_app_avas.msg_handler = test_appman_msg_avas_handler;
    test_app_avas.p_msg_pool = test_msg_arr_avas;
    test_app_avas.msg_pool_size = (size_t)32U;

    appman_app_t test_app_diag;
    test_app_diag.p_name = "TEST_APP_DIAG";
    test_app_diag.app_id = APPMAN_APP_ID_DIAG;
    test_app_diag.init = test_app_diag_send_init;
    test_app_diag.run = test_app_run;
    test_app_diag.deinit = test_app_deinit;
    test_app_diag.msg_handler = test_appman_msg_diag_handler;
    test_app_diag.p_msg_pool = test_msg_arr_diag;
    test_app_diag.msg_pool_size = (size_t)32U;
   
    appman_timer_t test_p_timer;
    test_p_timer.handler = test_appman_timer_handler;
    test_p_timer.mode = APPMAN_TIMER_MODE_SEQUANTIAL;
    test_p_timer.state = APPMAN_TIMER_STATE_RUN; 
    test_p_timer.interval_ms = 10U;
    test_p_timer.p_arg = &test_message;

    // Initialize appman
    helper_appman_init();

    // Create application
    helper_appman_app_create(&test_app_diag, &diag_status);
    helper_appman_app_create(&test_app_avas, &avas_status);

    // Create timer
    helper_appman_timer_create_with_ignore(&test_p_timer, &test_p_status);

    // ************************** Expected Calls **************************** //

    // Set the current tick 
    bsp_mcu_tick_get_Stub(test_stub_tick_get);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_process();

    TEST_STEP(2);
    appman_process();

    TEST_STEP(3);
    appman_process();

    TEST_STEP(4);
    appman_process();
}

/**
 * @brief The test verifies that appman_process() calls the handler when sending
 * messages for all applications.
 * 
 * https://codebeamer.daiichi.com/item/187703
 *  
 */
void test_appman_process__when_select_all_app_to_destination__success(void)
{
    TEST_CASE();
    
    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    uint8_t test_msg_arr_avas[32];
    uint8_t test_msg_arr_diag[32];
    uint8_t test_msg_arr_can[32];
    uint8_t test_msg_arr_main[32];

    appman_status_t diag_status = APPMAN_STATUS_APP_NOT_CREATED;
    appman_status_t avas_status = APPMAN_STATUS_APP_NOT_CREATED;
    appman_status_t main_status = APPMAN_STATUS_APP_NOT_CREATED;
    appman_status_t can_status = APPMAN_STATUS_APP_NOT_CREATED;

    appman_app_t test_app_avas;
    test_app_avas.p_name = "TEST_APP_AVAS";
    test_app_avas.app_id = APPMAN_APP_ID_AVAS;
    test_app_avas.init = test_app_avas_init;
    test_app_avas.run = test_app_run;
    test_app_avas.deinit = test_app_deinit;
    test_app_avas.msg_handler = test_appman_msg_avas_handler;
    test_app_avas.p_msg_pool = test_msg_arr_avas;
    test_app_avas.msg_pool_size = (size_t)32U;

    appman_app_t test_app_diag;
    test_app_diag.p_name = "TEST_APP_DIAG";
    test_app_diag.app_id = APPMAN_APP_ID_DIAG;
    test_app_diag.init = test_app_diag_init;
    test_app_diag.run = test_app_run;
    test_app_diag.deinit = test_app_deinit;
    test_app_diag.msg_handler = test_appman_msg_diag_handler;
    test_app_diag.p_msg_pool = test_msg_arr_diag;
    test_app_diag.msg_pool_size = (size_t)32U;

    appman_app_t test_app_main;
    test_app_main.p_name = "TEST_APP_MAIN";
    test_app_main.app_id = APPMAN_APP_ID_MAIN;
    test_app_main.init = test_app_main_init;
    test_app_main.run = test_app_run;
    test_app_main.deinit = test_app_deinit;
    test_app_main.msg_handler = test_appman_msg_main_handler;
    test_app_main.p_msg_pool = test_msg_arr_main;
    test_app_main.msg_pool_size = (size_t)32U;

    appman_app_t test_app_can;
    test_app_can.p_name = "TEST_APP_CAN";
    test_app_can.app_id = APPMAN_APP_ID_CAN;
    test_app_can.init = test_app_can_msg_send_init;
    test_app_can.run = test_app_run;
    test_app_can.deinit = test_app_deinit;
    test_app_can.msg_handler = test_appman_msg_can_handler;
    test_app_can.p_msg_pool = test_msg_arr_can;
    test_app_can.msg_pool_size = (size_t)32U;
    
    // Initialize appman
    helper_appman_init();

    // Create application
    helper_appman_app_create(&test_app_can, &can_status);
    helper_appman_app_create(&test_app_avas, &avas_status);
    helper_appman_app_create(&test_app_main, &main_status);
    helper_appman_app_create(&test_app_diag, &diag_status);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_process();

    TEST_STEP(2);
    appman_process();

    TEST_STEP(3);
    appman_process();

    TEST_STEP(4);
    appman_process();

    TEST_STEP(5);
    appman_process();
}

/**
 * @brief The test verifies that appman_process() adds the given message to the 
 * message inbox of the given application when sends multiple messages.
 * The appman_process() function updates the application message inbox to 
 * the next application message inbox.
 * 
 * https://codebeamer.daiichi.com/item/188358
 *  
 */
void test_appman_process__when_msg_inbox_not_null__success(void)
{
    TEST_CASE();
    
    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //
    uint8_t test_msg_arr_avas[32];
    uint8_t test_msg_arr_diag[48];
    uint8_t test_msg_arr_can[32];
    uint8_t test_msg_arr_main[32];

    appman_status_t diag_status = APPMAN_STATUS_APP_NOT_CREATED;
    appman_status_t avas_status = APPMAN_STATUS_APP_NOT_CREATED;
    appman_status_t main_status = APPMAN_STATUS_APP_NOT_CREATED;

    appman_app_t test_app_avas;
    test_app_avas.p_name = "TEST_APP_AVAS";
    test_app_avas.app_id = APPMAN_APP_ID_AVAS;
    test_app_avas.init = test_app_avas_msg_send_init;
    test_app_avas.run = test_app_run;
    test_app_avas.deinit = test_app_deinit;
    test_app_avas.msg_handler = test_appman_msg_avas_handler;
    test_app_avas.p_msg_pool = test_msg_arr_avas;
    test_app_avas.msg_pool_size = (size_t)32U;

    appman_app_t test_app_diag;
    test_app_diag.p_name = "TEST_APP_DIAG";
    test_app_diag.app_id = APPMAN_APP_ID_DIAG;
    test_app_diag.init = test_app_diag_init;
    test_app_diag.run = test_app_run;
    test_app_diag.deinit = test_app_deinit;
    test_app_diag.msg_handler = test_appman_msg_diag_second_handler;
    test_app_diag.p_msg_pool = test_msg_arr_diag;
    test_app_diag.msg_pool_size = (size_t)48U;

    appman_app_t test_app_can;
    test_app_can.p_name = "TEST_APP_CAN";
    test_app_can.app_id = APPMAN_APP_ID_CAN;
    test_app_can.init = test_app_can_msg_second_send_init;
    test_app_can.run = test_app_run;
    test_app_can.deinit = test_app_deinit;
    test_app_can.msg_handler = test_appman_msg_can_handler;
    test_app_can.p_msg_pool = test_msg_arr_can;
    test_app_can.msg_pool_size = (size_t)32U;

    appman_app_t test_app_main;
    test_app_main.p_name = "TEST_APP_MAIN";
    test_app_main.app_id = APPMAN_APP_ID_MAIN;
    test_app_main.init = test_app_main_msg_second_send_init;
    test_app_main.run = test_app_run;
    test_app_main.deinit = test_app_deinit;
    test_app_main.msg_handler = test_appman_msg_main_handler;
    test_app_main.p_msg_pool = test_msg_arr_main;
    test_app_main.msg_pool_size = (size_t)32U;
    
    // Initialize appman
    helper_appman_init();

    // Create application
    helper_appman_app_create(&test_app_avas, &avas_status);
    helper_appman_app_create(&test_app_main, &main_status);
    helper_appman_app_create(&test_app_can, &main_status);
    helper_appman_app_create(&test_app_diag, &diag_status);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_process();

    TEST_STEP(2);
    appman_process();

    TEST_STEP(3);
    appman_process();

    TEST_STEP(4);
    appman_process();

    TEST_STEP(5);
    appman_process();
}

/**
 * @brief The test verifies that appman_process() calls the handler when timeout 
 * and sends messages from one application to another at the same time.
 * 
 * https://codebeamer.daiichi.com/item/188164
 *  
 */
void test_appman_process__when_send_multiple_message_and_timeout__success(void)
{
    TEST_CASE();
    
    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    uint8_t test_msg_arr_avas[32];
    uint8_t test_msg_arr_diag[32];
    uint8_t test_msg_arr_can[48];
    uint8_t test_msg_arr_main[32];

    appman_status_t diag_status = APPMAN_STATUS_APP_NOT_CREATED;
    appman_status_t avas_status = APPMAN_STATUS_APP_NOT_CREATED;
    appman_status_t main_status = APPMAN_STATUS_APP_NOT_CREATED;
    appman_status_t can_status = APPMAN_STATUS_APP_NOT_CREATED;

    appman_app_t test_app_avas;
    test_app_avas.p_name = "TEST_APP_AVAS";
    test_app_avas.app_id = APPMAN_APP_ID_AVAS;
    test_app_avas.init = test_app_avas_init;
    test_app_avas.run = test_app_run;
    test_app_avas.deinit = test_app_deinit;
    test_app_avas.msg_handler = test_appman_msg_avas_handler;
    test_app_avas.p_msg_pool = test_msg_arr_avas;
    test_app_avas.msg_pool_size = (size_t)32U;

    appman_app_t test_app_diag;
    test_app_diag.p_name = "TEST_APP_DIAG";
    test_app_diag.app_id = APPMAN_APP_ID_DIAG;
    test_app_diag.init = test_app_diag_send_init;
    test_app_diag.run = test_app_run;
    test_app_diag.deinit = test_app_deinit;
    test_app_diag.msg_handler = test_appman_msg_diag_handler;
    test_app_diag.p_msg_pool = test_msg_arr_diag;
    test_app_diag.msg_pool_size = (size_t)32U;

    appman_app_t test_app_main;
    test_app_main.p_name = "TEST_APP_MAIN";
    test_app_main.app_id = APPMAN_APP_ID_MAIN;
    test_app_main.init = test_app_main_msg_send_init;
    test_app_main.run = test_app_run;
    test_app_main.deinit = test_app_deinit;
    test_app_main.msg_handler = test_appman_msg_main_handler;
    test_app_main.p_msg_pool = test_msg_arr_main;
    test_app_main.msg_pool_size = (size_t)32U;

    appman_app_t test_app_can;
    test_app_can.p_name = "TEST_APP_CAN";
    test_app_can.app_id = APPMAN_APP_ID_CAN;
    test_app_can.init = test_app_can_init;
    test_app_can.run = test_app_run;
    test_app_can.deinit = test_app_deinit;
    test_app_can.msg_handler = test_appman_msg_can_handler;
    test_app_can.p_msg_pool = test_msg_arr_can;
    test_app_can.msg_pool_size = (size_t)48U;

    appman_timer_t test_first_timer;
    test_first_timer.handler = test_appman_timer_handler;
    test_first_timer.mode = APPMAN_TIMER_MODE_SEQUANTIAL;
    test_first_timer.state = APPMAN_TIMER_STATE_RUN; 
    test_first_timer.interval_ms = 10U;
    test_first_timer.p_arg = &test_message;
    
    appman_status_t test_first_status = APPMAN_STATUS_ERROR_PARAM;

    appman_timer_t test_second_timer;
    test_second_timer.handler = test_appman_timer_handler;
    test_second_timer.mode = APPMAN_TIMER_MODE_SEQUANTIAL;
    test_second_timer.state = APPMAN_TIMER_STATE_RUN; 
    test_second_timer.interval_ms = 10U;
    test_second_timer.p_arg = &test_message;
    
    appman_status_t test_second_status = APPMAN_STATUS_ERROR_PARAM;
    
    // Initialize appman
    helper_appman_init();

    // Create application
    helper_appman_app_create(&test_app_can, &can_status);
    helper_appman_app_create(&test_app_avas, &avas_status);
    helper_appman_app_create(&test_app_main, &main_status);
    helper_appman_app_create(&test_app_diag, &diag_status);

    // Create timer
    helper_appman_timer_create_with_ignore(&test_first_timer, 
                                           &test_first_status);
                                           
    helper_appman_timer_create_with_ignore(&test_second_timer, 
                                           &test_second_status);

    // Set the current tick 
    bsp_mcu_tick_get_Stub(test_stub_tick_get);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_process();

    TEST_STEP(2);
    appman_process();

    TEST_STEP(3);
    appman_process();

    TEST_STEP(4);
    appman_process();

    TEST_STEP(5);
    appman_process();
}

/**
 * @brief The test verifies that appman_process() calls the init and run 
 * functions sequentially and continues the calls again from the beginning 
 * at the end of the calls.
 * 
 * https://codebeamer.daiichi.com/item/188181
 *  
 */
void test_appman_process__init_and_run_function__success(void)
{
    TEST_CASE();
    
    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    uint8_t test_msg_arr_avas[32];
    uint8_t test_msg_arr_diag[32];
    uint8_t test_msg_arr_can[32];
    uint8_t test_msg_arr_main[32];

    appman_status_t diag_status = APPMAN_STATUS_APP_NOT_CREATED;
    appman_status_t avas_status = APPMAN_STATUS_APP_NOT_CREATED;
    appman_status_t main_status = APPMAN_STATUS_APP_NOT_CREATED;
    appman_status_t can_status = APPMAN_STATUS_APP_NOT_CREATED;

    appman_app_t test_app_avas;
    test_app_avas.p_name = "TEST_APP_AVAS";
    test_app_avas.app_id = APPMAN_APP_ID_AVAS;
    test_app_avas.init = test_app_avas_init;
    test_app_avas.run = test_app_avas_run;
    test_app_avas.deinit = test_app_deinit;
    test_app_avas.msg_handler = test_appman_msg_avas_handler;
    test_app_avas.p_msg_pool = test_msg_arr_avas;
    test_app_avas.msg_pool_size = (size_t)32U;

    appman_app_t test_app_diag;
    test_app_diag.p_name = "TEST_APP_DIAG";
    test_app_diag.app_id = APPMAN_APP_ID_DIAG;
    test_app_diag.init = test_app_diag_init;
    test_app_diag.run = test_app_diag_run;
    test_app_diag.deinit = test_app_deinit;
    test_app_diag.msg_handler = test_appman_msg_diag_handler;
    test_app_diag.p_msg_pool = test_msg_arr_diag;
    test_app_diag.msg_pool_size = (size_t)32U;

    appman_app_t test_app_main;
    test_app_main.p_name = "TEST_APP_MAIN";
    test_app_main.app_id = APPMAN_APP_ID_MAIN;
    test_app_main.init = test_app_main_init;
    test_app_main.run = test_app_main_run;
    test_app_main.deinit = test_app_deinit;
    test_app_main.msg_handler = test_appman_msg_main_handler;
    test_app_main.p_msg_pool = test_msg_arr_main;
    test_app_main.msg_pool_size = (size_t)32U;

    appman_app_t test_app_can;
    test_app_can.p_name = "TEST_APP_CAN";
    test_app_can.app_id = APPMAN_APP_ID_CAN;
    test_app_can.init = test_app_can_init;
    test_app_can.run = test_app_can_run;
    test_app_can.deinit = test_app_deinit;
    test_app_can.msg_handler = test_appman_msg_can_handler;
    test_app_can.p_msg_pool = test_msg_arr_can;
    test_app_can.msg_pool_size = (size_t)32U;

    // Initialize appman
    helper_appman_init();

    // Create application
    helper_appman_app_create(&test_app_can, &can_status);
    helper_appman_app_create(&test_app_avas, &avas_status);
    helper_appman_app_create(&test_app_main, &main_status);
    helper_appman_app_create(&test_app_diag, &diag_status);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_process();

    TEST_STEP(2);
    appman_process();

    TEST_STEP(3);
    appman_process();

    TEST_STEP(4);
    appman_process();

    TEST_STEP(5);
    appman_process();

    TEST_STEP(6);
    appman_process();

    TEST_STEP(7);
    appman_process();

    TEST_STEP(8);
    appman_process();

    TEST_STEP(9);
    appman_process();
}

/**
 * @brief The test verifies that appman_process() uses the run handler with two
 * apps sending messages to each other.
 * 
 * https://codebeamer.daiichi.com/item/188380
 *  
 */
void test_appman_process__call_app_run_handler__success(void)
{
    TEST_CASE();
    
    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //
    uint8_t test_msg_arr_avas[16U];
    uint8_t test_msg_arr_diag[16U];

    appman_status_t diag_status = APPMAN_STATUS_APP_NOT_CREATED;
    appman_status_t avas_status = APPMAN_STATUS_APP_NOT_CREATED;

    appman_app_t test_app_avas;
    test_app_avas.p_name = "TEST_APP_AVAS";
    test_app_avas.app_id = APPMAN_APP_ID_AVAS;
    test_app_avas.init = test_app_avas_init;
    test_app_avas.run = test_app_avas_send_msg_run;
    test_app_avas.deinit = test_app_deinit;
    test_app_avas.msg_handler = test_appman_msg_avas_handler;
    test_app_avas.p_msg_pool = test_msg_arr_avas;
    test_app_avas.msg_pool_size = (size_t)16U;

    appman_app_t test_app_diag;
    test_app_diag.p_name = "TEST_APP_DIAG";
    test_app_diag.app_id = APPMAN_APP_ID_DIAG;
    test_app_diag.init = test_app_diag_init;
    test_app_diag.run = test_app_diag_send_msg_run;
    test_app_diag.deinit = test_app_deinit;
    test_app_diag.msg_handler = test_appman_msg_diag_handler;
    test_app_diag.p_msg_pool = test_msg_arr_diag;
    test_app_diag.msg_pool_size = (size_t)16U;
   
    // Initialize appman
    helper_appman_init();

    // Create application
    helper_appman_app_create(&test_app_diag, &diag_status);
    helper_appman_app_create(&test_app_avas, &avas_status);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    appman_process();

    TEST_STEP(2);
    appman_process();
    
    TEST_STEP(3);
    appman_process();
    
    TEST_STEP(4);
    appman_process();

    TEST_STEP(5);
    appman_process();
    
    TEST_STEP(6);
    appman_process();
}

/*******************************************************************************
 * Private Functions Definitions
 ******************************************************************************/
static void test_cb_stub(bsp_isr_t isr, 
                         bsp_isr_cb_t callback, 
                         int cmock_num_calls)
{
    (void)cmock_num_calls;

    cb_status = true;

    test_timer_cb = callback;

    TEST_ASSERT_EQUAL(TEST_APP_ISR_SYSTICK, isr);
}

static void test_app_init(void) 
{
    TEST_MESSAGE("CALLED APP INIT");
}

static void test_app_avas_init(void) 
{
    TEST_MESSAGE("CALLED AVAS INIT");
}

static void test_app_diag_init(void) 
{
    TEST_MESSAGE("CALLED DIAG INIT");
}

static void test_app_main_init(void) 
{
    TEST_MESSAGE("CALLED MAIN INIT");
}

static void test_app_can_init(void) 
{
    TEST_MESSAGE("CALLED CAN INIT");
}

static void test_app_avas_msg_send_init(void) 
{
    TEST_MESSAGE("CALLED AVAS INIT");

    appman_msg_t test_msg_avas;
    test_msg_avas.dest = APPMAN_APP_ID_DIAG;
    test_msg_avas.p_data = &test_data[1];
    test_msg_avas.size = (size_t)1;

    appman_status_t test_avas_status = APPMAN_STATUS_ERROR;

    // Send message
    appman_msg_send(&test_msg_avas, &test_avas_status);
}

static void test_app_can_msg_second_send_init(void) 
{
    TEST_MESSAGE("CALLED CAN INIT");

    appman_status_t test_msg_status = APPMAN_STATUS_ERROR;

    appman_msg_t test_msg_can;
    test_msg_can.dest = APPMAN_APP_ID_DIAG;
    test_msg_can.p_data = &test_data[2];
    test_msg_can.size = (size_t)1U;

    // Send message
    appman_msg_send(&test_msg_can, &test_msg_status);
}

static void test_app_main_msg_second_send_init(void) 
{
    TEST_MESSAGE("CALLED MAIN INIT");

    appman_msg_t test_msg_main;
    test_msg_main.dest = APPMAN_APP_ID_DIAG;
    test_msg_main.p_data = &test_data[2];
    test_msg_main.size = (size_t)1U;

    appman_status_t test_msg_status = APPMAN_STATUS_ERROR;

    // Send message
    appman_msg_send(&test_msg_main, &test_msg_status);
    appman_msg_send(&test_msg_main, &test_msg_status);
}

static void test_app_main_msg_send_init(void) 
{
    TEST_MESSAGE("CALLED MAIN INIT");

    appman_msg_t test_msg_diag;
    test_msg_diag.dest = APPMAN_APP_ID_CAN;
    test_msg_diag.p_data = &test_message;
    test_msg_diag.size = (size_t)16U;

    appman_status_t test_msg_status = APPMAN_STATUS_ERROR;

    // Send message
    appman_msg_send(&test_msg_diag, &test_msg_status);
    appman_msg_send(&test_msg_diag, &test_msg_status);
}

static void test_app_can_msg_send_init(void) 
{
    TEST_MESSAGE("CALLED CAN INIT");

    appman_status_t test_msg_status = APPMAN_STATUS_ERROR;

    appman_msg_t test_msg_diag;
    test_msg_diag.dest = APPMAN_APP_ID_ALL;
    test_msg_diag.p_data = &test_message;
    test_msg_diag.size = (size_t)16U;

    // Send message
    appman_msg_send(&test_msg_diag, &test_msg_status);
}

static void test_app_diag_send_init(void) 
{
    TEST_MESSAGE("CALLED DIAG INIT");

    appman_msg_t test_msg_diag;
    test_msg_diag.dest = APPMAN_APP_ID_AVAS;
    test_msg_diag.p_data = &test_message;
    test_msg_diag.size = (size_t)16U;

    appman_status_t test_msg_status = APPMAN_STATUS_ERROR;

    // Send message
    appman_msg_send(&test_msg_diag, &test_msg_status);
    appman_msg_send(&test_msg_diag, &test_msg_status);
}

static void test_app_run(void)
{
    TEST_MESSAGE("CALLED TEST APP RUN");
}

static void test_app_main_run(void)
{
    TEST_MESSAGE("CALLED MAIN APP RUN");
}

static void test_app_can_run(void)
{
    TEST_MESSAGE("CALLED CAN APP RUN");
}

static void test_app_diag_run(void)
{
    TEST_MESSAGE("CALLED DIAG APP RUN");
}

static void test_app_diag_send_msg_run(void)
{
    TEST_MESSAGE("CALLED DIAG APP RUN");

    appman_msg_t test_msg_diag;
    test_msg_diag.dest = APPMAN_APP_ID_AVAS;
    test_msg_diag.p_data = &test_message;
    test_msg_diag.size = (size_t)16U;

    appman_status_t test_msg_status = APPMAN_STATUS_ERROR;

    // Send message
    appman_msg_send(&test_msg_diag, &test_msg_status);
}

static void test_app_avas_send_msg_run(void)
{
    TEST_MESSAGE("CALLED AVAS APP RUN");

    appman_msg_t test_msg_avas;
    test_msg_avas.dest = APPMAN_APP_ID_DIAG;
    test_msg_avas.p_data = &test_message;
    test_msg_avas.size = (size_t)16U;

    appman_status_t test_msg_status = APPMAN_STATUS_ERROR;

    // Send message
    appman_msg_send(&test_msg_avas, &test_msg_status);
}

static void test_app_avas_run(void)
{
    TEST_MESSAGE("CALLED AVAS APP RUN");
}

static void test_app_deinit(void)
{
    TEST_MESSAGE("CALLED TEST APP DEINIT");
}

static void test_appman_timer_handler(void *p_arg)
{
    first_handler_status = true;

    TEST_ASSERT_EQUAL(&test_message, p_arg);

    TEST_MESSAGE("CALLED TEST TIMER HANDLER");
}

static void test_appman_second_timer_handler(void *p_arg)
{
    second_handler_status = true;

    TEST_ASSERT_EQUAL(&test_message, p_arg);

    TEST_MESSAGE("CALLED SECOND TIMER HANDLER");
}

static void test_appman_msg_avas_handler(void *p_arg)
{
    TEST_MESSAGE("CALLED AVAS MSG HANDLER");

    appman_msg_t * p_tmp_msg = (appman_msg_t *)p_arg;

    TEST_ASSERT_EQUAL(APPMAN_APP_ID_AVAS, p_tmp_msg->dest);
    TEST_ASSERT_EQUAL(&test_message, p_tmp_msg->p_data);
    TEST_ASSERT_EQUAL((size_t)16U, p_tmp_msg->size);
}

static void test_appman_msg_diag_handler(void *p_arg)
{
    TEST_MESSAGE("CALLED DIAG MSG HANDLER");

    appman_msg_t * p_tmp_msg = (appman_msg_t *)p_arg;

    TEST_ASSERT_EQUAL(APPMAN_APP_ID_DIAG, p_tmp_msg->dest);
    TEST_ASSERT_EQUAL(&test_message, p_tmp_msg->p_data);
    TEST_ASSERT_EQUAL((size_t)16U, p_tmp_msg->size);
}

static void test_appman_msg_diag_second_handler(void *p_arg)
{
    TEST_MESSAGE("CALLED DIAG MSG HANDLER");

    appman_msg_t * p_tmp_msg = (appman_msg_t *)p_arg;

    static uint8_t cnt = 0U;

    TEST_ASSERT_EQUAL(APPMAN_APP_ID_DIAG, p_tmp_msg->dest);

    if (0U == cnt)
    {
        TEST_ASSERT_EQUAL(&test_data[1], p_tmp_msg->p_data);
        TEST_ASSERT_EQUAL((size_t)1U, p_tmp_msg->size);
    }
    else if (1U == cnt)
    {
        TEST_ASSERT_EQUAL(&test_data[2], p_tmp_msg->p_data);
        TEST_ASSERT_EQUAL((size_t)1U, p_tmp_msg->size);
    }
    else
    {
        TEST_ASSERT_EQUAL(&test_data[2], p_tmp_msg->p_data);
        TEST_ASSERT_EQUAL((size_t)1U, p_tmp_msg->size);        
    }
    cnt++;
}

static void test_appman_msg_can_handler(void *p_arg)
{
    TEST_MESSAGE("CALLED CAN MSG HANDLER");

    appman_msg_t * p_tmp_msg = (appman_msg_t *)p_arg;

    TEST_ASSERT_EQUAL(APPMAN_APP_ID_CAN, p_tmp_msg->dest);
    TEST_ASSERT_EQUAL(&test_message, p_tmp_msg->p_data);
    TEST_ASSERT_EQUAL((size_t)16U, p_tmp_msg->size);
}

static void test_appman_msg_main_handler(void *p_arg)
{
    TEST_MESSAGE("CALLED MAIN MSG HANDLER");

    appman_msg_t * p_tmp_msg = (appman_msg_t *)p_arg;

    TEST_ASSERT_EQUAL(APPMAN_APP_ID_MAIN, p_tmp_msg->dest);
    TEST_ASSERT_EQUAL(&test_message, p_tmp_msg->p_data);
    TEST_ASSERT_EQUAL((size_t)16U, p_tmp_msg->size);
}

static void helper_appman_init()
{
    bsp_isr_cb_set_ExpectAnyArgs();
    appman_init();
}

static void helper_appman_init_stub()
{
    bsp_isr_cb_set_Stub(test_cb_stub);
    appman_init();
}

static void helper_appman_timer_create(appman_timer_t *p_timer, 
                                       appman_status_t *p_status)
{
    bsp_isr_enter_critical_Ignore();
    bsp_mcu_tick_get_ExpectAndReturn(10U);
    bsp_isr_exit_critical_Ignore();

    appman_timer_create(p_timer, p_status);
}

static uint32_t test_stub_tick_get(int cmock_num_calls)
{
    (void)cmock_num_calls;
    return 50U;
}

static void helper_appman_app_create(appman_app_t *p_app, 
                                     appman_status_t *p_status)
{
    appman_app_create(p_app, p_status);
}

static void helper_appman_timer_create_with_ignore(appman_timer_t *p_timer, 
                                                   appman_status_t *p_status)
{
    bsp_isr_enter_critical_Ignore();
    bsp_mcu_tick_get_IgnoreAndReturn(10U);
    bsp_isr_exit_critical_Ignore();

    appman_timer_create(p_timer, p_status);
}
// end of file
