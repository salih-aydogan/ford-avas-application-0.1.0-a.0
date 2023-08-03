// PRQA S 3108 EOF # Suppressed, Comments with links are required for ensure traceability.
// PRQA S 380 EOF # Suppressed, Too many macros in the driver provided by NXP causes this error.
/**
 * @file bsp_spi.c
 * @author Berke Yalcinkaya (berke.yalcinkaya@daiichi.com)
 * @brief Implementation of spi component.
 * @version 0.1
 * @date 2023-02-08
 * 
 * @copyright Copyright (c) 2023
 *
 * https://codebeamer.daiichi.com/issue/177747
 * 
 */

#include "bsp_spi.h"

#include "lpspi_master_driver.h"
#include "pins_driver.h"

#include "log.h"

LOG_MODULE_NAME("BSP_SPI");

/*******************************************************************************
 * Macros
 ******************************************************************************/

/**
 * @brief This macro specifies the timeout values used in blocking transmission.
 * 
 * https://codebeamer.daiichi.com/issue/183175
 * 
 */
#define BLOCKING_TIMEOUT_MS         (100U)

/**
 * @brief This macro specifies which LPSPI unit to use.
 * 
 * https://codebeamer.daiichi.com/issue/183180
 * 
 */
#define LPSPI_INSTANCE              (0U)

/**
 * @brief This macro specifies SPI pin count.
 * 
 * https://codebeamer.daiichi.com/issue/183181
 * 
 */
#define PIN_COUNT                   (3U)

/*******************************************************************************
 * Private Variables 
 ******************************************************************************/

/**
 * @brief This variable create from runtime state structure of LPSPI driver.
 * 
 * https://codebeamer.daiichi.com/issue/183184
 * 
 */
static lpspi_state_t m_lpspi_state;

/**
 * @brief This variable is used to indicate whether the initialize operation was 
 *        successfull or not.
 * 
 * https://codebeamer.daiichi.com/issue/183185
 * 
 */
static bool m_spi_init;

/**
 * @brief This variable is an function pointer.
 * 
 * https://codebeamer.daiichi.com/issue/183197
 * 
 */
static bsp_spi_err_cb_t m_spi_flash_cb;

/*******************************************************************************
 * Private Function Declarations
 ******************************************************************************/

/**
 * @brief This function is an initialization function that is used in SPI pins.
 * 
 * https://codebeamer.daiichi.com/issue/183202
 * 
 */
static bool gpio_init(void);

/**
 * @brief This function is an initialization function that is used in flexio 
 *        device and flexio spi.
 * 
 * https://codebeamer.daiichi.com/issue/183203
 * 
 */
static bool spi_driver_init(void);

/**
 * @brief This function is an initialization function that is used in FLEXIO, 
 *        FLEXIO SPI, LPSPI and GPIO initialization.
 * 
 * https://codebeamer.daiichi.com/issue/183189
 * 
 */
static bool spi_init(void);

/**
 * @brief This function handles the callback functions of the lpspi.
 *        Receive operation is non blocking and it handles with 
 *        interrupt, also transmit operation is blocking mode.
 * 
 * https://codebeamer.daiichi.com/issue/183186
 * 
 */
static void spi_flash_cb(void *driverState, 
                         spi_event_t event, 
                         void *userData);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/183220
 * 
 */
bool bsp_spi_init(void)
{
    bool ret = spi_init();

    if (false == ret)
    {
        LOG_ERROR("Init Fail");

        return false;
    }

    LOG_INFO("Initalized");
    
    return ret;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/183187
 * 
 */
bool bsp_spi_deinit(void)
{
    if (false == m_spi_init)
    {
        LOG_ERROR("Deinit Fail");

        return false;
    }

    status_t status = LPSPI_DRV_MasterDeinit((uint32_t)LPSPI_INSTANCE);

    if (STATUS_SUCCESS == status)
    {
        m_spi_init = false;

        m_spi_flash_cb = NULL;
    }
    else
    {
        LOG_ERROR("Deinit Fail");

        return false;
    }        

    LOG_INFO("Deinitalized");

    return (STATUS_SUCCESS == status);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/183188
 * 
 */
bool bsp_spi_write(const uint8_t *p_data, size_t size)
{
    if (NULL == p_data)
    {
        LOG_ERROR("Invalid argument in bsp_spi_write");

        return false;
    }

    if (0U == size)
    {
        LOG_ERROR("Zero size in bsp_spi_write");
        
        return false;
    }

    status_t status = LPSPI_DRV_MasterTransferBlocking((uint32_t)LPSPI_INSTANCE,
                                                       p_data,
                                                       NULL,
                                                       (uint16_t)size,
                                                       BLOCKING_TIMEOUT_MS);

    LOG_TX(p_data, size);

    return (STATUS_SUCCESS == status);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/197547
 * 
 */
bool bsp_spi_async_write(const uint8_t *p_data, size_t size)
{
    if (NULL == p_data)
    {
        LOG_ERROR("Invalid argument in bsp_spi_async_write");

        return false;
    }

    if (0U == size)
    {
        LOG_ERROR("Zero size in bsp_spi_async_write");

        return false;
    }

    status_t status = LPSPI_DRV_MasterGetTransferStatus(LPSPI_INSTANCE,
                                                        NULL);
    
    if (STATUS_SUCCESS == status)
    {
        status = LPSPI_DRV_MasterTransfer((uint32_t)LPSPI_INSTANCE,
                                                    p_data,
                                                    NULL,
                                                    (uint16_t)size);
    }

    LOG_TX(p_data, size);

    return (STATUS_SUCCESS == status);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/197548
 * 
 */
bool bsp_spi_is_busy(void)
{
    status_t status = LPSPI_DRV_MasterGetTransferStatus(LPSPI_INSTANCE, NULL);

    return (STATUS_BUSY == status);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/183192
 * 
 */
bool bsp_spi_read(uint8_t *p_data, size_t size)
{
    if (NULL == p_data)
    {
        LOG_ERROR("Invalid argument in bsp_spi_read");
        
        return false;
    }

    if (0U == size)
    {
        LOG_ERROR("Zero size in bsp_spi_read");
        
        return false;
    }

    status_t status = LPSPI_DRV_MasterTransferBlocking((uint32_t)LPSPI_INSTANCE,
                                                       NULL,
                                                       p_data,
                                                       (uint16_t)size,
                                                       BLOCKING_TIMEOUT_MS);

    LOG_RX(p_data, size);

    return (STATUS_SUCCESS == status);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/183190
 * 
 */
bool bsp_spi_async_read(uint8_t *p_data, size_t size)
{
    if (NULL == p_data)
    {
        LOG_ERROR("Invalid argument in bsp_spi_async_read");

        return false;
    }

    if (0U == size)
    {
        LOG_ERROR("Zero size in bsp_spi_async_read");

        return false;
    }

    status_t status = LPSPI_DRV_MasterGetTransferStatus(LPSPI_INSTANCE,
                                                        NULL);
    
    if (STATUS_SUCCESS == status)
    {
        status = LPSPI_DRV_MasterTransfer(LPSPI_INSTANCE,
                                          NULL,
                                          p_data,
                                          (uint16_t)size);
    }

    LOG_RX(p_data, size);
    
    return (STATUS_SUCCESS == status);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/183192
 * 
 */
bool bsp_spi_cb_set(bsp_spi_err_cb_t callback)
{
    if (NULL == callback)
    {
        LOG_ERROR("Invalid argument in bsp_spi_cb_set");
        
        return false;
    }

    if (false == m_spi_init)
    {
        LOG_ERROR("Invalid argument in bsp_spi_cb_set");
        
        return false;
    }

    m_spi_flash_cb = callback;

    return true;
}

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/183202
 * 
 */
static bool gpio_init(void)
{
    const pin_settings_config_t gpio_user_cfg_tbl[PIN_COUNT] = {

        {
            // PRQA S 306 ++ # Defined by nxp library to hold register base address. False positive warning.
            .base = PORTB,
            .pinPortIdx = 4U,
            .pullConfig = PORT_INTERNAL_PULL_NOT_ENABLED,
            .driveSelect = PORT_LOW_DRIVE_STRENGTH,
            .passiveFilter = false,
            .mux = PORT_MUX_ALT3,
            .pinLock = false,
            .intConfig = PORT_DMA_INT_DISABLED,
            .clearIntFlag = false,
            .digitalFilter = false,
            .gpioBase = NULL,
        },
        {
            .base = PORTD,
            .pinPortIdx = 15U,
            .pullConfig = PORT_INTERNAL_PULL_NOT_ENABLED,
            .driveSelect = PORT_LOW_DRIVE_STRENGTH,
            .passiveFilter = false,
            .mux = PORT_MUX_ALT4,
            .pinLock = false,
            .intConfig = PORT_DMA_INT_DISABLED,
            .clearIntFlag = false,
            .digitalFilter = false,
            .gpioBase = NULL,
        },
        {
            .base = PORTD,
            .pinPortIdx = 16U,
            .pullConfig = PORT_INTERNAL_PULL_NOT_ENABLED,
            .driveSelect = PORT_LOW_DRIVE_STRENGTH,
            .passiveFilter = false,
            .mux = PORT_MUX_ALT4,
            .pinLock = false,
            .intConfig = PORT_DMA_INT_DISABLED,
            .clearIntFlag = false,
            .digitalFilter = false,
            .gpioBase = NULL,
        },
        // PRQA S 306 --
    };    

    status_t status = PINS_DRV_Init(PIN_COUNT, gpio_user_cfg_tbl);

    for (uint8_t cnt = 0; cnt < PIN_COUNT; cnt++)
    {
        LOG_DEBUG("Pin init; base:%u, pin:%u, dir:%u, pull:%u, status:%u", 
                  gpio_user_cfg_tbl[cnt].base, 
                  gpio_user_cfg_tbl[cnt].pinPortIdx,
                  gpio_user_cfg_tbl[cnt].direction,
                  gpio_user_cfg_tbl[cnt].pullConfig,
                  status);
    }

    return (STATUS_SUCCESS == status);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/183203
 * 
 */
static bool spi_driver_init(void)
{
    const lpspi_master_config_t lpspi_user_cfg = {

        .bitsPerSec = 4000000UL, 
        .whichPcs = LPSPI_PCS0,
        .pcsPolarity = LPSPI_ACTIVE_LOW,
        .isPcsContinuous = true,
        .bitcount = 8U,
        .lpspiSrcClk = 32000000UL,
        .clkPhase = LPSPI_CLOCK_PHASE_1ST_EDGE,
        .clkPolarity = LPSPI_SCK_ACTIVE_HIGH,
        .lsbFirst = false,
        .transferType = LPSPI_USING_INTERRUPTS,
        .rxDMAChannel = 0U,
        .txDMAChannel = 0U,
        .callback = spi_flash_cb,
        .callbackParam = NULL,
    };
    
    status_t status = LPSPI_DRV_MasterInit((uint32_t)LPSPI_INSTANCE,
                                           &m_lpspi_state,
                                           &lpspi_user_cfg);

    LOG_DEBUG("SPI Init; instance:%u, baud:%u, status:%u", 
              LPSPI_INSTANCE,
              lpspi_user_cfg.bitsPerSec,
              status);

    return (STATUS_SUCCESS == status);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/183189
 * 
 */
static bool spi_init(void)
{
    bool ret = gpio_init();

    if (true == ret)
    {
        ret = spi_driver_init();
    }

    m_spi_init = (true == ret);

    return ret;
}

// PRQA S 3673 ++ # No const arguments were used in the prototype defined in the nxp library.
/**
 * @brief https://codebeamer.daiichi.com/issue/183186
 *
 */
static void spi_flash_cb(void *driverState, 
                         spi_event_t event, 
                         void *userData)
{   
    (void)event;
    (void)userData;

    if (NULL == m_spi_flash_cb)
    {
        LOG_DEBUG("NULL callback variable");

        return;
    }

    const lpspi_state_t *lpspi_state = (const lpspi_state_t *)driverState;

    bsp_spi_err_t spi_err = BSP_SPI_ERR_NONE;

    if (LPSPI_RECEIVE_FAIL == lpspi_state->status)
    {
        spi_err = BSP_SPI_ERR_RX_OVERRUN;

        LOG_ERROR("SPI RX Overrun Error");
    }

    if (LPSPI_TRANSMIT_FAIL == lpspi_state->status)
    {
        spi_err = BSP_SPI_ERR_TX_UNDERRUN;

        LOG_ERROR("SPI TX Underrun Error");
    }

    m_spi_flash_cb(spi_err);
}
// PRQA S 3673 --

// end of file
