/**
 * @file test_bsp_mcu.c
 * @author Muhammed Salih AYDOGAN
 * @brief These tests verify the units of the MCU module.
 * 
 * @version 0.1
 * 
 * @date 02/01/2023
 * 
 * @copyright Copyright (c) 2023
 * 
 * https://codebeamer.daiichi.com/issue/181034
 * 
 */

/*******************************************************************************
 * Includes 
 ******************************************************************************/

//First - include test framework headers
#include "unity.h"
#include "support.h"

// Second - include the module to be tested
#include "bsp_mcu.h"

// Third - include the mocks of required modules
#include "mock_clock_S32K1xx.h"
#include "mock_osif.h"

#include "mock_log.h"

/*******************************************************************************
 * Test Files
 ******************************************************************************/

TEST_FILE("bsp_mcu.c");

/*******************************************************************************             
 * Defines
 ******************************************************************************/
/**
 * @brief Index of target clock configuration
*/
#define TEST_MCU_TARGET_CLOCK_CFG_INDEX                  0U

/**
 * @brief Count of clock manager configuration
*/
#define TEST_MCU_CLOCK_MGR_CFG_CNT                       1U     

/**
 * @brief Count of clock manager callback count
*/
#define TEST_MCU_CLOCK_MGR_CB_CNT                        0U     

/**
 * @brief Count of peripheral clock user configuration
*/
#define TEST_NUM_OF_PERIPHERAL_CLOCKS_0                  15U   

/**
 * @brief Count of clock manager callback count
 *
*/
#define TEST_CLOCK_CB_CNT                                0U  

/**
 * @brief Register value of clock manager callback count
 *
*/
#define TEST_S32_SCB_AIRCR_REG                           0x5FA0004

/**
 * @brief Purpose of this macro is to configurate the system clock frequency.
 * 
 */
#define TEST_BSP_MCU_CFG_FREQ_HZ                        40000000U

/*******************************************************************************
 * Extern Variables
 ******************************************************************************/

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/**
 * @brief Array of peripheral clock configurations
 * Purpose of this array is to be argument for m_clockMan1_InitConfig0
 * 
 */
static peripheral_clock_config_t m_test_peripheral_clock_cfg[TEST_NUM_OF_PERIPHERAL_CLOCKS_0] = {
    {
        .clockName = PORTA_CLK,
        .clkGate = true,
        .clkSrc = CLK_SRC_OFF,
        .frac = MULTIPLY_BY_ONE,
        .divider = DIVIDE_BY_ONE,
    },
    {
        .clockName = PORTB_CLK,
        .clkGate = true,
        .clkSrc = CLK_SRC_OFF,
        .frac = MULTIPLY_BY_ONE,
        .divider = DIVIDE_BY_ONE,
    },
    {
        .clockName = PORTC_CLK,
        .clkGate = true,
        .clkSrc = CLK_SRC_OFF,
        .frac = MULTIPLY_BY_ONE,
        .divider = DIVIDE_BY_ONE,
    },
    {
        .clockName = PORTD_CLK,
        .clkGate = true,
        .clkSrc = CLK_SRC_OFF,
        .frac = MULTIPLY_BY_ONE,
        .divider = DIVIDE_BY_ONE,
    },
    {
        .clockName = PORTE_CLK,
        .clkGate = true,
        .clkSrc = CLK_SRC_OFF,
        .frac = MULTIPLY_BY_ONE,
        .divider = DIVIDE_BY_ONE,
    },
    {
        .clockName = FTFC0_CLK,
        .clkGate = true,
        .clkSrc = CLK_SRC_OFF,
        .frac = MULTIPLY_BY_ONE,
        .divider = DIVIDE_BY_ONE,
    },    
    {
        .clockName = LPSPI0_CLK,
        .clkGate = true,
        .clkSrc = CLK_SRC_SIRC_DIV2,
        .frac = MULTIPLY_BY_ONE,
        .divider = DIVIDE_BY_ONE,
    },
    {
        .clockName = LPUART0_CLK,
        .clkGate = true,
        .clkSrc = CLK_SRC_SOSC_DIV2,
        .frac = MULTIPLY_BY_ONE,
        .divider = DIVIDE_BY_ONE,
    },
    {
        .clockName = LPI2C0_CLK,
        .clkGate = true,
        .clkSrc = CLK_SRC_SIRC_DIV2,
        .frac = MULTIPLY_BY_ONE,
        .divider = DIVIDE_BY_ONE,
    },
    {
        .clockName = LPIT0_CLK,
        .clkGate = true,
        .clkSrc = CLK_SRC_SIRC_DIV2,
        .frac = MULTIPLY_BY_ONE,
        .divider = DIVIDE_BY_ONE,
    },
    {
        .clockName = FLEXIO0_CLK,
        .clkGate = true,
        .clkSrc = CLK_SRC_SIRC_DIV2,
        .frac = MULTIPLY_BY_ONE,
        .divider = DIVIDE_BY_ONE,
    },
    {
        .clockName = DMAMUX0_CLK,
        .clkGate = true,
        .clkSrc = CLK_SRC_OFF,
        .frac = MULTIPLY_BY_ONE,
        .divider = DIVIDE_BY_ONE,
    },
    {
        .clockName = FlexCAN0_CLK,
        .clkGate = true,
        .clkSrc = CLK_SRC_OFF,
        .frac = MULTIPLY_BY_ONE,
        .divider = DIVIDE_BY_ONE,
    },
    {
        .clockName = ADC0_CLK,
        .clkGate = true,
        .clkSrc = CLK_SRC_SIRC_DIV2,
        .frac = MULTIPLY_BY_ONE,
        .divider = DIVIDE_BY_ONE,
    },
    {
        .clockName = PDB0_CLK,
        .clkGate = true,
        .clkSrc = CLK_SRC_SOSC,
        .frac = MULTIPLY_BY_ONE,
        .divider = DIVIDE_BY_ONE,
    }, 
};

/**
 * @brief Purpose of this array is to configure clock peripheral of mcu.
 * This array will be argument to CLOCK_SYS_Init function.
 * 
 */
static clock_manager_user_config_t m_test_clock_mgr_init_cfg = {
    .scgConfig = 
    {
        .sircConfig = 
        {
            .initialize = true,
            .enableInStop = true,                 
            .enableInLowPower = true,                                                                                                                       
            .locked = false,                                                    
            .range = SCG_SIRC_RANGE_HIGH,                           
            .div1 = SCG_ASYNC_CLOCK_DIV_BY_1,     
            .div2 = SCG_ASYNC_CLOCK_DIV_BY_1,     
        },
        .fircConfig = 
        {
            .initialize = true,
            .regulator = true,                  
            .locked = false,                    
            .range = SCG_FIRC_RANGE_48M,        
            .div1 = SCG_ASYNC_CLOCK_DIV_BY_1,   
            .div2 = SCG_ASYNC_CLOCK_DIV_BY_1,   
        },
        .rtcConfig = 
        {
            .initialize = false,
        },
        .soscConfig = 
        {
            .initialize = true,
            .freq = TEST_BSP_MCU_CFG_FREQ_HZ,                                                
            .monitorMode = SCG_SOSC_MONITOR_DISABLE,    
            .locked = false,                                                            
            .extRef = SCG_SOSC_REF_OSC,                                                 
            .gain = SCG_SOSC_GAIN_LOW,                                                  
            .range = SCG_SOSC_RANGE_HIGH,               
            .div1 = SCG_ASYNC_CLOCK_DIV_BY_1,                                                   
            .div2 = SCG_ASYNC_CLOCK_DIV_BY_1,                                                       
        },
        .clockOutConfig = 
        {
            .initialize = true,
            .source = SCG_CLOCKOUT_SRC_FIRC, 
        },
        .clockModeConfig = 
        {
            .initialize = true,
            .rccrConfig = 
            {
                .src = SCG_SYSTEM_CLOCK_SRC_SYS_OSC, 
                .divCore = SCG_SYSTEM_CLOCK_DIV_BY_1,
                .divBus = SCG_SYSTEM_CLOCK_DIV_BY_1,
                .divSlow = SCG_SYSTEM_CLOCK_DIV_BY_2,
            },
            .vccrConfig = 
            {
                .src = SCG_SYSTEM_CLOCK_SRC_SIRC, 
                .divCore = SCG_SYSTEM_CLOCK_DIV_BY_2,
                .divBus = SCG_SYSTEM_CLOCK_DIV_BY_1,
                .divSlow = SCG_SYSTEM_CLOCK_DIV_BY_4,
            },
        },
    },
    .pccConfig = 
    {
        .peripheralClocks = m_test_peripheral_clock_cfg, 
        .count = TEST_NUM_OF_PERIPHERAL_CLOCKS_0,
    },
    .simConfig = 
    {
        .clockOutConfig = 
        {
            .initialize = true, 
            .enable = true,                   
            .source = SIM_CLKOUT_SEL_SYSTEM_SCG_CLKOUT,
            .divider = SIM_CLKOUT_DIV_BY_1,      
        },
        .lpoClockConfig = 
        {
            .initialize = true, 
            .enableLpo1k = true, 
            .enableLpo32k = true, 
            .sourceLpoClk = SIM_LPO_CLK_SEL_LPO_128K,
            .sourceRtcClk = SIM_RTCCLK_SEL_FIRCDIV1_CLK,
        },
        .platGateConfig = 
        {
            .initialize = true, 
            .enableEim = true,  
            .enableErm = true,  
            .enableDma = true,  
            .enableMpu = true,  
            .enableMscm = true, 
        },
        .tclkConfig = 
        {
            .initialize = false, 
        },
        .traceClockConfig = 
        {
            .initialize = true, 
            .divEnable = true, 
            .source = CLOCK_TRACE_SRC_CORE_CLK, 
            .divider = 0U, 
            .divFraction = false, 
        },
    },
    .pmcConfig = 
    {
        .lpoClockConfig = 
        {
        .initialize = true, 
        .enable = true, 
        .trimValue = 0,
        },
    },
};

/**
 * @brief This array contain address of user init configration.
 * 
 */
static clock_manager_user_config_t const * m_test_clock_mgr_cfg_arr[] =
{
    &m_test_clock_mgr_init_cfg  
};

/**
 * @brief This callback array contain not NULL a element.
 * 
 */
static clock_manager_callback_user_config_t* m_test_clock_mgr_callbacks_arr[] = {(void*)0};

/**
 * @brief This varibale used used for comparison in bsp_mcu_delay() function 
 * test.
 * 
 */
static uint32_t m_test_delay = 10U;

/*******************************************************************************
 * Global Variables
 ******************************************************************************/

/**
 * @brief This area is used during the mcu reset process.
 * 
 */
uint8_t allocated_area[PERIPH_MEM_SIZE];

/*******************************************************************************
 * Private Functions Prototype Declarations
 ******************************************************************************/

/**
 * @brief helper of mcu not success init.
 * 
 */
static status_t clock_sys_init_cb(
    clock_manager_user_config_t const** clockConfigsPtr, 
    uint8_t configsNumber, 
    clock_manager_callback_user_config_t** callbacksPtr, 
    uint8_t callbacksNumber, int cmock_num_calls);

/**
 * @brief callback function for osif delay.
 * 
 */
static void osif_delay_test_cb(const uint32_t delay, int cmock_num_calls);

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

/*******************************************************************************
 * Test Functions
 ******************************************************************************/

/**
 * @brief The test verifies the bsp_mcu_init() function by calling the 
 * CLOCK_SYS_Init() and CLOCK_SYS_UpdateConfiguration() functions with a 
 * successful call.
 * 
 * https://codebeamer.daiichi.com/issue/181162
 *  
 */
void test_bsp_mcu_init__usual_run__success(void)
{
    TEST_CASE();
    bsp_mcu_status_t ret = BSP_MCU_STATUS_ERROR;

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    CLOCK_SYS_Init_AddCallback(clock_sys_init_cb);

    CLOCK_SYS_Init_ExpectAndReturn(m_test_clock_mgr_cfg_arr, 
                                   TEST_MCU_CLOCK_MGR_CFG_CNT, 
                                   m_test_clock_mgr_callbacks_arr, 
                                   TEST_MCU_CLOCK_MGR_CB_CNT, 
                                   STATUS_SUCCESS);
    CLOCK_SYS_Init_IgnoreArg_clockConfigsPtr();
    CLOCK_SYS_Init_IgnoreArg_callbacksPtr();

    CLOCK_SYS_UpdateConfiguration_ExpectAndReturn(
        TEST_MCU_TARGET_CLOCK_CFG_INDEX, 
        CLOCK_MANAGER_POLICY_AGREEMENT,
        STATUS_SUCCESS);
                                            
    osif_systick_init_Expect();

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);

    ret = bsp_mcu_init();

    TEST_STEP(2);
    
    TEST_ASSERT_EQUAL(BSP_MCU_STATUS_OK, ret);
}

/**
 * @brief The test verifies the bsp_mcu_init() function by calling the 
 * CLOCK_SYS_Init() function with a fail call.
 * 
 * https://codebeamer.daiichi.com/issue/181913
 *  
 */
void test_bsp_mcu_init__init_return_status__invalid_state(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    CLOCK_SYS_Init_ExpectAnyArgsAndReturn(STATUS_MCU_INVALID_STATE);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);

    bsp_mcu_status_t ret = bsp_mcu_init();

    TEST_STEP(2);
    
    TEST_ASSERT_EQUAL(BSP_MCU_STATUS_INVALID, ret);
}

/**
 * @brief The test verifies the bsp_mcu_init() function by calling the 
 * CLOCK_SYS_Init() function with a fail call.
 * 
 * https://codebeamer.daiichi.com/issue/181092.
 *  
 */
void test_bsp_mcu_init__init_return_status__transition_fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    CLOCK_SYS_Init_ExpectAnyArgsAndReturn(STATUS_MCU_TRANSITION_FAILED);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);

    bsp_mcu_status_t ret = bsp_mcu_init();

    TEST_STEP(2);
    
    TEST_ASSERT_EQUAL(BSP_MCU_STATUS_TRANSITION_FAILED, ret);
}

/**
 * @brief The test verifies the bsp_mcu_init() function by calling the 
 * CLOCK_SYS_Init() function with a fail call.
 * 
 * https://codebeamer.daiichi.com/issue/181914
 *  
 */
void test_bsp_mcu_init__init_return_status__error(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    CLOCK_SYS_Init_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);

    bsp_mcu_status_t ret = bsp_mcu_init();

    TEST_STEP(2);
    
    TEST_ASSERT_EQUAL(BSP_MCU_STATUS_ERROR, ret);
}

/**
 * @brief The test verifies the bsp_mcu_init() function by calling the 
 * CLOCK_SYS_Init() function with a successful call and 
 * CLOCK_SYS_UpdateConfiguration() function with a fail call.
 * 
 * https://codebeamer.daiichi.com/issue/181923
 *  
 */
void test_bsp_mcu_init__update_return_status__invalid_state(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    CLOCK_SYS_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    CLOCK_SYS_UpdateConfiguration_ExpectAnyArgsAndReturn(
        STATUS_MCU_INVALID_STATE);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);

    bsp_mcu_status_t ret = bsp_mcu_init();

    TEST_STEP(2);
    
    TEST_ASSERT_EQUAL(BSP_MCU_STATUS_INVALID, ret);
}

/**
 * @brief The test verifies the bsp_mcu_init() function by calling the 
 * CLOCK_SYS_Init() function with a successful call and 
 * CLOCK_SYS_UpdateConfiguration() function with a fail call.
 * 
 * https://codebeamer.daiichi.com/issue/181915
 *  
 */
void test_bsp_mcu_init__update_return_status__transition_fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    CLOCK_SYS_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    CLOCK_SYS_UpdateConfiguration_ExpectAnyArgsAndReturn(
        STATUS_MCU_TRANSITION_FAILED);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);

    bsp_mcu_status_t ret = bsp_mcu_init();

    TEST_STEP(2);
    
    TEST_ASSERT_EQUAL(BSP_MCU_STATUS_TRANSITION_FAILED, ret);
}

/**
 * @brief The test verifies the bsp_mcu_init() function by calling the 
 * CLOCK_SYS_Init() function with a successful call and 
 * CLOCK_SYS_UpdateConfiguration() function with a fail call.
 * 
 * https://codebeamer.daiichi.com/issue/181924
 *  
 */
void test_bsp_mcu_init__update_return_status__error(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    CLOCK_SYS_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    CLOCK_SYS_UpdateConfiguration_ExpectAnyArgsAndReturn(STATUS_ERROR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);

    bsp_mcu_status_t ret = bsp_mcu_init();

    TEST_STEP(2);
    
    TEST_ASSERT_EQUAL(BSP_MCU_STATUS_ERROR, ret);
}

/**
 * @brief The test verifies the bsp_mcu_delay() function with a call.
 * 
 * https://codebeamer.daiichi.com/issue/181083
 *  
 */
void test_bsp_mcu_delay__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    OSIF_TimeDelay_AddCallback(osif_delay_test_cb);
    OSIF_TimeDelay_ExpectAnyArgs();

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);

    bsp_mcu_delay(m_test_delay);
}

/**
 * @brief The test verifies the bsp_mcu_reset() function with a call.
 * 
 * https://codebeamer.daiichi.com/issue/181091
 *  
 */
void test_bsp_mcu_reset__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);

    bsp_mcu_reset();

    TEST_STEP(2);

    TEST_ASSERT_EQUAL(TEST_S32_SCB_AIRCR_REG, S32_SCB->AIRCR);
}

/**
 * @brief The test verifies the bsp_mcu_tick_get() function by calling the 
 * OSIF_GetMilliseconds() function with successful call.
 * 
 * https://codebeamer.daiichi.com/issue/181087
 *  
 */
void test_bsp_mcu_tick_get__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    
    uint32_t systick = 100U;
    uint32_t ret = 0U;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //
    
    OSIF_GetMilliseconds_ExpectAndReturn(systick);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);

    ret = bsp_mcu_tick_get();

    TEST_STEP(2);
    
    TEST_ASSERT_EQUAL(systick, ret);
}

/*******************************************************************************
 * Private Functions Definitions
 ******************************************************************************/

static status_t clock_sys_init_cb(
    clock_manager_user_config_t const** clockConfigsPtr, 
    uint8_t configsNumber, 
    clock_manager_callback_user_config_t** callbacksPtr, 
    uint8_t callbacksNumber, int cmock_num_calls)
{
    (void)cmock_num_calls;
    (void)callbacksPtr;

    static clock_manager_state_t g_clockState;
    g_clockState.configTable = clockConfigsPtr;

    clock_manager_user_config_t const* cfg = g_clockState.configTable[0];
    clock_manager_user_config_t const* config = cfg;

    TEST_ASSERT_EQUAL(TEST_MCU_CLOCK_MGR_CFG_CNT, configsNumber);
    TEST_ASSERT_EQUAL(TEST_CLOCK_CB_CNT, callbacksNumber);

    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.sircConfig.initialize, config->scgConfig.sircConfig.initialize);
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.sircConfig.enableInStop, config->scgConfig.sircConfig.enableInStop);
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.sircConfig.enableInLowPower, config->scgConfig.sircConfig.enableInLowPower);       
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.sircConfig.locked, config->scgConfig.sircConfig.locked);                  
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.sircConfig.range, config->scgConfig.sircConfig.range);              
    /* SIRCDIV */
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.sircConfig.div1,config->scgConfig.sircConfig.div1);               
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.sircConfig.div2, config->scgConfig.sircConfig.div2);               
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.fircConfig.initialize, config->scgConfig.fircConfig.initialize);            
     /* FIRCCSR */
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.fircConfig.regulator, config->scgConfig.fircConfig.regulator);           
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.fircConfig.locked, config->scgConfig.fircConfig.locked);          
    /* FIRCCFG */
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.fircConfig.range, config->scgConfig.fircConfig.range);                    
    /* FIRCDIV */
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.fircConfig.div1, config->scgConfig.fircConfig.div1);                     
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.fircConfig.div2, config->scgConfig.fircConfig.div2);                     
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.rtcConfig.initialize, config->scgConfig.rtcConfig.initialize);                
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.rtcConfig.rtcClkInFreq, config->scgConfig.rtcConfig.rtcClkInFreq);             
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.soscConfig.initialize, config->scgConfig.soscConfig.initialize);               
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.soscConfig.freq, config->scgConfig.soscConfig.freq);                
    /* SOSCCSR */
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.soscConfig.monitorMode, config->scgConfig.soscConfig.monitorMode);            
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.soscConfig.locked, config->scgConfig.soscConfig.locked);              
    /* SOSCCFG */
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.soscConfig.extRef, config->scgConfig.soscConfig.extRef);                 
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.soscConfig.gain, config->scgConfig.soscConfig.gain);                  
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.soscConfig.range, config->scgConfig.soscConfig.range);                  
    /* SOSCDIV */
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.soscConfig.div1, config->scgConfig.soscConfig.div1);                   
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.soscConfig.div2, config->scgConfig.soscConfig.div2);
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.clockOutConfig.initialize, config->scgConfig.clockOutConfig.initialize);
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.clockOutConfig.source, config->scgConfig.clockOutConfig.source);                         
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.clockModeConfig.initialize, config->scgConfig.clockModeConfig.initialize);                             
    /*!< RCCR - Run Clock Control Register          */
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.clockModeConfig.rccrConfig.src, config->scgConfig.clockModeConfig.rccrConfig.src); 
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.clockModeConfig.rccrConfig.divCore, config->scgConfig.clockModeConfig.rccrConfig.divCore);       
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.clockModeConfig.rccrConfig.divBus, config->scgConfig.clockModeConfig.rccrConfig.divBus);           
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.clockModeConfig.rccrConfig.divSlow, config->scgConfig.clockModeConfig.rccrConfig.divSlow);        
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.clockModeConfig.vccrConfig.src, config->scgConfig.clockModeConfig.vccrConfig.src);            
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.clockModeConfig.vccrConfig.divCore, config->scgConfig.clockModeConfig.vccrConfig.divCore);       
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.clockModeConfig.vccrConfig.divBus, config->scgConfig.clockModeConfig.vccrConfig.divBus);         
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.scgConfig.clockModeConfig.vccrConfig.divSlow, config->scgConfig.clockModeConfig.vccrConfig.divSlow);         

    for(uint8_t i = 0; i < TEST_NUM_OF_PERIPHERAL_CLOCKS_0; i++)
    {
        TEST_ASSERT_EQUAL(m_test_peripheral_clock_cfg[i].clockName, config->pccConfig.peripheralClocks[i].clockName);
        TEST_ASSERT_EQUAL(m_test_peripheral_clock_cfg[i].clkGate, config->pccConfig.peripheralClocks[i].clkGate);
        TEST_ASSERT_EQUAL(m_test_peripheral_clock_cfg[i].clkSrc, config->pccConfig.peripheralClocks[i].clkSrc);
        TEST_ASSERT_EQUAL(m_test_peripheral_clock_cfg[i].frac, config->pccConfig.peripheralClocks[i].frac);
        TEST_ASSERT_EQUAL(m_test_peripheral_clock_cfg[i].divider, config->pccConfig.peripheralClocks[i].divider);
    }

    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.pccConfig.count, config->pccConfig.count);
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.simConfig.clockOutConfig.initialize, config->simConfig.clockOutConfig.initialize);
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.simConfig.clockOutConfig.enable, config->simConfig.clockOutConfig.enable);
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.simConfig.clockOutConfig.source, config->simConfig.clockOutConfig.source);
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.simConfig.clockOutConfig.divider, config->simConfig.clockOutConfig.divider);
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.simConfig.lpoClockConfig.initialize, config->simConfig.lpoClockConfig.initialize);
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.simConfig.lpoClockConfig.enableLpo1k, config->simConfig.lpoClockConfig.enableLpo1k);
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.simConfig.lpoClockConfig.enableLpo32k, config->simConfig.lpoClockConfig.enableLpo32k);
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.simConfig.lpoClockConfig.sourceLpoClk, config->simConfig.lpoClockConfig.sourceLpoClk);
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.simConfig.lpoClockConfig.sourceRtcClk, config->simConfig.lpoClockConfig.sourceRtcClk);
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.simConfig.platGateConfig.initialize, config->simConfig.platGateConfig.initialize);
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.simConfig.platGateConfig.enableMscm, config->simConfig.platGateConfig.enableMscm );
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.simConfig.platGateConfig.enableMpu, config->simConfig.platGateConfig.enableMpu);
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.simConfig.platGateConfig.enableDma, config->simConfig.platGateConfig.enableDma);
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.simConfig.platGateConfig.enableErm , config->simConfig.platGateConfig.enableErm );
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.simConfig.platGateConfig.enableEim, config->simConfig.platGateConfig.enableEim);
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.simConfig.qspiRefClkGating.enableQspiRefClk, config->simConfig.qspiRefClkGating.enableQspiRefClk);
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.simConfig.tclkConfig.initialize, config->simConfig.tclkConfig.initialize);
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.simConfig.traceClockConfig.initialize, config->simConfig.traceClockConfig.initialize);
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.simConfig.traceClockConfig.divEnable, config->simConfig.traceClockConfig.divEnable);
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.simConfig.traceClockConfig.source, config->simConfig.traceClockConfig.source);
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.simConfig.traceClockConfig.divider, config->simConfig.traceClockConfig.divider);
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.simConfig.traceClockConfig.divFraction, config->simConfig.traceClockConfig.divFraction);
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.pmcConfig.lpoClockConfig.initialize, config->pmcConfig.lpoClockConfig.initialize);
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.pmcConfig.lpoClockConfig.enable, config->pmcConfig.lpoClockConfig.enable);
    TEST_ASSERT_EQUAL(m_test_clock_mgr_init_cfg.pmcConfig.lpoClockConfig.trimValue, config->pmcConfig.lpoClockConfig.trimValue);

    return STATUS_SUCCESS;
}

static void osif_delay_test_cb(const uint32_t delay, int cmock_num_calls)
{
    (void)cmock_num_calls;

    TEST_ASSERT_EQUAL(m_test_delay, delay);
}

// end of file