// PRQA S 3108 EOF # Supressed, Comments with links are required for ensure traceability.
/**
 * @file app_main.c
 * @author Burak Utku Cecen (utku.cecen@daiichi.com)
 * @brief
 * @version 0.1
 * @date 2023-05-02
 *
 * @copyright Copyright (c) 2023
 *
 * https://codebeamer.daiichi.com/issue/190750
 *
 */

#include "app_main.h"

#include "bsp_mcu.h"
#include "bsp_isr.h"
#include "bsp_wdg.h"
#include "bsp_uart.h"
#include "bsp_gpio.h"
#include "bsp_can.h"
#include "bsp_i2c.h"
#include "bsp_spi.h"
#include "system_config.h"
#include "bsp_adc.h"
#include "bsp_i2s.h"
#include "api_data.h"
#include "bsp_dma.h"
#include "power_manager_S32K1xx.h" // added only for log
#include "canif.h" //todo silinecek

#include "log.h"

#include "appman.h"

LOG_MODULE_NAME("APP_MAIN");

#define BUILD_TIMESTAMP "BUILD: " __DATE__ " " __TIME__

static const char *const m_build_timestamp = BUILD_TIMESTAMP;

/*******************************************************************************
 * Public Variables
 ******************************************************************************/

/**
 * @brief Specifies app_main message pool.
 *
 */
uint8_t g_app_main_msg_pool[APP_MAIN_MSG_SIZE * APP_MAIN_MSG_POOL_SIZE];

/*******************************************************************************
 * Structs
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/191081
 *
 */
// PRQA S 3213 ++ # Supressed, The tag 'app_main_data_s' is not used and could be removed.
typedef struct app_main_data_s
{
    appman_app_id_t app_main_id;

    void *p_data;

    size_t size;

} app_main_data_t;

// PRQA S 3213 ++

/*******************************************************************************
 * Private Function Declarations
 ******************************************************************************/

/**
 * @brief This function used for report to SW version and initialization
 *        time.
 *
 */
static void system_config_log(void);

/**
 * @brief This function used to read last reset reason from CPU registers and
 *        report via log.
 *
 */
static void last_reset_reason_log(void);

/**
 * @brief This function provides report to watchdog timeout error.
 *
 */
static void wdg_cb(void);

/**
 * @brief https://codebeamer.daiichi.com/issue/190761
 *
 */
static void hard_fault_cb(void);


static void can_settings(void); //todo silinecek

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/190758
 *
 */
void app_main_init(void)
{
    bsp_mcu_status_t ret = bsp_mcu_init();
    
    if (BSP_MCU_STATUS_ERROR == ret)
    {
        return;
    }

    bsp_isr_init();

    bool ret_val = bsp_wdg_init(wdg_cb);

    if (false == ret_val)
    {
        return;
    }

    ret_val = bsp_dma_init();

    if (false == ret_val)
    {
        return;
    }

    ret_val = bsp_uart_init();

    if (false == ret_val)
    {
        return;
    }

    log_init();

    ret_val = bsp_gpio_init();

    if (false == ret_val)
    {
        LOG_ERROR("BSP_GPIO Init Fail");
        
        return;
    }

    ret_val = bsp_can_init();

    if (false == ret_val)
    {
        LOG_ERROR("BSP_CAN Init Fail");
        
        return;
    }

    can_settings(); //todo silinecek

    ret_val = bsp_i2c_init();

    if (false == ret_val)
    {
        LOG_ERROR("BSP_I2C Init Fail");
        
        return;
    }

    ret_val = bsp_i2s_init();

    if (false == ret_val)
    {
        LOG_ERROR("BSP_I2S Init Fail");
        
        return;
    }

    ret_val = bsp_spi_init();

    if (false == ret_val)
    {
        LOG_ERROR("BSP_SPI Init Fail");
        
        return;
    }

    bsp_adc_init();

    ret_val = api_data_init();

    if (false == ret_val)
    {
        LOG_ERROR("API_DATA Init Fail");

        return;
    }

    bsp_isr_cb_set(BSP_ISR_HARDFAULT, hard_fault_cb);

    system_config_log();

    last_reset_reason_log();

    drv_amplifier_init(); // TODO : tasinacak

    drv_amplifier_unmute(); // TODO : tasinacak

    return;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/190759
 *
 */
void app_main_deinit(void)
{
    bool ret = bsp_can_deinit();

    if (false == ret)
    {
        LOG_ERROR("BSP_CAN Deinit Fail");

        return;
    }

    ret = bsp_spi_deinit();

    if (false == ret)
    {
        LOG_ERROR("BSP_SPI Deinit Fail");

        return;
    }

    ret = bsp_i2c_deinit();

    if (false == ret)
    {
        LOG_ERROR("BSP_I2C Deinit Fail");

        return;
    }

    ret = bsp_i2s_deinit();

    if (false == ret)
    {
        LOG_ERROR("BSP_I2S Deinit Fail");

        return;
    }

    bsp_adc_deinit();

    ret = bsp_uart_deinit();

    if (false == ret)
    {
        LOG_ERROR("BSP_UART Deinit Fail");

        return;
    }

    ret = bsp_dma_deinit();

    if (false == ret)
    {
        LOG_ERROR("BSP_UART Deinit Fail");

        return;
    }
}

/**
 * @brief https://codebeamer.daiichi.com/issue/190760
 *
 */
void app_main_run(void)
{
    bsp_wdg_kick();

    log_process();

    api_data_process();

    drv_amplifier_process(); // TODO : tasinacak
}

/**
 * @brief https://codebeamer.daiichi.com/issue/190756
 *
 */
// PRQA S 3673 ++ # Since the function will be saved in appman, its p_msg argument cannot be made const.
void app_main_msg(void *p_msg)
// PRQA S 3673 --
{
    app_main_data_t app_main_buff;

    if (NULL == p_msg)
    {
        LOG_ERROR("Invalid argument in app_main_msg");

        return;
    }

    const app_main_data_t *p_data = (const app_main_data_t *)p_msg;

    app_main_buff.app_main_id = p_data->app_main_id;

    if ((APPMAN_APP_ID_MAIN != app_main_buff.app_main_id) &&
        (APPMAN_APP_ID_ALL != app_main_buff.app_main_id))
    {
        LOG_ERROR("APP_MAIN_MSG INVALID DEST ID");

        return;
    }
}

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief This function used for report to SW version and initialization
 *        time.
 *
 */
static void system_config_log(void)
{
    LOG_INFO("########################################");

    LOG_INFO("Daiichi AVS2022003");

    LOG_INFO("SW Ver: %.*s", SYSTEM_CONFIG_SW_VER_STR_SIZE, g_sw_ver);

    LOG_INFO("%s", m_build_timestamp);

    LOG_INFO("########################################");
}

/**
 * @brief This function used to read last reset reason from CPU registers and
 *        report via log.
 *
 */
static void last_reset_reason_log(void)
{
    // PRQA S 306 ++ # Suppressed, Defined by nxp library to cast register data. False positive warning.

    if (POWER_SYS_GetResetSrcStatusCmd(RCM, RCM_LOW_VOLT_DETECT) == true)
    {
        LOG_WARNING("Last reset reason: Low voltage detect");
    }

    if (POWER_SYS_GetResetSrcStatusCmd(RCM, RCM_LOSS_OF_CLK) == true)
    {
        LOG_WARNING("Last reset reason: Loss of clock");
    }

    if (POWER_SYS_GetResetSrcStatusCmd(RCM, RCM_LOSS_OF_LOCK) == true)
    {
        LOG_WARNING("Last reset reason: Loss of lock");
    }

    if (POWER_SYS_GetResetSrcStatusCmd(RCM, RCM_CMU_LOC) == true)
    {
        LOG_WARNING("Last reset reason: CMU Loss of lock");
    }

    if (POWER_SYS_GetResetSrcStatusCmd(RCM, RCM_WATCH_DOG) == true)
    {
        LOG_WARNING("Last reset reason: Watchdog");
    }

    if (POWER_SYS_GetResetSrcStatusCmd(RCM, RCM_EXTERNAL_PIN) == true)
    {
        LOG_WARNING("Last reset reason: External pin");
    }

    if (POWER_SYS_GetResetSrcStatusCmd(RCM, RCM_POWER_ON) == true)
    {
        LOG_WARNING("Last reset reason: Power On Reset");
    }

    if (POWER_SYS_GetResetSrcStatusCmd(RCM, RCM_SJTAG) == true)
    {
        LOG_WARNING("Last reset reason: JTAG reset");
    }

    if (POWER_SYS_GetResetSrcStatusCmd(RCM, RCM_CORE_LOCKUP) == true)
    {
        LOG_WARNING("Last reset reason: Core lockup");
    }

    if (POWER_SYS_GetResetSrcStatusCmd(RCM, RCM_SOFTWARE) == true)
    {
        LOG_WARNING("Last reset reason: Software reset");
    }

    if (POWER_SYS_GetResetSrcStatusCmd(RCM, RCM_SMDM_AP) == true)
    {
        LOG_WARNING("Last reset reason: MDM-AP system reset");
    }

    if (POWER_SYS_GetResetSrcStatusCmd(RCM, RCM_STOP_MODE_ACK_ERR) == true)
    {
        LOG_WARNING("Last reset reason: Stop mode ack error");
    }

    // PRQA S 306 --
}

/**
 * @brief This function provides report to watchdog timeout error and
 *
 * https://codebeamer.daiichi.com/issue/190757
 *
 */
static void wdg_cb(void)
{
    LOG_NOW(LOG_LEVEL_FATAL, "WDG TIMEOUT");

    bsp_mcu_reset();
}

/**
 * @brief This function used for report to hard fault error and
 *
 * https://codebeamer.daiichi.com/issue/190761
 *
 */
static void hard_fault_cb(void)
{
    LOG_NOW(LOG_LEVEL_FATAL, "HARDFAULT");

    bsp_mcu_reset();
}

//todo silinecek

uint8_t my_data[8U] = {0xA, 0xA, 0xA, 0xA, 0xA, 0xA, 0xA, 0xA};

static void can_settings(void)
{
    const uint32_t can_ids[3u] = {0x18F00503U,
                                  0x18FF7B00U,
                                  0x0CFE6CEEU};

    canif_init(&g_canif_config);

    std_return_t ret = canif_trcv_mode_set(CAN_TRCV_MODE_LISTENONLY);


    if (E_NOT_OK == ret)
    {
        return;
    }

    bsp_can_filter(can_ids);

    // bsp_can_data_t my_tx_messages;
    
    // my_tx_messages.msg_id = 0x18FED922U;

    // (void)memcpy(my_tx_messages.payload, my_data, 8U);

    // my_tx_messages.size = 8U;
    
    // bsp_can_status_t ret_val = bsp_can_async_write(&my_tx_messages);

    // if (BSP_CAN_STATUS_OK != ret_val)
    // {
    //     return;
    // }
}

// end of file
