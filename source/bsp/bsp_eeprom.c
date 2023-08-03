// PRQA S 3108 EOF # Comments with links are required for ensure traceability.
// PRQA S 380 EOF # Too many macros in the driver provided by NXP causes this error.
/**
 * @file bsp_eeprom.c
 * @author berke yalcinkaya (berke.yalcinkaya@daiichi.com)
 * @brief This module handles EEPROM operations and provides access to EEPROM HW
 * @version 0.1
 * @date 2023-04-12
 * 
 * @copyright Copyright (c) 2023
 *
 * https://codebeamer.daiichi.com/issue/188626
 */

#include "bsp_eeprom.h"

#include "flash_driver.h"

#include "log.h"

LOG_MODULE_NAME("BSP_EEPROM");

/*******************************************************************************
 * Macros
 ******************************************************************************/

/**
 * @brief This macro holds to FlexRAM base address for emulated EEPROM.
 * 
 * https://codebeamer.daiichi.com/issue/188636
 */
#define EEE_RAM_BASE        (0x14000000UL)

/**
 * @brief This macro holds to emulated EEPROM not ready status.
 * 
 * https://codebeamer.daiichi.com/issue/188638
 */
#define EEE_NOT_READY       (0U)

/*******************************************************************************
 * Private Function Declarations
 ******************************************************************************/

/**
 * @brief This function checks limits of FlexRAM.
 * 
 * @param addr The address of the data in the EEPROM.
 * @param size Data size.
 * 
 * @return bsp_eeprom_status_t 
 * 
 * https://codebeamer.daiichi.com/issue/188652
 */
static bsp_eeprom_status_t eeprom_limit_check(uint32_t addr,
                                              size_t size);

/**
 * @brief This function checks FlexRAM status for emulated EEPROM.
 * 
 * @return uint8_t 
 * 
 * https://codebeamer.daiichi.com/issue/188658
 */
static uint8_t eeprom_status_check(void);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/188651
 * 
 */
bsp_eeprom_status_t bsp_eeprom_read(uint32_t addr,
                                    uint8_t *p_data,
                                    size_t size)
{
    if (NULL == p_data)
    {
        LOG_ERROR("Invalid argument in bsp_eeprom_read");

        return BSP_EEPROM_STATUS_ERR_PARAM;
    }

    if (EEE_NOT_READY == eeprom_status_check())
    {
        return BSP_EEPROM_STATUS_NOT_RDY;
    }

    if (BSP_EEPROM_STATUS_SUCCESS != eeprom_limit_check(addr, size))
    {
        LOG_ERROR("Out of eeprom range in read; addr:%u, size:%u", addr, size);

        return BSP_EEPROM_STATUS_ERR_ADDR_SIZE;
    }
    // PRQA S 306 ++ # Its necessary for hold register base address. False positive warning.
    uint8_t const *p_addr = (uint8_t const *)(EEE_RAM_BASE + addr);

    uint32_t cnt = 0U;

    while (size > cnt)
    {
        p_data[cnt] = p_addr[cnt];

        cnt++;
    }

    return BSP_EEPROM_STATUS_SUCCESS;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/188653
 * 
 */
bsp_eeprom_status_t bsp_eeprom_write(uint32_t addr,
                                     const uint8_t *p_data,
                                     size_t size)
{
    if (NULL == p_data)
    {
        LOG_ERROR("Invalid argument in bsp_eeprom_write");

        return BSP_EEPROM_STATUS_ERR_PARAM;
    }

    uint32_t shifted_addr = addr + EEE_RAM_BASE;

    bsp_eeprom_status_t ret = eeprom_limit_check(shifted_addr, size);

    if (BSP_EEPROM_STATUS_SUCCESS == ret)
    {
        flash_ssd_config_t flash_cfg = {
            .PFlashBase = 0x00000000UL,
            .PFlashSize = 0x00020000UL,
            .DFlashBase = 0x10000000UL,
            .DFlashSize = 0x00008000UL,
            .EERAMBase = EEE_RAM_BASE,
            .EEESize = BSP_EEPROM_SIZE,
            // PRQA S 305 ++ # Defined by nxp library to hold NULL pointer for flash callback. False positive warning.
            .CallBack = NULL_CALLBACK,
            // PRQA S 305 --
        };

        status_t status = FLASH_DRV_EEEWrite(&flash_cfg,
                                             shifted_addr,
                                             size,
                                             p_data);

        switch (status)
        {
        case STATUS_SUCCESS:
        {
            ret = BSP_EEPROM_STATUS_SUCCESS;
        }
        break;
        
        case STATUS_UNSUPPORTED:
        {
            ret = BSP_EEPROM_STATUS_ERR_ADDR_SIZE;
            
            LOG_ERROR("EEPROM Address Size Error");
        }
        break;
        
        default:
        {
            ret = BSP_EEPROM_STATUS_ERR;

            LOG_ERROR("EEPROM Unknown Error");
        }
        break;
        }
    }
    else
    {
        LOG_ERROR("Out of eeprom range in write; addr:%u, size:%u", addr, size);
    }

    return ret;
}

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/188658
 * 
 */
static uint8_t eeprom_status_check(void)
{
    return (FTFx_FCNFG & FTFx_FCNFG_EEERDY_MASK);
    // PRQA S 306 --
}

/**
 * @brief https://codebeamer.daiichi.com/issue/188652
 * 
 */
static bsp_eeprom_status_t eeprom_limit_check(uint32_t addr, size_t size)
{
    uint32_t addr_threshold = EEE_RAM_BASE + BSP_EEPROM_SIZE;

    if (addr_threshold <= (uint32_t)(addr + size))
    {
        return BSP_EEPROM_STATUS_ERR_ADDR_SIZE;
    }

    return BSP_EEPROM_STATUS_SUCCESS;
}

// end of file
