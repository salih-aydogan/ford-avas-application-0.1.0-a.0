// PRQA S 3108 EOF # Comments with links are required for ensure traceability.
/**
 * @file mcu.c
 * @author Furkan Yildirim (furkan.yildirim@daiichi.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-26
 * 
 * @copyright Copyright (c) 2023
 * 
 * https://codebeamer.daiichi.com/issue/180925
 * 
 */

#include "bsp_mcu.h"

#include "clock.h"
#include "osif.h"
#include "S32K116/include/S32K116.h"
#include "S32K116/include/S32K116_features.h"

#include "log.h"

LOG_MODULE_NAME("BSP_MCU");

/*******************************************************************************             
 * Macros
 ******************************************************************************/

// PRQA S 380 ++ # Too many macros in the driver provided by NXP causes this error.

/**
 * @brief Index of target clock configuration
 * 
 * https://codebeamer.daiichi.com/issue/180932
 * 
*/
#define CLOCK_CFG_IDX           0U

/**
 * @brief Count of clock manager configuration
 * 
 * https://codebeamer.daiichi.com/issue/180933
 * 
*/
#define CLOCK_CFG_CNT           1U     

/**
 * @brief Count of clock manager callback count
 * 
 * https://codebeamer.daiichi.com/issue/180934
 * 
*/
#define CLOCK_CB_CNT            0U

/**
 * @brief Peripheral clock count
 * 
 * https://codebeamer.daiichi.com/issue/183201
 * 
 */
#define NUM_OF_PERIP_CLOCKS     15U

// PRQA S 380 --

/*******************************************************************************
 * Private Variables 
 ******************************************************************************/

/**
 * @brief Array of peripheral clock configurations
 * Purpose of this array is to be argument for m_clockMan1_InitConfig0
 * 
 * https://codebeamer.daiichi.com/issue/181939
 * 
 */
static peripheral_clock_config_t m_peripheral_clock_cfg[NUM_OF_PERIP_CLOCKS] = {
    {
        .clockName = PORTA_CLK,
        .clkGate = true,
        .clkSrc = CLK_SRC_OFF,
        .frac = MULTIPLY_BY_ONE,
        .divider = DIVIDE_BY_ONE,
    },
    {
        .clockName = PORTB_CLK,
        .clkGate = true,
        .clkSrc = CLK_SRC_OFF,
        .frac = MULTIPLY_BY_ONE,
        .divider = DIVIDE_BY_ONE,
    },
    {
        .clockName = PORTC_CLK,
        .clkGate = true,
        .clkSrc = CLK_SRC_OFF,
        .frac = MULTIPLY_BY_ONE,
        .divider = DIVIDE_BY_ONE,
    },
    {
        .clockName = PORTD_CLK,
        .clkGate = true,
        .clkSrc = CLK_SRC_OFF,
        .frac = MULTIPLY_BY_ONE,
        .divider = DIVIDE_BY_ONE,
    },
    {
        .clockName = PORTE_CLK,
        .clkGate = true,
        .clkSrc = CLK_SRC_OFF,
        .frac = MULTIPLY_BY_ONE,
        .divider = DIVIDE_BY_ONE,
    },
    {
        .clockName = FTFC0_CLK,
        .clkGate = true,
        .clkSrc = CLK_SRC_OFF,
        .frac = MULTIPLY_BY_ONE,
        .divider = DIVIDE_BY_ONE,
    },    
    {
        .clockName = LPSPI0_CLK,
        .clkGate = true,
        .clkSrc = CLK_SRC_SIRC_DIV2,
        .frac = MULTIPLY_BY_ONE,
        .divider = DIVIDE_BY_ONE,
    },
    {
        .clockName = LPUART0_CLK,
        .clkGate = true,
        .clkSrc = CLK_SRC_SOSC_DIV2,
        .frac = MULTIPLY_BY_ONE,
        .divider = DIVIDE_BY_ONE,
    },
    {
        .clockName = LPI2C0_CLK,
        .clkGate = true,
        .clkSrc = CLK_SRC_SIRC_DIV2,
        .frac = MULTIPLY_BY_ONE,
        .divider = DIVIDE_BY_ONE,
    },
    {
        .clockName = LPIT0_CLK,
        .clkGate = true,
        .clkSrc = CLK_SRC_SIRC_DIV2,
        .frac = MULTIPLY_BY_ONE,
        .divider = DIVIDE_BY_ONE,
    },
    {
        .clockName = FLEXIO0_CLK,
        .clkGate = true,
        .clkSrc = CLK_SRC_SIRC_DIV2,
        .frac = MULTIPLY_BY_ONE,
        .divider = DIVIDE_BY_ONE,
    },
    {
        .clockName = DMAMUX0_CLK,
        .clkGate = true,
        .clkSrc = CLK_SRC_OFF,
        .frac = MULTIPLY_BY_ONE,
        .divider = DIVIDE_BY_ONE,
    },
    {
        .clockName = FlexCAN0_CLK,
        .clkGate = true,
        .clkSrc = CLK_SRC_OFF,
        .frac = MULTIPLY_BY_ONE,
        .divider = DIVIDE_BY_ONE,
    },
    {
        .clockName = ADC0_CLK,
        .clkGate = true,
        .clkSrc = CLK_SRC_SIRC_DIV2,
        .frac = MULTIPLY_BY_ONE,
        .divider = DIVIDE_BY_ONE,
    },
    {
        .clockName = PDB0_CLK,
        .clkGate = true,
        .clkSrc = CLK_SRC_SOSC,
        .frac = MULTIPLY_BY_ONE,
        .divider = DIVIDE_BY_ONE,
    },   

};

/**
 * @brief Purpose of this array is to configure clock peripheral of mcu.
 * This array will be argument to CLOCK_SYS_Init function.
 * 
 * https://codebeamer.daiichi.com/issue/180954
 * 
 */
static const clock_manager_user_config_t m_clock_mgr_init_cfg = {
    .scgConfig = 
    {
        .sircConfig = 
        {
            .initialize = true,
            .enableInStop = true,                 
            .enableInLowPower = true,                                                                                                                       
            .locked = false,                                                    
            .range = SCG_SIRC_RANGE_HIGH,                           
            .div1 = SCG_ASYNC_CLOCK_DIV_BY_1,     
            .div2 = SCG_ASYNC_CLOCK_DIV_BY_1,     
        },
        .fircConfig = 
        {
            .initialize = true,
            .regulator = true,                  
            .locked = false,                    
            .range = SCG_FIRC_RANGE_48M,        
            .div1 = SCG_ASYNC_CLOCK_DIV_BY_1,   
            .div2 = SCG_ASYNC_CLOCK_DIV_BY_1,   
        },
        .rtcConfig = 
        {
            .initialize = false,
        },
        .soscConfig = 
        {
            .initialize = true,
            .freq = BSP_MCU_CFG_FREQ_HZ,                                                
            .monitorMode = SCG_SOSC_MONITOR_DISABLE,    
            .locked = false,                                                            
            .extRef = SCG_SOSC_REF_OSC,                                                 
            .gain = SCG_SOSC_GAIN_LOW,                                                  
            .range = SCG_SOSC_RANGE_HIGH,               
            .div1 = SCG_ASYNC_CLOCK_DIV_BY_1,                                                   
            .div2 = SCG_ASYNC_CLOCK_DIV_BY_1,                                                       
        },
        .clockOutConfig = 
        {
            .initialize = true,
            .source = SCG_CLOCKOUT_SRC_FIRC, 
        },
        .clockModeConfig = 
        {
            .initialize = true,
            .rccrConfig = 
            {
                .src = SCG_SYSTEM_CLOCK_SRC_SYS_OSC, 
                .divCore = SCG_SYSTEM_CLOCK_DIV_BY_1,
                .divBus = SCG_SYSTEM_CLOCK_DIV_BY_1,
                .divSlow = SCG_SYSTEM_CLOCK_DIV_BY_2,
            },
            .vccrConfig = 
            {
                .src = SCG_SYSTEM_CLOCK_SRC_SIRC, 
                .divCore = SCG_SYSTEM_CLOCK_DIV_BY_2,
                .divBus = SCG_SYSTEM_CLOCK_DIV_BY_1,
                .divSlow = SCG_SYSTEM_CLOCK_DIV_BY_4,
            },
        },
    },
    .pccConfig = 
    {
        .peripheralClocks = m_peripheral_clock_cfg, 
        .count = NUM_OF_PERIP_CLOCKS,
    },
    .simConfig = 
    {
        .clockOutConfig = 
        {
            .initialize = true, 
            .enable = true,                   
            .source = SIM_CLKOUT_SEL_SYSTEM_SCG_CLKOUT,
            .divider = SIM_CLKOUT_DIV_BY_1,      
        },
        .lpoClockConfig = 
        {
            .initialize = true, 
            .enableLpo1k = true, 
            .enableLpo32k = true, 
            .sourceLpoClk = SIM_LPO_CLK_SEL_LPO_128K,
            .sourceRtcClk = SIM_RTCCLK_SEL_FIRCDIV1_CLK,
        },
        .platGateConfig = 
        {
            .initialize = true, 
            .enableEim = true,  
            .enableErm = true,  
            .enableDma = true,  
            .enableMpu = true,  
            .enableMscm = true, 
        },
        .tclkConfig = 
        {
            .initialize = false, 
        },
        .traceClockConfig = 
        {
            .initialize = true, 
            .divEnable = true, 
            .source = CLOCK_TRACE_SRC_CORE_CLK, 
            .divider = 0U, 
            .divFraction = false, 
        },
    },
    .pmcConfig = 
    {
        .lpoClockConfig = 
        {
        .initialize = true, 
        .enable = true, 
        .trimValue = 0,
        },
    },
};

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/180807
 * 
 */
bsp_mcu_status_t bsp_mcu_init(void)
{
    clock_manager_callback_user_config_t *m_clock_mgr_callbacks_arr[] =
    {
        (void *)0
    };
    
    clock_manager_user_config_t const *m_clock_mgr_cfg_arr[] =
    {
        &m_clock_mgr_init_cfg
    };

    status_t status_ret = CLOCK_SYS_Init(m_clock_mgr_cfg_arr,
                                         CLOCK_CFG_CNT, 
                                         m_clock_mgr_callbacks_arr, 
                                         CLOCK_CB_CNT);
    
    if (STATUS_SUCCESS != status_ret)
    {
        LOG_ERROR("Init Fail, status:%u", status_ret);
    }

    if (STATUS_MCU_TRANSITION_FAILED == status_ret)
    {
        return BSP_MCU_STATUS_TRANSITION_FAILED;
    }

    if (STATUS_MCU_INVALID_STATE == status_ret)
    {
        return BSP_MCU_STATUS_INVALID;
    }

    if (STATUS_SUCCESS != status_ret)
    {
        return BSP_MCU_STATUS_ERROR;
    }

    status_ret = CLOCK_SYS_UpdateConfiguration(CLOCK_CFG_IDX,
                                               CLOCK_MANAGER_POLICY_AGREEMENT);

    if (STATUS_SUCCESS != status_ret)
    {
        LOG_ERROR("Init Fail, status:%u", status_ret);
    }

    if (STATUS_MCU_TRANSITION_FAILED == status_ret)
    {
        return BSP_MCU_STATUS_TRANSITION_FAILED;
    }

    if (STATUS_MCU_INVALID_STATE == status_ret)
    {
        return BSP_MCU_STATUS_INVALID;
    }

    if (STATUS_SUCCESS != status_ret)
    {
        return BSP_MCU_STATUS_ERROR;
    }

    osif_systick_init();

    LOG_INFO("Initialized");

    LOG_INFO("Clock Freq: %u", 
             m_clock_mgr_init_cfg.scgConfig.soscConfig.freq);

    return BSP_MCU_STATUS_OK;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/180806
 * 
 */
void bsp_mcu_delay(uint32_t delay)
{
    OSIF_TimeDelay(delay);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/180808
 * 
 */
uint32_t bsp_mcu_tick_get(void)
{
    return OSIF_GetMilliseconds();
}

/**
 * @brief https://codebeamer.daiichi.com/issue/180809
 * 
 */
void bsp_mcu_reset(void)
{
    LOG_NOW(LOG_LEVEL_WARNING, "MCU reset");

    uint32_t regValue = 0;

    // PRQA S 306 ++ # This struct and its content is used in driver layer provides from NXP and so cannot be hidden.
    regValue = S32_SCB->AIRCR;
    // PRQA S 306 --

    regValue &= ~(S32_SCB_AIRCR_VECTKEY_MASK);

    regValue |= S32_SCB_AIRCR_VECTKEY(FEATURE_SCB_VECTKEY);
    regValue |= S32_SCB_AIRCR_SYSRESETREQ(0x1u);
    
    // PRQA S 306 ++ # This struct and its content is used in driver layer provides from NXP and so cannot be hidden.
    S32_SCB->AIRCR = regValue; 
    // PRQA S 306 --
}

// end of file
