/**
 * @file test_module_a.c
 * @author Name of the test author and mail address.
 * @brief Information about to be tested module.
 * 
 * @version version info.
 * 
 * @date date info.
 * 
 * @copyright Copyright (c) 2021
 * 
 * Link of the test information item on the CodeBeamer.
 * 
 */

/*******************************************************************************
 * Includes 
 ******************************************************************************/

#include "unity.h"
#include "support.h"

/*******************************************************************************
 * Test Files
 ******************************************************************************/

/*******************************************************************************
 * Extern Variables
 ******************************************************************************/

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/*******************************************************************************
 * Global Variables
 ******************************************************************************/

/*******************************************************************************
 * Private Functions Prototype Declarations
 ******************************************************************************/

/*******************************************************************************
 * Setup & Teardown 
 ******************************************************************************/

void setUp()
{
    log_print_Ignore();
    log_comm_Ignore();
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
 * @brief Information about the test case.
 * 
 * Link of the test specification item on the CodeBeamer.
 *  
 */
void test_module_a_function_y__tested_condition__expected_behavior(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // Input parameters of the function to be tested.

    // **************************** Pre-Actions ***************************** //

    // Pre actions of the test case.

    // ************************** Expected Calls **************************** //

    // Expected calls of the test case.

    // **************************** Test Steps ****************************** //

    // Trigger of the functions to be tested and asserts.

    TEST_STEP(1);
    //uint8_t ret = module_a_function_x(a,b,c)

    TEST_STEP(2);
    // TEST_ASSERT_EQUAL(TRUE, ret)
}

/*******************************************************************************
 * Private Functions Definitions
 ******************************************************************************/

// end of file 
