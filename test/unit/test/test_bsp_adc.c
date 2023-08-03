/**
 * @file test_bsp_adc.c
 * @author Muhammed Salih AYDOGAN
 * @brief These tests verify the units of the ADC module.
 * 
 * @version 0.1
 * 
 * @date 02/06/2023
 * 
 * @copyright Copyright (c) 2023
 * 
 * https://codebeamer.daiichi.com/item/194765
 * 
 */

/*******************************************************************************
 * Includes 
 ******************************************************************************/

//First - include test framework headers
#include "unity.h"
#include "support.h"

// Second - include the module to be tested
#include "bsp_adc.h"

// Third - include the mocks of required modules
#include "mock_adc_driver.h"
#include "mock_edma_driver.h"
#include "mock_pdb_driver.h"
#include "mock_pins_driver.h"
#include "mock_clock_S32K1xx.h"

#include "mock_log.h"

/*******************************************************************************
 * Test Files
 ******************************************************************************/

TEST_FILE("bsp_adc.c");

/*******************************************************************************             
 * Defines
 ******************************************************************************/
/**
 * @brief This macro indicates which adc unit to be used.
 * 
 */
#define TEST_ADC_INST (0U)

/**
 * @brief This macro indicates which pdb unit to be used.
 * 
 */
#define TEST_PDB_INST (0U)

/**
 * @brief This macro indicates that adc sample time.
 * 
 */
#define TEST_ADC_SAMPLE_TIME (100U)

/**
 * @brief This macro indicates which port to be used.
 * 
 *        - PORTA = 0
 *        - PORTB = 1
 *        - PORTC = 2
 *        - PORTD = 3
 *        - PORTE = 4
 * 
 */
#define TEST_ADC_PORT (0U)

/**
 * @brief This macro indicates which pin to be used.
 * 
 */
#define TEST_ADC_PIN (7U)

/**
 * @brief This macro indicates that adc buffer size.
 * 
 */
#define TEST_ADC_BUFF_MAX_CNT (50U)

/**
 * @brief This macro indicates which edma channel to be used.
 * 
 */
#define TEST_EDMA_CH_3 (3U)

/**
 * @brief This macro indicates that ADC pin count.
 * 
 */
#define TEST_ADC_PIN_COUNT (1U)

/**
 * @brief This macro indicates which pdb channel to be used.
 * 
 */
#define TEST_PDB_CH_0 (0U)

/**
 * @brief This macro indicates that pdb period(us).
 * 
 * 
 */
#define TEST_PDB_PERIOD (10000U)

/**
 * @brief This macro holds ADC initial value.
 * 
 * 
 */
#define TEST_ADC_INIT_VAL (0xFFFFU)

/**
 * @brief This macro indicates the maximum value of the uint16_t type.
 *        It is used instead of the UINT16_MAX macro defined in the 
 *        library due to a QAC error.
 * 
 */
#define TEST_UINT16_T_MAX (0xFFFFU) 

/*******************************************************************************
 * Extern Variables
 ******************************************************************************/

/*******************************************************************************
 * Private Variables
 ******************************************************************************/
edma_callback_t m_test_adc_edma_cb;

/*******************************************************************************
 * Global Variables
 ******************************************************************************/
/**
 * @brief This variable is used to change the Peripheral PORTC base address.
 * 
 */
uint8_t allocated_area[PERIPH_MEM_SIZE];

/*******************************************************************************
 * Private Functions Prototype Declarations
 ******************************************************************************/
/**
 * @brief  Helper function for adc initialization.
*/
static void helper_adc_init_success(void);

/**
 * @brief  Helper function for adc initialization with fail.
*/
static void helper_adc_init_with_fail(void);

/**
 * @brief  Helper function for adc deinitialization.
*/
static void helper_adc_deinit(void);

/**
 * @brief  Stub function for EDMA_DRV_ChannelInit driver function.
*/
static status_t test_edma_drv_channel_init_stub(
    edma_chn_state_t *edmaChannelState, 
    const edma_channel_config_t *edmaChannelConfig,
    int cmock_num_calls);

/**
 * @brief  Stub function for EDMA_DRV_ConfigLoopTransfer driver function.
*/
static status_t test_edma_drv_config_loop_transfer_stub(
    uint8_t virtualChannel, 
    const edma_transfer_config_t* transferConfig, 
    int cmock_num_calls);

/**
 * @brief  Stub function for ADC_DRV_ConfigChan driver function.
*/
static void test_adc_drv_config_chan_stub (const uint32_t instance, 
    const uint8_t chanIndex, 
    const adc_chan_config_t* const config, 
    int cmock_num_calls);

/**
 * @brief  Stub function for PDB_DRV_ConfigAdcPreTrigger driver function.
*/
static void test_pdb_drv_config_adc_pretrigger_stub(const uint32_t instance, 
    const uint32_t chn, 
    const pdb_adc_pretrigger_config_t* configPtr, 
    int cmock_num_calls); 

/**
 * @brief  Stub function for PDB_DRV_Init driver function.
*/
static void test_pdb_drv_init_stub(const uint32_t instance, 
                                   const pdb_timer_config_t* userConfigPtr, 
                                   int cmock_num_calls);
/**
 * @brief  Stub function for pin initialization.
*/
static status_t test_pins_drv_init_stub(uint32_t pinCount, 
                                        const pin_settings_config_t* config, 
                                        int cmock_num_calls);

/**
 * @brief  Stub function for pin configuration.
*/
static void test_adc_drv_config_stub(const uint32_t instance, 
                                     const adc_converter_config_t* const config, 
                                     int cmock_num_calls);

/**
 * @brief  Stub function for changed frequency paramater.
*/
static status_t test_clock_sys_get_freq_stub(clock_names_t clockName, 
                                             uint32_t* frequency, 
                                             int cmock_num_calls);

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
 * @brief Verifies bsp_adc_init function when initialization is successful.
 * 
 * https://codebeamer.daiichi.com/item/194769
 *  
 */
void test_bsp_adc_init__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    PINS_DRV_Init_StubWithCallback(test_pins_drv_init_stub);

    ADC_DRV_ConfigConverter_StubWithCallback(test_adc_drv_config_stub);

    ADC_DRV_AutoCalibration_Expect(TEST_ADC_INST);

    ADC_DRV_ConfigChan_StubWithCallback(test_adc_drv_config_chan_stub);

    PDB_DRV_Init_StubWithCallback(test_pdb_drv_init_stub);

    PDB_DRV_Enable_Expect(TEST_ADC_INST);

    PDB_DRV_ConfigAdcPreTrigger_StubWithCallback(
        test_pdb_drv_config_adc_pretrigger_stub);

    uint32_t clck_freq = 4294967295U;
    CLOCK_SYS_GetFreq_ExpectAndReturn(CORE_CLK, 
                                      &clck_freq, 
                                      STATUS_SUCCESS);
    CLOCK_SYS_GetFreq_IgnoreArg_frequency();

    CLOCK_SYS_GetFreq_ReturnThruPtr_frequency(&clck_freq);

    uint16_t test_delay_val = 0U;
    PDB_DRV_SetTimerModulusValue_Expect(TEST_PDB_INST, test_delay_val);

    PDB_DRV_SetAdcPreTriggerDelayValue_Expect(TEST_PDB_INST, 
                                              TEST_PDB_CH_0,
                                              TEST_PDB_CH_0,
                                              test_delay_val);

    PDB_DRV_LoadValuesCmd_Expect(TEST_PDB_INST);

    PDB_DRV_SoftTriggerCmd_Expect(TEST_PDB_INST);

    EDMA_DRV_ChannelInit_StubWithCallback(test_edma_drv_channel_init_stub);
    
    EDMA_DRV_ConfigLoopTransfer_StubWithCallback(
        test_edma_drv_config_loop_transfer_stub);

    EDMA_DRV_SetChannelRequestAndTrigger_ExpectAndReturn(
        TEST_EDMA_CH_3,
        (uint8_t)EDMA_REQ_ADC0,
        false,
        STATUS_SUCCESS);

    EDMA_DRV_StartChannel_ExpectAndReturn(TEST_EDMA_CH_3, STATUS_SUCCESS);
    
    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_adc_init();
}

/**
 * @brief Verifies bsp_adc_init function when GPIO initialization state is fail.
 * 
 * https://codebeamer.daiichi.com/item/194826
 *  
 */
void test_bsp_adc_init__gpio_init_state___fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_adc_init();
}

/**
 * @brief Verifies bsp_adc_init function when EDMA initialization state is fail.
 * 
 * https://codebeamer.daiichi.com/item/194828
 *  
 */
void test_bsp_adc_init__edma_init_state___fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    ADC_DRV_ConfigConverter_ExpectAnyArgs();

    ADC_DRV_AutoCalibration_ExpectAnyArgs();

    ADC_DRV_ConfigChan_ExpectAnyArgs();

    PDB_DRV_Init_ExpectAnyArgs();

    PDB_DRV_Enable_ExpectAnyArgs();

    PDB_DRV_ConfigAdcPreTrigger_ExpectAnyArgs();

    CLOCK_SYS_GetFreq_ExpectAnyArgsAndReturn(STATUS_ERROR);

    PDB_DRV_SetTimerModulusValue_ExpectAnyArgs();

    PDB_DRV_SetAdcPreTriggerDelayValue_ExpectAnyArgs();

    PDB_DRV_LoadValuesCmd_ExpectAnyArgs();

    PDB_DRV_SoftTriggerCmd_ExpectAnyArgs();

    EDMA_DRV_ChannelInit_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_adc_init();
}

/**
 * @brief Verifies bsp_adc_init function when EDMA configuration loop state is 
 * fail.
 * 
 * https://codebeamer.daiichi.com/item/194829
 *  
 */
void test_bsp_adc_init__edma_config_loop_state___fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    
    ADC_DRV_ConfigConverter_ExpectAnyArgs();
    
    ADC_DRV_AutoCalibration_ExpectAnyArgs();
    
    ADC_DRV_ConfigChan_ExpectAnyArgs();
    
    PDB_DRV_Init_ExpectAnyArgs();
    
    PDB_DRV_Enable_ExpectAnyArgs();
    
    PDB_DRV_ConfigAdcPreTrigger_ExpectAnyArgs();
    
    CLOCK_SYS_GetFreq_ExpectAnyArgsAndReturn(STATUS_ERROR);
    
    PDB_DRV_SetTimerModulusValue_ExpectAnyArgs();
    
    PDB_DRV_SetAdcPreTriggerDelayValue_ExpectAnyArgs();
    
    PDB_DRV_LoadValuesCmd_ExpectAnyArgs();
    
    PDB_DRV_SoftTriggerCmd_ExpectAnyArgs();
    
    EDMA_DRV_ChannelInit_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    
    EDMA_DRV_ConfigLoopTransfer_ExpectAnyArgsAndReturn(STATUS_ERROR);
    

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_adc_init();
}

/**
 * @brief Verifies bsp_adc_init function when EDMA channel set process is 
 * failed.
 * 
 * https://codebeamer.daiichi.com/item/194830
 *  
 */
void test_bsp_adc_init__edma_set_channel_state___fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    
    ADC_DRV_ConfigConverter_ExpectAnyArgs();
    
    ADC_DRV_AutoCalibration_ExpectAnyArgs();
    
    ADC_DRV_ConfigChan_ExpectAnyArgs();
    
    PDB_DRV_Init_ExpectAnyArgs();
    
    PDB_DRV_Enable_ExpectAnyArgs();
    
    PDB_DRV_ConfigAdcPreTrigger_ExpectAnyArgs();
    
    CLOCK_SYS_GetFreq_ExpectAnyArgsAndReturn(STATUS_ERROR);
    
    PDB_DRV_SetTimerModulusValue_ExpectAnyArgs();
    
    PDB_DRV_SetAdcPreTriggerDelayValue_ExpectAnyArgs();
    
    PDB_DRV_LoadValuesCmd_ExpectAnyArgs();
    
    PDB_DRV_SoftTriggerCmd_ExpectAnyArgs();
    
    EDMA_DRV_ChannelInit_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    
    EDMA_DRV_ConfigLoopTransfer_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    
    EDMA_DRV_SetChannelRequestAndTrigger_ExpectAnyArgsAndReturn(STATUS_ERROR);
    

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_adc_init();
}

/**
 * @brief Verifies bsp_adc_init function when EDMA start channel process is 
 * failed.
 * 
 * https://codebeamer.daiichi.com/item/194831
 *  
 */
void test_bsp_adc_init__edma_start_channel_state___fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    
    ADC_DRV_ConfigConverter_ExpectAnyArgs();
    
    ADC_DRV_AutoCalibration_ExpectAnyArgs();
    
    ADC_DRV_ConfigChan_ExpectAnyArgs();
    
    PDB_DRV_Init_ExpectAnyArgs();
    
    PDB_DRV_Enable_ExpectAnyArgs();
    
    PDB_DRV_ConfigAdcPreTrigger_ExpectAnyArgs();
    
    CLOCK_SYS_GetFreq_ExpectAnyArgsAndReturn(STATUS_ERROR);
    
    PDB_DRV_SetTimerModulusValue_ExpectAnyArgs();
    
    PDB_DRV_SetAdcPreTriggerDelayValue_ExpectAnyArgs();
    
    PDB_DRV_LoadValuesCmd_ExpectAnyArgs();
    
    PDB_DRV_SoftTriggerCmd_ExpectAnyArgs();
    
    EDMA_DRV_ChannelInit_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    
    EDMA_DRV_ConfigLoopTransfer_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    
    EDMA_DRV_SetChannelRequestAndTrigger_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    
    EDMA_DRV_StartChannel_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_adc_init();
}

/**
 * @brief Verifies bsp_adc_deinit function when deinitialization is successful. 
 * 
 * https://codebeamer.daiichi.com/item/194841
 *  
 */
void test_bsp_adc_deinit__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_adc_init_success();

    // ************************** Expected Calls **************************** //

    ADC_DRV_Reset_Expect(TEST_ADC_INST);
    
    PDB_DRV_Deinit_Expect(TEST_PDB_INST);
    
    EDMA_DRV_ReleaseChannel_ExpectAndReturn(TEST_EDMA_CH_3, STATUS_SUCCESS);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_adc_deinit();

    TEST_STEP(2);
    bsp_adc_deinit();
}

/**
 * @brief Verifies bsp_adc_deinit function without initialization.
 * 
 * https://codebeamer.daiichi.com/item/194842
 *  
 */
void test_bsp_adc_deinit__without_init__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_adc_init_with_fail();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_adc_deinit();
}

/**
 * @brief Verifies bsp_adc_read function when adc private param reset. 
 * 
 * https://codebeamer.daiichi.com/item/194852
 *  
 */
void test_bsp_adc_read__adc_private_param_reset__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint16_t p_raw = 1U;
 
    // **************************** Pre-Actions ***************************** //

    // m_adc_raw_val variable is equal to ADC_INIT_VAL
    helper_adc_init_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_adc_read(&p_raw);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies bsp_adc_read function when EDMA channel normal.
 * 
 * https://codebeamer.daiichi.com/issue/194854
 *  
 */
void test_bsp_adc_read__when_edma_channel_normal__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint16_t p_raw = 1U;
    edma_chn_status_t test_status = EDMA_CHN_NORMAL;
 
    // **************************** Pre-Actions ***************************** //

    // m_adc_raw_val variable is equal to ADC_INIT_VAL
    helper_adc_init_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    m_test_adc_edma_cb(NULL, test_status);

    TEST_STEP(2);
    bool ret = bsp_adc_read(&p_raw);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(true, ret);

    TEST_STEP(4);
    TEST_ASSERT_EQUAL(0U, p_raw);
}

/**
 * @brief Verifies bsp_adc_read function when EDMA channel error.
 * 
 * https://codebeamer.daiichi.com/item/194855
 *  
 */
void test_bsp_adc_read__when_edma_channel_err__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint16_t p_raw = 1U;
    edma_chn_status_t test_status = EDMA_CHN_ERROR;
 
    // **************************** Pre-Actions ***************************** //

    helper_adc_init_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    m_test_adc_edma_cb(NULL, test_status);

    TEST_STEP(2);
    bool ret = bsp_adc_read(&p_raw);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies bsp_adc_read function with invalid raw argument.
 * 
 * https://codebeamer.daiichi.com/issue/194865
 *  
 */
void test_bsp_adc_read__invalid_raw_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    
    // **************************** Pre-Actions ***************************** //

    helper_adc_init_success();

    helper_adc_deinit();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_adc_read(NULL);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies bsp_adc_read function without ADC initialization.
 * 
 * https://codebeamer.daiichi.com/item/194891
 *  
 */
void test_bsp_adc_read__without_adc_init__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint16_t p_raw = 1U;
    
    // **************************** Pre-Actions ***************************** //

    helper_adc_init_success();

    helper_adc_deinit();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_adc_read(&p_raw);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies bsp_adc_init function when the period value is calculated.
 * 
 * https://codebeamer.daiichi.com/item/195092
 *  
 */
void test_bsp_adc_init__calculate_period___fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    ADC_DRV_ConfigConverter_ExpectAnyArgs();

    ADC_DRV_AutoCalibration_ExpectAnyArgs();

    ADC_DRV_ConfigChan_ExpectAnyArgs();

    PDB_DRV_Init_ExpectAnyArgs();

    PDB_DRV_Enable_ExpectAnyArgs();

    PDB_DRV_ConfigAdcPreTrigger_ExpectAnyArgs();

    CLOCK_SYS_GetFreq_StubWithCallback(test_clock_sys_get_freq_stub);

    PDB_DRV_SetTimerModulusValue_ExpectAnyArgs();

    PDB_DRV_SetAdcPreTriggerDelayValue_ExpectAnyArgs();

    PDB_DRV_LoadValuesCmd_ExpectAnyArgs();

    PDB_DRV_SoftTriggerCmd_ExpectAnyArgs();

    EDMA_DRV_ChannelInit_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_adc_init();
}

/*******************************************************************************
 * Private Functions Definitions
 ******************************************************************************/
static status_t test_clock_sys_get_freq_stub(clock_names_t clockName, 
                                             uint32_t* frequency, 
                                             int cmock_num_calls)
{
    (void)clockName;
    (void)cmock_num_calls;

    // set valid frequency value
    *frequency = 6553500U;

    return STATUS_SUCCESS;
}

static status_t test_edma_drv_channel_init_stub(
    edma_chn_state_t *edmaChannelState, 
    const edma_channel_config_t *edmaChannelConfig,
    int cmock_num_calls)
{
    (void)cmock_num_calls;
    (void)edmaChannelState;

    TEST_ASSERT_EQUAL(EDMA_CHN_PRIORITY_1, edmaChannelConfig->channelPriority);
    TEST_ASSERT_EQUAL(TEST_EDMA_CH_3, edmaChannelConfig->virtChnConfig);
    TEST_ASSERT_EQUAL(EDMA_REQ_ADC0, edmaChannelConfig->source);
    TEST_ASSERT_EQUAL(false, edmaChannelConfig->enableTrigger);
    TEST_ASSERT_EQUAL(NULL, edmaChannelConfig->callbackParam);

    m_test_adc_edma_cb = edmaChannelConfig->callback;
    
    return STATUS_SUCCESS;
}

static status_t test_edma_drv_config_loop_transfer_stub(
    uint8_t virtualChannel, 
    const edma_transfer_config_t* transferConfig, 
    int cmock_num_calls)
{
    (void)cmock_num_calls;

    TEST_ASSERT_EQUAL(TEST_EDMA_CH_3, virtualChannel);
    TEST_ASSERT_EQUAL(EDMA_TRANSFER_SIZE_2B, transferConfig->srcTransferSize);
    TEST_ASSERT_EQUAL(EDMA_TRANSFER_SIZE_2B, transferConfig->destTransferSize);
    TEST_ASSERT_EQUAL(4, transferConfig->srcOffset);
    TEST_ASSERT_EQUAL(2, transferConfig->destOffset);

    TEST_ASSERT_EQUAL((int32_t)(4 * (int32_t)TEST_ADC_BUFF_MAX_CNT), 
        transferConfig->srcLastAddrAdjust);

    TEST_ASSERT_EQUAL((int32_t)(2 * (int32_t)TEST_ADC_BUFF_MAX_CNT), 
        transferConfig->destLastAddrAdjust);

    TEST_ASSERT_EQUAL(EDMA_MODULO_OFF, transferConfig->srcModulo);
    TEST_ASSERT_EQUAL(EDMA_MODULO_OFF, transferConfig->destModulo);
    TEST_ASSERT_EQUAL(2U, transferConfig->minorByteTransferCount);
    TEST_ASSERT_EQUAL(false, transferConfig->scatterGatherEnable);
    TEST_ASSERT_EQUAL(true, transferConfig->interruptEnable);
    
    TEST_ASSERT_EQUAL(TEST_ADC_BUFF_MAX_CNT,
        transferConfig->loopTransferConfig->majorLoopIterationCount);

    TEST_ASSERT_EQUAL(true, 
        transferConfig->loopTransferConfig->srcOffsetEnable);
    TEST_ASSERT_EQUAL(true, 
        transferConfig->loopTransferConfig->dstOffsetEnable);
    TEST_ASSERT_EQUAL(0, 
        transferConfig->loopTransferConfig->minorLoopOffset);
    TEST_ASSERT_EQUAL(false, 
        transferConfig->loopTransferConfig->minorLoopChnLinkEnable);
    TEST_ASSERT_EQUAL(0U, 
        transferConfig->loopTransferConfig->minorLoopChnLinkNumber);
    TEST_ASSERT_EQUAL(false, 
        transferConfig->loopTransferConfig->majorLoopChnLinkEnable);
    TEST_ASSERT_EQUAL(0U, 
        transferConfig->loopTransferConfig->majorLoopChnLinkNumber);

    return STATUS_SUCCESS;
}

static void test_pdb_drv_init_stub(const uint32_t instance, 
                                   const pdb_timer_config_t* userConfigPtr, 
                                   int cmock_num_calls)
{
    (void)cmock_num_calls;

    TEST_ASSERT_EQUAL(TEST_PDB_INST, instance);
    TEST_ASSERT_EQUAL(PDB_LOAD_VAL_IMMEDIATELY, userConfigPtr->loadValueMode);
    TEST_ASSERT_EQUAL(false, userConfigPtr->seqErrIntEnable);
    TEST_ASSERT_EQUAL(PDB_CLK_PREDIV_BY_1, userConfigPtr->clkPreDiv);
    TEST_ASSERT_EQUAL(PDB_CLK_PREMULT_FACT_AS_10, 
        userConfigPtr->clkPreMultFactor);
    TEST_ASSERT_EQUAL(PDB_SOFTWARE_TRIGGER, userConfigPtr->triggerInput);
    TEST_ASSERT_EQUAL(true, userConfigPtr->continuousModeEnable);
    TEST_ASSERT_EQUAL(false, userConfigPtr->dmaEnable);
    TEST_ASSERT_EQUAL(false, userConfigPtr->intEnable);
}

static void test_pdb_drv_config_adc_pretrigger_stub(const uint32_t instance, 
    const uint32_t chn, 
    const pdb_adc_pretrigger_config_t* configPtr, 
    int cmock_num_calls)
{
    (void)cmock_num_calls;

    TEST_ASSERT_EQUAL(TEST_PDB_INST, instance);
    TEST_ASSERT_EQUAL(TEST_PDB_CH_0, chn);
    TEST_ASSERT_EQUAL(0U, configPtr->adcPreTriggerIdx);
    TEST_ASSERT_EQUAL(false, configPtr->preTriggerEnable);
    TEST_ASSERT_EQUAL(false, configPtr->preTriggerOutputEnable);
    TEST_ASSERT_EQUAL(false, configPtr->preTriggerBackToBackEnable);
}

static status_t test_pins_drv_init_stub(uint32_t pinCount, 
                                        const pin_settings_config_t* config, 
                                        int cmock_num_calls)
{
    (void)cmock_num_calls;

    TEST_ASSERT_EQUAL(TEST_ADC_PIN_COUNT, pinCount);
    TEST_ASSERT_EQUAL(PORTA, config->base);
    TEST_ASSERT_EQUAL(TEST_ADC_PIN, config->pinPortIdx);
    TEST_ASSERT_EQUAL(PORT_INTERNAL_PULL_NOT_ENABLED, config->pullConfig);
    TEST_ASSERT_EQUAL(false, config->passiveFilter);
    TEST_ASSERT_EQUAL(PORT_LOW_DRIVE_STRENGTH, config->driveSelect);
    TEST_ASSERT_EQUAL(PORT_PIN_DISABLED, config->mux);
    TEST_ASSERT_EQUAL(false, config->pinLock);
    TEST_ASSERT_EQUAL(PORT_DMA_INT_DISABLED, config->intConfig);
    TEST_ASSERT_EQUAL(false, config->clearIntFlag);
    TEST_ASSERT_EQUAL(false, config->digitalFilter);
    TEST_ASSERT_EQUAL(PTA, config->gpioBase);
    TEST_ASSERT_EQUAL(GPIO_INPUT_DIRECTION, config->direction);
    TEST_ASSERT_EQUAL(0U, config->initValue);

    return STATUS_SUCCESS;
}

static void test_adc_drv_config_stub(const uint32_t instance, 
    const adc_converter_config_t* const config, 
    int cmock_num_calls)
{
    (void)cmock_num_calls;

    TEST_ASSERT_EQUAL(TEST_ADC_INST, instance);
    TEST_ASSERT_EQUAL(ADC_CLK_DIVIDE_4, config->clockDivide);
    TEST_ASSERT_EQUAL(TEST_ADC_SAMPLE_TIME, config->sampleTime);
    TEST_ASSERT_EQUAL(ADC_RESOLUTION_12BIT, config->resolution);
    TEST_ASSERT_EQUAL(ADC_CLK_ALT_1, config->inputClock);
    TEST_ASSERT_EQUAL(ADC_TRIGGER_HARDWARE, config->trigger);
    TEST_ASSERT_EQUAL(ADC_PRETRIGGER_SEL_PDB, config->pretriggerSel);
    TEST_ASSERT_EQUAL(ADC_TRIGGER_SEL_PDB, config->triggerSel);
    TEST_ASSERT_EQUAL(true, config->dmaEnable);
    TEST_ASSERT_EQUAL(ADC_VOLTAGEREF_VREF, config->voltageRef);
    TEST_ASSERT_EQUAL(false, config->continuousConvEnable);
    TEST_ASSERT_EQUAL(false, config->supplyMonitoringEnable);
}

static void test_adc_drv_config_chan_stub (const uint32_t instance, 
    const uint8_t chanIndex, 
    const adc_chan_config_t* const config, 
    int cmock_num_calls)
{
    (void)cmock_num_calls;

    TEST_ASSERT_EQUAL(TEST_ADC_INST, instance);
    TEST_ASSERT_EQUAL(0U, chanIndex);
    TEST_ASSERT_EQUAL(ADC_INPUTCHAN_EXT3, config->channel);
    TEST_ASSERT_EQUAL(false, config->interruptEnable);
}   

static void helper_adc_init_success()
{
    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    
    ADC_DRV_ConfigConverter_ExpectAnyArgs();
    
    ADC_DRV_AutoCalibration_ExpectAnyArgs();
    
    ADC_DRV_ConfigChan_ExpectAnyArgs();
    
    PDB_DRV_Init_ExpectAnyArgs();
    
    PDB_DRV_Enable_ExpectAnyArgs();
    
    PDB_DRV_ConfigAdcPreTrigger_ExpectAnyArgs();
    
    CLOCK_SYS_GetFreq_ExpectAnyArgsAndReturn(STATUS_ERROR);
    
    PDB_DRV_SetTimerModulusValue_ExpectAnyArgs();
    
    PDB_DRV_SetAdcPreTriggerDelayValue_ExpectAnyArgs();
    
    PDB_DRV_LoadValuesCmd_ExpectAnyArgs();
    
    PDB_DRV_SoftTriggerCmd_ExpectAnyArgs();
    
    EDMA_DRV_ChannelInit_StubWithCallback(test_edma_drv_channel_init_stub);  

    EDMA_DRV_ConfigLoopTransfer_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    
    EDMA_DRV_SetChannelRequestAndTrigger_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    
    EDMA_DRV_StartChannel_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    
    bsp_adc_init();
}

static void helper_adc_init_with_fail()
{
    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_ERROR);

    bsp_adc_init();
}

static void helper_adc_deinit()
{
    ADC_DRV_Reset_ExpectAnyArgs();
    
    PDB_DRV_Deinit_ExpectAnyArgs();
    
    EDMA_DRV_ReleaseChannel_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    bsp_adc_deinit();
}

// end of file
