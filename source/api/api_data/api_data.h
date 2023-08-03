// PRQA S 3108 EOF # Comments with links are required for ensure traceability.
/**
 * @file api_data.h
 * @author berke yalcinkaya (berke.yalcinkaya@daiichi.com)
 * @brief The api_data interface provides to execute the read/write operation of
 *        an external flash or EEPROM.
 * @version 0.1
 * @date 2023-04-17
 *
 * @copyright Copyright (c) 2023
 *
 * https://codebeamer.daiichi.com/issue/177737
 * https://codebeamer.daiichi.com/issue/186875
 *
 */

#ifndef API_DATA_H
#define API_DATA_H

#include "daiichi_types.h"

/*******************************************************************************
 * Macros
 ******************************************************************************/
// PRQA S 380 ++ # Too many macros in the driver provided by NXP causes this error.

/**
 * @brief This macro indicates the size of one byte data.
 *
 * https://codebeamer.daiichi.com/issue/188767
 *
 */
#define API_DATA_BYTE_1 (1U)

/**
 * @brief This macro indicates the size of two byte data.
 *
 * https://codebeamer.daiichi.com/issue/188768
 *
 */
#define API_DATA_BYTE_2 (API_DATA_BYTE_1 * 2U)

/**
 * @brief This macro indicates the size of four byte data.
 *
 * https://codebeamer.daiichi.com/issue/188769
 *
 */
#define API_DATA_BYTE_4 (API_DATA_BYTE_1 * 4U)

/**
 * @brief This macro indicates the DTC message data size.
 *
 * https://codebeamer.daiichi.com/issue/189149
 *
 */
#define API_DATA_DTC_SIZE (API_DATA_BYTE_4 * 2U)

/**
 * @brief This macro indicates the DID message data size.
 *
 */
#define API_DATA_DID_SIZE (API_DATA_BYTE_1 * 24U)

/**
 * @brief This macro indicates the size of the audio file configuration table to
 *        be saved in Flash. This table contains the amplitude (2 bytes) and
 *        frequency (2 bytes) values for the audio file. The default value
 *        is given for 100 speed samples. It takes up 400 bytes in total.
 *        It also includes parameters such as the version (4 bytes), size
 *        (4 bytes), start speed (4 bytes), final speed (4 bytes), start
 *        frequency (4 bytes), final frequency (4 bytes) and audio name
 *        (32 bytes) of each audio file. It takes up 56 bytes in total.
 *        Some reserved space is left, resulting in a table size of 500
 *        bytes.
 *
 * https://codebeamer.daiichi.com/issue/189353
 *
 */
#define API_DATA_AUDIO_CFG_TBL_SIZE (API_DATA_BYTE_1 * 500U)

/**
 * @brief This macro indicates the default size of audio file data.
 *
 * https://codebeamer.daiichi.com/issue/188770
 *
 */
#define API_DATA_AUDIO_DEFAULT_SIZE (500U * 1024U)

/**
 * @brief This macro indicates the EEPROM authentication data address.
 *
 * https://codebeamer.daiichi.com/issue/189211
 *
 */
#define API_DATA_LABEL_AUTH (0U)

/**
 * @brief This macro indicates the Secure Boot status data address.
 *
 * https://codebeamer.daiichi.com/issue/189212
 *
 */
#define API_DATA_LABEL_BOOT_STATUS (API_DATA_LABEL_AUTH + API_DATA_BYTE_1)

/**
 * @brief This macro indicates the reserved data address.
 *
 * https://codebeamer.daiichi.com/issue/189213
 *
 */
#define API_DATA_LABEL_RESERVED (API_DATA_LABEL_BOOT_STATUS + API_DATA_BYTE_1)

/**
 * @brief This macro indicates the pedestrian alert data address.
 *
 * https://codebeamer.daiichi.com/issue/188772
 *
 */
#define API_DATA_LABEL_PED_ALERT \
    (API_DATA_LABEL_RESERVED + (API_DATA_BYTE_1 * 32U))

/**
 * @brief This macro indicates the car speed data address.
 *
 * https://codebeamer.daiichi.com/issue/188771
 *
 */
#define API_DATA_LABEL_CAR_SPEED (API_DATA_LABEL_PED_ALERT + API_DATA_BYTE_1)

/**
 * @brief This macro indicates the trans gear data address.
 *
 * https://codebeamer.daiichi.com/issue/188773
 *
 */
#define API_DATA_LABEL_TRANS_GEAR (API_DATA_LABEL_CAR_SPEED + API_DATA_BYTE_2)

/**
 * @brief This macro indicates the current gear data address.
 *
 * https://codebeamer.daiichi.com/issue/191735
 *
 */
#define API_DATA_LABEL_CURR_GEAR (API_DATA_LABEL_TRANS_GEAR + API_DATA_BYTE_1)

/**
 * @brief This macro indicates the ignition status data address.
 *
 * https://codebeamer.daiichi.com/issue/188774
 *
 */
#define API_DATA_LABEL_IGN_STATUS (API_DATA_LABEL_CURR_GEAR + API_DATA_BYTE_1)

/**
 * @brief This macro indicates the lifecyc mode data address.
 *
 * https://codebeamer.daiichi.com/issue/188775
 *
 */
#define API_DATA_LABEL_LCYC_MODE (API_DATA_LABEL_IGN_STATUS + API_DATA_BYTE_1)

/**
 * @brief This macro indicates the current audio data address.
 *
 * https://codebeamer.daiichi.com/issue/188776
 *
 */
#define API_DATA_LABEL_CURR_AUDIO (API_DATA_LABEL_LCYC_MODE + API_DATA_BYTE_1)

/**
 * @brief This macro indicates the DTC Speaker-1 Amplifier Open Circuit data
 *        address.
 *
 * https://codebeamer.daiichi.com/issue/188922
 *
 */
#define API_DATA_LABEL_DTC_SPKR_1_OC \
    (API_DATA_LABEL_CURR_AUDIO + API_DATA_BYTE_1)

/**
 * @brief This macro indicates the DTC Speaker-1 Amplifier Out Shorted to the
 *        Power data address.
 *
 * https://codebeamer.daiichi.com/issue/188923
 *
 */
#define API_DATA_LABEL_DTC_SPKR_1_OS_PWR \
    (API_DATA_LABEL_DTC_SPKR_1_OC + API_DATA_DTC_SIZE)

/**
 * @brief This macro indicates the DTC Speaker-1 Amplifier Out Shorted to the
 *        GND data address.
 *
 * https://codebeamer.daiichi.com/issue/188925
 *
 */
#define API_DATA_LABEL_DTC_SPKR_1_OS_GND \
    (API_DATA_LABEL_DTC_SPKR_1_OS_PWR + API_DATA_DTC_SIZE)

/**
 * @brief This macro indicates the DTC Speaker-2 Amplifier Open Circuit data
 *        address.
 *
 * https://codebeamer.daiichi.com/issue/188940
 *
 */
#define API_DATA_LABEL_DTC_SPKR_2_OC \
    (API_DATA_LABEL_DTC_SPKR_1_OS_GND + API_DATA_DTC_SIZE)

/**
 * @brief This macro indicates the DTC Speaker-2 Amplifier Out Shorted to the
 *        Power data address.
 *
 * https://codebeamer.daiichi.com/issue/188941
 *
 */
#define API_DATA_LABEL_DTC_SPKR_2_OS_PWR \
    (API_DATA_LABEL_DTC_SPKR_2_OC + API_DATA_DTC_SIZE)

/**
 * @brief This macro indicates the DTC Speaker-2 Amplifier Out Shorted to the
 *        GND data address.
 *
 * https://codebeamer.daiichi.com/issue/188942
 *
 */
#define API_DATA_LABEL_DTC_SPKR_2_OS_GND \
    (API_DATA_LABEL_DTC_SPKR_2_OS_PWR + API_DATA_DTC_SIZE)

/**
 * @brief This macro indicates the DTC Can communication error VCU data address.
 *
 * https://codebeamer.daiichi.com/issue/188943
 *
 */
#define API_DATA_LABEL_DTC_ERR_VCU \
    (API_DATA_LABEL_DTC_SPKR_2_OS_GND + API_DATA_DTC_SIZE)

/**
 * @brief This macro indicates the DTC Invalid data received from VCU data
 *        address.
 *
 * https://codebeamer.daiichi.com/issue/188944
 *
 */
#define API_DATA_LABEL_DTC_INV_DATA_VCU \
    (API_DATA_LABEL_DTC_ERR_VCU + API_DATA_DTC_SIZE)

/**
 * @brief This macro indicates the DTC Control module general checksum failure
 *        data address.
 *
 * https://codebeamer.daiichi.com/issue/188945
 *
 */
#define API_DATA_LABEL_DTC_CSUM_FAIL \
    (API_DATA_LABEL_DTC_INV_DATA_VCU + API_DATA_DTC_SIZE)

/**
 * @brief This macro indicates the DTC Control module general memory failure
 *        data address.
 *
 * https://codebeamer.daiichi.com/issue/188946
 *
 */
#define API_DATA_LABEL_DTC_MEM_FAIL \
    (API_DATA_LABEL_DTC_CSUM_FAIL + API_DATA_DTC_SIZE)

/**
 * @brief This macro indicates the DTC Control module component internal failure
 *        data address.
 *
 * https://codebeamer.daiichi.com/issue/188947
 *
 */
#define API_DATA_LABEL_DTC_CTRL_MDL_FAIL \
    (API_DATA_LABEL_DTC_MEM_FAIL + API_DATA_DTC_SIZE)

/**
 * @brief This macro indicates the DTC Battery voltage circuit voltage below
 *        threshold data address.
 *
 * https://codebeamer.daiichi.com/issue/188948
 *
 */
#define API_DATA_LABEL_DTC_LOW_BAT \
    (API_DATA_LABEL_DTC_CTRL_MDL_FAIL + API_DATA_DTC_SIZE)

/**
 * @brief This macro indicates the DTC Battery voltage circuit voltage above
 *        threshold data address.
 *
 * https://codebeamer.daiichi.com/issue/188949
 *
 */
#define API_DATA_LABEL_DTC_HIGH_BAT \
    (API_DATA_LABEL_DTC_LOW_BAT + API_DATA_DTC_SIZE)

/**
 * @brief This macro indicates the Subsystem diagnostic part number DID
 *        (Data Identification) address.
 *
 * https://codebeamer.daiichi.com/issue/189450
 *
 */
#define API_DATA_LABEL_DID_SUBSYS_DIAG_PNUM \
    (API_DATA_LABEL_DTC_HIGH_BAT + API_DATA_DTC_SIZE)

/**
 * @brief This macro indicates the ECU Core Assembly Number DID
 *        (Data Identification) address.
 *
 * https://codebeamer.daiichi.com/issue/189451
 *
 */
#define API_DATA_LABEL_DID_ECU_CORE_ASM_NUM \
    (API_DATA_LABEL_DID_SUBSYS_DIAG_PNUM + API_DATA_DID_SIZE)

/**
 * @brief This macro indicates the ECU Assembly Number DID
 *        (Data Identification) address.
 *
 * https://codebeamer.daiichi.com/issue/189452
 *
 */
#define API_DATA_LABEL_DID_ECU_ASM_NUM \
    (API_DATA_LABEL_DID_ECU_CORE_ASM_NUM + API_DATA_DID_SIZE)

/**
 * @brief This macro indicates the ECU Delivery Assembly Number DID
 *        (Data Identification) address.
 *
 * https://codebeamer.daiichi.com/issue/189453
 *
 */
#define API_DATA_LABEL_DID_ECU_DELIV_ASM_NUM \
    (API_DATA_LABEL_DID_ECU_ASM_NUM + API_DATA_DID_SIZE)

/**
 * @brief This macro indicates the Diagnostic Specification Version DID
 *        (Data Identification) address.
 *
 * https://codebeamer.daiichi.com/issue/189454
 *
 */
#define API_DATA_LABEL_DID_DIAG_SPEC_VER \
    (API_DATA_LABEL_DID_ECU_DELIV_ASM_NUM + API_DATA_DID_SIZE)

/**
 * @brief This macro indicates the Vehicle manufacturer ECU SW number DID
 *        (Data Identification) address.
 *
 * https://codebeamer.daiichi.com/issue/189455
 *
 */
#define API_DATA_LABEL_DID_ECU_SW_NUM \
    (API_DATA_LABEL_DID_DIAG_SPEC_VER + API_DATA_BYTE_1)

/**
 * @brief This macro indicates the ECU Serial Number DID
 *        (Data Identification) address.
 *
 * https://codebeamer.daiichi.com/issue/189456
 *
 */
#define API_DATA_LABEL_DID_ECU_SERIAL_NUM \
    (API_DATA_LABEL_DID_ECU_SW_NUM + API_DATA_DID_SIZE)

/**
 * @brief This macro indicates the Vehicle Identification Number DID
 *        (Data Identification) address.
 *
 * https://codebeamer.daiichi.com/issue/189457
 *
 */
#define API_DATA_LABEL_DID_VEH_ID_NUM \
    (API_DATA_LABEL_DID_ECU_SERIAL_NUM + (API_DATA_BYTE_1 * 16U))

/**
 * @brief This macro indicates the Daiichi SW version DID
 *        (Data Identification) address.
 *
 * https://codebeamer.daiichi.com/issue/189458
 *
 */
#define API_DATA_LABEL_DID_SW_VER \
    (API_DATA_LABEL_DID_VEH_ID_NUM + API_DATA_DID_SIZE)

/**
 * @brief This macro indicates the Daiichi HW version DID
 *        (Data Identification) address.
 *
 * https://codebeamer.daiichi.com/issue/189459
 *
 */
#define API_DATA_LABEL_DID_HW_VER \
    (API_DATA_LABEL_DID_SW_VER + API_DATA_DID_SIZE)

/**
 * @brief This macro indicates the last data address of EEPROM.
 *
 * https://codebeamer.daiichi.com/issue/188809
 *
 */
#define API_DATA_EEPROM_LAST_DATA \
    (API_DATA_LABEL_DID_HW_VER + API_DATA_DID_SIZE)

/**
 * @brief This macro indicates the audio file - 1 configuration table address to
 *        be saved in Flash.
 *
 * https://codebeamer.daiichi.com/issue/189231
 *
 */
#define API_DATA_LABEL_AUDIO_1_CFG (API_DATA_EEPROM_LAST_DATA)

/**
 * @brief This macro indicates the audio file - 1 data address.
 *
 * https://codebeamer.daiichi.com/issue/188784
 *
 */
#define API_DATA_LABEL_AUDIO_1 \
    (API_DATA_LABEL_AUDIO_1_CFG + API_DATA_AUDIO_CFG_TBL_SIZE)

/**
 * @brief This macro indicates the audio file - 2 configuration table address to
 *        be saved in Flash.
 *
 * https://codebeamer.daiichi.com/issue/189234
 *
 */
#define API_DATA_LABEL_AUDIO_2_CFG \
    (API_DATA_LABEL_AUDIO_1 + API_DATA_AUDIO_DEFAULT_SIZE)

/**
 * @brief This macro indicates the audio file - 2 data address.
 *
 * https://codebeamer.daiichi.com/issue/188785
 *
 */
#define API_DATA_LABEL_AUDIO_2 \
    (API_DATA_LABEL_AUDIO_2_CFG + API_DATA_AUDIO_CFG_TBL_SIZE)

/**
 * @brief This macro indicates the audio file - 3 configuration table address to
 *        be saved in Flash.
 *
 * https://codebeamer.daiichi.com/issue/189236
 *
 */
#define API_DATA_LABEL_AUDIO_3_CFG \
    (API_DATA_LABEL_AUDIO_2 + API_DATA_AUDIO_DEFAULT_SIZE)

/**
 * @brief This macro indicates the audio file - 3 data address.
 *
 * https://codebeamer.daiichi.com/issue/188786
 *
 */
#define API_DATA_LABEL_AUDIO_3 \
    (API_DATA_LABEL_AUDIO_3_CFG + API_DATA_AUDIO_CFG_TBL_SIZE)

/**
 * @brief This macro indicates the last data address of external flash.
 *
 * https://codebeamer.daiichi.com/issue/188787
 *
 */
#define API_DATA_FLASH_LAST_DATA \
    (API_DATA_LABEL_AUDIO_3 + API_DATA_AUDIO_DEFAULT_SIZE)

// PRQA S 380 --

/*******************************************************************************
 * Type Definitions
 ******************************************************************************/

/**
 * @brief Api Data callback function pointer for asynch transmissions.
 *
 * https://codebeamer.daiichi.com/issue/197674
 *
 */
typedef void (*api_data_cb_t)(bool err);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief This function initializes the component's variables and flash driver.
 *
 * https://codebeamer.daiichi.com/issue/186953
 *
 */
bool api_data_init(void);

/**
 * @brief This function reads the data in the given buffer to the Flash or the
 *        EEPROM.
 *
 * @param data_label Api data label.
 * @param p_data     Data pointer to be read.
 * @param size       Data size.
 * @param offset     It uses for shifting the audio file address.
 *
 * @return bool
 *
 * @retval true  Read operation successfully executed.
 * @retval false The read operation is not successfully executed.
 *
 * https://codebeamer.daiichi.com/issue/186956
 *
 */
bool api_data_read(uint32_t data_label,
                   uint8_t *p_data,
                   size_t size,
                   uint32_t offset);

/**
 * @brief This function writes the data in the given buffer to the flash or the
 *        EEPROM.
 *
 * @param data_label Api data label.
 * @param p_data     Data pointer to be written.
 * @param size       Data size.
 *
 * @return bool
 *
 * @retval true  Write operation successfully executed.
 * @retval false The write operation is not successfully executed.
 *
 * https://codebeamer.daiichi.com/issue/186958
 *
 */
bool api_data_write(uint32_t data_label,
                    const uint8_t *p_data,
                    size_t size);

/**
 * @brief This function packs the given DTC number, vehicle speed, current gear,
 *        and trans gear values.
 *
 * @param data_label Api data label.
 * @param dtc        Specifies the DTC number.
 *
 * @return bool Function status.
 *
 * @retval true
 * @retval false
 *
 * https://codebeamer.daiichi.com/issue/191739
 *
 */
bool api_data_dtc_set(uint32_t data_label, uint32_t dtc);

/**
 * @brief Registration the given callback.
 *
 * https://codebeamer.daiichi.com/issue/197672
 *
 * @param[in] callback The function pointer.
 *
 * @return true  The callback is registered successfully.
 * @return false Error.
 */
bool api_data_cb_set(api_data_cb_t callback);

/**
 * @brief This function calls necessary driver-level processes to
 *        complete ongoing data transmissions.
 *
 *        This function should be called continuously for the
 *        flash operations to work correctly.
 *
 * https://codebeamer.daiichi.com/issue/197616
 *
 */
void api_data_process(void);

#endif /* API_DATA_H */

// end of file
