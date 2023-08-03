// PRQA S 3108 EOF # Comments with links are required for ensure traceability.

/**
 * @file bsp_i2c.c
 * @author Furkan Yildirim (furkan.yildirim@daiichi.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-23
 * 
 * @copyright Copyright (c) 2023
 * 
 * https://codebeamer.daiichi.com/issue/180816
 * 
 */

#include "bsp_i2c.h"

#include "lpi2c_driver.h"
#include "pins_driver.h"

#include "log.h"

LOG_MODULE_NAME("BSP_I2C");

/*******************************************************************************
 * Macros
 ******************************************************************************/

#define POLLING 1U

// PRQA S 380 ++ # Too many macros in the driver provided by NXP causes this error.

/**
 * @brief https://codebeamer.daiichi.com/issue/193908
 * 
 */
#define PORT1_ADDR           0x66U

/**
 * @brief https://codebeamer.daiichi.com/issue/193909
 * 
 */
#define PORT2_ADDR           0x67U

/**
 * @brief https://codebeamer.daiichi.com/issue/181101
 * 
 */
#define LPI2C_INST          0U

/**
 * @brief https://codebeamer.daiichi.com/issue/182642
 * 
 */
#define I2C_PORT_COUNT      2U

/**
 * @brief https://codebeamer.daiichi.com/issue/182643
 * 
 */
#define INIT_PINS_COUNT     2U

// PRQA S 380 --

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/181111
 * 
 */
static bsp_i2c_err_cb_t m_error_cb[I2C_PORT_COUNT];

/**
 * @brief https://codebeamer.daiichi.com/issue/193911
 * 
 */
static lpi2c_master_state_t m_lpi2c_master_state;

/**
 * @brief https://codebeamer.daiichi.com/issue/194149
 * 
 */
static uint32_t m_current_amp;

/*******************************************************************************
 * Private Function Declarations
 ******************************************************************************/

/**
 * @brief This function matches error numbers of type status_t to bsp_i2c_err_t. 
 *        After converting the error numbers that occur during communication to 
 *        bsp_i2c_err_t type, it transmits them to the upper layer.
 * 
 * https://codebeamer.daiichi.com/issue/182851
 * 
 * @param[in] port 
 * @param[in] status_ret 
 * 
 */
static void i2c_error_cb(uint32_t port, status_t status_ret);

/**
 * @brief This function provides pins of I2C ports configuration and pins init.
 * 
 * https://codebeamer.daiichi.com/issue/182695
 * 
 * @return bool Success or fail.
 * 
 * @return true Successful.
 * @return false Possible errors:
 *               -Pins not initialized properly
 * 
 */
static bool pin_cfg_set(void);

/**
 * @brief The purpose of this wrapper function is to wrap the 
 *        LPI2C_DRV_MasterSetSlaveAddr function. It should not be used with any 
 *        argument other than PORT1_ADDR and PORT2_ADDR macros.
 * 
 * https://codebeamer.daiichi.com/issue/193942
 * 
 * @param[in] addr 
 * 
 */
static void i2c_addr_set(uint32_t addr);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/180817
 * 
 */
bool bsp_i2c_init(void)
{
    bool pin_init_ret = pin_cfg_set();

    if (false == pin_init_ret)
    {
        return false;
    }

    const uint32_t m_baudrate_value = 400000UL;

    status_t status_ret = LPI2C_DRV_MasterInit(
                              LPI2C_INST,
                              &(const lpi2c_master_user_config_t)
                              {
                                  .slaveAddress = PORT1_ADDR,
                                  .is10bitAddr = false,
                                  .operatingMode = LPI2C_FAST_MODE,
                                  .baudRate = m_baudrate_value,
                                  .transferType = LPI2C_USING_INTERRUPTS,
                                  .dmaChannel = 0U,
                                  .masterCallback = NULL,
                                  .callbackParam = NULL
                              }, 
                              &m_lpi2c_master_state);

    bool ret = (STATUS_SUCCESS == status_ret);

    LOG_DEBUG("I2C Init; instance:%u, baud:%u, status:%u", 
              LPI2C_INST,
              m_baudrate_value,
              ret);

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
 * @brief https://codebeamer.daiichi.com/issue/181849
 * 
 */
bool bsp_i2c_deinit(void)
{    
    status_t status_ret = LPI2C_DRV_MasterDeinit(LPI2C_INST);

    LOG_DEBUG("I2C DeInit; instance:%u, status:%u", 
              LPI2C_INST,
              status_ret);

    if (STATUS_SUCCESS == status_ret)
    {
        for (uint32_t idx = 0; idx < I2C_PORT_COUNT; ++idx)
        {
            m_error_cb[idx] = NULL;
        }

        LOG_INFO("Deinitialized");

        return true;
    }
    else
    {
        LOG_ERROR("Deinit Fail");
    }

    m_current_amp = 0U;

    return false;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/181846
 * 
 */
bool bsp_i2c_write(uint32_t port, const uint8_t* p_data, size_t size)
{
    if (NULL == p_data)
    {
        LOG_ERROR("Invalid argument in bsp_i2c_write");

        return false;
    }

    if (0U == size)
    {
        LOG_ERROR("Zero size in bsp_i2c_write");

        return false;
    }

    while(true != m_lpi2c_master_state.i2cIdle){};

    if (port != m_current_amp)
    {
        i2c_addr_set(port);

        m_current_amp = port;  
    }


    #if POLLING

    status_t status_ret = LPI2C_DRV_MasterSendDataBlocking(LPI2C_INST,
                                                           p_data,
                                                           size,
                                                           true,
                                                           OSIF_WAIT_FOREVER);

    #else

    status_t status_ret = LPI2C_DRV_MasterSendData(LPI2C_INST,
                                                   p_data,
                                                   size,
                                                   true);

    #endif
    LOG_TX(p_data, size); 

    i2c_error_cb(port, m_lpi2c_master_state.status);

    return (STATUS_SUCCESS == status_ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/181848
 * 
 */
bool bsp_i2c_read(uint32_t port, uint8_t* p_data, size_t size)
{
    if (NULL == p_data)
    {
        LOG_ERROR("Invalid argument in bsp_i2c_read");

        return false;
    }

    if (0U == size)
    {
        LOG_ERROR("Zero size in bsp_i2c_read");

        return false;
    }

    while(true != m_lpi2c_master_state.i2cIdle){};

    if (port != m_current_amp)
    {
        i2c_addr_set(port);

        m_current_amp = port;  
    }

    #if POLLING

    status_t status_ret = LPI2C_DRV_MasterReceiveDataBlocking(LPI2C_INST,
                                                              p_data,
                                                              size,
                                                              true,
                                                              OSIF_WAIT_FOREVER);

    #else

    status_t status_ret = LPI2C_DRV_MasterReceiveData(LPI2C_INST,
                                                      p_data,
                                                      size,
                                                      true);

    #endif
    LOG_RX(p_data, size); 

    i2c_error_cb(port, m_lpi2c_master_state.status);

    return (STATUS_SUCCESS == status_ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/181850
 * 
 */
bool bsp_i2c_write_read(uint32_t port, 
                        uint8_t* p_rx_data,
                        size_t rx_size, 
                        const uint8_t* p_tx_data,
                        size_t tx_size)
{
    if (NULL == p_rx_data)
    {
        LOG_ERROR("Invalid argument in bsp_i2c_write_read");

        return false;
    }

    if (0U == rx_size)
    {
        LOG_ERROR("Zero size in bsp_i2c_write_read");

        return false;
    }   

    if (NULL == p_tx_data)
    {
        LOG_ERROR("Invalid argument in bsp_i2c_write_read");

        return false;
    }

    if (0U == tx_size)
    {
        LOG_ERROR("Invalid argument in bsp_i2c_write_read");

        return false;
    }    

    while(true != m_lpi2c_master_state.i2cIdle){};

    if (port != m_current_amp)
    {
        i2c_addr_set(port);

        m_current_amp = port;
    }

    #if POLLING

    status_t status_ret = LPI2C_DRV_MasterSendDataBlocking(LPI2C_INST, 
                                                           p_tx_data,
                                                           tx_size,
                                                           false,
                                                           OSIF_WAIT_FOREVER);

    #else

    status_t status_ret = LPI2C_DRV_MasterSendData(LPI2C_INST, 
                                                   p_tx_data,
                                                   tx_size,
                                                   false);

    #endif

    if (STATUS_SUCCESS == status_ret)
    {
        while(true != m_lpi2c_master_state.i2cIdle){};

        #if POLLING

        status_ret = LPI2C_DRV_MasterReceiveDataBlocking(LPI2C_INST,
                                                         p_rx_data, 
                                                         rx_size, 
                                                         true, 
                                                         OSIF_WAIT_FOREVER);

        #else

        status_ret = LPI2C_DRV_MasterReceiveData(LPI2C_INST,
                                                 p_rx_data, 
                                                 rx_size, 
                                                 true);

        #endif                                         
    }

    LOG_TX(p_tx_data, tx_size);

    LOG_RX(p_rx_data, rx_size);

    i2c_error_cb(port, m_lpi2c_master_state.status);

    return (STATUS_SUCCESS == status_ret);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/181847
 * 
 */
void bsp_i2c_cb_set(uint32_t port, bsp_i2c_err_cb_t callback)
{
    if ((BSP_I2C_AMP_1_PORT != port) && (BSP_I2C_AMP_2_PORT != port))
    {
        LOG_ERROR("Invalid argument in bsp_i2c_cb_set");

        return;
    }

    if (NULL == callback)
    {
        LOG_ERROR("Invalid argument in bsp_i2c_cb_set");

        return;
    }

    m_error_cb[port] = callback;
}

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

static void i2c_error_cb(uint32_t port, status_t status_ret)
{
    bsp_i2c_err_t error_type = BSP_I2C_STATUS_ERR_GENERIC;    

    if (STATUS_SUCCESS != status_ret)
    {
        return;
    }

    if (STATUS_I2C_RECEIVED_NACK == status_ret)
    {
        error_type = BSP_I2C_STATUS_ERR_RECEIVED_NACK;

        LOG_ERROR("I2C Received NACK Error");
    }

    if (STATUS_I2C_TX_UNDERRUN == status_ret)
    {
        error_type = BSP_I2C_STATUS_ERR_TX_UNDERRUN;

        LOG_ERROR("I2C TX Underrun Error");
    }

    if (STATUS_I2C_RX_OVERRUN == status_ret)
    {
        error_type = BSP_I2C_STATUS_ERR_RX_OVERRUN;

        LOG_ERROR("I2C RX Underrun Error");
    }

    if (STATUS_I2C_ARBITRATION_LOST == status_ret)
    {
        error_type = BSP_I2C_STATUS_ERR_ARBITRATION_LOST;

        LOG_ERROR("I2C Arbitration Lost Error");
    }

    if (STATUS_I2C_ABORTED == status_ret)
    {
        error_type = BSP_I2C_STATUS_ERR_ABORTED;

        LOG_ERROR("I2C Aborted Error");
    }

    if (STATUS_I2C_BUS_BUSY == status_ret)
    {
        error_type = BSP_I2C_STATUS_ERR_BUS_BUSY;

        LOG_ERROR("I2C Busy Error");
    }  

    if (NULL != m_error_cb[port])
    {
        m_error_cb[(port)](error_type);
    }    
    else
    {
        LOG_DEBUG("NULL callback variable");
    }
}

static bool pin_cfg_set(void)
{
    const pin_settings_config_t pin_mux_init_config[] = 
    {
        {
            // PRQA S 306 ++ # Defined by nxp library to hold register base address. False positive warning.
            .base            = PORTA,
            // PRQA S 306 --
            .pinPortIdx      = 2U,
            .pullConfig      = PORT_INTERNAL_PULL_NOT_ENABLED,
            .driveSelect     = PORT_LOW_DRIVE_STRENGTH,
            .passiveFilter   = false,
            .mux             = PORT_MUX_ALT3,
            .pinLock         = false,
            .intConfig       = PORT_DMA_INT_DISABLED,
            .clearIntFlag    = false,
            .gpioBase        = NULL,
            .digitalFilter   = false,
        },
        {
            // PRQA S 306 ++ # Defined by nxp library to hold register base address. False positive warning.
            .base            = PORTA,
            // PRQA S 306 --
            .pinPortIdx      = 3U,
            .pullConfig      = PORT_INTERNAL_PULL_NOT_ENABLED,
            .driveSelect     = PORT_LOW_DRIVE_STRENGTH,
            .passiveFilter   = false,
            .mux             = PORT_MUX_ALT3,
            .pinLock         = false,
            .intConfig       = PORT_DMA_INT_DISABLED,
            .clearIntFlag    = false,
            .gpioBase        = NULL,
            .digitalFilter   = false,
        }    
    };
    
    status_t status_ret = PINS_DRV_Init(INIT_PINS_COUNT, pin_mux_init_config);

    for (uint8_t cnt = 0; cnt < INIT_PINS_COUNT; cnt++)
    {
        LOG_DEBUG("Pin init; base:%u, pin:%u, dir:%u, pull:%u, status:%u", 
                  pin_mux_init_config[cnt].base, 
                  pin_mux_init_config[cnt].pinPortIdx,
                  pin_mux_init_config[cnt].direction,
                  pin_mux_init_config[cnt].pullConfig,
                  status_ret);
    }

    return (STATUS_SUCCESS == status_ret);
}

static void i2c_addr_set(uint32_t addr)
{
    if ((BSP_I2C_AMP_1_PORT != addr) && (BSP_I2C_AMP_2_PORT != addr))
    {
        LOG_DEBUG("Wrong I2C address %lu", addr);
    }

    const uint16_t i2c_addr = (BSP_I2C_AMP_1_PORT == addr) 
                              ? (const uint16_t)PORT1_ADDR 
                              : (const uint16_t)PORT2_ADDR;

    LPI2C_DRV_MasterSetSlaveAddr(LPI2C_INST, i2c_addr, false);

    if (BSP_I2C_AMP_1_PORT == addr)
    {
        LOG_VERBOSE("I2C addr set for Port 1; addr:%hu", i2c_addr);
    }
    else
    {
        LOG_VERBOSE("I2C addr set for Port 2; addr:%hu", i2c_addr);
    }
}

// end of file
