// PRQA S 3108 EOF # Comments with links are required for ensure traceability.
/**
 * @file bsp_adc.c
 * @author Berke Yalcinkaya (berke.yalcinkaya@daiichi.com)
 * @brief This module handles analog to digital conversion.
 * @version 0.1
 * @date 2023-05-04
 * 
 * @copyright Copyright (c) 2023
 *
 * https://codebeamer.daiichi.com/issue/191470
 * 
 */

#include "bsp_adc.h"

#include "adc_driver.h"
#include "edma_driver.h"
#include "pdb_driver.h"
#include "pins_driver.h"

#include "log.h"

LOG_MODULE_NAME("BSP_ADC");

/*******************************************************************************
 * Macros
 ******************************************************************************/
// PRQA S 380 ++ # Too many macros in the driver provided by NXP causes this error.
/**
 * @brief This macro indicates which adc unit to be used.
 * 
 * https://codebeamer.daiichi.com/issue/191561
 * 
 */
#define ADC_INST (0U)

/**
 * @brief This macro indicates which pdb unit to be used.
 * 
 * https://codebeamer.daiichi.com/issue/191562
 * 
 */
#define PDB_INST (0U)

/**
 * @brief This macro indicates that adc sample time.
 * 
 * https://codebeamer.daiichi.com/issue/191563
 * 
 */
#define ADC_SAMPLE_TIME (100U)

/**
 * @brief This macro indicates which port to be used.
 * 
 *        - PORTA = 0
 *        - PORTB = 1
 *        - PORTC = 2
 *        - PORTD = 3
 *        - PORTE = 4
 * 
 * https://codebeamer.daiichi.com/issue/191564
 * 
 */
#define ADC_PORT (0U)

/**
 * @brief This macro indicates which pin to be used.
 * 
 * https://codebeamer.daiichi.com/issue/191565
 * 
 */
#define ADC_PIN (7U)

/**
 * @brief This macro indicates that adc buffer size.
 * 
 * https://codebeamer.daiichi.com/issue/191566
 * 
 */
#define ADC_BUFF_MAX_CNT (50U)

/**
 * @brief This macro indicates which edma channel to be used.
 * 
 * https://codebeamer.daiichi.com/issue/191567
 * 
 */
#define EDMA_CH_3 (3U)

/**
 * @brief This macro indicates that ADC pin count.
 * 
 * https://codebeamer.daiichi.com/issue/191568
 * 
 */
#define ADC_PIN_COUNT (1U)

/**
 * @brief This macro indicates which pdb channel to be used.
 * 
 * https://codebeamer.daiichi.com/issue/191570
 * 
 */
#define PDB_CH_0 (0U)

/**
 * @brief This macro indicates that pdb period(us).
 * 
 * https://codebeamer.daiichi.com/issue/191571
 * 
 */
#define PDB_PERIOD (10000U)

/**
 * @brief This macro holds ADC initial value.
 * 
 * https://codebeamer.daiichi.com/issue/191852
 * 
 */
#define ADC_INIT_VAL (0xFFFFU)

/**
 * @brief This macro indicates the maximum value of the uint16_t type.
 *        It is used instead of the UINT16_MAX macro defined in the 
 *        library due to a QAC error.
 * 
 */
#define UINT16_T_MAX (0xFFFFU) 

// PRQA S 380 --

/*******************************************************************************
 * Private Variables 
 ******************************************************************************/

/**
 * @brief This variable indicates that the external flash components initialize 
 *        status.
 * 
 * https://codebeamer.daiichi.com/issue/191572
 * 
 */
static bool m_adc_init = false;

/**
 * @brief This variable indicates the adc buffer.
 * 
 * https://codebeamer.daiichi.com/issue/191573
 * 
 */
static uint16_t m_adc_raw_buff[ADC_BUFF_MAX_CNT] = {0U};

/**
 * @brief This variable indicates the filtered adc value.
 * 
 * https://codebeamer.daiichi.com/issue/191851
 * 
 */
static uint16_t m_adc_raw_val = 0U;

/**
 * @brief This variable indicates the edma channle run-tine state.
 * 
 * https://codebeamer.daiichi.com/issue/191575
 * 
 */
static edma_chn_state_t m_edma_ch_state = {0U};

/*******************************************************************************
 * Private Function Declarations
 ******************************************************************************/

/**
 * @brief This function provides to calculate PDB trigger period.
 * 
 * @param[in] p_pdb_cfg    PDB configuration structure address.
 * @param[in] period       Period value in microseconds.
 * @param[out] p_timer_val Period variable address.
 * 
 * https://codebeamer.daiichi.com/issue/191541
 * 
 */
static inline void pdb_period_calc(const pdb_timer_config_t *p_pdb_cfg,
                                   const uint32_t period,
                                   uint16_t *p_timer_val);

/**
 * @brief This function provides the reset of adc variables.
 * 
 * https://codebeamer.daiichi.com/issue/191551
 * 
 */
static inline void adc_private_param_reset(void);

/**
 * @brief This function provides the pdb initialization.
 * 
 * https://codebeamer.daiichi.com/issue/191550
 * 
 */
static void pdb_init(void);

/**
 * @brief This function provides to get adc register address.
 * 
 * @param[out] p_addr ADC source address.
 * 
 * https://codebeamer.daiichi.com/issue/191543
 * 
 */
static inline void adc_src_addr_get(uint32_t *p_addr);

/**
 * @brief This function provides to start edma loop.
 * 
 * @return bool Function status.
 * 
 * @retval true  eDMA loop successfully starts.
 * @retval false eDMA driver fail.
 * 
 * https://codebeamer.daiichi.com/issue/191547
 * 
 */
static bool edma_loop_start(void);

/**
 * @brief This function provides to initialize eDMA unit.
 * 
 * @return bool Function status.
 * 
 * @retval true  eDMA successfully initialize.
 * @retval false eDMA initialize fail.
 * 
 * https://codebeamer.daiichi.com/issue/191552
 * 
 */
static bool edma_init(void);

/**
 * @brief This function provides the adc pin initialization.
 * 
 * https://codebeamer.daiichi.com/issue/191546
 * 
 */
static bool adc_gpio_init(void);

/**
 * @brief This function provides the adc, adc pin, pdb, and edma initialization.
 * 
 * @param[in] instance Provides the adc unit.
 * 
 * @return bool Function status.
 * 
 * @retval true  ADC and ADC-related units successfully initialized.
 * @retval false ADC and ADC-related unit initialization fail.
 * 
 * https://codebeamer.daiichi.com/issue/191542
 * 
 */
static bool adc_init(const uint32_t instance);

/**
 * @brief This function provides the edma callback function.
 * 
 * @param[in] parameter 
 * @param[in] status 
 * 
 * https://codebeamer.daiichi.com/issue/191544
 * 
 */
static void adc_edma_cb(void *parameter, edma_chn_status_t status);

/**
 * @brief This function provides to filter for adc buffer.
 * 
 * @param[in] p_buff     Adc buffer address.
 * @param[in] size       Adc buffer size.
 * @param[out] p_adc_val Filtered adc variable.
 * 
 * https://codebeamer.daiichi.com/issue/191553
 * 
 */
static inline void adc_avg_filter(const uint16_t *p_buff, 
                                  size_t size, 
                                  uint16_t *p_adc_val);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/191540
 * 
 */
void bsp_adc_init(void)
{
    adc_private_param_reset();

    bool ret = adc_gpio_init();

    if (true == ret)
    {
        m_adc_init = adc_init(ADC_INST);

        LOG_INFO("Initialized");
    }
    else
    {
        LOG_ERROR("Init Fail");
    }
}

/**
 * @brief https://codebeamer.daiichi.com/issue/191545
 * 
 */
void bsp_adc_deinit(void)
{
    if (false == m_adc_init)
    {
        LOG_ERROR("BSP_ADC not initilized");

        return;
    }

    m_adc_init = false;

    ADC_DRV_Reset(ADC_INST);

    PDB_DRV_Deinit(PDB_INST);

    status_t status = EDMA_DRV_ReleaseChannel(EDMA_CH_3);

    adc_private_param_reset();

    LOG_DEBUG("ADC PDB EDMA Deinit; adc:%u, pdb:%u, edma:%u", 
              ADC_INST,
              PDB_INST,
              EDMA_CH_3,
              status);

    LOG_INFO("Denitialized");
}

/**
 * @brief https://codebeamer.daiichi.com/issue/191548
 * 
 */
bool bsp_adc_read(uint16_t *p_raw)
{
    if (NULL == p_raw)
    {
        LOG_ERROR("Invalid argument in bsp_adc_read");

        return false;
    }

    if (false == m_adc_init)
    {
        LOG_ERROR("BSP_ADC not initilized");
        
        return false;
    }

    if (ADC_INIT_VAL == m_adc_raw_val)
    {
        LOG_WARNING("ADC variable has initial value");

        return false;
    }

    *p_raw = m_adc_raw_val;

    LOG_VERBOSE("ADC value read: %u", m_adc_raw_val);

    return true;
}

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/191550
 * 
 */
static void pdb_init(void)
{
    const pdb_timer_config_t pdb_cfg = {
        .loadValueMode = PDB_LOAD_VAL_IMMEDIATELY,
        .seqErrIntEnable = false,
        .clkPreDiv = PDB_CLK_PREDIV_BY_1,
        .clkPreMultFactor = PDB_CLK_PREMULT_FACT_AS_10,
        .triggerInput = PDB_SOFTWARE_TRIGGER,
        .continuousModeEnable = true,
        .dmaEnable = false,
        .intEnable = false,
    };

    pdb_adc_pretrigger_config_t pdb_pretrgr_cfg = {
        .adcPreTriggerIdx = 0U,
        .preTriggerEnable = true,
        .preTriggerOutputEnable = true,
        .preTriggerBackToBackEnable = false,
    };

    uint16_t pdb_delay_val = 0U;

    PDB_DRV_Init(PDB_INST, &pdb_cfg);

    PDB_DRV_Enable(PDB_INST);

    PDB_DRV_ConfigAdcPreTrigger(PDB_INST, PDB_CH_0, &pdb_pretrgr_cfg);

    pdb_period_calc(&pdb_cfg, PDB_PERIOD, &pdb_delay_val);

    PDB_DRV_SetTimerModulusValue(PDB_INST, pdb_delay_val);

    PDB_DRV_SetAdcPreTriggerDelayValue(PDB_INST,
                                       PDB_CH_0,
                                       PDB_CH_0,
                                       pdb_delay_val);
    
    PDB_DRV_LoadValuesCmd(PDB_INST);

    LOG_DEBUG("PDB configured; instance:%u, period:%u, PreDiv:%u, PreMulFac:%u", 
              (uint32_t)PDB_INST,
              pdb_delay_val, 
              pdb_cfg.clkPreDiv,
              pdb_cfg.clkPreMultFactor);

    PDB_DRV_SoftTriggerCmd(PDB_INST);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/191547
 * 
 */
static bool edma_loop_start(void)
{
    uint32_t src_addr = 0U;

    adc_src_addr_get(&src_addr);

    edma_loop_transfer_config_t edma_loop_cfg = {
        .majorLoopIterationCount = ADC_BUFF_MAX_CNT,
        .srcOffsetEnable = true,
        .dstOffsetEnable = true,
        .minorLoopOffset = 0,
        .minorLoopChnLinkEnable = false,
        .minorLoopChnLinkNumber = 0U,
        .majorLoopChnLinkEnable = false,
        .majorLoopChnLinkNumber = 0U,
    };

    const edma_transfer_config_t edma_transfer_cfg = {
        .srcAddr = src_addr,
// PRQA S 306 ++ # Since the destination address is requested in integer type, this conversion should be done. False positive warning.
        .destAddr = (uint32_t)&m_adc_raw_buff[0],
// PRQA S 306 --
        .srcTransferSize = EDMA_TRANSFER_SIZE_2B,
        .destTransferSize = EDMA_TRANSFER_SIZE_2B,
        .srcOffset = 0,
        .destOffset = 2,
        .srcLastAddrAdjust = 0,
        .destLastAddrAdjust = -(int32_t)(2 * (int32_t)ADC_BUFF_MAX_CNT),
        .srcModulo = EDMA_MODULO_OFF,
        .destModulo = EDMA_MODULO_OFF,
        .minorByteTransferCount = 2U,
        .scatterGatherEnable = false,
        .interruptEnable = true,
        .loopTransferConfig = &edma_loop_cfg,
    };

    status_t status = EDMA_DRV_ConfigLoopTransfer(EDMA_CH_3, 
                                                  &edma_transfer_cfg);
   
    if (STATUS_SUCCESS != status)
    {
        return false;
    }

    status = EDMA_DRV_SetChannelRequestAndTrigger(EDMA_CH_3,
                                                  (uint8_t)EDMA_REQ_ADC0,
                                                  false);

    if (STATUS_SUCCESS != status)
    {
        return false;
    }

    status = EDMA_DRV_StartChannel(EDMA_CH_3);

    LOG_DEBUG("EDMA Started; channel:%u, mjrloop:%u, mnrloop:%u, status:%u", 
              EDMA_CH_3,
              edma_transfer_cfg.loopTransferConfig->majorLoopIterationCount,
              edma_transfer_cfg.minorByteTransferCount,
              status);

    return (STATUS_SUCCESS == status);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/191552
 * 
 */
static bool edma_init(void)
{
    const edma_channel_config_t edma_ch_cfg = {

        .channelPriority = EDMA_CHN_PRIORITY_1,
        .virtChnConfig = EDMA_CH_3,
        .source = EDMA_REQ_ADC0,
        .enableTrigger = false,
        .callback = adc_edma_cb,
        .callbackParam = NULL,
    };

    status_t status = EDMA_DRV_ChannelInit(&m_edma_ch_state, &edma_ch_cfg);

    LOG_DEBUG("EDMA Init; channel:%u, source:%u, status:%u", 
              edma_ch_cfg.virtChnConfig,
              edma_ch_cfg.source,
              status);

    if (STATUS_SUCCESS != status)
    {
        return false;
    }

    bool ret = edma_loop_start();

    return ret;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/191543
 * 
 */
static inline void adc_src_addr_get(uint32_t *p_addr)
{
// PRQA S 306 ++ # Memory mapped register must be addressed. False positive warning.
// PRQA S 303 ++ # Since the destination address is requested in integer type, this conversion should be done. False positive warning.
    *p_addr = (uint32_t)&(ADC0->R[0U]);
// PRQA S 306 --
// PRQA S 303 --
}

/**
 * @brief https://codebeamer.daiichi.com/issue/191541
 *  
 */
static inline void pdb_period_calc(const pdb_timer_config_t *p_pdb_cfg,
                                   const uint32_t period,
                                   uint16_t *p_timer_val)
{
    const uint32_t pdb_prsc = (uint32_t)(1UL << (uint32_t)p_pdb_cfg->clkPreDiv);
    uint32_t pdb_premult = 0U;

    uint32_t clck_freq = 0U;

    const uint32_t factor_us = 1000000UL;

    switch (p_pdb_cfg->clkPreMultFactor)
    {
    case PDB_CLK_PREMULT_FACT_AS_1:
    {
        pdb_premult = 1U;
    }
    break;
    
    case PDB_CLK_PREMULT_FACT_AS_10:
    {
        pdb_premult = 10U;
    }
    break;
    
    case PDB_CLK_PREMULT_FACT_AS_20:
    {
        pdb_premult = 20U;
    }
    break;
    
    case PDB_CLK_PREMULT_FACT_AS_40:
    {
        pdb_premult = 40U;
    }
    break;
    
    default:
    {
        pdb_premult = 1U;
    }
    break;
    }
    
    (void)CLOCK_SYS_GetFreq(CORE_CLK, &clck_freq);

    uint32_t tmp_val = ((uint32_t)(clck_freq / factor_us) * period);

    tmp_val = (uint32_t)(tmp_val / (pdb_prsc * pdb_premult));

    if ((0U == tmp_val) || (UINT16_T_MAX < tmp_val))
    {
        *p_timer_val = 0U;
    }
    else
    {
        *p_timer_val = (uint16_t)tmp_val;
    }
}

/**
 * @brief https://codebeamer.daiichi.com/issue/191542
 * 
 */
static bool adc_init(const uint32_t instance)
{
    const adc_converter_config_t adc_cfg = {

        .clockDivide = ADC_CLK_DIVIDE_4,
        .sampleTime = ADC_SAMPLE_TIME,
        .resolution = ADC_RESOLUTION_12BIT,
        .inputClock = ADC_CLK_ALT_1,
        .trigger = ADC_TRIGGER_HARDWARE,
        .pretriggerSel = ADC_PRETRIGGER_SEL_PDB,
        .triggerSel = ADC_TRIGGER_SEL_PDB,
        .dmaEnable = true,
        .voltageRef = ADC_VOLTAGEREF_VREF,
        .continuousConvEnable = false,
        .supplyMonitoringEnable = false,
    };

    const adc_chan_config_t adc_ch_cfg = {
        .channel = ADC_INPUTCHAN_EXT3,
        .interruptEnable = true,
    };

    ADC_DRV_ConfigConverter(instance, &adc_cfg);
    ADC_DRV_AutoCalibration(instance);
    ADC_DRV_ConfigChan(instance, 0U, &adc_ch_cfg);

    LOG_DEBUG("ADC configured; channel:%u, sampletime:%u, resolution:%u", 
              adc_ch_cfg.channel,
              adc_cfg.sampleTime, 
              adc_cfg.resolution);

    pdb_init();

    if (true == edma_init())
    {
        return true;
    }

    return false;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/191546
 * 
 */
static bool adc_gpio_init(void)
{
// PRQA S 306 ++ # Defined by nxp library to hold register base address. False positive warning.
    PORT_Type * const gpio_port[PORT_INSTANCE_COUNT] = {

        PORTA,
        PORTB,
        PORTC,
        PORTD,
        PORTE,
    };

    GPIO_Type * const gpio_type[PORT_INSTANCE_COUNT] = {

        PTA,
        PTB,
        PTC,
        PTD,
        PTE,
    };
// PRQA S 306 --
    const pin_settings_config_t adc_pin_cfg = {

        .base = gpio_port[ADC_PORT],
        .pinPortIdx = ADC_PIN,
        .pullConfig = PORT_INTERNAL_PULL_NOT_ENABLED,
        .passiveFilter = false,
        .driveSelect = PORT_LOW_DRIVE_STRENGTH,
        .mux = PORT_PIN_DISABLED,
        .pinLock = false,
        .intConfig = PORT_DMA_INT_DISABLED,
        .clearIntFlag = false,
        .digitalFilter = false,
        .gpioBase = gpio_type[ADC_PORT],
        .direction = GPIO_INPUT_DIRECTION,
        .initValue = 0U,
    };

    const pin_settings_config_t * const p_cfg = &adc_pin_cfg;

    status_t status = PINS_DRV_Init(ADC_PIN_COUNT, p_cfg);

    for (uint8_t cnt = 0; cnt < ADC_PIN_COUNT; cnt++)
    {
        LOG_DEBUG("Pin init; base:%u, pin:%u, dir:%u, pull:%u, status:%u", 
                  p_cfg[cnt].base, 
                  p_cfg[cnt].pinPortIdx,
                  p_cfg[cnt].direction,
                  p_cfg[cnt].pullConfig,
                  status);
    }

    return (STATUS_SUCCESS == status);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/191553
 * 
 */
static inline void adc_avg_filter(const uint16_t *p_buff, 
                                  size_t size, 
                                  uint16_t *p_adc_val)
{    
    if ((0 == size) || (NULL == p_buff) || (NULL == p_adc_val))
    {
        return;
    }

    uint32_t tmp_val = 0U;

    for (uint32_t idx = 0U; size > idx; idx++)
    {
        tmp_val = tmp_val + p_buff[idx];
    }

    *p_adc_val = (uint16_t)(tmp_val / size);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/191551
 * 
 */
static inline void adc_private_param_reset(void)
{
    m_adc_init = false;

    for (uint32_t idx = 0U; ADC_BUFF_MAX_CNT > idx; idx++)
    {
        m_adc_raw_buff[idx] = 0U;
    }

    m_adc_raw_val = ADC_INIT_VAL;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/191544
 * 
 */
// PRQA S 3673 ++ # No const arguments were used in the prototype defined in the nxp library.
static void adc_edma_cb(void *parameter, edma_chn_status_t status)
{
// PRQA S 3673 --
    (void)parameter;

    if (EDMA_CHN_NORMAL == status)
    {
        adc_avg_filter(m_adc_raw_buff, ADC_BUFF_MAX_CNT, &m_adc_raw_val);
    }

    if (EDMA_CHN_ERROR == status)
    {
        // PRQA S 306 ++ # Suppressed, Defined by nxp library to cast register data. False positive warning.
        LOG_ERROR("DMA error on ADC, DMA->ES = 0x%x", DMA->ES);
        LOG_ERROR("DMA->TCD[3].SADDR = 0x%x", DMA->TCD[EDMA_CH_3].SADDR);
        LOG_ERROR("DMA->TCD[3].DADDR = 0x%x", DMA->TCD[EDMA_CH_3].DADDR);
        // PRQA S 306 --
    }
}

// end of file
