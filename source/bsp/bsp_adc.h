// PRQA S 3108 EOF # Comments with links are required for ensure traceability.
/**
 * @file bsp_adc.h
 * @author Berke Yalcinkaya (berke.yalcinkaya@daiichi.com)
 * @brief Interface of the ADC.
 * @version 0.1
 * @date 2023-05-04
 *
 * @copyright Copyright (c) 2023
 *
 * https://codebeamer.daiichi.com/issue/190860
 * https://codebeamer.daiichi.com/issue/190350
 *
 */

#ifndef BSP_ADC_H
#define BSP_ADC_H

#include "daiichi_types.h"

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief This function configures the ADC unit and the ADC pins.
 *
 * https://codebeamer.daiichi.com/issue/190864
 *
 */
void bsp_adc_init(void);

/**
 * @brief This function resets the ADC registers.
 *
 * https://codebeamer.daiichi.com/issue/190865
 *
 */
void bsp_adc_deinit(void);

/**
 * @brief This function does the conversion and writes it to the given parameter
 *        After starting the conversion, it waits for it to complete.
 *
 * @param[out] p_raw Variable address to keep the conversion result.
 *
 * https://codebeamer.daiichi.com/issue/190866
 *
 */
bool bsp_adc_read(uint16_t *p_raw);

#endif /* BSP_ADC_H */

// end of file
