// PRQA S 3108 EOF # Suppressed, Comments with links are required for ensure traceability.
// PRQA S 380 EOF # Suppressed, Too many macros in the driver provided by NXP causes this error.
/**
 * @file bsp_uart.c
 * @author Berke Yalcinkaya (berke.yalcinkaya@daiichi.com)
 * @brief Implementation of uart component.
 * @version 0.1
 * @date 2023-01-27
 *
 * @copyright Copyright (c) 2023
 *
 * https://codebeamer.daiichi.com/issue/180818
 *
 */

#include "bsp_uart.h"

#include "pins_driver.h"
#include "edma_driver.h"
#include "lpuart_driver.h"

#include "log.h"

LOG_MODULE_NAME("BSP_UART");

/*******************************************************************************
 * Macros
 ******************************************************************************/

/**
 * @brief This macro specifies eDMA channel-0 number.
 *
 * https://codebeamer.daiichi.com/issue/180993
 *
 */
#define EDMA_CH_0                   (0U)

/**
 * @brief This macro specifies UART tx pin number.
 *
 * https://codebeamer.daiichi.com/issue/181000
 *
 */
#define UART_TX_PIN_NO              (3U)

/**
 * @brief This macro specifies UART pin count.
 *
 * https://codebeamer.daiichi.com/issue/181001
 *
 */
#define UART_PIN_COUNT              (1U)

/**
 * @brief This macro specifies which uart unit to use.
 *
 * https://codebeamer.daiichi.com/issue/181002
 *
 */
#define UART_INSTANCE               (0U)

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/**
 * @brief Data structure variable for eDMA channel state.
 *
 * https://codebeamer.daiichi.com/issue/181005
 *
 */
static edma_chn_state_t m_edma_ch_state;

/**
 * @brief Runtime state variable of the LPUART driver.
 *
 * https://codebeamer.daiichi.com/issue/181006
 *
 */
static lpuart_state_t m_lpuart_state;

/**
 * @brief Callback funtion variable for uart write.
 *
 * https://codebeamer.daiichi.com/issue/181013
 *
 */
static bsp_uart_err_cb_t m_uart_write_cb;

/*******************************************************************************
 * Private Function Declarations
 ******************************************************************************/

/**
 * @brief UART transmit callback function. This callback function is called
 *        when the sending operation is complete or an error occurs. It
 *        calls the user callback function defined from the upper
 *        layer.
 *
 * https://codebeamer.daiichi.com/issue/181174
 *
 */
static void uart_write_cb(void * driverState, uart_event_t event, void * userData); 

/**
 * @brief This function uses the NXP driver to initialize UART tx pin.
 *
 * https://codebeamer.daiichi.com/issue/181015
 *
 */
static bool gpio_init(void);

/**
 * @brief This function uses the NXP driver to initialize eDMA unit.
 *
 * https://codebeamer.daiichi.com/issue/181016
 *
 */
static bool edma_init(void);

/**
 * @brief This function uses the NXP driver to initialize UART unit.
 *
 * https://codebeamer.daiichi.com/issue/180819
 *
 */
static bool uart_init(void);

/**
 * @brief This function uses the NXP driver to deinitialize UART unit.
 *
 * https://codebeamer.daiichi.com/issue/181173
 *
 */
static bool uart_deinit(void);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/181017
 *
 */
bool bsp_uart_init(void)
{
    bool ret = uart_init();

    if (true == ret)
    {
        LOG_INFO("Initialized");
    }
    else
    {
        LOG_ERROR("Init Fail");
    }

    return ret;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/181093
 *
 */
bool bsp_uart_deinit(void)
{
    bool ret = uart_deinit();

    if (true == ret)
    {
        LOG_INFO("Deinitialized");
    }
    else
    {
        LOG_ERROR("Deinit Fail");
    }

    return ret;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/180820
 *
 */
bool bsp_uart_async_write(const uint8_t *p_data,
                          size_t size)
{
    if (NULL == p_data)
    {
        LOG_ERROR("Invalid argument in bsp_uart_async_write");
        
        return false;
    }

    if (0U == size)
    {
        LOG_ERROR("Zero size in bsp_uart_async_write");
        
        return false;
    }

    status_t status = LPUART_DRV_GetTransmitStatus((uint32_t)UART_INSTANCE,
                                                   NULL);

    if (STATUS_SUCCESS == status)
    {
        status = LPUART_DRV_SendData((uint32_t)UART_INSTANCE,
                                     p_data,
                                     (uint32_t)size);
    }

    return (STATUS_SUCCESS == status);
}

/**
 * @brief TODO
 *
 */
bool bsp_uart_write(const uint8_t *p_data,
                    size_t size)
{
    if (NULL == p_data)
    {
        LOG_ERROR("Invalid argument in bsp_uart_write");
        
        return false;
    }

    if (0U == size)
    {
        LOG_ERROR("Zero size in bsp_uart_write");
        
        return false;
    }

    status_t status = STATUS_BUSY;

    while (STATUS_BUSY == status)
    {
        status = LPUART_DRV_GetTransmitStatus((uint32_t)UART_INSTANCE, NULL);
    }

    status = LPUART_DRV_SendDataBlocking((uint32_t)UART_INSTANCE,
                                         p_data,
                                         (uint32_t)size,
                                         1000U);

    return (STATUS_SUCCESS == status);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/181094
 *
 */
void bsp_uart_cb_set(bsp_uart_err_cb_t callback)
{
    if (NULL == callback)
    {
        LOG_ERROR("Invalid argument in bsp_uart_cb_set");
        
        return;
    }

    m_uart_write_cb = callback;
}

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/181015
 *
 */
static bool gpio_init(void)
{
    pin_settings_config_t const gpio_cfg_tbl[UART_PIN_COUNT] = {

        {
        // PRQA S 306 ++ # Suppressed, Defined by nxp library to hold register base address. False positive warning.
            .base = PORTC,
        // PRQA S 306 --
            .pinPortIdx = UART_TX_PIN_NO,
            .pullConfig = PORT_INTERNAL_PULL_NOT_ENABLED,
            .driveSelect = PORT_LOW_DRIVE_STRENGTH,
            .passiveFilter = false,
            .mux = PORT_MUX_ALT4,
            .pinLock = false,
            .intConfig = PORT_DMA_INT_DISABLED,
            .clearIntFlag = false,
            .gpioBase = NULL,
            .digitalFilter = false,
        },
    }; 

    status_t status = PINS_DRV_Init((uint32_t)UART_PIN_COUNT,
                                    gpio_cfg_tbl);

    for (uint8_t cnt = 0; cnt < UART_PIN_COUNT; cnt++)
    {
        LOG_DEBUG("Pin init; base:%u, pin:%u, dir:%u, pull:%u, status:%u", 
                  gpio_cfg_tbl[cnt].base, 
                  gpio_cfg_tbl[cnt].pinPortIdx,
                  gpio_cfg_tbl[cnt].direction,
                  gpio_cfg_tbl[cnt].pullConfig,
                  status);
    }

    return (STATUS_SUCCESS == status);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/181016
 *
 */
static bool edma_init(void)
{
    const edma_channel_config_t edma_ch_cfg = {

        .channelPriority = EDMA_CHN_PRIORITY_0,
        .virtChnConfig = EDMA_CH_0,
        .source = EDMA_REQ_LPUART0_TX,
        .callback = NULL,
        // PRQA S 2983 ++ # Suppressed, It is used within the nxp library. False positive warning.
        .enableTrigger = false,
        // PRQA S 2983 --
        .callbackParam = NULL,
    };

    status_t status = EDMA_DRV_ChannelInit(&m_edma_ch_state, &edma_ch_cfg);

    return (STATUS_SUCCESS == status);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/180819
 *
 */
static bool uart_init(void)
{
    lpuart_user_config_t lpuart_user_cfg = {0U};

    lpuart_user_cfg.transferType = LPUART_USING_DMA;
    lpuart_user_cfg.baudRate = BSP_UART_CFG_BAUDRATE;
    lpuart_user_cfg.parityMode = LPUART_PARITY_DISABLED;
    lpuart_user_cfg.stopBitCount = LPUART_ONE_STOP_BIT;
    lpuart_user_cfg.bitCountPerChar = LPUART_8_BITS_PER_CHAR;
    lpuart_user_cfg.txDMAChannel = EDMA_CH_0;

    bool ret = gpio_init();

    if (false == ret)
    {
        return false;
    }

    ret = edma_init();

    if (false == ret)
    {
        return false;
    }

    status_t status = LPUART_DRV_Init((uint32_t)UART_INSTANCE,
                                      &m_lpuart_state,
                                      &lpuart_user_cfg);

    (void)LPUART_DRV_InstallTxCallback((uint32_t)UART_INSTANCE,
                                       (uart_callback_t)uart_write_cb,
                                       NULL);

    LOG_DEBUG("UART Init; instance:%u, baud:%u, stopbit:%u, parity:%u, status:%u", 
              UART_INSTANCE,
              lpuart_user_cfg.baudRate,
              lpuart_user_cfg.stopBitCount,
              lpuart_user_cfg.parityMode,
              status);

    return (STATUS_SUCCESS == status);
}

// PRQA S 3673 ++ # Suppressed, No const arguments were used in the prototype defined in the nxp library.
/**
 * @brief https://codebeamer.daiichi.com/issue/181174
 *
 */
static void uart_write_cb(void *driverState, uart_event_t event, void *userData)
{
    (void)userData;

    if (NULL == m_uart_write_cb)
    {
        return;
    }

    const lpuart_state_t *uart_state = (const lpuart_state_t *)driverState;

    status_t transmit_status = uart_state->transmitStatus;

    bsp_uart_err_t uart_err = BSP_UART_ERR_NONE;

    if (UART_EVENT_ERROR == event)
    {
        uart_err = BSP_UART_ERR_DMA;
    }
    else if (UART_EVENT_TX_EMPTY == event)
    {
        uart_err = BSP_UART_ERR_TX_EMPTY;
    }
    else if (UART_EVENT_END_TRANSFER == event)
    {
        if (STATUS_UART_ABORTED == transmit_status)
        {
            uart_err = BSP_UART_ERR_ABORTED;
        }
    }
    else
    {
        uart_err = BSP_UART_ERR_NONE;
    }

    m_uart_write_cb(uart_err);
}

// PRQA S 3673 --

/**
 * @brief https://codebeamer.daiichi.com/issue/181173
 *
 */
static bool uart_deinit(void)
{
    status_t status = EDMA_DRV_ReleaseChannel(EDMA_CH_0);

    if (STATUS_SUCCESS != status)
    {
        return false;
    }

    status = LPUART_DRV_Deinit(UART_INSTANCE);

    if (STATUS_SUCCESS == status)
    {
        m_uart_write_cb = NULL;

        return true;
    }

    return false;
}

// end of file
