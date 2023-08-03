// PRQA S 3108 EOF # Comments with links are required for ensure traceability.
/**
 * @file api_audio.h
 * @author berke yalcinkaya (berke.yalcinkaya@daiichi.com)
 * @brief  Provides to manages the playing audio operations.
 * @version 0.1
 * @date 2023-06-02
 *
 * @copyright Copyright (c) 2023
 *
 * https://codebeamer.daiichi.com/issue/177735
 * https://codebeamer.daiichi.com/issue/192422
 *
 */

#ifndef API_AUDIO_H
#define API_AUDIO_H

#include "daiichi_types.h"

/*******************************************************************************
 * Macros
 ******************************************************************************/

/**
 * @brief This macro indicates the first audio file number.
 *
 * https://codebeamer.daiichi.com/issue/192453
 *
 */
#define API_AUDIO_1 (0U)

/**
 * @brief This macro indicates the second audio file number.
 *
 * https://codebeamer.daiichi.com/issue/192454
 *
 */
#define API_AUDIO_2 (1U)

/**
 * @brief This macro indicates the third audio file number.
 *
 * https://codebeamer.daiichi.com/issue/192455
 *
 */
#define API_AUDIO_3 (2U)

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief This function inits the component's variables and the amplifier driver
 *
 * @return bool Initialization status.
 *
 * @retval true  Initialization has been successfully done.
 * @retval false Initialization failed.
 *               - EEPROM communication error.
 *               - EXT. Flash communication error.
 *
 * https://codebeamer.daiichi.com/issue/192456
 *
 */
bool api_audio_init(void);

/**
 * @brief This function provides to deinitialize the module. It resets the
 *        module control states, buffers, and module-related parameters.
 *
 * https://codebeamer.daiichi.com/issue/195198
 *
 */
void api_audio_deinit(void);

/**
 * @brief This function provides to check audio states.
 *
 * https://codebeamer.daiichi.com/issue/192457
 *
 */
void api_audio_process(void);

/**
 * @brief This function mutes the sound through the amplifier.
 *
 * @return bool Mute status.
 *
 * @retval true  Mute is successfully done.
 * @retval false Mute is failed.
 *               - API-AUDIO module did not initialize.
 *               - Amplifier communication error.
 *
 * https://codebeamer.daiichi.com/issue/192460
 *
 */
bool api_audio_mute(void);

/**
 * @brief This function unmutes the sound through the amplifier.
 *
 * @return bool Unmute status.
 *
 * @retval true  Unmute is successfully done.
 * @retval false Unmute is failed.
 *               - API-AUDIO module did not initialize.
 *               - Amplifier communication error.
 *
 * https://codebeamer.daiichi.com/issue/192461
 *
 */
bool api_audio_unmute(void);

/**
 * @brief This function starts sending the audio data to the amplifier via I2S.
 *
 * @return bool Audio play status.
 *
 * @retval true  Audio play is successfully done.
 * @retval false Audio play is failed.
 *               - API-AUDIO module did not initialize.
 *               - Audio buffers are still not ready.
 *
 * https://codebeamer.daiichi.com/issue/192462
 *
 */
bool api_audio_play(void);

/**
 * @brief This function stops sending the audio data to the amplifier via I2S.
 *
 * @return bool Function status.
 *
 * @retval true  The audio pause is successfully done.
 * @retval false The audio pause failed.
 *
 * https://codebeamer.daiichi.com/issue/197381
 *
 */
bool api_audio_pause(void);

/**
 * @brief This function returns the audio module play status.
 *
 * @return bool Function status
 *
 * @retval true  The audio module is playing.
 * @retval false The audio module is paused.
 *
 * https://codebeamer.daiichi.com/issue/197382
 *
 */
bool api_audio_status_get(void);

/**
 * @brief This function updates frequency and gain variables from the
 *        configuration table according to speed value.
 *
 * @param[in] speed Indicates vehicle speed.
 *
 * https://codebeamer.daiichi.com/issue/192463
 *
 */
void api_audio_param_update(uint16_t speed);

/**
 * @brief This function switches the audio file to be played.
 *
 * @param[in] audio_num Audio number to be selected.
 *                      - API_AUDIO_1
 *                      - API_AUDIO_2
 *                      - API_AUDIO_3
 *
 * @return bool Function status.
 *
 * @retval true  The audio number switch is successful.
 * @retval false The audio number switch failed.
 *
 * https://codebeamer.daiichi.com/issue/192464
 *
 */
bool api_audio_switch(uint8_t audio_num);

/**
 * @brief This function gives the audio name from the audio configuration table.
 *
 * @param[out] p_name The audio file name.
 *
 * @return bool Function status.
 *
 * @retval true  Function successfully done.
 * @retval false Function failed.
 *
 * https://codebeamer.daiichi.com/issue/194199
 *
 */
bool api_audio_name_get(char_t *p_name);

/**
 * @brief This function gives the audio size from the audio configuration table.
 *
 * @param[out] p_size The audio file size.
 *
 * @return bool Function status.
 *
 * @retval true  Function successfully done.
 * @retval false Function failed.
 *
 * https://codebeamer.daiichi.com/issue/194200
 *
 */
bool api_audio_size_get(uint32_t *p_size);

/**
 * @brief This function gives the audio calibration date from the audio
 *        configuration table.
 *
 * @param[out] p_calib_date The audio calibration date.
 *
 * @return bool Function status.
 *
 * @retval true  Function successfully done.
 * @retval false Function failed.
 *
 * https://codebeamer.daiichi.com/issue/194201
 *
 */
bool api_audio_calib_date_get(char_t *p_calib_date);

/**
 * @brief This function gives the audio calibration tool version from the audio
 *        configuration table.
 *
 * @param[out] p_tool_version The audio calibration tool.
 *
 * @return bool Function status.
 *
 * @retval true  Function successfully done.
 * @retval false Function failed.
 *
 * https://codebeamer.daiichi.com/issue/194202
 *
 */
bool api_audio_calib_tool_get(char_t *p_tool_version);

/**
 * @brief This function gives the audio calibration version from the audio
 *        configuration table.
 *
 * @param[out] p_calib_version The audio calibration version.
 *
 * @return bool Function status.
 *
 * @retval true  Function successfully done.
 * @retval false Function failed.
 *
 * https://codebeamer.daiichi.com/issue/194203
 *
 */
bool api_audio_calib_ver_get(char_t *p_calib_version);

/**
 * @brief This function gives the audio frequency from the audio configuration
 *        table.
 *
 * @param[out] p_freq The audio frequency.
 * @param[in] offset The frequency offset value in the configuration table.
 *
 * @return bool Function status.
 *
 * @retval true  Function successfully done.
 * @retval false Function failed.
 *
 * https://codebeamer.daiichi.com/issue/194204
 *
 */
bool api_audio_freq_get(uint16_t *p_freq, uint32_t offset);

/**
 * @brief This function gives the audio gain from the audio configuration table.
 *
 * @param[out] p_gain The audio gain.
 * @param[in] offset The gain offset value in the configuration table.
 *
 * @return bool Function status.
 *
 * @retval true  Function successfully done.
 * @retval false Function failed.
 *
 * https://codebeamer.daiichi.com/issue/194205
 *
 */
bool api_audio_gain_get(uint16_t *p_gain, uint32_t offset);

/**
 * @brief This function gives the audio version from the audio configuration
 *        table.
 *
 * @param[out] p_version The audio version.
 *
 * @return bool Function status.
 *
 * @retval true  Function successfully done.
 * @retval false Function failed.
 *
 * https://codebeamer.daiichi.com/issue/194206
 *
 */
bool api_audio_ver_get(char_t *p_version);

#endif /* API_AUDIO_H */

// end of file
