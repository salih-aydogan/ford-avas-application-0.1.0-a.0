/**
 * @file test_bsp_gpio.c
 * @author Berke Yalcinkaya (berke.yalcinkaya@daiichi.com)
 * @brief These tests verify the units of the bsp_gpio module.
 * 
 * @version 0.1
 * 
 * @date 2023-02-15
 * 
 * @copyright Copyright (c) 2023
 * 
 * @link https://codebeamer.daiichi.com/issue/183890
 * 
 */

/*******************************************************************************
 * Includes 
 ******************************************************************************/

#include "unity.h"
#include "support.h"

#include "bsp_gpio.h"
#include "daiichi_types.h"

#include "mock_pins_driver.h"
#include "mock_interrupt_manager.h"

#include "mock_log.h"

/*******************************************************************************
 * Test Files
 ******************************************************************************/

TEST_FILE("bsp_gpio.c")

/*******************************************************************************
 * Typedef and Definations
 ******************************************************************************/

/**
 * @brief This macro specifies total pin (output / input) count.
 * 
 */
#define TEST_PIN_COUNT            (11U)

/**
 * @brief This macro specifies output count.
 * 
 */
#define TEST_OUTPUT_COUNT         (8U)

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/**
 * @brief This table specifies port values.
 * 
 */
static uint32_t m_test_port_arr[TEST_PIN_COUNT] = {

    (BSP_GPIO_AMP_STB_1_OUT_PIN_NO 
    / (BSP_GPIO_AMP_STB_1_OUT_PIN_NO < 100U ? 10U : 100U)) - 1,

    (BSP_GPIO_AMP_STB_2_OUT_PIN_NO 
    / (BSP_GPIO_AMP_STB_2_OUT_PIN_NO < 100U ? 10U : 100U)) - 1,

    (BSP_GPIO_CAN_STB_OUT_PIN_NO 
    / (BSP_GPIO_CAN_STB_OUT_PIN_NO < 100U ? 10U : 100U)) - 1,

    (BSP_GPIO_CAN_EN_OUT_PIN_NO 
    / (BSP_GPIO_CAN_EN_OUT_PIN_NO < 100U ? 10U : 100U)) - 1,

    (BSP_GPIO_AMP_MUTE_1_OUT_PIN_NO 
    / (BSP_GPIO_AMP_MUTE_1_OUT_PIN_NO < 100U ? 10U : 100U)) - 1,
    
    (BSP_GPIO_AMP_MUTE_2_OUT_PIN_NO 
    / (BSP_GPIO_AMP_MUTE_2_OUT_PIN_NO < 100U ? 10U : 100U)) - 1,

    (BSP_GPIO_CAN_WAKE_OUT_PIN_NO 
    / (BSP_GPIO_CAN_WAKE_OUT_PIN_NO < 100U ? 10U : 100U)) - 1,

    (BSP_GPIO_SPI_CS_OUT_PIN_NO 
    / (BSP_GPIO_SPI_CS_OUT_PIN_NO < 100U ? 10U : 100U)) - 1,

    (BSP_GPIO_AMP_1_ERROR_IN_PIN_NO 
    / (BSP_GPIO_AMP_1_ERROR_IN_PIN_NO < 100U ? 10U : 100U)) - 1,

    (BSP_GPIO_AMP_2_ERROR_IN_PIN_NO 
    / (BSP_GPIO_AMP_2_ERROR_IN_PIN_NO < 100U ? 10U : 100U)) - 1,

    (BSP_GPIO_CAN_ERROR_IN_PIN_NO 
    / (BSP_GPIO_CAN_ERROR_IN_PIN_NO < 100U ? 10U : 100U)) - 1,
};

/**
 * @brief This table specifies pin values.
 * 
 */
static uint32_t m_test_pin_arr[TEST_PIN_COUNT] = {

    (BSP_GPIO_AMP_STB_1_OUT_PIN_NO 
    % (BSP_GPIO_AMP_STB_1_OUT_PIN_NO < 100U ? 10U : 100U)),

    (BSP_GPIO_AMP_STB_2_OUT_PIN_NO 
    % (BSP_GPIO_AMP_STB_2_OUT_PIN_NO < 100U ? 10U : 100U)),

    (BSP_GPIO_CAN_STB_OUT_PIN_NO 
    % (BSP_GPIO_CAN_STB_OUT_PIN_NO < 100U ? 10U : 100U)),

    (BSP_GPIO_CAN_EN_OUT_PIN_NO 
    % (BSP_GPIO_CAN_EN_OUT_PIN_NO < 100U ? 10U : 100U)),

    (BSP_GPIO_AMP_MUTE_1_OUT_PIN_NO 
    % (BSP_GPIO_AMP_MUTE_1_OUT_PIN_NO < 100U ? 10U : 100U)),

    (BSP_GPIO_AMP_MUTE_2_OUT_PIN_NO 
    % (BSP_GPIO_AMP_MUTE_2_OUT_PIN_NO < 100U ? 10U : 100U)),

    (BSP_GPIO_CAN_WAKE_OUT_PIN_NO 
    % (BSP_GPIO_CAN_WAKE_OUT_PIN_NO < 100U ? 10U : 100U)),

    (BSP_GPIO_SPI_CS_OUT_PIN_NO 
    % (BSP_GPIO_SPI_CS_OUT_PIN_NO < 100U ? 10U : 100U)),    

    (BSP_GPIO_AMP_1_ERROR_IN_PIN_NO 
    % (BSP_GPIO_AMP_1_ERROR_IN_PIN_NO < 100U ? 10U : 100U)),

    (BSP_GPIO_AMP_2_ERROR_IN_PIN_NO 
    % (BSP_GPIO_AMP_2_ERROR_IN_PIN_NO < 100U ? 10U : 100U)),

    (BSP_GPIO_CAN_ERROR_IN_PIN_NO 
    % (BSP_GPIO_CAN_ERROR_IN_PIN_NO < 100U ? 10U : 100U)),       
};

/**
 * @brief This variables help of configure pin settings.
 * 
 */
pin_settings_config_t m_test_pin_settings[TEST_PIN_COUNT];

/**
 * @brief This variable checks when the callback function is called.
 * 
 */
static bool m_test_amp_1_flag = false;

/**
 * @brief This variable checks when the callback function is called.
 * 
 */
static bool m_test_amp_2_flag = false;

/**
 * @brief This variable checks when the callback function is called.
 * 
 */
static bool m_test_can_flag = false;

/**
 * @brief Callback function pointer.
 * 
 */
static isr_t m_test_port_isr_handler;

/*******************************************************************************
 * Global Variables
 ******************************************************************************/

uint8_t allocated_area[PERIPH_MEM_SIZE];

/*******************************************************************************
 * Private Functions Prototype Declarations
 ******************************************************************************/

/**
 * @brief Pin configuration function.
 * 
 */
static void fill_pin_cfg(void);

/**
 * @brief GPIO callback function.
 * 
 */
static void gpio_cb(void);

/**
 * @brief Callback of PINS_DRV_Init function.
 * 
 */
static status_t pins_drv_init_cb(uint32_t pinCount, 
                                 const pin_settings_config_t* config,
                                 int cmock_num_calls);

/**
 * @brief Callback of PINS_DRV_SetPinIntSel function.INT_SYS_InstallHandler
 * 
 */
static void pins_irq_cfg(PORT_Type* const base, 
                         uint32_t pin, 
                         port_interrupt_config_t intConfig, 
                         int cmock_num_calls);

/**
 * @brief Callback of INT_SYS_InstallHandler function.
 * 
 */
static void install_handler_cb(IRQn_Type irqNumber, 
                               const isr_t newHandler, 
                               isr_t* const oldHandler, 
                               int cmock_num_calls);

/**
 * @brief Amplifier-1 callback function.
 * 
 */
static void amp_1_callback(void);

/**
 * @brief Amplifier-2 callback function.
 * 
 */
static void amp_2_callback(void);

/**
 * @brief CAN callback function.
 * 
 */
static void can_callback(void);

/**
 * @brief This function helps to initialize gpio module. 
 * 
 */
static void helper_gpio_init(void);

/*******************************************************************************
 * Setup & Teardown 
 ******************************************************************************/

void setUp()
{
    log_print_Ignore();

    m_test_amp_1_flag = false;
    m_test_amp_2_flag = false;
    m_test_can_flag = false;
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
 * @brief This test verifies that gpio module successfully initialize with user 
 *        configurations.
 * 
 * @link https://codebeamer.daiichi.com/issue/183899
 *  
 */
void test_bsp_gpio_init__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    PINS_DRV_Init_AddCallback(pins_drv_init_cb);

    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    
    PINS_DRV_SetPinIntSel_AddCallback(pins_irq_cfg);

    PINS_DRV_SetPinIntSel_ExpectAnyArgs();
    PINS_DRV_SetPinIntSel_ExpectAnyArgs();
    PINS_DRV_SetPinIntSel_ExpectAnyArgs();


    INT_SYS_InstallHandler_Expect(PORT_IRQn,
                                  gpio_cb,
                                  NULL);

    INT_SYS_InstallHandler_IgnoreArg_newHandler();

    INT_SYS_EnableIRQ_Expect(PORT_IRQn);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_gpio_init();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief This test verifies that the gpio module resets the callback value on 
 *        successful initialization.
 * 
 * @link https://codebeamer.daiichi.com/item/186596
 *  
 */
void test_bsp_gpio_init__usual_run__callback_assignment_removed(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);
    
    PINS_DRV_SetPinIntSel_AddCallback(pins_irq_cfg);

    PINS_DRV_SetPinIntSel_ExpectAnyArgs();
    PINS_DRV_SetPinIntSel_ExpectAnyArgs();
    PINS_DRV_SetPinIntSel_ExpectAnyArgs();

    INT_SYS_InstallHandler_AddCallback(install_handler_cb);

    INT_SYS_InstallHandler_ExpectAnyArgs();

    INT_SYS_EnableIRQ_Expect(PORT_IRQn);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_gpio_init();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);

    TEST_STEP(3);
    m_test_port_isr_handler();
}

/**
 * @brief This test verifies that gpio module did not initialize successfully 
 *        because the gpio initialization failed.
 * 
 * @link https://codebeamer.daiichi.com/issue/183907
 *  
 */
void test_bsp_gpio_init__pin_init_error__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_ERROR);    

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = bsp_gpio_init();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief This test verifies that gpio module successfully get the gpio status.
 * 
 * @link https://codebeamer.daiichi.com/issue/183908
 *  
 */
void test_bsp_gpio_status_get__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint32_t pin_num = 0U;

    // **************************** Pre-Actions ***************************** //

    pin_num = BSP_GPIO_AMP_1_ERROR_IN_PIN_NO;

    // ************************** Expected Calls **************************** //

    bsp_gpio_status_t port_status = BSP_GPIO_STATUS_SET;

    PINS_DRV_ReadPins_ExpectAndReturn(PTB, port_status);    

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_gpio_status_t ret = bsp_gpio_status_get(pin_num);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(BSP_GPIO_STATUS_SET, ret);   
}

/**
 * @brief This test verifies that gpio module did not get gpio status 
 *        successfully because the port value incorrect.
 * 
 * @link https://codebeamer.daiichi.com/issue/183909
 *  
 */
void test_bsp_gpio_status_get__invalid_port_value__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint32_t pin_num = 0U;

    // **************************** Pre-Actions ***************************** //

    pin_num = 616U;

    // ************************** Expected Calls **************************** // 

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_gpio_status_t ret = bsp_gpio_status_get(pin_num);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(BSP_GPIO_STATUS_ERR, ret);   
}

/**
 * @brief This test verifies that gpio module did not get gpio status 
 *        successfully because the pin value incorrect.
 * 
 * @link https://codebeamer.daiichi.com/issue/183910
 *  
 */
void test_bsp_gpio_status_get__invalid_pin_value__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint32_t pin_num = 0U;

    // **************************** Pre-Actions ***************************** //
        
    pin_num = 217U;

    // ************************** Expected Calls **************************** // 

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_gpio_status_t ret = bsp_gpio_status_get(pin_num);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(BSP_GPIO_STATUS_ERR, ret);   
}

/**
 * @brief This test verifies that gpio module successfully set the gpio pin.
 * 
 * @link https://codebeamer.daiichi.com/issue/183911
 *  
 */
void test_bsp_gpio_set__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint32_t pin_num = 0U;

    // **************************** Pre-Actions ***************************** //

    pin_num = BSP_GPIO_AMP_STB_1_OUT_PIN_NO;

    // ************************** Expected Calls **************************** // 

    uint32_t pin_idx = 0U;
    
    pins_level_type_t set = 1U;

    PINS_DRV_WritePin_Expect(PTC,
                             m_test_pin_arr[pin_idx],
                             set);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_gpio_set(pin_num);
}

/**
 * @brief This test verifies that gpio module did not set gpio pin successfully 
 *        because the port value incorrect.
 * 
 * @link https://codebeamer.daiichi.com/issue/183912
 *  
 */
void test_bsp_gpio_set__invalid_port_value__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint32_t pin_num = 0U;

    // **************************** Pre-Actions ***************************** //

    pin_num = 601U;

    // ************************** Expected Calls **************************** // 

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_gpio_set(pin_num);
}

/**
 * @brief This test verifies that gpio module did not set gpio pin successfully 
 *        because the pin value incorrect.
 * 
 * @link https://codebeamer.daiichi.com/issue/183913
 *  
 */
void test_bsp_gpio_set__invalid_pin_value__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint32_t pin_num = 1U;

    // **************************** Pre-Actions ***************************** //

    pin_num = 0U;

    // ************************** Expected Calls **************************** // 

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_gpio_set(pin_num);
}

/**
 * @brief This test verifies that gpio module successfully reset the gpio pin.
 * 
 * @link https://codebeamer.daiichi.com/issue/183914
 *  
 */
void test_bsp_gpio_reset__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint32_t pin_num = 0U;

    // **************************** Pre-Actions ***************************** //

    pin_num = BSP_GPIO_AMP_STB_2_OUT_PIN_NO;

    // ************************** Expected Calls **************************** // 

    pins_level_type_t reset = 0U;

    uint32_t pin_idx = 1U;

    PINS_DRV_WritePin_Expect(PTC,
                             m_test_pin_arr[pin_idx],
                             reset);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_gpio_reset(pin_num);
}

/**
 * @brief This test verifies that gpio module did not reset gpio pin 
 *        successfully because the port value incorrect.
 * 
 * @link https://codebeamer.daiichi.com/issue/183915
 *  
 */
void test_bsp_gpio_reset__invalid_port_value__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    uint32_t pin_num = 611U;

    // ************************** Expected Calls **************************** // 

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_gpio_reset(pin_num);
}

/**
 * @brief This test verifies that gpio module did not reset gpio pin 
 *        successfully because the pin value incorrect.
 * 
 * @link https://codebeamer.daiichi.com/issue/183916
 *  
 */
void test_bsp_gpio_reset__invalid_pin_value__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    uint32_t pin_num = 117U;

    // ************************** Expected Calls **************************** // 

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_gpio_reset(pin_num);
}

/**
 * @brief This test verifies that gpio module successfully set user callback 
 *        function.
 * 
 * @link https://codebeamer.daiichi.com/issue/184135
 *  
 */
void test_bsp_gpio_cb_set__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    bsp_gpio_cb_t callback = NULL;

    // **************************** Pre-Actions ***************************** //
    
    callback = gpio_cb;

    // ************************** Expected Calls **************************** // 

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_gpio_cb_set(BSP_GPIO_AMP_1_ERROR_IN_PIN_NO, callback);
}

/**
 * @brief This test verifies that gpio module did not successfully set user 
 *        callback function.
 * 
 * @link https://codebeamer.daiichi.com/issue/184136
 *  
 */
void test_bsp_gpio_cb_set__invalid_callback_parameter__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    bsp_gpio_cb_t callback = NULL;

    // **************************** Pre-Actions ***************************** //

    callback = NULL;

    // ************************** Expected Calls **************************** // 

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_gpio_cb_set(BSP_GPIO_AMP_1_ERROR_IN_PIN_NO, callback);
}

/**
 * @brief This test verifies that gpio module successfully register and call 
 *        callback function.
 * 
 * @link https://codebeamer.daiichi.com/issue/184430
 * 
 */
void test_bsp_gpio_cb_set__install_and_call_callback__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_gpio_init();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bsp_gpio_cb_set(BSP_GPIO_AMP_1_ERROR_IN_PIN_NO, amp_1_callback);

    TEST_STEP(2);
    bsp_gpio_cb_set(BSP_GPIO_AMP_2_ERROR_IN_PIN_NO, amp_2_callback);

    TEST_STEP(3);
    bsp_gpio_cb_set(BSP_GPIO_CAN_ERROR_IN_PIN_NO, can_callback);

    TEST_STEP(4);
    m_test_port_isr_handler();

    TEST_STEP(5);
    TEST_ASSERT_EQUAL(true, m_test_amp_1_flag);
    TEST_ASSERT_EQUAL(true, m_test_amp_2_flag);
    TEST_ASSERT_EQUAL(true, m_test_can_flag);
}

/*******************************************************************************
 * Private Functions Definitions
 ******************************************************************************/

static void fill_pin_cfg(void)
{
    PORT_Type *port_base_arr[] = {
        PORTA,
        PORTB,
        PORTC,
        PORTD,
        PORTE,

    };

    GPIO_Type *gpio_base_arr[] = {
        PTA,
        PTB,
        PTC,
        PTD,
        PTE,

    };

    for (uint8_t i = 0U; i < TEST_PIN_COUNT; ++i)
    {
        m_test_pin_settings[i].pullConfig = PORT_INTERNAL_PULL_NOT_ENABLED;
        m_test_pin_settings[i].passiveFilter = false;
        m_test_pin_settings[i].driveSelect = PORT_LOW_DRIVE_STRENGTH;        
        m_test_pin_settings[i].mux = PORT_MUX_AS_GPIO;
        m_test_pin_settings[i].pinLock = false;
        m_test_pin_settings[i].intConfig = PORT_DMA_INT_DISABLED;
        m_test_pin_settings[i].digitalFilter = false;        
        m_test_pin_settings[i].clearIntFlag = false;    
        m_test_pin_settings[i].initValue = 0U;
        m_test_pin_settings[i].base = 
            port_base_arr[m_test_port_arr[i]];
        m_test_pin_settings[i].direction = (i < TEST_OUTPUT_COUNT) 
                                           ? GPIO_OUTPUT_DIRECTION 
                                           : GPIO_INPUT_DIRECTION;
        m_test_pin_settings[i].gpioBase = 
            gpio_base_arr[m_test_port_arr[i]];
        m_test_pin_settings[i].pinPortIdx = m_test_pin_arr[i];
    }
}

static void gpio_cb(void)
{
    ;
}

static status_t pins_drv_init_cb(uint32_t pinCount, 
                                 const pin_settings_config_t* config,
                                 int cmock_num_calls)
{
    (void)cmock_num_calls;

    fill_pin_cfg();

    TEST_ASSERT_EQUAL(TEST_PIN_COUNT, pinCount);

    for (uint8_t i = 0U; i < TEST_PIN_COUNT; ++i)
    {
        TEST_ASSERT_EQUAL(m_test_pin_settings[i].base, 
                          config[i].base);
        TEST_ASSERT_EQUAL(m_test_pin_settings[i].clearIntFlag, 
                          config[i].clearIntFlag);
        TEST_ASSERT_EQUAL(m_test_pin_settings[i].digitalFilter, 
                          config[i].digitalFilter);
        TEST_ASSERT_EQUAL(m_test_pin_settings[i].direction, 
                          config[i].direction);
        TEST_ASSERT_EQUAL(m_test_pin_settings[i].driveSelect, 
                          config[i].driveSelect);
        TEST_ASSERT_EQUAL(m_test_pin_settings[i].gpioBase, 
                          config[i].gpioBase);
        TEST_ASSERT_EQUAL(m_test_pin_settings[i].initValue, 
                          config[i].initValue);
        TEST_ASSERT_EQUAL(m_test_pin_settings[i].intConfig, 
                          config[i].intConfig);
        TEST_ASSERT_EQUAL(m_test_pin_settings[i].mux, 
                          config[i].mux);
        TEST_ASSERT_EQUAL(m_test_pin_settings[i].passiveFilter, 
                          config[i].passiveFilter);
        TEST_ASSERT_EQUAL(m_test_pin_settings[i].pinLock, 
                          config[i].pinLock);
        TEST_ASSERT_EQUAL(m_test_pin_settings[i].pinPortIdx, 
                          config[i].pinPortIdx);
        TEST_ASSERT_EQUAL(m_test_pin_settings[i].pullConfig, 
                          config[i].pullConfig);
    }

    return STATUS_SUCCESS;
}

static void pins_irq_cfg(PORT_Type* const base, 
                         uint32_t pin, 
                         port_interrupt_config_t intConfig, 
                         int cmock_num_calls)
{
    PORT_Type *port_base_arr[] = {
        PORTA,
        PORTB,
        PORTC,
        PORTD,
        PORTE,

    };

    uint8_t isr_edge_tbl[3U] = {
        BSP_GPIO_CFG_AMP_1_ERROR_IN_PIN,
        BSP_GPIO_CFG_AMP_2_ERROR_IN_PIN,
        BSP_GPIO_CFG_CAN_ERROR_IN_PIN,
    
    };

    const port_interrupt_config_t isr_edge_arr[] = {
        0U,
        PORT_INT_EITHER_EDGE,
        PORT_INT_RISING_EDGE,
        0U,
        PORT_INT_FALLING_EDGE,

    };    

    TEST_ASSERT_EQUAL(
        port_base_arr[m_test_port_arr[8U + cmock_num_calls]], 
        base);

    TEST_ASSERT_EQUAL(m_test_pin_arr[8U + cmock_num_calls], pin);

    TEST_ASSERT_EQUAL(isr_edge_arr[isr_edge_tbl[cmock_num_calls]], intConfig);
}

static void install_handler_cb(IRQn_Type irqNumber, 
                               const isr_t newHandler, 
                               isr_t* const oldHandler, 
                               int cmock_num_calls)
{
    (void)irqNumber;
    (void)oldHandler;
    (void)cmock_num_calls;

    PINS_DRV_GetPortIntFlag_ExpectAndReturn(PORTB, 1U);
    PINS_DRV_GetPortIntFlag_ExpectAndReturn(PORTB, 2U);
    PINS_DRV_GetPortIntFlag_ExpectAndReturn(PORTC, 2U);

    PINS_DRV_ClearPortIntFlagCmd_Expect(PORTC);

    m_test_port_isr_handler = newHandler;
}

static void amp_1_callback(void)
{
    m_test_amp_1_flag = true;
}

static void amp_2_callback(void)
{
    m_test_amp_2_flag = true;
}

static void can_callback(void)
{
    m_test_can_flag = true;
}

static void helper_gpio_init(void)
{
    PINS_DRV_Init_ExpectAnyArgsAndReturn(STATUS_SUCCESS);

    PINS_DRV_SetPinIntSel_ExpectAnyArgs();
    PINS_DRV_SetPinIntSel_ExpectAnyArgs();
    PINS_DRV_SetPinIntSel_ExpectAnyArgs();

    INT_SYS_InstallHandler_AddCallback(install_handler_cb);

    INT_SYS_InstallHandler_ExpectAnyArgs();

    INT_SYS_EnableIRQ_ExpectAnyArgs();

    (void)bsp_gpio_init();
}

// end of file 
