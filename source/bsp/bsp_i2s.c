// PRQA S 3108 EOF # Comments with links are required for ensure traceability.
// PRQA S 380 EOF # Suppressed, Too many macros in the driver provided by NXP causes this error.
/**
 * @file bsp_i2s.c
 * @author Burak Utku Cecen (utku.cecen@daiichi.com)
 * @brief Implementation of the I2S component.
 * @version 0.1
 * @date 2023-04-12
 *
 * @copyright Copyright (c) 2023
 *
 * https://codebeamer.daiichi.com/issue/188962
 *
 */

#include "bsp_i2s.h"

#include "flexio_common.h"
#include "flexio_i2s_driver.h"
#include "edma_driver.h"
#include "pins_driver.h"

#include "log.h"

LOG_MODULE_NAME("BSP_I2S");

/*******************************************************************************
 * Macros
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/189282
 *
 */
#define I2S_INSTANCE 0U

/**
 * @brief https://codebeamer.daiichi.com/issue/189284
 *
 */
#define I2S_PINS_COUNT 4U

/**
 * @brief https://codebeamer.daiichi.com/issue/189287
 *
 */
#define EDMA_CH_1 1U

/**
 * @brief https://codebeamer.daiichi.com/issue/189289
 *
 */
#define EDMA_CH_2 2U

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/189290
 *
 */
static bsp_i2s_cb_t m_i2s_callback;

/**
 * @brief https://codebeamer.daiichi.com/issue/189293
 *
 */
static flexio_i2s_master_state_t m_i2s_master_state = {0U};

/**
 * @brief https://codebeamer.daiichi.com/issue/189295
 *
 */
static volatile bool m_i2s_write = false;

/**
 * @brief https://codebeamer.daiichi.com/issue/189300
 *
 */
static edma_chn_state_t m_dma_controller_ch0_state;

/**
 * @brief https://codebeamer.daiichi.com/issue/189301
 *
 */
static edma_chn_state_t m_dma_controller_ch1_state;

/*******************************************************************************
 * Private Function Declarations
 ******************************************************************************/

/**
 * @brief Initializes I2S driver.
 *
 * https://codebeamer.daiichi.com/issue/189040
 *
 * @return true   Initialization of I2S driver success.
 * @return false  Initialization of I2S driver failed.
 */
static bool i2s_init_cfg(void);

/**
 * @brief Initializes PIN driver.
 *
 * https://codebeamer.daiichi.com/issue/189045
 *
 * @return true   Initialization of PIN driver success.
 * @return false  Initialization of PIN driver failed.
 */
static bool i2s_pin_cfg(void);

/**
 * @brief Initializes eDMA driver.
 *
 * https://codebeamer.daiichi.com/issue/189041
 *
 * @return true   Initialization of eDMA driver success.
 * @return false  Initialization of eDMA driver failed.
 */
static bool i2s_edma_cfg(void);

/**
 * @brief I2S event callback.
 *
 * https://codebeamer.daiichi.com/issue/189044
 *
 * @param event Type of I2S event that occured.
 *
 * @param userData A pointer to user data.
 */
static void i2s_event_cb(i2s_event_t event, void *userData);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/189042
 *
 */
bool bsp_i2s_init(void)
{
    bool ret = i2s_pin_cfg();

    if (true != ret)
    {
        LOG_ERROR("Init Fail");

        return false;
    }

    ret = i2s_edma_cfg();

    if (true != ret)
    {
        LOG_ERROR("Init Fail");

        return false;
    }

    flexio_device_state_t i2s_device_init_cfg = {0U};

    status_t ret_val = FLEXIO_DRV_InitDevice(I2S_INSTANCE,
                                             &i2s_device_init_cfg);

    if (STATUS_SUCCESS != ret_val)
    {
        LOG_ERROR("Init Fail");

        return false;
    }

    ret = i2s_init_cfg();

    if (true != ret)
    {
        LOG_ERROR("Init Fail");

        return false;
    }

    LOG_INFO("Initialized");

    return ret;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/189043
 *
 */
bool bsp_i2s_deinit(void)
{
    status_t ret = FLEXIO_I2S_DRV_MasterDeinit(&m_i2s_master_state);

    if (STATUS_SUCCESS != ret)
    {
        LOG_ERROR("Deinit Fail");

        return false;
    }
    
    ret = FLEXIO_DRV_DeinitDevice(I2S_INSTANCE);

    if (STATUS_SUCCESS != ret)
    {    
        LOG_ERROR("Deinit Fail");

        return false;
    }

    ret = EDMA_DRV_ReleaseChannel(EDMA_CH_1);

    if (STATUS_SUCCESS != ret)
    {
        LOG_ERROR("DMA CH-1 Deinit Fail");
        
        return false;
    }

    ret = EDMA_DRV_ReleaseChannel(EDMA_CH_2);

    if (STATUS_SUCCESS != ret)
    {
        LOG_ERROR("DMA CH-2 Deinit Fail");
        
        return false;
    }

    m_i2s_callback = NULL;

    m_i2s_write = false;

    LOG_INFO("Denitialized");

    return true;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/189038
 *
 */
bool bsp_i2s_write(const uint8_t *data, size_t size)
{
    if (NULL == data)
    {
        LOG_ERROR("Invalid argument in bsp_i2s_write");

        return false;
    }

    if (0U == size)
    {
        LOG_ERROR("Zero size in bsp_i2s_write");
        
        return false;
    }

    status_t ret = STATUS_ERROR;

    if (false == m_i2s_write)
    {
        ret = FLEXIO_I2S_DRV_MasterSendData(&m_i2s_master_state, data, size);
        
        if (STATUS_SUCCESS != ret)
        {
            LOG_ERROR("I2S Send new data Error");

            return false;
        }
    }
    else
    {
        ret = FLEXIO_I2S_DRV_MasterSetTxBuffer(&m_i2s_master_state, data, size);

        if (STATUS_SUCCESS != ret)
        {
            LOG_ERROR("I2S Set TX Buffer Error");

            return false;
        }
    }

    LOG_TX(data, size);

    return true;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/189039
 *
 */
bool bsp_i2s_cb_set(bsp_i2s_cb_t callback)
{
    if (NULL == callback)
    {
        LOG_ERROR("Invalid argument in bsp_i2s_cb_set");

        return false;
    }

    m_i2s_callback = callback;

    return true;
}

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/189040
 *
 */
static bool i2s_init_cfg(void)
{
    const flexio_i2s_master_user_config_t i2s_master_cfg =
    {
        .baudRate = 1024000,
        .bitsWidth = 32U,
        .callback = i2s_event_cb,
        .callbackParam = NULL,
        .driverType = FLEXIO_DRIVER_TYPE_DMA,
        .rxDMAChannel = EDMA_CH_2,
        .txDMAChannel = EDMA_CH_1,
        .rxPin = 0U,
        .sckPin = 1U,
        .txPin = 5U,
        .wsPin = 4U,
    };

    status_t ret = FLEXIO_I2S_DRV_MasterInit(I2S_INSTANCE,
                                             &i2s_master_cfg,
                                             &m_i2s_master_state);

    LOG_DEBUG("I2S Init; instance:%u, baud:%u, bitswidth:%u, status:%u", 
              I2S_INSTANCE,
              i2s_master_cfg.baudRate,
              i2s_master_cfg.bitsWidth,
              ret);

    return (STATUS_SUCCESS == ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/189045
 *
 */
static bool i2s_pin_cfg(void)
{
    const pin_settings_config_t pin_mux_init_cfg[I2S_PINS_COUNT] = 
    {
        {
            // PRQA S 306 ++ # Defined by nxp library to hold register base address. False positive warning.
            .base            = PORTD,
            // PRQA S 306 --
            .pinPortIdx      = 0U,
            .pullConfig      = PORT_INTERNAL_PULL_NOT_ENABLED,
            .driveSelect     = PORT_LOW_DRIVE_STRENGTH,
            .passiveFilter   = false,
            .mux             = PORT_MUX_ALT6,
            .pinLock         = false,
            .intConfig       = PORT_DMA_INT_DISABLED,
            .clearIntFlag    = false,
            .gpioBase        = NULL,
            .digitalFilter   = false,
        },
        {
            // PRQA S 306 ++ # Defined by nxp library to hold register base address. False positive warning.
            .base            = PORTD,
            // PRQA S 306 --
            .pinPortIdx      = 1U,
            .pullConfig      = PORT_INTERNAL_PULL_NOT_ENABLED,
            .driveSelect     = PORT_LOW_DRIVE_STRENGTH,
            .passiveFilter   = false,
            .mux             = PORT_MUX_ALT6,
            .pinLock         = false,
            .intConfig       = PORT_DMA_INT_DISABLED,
            .clearIntFlag    = false,
            .gpioBase        = NULL,
            .digitalFilter   = false,
        },
        {
            // PRQA S 306 ++ # Defined by nxp library to hold register base address. False positive warning.
            .base            = PORTD,
            // PRQA S 306 --
            .pinPortIdx      = 3U,
            .pullConfig      = PORT_INTERNAL_PULL_NOT_ENABLED,
            .driveSelect     = PORT_LOW_DRIVE_STRENGTH,
            .passiveFilter   = false,
            .mux             = PORT_MUX_ALT4,
            .pinLock         = false,
            .intConfig       = PORT_DMA_INT_DISABLED,   
            .clearIntFlag    = false,
            .gpioBase        = NULL,
            .digitalFilter   = false,
        },
        {
            // PRQA S 306 ++ # Defined by nxp library to hold register base address. False positive warning.
            .base            = PORTD,
            // PRQA S 306 --
            .pinPortIdx      = 2U,
            .pullConfig      = PORT_INTERNAL_PULL_NOT_ENABLED,
            .driveSelect     = PORT_LOW_DRIVE_STRENGTH,
            .passiveFilter   = false,
            .mux             = PORT_MUX_ALT4,
            .pinLock         = false,
            .intConfig       = PORT_DMA_INT_DISABLED,
            .clearIntFlag    = false,
            .gpioBase        = NULL,
            .digitalFilter   = false,
        }    
    };
    
    status_t status_ret = PINS_DRV_Init(I2S_PINS_COUNT, pin_mux_init_cfg);

    for (uint8_t cnt = 0; cnt < I2S_PINS_COUNT; cnt++)
    {
        LOG_DEBUG("Pin init; base:%u, pin:%u, dir:%u, pull:%u, status:%u", 
                  pin_mux_init_cfg[cnt].base, 
                  pin_mux_init_cfg[cnt].pinPortIdx,
                  pin_mux_init_cfg[cnt].direction,
                  pin_mux_init_cfg[cnt].pullConfig,
                  status_ret);
    }

    return (STATUS_SUCCESS == status_ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/189041
 *
 */
static bool i2s_edma_cfg(void)
{    
    const edma_channel_config_t dma_controller_ch0_cfg =
    {
        .channelPriority = EDMA_CHN_PRIORITY_3,
        .virtChnConfig = EDMA_CH_1,
        .source = EDMA_REQ_DISABLED,
        .callback = NULL,
        .callbackParam = NULL,
        // PRQA S 2983 ++ # Suppressed, It is used within the nxp library. False positive warning. 
        .enableTrigger = false,
        // PRQA S 2983 --
    };

    const edma_channel_config_t dma_controller_ch1_cfg = 
    {
        .channelPriority = EDMA_CHN_PRIORITY_3,
        .virtChnConfig = EDMA_CH_2,
        .source = EDMA_REQ_DISABLED,
        .callback = NULL,
        .callbackParam = NULL,
        // PRQA S 2983 ++ # Suppressed, It is used within the nxp library. False positive warning. 
        .enableTrigger = false,
        // PRQA S 2983 --
    };

    status_t ret = EDMA_DRV_ChannelInit(&m_dma_controller_ch0_state, 
                                        &dma_controller_ch0_cfg);

    if (STATUS_SUCCESS == ret)
    {
        ret = EDMA_DRV_ChannelInit(&m_dma_controller_ch1_state, 
                                   &dma_controller_ch1_cfg);
    }

    if (STATUS_SUCCESS != ret)
    {
        return false;
    }

    return true;
}

//PRQA S 3673 ++ # No const arguments were used in the prototype defined in the nxp library.
/**
 * @brief https://codebeamer.daiichi.com/issue/189044
 *
 */
static void i2s_event_cb(i2s_event_t event, void *userData)
{
    (void)userData;

    if (I2S_EVENT_RX_FULL == event)
    {
        (*m_i2s_callback)(BSP_I2S_RX_FULL);

        LOG_VERBOSE("BSP_I2S_RX_FULL");
    }

    if (I2S_EVENT_TX_EMPTY == event)
    {
        m_i2s_write = true;

        (*m_i2s_callback)(BSP_I2S_TX_EMPTY);

        

        LOG_VERBOSE("BSP_I2S_TX_EMPTY");
    }

    if (I2S_EVENT_END_TRANSFER == event)
    {
        (*m_i2s_callback)(BSP_I2S_END_TRANSFER);

        LOG_VERBOSE("BSP_I2S_END_TRANSFER");
    }

    if (I2S_EVENT_ERROR == event)
    {
        (*m_i2s_callback)(BSP_I2S_ERROR);

        LOG_ERROR("BSP_I2S_ERROR");
    }
}
//PRQA S 3673 -- 

/*end of line*/
