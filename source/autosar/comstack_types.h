/**
 * @file comstack_types.h
 * @author Burak Utku Cecen (utku.cecen@daiichi.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-30
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef COMSTACK_TYPES_H
#define COMSTACK_TYPES_H

#include "daiichi_types.h"

/**
 * @brief Represents the Identifier of an L-PDU. 
 * 
 * https://codebeamer.daiichi.com/issue/186662
 * 
 */
typedef uint32_t can_id_t;	

/**
 * @brief Specified generic PDU ID.
 * 
 * https://codebeamer.daiichi.com/issue/186660
 * 
 */
typedef uint16_t pdu_id_t;

/**
 * @brief Specified PDU data length.
 *  
 * The size of this global type depends on the maximum length of PDUs to
 * be sent by an ECU.
 * 
 * https://codebeamer.daiichi.com/issue/186659
 * 
 */
typedef uint16_t pdu_length_t;

/**
 * @brief https://codebeamer.daiichi.com/issue/187039
 * 
 *  Variables of this type shall be used to store the basic information about a
 *  PDU of any type, namely a pointer variable pointing to its SDU (payload),
 *  a pointer to Meta Data of the PDU, and the corresponding length of the SDU
 *  in bytes.
 * 
 */
typedef struct pdu_info_s
{
	uint8_t* p_sdu_data;
	uint8_t* p_meta_data;
	pdu_length_t sdu_length;

} pdu_info_t;

/**
 * @brief https://codebeamer.daiichi.com/issue/187040
 * 
 * Used for received PDU information.
 * 
 */

// PRQA S 1535 ++ # Supressed, 'canif_transmission_type_t' and  'canif_id_type_t' used this project.
typedef struct j1939_if_meta_data_s
{
    can_id_t can_id;

} j1939_if_meta_data_t;

// PRQA S 1535 --

/**
 * @brief List of AUTOSAR geceric return states.
 * 
 * https://codebeamer.daiichi.com/issue/187051
 * 
 */
typedef enum std_return_e
{
    E_OK = 0U,
    
    E_NOT_OK,

    CAN_BUSY,

} std_return_t;

#endif /* COMSTACK_TYPES_H */

// end of file
