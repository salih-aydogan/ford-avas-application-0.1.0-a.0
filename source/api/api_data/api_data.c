// PRQA S 3108 EOF # Comments with links are required for ensure traceability.
/**
 * @file api_data.c
 * @author berke yalcinkaya (berke.yalcinkaya@daiichi.com)
 * @brief This module is the implementation of the Api Data interface.
 * @version 0.1
 * @date 2023-04-17
 *
 * @copyright Copyright (c) 2023
 *
 * https://codebeamer.daiichi.com/issue/188750
 *
 */

#include "api_data.h"

#include "bsp_eeprom.h"
#include "bsp_mcu.h"
#include "drv_flash.h"

#include "log.h"

LOG_MODULE_NAME("API_DATA");


#define USE_INTERNAL_FLASH_FOR_PLAYING_AUDIO (0)

#if USE_INTERNAL_FLASH_FOR_PLAYING_AUDIO
#warning USE_INTERNAL_FLASH_FOR_PLAYING_AUDIO enabled.

extern const uint8_t *p_flash_update_data;

#endif

/*******************************************************************************
 * Macros
 ******************************************************************************/
// PRQA S 380 ++ # Too many macros in the driver provided by NXP causes this error.

/**
 * @brief This macro indicates the EEPROM authentication value.
 *
 * https://codebeamer.daiichi.com/issue/189307
 *
 */
#define EEPROM_AUTH (0xAAU)

/**
 * @brief This macro indicates the number of data to save with default values.
 *
 * https://codebeamer.daiichi.com/issue/189308
 *
 */
#define EEPROM_DEF_DATA_COUNT (2U)

/**
 * @brief This macro indicates the current audio default value.
 *        - AUDIO_1 - 0U
 *        - AUDIO_2 - 1U
 *        - AUDIO_3 - 2U
 *
 * https://codebeamer.daiichi.com/issue/189241
 *
 */
#define CURR_AUDIO (0U)

// PRQA S 380 --

/*******************************************************************************
 * Enumerations
 ******************************************************************************/

/**
 * @brief List of memory locations where data is saved.
 *
 * https://codebeamer.daiichi.com/issue/188892
 *
 */
// PRQA S 3213 ++ # Supressed, The tag 'mem_location_e' is not used and could be removed.
typedef enum mem_location_e
// PRQA S 3213 --
{
    MEM_LOCATION_UNKNOWN = 0U,
    MEM_LOCATION_EEPROM = 1U,
    MEM_LOCATION_EXT_FLASH = 2U,

} mem_location_t;

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/**
 * @brief This variable holds the external flash initialize status.
 *
 * https://codebeamer.daiichi.com/issue/188890
 *
 */
static bool m_flash_init = false;

/*******************************************************************************
 * Private Function Declarations
 ******************************************************************************/

/**
 * @brief This function calculates the address saved in the external flash.
 *
 * @param data_label Data label.
 * @param offset     The desired value of the audio file is to be shifted.
 * @param p_addr     Calculated external flash address.
 *
 * @return bool
 *
 * @retval true  The calculation is successfully done.
 * @retval false Outside the given data_label and offset audio file limits.
 *
 * https://codebeamer.daiichi.com/issue/188878
 *
 */
static bool flash_addr_calc(uint32_t data_label,
                            uint32_t offset,
                            uint32_t *p_addr);

/**
 * @brief This function gets the memory location of the given data.
 *
 * @param data_label Data label specified in api_data interface.
 *
 * @return mem_location_t
 *
 * @retval MEM_LOCATİON_EEPROM
 * @retval MEM_LOCATION_EXT_FLASH
 * @retval MEM_LOCATION_UNKNOWN
 *
 * https://codebeamer.daiichi.com/issue/188880
 *
 */
static mem_location_t mem_location_get(uint32_t data_label);

/**
 * @brief This function checks for EEPROM errors. It prints a LOG message
 *        according to the error.
 *
 * @param status EEPROM status variable.
 *
 * @return bool
 *
 * @retval true  EEPROM error does not exist.
 * @retval false EEPROM error exist.
 */
static bool eeprom_err_check(bsp_eeprom_status_t status);

/**
 * @brief This function provides to manages EEPROM read operations.
 *
 * @param data_label Data label.
 * @param p_data     Data address to be read.
 * @param size       Data size.
 *
 * @return bool
 *
 * @retval true  Data read is successfully done.
 * @retval false EEPROM read error.
 *
 * https://codebeamer.daiichi.com/issue/191038
 *
 */
static bool eeprom_read(uint32_t data_label,
                        uint8_t *p_data,
                        size_t size);

/**
 * @brief This function provides to manages EEPROM write operations.
 *
 * @param data_label Data label.
 * @param p_data     Data address to be written.
 * @param size       Data size.
 *
 * @return bool
 *
 * @retval true  Data written is successfully done.
 * @retval false EEPROM written error.
 *
 * https://codebeamer.daiichi.com/issue/191039
 *
 */
static bool eeprom_write(uint32_t data_label,
                         const uint8_t *p_data,
                         size_t size);

/**
 * @brief This function fills the parameter fields after the eeprom reserve
 *        field with zero.
 *
 * @return bool Function status.
 *
 * @retval true  EEPROM write successfully done.
 * @retval false EEPROM write fail.
 *
 * https://codebeamer.daiichi.com/issue/190762
 *
 */
static bool eeprom_reset(void);

/**
 * @brief This function writes the default values of EEPROM data to EEPROM.
 *
 * https://codebeamer.daiichi.com/issue/189280
 *
 */
static bool default_set(void);

/**
 * @brief This function checks the correctness of the EEPROM.
 *
 * @return bool
 *
 * @retval true  EEPROM authentication success.
 * @retval false EEPROM authentication fails.
 *
 * https://codebeamer.daiichi.com/issue/189279
 *
 */
static bool eeprom_auth_check(void);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/188882
 *
 */
bool api_data_init(void)
{
    bool ret = drv_flash_init();

    m_flash_init = ret;

    if (true == ret)
    {
        if (false == eeprom_auth_check())
        {
            LOG_WARNING("EEE Auth fail");

            ret = default_set();
        }

        LOG_INFO("Initialized");
    }
    else
    {
        LOG_ERROR("Init Failed");
    }

    return ret;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/188879
 *
 */
bool api_data_read(uint32_t data_label,
                   uint8_t *p_data,
                   size_t size,
                   uint32_t offset)
{
    if (NULL == p_data)
    {
        LOG_ERROR("Invalid argument in api_data_read");

        return false;
    }

    bool ret = true;

    mem_location_t mem = mem_location_get(data_label);

    if (MEM_LOCATION_EEPROM == mem)
    {
        ret = eeprom_read(data_label, p_data, size);
    }
    else if ((MEM_LOCATION_EXT_FLASH == mem) &&
             (true == m_flash_init))
    {
        #if USE_INTERNAL_FLASH_FOR_PLAYING_AUDIO

        uint32_t addr = 0U;

        if (true == flash_addr_calc(data_label, offset, &addr))
        {
            memcpy(p_data, &p_flash_update_data[addr], size);

            LOG_VERBOSE("Internal Audio read addr: 0x%x, size: %d", addr, size);
        }

        #else

        uint32_t addr = 0U;

        if (true == flash_addr_calc(data_label, offset, &addr))
        {
            ret = drv_flash_read(addr, p_data, size);

            LOG_VERBOSE("Flash async read. addr: 0x%x, size: %d", addr, size);
        }

        if (false == ret)
        {
            static uint32_t last_flash_error_timeout = 0U;

            if (bsp_mcu_tick_get() > last_flash_error_timeout)
            {
                LOG_ERROR("Read error; flash addr: 0x%x, size: %d", 
                          addr, 
                          size);

                last_flash_error_timeout = bsp_mcu_tick_get() + 1000U;
            }
        }

        #endif
    }
    else if (MEM_LOCATION_UNKNOWN == mem)
    {
        ret = false;

        LOG_ERROR("UNKNOWN data");
    }
    else
    {
        ret = false;

        LOG_ERROR("EXTFLSH not init");
    }

    return ret;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/188881
 *
 */
bool api_data_write(uint32_t data_label,
                    const uint8_t *p_data,
                    size_t size)
{
    if (NULL == p_data)
    {
        LOG_ERROR("Invalid argument in api_data_write");

        return false;
    }

    bool ret = false;

    mem_location_t mem = mem_location_get(data_label);

    if (MEM_LOCATION_EEPROM == mem)
    {
        ret = eeprom_write(data_label, p_data, size);
    }
    else if ((MEM_LOCATION_EXT_FLASH == mem) &&
             (true == m_flash_init))
    {
        uint32_t addr = 0U;

        if (true == flash_addr_calc(data_label, 0U, &addr))
        {
            ret = drv_flash_write(addr, p_data, size);

            LOG_VERBOSE("Flash async write. addr: %u, size: %d", addr, size);

            LOG_TX(p_data, size);
        }

        if (false == ret)
        {
            static uint32_t last_flash_error_timeout = 0U;

            if (bsp_mcu_tick_get() > last_flash_error_timeout)
            {
                LOG_ERROR("Write error; flash addr: 0x%x, size: %d", 
                          addr, 
                          size);

                last_flash_error_timeout = bsp_mcu_tick_get() + 1000U;
            }
        }
    }
    else if (MEM_LOCATION_UNKNOWN == mem)
    {
        ret = false;

        LOG_ERROR("UNKNOWN data");
    }
    else
    {
        ret = false;

        LOG_ERROR("EXTFLSH not init");
    }

    return ret;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/191744
 *
 */
bool api_data_dtc_set(uint32_t data_label, uint32_t dtc)
{
    if (0U == dtc)
    {
        return false;
    }

    uint8_t dtc_arr[API_DATA_DTC_SIZE] = {0U};

    uint32_t idx = 0U;

    const uint32_t DTC = dtc;

    uint8_t const *p_dtc = (uint8_t const *)&DTC;

    for (idx = 0U; sizeof(uint32_t) > idx; idx++)
    {
        dtc_arr[idx] = p_dtc[idx];
    }

    // TODO - Vehicle speed, current gear, and trans gear -> CAN
    (void)dtc_arr;
    (void)data_label;

    return true;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/197670
 *
 */
bool api_data_cb_set(api_data_cb_t callback)
{
    bool ret = drv_flash_cb_set((drv_flash_cb_t)callback);

    if (false == ret)
    {
        LOG_ERROR("Flash cb set failed");
    }
    
    return ret;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/197543
 *
 */
void api_data_process(void)
{
    drv_flash_process();
}

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/188878
 *
 */
static bool flash_addr_calc(uint32_t data_label,
                            uint32_t offset,
                            uint32_t *p_addr)
{
    *p_addr = (data_label - API_DATA_EEPROM_LAST_DATA) + offset;

    if (*p_addr > (data_label + API_DATA_AUDIO_DEFAULT_SIZE))
    {
        LOG_ERROR("EXTFLSH limit access error");

        return false;
    }

    return true;
}

/**
 * https://codebeamer.daiichi.com/issue/188880
 *
 */
static mem_location_t mem_location_get(uint32_t data_label)
{
    if (API_DATA_EEPROM_LAST_DATA > data_label)
    {
        return MEM_LOCATION_EEPROM;
    }
    else if ((API_DATA_FLASH_LAST_DATA > data_label) &&
             (API_DATA_EEPROM_LAST_DATA <= data_label))
    {
        return MEM_LOCATION_EXT_FLASH;
    }
    else
    {
        return MEM_LOCATION_UNKNOWN;
    }
}

/**
 * @brief https://codebeamer.daiichi.com/issue/188877
 *
 */
static bool eeprom_err_check(bsp_eeprom_status_t status)
{
    bool ret = true;

    switch (status)
    {
    case BSP_EEPROM_STATUS_NOT_RDY:
    {
        ret = false;

        LOG_ERROR("EEE not ready");
    }
    break;

    case BSP_EEPROM_STATUS_ERR_ADDR_SIZE:
    {
        ret = false;

        LOG_ERROR("EEE limit access error");
    }
    break;

    case BSP_EEPROM_STATUS_ERR:
    {
        ret = false;

        LOG_ERROR("EEE write error");
    }
    break;

    default:
    {
        ret = true;
    }
    break;
    }

    return ret;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/191038
 *
 */
static bool eeprom_read(uint32_t data_label,
                        uint8_t *p_data,
                        size_t size)
{
    bsp_eeprom_status_t status = bsp_eeprom_read(data_label,
                                                 p_data,
                                                 size);

    LOG_VERBOSE("EEPROM read. label: %u, size: %u, status: %u", 
                 data_label, 
                 size,
                 status);

    LOG_RX(p_data, size);

    bool ret = eeprom_err_check(status);

    return ret;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/191039
 *
 */
static bool eeprom_write(uint32_t data_label,
                         const uint8_t *p_data,
                         size_t size)
{
    bsp_eeprom_status_t status = bsp_eeprom_write(data_label,
                                                  p_data,
                                                  size);
    
    LOG_VERBOSE("EEPROM write. label: %u, size: %u, status: %u", 
                 data_label, 
                 size,
                 status);

    LOG_TX(p_data, size);

    bool ret = eeprom_err_check(status);

    return ret;
}

/**
 * @brief This function writes the zero of the defined EEPROM parameters.
 *
 * @return bool Function status.
 *
 * @retval true  The EEPROM write operations were successfully done.
 * @retval false The EEPROM write failed.
 *
 * https://codebeamer.daiichi.com/issue/190762
 *
 */
static bool eeprom_reset(void)
{
    uint32_t eeprom_addr = API_DATA_LABEL_CAR_SPEED;

    const uint8_t data_tbl[250U] = {0U};

    size_t data_tbl_size = 250U;

    bool ret = true;

    while ((false != ret) &&
           (0U != data_tbl_size))
    {
        if (API_DATA_EEPROM_LAST_DATA > eeprom_addr)
        {
            ret = eeprom_write(eeprom_addr, data_tbl, data_tbl_size);

            eeprom_addr = eeprom_addr + data_tbl_size;
        }
        else
        {
            data_tbl_size = eeprom_addr % API_DATA_EEPROM_LAST_DATA;

            if (0U != data_tbl_size)
            {
                ret = eeprom_write(eeprom_addr, data_tbl, data_tbl_size);

                data_tbl_size = 0U;
            }
        }
    }

    return (0U == data_tbl_size);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/189280
 *
 */
static bool default_set(void)
{
    const uint32_t eeprom_default_tbl[EEPROM_DEF_DATA_COUNT][3U] = {

        {EEPROM_AUTH, API_DATA_LABEL_AUTH, 1U},
        {CURR_AUDIO, API_DATA_LABEL_CURR_AUDIO, 1U},
    };

    if (true == eeprom_reset())
    {
        for (uint8_t idx = 0; EEPROM_DEF_DATA_COUNT > idx; idx++)
        {
            uint32_t data = (uint32_t)eeprom_default_tbl[idx][0U];
            uint32_t label = (uint32_t)eeprom_default_tbl[idx][1U];

            size_t size = (size_t)eeprom_default_tbl[idx][2U];

            (void)eeprom_write(label,
                               (uint8_t *)&data,
                               size);
        }
    
        LOG_WARNING("EEE default values are set");
    }
    else
    {
        LOG_ERROR("EEE default values set fail");

        return false;
    }

    return true;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/189279
 *
 */
static bool eeprom_auth_check(void)
{
    bool success = false;

    uint8_t eeprom_auth = 0U;

    const uint32_t max_try_cnt = 10U;
    uint32_t try_idx;

    for (try_idx = 0U;
         (max_try_cnt > try_idx) && (false == success);
         try_idx++)
    {
        (void)eeprom_read(API_DATA_LABEL_AUTH,
                          &eeprom_auth,
                          1U);

        if (EEPROM_AUTH != eeprom_auth)
        {
            bsp_mcu_delay(10U);
        }
        else
        {
            success = true;
        }
    }

    if ((max_try_cnt <= try_idx) && (0x00 == eeprom_auth))
    {
        LOG_WARNING("EEPROM is empty. Expected val: %u, Found val: %u",
                    (uint8_t)EEPROM_AUTH,
                    eeprom_auth);
    }

    if ((max_try_cnt <= try_idx) && (0x00 != eeprom_auth))
    {
        LOG_WARNING("EEPROM table is changed. Expected val: %u, Found val: %u",
                    (uint8_t)EEPROM_AUTH,
                    eeprom_auth);
    }

    return (max_try_cnt > try_idx);
}

// end of file
