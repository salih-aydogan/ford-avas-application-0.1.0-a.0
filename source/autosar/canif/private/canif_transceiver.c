// PRQA S 3108 EOF # Comments with links are required for ensure traceability.
/**
 * @file canif_transceiver.c
 * @author Burak Utku Cecen (utku.cecen@daiichi.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-30
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "public/canif.h"

#include "bsp_gpio.h"

#include "log.h"

LOG_MODULE_NAME("CANIF_TRANSCEIVER");

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/187951
 * 
 */
static can_trcv_mode_t m_trcv_mode;

/*******************************************************************************
 * Private Function Declarations
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/188053
 * 
 * 
 */
static std_return_t canif_trcv_errn_flag_read(can_trcv_errn_flag_t* p_errn_flag,
                                             can_trcv_flag_status_t* status);

/**
 * @brief Construct a new transceiver mode request object
 * 
 * @param trcv_mode_req 
 */
static std_return_t transceiver_mode_request(can_trcv_mode_t trcv_mode_req);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/188054
 * 
 */
std_return_t canif_trcv_mode_set(can_trcv_mode_t trcv_mode_req)
{
    bool set_status = true;

    if (false == g_canif_init)
    {
        return E_NOT_OK;
    }

    switch (g_transceiver_cfg.trcv_current_mode)
    {
    case CAN_TRCV_MODE_SLEEP:
    {
        // Placeholder

    }
    break;

    case CAN_TRCV_MODE_STANDBY:
    {
        // Placeholder
    }
    break;

    case CAN_TRCV_MODE_LISTENONLY:
    {
        // Placeholder
    }
    break;

    case CAN_TRCV_MODE_NORMAL:
    {
        // Placeholder
    }
    break;

    case CAN_TRCV_MODE_GOTOSLEEP:
    {
        if (CAN_TRCV_MODE_SLEEP == trcv_mode_req)
        {
            set_status = false;
        }
    }
    break;

    default:
    {
        set_status = true;
    }
    break;
    }

    std_return_t ret = transceiver_mode_request(trcv_mode_req);

    if ((E_OK != ret) || (true != set_status))
    {
        return E_NOT_OK;
    }

    m_trcv_mode = g_transceiver_cfg.trcv_current_mode; 

    canif_trcv_mode_indication(m_trcv_mode); 

    return E_OK;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/188055
 * 
 */
std_return_t canif_trcv_mode_get(can_trcv_mode_t* p_current_trcv_mode)
{  
    if (true != g_canif_init)
    {
        return E_NOT_OK;
    }
    
    *p_current_trcv_mode = m_trcv_mode;

    return E_OK;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/188244
 *          
 */
std_return_t canif_trcv_wakeup_reason_get(
        can_trcv_wakeup_reason_t* p_trcv_wu_reason)
{
    can_trcv_errn_flag_t flag = CAN_TRCV_ERR_N_FLAG_NONE;

    can_trcv_flag_status_t status = CAN_TRCV_INTERNAL_FLAG_CLEARED;

    g_transceiver_cfg.trcv_current_mode = m_trcv_mode;

    if (CAN_TRCV_MODE_LISTENONLY == g_transceiver_cfg.trcv_current_mode)
    {
        *p_trcv_wu_reason = CANTRCV_WU_BY_PIN;
    }
    else if (CAN_TRCV_MODE_STANDBY == g_transceiver_cfg.trcv_current_mode)
    {
        (void)canif_trcv_errn_flag_read(&flag, &status);

        if ((CAN_TRCV_INTERNAL_FLAG_SET == status) &&
            (CAN_TRCV_WAKEUP_FLAG == flag))
        {
            *p_trcv_wu_reason = CANTRCV_WU_INTERNALLY;
        }
        else 
        {
            *p_trcv_wu_reason = CANTRCV_WU_BY_BUS;
        }
    }
    else
    {
        *p_trcv_wu_reason = CANTRCV_WU_ERROR;
    }

    return E_OK;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/188210
 * 
 */
void canif_trcv_main_func_diagnostics(void)
{
    std_return_t ret = 
        canif_trcv_errn_flag_read(&g_transceiver_cfg.errn_flag,
        &g_transceiver_cfg.trcv_flag_status);

    if (E_NOT_OK == ret)
    {
        return;
    }

    canif_fault_state_t error = CAN_ERRORSTATE_NONE;

    (void)canif_controller_error_state_get(&error);

    if (CAN_TRCV_INTERNAL_FLAG_SET != g_transceiver_cfg.trcv_flag_status)
    { 
        return;
    }

    switch (g_transceiver_cfg.errn_flag)
    {
    case CAN_TRCV_BUS_FAILURE_FLAG:
    {
        if (CAN_ERRORSTATE_NONE != error)
        {
        // Placeholder
        }
    }
    break;

    case CAN_TRCV_LOCAL_FAILURE_FLAG:
    {
        // Placeholder
    }
    break;

    case CAN_TRCV_PWON_FLAG:
    {
        // Placeholder
    }
    break;

    case CAN_TRCV_WAKEUP_FLAG:
    {
        // Placeholder
    }    
    break;

    default:
    {
        // Placeholder
    }
    break;
    }
}

/**
 * @brief https://codebeamer.daiichi.com/issue/187929
 * 
 */
void canif_trcv_wake_flag_indication(void)
{
    g_canif_generic_callouts->canif_wake_flag_callout();
}

/**
 * @brief https://codebeamer.daiichi.com/issue/187930
 * 
 * @param transceiver_mode 
 */
void canif_trcv_mode_indication(can_trcv_mode_t transceiver_mode)
{
    g_canif_generic_callouts->canif_trcv_mode_callout(transceiver_mode);
}
       
/**
 * @brief https://codebeamer.daiichi.com/issue/188053
 * 
 * @param p_errn_flag used to return the read error flag status.
 * @param status  used to return the transceiver flag status.
 * 
 * @return std_return_t  if either of the input parameters 
 * (p_errn_flag or status) is NULL, or if the err_n_pin_status value is not 0 or
 *  1.
 */
static std_return_t canif_trcv_errn_flag_read(can_trcv_errn_flag_t* p_errn_flag,
                                              can_trcv_flag_status_t* status)
{
    bool errn_flag_readable = true;

    if ((NULL == p_errn_flag) || (NULL == status))
    {
        return E_NOT_OK;
    }

    bsp_gpio_status_t err_n_pin_status = 
        bsp_gpio_status_get(BSP_GPIO_CAN_ERROR_IN_PIN_NO);

    if (BSP_GPIO_STATUS_SET == err_n_pin_status)
    {
        g_transceiver_cfg.trcv_flag_status = CAN_TRCV_INTERNAL_FLAG_SET;

        switch (g_transceiver_cfg.trcv_current_mode)
        {
        case CAN_TRCV_MODE_NORMAL:
        {
            *p_errn_flag = CAN_TRCV_BUS_FAILURE_FLAG;

            errn_flag_readable = true;

            if (CAN_TRCV_MODE_LISTENONLY == g_transceiver_cfg.trcv_old_mode)
            {
                *p_errn_flag = CAN_TRCV_WAKEUP_FLAG;

                canif_trcv_wake_flag_indication();

                errn_flag_readable = true;
            }
        }
        break;

        case CAN_TRCV_MODE_LISTENONLY:
        {

            if (CAN_TRCV_MODE_NORMAL == g_transceiver_cfg.trcv_old_mode)
            {
                *p_errn_flag = CAN_TRCV_LOCAL_FAILURE_FLAG;

                errn_flag_readable = true;
            }
            else if ((CAN_TRCV_MODE_NORMAL != 
                     g_transceiver_cfg.trcv_old_mode) &&
                     (CAN_TRCV_MODE_LISTENONLY !=
                     g_transceiver_cfg.trcv_old_mode))
            {
                *p_errn_flag = CAN_TRCV_PWON_FLAG;

                errn_flag_readable = true;
            }
            else
            {   
                errn_flag_readable = false;
            }
            break;
        }
        break;

        case CAN_TRCV_MODE_STANDBY:
        {
            // Placeholder
        }
        break;

        case CAN_TRCV_MODE_GOTOSLEEP:
        {
            // Placeholder
        }
        break;

        case CAN_TRCV_MODE_SLEEP:
        {
            //Placeholder
        }
        break;

        default:
        {
            errn_flag_readable = false;
        }
        break;
        }
    }
    else if (BSP_GPIO_STATUS_RESET == err_n_pin_status)
    {
        g_transceiver_cfg.trcv_flag_status = CAN_TRCV_INTERNAL_FLAG_CLEARED;
    }
    else
    {
        errn_flag_readable = false;

        LOG_ERROR("BSP_GPIO_STATUS_ERR TRCV FLAG");
    }

    if (false == errn_flag_readable)
    {
        LOG_ERROR("ERRN FLAG NOT READABLE");

        return E_NOT_OK;   
    }
    
    *status = g_transceiver_cfg.trcv_flag_status;

    return E_OK;
}

/**
 * @brief This function sets the transceiver mode of a CAN transceiver according
 * to requested mode.
 * 
 * https://codebeamer.daiichi.com/issue/191554
 * 
 * @param trcv_mode_req Requested transceiver mode.
 * 
 * @return std_return_t E_OK : Requested transceiver mode is valid.
 *                      E_NOT_OK: Requested transceiver mode is invalid.
 */
static std_return_t transceiver_mode_request(can_trcv_mode_t trcv_mode_req)
{
    bool state_req_valid = false;

    switch (trcv_mode_req)
    {
    case CAN_TRCV_MODE_NORMAL:
    {
        if ((CAN_TRCV_MODE_NORMAL != g_transceiver_cfg.trcv_current_mode))
        {
            g_transceiver_cfg.trcv_old_mode = 
                g_transceiver_cfg.trcv_current_mode;

            bsp_gpio_set(BSP_GPIO_CAN_STB_OUT_PIN_NO);

            bsp_gpio_set(BSP_GPIO_CAN_EN_OUT_PIN_NO);

            g_transceiver_cfg.trcv_current_mode = CAN_TRCV_MODE_NORMAL;

            state_req_valid = true;
        }
    }
    break;

    case CAN_TRCV_MODE_LISTENONLY:
    {
        if (CAN_TRCV_MODE_LISTENONLY != g_transceiver_cfg.trcv_current_mode)
        {
            g_transceiver_cfg.trcv_old_mode = 
                g_transceiver_cfg.trcv_current_mode;

            bsp_gpio_set(BSP_GPIO_CAN_STB_OUT_PIN_NO);
            
            bsp_gpio_reset(BSP_GPIO_CAN_EN_OUT_PIN_NO);

            g_transceiver_cfg.trcv_current_mode = CAN_TRCV_MODE_LISTENONLY;

            state_req_valid = true;
        }
    }
    break;

    case CAN_TRCV_MODE_GOTOSLEEP:  
    {
        if (CAN_TRCV_MODE_SLEEP != g_transceiver_cfg.trcv_current_mode)
        {
            g_transceiver_cfg.trcv_old_mode = 
                g_transceiver_cfg.trcv_current_mode;
            
            bsp_gpio_reset(BSP_GPIO_CAN_STB_OUT_PIN_NO);
            
            bsp_gpio_set(BSP_GPIO_CAN_EN_OUT_PIN_NO);
            
            bsp_gpio_set(BSP_GPIO_CAN_WAKE_OUT_PIN_NO);

            g_transceiver_cfg.trcv_current_mode = CAN_TRCV_MODE_GOTOSLEEP;

            state_req_valid = true;
        }
    }
    break;
    
    case CAN_TRCV_MODE_STANDBY:
    {
        if ((CAN_TRCV_MODE_STANDBY != g_transceiver_cfg.trcv_current_mode) && 
            (CAN_TRCV_MODE_SLEEP != g_transceiver_cfg.trcv_current_mode))
        {
            g_transceiver_cfg.trcv_old_mode = 
                g_transceiver_cfg.trcv_current_mode;

            bsp_gpio_reset(BSP_GPIO_CAN_STB_OUT_PIN_NO);

            bsp_gpio_reset(BSP_GPIO_CAN_EN_OUT_PIN_NO);

            g_transceiver_cfg.trcv_current_mode = CAN_TRCV_MODE_STANDBY;

            state_req_valid = true;
        }
        else if (CAN_TRCV_MODE_SLEEP == g_transceiver_cfg.trcv_current_mode)
        {     
            g_transceiver_cfg.trcv_old_mode =
                 g_transceiver_cfg.trcv_current_mode;

            bsp_gpio_reset(BSP_GPIO_CAN_STB_OUT_PIN_NO);

            bsp_gpio_reset(BSP_GPIO_CAN_WAKE_OUT_PIN_NO);

            g_transceiver_cfg.trcv_current_mode = CAN_TRCV_MODE_STANDBY;

            state_req_valid = true;
        }
        else
        {
            state_req_valid = false;
        }
    }
    break;
    
    default:
    {
        state_req_valid = false;
    }
    break;
    }

    if (false == state_req_valid)
    {
        LOG_ERROR("CANIF TRCV STATE REQ FAILED");

        return E_NOT_OK;  
    }

    return E_OK;
}

// end of file
