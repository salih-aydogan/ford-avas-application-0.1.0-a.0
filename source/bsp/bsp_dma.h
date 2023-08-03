// PRQA S 3108 EOF # Comments with links are required for ensure traceability.
/**
 * @file bsp_dma.h
 * @author berke yalcinkaya (berke.yalcinkaya@daiichi.com)
 * @brief The interface of the DMA.
 * @version 0.1
 * @date 2023-05-22
 *
 * @copyright Copyright (c) 2023
 *
 * https://codebeamer.daiichi.com/issue/192918
 * https://codebeamer.daiichi.com/issue/192917
 *
 */

#ifndef BSP_DMA_H
#define BSP_DMA_H

#include "daiichi_types.h"

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief This function configures the DMA driver unit.
 *
 * @return bool Function status.
 *
 * @retval true  DMA initialization success.
 * @retval false DMA init driver function fail.
 *
 * https://codebeamer.daiichi.com/issue/192920
 *
 */
bool bsp_dma_init(void);

/**
 * @brief This function resets the DMA driver unit.
 *
 * @return bool Function status.
 *
 * @retval true  DMA deinitialization success.
 * @retval false DMA deinit driver function fail.
 *
 * https://codebeamer.daiichi.com/issue/192921
 *
 */
bool bsp_dma_deinit(void);

#endif /* BSP_DMA_H  */

// end of file
