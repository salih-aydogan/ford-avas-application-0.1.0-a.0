// PRQA S 3108 EOF # Comments with links are required for ensure traceability.
/**
 * @file api_audio.h
 * @author berke yalcinkaya (berke.yalcinkaya@daiichi.com)
 * @brief  This module is the implementation of the API Audio interface.
 * @version 0.1
 * @date 2023-06-02
 *
 * @copyright Copyright (c) 2023
 *
 * https://codebeamer.daiichi.com/issue/193677
 *
 */

#include "api_audio.h"

#include "api_data.h"
#include "bsp_i2s.h"
#include "drv_amplifier.h"

#include "log.h"

LOG_MODULE_NAME("API_AUDIO");

#include "drv_flash.h" // TODO


/*******************************************************************************
 * Macros
 ******************************************************************************/

/**
 * @brief This macro indicates the audio buffer size.
 *
 * https://codebeamer.daiichi.com/issue/194275
 *
 */
#define AUDIO_BUFF_SIZE (2048U)

/**
 * @brief This macro indicates the audio mute buffer size.
 *
 * https://codebeamer.daiichi.com/issue/194276
 *
 */
#define AUDIO_MUTE_BUFF_SIZE (100U)

/**
 * @brief This macro indicates the default value of the audio shifting rate.
 *
 * https://codebeamer.daiichi.com/issue/194286
 *
 */
#define AUDIO_SHIFTING_RATE (1U)

/**
 * @brief This macro indicates the Audio calculation rate.
 *
 * https://codebeamer.daiichi.com/issue/195752
 *
 */
#define AUDIO_GAIN_CALC_RATE (1000.0)

/**
 * @brief This macro indicates the audio offset value in the External Flash.
 *
 * https://codebeamer.daiichi.com/issue/194315
 *
 */
#define AUDIO_OFFSET (API_DATA_AUDIO_CFG_TBL_SIZE + API_DATA_AUDIO_DEFAULT_SIZE)

/**
 * @brief This macro indicates the audio name (Audio File Name) parameter size
 *        in the audio configuration table.
 *
 * https://codebeamer.daiichi.com/issue/194316
 *
 */
#define CFG_TBL_NAME_PARAM_SIZE (32U)

/**
 * @brief This macro indicates the audio size (Audio File Size) parameter size
 *        in the audio configuration table.
 *
 * https://codebeamer.daiichi.com/issue/194317
 *
 */
#define CFG_TBL_SIZE_PARAM_SIZE (4U)

/**
 * @brief This macro indicates the audio version (Audio File Version) parameter
 *        size in the audio configuration table.
 *
 * https://codebeamer.daiichi.com/issue/194318
 *
 */
#define CFG_TBL_VER_PARAM_SIZE (4U)

/**
 * @brief This macro indicates the audio calibration version parameter size in
 *        the audio configuration table.
 *
 * https://codebeamer.daiichi.com/issue/194319
 *
 */
#define CFG_TBL_CALIB_VER_PARAM_SIZE (4U)

/**
 * @brief This macro indicates the audio calibration tool version parameter size
 *        in the audio configuration table.
 *
 * https://codebeamer.daiichi.com/issue/194320
 *
 */
#define CFG_TBL_CALIB_TOOL_VER_PARAM_SIZE (4U)

/**
 * @brief This macro indicates the audio calibration date parameter size in the
 *        audio configuration table.
 *
 * https://codebeamer.daiichi.com/issue/194321
 *
 */
#define CFG_TBL_CALIB_DATE_PARAM_SIZE (16U)

/**
 * @brief This macro indicates the max sample parameter (Audio Frequency Shift
 *        Denominator) size in audio configuration table.
 *
 * https://codebeamer.daiichi.com/issue/198117
 *
 */
#define CFG_TBL_MAX_SAMPLE_CNT_SIZE (2U)

/**
 * @brief This macro indicates the reserved size in the audio configuration
 *        table.
 *
 * https://codebeamer.daiichi.com/issue/194322
 *
 */
#define CFG_TBL_RESERVED_SIZE (34U)

/**
 * @brief This macro indicates the audio gain parameter size in the audio
 *        configuration table.
 *
 * https://codebeamer.daiichi.com/issue/194323
 *
 */
#define CFG_TBL_GAIN_PARAM_SIZE (200U)

/**
 * @brief This macro indicates the audio name (Audio File Name) offset in the
 *        audio configuration table.
 *
 * https://codebeamer.daiichi.com/issue/194325
 *
 */
#define CFG_TBL_AUDIO_NAME_OFFSET (0U)

/**
 * @brief This macro indicates the audio size (Audio File Size) offset in the
 *        audio configuration table.
 *
 * https://codebeamer.daiichi.com/issue/194326
 *
 */
#define CFG_TBL_SIZE_OFFSET (CFG_TBL_AUDIO_NAME_OFFSET + CFG_TBL_NAME_PARAM_SIZE)

/**
 * @brief This macro indicates the audio version (Audio File Version) offset
 *        in the audio configuration table.
 *
 * https://codebeamer.daiichi.com/issue/194327
 *
 */
#define CFG_TBL_VER_OFFSET (CFG_TBL_SIZE_OFFSET + CFG_TBL_SIZE_PARAM_SIZE)

/**
 * @brief This macro indicates the audio calibration version offset in the audio
 *        configuration table.
 *
 * https://codebeamer.daiichi.com/issue/194328
 *
 */
#define CFG_TBL_CALIB_VER_OFFSET (CFG_TBL_VER_OFFSET + CFG_TBL_VER_PARAM_SIZE)

/**
 * @brief This macro indicates the audio calibration tool version offset in the
 *        audio configuration table.
 *
 * https://codebeamer.daiichi.com/issue/194329
 *
 */
#define CFG_TBL_CALIB_TOOL_VER_OFFSET \
    (CFG_TBL_CALIB_VER_OFFSET + CFG_TBL_CALIB_VER_PARAM_SIZE)

/**
 * @brief This macro indicates the audio calibration date offset in the audio
 *        configuration table.
 *
 * https://codebeamer.daiichi.com/issue/194330
 *
 */
#define CFG_TBL_CALIB_DATE_OFFSET \
    (CFG_TBL_CALIB_TOOL_VER_OFFSET + CFG_TBL_CALIB_TOOL_VER_PARAM_SIZE)

/**
 * @brief This macro indicates the max sample parameter (Audio Frequency Shift
 *        Denominator) offset in audio configuration table.
 *
 * https://codebeamer.daiichi.com/issue/198122
 *
 */
#define CFG_TBL_MAX_SAMPLE_CNT_OFFSET \
    (CFG_TBL_CALIB_DATE_OFFSET + CFG_TBL_CALIB_DATE_PARAM_SIZE)

/**
 * @brief This macro indicates the reserved area offset in the audio
 *        configuration table.
 *
 * https://codebeamer.daiichi.com/issue/194331
 *
 */
#define CFG_TBL_RESERVED_OFFSET \
    (CFG_TBL_MAX_SAMPLE_CNT_OFFSET + CFG_TBL_MAX_SAMPLE_CNT_SIZE)

/**
 * @brief This macro indicates the audio gain value offset in the audio
 *        configuration table.
 *
 * https://codebeamer.daiichi.com/issue/194332
 *
 */
#define CFG_TBL_GAIN_OFFSET (CFG_TBL_RESERVED_OFFSET + CFG_TBL_RESERVED_SIZE)

/**
 * @brief This macro indicates the audio frequency (Audio Frequency Shift
 *        Nominator) value offset in the audio configuration table.
 *
 * https://codebeamer.daiichi.com/issue/194333
 *
 */
#define CFG_TBL_FREQ_OFFSET (CFG_TBL_GAIN_OFFSET + CFG_TBL_GAIN_PARAM_SIZE)

/*******************************************************************************
 * Enumerations
 ******************************************************************************/
// PRQA S 3213 ++ # The tag can not removed. False positive warning.
/**
 * @brief List of the audio component states.
 *
 * https://codebeamer.daiichi.com/issue/194334
 * https://codebeamer.daiichi.com/issue/194339
 *
 */
typedef enum audio_state_e
{
    AUDIO_STATE_INIT = 0U,
    AUDIO_STATE_READY = 1U,
    AUDIO_STATE_RUN = 2U,
    AUDIO_STATE_SWITCH = 3U,

} audio_state_t;

/**
 * @brief List of the External flash read states.
 *
 * https://codebeamer.daiichi.com/issue/194335
 * https://codebeamer.daiichi.com/issue/194340
 *
 */
typedef enum flash_read_state_e
{
    FLASH_READ_STATE_IDLE = 0U,
    FLASH_READ_STATE_INIT = 1U,
    FLASH_READ_STATE_CFG_TBL = 2U,
    FLASH_READ_STATE_CURR_AUDIO_BUFF = 3U,
    FLASH_READ_STATE_SPARE_AUDIO_BUFF = 4U,

} flash_read_state_t;

/**
 * @brief List of the audio buffer states.
 *
 * https://codebeamer.daiichi.com/issue/194336
 * https://codebeamer.daiichi.com/issue/194341
 *
 */
typedef enum buffer_state_e
{
    BUFFER_STATE_IDLE = 0U,
    BUFFER_STATE_READ = 1U,
    BUFFER_STATE_READ_WAIT = 2U,
    BUFFER_STATE_EDIT = 3U,
    BUFFER_STATE_READY = 4U,
    BUFFER_STATE_BUSY = 5U,

} buffer_state_t;

/*******************************************************************************
 * Structs
 ******************************************************************************/

/**
 * @brief Data structure corresponds to the audio-related parameters.
 *
 * https://codebeamer.daiichi.com/issue/194337
 * https://codebeamer.daiichi.com/issue/194342
 *
 */
typedef struct audio_param_s
{
    uint8_t curr_audio;

    bool mute;

    bool play;

    audio_state_t state;

    uint32_t cursor;
    uint32_t size;

    uint32_t audio_addr;
    uint32_t cfg_tbl_addr;

    uint16_t curr_gain;
    uint16_t max_sample_cnt;

} audio_param_t;

/**
 * @brief Data structure corresponds to the audio buffer related parameters.
 *
 * https://codebeamer.daiichi.com/issue/194338
 * https://codebeamer.daiichi.com/issue/194343
 *
 */
typedef struct audio_buff_s
{
    uint8_t *p_buff;
    uint32_t max_size;
    uint32_t cur_size;
    uint16_t shifting_rate;
    buffer_state_t buff_state;

} audio_buff_t;

// PRQA S 3108 --

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/**
 * @brief Holds the sample of the audio file in the External Flash up to
 *        AUDIO_BUFF_SIZE.
 *
 * https://codebeamer.daiichi.com/issue/194344
 *
 */
static uint8_t m_audio_buff_1[AUDIO_BUFF_SIZE] = {0U};

/**
 * @brief Holds the sample of the audio file in the External Flash up to
 *        AUDIO_BUFF_SIZE.
 *
 * https://codebeamer.daiichi.com/issue/194345
 *
 */
static uint8_t m_audio_buff_2[AUDIO_BUFF_SIZE] = {0U};

/**
 * @brief Holds the zeros to be played in mute state.
 *
 * https://codebeamer.daiichi.com/issue/194346
 *
 */
static uint8_t m_audio_mute_buff[AUDIO_MUTE_BUFF_SIZE] = {0U};

/**
 * @brief Holds parameters of the configuration table.
 *
 * https://codebeamer.daiichi.com/issue/194347
 *
 */
static uint8_t m_audio_cfg_tbl[API_DATA_AUDIO_CFG_TBL_SIZE] = {0U};

/**
 * @brief Holds the API audio module parameters.
 *
 * https://codebeamer.daiichi.com/issue/194362
 *
 */
static audio_param_t m_audio_param = {0U};

/**
 * @brief Holds the current audio related parameters.
 *
 * https://codebeamer.daiichi.com/issue/194348
 *
 */
static audio_buff_t m_curr_buff = {

    .p_buff = m_audio_buff_1,
    .max_size = AUDIO_BUFF_SIZE,
    .cur_size = AUDIO_BUFF_SIZE,
    .shifting_rate = AUDIO_SHIFTING_RATE,
    .buff_state = BUFFER_STATE_IDLE,
};

/**
 * @brief Holds the spare audio buffer related parameters.
 *
 * https://codebeamer.daiichi.com/issue/194349
 *
 */
static audio_buff_t m_spare_buff = {

    .p_buff = m_audio_buff_2,
    .max_size = AUDIO_BUFF_SIZE,
    .cur_size = AUDIO_BUFF_SIZE,
    .shifting_rate = AUDIO_SHIFTING_RATE,
    .buff_state = BUFFER_STATE_IDLE,
};

/**
 * @brief Holds the External flash read states.
 *
 * https://codebeamer.daiichi.com/issue/194350
 *
 */
static flash_read_state_t m_flash_read_state = FLASH_READ_STATE_IDLE;

/**
 * @brief Holds the Configuration buffer status.
 *
 * https://codebeamer.daiichi.com/issue/196956
 *
 */
static buffer_state_t m_cfg_tbl_state = BUFFER_STATE_IDLE;

/**
 * @brief Holds the API audio module initialization status.
 *
 * https://codebeamer.daiichi.com/issue/194351
 *
 */
static bool m_api_audio_init = false;

/*******************************************************************************
 * Private Function Declarations
 ******************************************************************************/

/**
 * @brief This function returns the uint16_t variable from the given array.
 *
 * @param[out] p_data Variable starting address inside of an array.
 *
 * @return uint16_t Variable value.
 *
 * https://codebeamer.daiichi.com/issue/194850
 *
 */
static inline uint16_t uint16_convert(const uint8_t *p_data);

/**
 * @brief This function returns the uint32_t variable from the given array.

 *
 * @param[out] p_data Variable starting address inside of an array.
 *
 * @return uint32_t Variable value.
 *
 * https://codebeamer.daiichi.com/issue/194914
 *
 */
static uint32_t uint32_convert(const uint8_t *p_data);

/**
 * @brief This function provides to update the shifting rate according to the
 *        vehicle speed.
 *
 * @param[in] speed The vehicle speed.
 *
 * https://codebeamer.daiichi.com/issue/194435
 *
 */
static inline void shifting_rate_update(uint16_t speed);

/**
 * @brief This function provides to calculate the removed sample address in the
 *        audio buffer.
 *
 * @param[in] max_sample_cnt  The number of maximum edit parts of the buffer.
 * @param[in] shifting_rate The shifting rate.
 *
 * @return uint32_t The to-be-removed sample address is in the audio buffer.
 *
 * https://codebeamer.daiichi.com/issue/194436
 *
 */
static inline uint32_t sample_pos_get(uint16_t max_sample_cnt,
                                      uint16_t shifting_rate);

/**
 * @brief This function provides to shift of the audio buffer.
 *
 * @param[in] p_audio Audio buffer structure.
 *
 * @return uint32_t The total number of samples to be removed.
 *
 * https://codebeamer.daiichi.com/issue/194261
 *
 */
static uint32_t audio_buff_freq_shift(const audio_buff_t *p_audio);

/**
 * @brief This function provides to edit dB of the audio buffer.
 *
 * @param[out] p_buff The audio buffer pointer.
 * @param[in]  size   The audio buffer size.
 * @param[in]  gain   The audio gain.
 *
 * https://codebeamer.daiichi.com/issue/194437
 *
 */
static inline void audio_buff_db_edit(uint8_t *p_buff,
                                      uint32_t size,
                                      uint16_t gain);

/**
 * @brief This function provides to edit the audio buffer.
 *
 * @param[out] p_audio The audio buffer structure pointer.
 * @param[in]  gain    The audio gain.
 *
 * https://codebeamer.daiichi.com/issue/194263
 *
 */
static void audio_buff_edit(audio_buff_t *p_audio, uint16_t gain);

/**
 * @brief This function provides to update of the audio sample-related parameter
 *        such as gain and shifting rate.
 *
 * @param[in] speed The vehicle speed.
 *
 * https://codebeamer.daiichi.com/issue/194264
 *
 */
static inline void audio_param_update(uint16_t speed);

/**
 * @brief This function provides the reset private variable.
 *
 * https://codebeamer.daiichi.com/issue/194260
 *
 */
static inline void local_var_reset(void);

/**
 * @brief This function updates the variable holding the play status of the module.
 *
 * @param[in] status Audio module playing status.
 *
 * https://codebeamer.daiichi.com/issue/195191
 *
 */
static inline void audio_play_set(bool status);

/**
 * @brief This function gets the variable holding the play status of the module.
 *
 * @return bool Status of audio module play function.
 *
 * @retval true  The module is playing.
 * @retval false The module is not playing.
 *
 * https://codebeamer.daiichi.com/issue/195192
 *
 */
static inline bool audio_play_get(void);

/**
 * @brief This function provides to get audio samples from the External Flash
 *        via api_data_read function.
 *
 * @param[out] p_buff Audio buffer pointer.
 * @param[in]  offset Specifies the sample which points by cursor parameter.
 *
 * https://codebeamer.daiichi.com/issue/194262
 *
 */
static inline void audio_buff_get(uint8_t *p_buff, 
                                  uint32_t offset, 
                                  uint32_t size);

/**
 * @brief This function provides to manages audio buffers.
 *
 * @param[out] pp_audio Audio buffer structure array pointer.
 * @param[in]  buff_cnt Buffer count.
 *
 * https://codebeamer.daiichi.com/issue/194265
 *
 */
static void audio_buff_check(audio_buff_t **pp_audio, uint32_t buff_cnt);

/**
 * @brief This function provides to manages the buffer-filling operations.
 *
 * https://codebeamer.daiichi.com/issue/194089
 *
 */
static void flash_state_manager(void);

/**
 * @brief This function provides to check the audio buffer and flash are in the
 *        ready state.
 *
 * @return bool Audio init finish status.
 *
 * @retval true  The flash control state is in FLASH_READ_STATE_INIT
 * @return false Buffers are not in BUFFER_STATE_READY
 *
 * https://codebeamer.daiichi.com/issue/194439
 *
 */
static bool audio_init_state_check(void);

/**
 * @brief This function provides to sends the buffer to the audio amplifier.
 *
 * @param[in] p_buff The audio buffer pointer.
 * @param[in] size   The audio buffer size.
 *
 * https://codebeamer.daiichi.com/issue/194253
 *
 */
static inline bool audio_play(const uint8_t *p_buff, uint32_t size);

/**
 * @brief This function provides to check the buffer state.
 *
 * https://codebeamer.daiichi.com/issue/194259
 *
 */
static void audio_run_state_check(void);

/**
 * @brief This function provides to manages the API audio module in the process.
 *
 * https://codebeamer.daiichi.com/issue/194087
 *
 */
static void state_manager(void);

/**
 * @brief This function provides to switch current buffer to the spare buffer.
 *
 * https://codebeamer.daiichi.com/issue/194488
 *
 */
static inline void audio_buff_curr_to_spare_switch(void);

/**
 * @brief This function provides to switch current buffer to the mute buffer.
 *
 * https://codebeamer.daiichi.com/issue/194486
 *
 */
static inline void audio_buff_curr_to_mute_switch(void);

/**
 * @brief This function provides to switch the current buffer.
 *
 * @param[in] status Callback event.
 *
 * https://codebeamer.daiichi.com/issue/194090
 *
 */
static void audio_write_cb(bsp_i2s_status_t status);

/**
 * @brief This function provides to get the Audio module's current state.
 *
 * @return audio_state_t Audio module state.
 *
 * https://codebeamer.daiichi.com/issue/194797
 *
 */
static inline audio_state_t curr_audio_state_get(void);

/**
 * @brief This function provides to set the Audio module's current state.
 *
 * @param[in] state Audio module state.
 *
 * https://codebeamer.daiichi.com/issue/194798
 *
 */
static inline void curr_audio_state_set(audio_state_t state);

/**
 * @brief This function provides to get the Audio module's current state.
 *
 * @return buffer_state_t Audio module state.
 *
 * https://codebeamer.daiichi.com/issue/194797
 *
 */
static inline buffer_state_t curr_buff_state_get(void);

/**
 * @brief This function provides to set the current audio buffer's state.
 *
 * @param[in] state Current audio buffer state.
 *
 * https://codebeamer.daiichi.com/issue/194800
 *
 */
static inline void curr_buff_state_set(buffer_state_t state);

/**
 * @brief This function provides to get the spare audio buffer's state.
 *
 * @return buffer_state_t Audio buffer control state.
 *
 * https://codebeamer.daiichi.com/issue/194801
 *
 */
static inline buffer_state_t spare_buff_state_get(void);

/**
 * @brief This function provides to set the spare audio buffer's state.
 *
 * @param[in] state Spare audio buffer state.
 *
 * https://codebeamer.daiichi.com/issue/194802
 *
 */
static inline void spare_buff_state_set(buffer_state_t state);

/**
 * @brief This function provides to get the current external flash control state.
 *
 * @return flash_read_state_t External Flash control state.
 *
 * https://codebeamer.daiichi.com/issue/194803
 *
 */
static inline flash_read_state_t curr_flash_state_get(void);

/**
 * @brief This function provides to set the current external flash control state.
 *
 * @param[in] state External Flash control state.
 *
 * https://codebeamer.daiichi.com/issue/194804
 *
 */
static inline void curr_flash_state_set(flash_read_state_t state);

/**
 * @brief This function provides to get the audio address.
 *
 * @return uint32_t Audio address.
 *
 * https://codebeamer.daiichi.com/issue/194805
 *
 */
static inline uint32_t audio_addr_get(void);

/**
 * @brief This function provides to calculate the given audio address and the
 *        configuration table address.
 *
 * @param[in] curr_audio The current audio number.
 *
 * https://codebeamer.daiichi.com/issue/194438
 *
 */
static inline void audio_addr_set(uint8_t curr_audio);

/**
 * @brief This function gets the variable holding the address of the
 *        configuration table.
 *
 * @return uint32_t Configuration Table address.
 *
 * https://codebeamer.daiichi.com/issue/194807
 *
 */
static inline uint32_t cfg_tbl_addr_get(void);

/**
 * @brief This function updates the variable holding the address of the
 *        configuration table.
 *
 * @param[in] curr_audio Current audio number.
 *
 * https://codebeamer.daiichi.com/issue/194808
 *
 */
static inline void cfg_tbl_addr_set(uint8_t curr_audio);

/**
 * @brief This function updates the variable holding the current audio.
 *
 * @param[in] audio Current audio number.
 *
 * https://codebeamer.daiichi.com/issue/194809
 *
 */
static inline void curr_audio_set(uint8_t audio);

/**
 * @brief This function gets the variable holding the current audio value.
 *
 * @return uint8_t Value of the current audio.
 *
 * @retval API_AUDIO_1 Specifies the audio-1.
 * @retval API_AUDIO_2 Specifies the audio-2.
 * @retval API_AUDIO_3 Specifies the audio-3.
 *
 * https://codebeamer.daiichi.com/issue/195193
 *
 */
static inline uint8_t curr_audio_get(void);

/**
 * @brief This function updates the variable holding the configuration table
 *        status.
 *
 * @param[in] status Configuration buffer status.
 *
 * https://codebeamer.daiichi.com/issue/196950
 *
 */
static inline void cfg_tbl_state_set(buffer_state_t status);

/**
 * @brief This function gets the variable holding the configuration table
 *        status.
 *
 * @return buffer_state_t Value of the current configuration table status.
 *
 * @retval BUFFER_STATE_READY
 * @retval BUFFER_STATE_IDLE
 *
 * https://codebeamer.daiichi.com/issue/196951
 *
 */
static inline buffer_state_t cfg_tbl_state_get(void);

/**
 * @brief This function indicates the completion of the reading audio or
 *        configuration table from External Flash.
 *
 * https://codebeamer.daiichi.com/issue/197524
 *
 */
static void ext_flash_cb(bool err);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/194088
 *
 */
bool api_audio_init(void)
{
    local_var_reset();

    uint8_t curr_audio = API_AUDIO_1;

    bool ret = api_data_read(API_DATA_LABEL_CURR_AUDIO,
                             &curr_audio,
                             1U,
                             0U);

    if (false == ret)
    {
        LOG_ERROR("Current audio could not read");

        curr_audio_set(API_AUDIO_1);
    }
    else
    {
        curr_audio_set(curr_audio);
    }

    cfg_tbl_addr_set(curr_audio);

    audio_addr_set(curr_audio);

    uint32_t addr = cfg_tbl_addr_get();

    ret = api_data_read(addr,
                        m_audio_cfg_tbl,
                        API_DATA_AUDIO_CFG_TBL_SIZE,
                        0U);

    if (false == ret)
    {
        LOG_ERROR("Audio cfg table could not read");

        return false;
    }

    (void)bsp_i2s_cb_set(audio_write_cb);

    (void)api_data_cb_set(ext_flash_cb);

    cfg_tbl_state_set(BUFFER_STATE_READ);

    curr_flash_state_set(FLASH_READ_STATE_CFG_TBL);

    m_api_audio_init = true;

    LOG_INFO("Initialized");

    return true;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/195210
 *
 */
void api_audio_deinit(void)
{
    local_var_reset();

    m_api_audio_init = false;

    LOG_INFO("Denitialized");
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194092
 *
 */
void api_audio_process(void)
{
    if (false == m_api_audio_init)
    {
        LOG_ERROR("API_AUDIO not initilized");

        return;
    }

    if (false == drv_flash_is_busy()) // TODO
    {
        state_manager();
    }

    if (false == drv_flash_is_busy()) // TODO
    {
        flash_state_manager();
    }
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194086
 *
 */
bool api_audio_mute(void)
{
    if (false == m_api_audio_init)
    {
        LOG_ERROR("API_AUDIO not initilized");

        return false;
    }

    bool ret = drv_amplifier_mute();

    if (false == ret)
    {
        LOG_ERROR("Amplifier mute error");

        return false;
    }
    else
    {
        LOG_INFO("Amplifier mute");
    }

    m_audio_param.mute = true;

    if (BUFFER_STATE_READY == curr_buff_state_get())
    {
        audio_buff_curr_to_mute_switch();
    }

    return true;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194091
 *
 */
bool api_audio_unmute(void)
{
    if (false == m_api_audio_init)
    {
        LOG_ERROR("API_AUDIO not initilized");

        return false;
    }

    bool ret = drv_amplifier_unmute();

    if (false == ret)
    {
        LOG_ERROR("Amplifier unmute error");

        return false;
    }
    else
    {
        LOG_INFO("Amplifier unmute");
    }

    m_audio_param.mute = false;

    if (BUFFER_STATE_READY == curr_buff_state_get())
    {
        audio_buff_curr_to_spare_switch();
    }

    return true;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194095
 *
 */
bool api_audio_play(void)
{
    if (false == m_api_audio_init)
    {
        LOG_ERROR("API_AUDIO not initilized");

        return false;
    }

    bool ret = false;

    audio_play_set(true);

    LOG_INFO("Audio play");

    if (AUDIO_STATE_READY == curr_audio_state_get())
    {
        curr_audio_state_set(AUDIO_STATE_RUN);

        ret = true;
    }

    return ret;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/197386
 *
 */
bool api_audio_pause(void)
{
    if (false == m_api_audio_init)
    {
        LOG_ERROR("API_AUDIO not initilized");

        return false;
    }

    audio_play_set(false);

    LOG_INFO("Audio pause");

    return true;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/197387
 *
 */
bool api_audio_status_get(void)
{
    bool status = audio_play_get();

    return status;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194093
 *
 */
void api_audio_param_update(uint16_t speed)
{
    if (false == m_api_audio_init)
    {
        LOG_ERROR("API_AUDIO not initilized");

        return;
    }

    audio_param_update(speed);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194094
 *
 */
bool api_audio_switch(uint8_t audio_num)
{
    if (false == m_api_audio_init)
    {
        LOG_ERROR("API_AUDIO not initilized");

        return false;
    }

    uint8_t curr_audio = curr_audio_get();

    if (audio_num == curr_audio)
    {
        return true;
    }

    if (API_AUDIO_3 < curr_audio)
    {
        LOG_ERROR("Invalid audio number for switching: %u", curr_audio);

        return false;
    }

    curr_audio_set(audio_num);

    cfg_tbl_addr_set(audio_num);

    audio_addr_set(audio_num);

    curr_audio_state_set(AUDIO_STATE_SWITCH);

    LOG_INFO("Switch Audio-%u to Audio-%u",
             (m_audio_param.curr_audio),
             (audio_num));

    return true;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194477
 *
 */
bool api_audio_name_get(char_t *p_name)
{
    if (NULL == p_name)
    {
        LOG_ERROR("Invalid argument in api_audio_name_get");

        return false;
    }

    if ((true == m_api_audio_init) &&
        (BUFFER_STATE_READY == cfg_tbl_state_get()))
    {
        const char_t *p_tmp =
            (char_t *)&m_audio_cfg_tbl[CFG_TBL_AUDIO_NAME_OFFSET];

        (void)strncpy(p_name, p_tmp, CFG_TBL_NAME_PARAM_SIZE);

        p_name[CFG_TBL_NAME_PARAM_SIZE] = '\0';

        return true;
    }

    return false;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194478
 *
 */
bool api_audio_size_get(uint32_t *p_size)
{
    if (NULL == p_size)
    {
        LOG_ERROR("Invalid argument in api_audio_size_get");

        return false;
    }

    if ((true == m_api_audio_init) &&
        (BUFFER_STATE_READY == cfg_tbl_state_get()))
    {
        const uint8_t *p_addr = &m_audio_cfg_tbl[CFG_TBL_SIZE_OFFSET];

        *p_size = uint32_convert(p_addr);

        return true;
    }

    return false;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194479
 *
 */
bool api_audio_calib_date_get(char_t *p_calib_date)
{
    if (NULL == p_calib_date)
    {
        LOG_ERROR("Invalid argument in api_audio_calib_date_get");

        return false;
    }

    if ((true == m_api_audio_init) &&
        (BUFFER_STATE_READY == cfg_tbl_state_get()))
    {
        const char_t *p_tmp =
            (char_t *)&m_audio_cfg_tbl[CFG_TBL_CALIB_DATE_OFFSET];

        (void)strncpy(p_calib_date,
                      p_tmp,
                      CFG_TBL_CALIB_DATE_PARAM_SIZE);

        p_calib_date[CFG_TBL_CALIB_DATE_PARAM_SIZE] = '\0';

        return true;
    }

    return false;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194480
 *
 */
bool api_audio_calib_tool_get(char_t *p_tool_version)
{
    if (NULL == p_tool_version)
    {
        LOG_ERROR("Invalid argument in api_audio_calib_tool_get");

        return false;
    }

    if ((true == m_api_audio_init) &&
        (BUFFER_STATE_READY == cfg_tbl_state_get()))
    {
        const char_t *p_addr =
            (char_t *)&m_audio_cfg_tbl[CFG_TBL_CALIB_TOOL_VER_OFFSET];

        (void)strncpy(p_tool_version,
                      p_addr,
                      CFG_TBL_CALIB_TOOL_VER_PARAM_SIZE);

        p_tool_version[CFG_TBL_CALIB_TOOL_VER_PARAM_SIZE] = '\0';

        return true;
    }

    return false;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194481
 *
 */
bool api_audio_calib_ver_get(char_t *p_calib_version)
{
    if (NULL == p_calib_version)
    {
        LOG_ERROR("Invalid argument in api_audio_calib_ver_get");

        return false;
    }

    if ((true == m_api_audio_init) &&
        (BUFFER_STATE_READY == cfg_tbl_state_get()))
    {
        const char_t *p_addr =
            (char_t *)&m_audio_cfg_tbl[CFG_TBL_CALIB_VER_OFFSET];

        (void)strncpy(p_calib_version,
                      p_addr,
                      CFG_TBL_CALIB_VER_PARAM_SIZE);

        p_calib_version[CFG_TBL_CALIB_VER_PARAM_SIZE] = '\0';

        return true;
    }

    return false;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194482
 *
 */
bool api_audio_freq_get(uint16_t *p_freq, uint32_t offset)
{
    if (NULL == p_freq)
    {
        LOG_ERROR("Invalid argument in api_audio_freq_get");

        return false;
    }

    if ((true == m_api_audio_init) &&
        (BUFFER_STATE_READY == cfg_tbl_state_get()))
    {
        const uint8_t *p_addr = &m_audio_cfg_tbl[CFG_TBL_FREQ_OFFSET + offset];

        *p_freq = uint16_convert(p_addr);

        return true;
    }

    return false;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194483
 *
 */
bool api_audio_gain_get(uint16_t *p_gain, uint32_t offset)
{
    if (NULL == p_gain)
    {
        LOG_ERROR("Invalid argument in api_audio_gain_get");

        return false;
    }

    if ((true == m_api_audio_init) &&
        (BUFFER_STATE_READY == cfg_tbl_state_get()))
    {
        const uint8_t *p_addr = &m_audio_cfg_tbl[CFG_TBL_GAIN_OFFSET + offset];

        *p_gain = uint16_convert(p_addr);

        return true;
    }

    return false;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194484
 *
 */
bool api_audio_ver_get(char_t *p_version)
{
    if (NULL == p_version)
    {
        LOG_ERROR("Invalid argument in api_audio_ver_get");

        return false;
    }

    if ((true == m_api_audio_init) &&
        (BUFFER_STATE_READY == cfg_tbl_state_get()))
    {
        const char_t *p_addr = (char_t *)&m_audio_cfg_tbl[CFG_TBL_VER_OFFSET];

        (void)strncpy(p_version,
                      p_addr,
                      CFG_TBL_VER_PARAM_SIZE);

        p_version[CFG_TBL_VER_PARAM_SIZE] = '\0';

        return true;
    }

    return false;
}

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/194850
 *
 */
static inline uint16_t uint16_convert(const uint8_t *p_data)
{
    uint16_t val = 0U;

    // PRQA S 1863 ++ # The daiichi_types macro is being used.
    // PRQA S 3396 ++ # The daiichi_types macro is being used. The extra parantheses suggestion cannot be applied.
    bool endian = IS_LITTLE_ENDIAN;
    // PRQA S 3396 --
    // PRQA S 1863 --

    if (false == endian)
    {
        val = val + (uint16_t)((uint16_t)p_data[1U] <<  8U);
        val = val + (uint16_t)((uint16_t)p_data[0U] <<  0U);
    }
    else
    {
        val = val + (uint16_t)((uint16_t)p_data[0U] <<  8U);
        val = val + (uint16_t)((uint16_t)p_data[1U] <<  0U);
    }

    return val;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194851
 *
 */
static uint32_t uint32_convert(const uint8_t *p_data)
{
    uint32_t val = 0U;

    // PRQA S 1863 ++ # The daiichi_types macro is being used.
    // PRQA S 3396 ++ # The daiichi_types macro is being used. The extra parantheses suggestion cannot be applied.
    bool endian = IS_LITTLE_ENDIAN;
    // PRQA S 3396 --
    // PRQA S 1863 --

    if (false == endian)
    {
        val = val + ((uint32_t)p_data[3U] << 24U);
        val = val + ((uint32_t)p_data[2U] << 16U);
        val = val + ((uint32_t)p_data[1U] <<  8U);
        val = val + ((uint32_t)p_data[0U] <<  0U);
    }
    else
    {
        val = val + ((uint32_t)p_data[0U] << 24U);
        val = val + ((uint32_t)p_data[1U] << 16U);
        val = val + ((uint32_t)p_data[2U] <<  8U);
        val = val + ((uint32_t)p_data[3U] <<  0U);
    }

    return val;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194260
 *
 */
static inline void local_var_reset(void)
{
    m_audio_param.cursor = 0U;

    m_audio_param.cfg_tbl_addr = 0U;

    m_audio_param.audio_addr = 0U;

    m_audio_param.size = 0U;

    audio_play_set(false);

    curr_flash_state_set(FLASH_READ_STATE_INIT);

    curr_audio_state_set(AUDIO_STATE_INIT);

    cfg_tbl_state_set(BUFFER_STATE_IDLE);

    m_curr_buff.p_buff = m_audio_buff_1;
    m_curr_buff.max_size = AUDIO_BUFF_SIZE;
    m_curr_buff.cur_size = AUDIO_BUFF_SIZE;
    m_curr_buff.shifting_rate = AUDIO_SHIFTING_RATE;
    m_curr_buff.buff_state = BUFFER_STATE_IDLE;

    m_spare_buff.p_buff = m_audio_buff_2;
    m_spare_buff.max_size = AUDIO_BUFF_SIZE;
    m_spare_buff.cur_size = AUDIO_BUFF_SIZE;
    m_spare_buff.shifting_rate = AUDIO_SHIFTING_RATE;
    m_spare_buff.buff_state = BUFFER_STATE_IDLE;

    (void)memset(m_audio_buff_1, 0, AUDIO_BUFF_SIZE);

    (void)memset(m_audio_buff_2, 0, AUDIO_BUFF_SIZE);

    (void)memset(m_audio_cfg_tbl, 0, API_DATA_AUDIO_CFG_TBL_SIZE);
}

/**
 * @brief https://codebeamer.daiichi.com/tracker/676656?view_id=-2
 *
 */
static inline void audio_buff_curr_to_spare_switch(void)
{
    audio_buff_t tmp_buff = {
        .buff_state = m_curr_buff.buff_state,
        .p_buff = m_curr_buff.p_buff,
        .shifting_rate = m_curr_buff.shifting_rate,
        .cur_size = m_curr_buff.cur_size,
    };

    m_curr_buff.buff_state = BUFFER_STATE_READY;
    m_curr_buff.shifting_rate = m_spare_buff.shifting_rate;
    m_curr_buff.p_buff = m_spare_buff.p_buff;
    m_curr_buff.cur_size = m_spare_buff.cur_size;

    m_spare_buff.buff_state = BUFFER_STATE_READ;
    m_spare_buff.shifting_rate = tmp_buff.shifting_rate;
    m_spare_buff.p_buff = tmp_buff.p_buff;
    m_spare_buff.cur_size = tmp_buff.cur_size;

    LOG_VERBOSE("buff:%x", m_curr_buff.p_buff);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194486
 *
 */
static inline void audio_buff_curr_to_mute_switch(void)
{
    m_curr_buff.buff_state = BUFFER_STATE_READY;
    m_curr_buff.p_buff = m_audio_mute_buff;
    m_curr_buff.max_size = AUDIO_MUTE_BUFF_SIZE;
    m_curr_buff.cur_size = AUDIO_MUTE_BUFF_SIZE;
    m_curr_buff.shifting_rate = 0U;

    LOG_INFO("Buff switch to mute");
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194805
 *
 */
static inline uint32_t audio_addr_get(void)
{
    return m_audio_param.audio_addr;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194438
 *
 */
static inline void audio_addr_set(uint8_t curr_audio)
{
    uint32_t addr = (AUDIO_OFFSET * curr_audio) + API_DATA_EEPROM_LAST_DATA;

    m_audio_param.audio_addr = addr + API_DATA_AUDIO_CFG_TBL_SIZE;

    LOG_INFO("Audio File Selected; id:%u, address:%u, ", 
             curr_audio,
             m_audio_param.audio_addr);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194807
 *
 */
static inline uint32_t cfg_tbl_addr_get(void)
{
    return m_audio_param.cfg_tbl_addr;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194808
 *
 */
static inline void cfg_tbl_addr_set(uint8_t curr_audio)
{
    m_audio_param.cfg_tbl_addr =
        (AUDIO_OFFSET * curr_audio) + API_DATA_EEPROM_LAST_DATA;

    LOG_INFO("Config File Selected; id:%u, address:%u", 
             curr_audio,
             m_audio_param.cfg_tbl_addr);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194435
 *
 */
static inline void shifting_rate_update(uint16_t speed)
{
    uint32_t offset = CFG_TBL_FREQ_OFFSET + ((uint32_t)speed * 2U);

    const uint8_t *p_addr = &m_audio_cfg_tbl[offset];

    m_curr_buff.shifting_rate = uint16_convert(p_addr);
    m_spare_buff.shifting_rate = m_curr_buff.shifting_rate;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194264
 *
 */
static inline void audio_param_update(uint16_t speed)
{
    uint32_t offset = CFG_TBL_GAIN_OFFSET + ((uint32_t)speed * 2U);

    const uint8_t *p_addr = &m_audio_cfg_tbl[offset];

    m_audio_param.curr_gain = uint16_convert(p_addr);

    shifting_rate_update(speed);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/195191
 *
 */
static inline void audio_play_set(bool status)
{
    m_audio_param.play = status;

    LOG_INFO("Audio play status changed; status:%u", status);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/195192
 *
 */
static inline bool audio_play_get(void)
{
    return m_audio_param.play;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194436
 *
 */
static inline uint32_t sample_pos_get(uint16_t max_sample_cnt,
                                      uint16_t shifting_rate)
{
    if (max_sample_cnt < shifting_rate)
    {
        return shifting_rate;
    }

    return ((uint32_t)max_sample_cnt - shifting_rate);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194437
 *
 */
static inline void audio_buff_db_edit(uint8_t *p_buff,
                                      uint32_t size,
                                      uint16_t gain)
{
    da_double_t calculated_gain = (da_double_t)gain / AUDIO_GAIN_CALC_RATE;

    if (1.0 == calculated_gain)
    {
        return;
    }

    for (uint32_t idx = 0U; idx < size; idx++)
    {
        // PRQA S 4395 ++ # Conversion from double data type to unsigned data type is required. Data loss to occur is taken into consideration.
        p_buff[idx] = (uint8_t)((da_double_t)p_buff[idx] * calculated_gain);
        // PRQA S 4395 --
    }
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194262
 *
 */
static inline void audio_buff_get(uint8_t *p_buff, 
                                  uint32_t offset, 
                                  uint32_t size)
{
    uint32_t addr = audio_addr_get();

    bool ret = api_data_read(addr,
                             p_buff,
                             size,
                             offset);

    if (ret)
    {
        m_audio_param.cursor++;
    }
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194253
 *
 */
static inline bool audio_play(const uint8_t *p_buff, uint32_t size)
{
    if (false == bsp_i2s_write(p_buff, (size_t)size))
    {
        LOG_ERROR("I2S write error");

        return false;
    }

    return true;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194797
 *
 */
static inline audio_state_t curr_audio_state_get(void)
{
    return m_audio_param.state;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194798
 *
 */
static inline void curr_audio_state_set(audio_state_t state)
{
    m_audio_param.state = state;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194797
 *
 */
static inline buffer_state_t curr_buff_state_get(void)
{
    return m_curr_buff.buff_state;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194800
 *
 */
static inline void curr_buff_state_set(buffer_state_t state)
{
    m_curr_buff.buff_state = state;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194801
 *
 */
static inline buffer_state_t spare_buff_state_get(void)
{
    return m_spare_buff.buff_state;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194802
 *
 */
static inline void spare_buff_state_set(buffer_state_t state)
{
    m_spare_buff.buff_state = state;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194803
 *
 */
static inline flash_read_state_t curr_flash_state_get(void)
{
    return m_flash_read_state;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194804
 *
 */
static inline void curr_flash_state_set(flash_read_state_t state)
{
    m_flash_read_state = state;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194809
 *
 */
static inline void curr_audio_set(uint8_t audio)
{
    m_audio_param.curr_audio = audio;

    LOG_INFO("Audio id selected; id:%u", m_audio_param.curr_audio);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/195193
 *
 */
static inline uint8_t curr_audio_get(void)
{
    return m_audio_param.curr_audio;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/196950
 *
 */
static inline void cfg_tbl_state_set(buffer_state_t status)
{
    m_cfg_tbl_state = status;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/196951
 *
 */
static inline buffer_state_t cfg_tbl_state_get(void)
{
    return m_cfg_tbl_state;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194261
 *
 */
static uint32_t audio_buff_freq_shift(const audio_buff_t *p_audio)
{
    uint16_t edit_cnt = m_audio_param.max_sample_cnt;

    uint32_t removed_sample_cnt = 0U;

    uint32_t src_addr_offset = sample_pos_get(edit_cnt,
                                              p_audio->shifting_rate);

    uint16_t rate = p_audio->shifting_rate;

    if (0U == rate)
    {
        return 0U;
    }

    uint8_t *p_src = p_audio->p_buff;

    size_t remaining_byte = (size_t)(p_audio->max_size - edit_cnt);

    while ((src_addr_offset < p_audio->max_size) &&
           (0U != remaining_byte))
    {
        (void)memmove(&p_src[src_addr_offset],
                      &p_src[src_addr_offset + rate],
                      remaining_byte);

        remaining_byte =
            (edit_cnt < remaining_byte) ? (remaining_byte - edit_cnt) : 0U;

        src_addr_offset = src_addr_offset + ((uint32_t)edit_cnt - rate);

        removed_sample_cnt += rate;
    }

    return removed_sample_cnt;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194263
 *
 */
static void audio_buff_edit(audio_buff_t *p_audio, uint16_t gain)
{
    if (0U == gain)
    {
        // TODO : 0km'de ses mute olması için burası açılacak
        //(void)memset(p_audio->p_buff, 0, p_audio->size);

        return;
    }

    uint32_t removed_sample_cnt = audio_buff_freq_shift(p_audio);

    p_audio->cur_size -= removed_sample_cnt;

    audio_buff_db_edit(p_audio->p_buff,
                       p_audio->cur_size,
                       gain);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194265
 *
 */
// PRQA S 3673 ++ # The pointer 'pp_audio' is modified in function. Can not be 'pointer to const'.
static void audio_buff_check(audio_buff_t **pp_audio, uint32_t buff_cnt)
{
    // PRQA S 3673 --
    for (uint32_t idx = 0U; idx < buff_cnt; idx++)
    {
        buffer_state_t state = pp_audio[idx]->buff_state;

        if (BUFFER_STATE_READ == state)
        {
            pp_audio[idx]->buff_state = BUFFER_STATE_READ_WAIT;

            uint32_t audio_size = pp_audio[idx]->max_size;
            
            uint32_t audio_offset = m_audio_param.cursor * audio_size;

            if ((audio_offset + audio_size) > m_audio_param.size)
            {
                audio_offset = 0;

                m_audio_param.cursor = 0;

                LOG_INFO("End of audio reached");
            }

            audio_buff_get(pp_audio[idx]->p_buff, audio_offset, audio_size);

            pp_audio[idx]->cur_size = pp_audio[idx]->max_size;
        }

        if (BUFFER_STATE_EDIT == state)
        {
            audio_buff_edit(pp_audio[idx],
                            m_audio_param.curr_gain);

            pp_audio[idx]->buff_state = BUFFER_STATE_READY;
        }
    }
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194439
 *
 */
static bool audio_init_state_check(void)
{
    if (FLASH_READ_STATE_INIT == curr_flash_state_get())
    {
        uint32_t addr = cfg_tbl_addr_get();

        bool ret = api_data_read(addr,
                                 m_audio_cfg_tbl,
                                 API_DATA_AUDIO_CFG_TBL_SIZE,
                                 0U);

        if (true == ret)
        {
            curr_flash_state_set(FLASH_READ_STATE_CFG_TBL);
        }

        return false;
    }

    if ((BUFFER_STATE_READY == curr_buff_state_get()) &&
        (BUFFER_STATE_READY == spare_buff_state_get()))
    {
        return true;
    }

    return false;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194259
 *
 */
static void audio_run_state_check(void)
{
    if (BUFFER_STATE_READY == curr_buff_state_get())
    {
        if (true == audio_play(m_curr_buff.p_buff, m_curr_buff.cur_size))
        {
            curr_buff_state_set(BUFFER_STATE_BUSY);
        }
    }
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194089
 *
 */
static void flash_state_manager(void)
{
    audio_buff_t *p_buff = NULL;

    flash_read_state_t curr_state = curr_flash_state_get();

    static flash_read_state_t log_state = 0xFFU;

    if (log_state != curr_state)
    {
        LOG_DEBUG("Flash Process, Prev state: %u, New state: %u", 
                   log_state,
                   curr_state);

        log_state = curr_state;
    }

    if (FLASH_READ_STATE_CFG_TBL == curr_state)
    {
        cfg_tbl_state_set(BUFFER_STATE_READY);

        const uint8_t *p_addr = &m_audio_cfg_tbl[CFG_TBL_SIZE_OFFSET];

        m_audio_param.size = uint32_convert(p_addr);

        p_addr = &m_audio_cfg_tbl[CFG_TBL_MAX_SAMPLE_CNT_OFFSET];

        m_audio_param.max_sample_cnt = uint16_convert(p_addr);

        curr_state = FLASH_READ_STATE_CURR_AUDIO_BUFF;
    }

    switch (curr_state)
    {
    case FLASH_READ_STATE_CURR_AUDIO_BUFF:
    {
        p_buff = &m_curr_buff;

        curr_buff_state_set(BUFFER_STATE_READ);

        curr_flash_state_set(FLASH_READ_STATE_SPARE_AUDIO_BUFF);
    }
    break;

    case FLASH_READ_STATE_SPARE_AUDIO_BUFF:
    {
        if (BUFFER_STATE_READ_WAIT == curr_buff_state_get())
        {
            curr_buff_state_set(BUFFER_STATE_EDIT);
        }

        p_buff = &m_spare_buff;

        spare_buff_state_set(BUFFER_STATE_READ);

        curr_flash_state_set(FLASH_READ_STATE_IDLE);
    }
    break;

    case FLASH_READ_STATE_IDLE:
    {
        if (BUFFER_STATE_READ_WAIT == spare_buff_state_get())
        {
            spare_buff_state_set(BUFFER_STATE_EDIT);
        }
    }
    break;

    default:
    {
        // Placeholder
    }
    break;
    }

    if (NULL != p_buff)
    {
        audio_buff_check(&p_buff, 1U);
    }
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194087
 *
 */
static void state_manager(void)
{
    audio_buff_t *audio_buff_arr[2U] = {
        &m_curr_buff,
        &m_spare_buff,

    };

    audio_state_t curr_audio_state = curr_audio_state_get();

    static audio_state_t log_state = 0xFFU;

    if (log_state != curr_audio_state)
    {
        LOG_DEBUG("Audio Process, Prev state: %u, New state: %u", 
                   log_state,
                   curr_audio_state);

        log_state = curr_audio_state;
    }

    switch (curr_audio_state)
    {
    case AUDIO_STATE_INIT:
    {
        if (true == audio_init_state_check())
        {
            curr_audio_state_set(AUDIO_STATE_READY);
        }
    }
    break;

    case AUDIO_STATE_SWITCH:
    {
        curr_flash_state_set(FLASH_READ_STATE_INIT);

        uint32_t addr = cfg_tbl_addr_get();

        bool ret = api_data_read(addr,
                                 m_audio_cfg_tbl,
                                 API_DATA_AUDIO_CFG_TBL_SIZE,
                                 0U);

        cfg_tbl_state_set(BUFFER_STATE_READ);

        curr_buff_state_set(BUFFER_STATE_IDLE);

        spare_buff_state_set(BUFFER_STATE_IDLE);

        curr_audio_state_set(AUDIO_STATE_INIT);

        if (false == ret)
        {
            LOG_ERROR("Audio cfg table could not read");

            return;
        }

        curr_flash_state_set(FLASH_READ_STATE_CFG_TBL);
    }
    break;
    
    case AUDIO_STATE_READY:
    {
        if (true == audio_play_get())
        {
            curr_audio_state_set(AUDIO_STATE_RUN);
        }
    }
    break;

    default:
    {
        audio_run_state_check();
    }
    break;
    }

    audio_buff_check(audio_buff_arr, 2U);
}

/**
 * @brief https://codebeamer.daiichi.com/issue/194090
 *
 */
static void audio_write_cb(bsp_i2s_status_t status)
{
    if (AUDIO_STATE_INIT == curr_audio_state_get())
    {
        LOG_ERROR("Module is in SWITCH state");

        return;
    }

    if (BSP_I2S_END_TRANSFER == status)
    {
        LOG_VERBOSE("I2S event end transfer");

        if (false == m_audio_param.mute)
        {
            audio_buff_curr_to_spare_switch();
        }
        else
        {
            audio_buff_curr_to_mute_switch();
        }

        audio_run_state_check();
    }
    else if (BSP_I2S_RX_FULL == status)
    {
        LOG_ERROR("I2S event rx full");
    }
    else if (BSP_I2S_TX_EMPTY == status)
    {
        LOG_VERBOSE("I2S event tx empty");

        if (false == m_audio_param.mute)
        {
            audio_buff_curr_to_spare_switch();
        }
        else
        {
            audio_buff_curr_to_mute_switch();
        }

        audio_run_state_check();
    }
    else
    {
        LOG_ERROR("I2S event error");
    }
}

/**
 * @brief https://codebeamer.daiichi.com/issue/197524
 *
 */
static void ext_flash_cb(bool err)
{
    if (false == err)
    {
        //flash_state_manager(); // TODO : ne yapilacak?
    }
    else
    {
        // TODO: What to do in case of an error will be discussed.
    }
}

// end of file
