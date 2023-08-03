/**
 ******************************************************************************
 * @file    drv_flash.h
 * @author  Daiichi Arge Team
 * @version V1.0.0
 * @date    04-Apr-2017
 * @brief   Generic Flash Driver header file
 ******************************************************************************
 * @attention COPYRIGHT(c) 2017 Daiichi Arge
 ******************************************************************************
 */

#ifndef DRV_FLASH_H
#define DRV_FLASH_H

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "daiichi_types.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

/**
 * @brief Defines the timeout value of initialization.
 *
 */
#define DRV_FLASH_CFG_TIMEOUT_MS 500U

/**
 * @brief Defines the size of the sector in bytes.
 *
 */
#define DRV_FLASH_CFG_SECTOR_SIZE 4096U

/**
 * @brief The JEDEC ID of the current FLASH
 *
 */
#define DRV_FLASH_CFG_JEDEC_ID1 0x9DU
#define DRV_FLASH_CFG_JEDEC_ID2 0x60U
#define DRV_FLASH_CFG_JEDEC_ID3 0x15U

/**
 * @brief Define the maximum of the sector number
 *
 */
#define DRV_FLASH_SECTOR_ID_MAX 512U
/*******************************************************************************
 * Type Definitions
 ******************************************************************************/

/**
 * @brief Flash driver callback function pointer.
 *
 */
typedef void (*drv_flash_cb_t)(bool err);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Initialize the FLASH driver.
 *
 * @return true  Inıtialization Success.
 * @return false Error.
 *               - JEDEC-ID conflict
 *               - TIMEOUT occurs.
 */
bool drv_flash_init(void);

/**
 * @brief Erase the FLASH chip full area.
 *
 * @return true  Success.
 * @return false Error. FLASH not erased.
 */
bool drv_flash_full_erase(void);

/**
 * @brief Erase the FLASH according to the sector id.
 *
 * @param[in] sector_id The sector number.
 *                      It should be between 0 and DRV_FLASH_SECTOR_ID_MAX
 *
 * @return true  Success.
 * @return false Error. FLASH is not erased.
 */
bool drv_flash_sector_erase(uint32_t sector_id);

/**
 * @brief Lockes the FLASH for any write or erase access.
 *
 * @return true  Success.
 * @return false Error.
 */
bool drv_flash_protect(void);

/**
 * @brief Unlockes the FLASH for any write or erase access.
 *
 * @return true  Success.
 * @return false Error. .
 */
bool drv_flash_unprotect(void);

/**
 * @brief Convert the address to sector number
 *
 * @param[in]  addr        The address.
 * @param[out] p_sector_id Pointer to sector id. It SHOULD NOT be NULL.
 *
 * @return true  Convert is success.
 * @return false Error.
 *               - if the p_sector_id param is NULL.
 *               - if addr is oversize of the FLASH area.
 */
bool drv_flash_addr_to_sector(uint32_t addr, uint32_t *p_sector_id);

/**
 * @brief Convert the sector number to address
 *
 * @param[in]   sector_id     The sector id.
 * @param[out]  p_addr        Pointer to address. It SHOULD NOT be NULL.
 *
 * @return true  Convert is success.
 * @return false Error.
 *               - if the p_addr param is NULL.
 *               - if sector_id is oversize of the FLASH area.
 */
bool drv_flash_sector_to_addr(uint32_t sector_id, uint32_t *p_addr);

/**
 * @brief Write data to the FLASH
 *
 * @param[in] addr   The flash address.
 * @param[in] p_data Pointer to the address of the data written.
 * @param[in] size   The size of data.
 *
 * @return true  The data is written succesfully.
 * @return false Error.
 */
bool drv_flash_write(uint32_t addr, const uint8_t *p_data, uint32_t size);

/**
 * @brief Read data from the FLASH
 *
 * @param[in] addr    The flash address.
 * @param[out] p_data Pointer to the address of the data read.
 * @param[in] size    The size of data.
 *
 * @return true  The data is written succesfully.
 * @return false Error.
 */
bool drv_flash_read(uint32_t addr, uint8_t *p_data, uint32_t size);

/**
 * @brief Registration the given callback.
 *
 * @param[in] callback The function pointer.
 *
 * @return true  The callback is registered successfully.
 * @return false Error.
 */
bool drv_flash_cb_set(drv_flash_cb_t callback);

/**
 * @brief Returns if the flash module is currently busy with performing the last
 *        operation or not.
 *
 * @return true  The flash module is busy. New commands cannot be called.
 * @return false The flash module is free. New commands can be called.
 */
bool drv_flash_is_busy(void);

/**
 * @brief Performs flash command operations.
 *
 *        Must be called in main loop continuosly for flash commands to be
 *        processed correctly.
 */
void drv_flash_process(void);

#endif /* DRV_FLASH_H */

// end of file
