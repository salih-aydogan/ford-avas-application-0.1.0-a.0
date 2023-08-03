/**
 * @file test_component_a.c
 * @author Name of the test author.
 * @brief Information about to be tested module.
 * 
 * # Tested Interface:
 * 
 * - component_a.h
 * 
 * # Mocked Components:
 * 
 * - component_b.c
 * 
 * @link Link of the test information item on the CodeBeamer.
 * 
 * @version version info.
 * 
 * @date date info.
 * 
 * @copyright Copyright (c) 2021
 * 
 */

/*******************************************************************************
 * Includes 
 ******************************************************************************/

#include "unity.h"
#include "support.h"

/*******************************************************************************
 * Setup & Teardown 
 ******************************************************************************/

void setUp()
{

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

/**
 * @brief Information about the test case.
 * 
 * @link Link of the test specification item on the CodeBeamer.
 *  
 * # Pre-Actions:
 * 
 * - Pre-actions of the test case.
 * 
 * Example:
 * - Initialize the component_a.
 *  
 * # Post-Actions:
 * 
 * - Post-actions of the test case.
 * 
 * Example:
 * - Deinitialize the module_a.
 * 
 * # Expected Calls:
 *
 * - Expected calls to the mock modules.
 *
 * Example:
 * - component_b_function_y() was called with c,d parameters. 
 * 
 * # Actions:
 *
 * - Actions of the test case.
 * 
 * Example:
 * - Call component_a_function_x() with a,b,c parameters.
 * 
 */
void test_module_a_function_y__param_y_valid_part__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // Input parameters of the function to be tested.

    // **************************** Pre-Actions ***************************** //

    // Pre-actions of the test case.

    // **************************** Pre-Actions ***************************** //

    // Pre-actions of the test case.

    // **************************** Test Steps ****************************** //

    // Trigger of the function to be tested.

    TEST_STEP(1);
    //ret = module_a_function_x(a,b,c)

    TEST_STEP(2);
    // TEST_ASSERT_EQUAL(TRUE, ret)
}

/*** end of file ***/
