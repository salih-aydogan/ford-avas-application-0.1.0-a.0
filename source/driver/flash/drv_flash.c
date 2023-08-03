// PRQA S 5209 EOF # Suppressed, Basic int type must be used for third party integration
/**
 ******************************************************************************
 * @file    /LanciaBase/Drivers/Flash/flash.c
 * @author  Daiichi Arge Team
 * @version V1.0.0
 * @date    22/08/2017
 * @brief   Generic Flash Driver Source Code
 ******************************************************************************
 * @attention
 *     COPYRIGHT(c) 2017 Daiichi Arge
 ******************************************************************************
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "drv_flash.h"

#include "spiflash.h"
#include "log.h"

#include "bsp_spi.h"
#include "bsp_mcu.h"
#include "bsp_gpio.h"

#include <string.h>

#ifndef DRV_FLASH_CFG_SECTOR_SIZE
#error "Flash Driver not config properly"
#endif

LOG_MODULE_NAME("DRV_FLASH");

#define DRV_FLASH_PROCTECT_AFTER_INIT (0)

/*******************************************************************************
 * Private Function Declarations
 ******************************************************************************/
static void wait_for_flash_complete(void);

static int flash_spi_write_read(const spiflash_t *ph_flash,
                                const uint8_t *p_tx_data,
                                uint32_t tx_len,
                                uint8_t *p_rx_data,
                                uint32_t rx_len);

static void flash_spi_cs(const spiflash_t *ph_flash, uint8_t spi_cs);

static void flash_wait(const spiflash_t *ph_flash, uint32_t delay_ms);

static void flash_spi_cb(bsp_spi_err_t err);

static void flash_cmd_completed_cb(struct spiflash_s *spi,
                                   spiflash_op_t operation,
                                   int err_code);

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

static spiflash_t mh_flash;

static const spiflash_config_t m_spiflash_config =
    {
        .sz = DRV_FLASH_CFG_SECTOR_SIZE * DRV_FLASH_SECTOR_ID_MAX,
        .page_sz = 256U,    // normally 256 byte pages
        .addr_sz = 3U,      // normally 3 byte addressing
        .addr_dummy_sz = 0, // using single line data, not quad or something
        .addr_endian = SPIFLASH_ENDIANNESS_BIG,
        .sr_write_ms = 10U,
        .page_program_ms = 1U,
        .block_erase_4_ms = 70U,
        .block_erase_8_ms = 0,  // not supported
        .block_erase_16_ms = 0, // not supported
        .block_erase_32_ms = 100U,
        .block_erase_64_ms = 150U,
        .chip_erase_ms = 20000U,
};

static const spiflash_cmd_tbl_t m_spiflash_cmd_tbl =
{
    .write_disable = 0x04,
    .write_enable = 0x06,
    .page_program = 0x02,
    .read_data = 0x03,
    .read_data_fast = 0x0b,
    .write_sr = 0x01,
    .read_sr = 0x05,
    .block_erase_4 = 0x20,
    .block_erase_8 = 0x00,
    .block_erase_16 = 0x00,
    .block_erase_32 = 0x52,
    .block_erase_64 = 0xd8,
    .chip_erase = 0xc7,
    .device_id = 0x90,
    .jedec_id = 0x9f,
    .sr_busy_bit = 0x01,
};

static const spiflash_hal_t m_spiflash_hal =
    {
        ._spiflash_spi_txrx = flash_spi_write_read,
        ._spiflash_spi_cs = flash_spi_cs,
        ._spiflash_wait = flash_wait,
};

static drv_flash_cb_t m_flash_cb;

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

bool drv_flash_init(void)
{
    bsp_gpio_set(BSP_GPIO_SPI_CS_OUT_PIN_NO);

    SPIFLASH_init(&mh_flash,
                  &m_spiflash_config,
                  &m_spiflash_cmd_tbl,
                  &m_spiflash_hal,
                  flash_cmd_completed_cb,
                  SPIFLASH_ASYNCHRONOUS,
                  NULL);
    
    LOG_DEBUG("SPIFLASH init; sz:%u, page_sz:%u, addr_sz:%u",
              m_spiflash_config.sz,
              m_spiflash_config.page_sz,
              m_spiflash_config.addr_sz);

    m_flash_cb = NULL;

    (void)bsp_spi_cb_set(flash_spi_cb);

    uint32_t start_tick = bsp_mcu_tick_get();

    uint32_t end_tick = start_tick + DRV_FLASH_CFG_TIMEOUT_MS;

    bool loop_exit = false;

    while ((true != loop_exit) && (end_tick > start_tick))
    {
        uint8_t busy_status = 0U;

        (void)SPIFLASH_read_sr_busy(&mh_flash, &busy_status);

        wait_for_flash_complete();

        loop_exit = (busy_status != 1U);

        start_tick = bsp_mcu_tick_get();
    }

    if (true != loop_exit)
    {
        LOG_ERROR("Communication cannot be established");

        return false;
    }

    uint8_t jedec_data[4U] = {0};

    int ret = SPIFLASH_read_jedec_id(&mh_flash, (uint32_t *)jedec_data);

    wait_for_flash_complete();

    if (SPIFLASH_OK != ret)
    {
        LOG_ERROR("Jedec cannot be read");

        return false;
    }

    const uint8_t jedec_id[4U] = {

        DRV_FLASH_CFG_JEDEC_ID1,
        DRV_FLASH_CFG_JEDEC_ID2,
        DRV_FLASH_CFG_JEDEC_ID3,
        0U,
    };

    LOG_DEBUG("Read jedec:%u", (uint32_t *)jedec_data);
    LOG_DEBUG("Expected jedec:%u", (const uint32_t *)jedec_id);

    if (0 != memcmp(jedec_data, jedec_id, 2U))
    {
        LOG_ERROR("Jedec are not matched, Read jedec:%u, Expected jedec:%u",
                  (uint32_t *)jedec_data,
                  (const uint32_t *)jedec_id);

        return false;
    }

#if DRV_FLASH_PROCTECT_AFTER_INIT

    bool ret_protect = drv_flash_protect();

    wait_for_flash_complete();

    if (true != ret_protect)
    {
        LOG_ERROR("Cannot be made protected");

        return false;
    }

#endif

    LOG_INFO("Initialized");

    return true;
}

bool drv_flash_full_erase(void)
{
    LOG_INFO("Flash full chip erase");

    return (SPIFLASH_OK == SPIFLASH_chip_erase(&mh_flash));
}

bool drv_flash_sector_erase(uint32_t sector_id)
{
    if (sector_id >= DRV_FLASH_SECTOR_ID_MAX)
    {
        return false;
    }

    uint32_t addr = sector_id * DRV_FLASH_CFG_SECTOR_SIZE;

    LOG_INFO("Flash sector erase; sector_id:%u, address:%u", sector_id, addr);

    return (SPIFLASH_OK == SPIFLASH_erase(&mh_flash,
                                          addr,
                                          DRV_FLASH_CFG_SECTOR_SIZE));
}

bool drv_flash_protect(void)
{
    // Staus Registers: SRWD:QE:BP3:BP2:BP1:BP0:WEL:WIP
    uint8_t status_reg = (uint8_t)0x3CU; // 0011 1100

    return (SPIFLASH_OK == SPIFLASH_write_sr(&mh_flash, status_reg));
}

bool drv_flash_unprotect(void)
{
    // Staus Registers: SRWD:QE:BP3:BP2:BP1:BP0:WEL:WIP
    uint8_t status_reg = 0x00; // 0000 0000

    return (SPIFLASH_OK == SPIFLASH_write_sr(&mh_flash, status_reg));
}

bool drv_flash_addr_to_sector(uint32_t addr, uint32_t *p_sector_id)
{
    if (NULL == p_sector_id)
    {
        LOG_ERROR("Invalid argument in drv_flash_cb_set");

        return false;
    }

    *p_sector_id = (addr / DRV_FLASH_CFG_SECTOR_SIZE);

    return (*p_sector_id < DRV_FLASH_SECTOR_ID_MAX);
}

bool drv_flash_sector_to_addr(uint32_t sector_id, uint32_t *p_addr)
{
    if (NULL == p_addr)
    {
        LOG_ERROR("Invalid argument in drv_flash_cb_set");

        return false;
    }

    *p_addr = (uint32_t)(sector_id * DRV_FLASH_CFG_SECTOR_SIZE);

    return (sector_id < DRV_FLASH_SECTOR_ID_MAX);
}

bool drv_flash_write(uint32_t addr, const uint8_t *p_data, uint32_t size)
{
    if (NULL == p_data)
    {
        LOG_ERROR("Invalid argument in drv_flash_cb_set");

        return false;
    }

    return (SPIFLASH_OK == SPIFLASH_write(&mh_flash, addr, size, p_data));
}

bool drv_flash_read(uint32_t addr, uint8_t *p_data, uint32_t size)
{
    if (NULL == p_data)
    {
        LOG_ERROR("Invalid argument in drv_flash_cb_set");

        return false;
    }

    return (SPIFLASH_OK == SPIFLASH_read(&mh_flash, addr, size, p_data));
}

bool drv_flash_cb_set(drv_flash_cb_t callback)
{
    if (NULL != callback)
    {
        m_flash_cb = callback;

        return true;
    }

    LOG_ERROR("Invalid argument in drv_flash_cb_set");

    return false;
}

bool drv_flash_is_busy(void)
{
    return (SPIFLASH_ERR_BUSY == SPIFLASH_is_busy(&mh_flash));
}

void drv_flash_process(void)
{
    bool is_drv_flash_busy = drv_flash_is_busy();
  
    bool is_bsp_spi_busy = bsp_spi_is_busy();

    if (is_drv_flash_busy && !(is_bsp_spi_busy))
    {
        (void)SPIFLASH_async_trigger(&mh_flash, SPIFLASH_OK);
    }
}

/*******************************************************************************
 * Private Functions
 ******************************************************************************/
static void wait_for_flash_complete(void)
{
    uint32_t start_tick = bsp_mcu_tick_get();

    uint32_t end_tick = start_tick + DRV_FLASH_CFG_TIMEOUT_MS;

    bool loop_exit = false;

    while ((true != loop_exit) && (end_tick > start_tick))
    {
        drv_flash_process();

        loop_exit = (false == drv_flash_is_busy());

        start_tick = bsp_mcu_tick_get();
    }

    if (end_tick <= start_tick)
    {
        LOG_ERROR("Timeout occured in init wait: %d", end_tick);
    }
}

static int flash_spi_write_read(const spiflash_t *ph_flash,
                                const uint8_t *p_tx_data,
                                uint32_t tx_len,
                                uint8_t *p_rx_data,
                                uint32_t rx_len)
{
    (void)ph_flash;

    bool ret = false;

    if ((0U < tx_len) && (0U < rx_len))
    {
        // both transmit and receive

        ret = bsp_spi_write(p_tx_data, tx_len);

        if (true == ret)
        {
            ret = bsp_spi_async_read(p_rx_data, rx_len);
        }
    }

    if ((0U < tx_len) && (0U == rx_len))
    {
        // only transmit

        ret = bsp_spi_async_write(p_tx_data, tx_len);
    }

    if ((0U == tx_len) && (0U < rx_len))
    {
        // only receive

        ret = bsp_spi_async_read(p_rx_data, rx_len);
    }

    return ((true == ret) ? SPIFLASH_OK : (1));
}

static void flash_spi_cs(const spiflash_t *ph_flash, uint8_t spi_cs)
{
    (void)ph_flash;

    if (0U == spi_cs)
    {
        bsp_gpio_set(BSP_GPIO_SPI_CS_OUT_PIN_NO);
    }
    else
    {
        bsp_gpio_reset(BSP_GPIO_SPI_CS_OUT_PIN_NO);
    }
}

static void flash_wait(const spiflash_t *ph_flash, uint32_t delay_ms)
{
    (void)ph_flash;

    bsp_mcu_delay(delay_ms);

    (void)SPIFLASH_async_trigger(&mh_flash, SPIFLASH_OK);
}

static void flash_spi_cb(bsp_spi_err_t err)
{
    if (BSP_SPI_ERR_NONE != err)
    {
        LOG_ERROR("SPI error occured, error: %d", err);
    }
}

static void flash_cmd_completed_cb(struct spiflash_s *spi,
                                   spiflash_op_t operation,
                                   int err_code)
{
    (void)spi;
    (void)operation;

    bool error = false;

    if (SPIFLASH_OK != err_code)
    {
        error = true;

        LOG_ERROR("Cmd completed cb error occured, error: %d", err_code);
    }

    if (NULL != m_flash_cb)
    {
        m_flash_cb(error);
    }
}

// end of file
