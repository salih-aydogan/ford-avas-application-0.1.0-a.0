/**
 * @file main.c
 * @author Sercan Agacdograyan (sercan.agacdograyan@daiichi.com)
 * @brief Implementation of main.
 * @version 0.1
 * @date 2022-05-01
 *
 * @copyright Copyright (c) 2023
 *
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "daiichi_types.h"

#include "appman.h"
#include "app_main.h"
#include "app_avas.h"

#include "log.h"

#include "bsp_mcu.h"
#include "bsp_isr.h"
#include "bsp_uart.h"
#include "bsp_wdg.h"
#include "bsp_dma.h"
#include "flash_driver.h"

#include "power_manager_S32K1xx.h" //added only for log

#include "drv_flash.h" //added only for manual flash update
#include "bsp_spi.h" //added only for manual flash update
#include "bsp_gpio.h" //added only for manual flash update

/*******************************************************************************
 * Macros
 ******************************************************************************/

LOG_MODULE_NAME("MAIN");

// PRQA S 380 ++ # Suppressed, NXP flash driver uses too many macros.

#define SECURITY_ENABLED (0U)

#if SECURITY_ENABLED
#warning SECURITY enabled, DO NOT download this code to a chip before \
             making sure the CSEc code works. The chip may become bricked.
#endif

#define MANUAL_FLASH_UPDATE_ENABLED (0)

#if MANUAL_FLASH_UPDATE_ENABLED
#warning MANUAL_FLASH_UPDATE_ENABLED enabled.

extern const uint8_t *p_flash_update_data;

extern const uint32_t flash_update_size;

extern const uint32_t flash_update_start_sector;

#endif

// PRQA S 380 --

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

static appman_app_t m_app_main =
    {
        .p_name = "APP_MAIN",

        .app_id = APPMAN_APP_ID_MAIN,

        .init = app_main_init,
        .run = app_main_run,
        .deinit = app_main_deinit,

        .msg_handler = app_main_msg,

        // PRQA S 314 ++ # Suppressed, Message pools in Apps are defined as a raw memory area, and casted when used.
        .p_msg_pool = (void *)&g_app_main_msg_pool,
        // PRQA S 314 --

        .msg_pool_size = APP_MAIN_MSG_POOL_SIZE,
};

static appman_app_t m_app_avas =
    {
        .p_name = "APP_AVAS",

        .app_id = APPMAN_APP_ID_AVAS,

        .init = app_avas_init,
        .run = app_avas_run,
        .deinit = app_avas_deinit,

        .msg_handler = app_avas_msg_handler,

        // PRQA S 314 ++ # Suppressed, Message pools in Apps are defined as a raw memory area, and casted when used.
        .p_msg_pool = (void *)&g_app_avas_msg_pool,
        // PRQA S 314 --

        .msg_pool_size = APP_AVAS_MSG_POOL_SIZE,
};

static const flash_user_config_t m_flash_user_cfg =
    {
        .PFlashBase = 0x00000000U, /* Base address of Program Flash block */
        .PFlashSize = 0x00020000U, /* Total size of Program Flash block */
        .DFlashBase = 0x10000000U, /* Base address of Data Flash block */
        .EERAMBase = 0x14000000U,  /* Base address of EERAM */

        // PRQA S 305 ++ # Suppressed, Defined by nxp library to hold a placeholder value. False positive warning.
        .CallBack = NULL_CALLBACK
        // PRQA S 305 --
};

/*******************************************************************************
 * Private Function Declarations
 ******************************************************************************/
static void init_main_peripherals(void);

static bool is_memory_partitioned(void);

static bool is_secure_boot_enabled(void);

static bool is_backdoor_rescue_triggered(void);

static void perform_memory_partitioning(void);

#if SECURITY_ENABLED
static void load_keys(void);

static void enable_secure_boot(void);
#endif

static void erase_keys(void);

#if MANUAL_FLASH_UPDATE_ENABLED

static void update_flash(void);

#endif

static void log_flash_security_statuses(void);

static void log_last_reset_reason(void);

static void software_reset(void);

static void wait_for_flash_complete(void);

static void main_wdg_cb(void);

static void main_hard_fault_cb(void);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

int main(void)
{
    if (false == is_memory_partitioned())
    {        
        init_main_peripherals();

        LOG_NOW(LOG_LEVEL_WARNING, "Empty memory partition detected");

        perform_memory_partitioning();

        software_reset();
    }

#if SECURITY_ENABLED
    if (false == is_secure_boot_enabled())
    {
        init_main_peripherals();

        LOG_NOW(LOG_LEVEL_WARNING, "Secure boot not enabled detected");

        load_keys();

        enable_secure_boot();

        software_reset();
    }
#endif

    if ((true == is_secure_boot_enabled()) &&
        (true == is_backdoor_rescue_triggered()))
    {
        init_main_peripherals();

        LOG_NOW(LOG_LEVEL_WARNING, 
                "Secure Boot Backdoor Rescue request detected");

        erase_keys();

        software_reset();
    }

#if MANUAL_FLASH_UPDATE_ENABLED

    init_main_peripherals();

    LOG_NOW(LOG_LEVEL_WARNING, "Updating external flash manually");

    update_flash();

    while(1){bsp_wdg_kick();};

#endif

    LOG_INFO("---------------------------------");
    LOG_INFO("System Started");

    appman_init();

    appman_app_create(&m_app_main, NULL);

    appman_app_create(&m_app_avas, NULL);

    // TODO: Add appman_app_create create app_can

    // TODO: Add appman_app_create create app_diagnostic

    for (;;)
    {
        appman_process();
    }
}

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief Initializes critical peripherals to complete basic MCU works and log.
 *
 */
static void init_main_peripherals(void)
{
    (void)bsp_mcu_init();

    bsp_isr_init();

    (void)bsp_wdg_init(main_wdg_cb);

    (void)bsp_dma_init();

    (void)bsp_uart_init();

    log_init();

    bsp_isr_cb_set(BSP_ISR_HARDFAULT, main_hard_fault_cb);

    LOG_NOW(LOG_LEVEL_WARNING, "---------------------------------");
    LOG_NOW(LOG_LEVEL_WARNING, "Main Started");

    log_last_reset_reason();
}

/**
 * @brief Returns if the configurable memory parts (FlexRAM, FlexNVM) is already
 *        configured to be used or not. Memory should configured once after each
 *        mass erase.
 *
 * @return bool partition status.
 *
 * @retval true  Memory is partitioned before.
 * @retval false Memory is not partitioned.
 */
static bool is_memory_partitioned(void)
{
    flash_ssd_config_t flash_cfg = {0};

    (void)FLASH_DRV_Init(&m_flash_user_cfg, &flash_cfg);

    return (0U != flash_cfg.EEESize);
}

/**
 * @brief Returns if secure boot is enabled in this chip or not.
 *        Checks CSEc keys is loaded.
 *
 * @return bool Secure boot status.
 *
 * @retval true  Secure boot is enabled.
 * @retval false Secure boot is not enabled.
 */
static bool is_secure_boot_enabled(void)
{
    // TODO : Add "CSEc get status"

    return false;
}

/**
 * @brief Returns if the user activated the backdoor rescue sequence or not.
 *
 * @return bool Backdoor rescue sequence.
 *
 * @retval true  Backdoor rescue sequence is triggered.
 * @retval false  Backdoor rescue sequence is not triggered.
 */
static bool is_backdoor_rescue_triggered(void)
{
    // TODO : Add "Backdoor rescue check"

    return false;
}

/**
 * @brief Partitions the configurable memory parts using the selected
 *        configuration settings.
 *
 */
static void perform_memory_partitioning(void)
{
    LOG_NOW(LOG_LEVEL_WARNING, "Performing Memory Partition");

    log_flash_security_statuses();

    flash_ssd_config_t flash_cfg = {0};

    (void)FLASH_DRV_Init(&m_flash_user_cfg, &flash_cfg);

    const uint8_t eeprom_data_size_code = 0x3;
    /* EEPROM Data Size Code:
        0x3 => 2kB EEPROM (necessary for CSEc)
        0xF => 0kB EEPROM (no EEPROM) */

    const uint8_t flexnvm_partition_code = 0x3;
    /* FlexNVM Partition Code:
        0x0 => 32 kB Data flash,  0 kB EEPROM-Backup
        0x3 =>  0 kB Data flash, 32 kB EEPROM-Backup (necessary for CSEc)
        0x8 =>  0 kB Data flash, 32 kB EEPROM-Backup (necessary for CSEc)
        0x9 =>  8 kB Data flash, 24 kB EEPROM-Backup
        0xB => 32 kB Data flash,  0 kB EEPROM-Backup */

#if SECURITY_ENABLED
    const uint8_t csec_key_size_code = 0x1;
#else
    const uint8_t csec_key_size_code = 0x0;
#endif
    /* CSEc Key Size Code:
        0x0 => 0  keys,   0 bytes used in EEERAM
        0x1 => 5  keys, 128 bytes used in EEERAM
        0x2 => 10 keys, 256 bytes used in EEERAM
        0x3 => 20 keys, 512 bytes used in EEERAM */

    const bool security_flag_extention = false;
    /* Security Flag Extension:
        0 => Flash Key 'Verify Only' attribute not enabled
        1 => Flash Key 'Verify Only' attribute enabled */

#if SECURITY_ENABLED
    const bool flexram_load_during_reset_option = true;
#else
    const bool flexram_load_during_reset_option = true;
#endif
    /* FlexRAM load during reset option:
        0 => FlexRAM loaded with valid EEPROM data during reset(needed for CSEc)
        1 => FlexRAM not loaded during reset sequence */

    wait_for_flash_complete();

    LOG_NOW(LOG_LEVEL_WARNING,
        "Partitioning; eeprom:%u, flexnvm:%u, csec:%u, sec_flg:%u, ramload:%u",
        eeprom_data_size_code,
        flexnvm_partition_code,
        csec_key_size_code,
        security_flag_extention,
        flexram_load_during_reset_option);

    bsp_isr_enter_critical();

    status_t status = FLASH_DRV_DEFlashPartition(
        &flash_cfg,
        eeprom_data_size_code,
        flexnvm_partition_code,
        csec_key_size_code,
        security_flag_extention,
        flexram_load_during_reset_option);

    bsp_isr_exit_critical();

    LOG_NOW(LOG_LEVEL_WARNING,
            "Partition command result, status=%u", status);

    wait_for_flash_complete();

    bsp_isr_enter_critical();

    status = FLASH_DRV_SetFlexRamFunction(
        &flash_cfg,
        EEE_ENABLE,
        0x0000,
        NULL);

    bsp_isr_exit_critical();

    wait_for_flash_complete();

    LOG_NOW(LOG_LEVEL_WARNING,
            "FlexRAM function set command result, status=%u", status);
}

#if SECURITY_ENABLED
/**
 * @brief Loads master key, boot keys and other keys
 *
 */
static void load_keys(void)
{
    LOG_NOW(LOG_LEVEL_WARNING, "Loading all keys");

    log_flash_security_statuses();

    bsp_isr_enter_critical();

    // TODO : Add CSEc key loads
    // TODO : Add secure boot enable

    bsp_isr_exit_critical();
}

/**
 * @brief Enables secure boot.
 *
 */
static void enable_secure_boot(void)
{
    LOG_NOW(LOG_LEVEL_WARNING, "Enabling secure boot");

    log_flash_security_statuses();

    bsp_isr_enter_critical();

    // TODO : Add CSEc key loads
    // TODO : Add secure boot enable

    bsp_isr_exit_critical();
}
#endif

/**
 * @brief Erases all keys stored in CSEc to release security.
 *
 */
static void erase_keys(void)
{
    LOG_NOW(LOG_LEVEL_WARNING,
            "Backdoor rescue trigger detected, erasing all keys");

    log_flash_security_statuses();

    bsp_isr_enter_critical();

    // TODO : Add CSEc all key erase

    bsp_isr_exit_critical();
}

#if MANUAL_FLASH_UPDATE_ENABLED

static void update_flash(void)
{
    uint32_t remaining_bytes = flash_update_size;

    uint32_t updated_sector_cnt = 0U;

    (void)bsp_gpio_init();

    (void)bsp_spi_init();

    drv_flash_init();
    while(drv_flash_is_busy()){drv_flash_process();};

    drv_flash_unprotect();
    while(drv_flash_is_busy()){drv_flash_process();};

    while(0U < remaining_bytes)
    {
        bsp_wdg_kick();

        const uint32_t target_sector = updated_sector_cnt + 
                                       flash_update_start_sector;

        uint32_t target_addr = 0;
        
        (void)drv_flash_sector_to_addr(target_sector, &target_addr);

        const uint8_t *p_target_data = p_flash_update_data + 
                            (DRV_FLASH_CFG_SECTOR_SIZE * updated_sector_cnt);
        
        const uint32_t target_size = 
                            (DRV_FLASH_CFG_SECTOR_SIZE <= remaining_bytes) ? 
                             DRV_FLASH_CFG_SECTOR_SIZE : remaining_bytes;
        
        uint8_t read_data[DRV_FLASH_CFG_SECTOR_SIZE] = {0};

        LOG_NOW(LOG_LEVEL_WARNING, "Updating; Sector:%u, Address:%u, Size:%u",
                target_sector, 
                target_addr, 
                target_size);


        drv_flash_read(target_addr, read_data, target_size);
        while(drv_flash_is_busy()){drv_flash_process();};

        bsp_wdg_kick();

        if (0 != memcmp(read_data, p_target_data, target_size))
        {
            drv_flash_sector_erase(target_sector);
            while(drv_flash_is_busy()){drv_flash_process();};

            bsp_wdg_kick();

            drv_flash_write(target_addr, p_target_data, target_size);
            while(drv_flash_is_busy()){drv_flash_process();};

            bsp_wdg_kick();

            drv_flash_read(target_addr, read_data, target_size);
            while(drv_flash_is_busy()){drv_flash_process();};

            bsp_wdg_kick();

            if (0 != memcmp(read_data, p_target_data, target_size))
            {
                LOG_NOW(LOG_LEVEL_FATAL, "Flash Update Failed!");
                LOG_NOW(LOG_LEVEL_FATAL, "Sector:%u, Address:%u, Size:%u",
                        target_sector, 
                        target_addr, 
                        target_size);

                while(1){bsp_wdg_kick();};
            }
            else
            {
                LOG_NOW(LOG_LEVEL_WARNING, "Sector write successful");
            }
        }
        else
        {
            LOG_NOW(LOG_LEVEL_WARNING, 
                "Skipping this sector since all data in flash is matched");
        }
        
        remaining_bytes = remaining_bytes - target_size;

        updated_sector_cnt++;
    }

    LOG_NOW(LOG_LEVEL_WARNING, "Flash Update Successfully Finished!");
}

#endif

/**
 * @brief Printf useful information about current flash security bits.
 *        Used for only debug purposes.
 *
 */
static void log_flash_security_statuses(void)
{
    // PRQA S 2985 ++ # Suppressed, Defined by nxp library to mask register bits. False positive warning.
    // PRQA S 306 ++ # Suppressed, Defined by nxp library to cast register data. False positive warning.

    const uint8_t fsec = FTFx_FSEC;

    LOG_NOW(LOG_LEVEL_INFO, "Flash Security Register (FSEC): 0x%x", 
            fsec);
    
    LOG_NOW(LOG_LEVEL_INFO, "Backdoor Key Enable (KEYEN): 0x%x", 
            (fsec & FTFx_FSEC_KEYEN_MASK) >> FTFx_FSEC_KEYEN_SHIFT);

    LOG_NOW(LOG_LEVEL_INFO, "Mass Erase Enable (MEEN): 0x%x", 
            (fsec & FTFx_FSEC_MEEN_MASK) >> FTFx_FSEC_MEEN_SHIFT);

    LOG_NOW(LOG_LEVEL_INFO, "Factory Failure Acccess (FSLACC): 0x%x", 
            (fsec & FTFx_FSEC_FSLACC_MASK) >> FTFx_FSEC_FSLACC_SHIFT);

    LOG_NOW(LOG_LEVEL_INFO, "Flash Security (SEC): 0x%x", 
            (fsec & FTFx_FSEC_SEC_MASK) >> FTFx_FSEC_SEC_SHIFT);

    uint32_t pflash_protection;

    FLASH_DRV_GetPFlashProtection(&pflash_protection);

    LOG_NOW(LOG_LEVEL_INFO, "P-Flash Protection (FPROT): 0x%x", 
            pflash_protection);

    const uint8_t fopt = FTFx_FOPT;

    LOG_NOW(LOG_LEVEL_INFO, "Flash Option (FOPT): 0x%x", 
            (fopt & FTFx_FOPT_OPT_MASK) >> FTFx_FOPT_OPT_SHIFT);

    const uint8_t feprot = FTFx_FEPROT;

    LOG_NOW(LOG_LEVEL_INFO, "EEPROM Protection (FEPROT): 0x%x", 
            (feprot & FTFx_FEPROT_EPROT_MASK) >> FTFx_FEPROT_EPROT_SHIFT);

    const uint8_t fdprot = FTFx_FDPROT;

    LOG_NOW(LOG_LEVEL_INFO, "Data Flash Protection (FDPROT): 0x%x", 
            (fdprot & FTFx_FDPROT_DPROT_MASK) >> FTFx_FDPROT_DPROT_SHIFT);

    const uint8_t fcestat = FTFx_FCSESTAT;

    LOG_NOW(LOG_LEVEL_INFO, "Flash CSEc Status (FCSESTAT): 0x%x", 
            fcestat);

    // PRQA S 306 --
    // PRQA S 2985 --
}

/**
 * @brief Reads last reset reason from CPU registers and logs
 *
 */
static void log_last_reset_reason(void)
{
    // PRQA S 306 ++ # Suppressed, Defined by nxp library to cast register data. False positive warning.

    if (POWER_SYS_GetResetSrcStatusCmd(RCM, RCM_LOW_VOLT_DETECT) == true)
    {
        LOG_NOW(LOG_LEVEL_WARNING, "Last reset reason: Low voltage detect");
    }

    if (POWER_SYS_GetResetSrcStatusCmd(RCM, RCM_LOSS_OF_CLK) == true)
    {
        LOG_NOW(LOG_LEVEL_WARNING, "Last reset reason: Loss of clock");
    }

    if (POWER_SYS_GetResetSrcStatusCmd(RCM, RCM_LOSS_OF_LOCK) == true)
    {
        LOG_NOW(LOG_LEVEL_WARNING, "Last reset reason: Loss of lock");
    }

    if (POWER_SYS_GetResetSrcStatusCmd(RCM, RCM_CMU_LOC) == true)
    {
        LOG_NOW(LOG_LEVEL_WARNING, "Last reset reason: CMU Loss of lock");
    }

    if (POWER_SYS_GetResetSrcStatusCmd(RCM, RCM_WATCH_DOG) == true)
    {
        LOG_NOW(LOG_LEVEL_WARNING, "Last reset reason: Watchdog");
    }

    if (POWER_SYS_GetResetSrcStatusCmd(RCM, RCM_EXTERNAL_PIN) == true)
    {
        LOG_NOW(LOG_LEVEL_WARNING, "Last reset reason: External pin");
    }

    if (POWER_SYS_GetResetSrcStatusCmd(RCM, RCM_POWER_ON) == true)
    {
        LOG_NOW(LOG_LEVEL_WARNING, "Last reset reason: Power On Reset");
    }

    if (POWER_SYS_GetResetSrcStatusCmd(RCM, RCM_SJTAG) == true)
    {
        LOG_NOW(LOG_LEVEL_WARNING, "Last reset reason: JTAG reset");
    }

    if (POWER_SYS_GetResetSrcStatusCmd(RCM, RCM_CORE_LOCKUP) == true)
    {
        LOG_NOW(LOG_LEVEL_WARNING, "Last reset reason: Core lockup");
    }

    if (POWER_SYS_GetResetSrcStatusCmd(RCM, RCM_SOFTWARE) == true)
    {
        LOG_NOW(LOG_LEVEL_WARNING, "Last reset reason: Software reset");
    }

    if (POWER_SYS_GetResetSrcStatusCmd(RCM, RCM_SMDM_AP) == true)
    {
        LOG_NOW(LOG_LEVEL_WARNING, "Last reset reason: MDM-AP system reset");
    }

    if (POWER_SYS_GetResetSrcStatusCmd(RCM, RCM_STOP_MODE_ACK_ERR) == true)
    {
        LOG_NOW(LOG_LEVEL_WARNING, "Last reset reason: Stop mode ack error");
    }

    // PRQA S 306 --
}

/**
 * @brief Blocking wait forever for the ready status of flash
 *
 */
static void wait_for_flash_complete(void)
{
    bool flash_ready = false;

    while (!flash_ready)
    {
        flash_ready = FLASH_DRV_GetCmdCompleteFlag();
    }
}

/**
 * @brief Performs software reset
 *
 */
static void software_reset(void)
{
    LOG_NOW(LOG_LEVEL_INFO, "Reseting CPU now");

    bsp_mcu_reset();
}

/**
 * @brief Watchdog reset callback used only for main operations
 *
 */
static void main_wdg_cb(void)
{
    LOG_NOW(LOG_LEVEL_FATAL, "WDG TIMEOUT AT MAIN");

    software_reset();
}

/**
 * @brief Hard-fault reset callback used only for main operations
 *
 */
static void main_hard_fault_cb(void)
{
    LOG_NOW(LOG_LEVEL_FATAL, "HARDFAULT AT MAIN");

    software_reset();
}

// end of file
