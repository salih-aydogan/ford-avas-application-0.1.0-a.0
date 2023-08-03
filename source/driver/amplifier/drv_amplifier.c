// PRQA S 3108 EOF # Comments with links are required for ensure traceability.

/**
 * @file drv_amplifier.c
 * @author Furkan Yildirim (furkan.yildirim@daiichi.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-15
 * 
 * @copyright Copyright (c) 2023
 * 
 * https://codebeamer.daiichi.com/issue/192956
 * 
 */

#include "drv_amplifier.h"

#include "bsp_mcu.h"
#include "bsp_i2c.h"
#include "bsp_gpio.h"

#include "log.h"

LOG_MODULE_NAME("DRV_AMLIFIER");

/*******************************************************************************
 * Macros
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/192996
 * 
 */
#define IB_ADDR                         0x80U

/**
 * @brief https://codebeamer.daiichi.com/issue/192997
 * 
 */
#define IB11_ADDR                       0x0BU

/**
 * @brief https://codebeamer.daiichi.com/issue/192998
 * 
 */
#define IB0_ADDR_SIZE                   1U

/**
 * @brief https://codebeamer.daiichi.com/issue/192999
 * 
 */
#define MUTE_UNMUTE_DATA_SIZE           2U

/**
 * @brief https://codebeamer.daiichi.com/issue/193000
 * 
 */
#define WAIT_TIME_FOR_DIAG              300U

/**
 * @brief https://codebeamer.daiichi.com/issue/193001
 * 
 */
#define DB_SIZE                         11U

/**
 * @brief https://codebeamer.daiichi.com/issue/193002
 * 
 */
#define IB_SIZE                         32U

/**
 * @brief https://codebeamer.daiichi.com/issue/193003
 * 
 */
#define AMPLIFIER_FDA903S_MUTE_UNMUTE   (0x01U << 0U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193004
 * 
 */
#define AMPLIFIER_DIAG_START_BIT_1      (0x01U << 1U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193005
 * 
 */
#define AMPLIFIER_DIAG_START_BIT_2      (0x01U << 2U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193006
 * 
 */
#define DIAG_INPUT_OFFSET_POS           (0x01U << 7U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193007
 * 
 */
#define DIAG_OVERCURR_PROTECT_POS       (0x01U << 6U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193008
 * 
 */
#define DIAG_CLIPPING_DETECT_POS        (0x01U << 3U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193009
 * 
 */
#define DIAG_THERMAL_SD_POS             (0x01U << 0U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193010
 * 
 */
#define DIAG_ANALOG_MUTE_POS            (0x01U << 5U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193011
 * 
 */
#define DIAG_VOV_SD_POS                 (0x01U << 3U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193012
 * 
 */
#define DIAG_OUTCURR_OFFSET_POS         (0x01U << 7U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193013
 * 
 */
#define DIAG_DATA_VALID_POS             (0x01U << 2U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193014
 * 
 */
#define DIAG_PLAY_POS                   (0x01U << 0U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193015
 * 
 */
#define ERROR_SHORT_TO_VCC_POS          (0x01U << 5U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193016
 * 
 */
#define ERROR_SHORT_TO_GND_POS          (0x01U << 4U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193017
 * 
 */
#define ERROR_WDG_POS                   (0x01U << 2U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193018
 * 
 */
#define ERROR_FRAME_CHECK_POS           (0x01U << 1U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193019
 * 
 */
#define ERROR_PLL_LOCKED_POS            (0x01U << 0U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193020
 * 
 */
#define ERROR_TW_1_POS                  (0x01U << 4U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193021
 * 
 */
#define ERROR_TW_2_POS                  (0x01U << 3U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193022
 * 
 */
#define ERROR_TW_4_POS                  (0x01U << 2U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193023
 * 
 */
#define ERROR_UVLO_POS                  (0x01U << 2U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193024
 * 
 */
#define ERROR_LOW_BATT_POS              (0x01U << 1U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193025
 * 
 */
#define ERROR_HIGH_BATT_POS             (0x01U << 0U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193026
 * 
 */
#define ERROR_SHORT_LOAD_POS            (0x01U << 4U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193027
 * 
 */
#define ERROR_OPEN_LOAD_POS             (0x01U << 3U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193028
 * 
 */
#define COMMAND_WRITING_ENABLE          0xEAU

/**
 * @brief https://codebeamer.daiichi.com/issue/193029
 * 
 */
#define SELECT_48KHZ_WS                 (0x0100U << 2U)

// TODO
#define SELECT_32KHZ_WS                 (0x0010U << 2U)

// TODO
#define SELECT_16KHZ_WS                 (0x0001U << 2U)

// TODO
#define SELECT_8KHZ_WS                  (0x0000U << 2U)


/**
 * @brief https://codebeamer.daiichi.com/issue/193030
 * 
 */
#define SELECT_448KHZ_PWM               (0x01U << 0U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193031
 * 
 */
#define I2SDATAOUT_DISABLE              0x00U

/**
 * @brief https://codebeamer.daiichi.com/issue/193032
 * 
 */
#define NO_LIMITER_NO_GAIN_LIN_INTERPOL 0x00U

/**
 * @brief https://codebeamer.daiichi.com/issue/193033
 * 
 */
#define INPUT_OFFSET_DETECTOR_ENABLE    (0x01U << 6U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193034
 * 
 */
#define MUTE_TIME_92MSEC                0x48U

/**
 * @brief https://codebeamer.daiichi.com/issue/193035
 * 
 */
#define NO_VOLUME_DECREASE              0x00U

// TODO
#define VOLUME_DECREASE                 80U //Each step is 0.5dB decrease

/**
 * @brief https://codebeamer.daiichi.com/issue/193036
 * 
 */
#define INPUT_OFFSET_TH_30DB            0x27U

/**
 * @brief https://codebeamer.daiichi.com/issue/193037
 * 
 */
#define SHORT_1_OPEN_15_OHM_TH          0x50U

/**
 * @brief https://codebeamer.daiichi.com/issue/193038
 * 
 */
#define DEFAULT_VALUE_OF_IB9            0x80U

/**
 * @brief https://codebeamer.daiichi.com/issue/193039
 * 
 */
#define OUTCURR_TH_1_A_NO_LCM           0xC0U

/**
 * @brief https://codebeamer.daiichi.com/issue/193040
 * 
 */
#define START_DIAG_30_MSEC              (0x01U << 6U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193041
 * 
 */
#define OFFSET_DETECTOR_ENABLE          (0x01U << 4U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193042
 * 
 */
#define PWM_ON                          (0x01U << 3U)

/**
 * @brief https://codebeamer.daiichi.com/issue/193043
 * 
 */
#define DEFAULT_VALUE_OF_IB12           0x00U

/**
 * @brief https://codebeamer.daiichi.com/issue/193044
 * 
 */
#define DEFAULT_VALUE_OF_IB13           0x00U

/**
 * @brief https://codebeamer.daiichi.com/issue/193045
 * 
 */
#define DEFAULT_VALUE_OF_IB14           0x00U

/**
 * @brief https://codebeamer.daiichi.com/issue/193046
 * 
 */
#define DEFAULT_VALUE_OF_IB15           0x00U

/**
 * @brief https://codebeamer.daiichi.com/issue/193047
 * 
 */
#define DEFAULT_VALUE_OF_IB16           0x02U

/**
 * @brief https://codebeamer.daiichi.com/issue/193048
 * 
 */
#define DEFAULT_VALUE_OF_IB17           0x04U

/**
 * @brief https://codebeamer.daiichi.com/issue/193049
 * 
 */
#define WATCH_DOG_DISABLE               0xADU

/**
 * @brief https://codebeamer.daiichi.com/issue/193050
 * 
 */
#define DEVICE_PROGRAMMED               0x25U

/**
 * @brief https://codebeamer.daiichi.com/issue/193051
 * 
 */
#define DEFAULT_VALUE_OF_IB20           0x11U

/**
 * @brief https://codebeamer.daiichi.com/issue/193052
 * 
 */
#define DEFAULT_VALUE_OF_IB21           0x00U

/**
 * @brief https://codebeamer.daiichi.com/issue/193053
 * 
 */
#define DEFAULT_VALUE_OF_IB22           0x00U

/**
 * @brief https://codebeamer.daiichi.com/issue/193153
 * 
 */
#define DC_DIAG_IN_PLAY_FOR_IB20        0xB8U

/**
 * @brief https://codebeamer.daiichi.com/issue/193154
 * 
 */
#define DC_DIAG_IN_PLAY_FOR_IB22        0x02U

/*******************************************************************************
 * Private Variables 
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/193451
 * 
 */
static drv_amplifier_diag_cb_t m_diag_cb;

/**
 * @brief https://codebeamer.daiichi.com/issue/193452
 * 
 */
static uint32_t m_diag_start_tick;

/**
 * @brief https://codebeamer.daiichi.com/issue/193453
 * 
 */
static uint8_t m_data_bytes_amplifier_1[DB_SIZE];

/**
 * @brief https://codebeamer.daiichi.com/issue/193454
 * 
 */
static uint8_t m_data_bytes_amplifier_2[DB_SIZE];

/**
 * @brief https://codebeamer.daiichi.com/issue/193455
 * 
 */
static uint8_t m_instruction_bytes[IB0_ADDR_SIZE + IB_SIZE];

/**
 * @brief https://codebeamer.daiichi.com/issue/193456
 * 
 */
static bool m_amplifier_is_mute = true;

/*******************************************************************************
 * Private Function Declarations
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/193480
 * 
 * @param[in] channel 
 * @param[in] p_data 
 * @param[in] size 
 * 
 * @return bool Success or fail.
 * 
 * @retval true  Successful.
 * @retval false Possible errors:
 *               -A communication error.
 *               -Invalid argument
 * 
 */
static bool fda903s_write(drv_amplifier_channel_t channel, 
                          const uint8_t *p_data, 
                          size_t size);

/**
 * @brief https://codebeamer.daiichi.com/issue/193485
 * 
 * @param[in] channel 
 * @param[out] p_rx_data 
 * @param[in] rx_size 
 * @param[in] p_tx_data 
 * @param[in] tx_size 
 * 
 * @return bool Success or fail.
 * 
 * @retval true  Successful.
 * @retval false Possible errors:
 *               -A communication error.
 *               -Invalid argument
 * 
 */
static bool fda903s_write_read(drv_amplifier_channel_t channel, 
                               uint8_t *p_rx_data, 
                               size_t rx_size,
                               const uint8_t *p_tx_data, 
                               size_t tx_size);

/**
 * @brief https://codebeamer.daiichi.com/issue/195482
 * 
 * @return bool Success or fail.
 * 
 * @retval true  Successful.
 * @retval false Possible errors:
 *               -A communication error.
 * 
 */
static bool diag_start(void);

/**
 * @brief https://codebeamer.daiichi.com/issue/193479
 * 
 * @param[in] p_diag_data 
 * 
 */
static void amplifier_diag_cb(const uint8_t* const p_diag_data);

/**
 * @brief https://codebeamer.daiichi.com/issue/193481
 * 
 * @param[in] db1_value 
 * 
 */
static void thermal_warning_control(const uint8_t db1_value);

/**
 * @brief https://codebeamer.daiichi.com/issue/193483
 * 
 * @param[in] p_diag_data 
 * 
 */
static void amplifier_error_cb(const uint8_t* const p_diag_data);

/**
 * @brief https://codebeamer.daiichi.com/issue/193478
 * 
 */
static void fda903s_default_values_set(void);

/**
 * @brief https://codebeamer.daiichi.com/issue/193672
 * 
 */
static void fda903s_i2c_port_1_error_cb(bsp_i2c_err_t error);

/**
 * @brief https://codebeamer.daiichi.com/issue/193671
 * 
 */
static void fda903s_i2c_port_2_error_cb(bsp_i2c_err_t error);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/193486
 * 
 */
bool drv_amplifier_init(void)
{    
    (void)memset(m_data_bytes_amplifier_1, 0, DB_SIZE); 

    (void)memset(m_data_bytes_amplifier_2, 0, DB_SIZE); 
    
    (void)memset(m_instruction_bytes, 0, IB0_ADDR_SIZE + IB_SIZE);

    bsp_i2c_cb_set(BSP_I2C_AMP_1_PORT, fda903s_i2c_port_1_error_cb);

    bsp_i2c_cb_set(BSP_I2C_AMP_2_PORT, fda903s_i2c_port_2_error_cb);

    fda903s_default_values_set();

    LOG_DEBUG("Amplifier init values [ 0- 7]; %u %u %u %u  %u %u %u %u",
               m_instruction_bytes[ 0], m_instruction_bytes[ 1],
               m_instruction_bytes[ 2], m_instruction_bytes[ 3],
               m_instruction_bytes[ 4], m_instruction_bytes[ 5],
               m_instruction_bytes[ 6], m_instruction_bytes[ 7]);

    LOG_DEBUG("Amplifier init values [ 8-15]; %u %u %u %u  %u %u %u %u",
               m_instruction_bytes[ 8], m_instruction_bytes[ 9],
               m_instruction_bytes[10], m_instruction_bytes[11],
               m_instruction_bytes[12], m_instruction_bytes[13],
               m_instruction_bytes[14], m_instruction_bytes[15]);

    LOG_DEBUG("Amplifier init values [16-23]; %u %u %u %u  %u %u %u %u",
               m_instruction_bytes[16], m_instruction_bytes[17],
               m_instruction_bytes[18], m_instruction_bytes[19],
               m_instruction_bytes[20], m_instruction_bytes[21],
               m_instruction_bytes[22], m_instruction_bytes[23]);

    bool amp_write_ret = fda903s_write(DRV_AMPLIFIER_CHANNEL_1, 
                                       m_instruction_bytes, 
                                       IB0_ADDR_SIZE + IB_SIZE);

    if (false == amp_write_ret)
    {
        LOG_ERROR("CH-1 Init Failed");

        return false;
    }                                     
                                  
    amp_write_ret = fda903s_write(DRV_AMPLIFIER_CHANNEL_2, 
                                  m_instruction_bytes, 
                                  IB0_ADDR_SIZE + IB_SIZE);

    if (false == amp_write_ret)
    {
        LOG_ERROR("CH-2 Init Failed");

        return false;
    }

    m_amplifier_is_mute = true; 

    m_diag_cb = NULL;

    LOG_INFO("Initialized");

    return true;
}

/**
 * @brief  https://codebeamer.daiichi.com/issue/193488
 *  
 */
bool drv_amplifier_mute(void)
{
    if (true == m_amplifier_is_mute)
    {
        return true;
    }
    
    m_instruction_bytes[IB0_ADDR_SIZE + 11U] &= 
        (uint8_t)~AMPLIFIER_FDA903S_MUTE_UNMUTE;

    uint8_t m_mute_data[2U] = {IB11_ADDR, 0U};

    m_mute_data[1U] = m_instruction_bytes[IB0_ADDR_SIZE + 11U];

    bool amp_write_ret = fda903s_write(DRV_AMPLIFIER_CHANNEL_1, 
                                       m_mute_data, 
                                       MUTE_UNMUTE_DATA_SIZE);

    if (false == amp_write_ret) 
    {
        LOG_ERROR("CH-1 mute failed");

        m_amplifier_is_mute = false;

        return false;
    }

    amp_write_ret = fda903s_write(DRV_AMPLIFIER_CHANNEL_2, 
                                  m_mute_data, 
                                  MUTE_UNMUTE_DATA_SIZE);

    if (false == amp_write_ret) 
    {
        LOG_ERROR("CH-2 mute failed");

        m_amplifier_is_mute = false;
                  
        return false;
    }
    else
    {
        m_amplifier_is_mute = true;

        bsp_gpio_reset(BSP_GPIO_AMP_MUTE_1_OUT_PIN_NO);
        
        bsp_gpio_reset(BSP_GPIO_AMP_MUTE_2_OUT_PIN_NO);

        return true; 
    }    
}

/**
 * @brief  https://codebeamer.daiichi.com/issue/193487
 *
 */
bool drv_amplifier_unmute(void) 
{
    if (false == m_amplifier_is_mute)
    {
        return true;
    }
    
    m_instruction_bytes[IB0_ADDR_SIZE + 11U] |= AMPLIFIER_FDA903S_MUTE_UNMUTE;

    uint8_t m_unmute_data[2U] = {IB11_ADDR, 0U};

    m_unmute_data[1U] = m_instruction_bytes[IB0_ADDR_SIZE + 11U];

    bool amp_write_ret = fda903s_write(DRV_AMPLIFIER_CHANNEL_1, 
                                       m_unmute_data, 
                                       MUTE_UNMUTE_DATA_SIZE);

    if (false == amp_write_ret) 
    {
        LOG_ERROR("CH-1 unmute failed");

        m_amplifier_is_mute = true;

        return false;
    }

    amp_write_ret = fda903s_write(DRV_AMPLIFIER_CHANNEL_2, 
                                  m_unmute_data, 
                                  MUTE_UNMUTE_DATA_SIZE);

    if (false == amp_write_ret) 
    {
        LOG_ERROR("CH-2 unmute failed");

        m_amplifier_is_mute = true;
                  
        return false;
    }
    else
    {
        m_amplifier_is_mute = false;


        bsp_gpio_set(BSP_GPIO_AMP_MUTE_1_OUT_PIN_NO);
        
        bsp_gpio_set(BSP_GPIO_AMP_MUTE_2_OUT_PIN_NO);

        return true;
    }
}

/**
 * @brief  https://codebeamer.daiichi.com/issue/193484
 *
 */
bool drv_amplifier_process(void) 
{
    uint32_t mcu_tick = bsp_mcu_tick_get();

    uint32_t elapsed_time = (mcu_tick >= m_diag_start_tick)
                            ? (mcu_tick - m_diag_start_tick)
                            : ((UINT32_MAX - m_diag_start_tick) + mcu_tick);

    if (elapsed_time < WAIT_TIME_FOR_DIAG)
    {
        return true;
    }

    uint8_t diag_data_amplifier_1[DB_SIZE] = {0U};
    uint8_t diag_data_amplifier_2[DB_SIZE] = {0U}; 

    const uint8_t m_DB_ADDR = 0xA0U;
 
    bool amp_write_ret = fda903s_write_read(DRV_AMPLIFIER_CHANNEL_1,
                                            diag_data_amplifier_1,
                                            DB_SIZE,
                                            &m_DB_ADDR,
                                            1U);
         
    if (false == amp_write_ret)
    {
        LOG_ERROR("Diag data not reading CH-1");

        return false;
    }
         
    amp_write_ret = fda903s_write_read(DRV_AMPLIFIER_CHANNEL_2,
                                       diag_data_amplifier_2,
                                       DB_SIZE,
                                       &m_DB_ADDR,
                                       1U);
         
    if (false == amp_write_ret)
    {
        LOG_ERROR("Diag data not reading CH-2");

        return false;
    }

    m_diag_start_tick = bsp_mcu_tick_get();

    if ((0U != (diag_data_amplifier_1[3U] & DIAG_DATA_VALID_POS)) && 
        (0U != (diag_data_amplifier_2[3U] & DIAG_DATA_VALID_POS)))
    {
        LOG_VERBOSE("Diag data valid");

        (void)memcpy(m_data_bytes_amplifier_1, 
                     diag_data_amplifier_1, 
                     DB_SIZE);
                             
        (void)memcpy(m_data_bytes_amplifier_2, 
                     diag_data_amplifier_2, 
                     DB_SIZE);

        LOG_VERBOSE("m_data_bytes_amplifier_1: %d", 
                    m_data_bytes_amplifier_1[0U],
                    m_data_bytes_amplifier_1[1U],
                    m_data_bytes_amplifier_1[2U],
                    m_data_bytes_amplifier_1[3U]);

        amplifier_diag_cb(m_data_bytes_amplifier_1);

        amplifier_error_cb(m_data_bytes_amplifier_1);

        LOG_VERBOSE("m_data_bytes_amplifier_2: %d", 
                    m_data_bytes_amplifier_2[0U],
                    m_data_bytes_amplifier_2[1U],
                    m_data_bytes_amplifier_2[2U],
                    m_data_bytes_amplifier_2[3U]);

        amplifier_diag_cb(m_data_bytes_amplifier_2);

        amplifier_error_cb(m_data_bytes_amplifier_2);

        bool diag_start_ret = diag_start();

        if (false == diag_start_ret)
        {
            return false;
        }
    }
    else
    {
        LOG_VERBOSE("Diag data not valid");
    }

    return true;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/193489
 * 
 */
void drv_amplifier_diag_cb_set(drv_amplifier_diag_cb_t callback)
{
    if (NULL == callback)
    {
        LOG_ERROR("Invalid argument in drv_amplifier_diag_cb_set");

        return;
    }

    m_diag_cb = callback;

    return; 
}

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

static bool fda903s_write(drv_amplifier_channel_t channel, 
                          const uint8_t *p_data, 
                          size_t size)
{
    bool bsp_ret = false;
    
    uint32_t m_channel = (channel == DRV_AMPLIFIER_CHANNEL_1)
                         ? (BSP_I2C_AMP_1_PORT) 
                         : (BSP_I2C_AMP_2_PORT);

    bsp_ret = bsp_i2c_write(m_channel, p_data, size);

    if (false == bsp_ret)
    {
        for (uint32_t idx = 0U; ((idx < 4U) && (!bsp_ret)); ++idx)
        {
            bsp_mcu_delay(10U);

            bsp_ret = bsp_i2c_write(m_channel, p_data, size);
        }
    } 

    return bsp_ret;
}

static bool fda903s_write_read(drv_amplifier_channel_t channel, 
                               uint8_t *p_rx_data, 
                               size_t rx_size,
                               const uint8_t *p_tx_data, 
                               size_t tx_size)
{
    bool bsp_ret = false;

    uint32_t m_channel = (channel == DRV_AMPLIFIER_CHANNEL_1)
                         ? (BSP_I2C_AMP_1_PORT) 
                         : (BSP_I2C_AMP_2_PORT);

    bsp_ret = bsp_i2c_write_read(m_channel, 
                                 p_rx_data,
                                 rx_size,  
                                 p_tx_data,
                                 tx_size); 

    if (false == bsp_ret)
    {
        for (uint32_t idx = 0U; ((idx < 4U) && (!bsp_ret)); ++idx)
        {
            bsp_mcu_delay(10U);

            bsp_ret = bsp_i2c_write_read(m_channel, 
                                         p_rx_data,
                                         rx_size,  
                                         p_tx_data,
                                         tx_size); 
        }
    }

    return bsp_ret;
}

static bool diag_start(void)
{
    if (true == m_amplifier_is_mute)
    {
        m_instruction_bytes[IB0_ADDR_SIZE + 11U] |= 
            (uint8_t)AMPLIFIER_DIAG_START_BIT_1;

        m_instruction_bytes[IB0_ADDR_SIZE + 11U] &= 
            (uint8_t)~AMPLIFIER_DIAG_START_BIT_2;

        m_instruction_bytes[21U] = DEFAULT_VALUE_OF_IB20;
        m_instruction_bytes[23U] = DEFAULT_VALUE_OF_IB22;
    }
    else
    {
        m_instruction_bytes[IB0_ADDR_SIZE + 11U] &= 
            (uint8_t)~AMPLIFIER_DIAG_START_BIT_1;
            
        m_instruction_bytes[IB0_ADDR_SIZE + 11U] |= 
            (uint8_t)AMPLIFIER_DIAG_START_BIT_2;

        m_instruction_bytes[21U] = (uint8_t)DC_DIAG_IN_PLAY_FOR_IB20;
        m_instruction_bytes[23U] = (uint8_t)DC_DIAG_IN_PLAY_FOR_IB22;
    }
    
    bool amp_write_ret = fda903s_write(DRV_AMPLIFIER_CHANNEL_1, 
                                       m_instruction_bytes, 
                                       IB0_ADDR_SIZE + IB_SIZE);
    
    if (false == amp_write_ret)
    {
        LOG_ERROR("Diag not started for amplifier 1");
        
        return false;
    }
    else
    {
        LOG_DEBUG("Diagnostic start again for amplifier 1");
    }
    
    amp_write_ret = fda903s_write(DRV_AMPLIFIER_CHANNEL_2, 
                                  m_instruction_bytes, 
                                  IB0_ADDR_SIZE + IB_SIZE);
    
    if (false == amp_write_ret)
    {
        LOG_ERROR("Diag not started for amplifier 2");
        
        return false;
    }
    else
    {
        LOG_DEBUG("Diagnostic start again for amplifier 2");
    }

    return true;        
}

static void amplifier_diag_cb(const uint8_t* const p_diag_data)
{
    if (0U != (p_diag_data[0U] & DIAG_INPUT_OFFSET_POS))
    {
        m_diag_cb(DRV_AMPLIFIER_DIAG_INPUT_OFFSET); 
    } 

    if (0U != (p_diag_data[0U] & DIAG_OVERCURR_PROTECT_POS))
    {
        m_diag_cb(DRV_AMPLIFIER_DIAG_OVERCURR_PROTECT); 
    } 

    if (0U != (p_diag_data[0U] & DIAG_CLIPPING_DETECT_POS))
    {
        m_diag_cb(DRV_AMPLIFIER_DIAG_CLIPPING_DETECT); 
    } 

    if (0U != (p_diag_data[1U] & DIAG_THERMAL_SD_POS))
    {
        m_diag_cb(DRV_AMPLIFIER_DIAG_THERMAL_SD); 
    } 

    if (0U != (p_diag_data[2U] & DIAG_ANALOG_MUTE_POS))
    {
        m_diag_cb(DRV_AMPLIFIER_DIAG_ANALOG_MUTE); 
    } 

    if (0U != (p_diag_data[2U] & DIAG_VOV_SD_POS))
    {
        m_diag_cb(DRV_AMPLIFIER_DIAG_VOV_SD); 
    } 

    if (0U != (p_diag_data[3U] & DIAG_OUTCURR_OFFSET_POS))
    {
        m_diag_cb(DRV_AMPLIFIER_DIAG_OUTCURR_OFFSET); 
    } 

    if (0U != (p_diag_data[3U] & DIAG_DATA_VALID_POS))
    {
        m_diag_cb(DRV_AMPLIFIER_DIAG_DATA_VALID); 
    } 

    if (0U != (p_diag_data[3U] & DIAG_PLAY_POS))
    {
        m_diag_cb(DRV_AMPLIFIER_DIAG_PLAY); 
    } 
}

static void thermal_warning_control(const uint8_t db1_value)
{
    if (0U != (db1_value & ERROR_TW_1_POS))
    {
        m_diag_cb(DRV_AMPLIFIER_DIAG_ERR_TW_1); 
    }

    if (0U == (db1_value & ERROR_TW_4_POS))
    {
        m_diag_cb(DRV_AMPLIFIER_DIAG_ERR_TW_4); 
    }

    if ((0U == (db1_value & ERROR_TW_1_POS)) && 
        (0U != (db1_value & ERROR_TW_4_POS)))
    {
        if (0U != (db1_value & ERROR_TW_2_POS))
        {
            m_diag_cb(DRV_AMPLIFIER_DIAG_ERR_TW_2); 
        }
        else
        {
            m_diag_cb(DRV_AMPLIFIER_DIAG_ERR_TW_3); 
        }
    }
}

static void amplifier_error_cb(const uint8_t* const p_diag_data)
{
    if (0U != (p_diag_data[0U] & ERROR_SHORT_TO_VCC_POS))
    {
        m_diag_cb(DRV_AMPLIFIER_DIAG_ERR_SHORT_TO_VCC); 
    }

    if (0U != (p_diag_data[0U] & ERROR_SHORT_TO_GND_POS))
    {
        m_diag_cb(DRV_AMPLIFIER_DIAG_ERR_SHORT_TO_GND); 
    } 

    if ((0U != (p_diag_data[0U] & ERROR_WDG_POS)) ||
        (0U != (p_diag_data[0U] & ERROR_FRAME_CHECK_POS)) ||
        (0U == (p_diag_data[0U] & ERROR_PLL_LOCKED_POS)))
    {
        m_diag_cb(DRV_AMPLIFIER_DIAG_ERR_COMMUNICATION); 
    }

    thermal_warning_control(p_diag_data[1U]);

    if (0U != (p_diag_data[2U] & ERROR_UVLO_POS))
    {
        m_diag_cb(DRV_AMPLIFIER_DIAG_ERR_UVLO); 
    }

    if (0U != (p_diag_data[2U] & ERROR_LOW_BATT_POS))
    {
        m_diag_cb(DRV_AMPLIFIER_DIAG_ERR_LOW_BATT); 
    }

    if (0U != (p_diag_data[2U] & ERROR_HIGH_BATT_POS))
    {
        m_diag_cb(DRV_AMPLIFIER_DIAG_ERR_HIGH_BATT); 
    }

    if (0U != (p_diag_data[3U] & ERROR_SHORT_LOAD_POS))
    {
        m_diag_cb(DRV_AMPLIFIER_DIAG_ERR_SHORT_LOAD); 
    }

    if (0U != (p_diag_data[3U] & ERROR_OPEN_LOAD_POS))
    {
        m_diag_cb(DRV_AMPLIFIER_DIAG_ERR_OPEN_LOAD); 
    }  
}

static void fda903s_default_values_set(void)
{
    m_instruction_bytes[0U] = IB_ADDR;
    m_instruction_bytes[IB0_ADDR_SIZE + 0U] = COMMAND_WRITING_ENABLE;
    m_instruction_bytes[IB0_ADDR_SIZE + 1U] |= SELECT_16KHZ_WS;
    m_instruction_bytes[IB0_ADDR_SIZE + 1U] |= SELECT_448KHZ_PWM;
    m_instruction_bytes[IB0_ADDR_SIZE + 2U] = I2SDATAOUT_DISABLE;
    m_instruction_bytes[IB0_ADDR_SIZE + 3U] = NO_LIMITER_NO_GAIN_LIN_INTERPOL;
    m_instruction_bytes[IB0_ADDR_SIZE + 4U] |= INPUT_OFFSET_DETECTOR_ENABLE;
    m_instruction_bytes[IB0_ADDR_SIZE + 5U] = MUTE_TIME_92MSEC; 
    m_instruction_bytes[IB0_ADDR_SIZE + 6U] = VOLUME_DECREASE; 
    m_instruction_bytes[IB0_ADDR_SIZE + 7U] = INPUT_OFFSET_TH_30DB; 
    m_instruction_bytes[IB0_ADDR_SIZE + 8U] = SHORT_1_OPEN_15_OHM_TH; 
    m_instruction_bytes[IB0_ADDR_SIZE + 9U] = DEFAULT_VALUE_OF_IB9; 
    m_instruction_bytes[IB0_ADDR_SIZE + 10U] = OUTCURR_TH_1_A_NO_LCM; 
    m_instruction_bytes[IB0_ADDR_SIZE + 11U] |= START_DIAG_30_MSEC; 
    m_instruction_bytes[IB0_ADDR_SIZE + 11U] |= OFFSET_DETECTOR_ENABLE; 
    m_instruction_bytes[IB0_ADDR_SIZE + 11U] |= PWM_ON; 
    m_instruction_bytes[IB0_ADDR_SIZE + 12U] = DEFAULT_VALUE_OF_IB12; 
    m_instruction_bytes[IB0_ADDR_SIZE + 13U] = DEFAULT_VALUE_OF_IB13; 
    m_instruction_bytes[IB0_ADDR_SIZE + 14U] = DEFAULT_VALUE_OF_IB14; 
    m_instruction_bytes[IB0_ADDR_SIZE + 15U] = DEFAULT_VALUE_OF_IB15; 
    m_instruction_bytes[IB0_ADDR_SIZE + 16U] = DEFAULT_VALUE_OF_IB16; 
    m_instruction_bytes[IB0_ADDR_SIZE + 17U] = DEFAULT_VALUE_OF_IB17; 
    m_instruction_bytes[IB0_ADDR_SIZE + 18U] = WATCH_DOG_DISABLE; 
    m_instruction_bytes[IB0_ADDR_SIZE + 19U] = DEVICE_PROGRAMMED; 
    m_instruction_bytes[IB0_ADDR_SIZE + 20U] = DEFAULT_VALUE_OF_IB20; 
    m_instruction_bytes[IB0_ADDR_SIZE + 21U] = DEFAULT_VALUE_OF_IB21; 
    m_instruction_bytes[IB0_ADDR_SIZE + 22U] = DEFAULT_VALUE_OF_IB22; 
    m_instruction_bytes[IB0_ADDR_SIZE + 23U] =  0x9E;
    m_instruction_bytes[IB0_ADDR_SIZE + 24U] =  0x00;
    m_instruction_bytes[IB0_ADDR_SIZE + 25U] =  0x00;
    m_instruction_bytes[IB0_ADDR_SIZE + 26U] =  0xB9;
    m_instruction_bytes[IB0_ADDR_SIZE + 27U] =  0x51;
    m_instruction_bytes[IB0_ADDR_SIZE + 28U] =  0x46;
    m_instruction_bytes[IB0_ADDR_SIZE + 29U] =  0x00;
    m_instruction_bytes[IB0_ADDR_SIZE + 30U] =  0x08;
    m_instruction_bytes[IB0_ADDR_SIZE + 31U] =  0x0C;
}

static void fda903s_i2c_port_1_error_cb(bsp_i2c_err_t error)
{
    if (BSP_I2C_STATUS_ERR_RECEIVED_NACK == error)
    {
        LOG_ERROR("BSP_I2C_STATUS_ERR_RECEIVED_NACK");
    }
       
    if (BSP_I2C_STATUS_ERR_TX_UNDERRUN == error)
    {
        LOG_ERROR("BSP_I2C_STATUS_ERR_TX_UNDERRUN");
    }
       
    if (BSP_I2C_STATUS_ERR_RX_OVERRUN == error)
    {
        LOG_ERROR("BSP_I2C_STATUS_ERR_RX_OVERRUN");
    }
       
    if (BSP_I2C_STATUS_ERR_ARBITRATION_LOST == error)
    {
        LOG_ERROR("BSP_I2C_STATUS_ERR_ARBITRATION_LOST");
    }
       
    if (BSP_I2C_STATUS_ERR_ABORTED == error)
    {
        LOG_ERROR("BSP_I2C_STATUS_ERR_ABORTED");
    }
       
    if (BSP_I2C_STATUS_ERR_BUS_BUSY == error)
    {
        LOG_ERROR("BSP_I2C_STATUS_ERR_BUS_BUSY");
    }
}

static void fda903s_i2c_port_2_error_cb(bsp_i2c_err_t error)
{
    if (BSP_I2C_STATUS_ERR_RECEIVED_NACK == error)
    {
        LOG_ERROR("BSP_I2C_STATUS_ERR_RECEIVED_NACK");
    }

    if (BSP_I2C_STATUS_ERR_TX_UNDERRUN == error)
    {
        LOG_ERROR("BSP_I2C_STATUS_ERR_TX_UNDERRUN");
    }

    if (BSP_I2C_STATUS_ERR_RX_OVERRUN == error)
    {
        LOG_ERROR("BSP_I2C_STATUS_ERR_RX_OVERRUN");
    }

    if (BSP_I2C_STATUS_ERR_ARBITRATION_LOST == error)
    {
        LOG_ERROR("BSP_I2C_STATUS_ERR_ARBITRATION_LOST");
    }

    if (BSP_I2C_STATUS_ERR_ABORTED == error)
    {
        LOG_ERROR("BSP_I2C_STATUS_ERR_ABORTED");
    }

    if (BSP_I2C_STATUS_ERR_BUS_BUSY == error)
    {
        LOG_ERROR("BSP_I2C_STATUS_ERR_BUS_BUSY");
    }
}

// end of file
