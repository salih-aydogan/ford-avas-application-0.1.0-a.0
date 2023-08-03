// PRQA S 3108 EOF # Comments with links are required for ensure traceability.
// PRQA S 7008 EOF # If avariable is a term, it can be named shorter than 3 characters.
/**
 * @file bsp_gpio.c
 * @author Furkan Yildirim (furkan.yildirim@daiichi.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-08
 * 
 * @copyright Copyright (c) 2023
 * 
 * https://codebeamer.daiichi.com/issue/183300
 * 
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "bsp_gpio.h"

#include "pins_driver.h"
#include "interrupt_manager.h"

#include "log.h"

LOG_MODULE_NAME("BSP_GPIO");

/*******************************************************************************
 * Macros
 ******************************************************************************/

// PRQA S 380 ++ # Too many macros in the driver provided by NXP causes this error.

/**
 * @brief https://codebeamer.daiichi.com/issue/183987
 * 
 */
#define INIT_PINS_CNT_MAX       11U

/**
 * @brief https://codebeamer.daiichi.com/issue/183988
 * 
 */
#define INTERRUPT_PINS_CNT_MAX  3U

// PRQA S 380 --

/*******************************************************************************
 * Structs
 ******************************************************************************/

// PRQA S 3213 ++ # Allowed for Daiichi code rules.

/**
 * @brief https://codebeamer.daiichi.com/issue/184640
 * 
 */
typedef struct interrupt_pins_data_s
{

    uint32_t pin_number;
    uint32_t interrupt_type;

} interrupt_pins_data_t;

// PRQA S 3213 --

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/183989
 * 
 */
static bsp_gpio_cb_t m_gpio_cb[INTERRUPT_PINS_CNT_MAX];

/**
 * @brief https://codebeamer.daiichi.com/issue/184404
 * 
 */
static uint32_t const m_init_pins_table[INIT_PINS_CNT_MAX] = {

    BSP_GPIO_AMP_STB_1_OUT_PIN_NO,
    BSP_GPIO_AMP_STB_2_OUT_PIN_NO,
    BSP_GPIO_CAN_STB_OUT_PIN_NO,
    BSP_GPIO_CAN_EN_OUT_PIN_NO,
    BSP_GPIO_AMP_MUTE_1_OUT_PIN_NO,
    BSP_GPIO_AMP_MUTE_2_OUT_PIN_NO,
    BSP_GPIO_CAN_WAKE_OUT_PIN_NO,
    BSP_GPIO_SPI_CS_OUT_PIN_NO,
    BSP_GPIO_AMP_1_ERROR_IN_PIN_NO,
    BSP_GPIO_AMP_2_ERROR_IN_PIN_NO,
    BSP_GPIO_CAN_ERROR_IN_PIN_NO,
};

/**
 * @brief https://codebeamer.daiichi.com/issue/184642
 * 
 */
static interrupt_pins_data_t const m_interrupt_pins[INTERRUPT_PINS_CNT_MAX] = {

    [0U].pin_number = BSP_GPIO_AMP_1_ERROR_IN_PIN_NO,
    [0U].interrupt_type = BSP_GPIO_CFG_AMP_1_ERROR_IN_PIN,
    [1U].pin_number = BSP_GPIO_AMP_2_ERROR_IN_PIN_NO,
    [1U].interrupt_type = BSP_GPIO_CFG_AMP_2_ERROR_IN_PIN,
    [2U].pin_number = BSP_GPIO_CAN_ERROR_IN_PIN_NO,
    [2U].interrupt_type = BSP_GPIO_CFG_CAN_ERROR_IN_PIN,
};

// PRQA S 306 ++ # Defined by nxp library to hold register base address. False positive warning.

/**
 * @brief https://codebeamer.daiichi.com/issue/184643
 * 
 */
static PORT_Type* const m_mcu_ports[5U] = {PORTA, PORTB, PORTC, PORTD, PORTE};

/**
 * @brief https://codebeamer.daiichi.com/issue/184644
 * 
 */
static GPIO_Type* const m_port_bases[5U] = {PTA, PTB, PTC, PTD, PTE};

// PRQA S 306 --

/*******************************************************************************
 * Private Function Declarations
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/183966
 * 
 */
static bool pin_num_convert(uint32_t pin_num,
                            uint32_t* p_port, 
                            uint32_t* p_pin);

/**
 * @brief https://codebeamer.daiichi.com/issue/183396
 * 
 */
static bool pin_init(void);

/**
 * @brief https://codebeamer.daiichi.com/issue/183960
 * 
 */
static void pin_interrupt_set(void);

/**
 * @brief https://codebeamer.daiichi.com/issue/183967
 * 
 */
static void port_isr(void);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/183303
 *  
 */
bool bsp_gpio_init(void)
{    
    for (uint32_t i = 0U; i < INTERRUPT_PINS_CNT_MAX; ++i)
    {
        m_gpio_cb[i] = NULL;
    }

    bool ret = pin_init();

    if (true == ret)
    {
        pin_interrupt_set();

        INT_SYS_InstallHandler(PORT_IRQn, port_isr, NULL);

        INT_SYS_EnableIRQ(PORT_IRQn);

        LOG_INFO("Initialized");
    }
    else
    {
        LOG_ERROR("Init Fail");
    }
    
    return ret;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/183305
 *  
 */
void bsp_gpio_set(uint32_t pin_num)
{
    uint32_t port = 0U;
    uint32_t pin = 0U;

    bool ret = pin_num_convert(pin_num, &port, &pin);    

    if (true == ret)
    {   
        PINS_DRV_WritePin(m_port_bases[port], 
                          (pins_channel_type_t)pin, 
                          (pins_level_type_t)BSP_GPIO_STATUS_SET); 
    }
}

/**
 * @brief https://codebeamer.daiichi.com/issue/183349
 *  
 */
void bsp_gpio_reset(uint32_t pin_num)
{
    uint32_t port = 0U;
    uint32_t pin = 0U;

    bool ret = pin_num_convert(pin_num, &port, &pin);    
                   
    if (true == ret)
    {                   
        PINS_DRV_WritePin(m_port_bases[port], 
                          (pins_channel_type_t)pin, 
                          (pins_level_type_t)BSP_GPIO_STATUS_RESET);
    }                   
}

/**
 * @brief https://codebeamer.daiichi.com/issue/183304
 *  
 */
bsp_gpio_status_t bsp_gpio_status_get(uint32_t pin_num)
{
    uint32_t port = 0U;
    uint32_t pin = 0U;

    bool ret = pin_num_convert(pin_num, &port, &pin);    

    if (true == ret)
    {
        pins_channel_type_t port_status = PINS_DRV_ReadPins(m_port_bases[port]);

        bsp_gpio_status_t status = (0x01U == ((port_status >> pin) & 0x01U))
                                   ? BSP_GPIO_STATUS_SET
                                   : BSP_GPIO_STATUS_RESET;
        
        return status;
    }

    return BSP_GPIO_STATUS_ERR;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/183302
 *  
 */
void bsp_gpio_cb_set(uint32_t pin_num, bsp_gpio_cb_t callback)
{
    if (NULL == callback)
    {
        LOG_ERROR("Invalid argument in bsp_gpio_cb_set");

        return;           
    }

    for (uint32_t i = 0U; i < INTERRUPT_PINS_CNT_MAX; i++)
    {
        if (pin_num == m_interrupt_pins[i].pin_number)
        {
            m_gpio_cb[i] = callback;
        }
    }
}

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/183966
 * 
 */
static bool pin_num_convert(uint32_t pin_num,
                            uint32_t* p_port, 
                            uint32_t* p_pin)
{
    bool number_is_valid = false;

    for (uint32_t i = 0U; 
         ((i < INIT_PINS_CNT_MAX) && (false == number_is_valid)); 
         i++)
    {
        if (pin_num == m_init_pins_table[i])
        {
            number_is_valid = true;

            uint32_t divider = (pin_num < 100U) ? 10U : 100U;
            *p_port = (pin_num / divider) - 1U;
            *p_pin = pin_num % divider;
        }
    }

    return number_is_valid;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/183396
 * 
 */
static bool pin_init(void)
{    
    pin_settings_config_t pin_init_cfg[INIT_PINS_CNT_MAX];

    for (uint32_t i = 0U; i < INIT_PINS_CNT_MAX; i++)
    {
        uint32_t divider = (m_init_pins_table[i] < 100U) ? 10U : 100U;

        uint32_t port = (m_init_pins_table[i] / divider) - 1U;
        uint32_t pin = m_init_pins_table[i] % divider;

        pin_init_cfg[i].base          = m_mcu_ports[port];
        pin_init_cfg[i].pinPortIdx    = pin;          
        pin_init_cfg[i].pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED;
        pin_init_cfg[i].passiveFilter = false; 
        pin_init_cfg[i].driveSelect   = PORT_LOW_DRIVE_STRENGTH; 
        pin_init_cfg[i].mux           = PORT_MUX_AS_GPIO; 
        pin_init_cfg[i].pinLock       = false; 
        pin_init_cfg[i].intConfig     = PORT_DMA_INT_DISABLED; 
        pin_init_cfg[i].clearIntFlag  = false; 
        pin_init_cfg[i].digitalFilter = false; 
        pin_init_cfg[i].gpioBase      = m_port_bases[port];  
        pin_init_cfg[i].direction     = (i < 8U) 
                                        ? GPIO_OUTPUT_DIRECTION 
                                        : GPIO_INPUT_DIRECTION;
        pin_init_cfg[i].initValue     = 0U;                         
    }

    status_t ret = PINS_DRV_Init(INIT_PINS_CNT_MAX, pin_init_cfg); 

    for (uint8_t cnt = 0; cnt < INIT_PINS_CNT_MAX; cnt++)
    {
        LOG_DEBUG("Pin init; base:%u, pin:%u, dir:%u, pull:%u, status:%u", 
                  pin_init_cfg[cnt].base, 
                  pin_init_cfg[cnt].pinPortIdx,
                  pin_init_cfg[cnt].direction,
                  pin_init_cfg[cnt].pullConfig,
                  ret);
    }

    return (STATUS_SUCCESS == ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/183960
 * 
 */
static void pin_interrupt_set(void)
{
    uint32_t port = 0U;
    uint32_t pin = 0U;

    port_interrupt_config_t pin_interrupt_type = PORT_DMA_INT_DISABLED;

    for (uint32_t i = 0U; i < INTERRUPT_PINS_CNT_MAX; i++)
    {
        bool ret = pin_num_convert(m_interrupt_pins[i].pin_number, &port, &pin);

        if (true == ret)
        {
            if (BSP_GPIO_INT_BOTH_EDGE == m_interrupt_pins[i].interrupt_type)
            {
                pin_interrupt_type = PORT_INT_EITHER_EDGE;
            }

            if (BSP_GPIO_INT_RISING_EDGE == m_interrupt_pins[i].interrupt_type)
            {
                pin_interrupt_type = PORT_INT_RISING_EDGE;
            }

            if (BSP_GPIO_INT_FALLING_EDGE == m_interrupt_pins[i].interrupt_type)
            {
                pin_interrupt_type = PORT_INT_FALLING_EDGE;
            }  
       
            PINS_DRV_SetPinIntSel(m_mcu_ports[port], pin, pin_interrupt_type);

            LOG_DEBUG("Pin interrupt set; base:%u, pin:%u, int type:%u", 
                      m_mcu_ports[port], 
                      pin,
                      pin_interrupt_type);
        }
    }
}

/**
 * @brief https://codebeamer.daiichi.com/issue/183967
 * 
 */
static void port_isr(void)
{
    uint32_t port = 0;
    uint32_t pin = 0;

    for (uint32_t i = 0U; i < INTERRUPT_PINS_CNT_MAX; i++)
    {
        (void)pin_num_convert(m_interrupt_pins[i].pin_number, &port, &pin);

        uint32_t status = PINS_DRV_GetPortIntFlag(m_mcu_ports[port]);

        if (0x01U == ((status >> pin) & 0x01U))
        {
            if (NULL != m_gpio_cb[i])
            {
                m_gpio_cb[i](); 
            }
            else
            {
                LOG_DEBUG("NULL callback variable");
            }
        }   
    }

    PINS_DRV_ClearPortIntFlagCmd(m_mcu_ports[port]);
}

// end of file
