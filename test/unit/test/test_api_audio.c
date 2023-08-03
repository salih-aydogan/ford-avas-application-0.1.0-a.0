/**
 * @file test_api_audio.c
 * @author Muhammed Salih AYDOGAN
 * @brief These tests verify the units of the AUDIO module.
 * 
 * @version 0.1
 * 
 * @date 23/06/2023
 * 
 * @copyright Copyright (c) 2023
 * 
 * https://codebeamer.daiichi.com/item/195470
 *  
 */

/*******************************************************************************
 * Includes 
 ******************************************************************************/

//First - include test framework headers
#include "unity.h"
#include "support.h"

// Second - include the module to be tested
#include "api_audio.h"

#include <string.h>

// Third - include the mocks of required modules
#include "mock_api_data.h"
#include "mock_bsp_i2s.h"
#include "mock_drv_amplifier.h"
#include "mock_drv_flash.h"

#include "mock_log.h"

/*******************************************************************************
 * Test Files
 ******************************************************************************/

TEST_FILE("api_audio.c");

/*******************************************************************************             
 * Defines
 ******************************************************************************/
/**
 * @brief This macro indicates the audio buffer size.
 * 
 */
#define TEST_AUDIO_BUFF_SIZE (2046U)

/**
 * @brief This macro indicates the audio name parameter size in the audio 
 *        configuration table.
 * 
 */
#define TEST_CFG_TBL_NAME_PARAM_SIZE (32U)

/**
 * @brief This macro indicates the audio size parameter size in the audio 
 *        configuration table.
 * 
 */
#define TEST_CFG_TBL_SIZE_PARAM_SIZE (4U)

/**
 * @brief This macro indicates the audio version parameter size in the audio 
 *        configuration table.
 * 
 */
#define TEST_CFG_TBL_VER_PARAM_SIZE (4U)

/**
 * @brief This macro indicates the audio calibration version parameter size in 
 *        the audio configuration table.
 * 
 */
#define TEST_CFG_TBL_CALIB_VER_PARAM_SIZE (4U)

/**
 * @brief This macro indicates the audio calibration tool version parameter size
 *        in the audio configuration table.
 * 
 */
#define TEST_CFG_TBL_CALIB_TOOL_VER_SIZE (4U)

/**
 * @brief This macro indicates the audio calibration date parameter size in the 
 *        audio configuration table.
 * 
 */
#define TEST_CFG_TBL_CALIB_DATE_PARAM_SIZE (16U)

/**
 * @brief This macro indicates the reserved size in the audio configuration 
 *        table.
 * 
 */
#define TEST_CFG_TBL_RESERVED_SIZE (36U)

/**
 * @brief This macro indicates the audio gain parameter size in the audio 
 *        configuration table.
 * 
 */
#define TEST_CFG_TBL_GAIN_PARAM_SIZE (200U)

/**
 * @brief This macro indicates the audio name offset in the audio configuration 
 *        table.
 * 
 */
#define TEST_CFG_TBL_AUDIO_NAME_OFFSET (0U)

/**
 * @brief This macro indicates the audio size offset in the audio configuration 
 *        table.
 * 
 */
#define TEST_CFG_TBL_SIZE_OFFSET \
    (TEST_CFG_TBL_AUDIO_NAME_OFFSET + TEST_CFG_TBL_NAME_PARAM_SIZE)

/**
 * @brief This macro indicates the audio version offset in the audio 
 *        configuration table.
 * 
 */
#define TEST_CFG_TBL_VER_OFFSET \
    (TEST_CFG_TBL_SIZE_OFFSET + TEST_CFG_TBL_SIZE_PARAM_SIZE)

/**
 * @brief This macro indicates the audio calibration version offset in the audio 
 *        configuration table.
 * 
 */
#define TEST_CFG_TBL_CALIB_VER_OFFSET \
    (TEST_CFG_TBL_VER_OFFSET + TEST_CFG_TBL_VER_PARAM_SIZE)

/**
 * @brief This macro indicates the audio calibration tool version offset in the 
 *        audio configuration table.
 * 
 */
#define TEST_CFG_TBL_CALIB_TOOL_VER_OFFSET \
    (TEST_CFG_TBL_CALIB_VER_OFFSET + TEST_CFG_TBL_CALIB_VER_PARAM_SIZE)

/**
 * @brief This macro indicates the audio calibration date offset in the audio 
 *        configuration table.
 * 
 */
#define TEST_CFG_TBL_CALIB_DATE_OFFSET \
    (TEST_CFG_TBL_CALIB_TOOL_VER_OFFSET + TEST_CFG_TBL_CALIB_TOOL_VER_SIZE)

/**
 * @brief This macro indicates the reserved area offset in the audio 
 *        configuration table.
 * 
 */
#define TEST_CFG_TBL_RESERVED_OFFSET \
    (TEST_CFG_TBL_CALIB_DATE_OFFSET + TEST_CFG_TBL_CALIB_DATE_PARAM_SIZE)

/**
 * @brief This macro indicates the audio gain value offset in the audio
 *        configuration table.
 * 
 */
#define TEST_CFG_TBL_GAIN_OFFSET \
    (TEST_CFG_TBL_RESERVED_OFFSET + TEST_CFG_TBL_RESERVED_SIZE)

/**
 * @brief This macro indicates the audio frequency value offset in the audio 
 *        configuration table.
 * 
 */
#define TEST_CFG_TBL_FREQ_OFFSET \
    (TEST_CFG_TBL_GAIN_OFFSET + TEST_CFG_TBL_GAIN_PARAM_SIZE)

/**
 * @brief This macro indicates the size of one byte data.
 * 
 */
#define TEST_API_DATA_BYTE_1 (1U)

/**
 * @brief This macro indicates the size of two byte data.
 * 
 */
#define TEST_API_DATA_BYTE_2 (TEST_API_DATA_BYTE_1 * 2U)

/**
 * @brief This macro indicates the size of four byte data.
 * 
 */
#define TEST_API_DATA_BYTE_4 (TEST_API_DATA_BYTE_1 * 4U)

/**
 * @brief This macro indicates the DTC message data size.
 * 
 */
#define TEST_API_DATA_DTC_SIZE (TEST_API_DATA_BYTE_4 * 2U)

/**
 * @brief This macro indicates the DID message data size.
 * 
 */
#define TEST_API_DATA_DID_SIZE (TEST_API_DATA_BYTE_1 * 24U)

/**
 * @brief This macro indicates the size of the audio file configuration table to 
 *        be saved in Flash. 
 * 
 */
#define TEST_API_DATA_AUDIO_CFG_TBL_SIZE (TEST_API_DATA_BYTE_1 * 500U)

/**
 * @brief This macro indicates the default size of audio file data.
 * 
 */
#define TEST_API_DATA_AUDIO_DEFAULT_SIZE (500U * 1024U)

/**
 * @brief This macro indicates the EEPROM authentication data address.
 * 
 */
#define TEST_API_DATA_LABEL_AUTH (0U)

/**
 * @brief This macro indicates the Secure Boot status data address.
 * 
 */
#define TEST_API_DATA_LABEL_BOOT_STATUS \
    (TEST_API_DATA_LABEL_AUTH + TEST_API_DATA_BYTE_1)

/**
 * @brief This macro indicates the reserved data address.
 * 
 */
#define TEST_API_DATA_LABEL_RESERVED \
    (TEST_API_DATA_LABEL_BOOT_STATUS + TEST_API_DATA_BYTE_1)

/**
 * @brief This macro indicates the pedestrian alert data address.
 * 
 */
#define TEST_API_DATA_LABEL_PED_ALERT \
    (TEST_API_DATA_LABEL_RESERVED + (TEST_API_DATA_BYTE_1 * 32U))

/**
 * @brief This macro indicates the car speed data address.
 * 
 */
#define TEST_API_DATA_LABEL_CAR_SPEED \
    (TEST_API_DATA_LABEL_PED_ALERT + TEST_API_DATA_BYTE_1)
    

/**
 * @brief This macro indicates the trans gear data address.
 * 
 */
#define TEST_API_DATA_LABEL_TRANS_GEAR \
    (TEST_API_DATA_LABEL_CAR_SPEED + TEST_API_DATA_BYTE_2)

/**
 * @brief This macro indicates the current gear data address.
 * 
 */
#define TEST_API_DATA_LABEL_CURR_GEAR \
    (TEST_API_DATA_LABEL_TRANS_GEAR + TEST_API_DATA_BYTE_1)

/**
 * @brief This macro indicates the ignition status data address.
 * 
 */
#define TEST_API_DATA_LABEL_IGN_STATUS \
    (TEST_API_DATA_LABEL_CURR_GEAR + TEST_API_DATA_BYTE_1)

/**
 * @brief This macro indicates the lifecyc mode data address.
 * 
 */
#define TEST_API_DATA_LABEL_LCYC_MODE \
    (TEST_API_DATA_LABEL_IGN_STATUS + TEST_API_DATA_BYTE_1)

/**
 * @brief This macro indicates the current audio data address.
 * 
 */
#define TEST_API_DATA_LABEL_CURR_AUDIO \
    (TEST_API_DATA_LABEL_LCYC_MODE + TEST_API_DATA_BYTE_1)

/**
 * @brief This macro indicates the DTC Speaker-1 Amplifier Open Circuit data 
 *        address.
 * 
 */
#define TEST_API_DATA_LABEL_DTC_SPKR_1_OC \
    (TEST_API_DATA_LABEL_CURR_AUDIO + TEST_API_DATA_BYTE_1)

/**
 * @brief This macro indicates the DTC Speaker-1 Amplifier Out Shorted to the 
 *        Power data address.
 * 
 */
#define TEST_API_DATA_LABEL_DTC_SPKR_1_OS_PWR \
    (TEST_API_DATA_LABEL_DTC_SPKR_1_OC + TEST_API_DATA_DTC_SIZE)

/**
 * @brief This macro indicates the DTC Speaker-1 Amplifier Out Shorted to the 
 *        GND data address.
 * 
 */
#define TEST_API_DATA_LABEL_DTC_SPKR_1_OS_GND \
    (TEST_API_DATA_LABEL_DTC_SPKR_1_OS_PWR + TEST_API_DATA_DTC_SIZE)

/**
 * @brief This macro indicates the DTC Speaker-2 Amplifier Open Circuit data 
 *        address.
 * 
 */
#define TEST_API_DATA_LABEL_DTC_SPKR_2_OC \
    (TEST_API_DATA_LABEL_DTC_SPKR_1_OS_GND + TEST_API_DATA_DTC_SIZE)

/**
 * @brief This macro indicates the DTC Speaker-2 Amplifier Out Shorted to the 
 *        Power data address.
 * 
 */
#define TEST_API_DATA_LABEL_DTC_SPKR_2_OS_PWR \
    (TEST_API_DATA_LABEL_DTC_SPKR_2_OC + TEST_API_DATA_DTC_SIZE)

/**
 * @brief This macro indicates the DTC Speaker-2 Amplifier Out Shorted to the 
 *        GND data address.
 * 
 */
#define TEST_API_DATA_LABEL_DTC_SPKR_2_OS_GND \
    (TEST_API_DATA_LABEL_DTC_SPKR_2_OS_PWR + TEST_API_DATA_DTC_SIZE)

/**
 * @brief This macro indicates the DTC Can communication error VCU data address.
 * 
 */
#define TEST_API_DATA_LABEL_DTC_ERR_VCU \
    (TEST_API_DATA_LABEL_DTC_SPKR_2_OS_GND + TEST_API_DATA_DTC_SIZE)

/**
 * @brief This macro indicates the DTC Invalid data received from VCU data 
 *        address.
 * 
 */
#define TEST_API_DATA_LABEL_DTC_INV_DATA_VCU \
    (TEST_API_DATA_LABEL_DTC_ERR_VCU + TEST_API_DATA_DTC_SIZE)

/**
 * @brief This macro indicates the DTC Control module general checksum failure 
 *        data address.
 * 
 */
#define TEST_API_DATA_LABEL_DTC_CSUM_FAIL \
    (TEST_API_DATA_LABEL_DTC_INV_DATA_VCU + TEST_API_DATA_DTC_SIZE)

/**
 * @brief This macro indicates the DTC Control module general memory failure 
 *        data address.
 * 
 */
#define TEST_API_DATA_LABEL_DTC_MEM_FAIL \
    (TEST_API_DATA_LABEL_DTC_CSUM_FAIL + TEST_API_DATA_DTC_SIZE)

/**
 * @brief This macro indicates the DTC Control module component internal failure 
 *        data address.
 * 
 */
#define TEST_API_DATA_LABEL_DTC_CTRL_MDL_FAIL \
    (TEST_API_DATA_LABEL_DTC_MEM_FAIL + TEST_API_DATA_DTC_SIZE)

/**
 * @brief This macro indicates the DTC Battery voltage circuit voltage below 
 *        threshold data address.
 * 
 */
#define TEST_API_DATA_LABEL_DTC_LOW_BAT \
    (TEST_API_DATA_LABEL_DTC_CTRL_MDL_FAIL + TEST_API_DATA_DTC_SIZE)

/**
 * @brief This macro indicates the DTC Battery voltage circuit voltage above 
 *        threshold data address.
 * 
 */
#define TEST_API_DATA_LABEL_DTC_HIGH_BAT \
    (TEST_API_DATA_LABEL_DTC_LOW_BAT + TEST_API_DATA_DTC_SIZE)

/**
 * @brief This macro indicates the Subsystem diagnostic part number DID 
 *        (Data Identification) address.
 * 
 */
#define TEST_API_DATA_LABEL_DID_SUBSYS_DIAG_PNUM \
    (TEST_API_DATA_LABEL_DTC_HIGH_BAT + TEST_API_DATA_DTC_SIZE)

/**
 * @brief This macro indicates the ECU Core Assembly Number DID 
 *        (Data Identification) address.
 * 
 */
#define TEST_API_DATA_LABEL_DID_ECU_CORE_ASM_NUM \
    (TEST_API_DATA_LABEL_DID_SUBSYS_DIAG_PNUM + TEST_API_DATA_DID_SIZE)

/**
 * @brief This macro indicates the ECU Assembly Number DID 
 *        (Data Identification) address.
 * 
 */
#define TEST_API_DATA_LABEL_DID_ECU_ASM_NUM \
    (TEST_API_DATA_LABEL_DID_ECU_CORE_ASM_NUM + TEST_API_DATA_DID_SIZE)

/**
 * @brief This macro indicates the ECU Delivery Assembly Number DID 
 *        (Data Identification) address.
 * 
 */
#define TEST_API_DATA_LABEL_DID_ECU_DELIV_ASM_NUM \
    (TEST_API_DATA_LABEL_DID_ECU_ASM_NUM + TEST_API_DATA_DID_SIZE)

/**
 * @brief This macro indicates the Diagnostic Specification Version DID 
 *        (Data Identification) address.
 * 
 */
#define TEST_API_DATA_LABEL_DID_DIAG_SPEC_VER \
    (TEST_API_DATA_LABEL_DID_ECU_DELIV_ASM_NUM + TEST_API_DATA_DID_SIZE)

/**
 * @brief This macro indicates the Vehicle manufacturer ECU SW number DID 
 *        (Data Identification) address.
 * 
 */
#define TEST_API_DATA_LABEL_DID_ECU_SW_NUM \
    (TEST_API_DATA_LABEL_DID_DIAG_SPEC_VER + TEST_API_DATA_BYTE_1)

/**
 * @brief This macro indicates the ECU Serial Number DID 
 *        (Data Identification) address.
 * 
 */
#define TEST_API_DATA_LABEL_DID_ECU_SERIAL_NUM \
    (TEST_API_DATA_LABEL_DID_ECU_SW_NUM + TEST_API_DATA_DID_SIZE)

/**
 * @brief This macro indicates the Vehicle Identification Number DID 
 *        (Data Identification) address.
 * 
 */
#define TEST_API_DATA_LABEL_DID_VEH_ID_NUM \
    (TEST_API_DATA_LABEL_DID_ECU_SERIAL_NUM + (TEST_API_DATA_BYTE_1 * 16U))

/**
 * @brief This macro indicates the Daiichi SW version DID 
 *        (Data Identification) address.
 * 
 */
#define TEST_API_DATA_LABEL_DID_SW_VER \
    (TEST_API_DATA_LABEL_DID_VEH_ID_NUM + TEST_API_DATA_DID_SIZE)

/**
 * @brief This macro indicates the Daiichi HW version DID 
 *        (Data Identification) address.
 * 
 */
#define TEST_API_DATA_LABEL_DID_HW_VER \
    (TEST_API_DATA_LABEL_DID_SW_VER + TEST_API_DATA_DID_SIZE)

/**
 * @brief This macro indicates the last data address of EEPROM.
 * 
 */
#define TEST_API_DATA_EEPROM_LAST_DATA \
    (TEST_API_DATA_LABEL_DID_HW_VER + TEST_API_DATA_DID_SIZE)

/**
 * @brief This macro indicates the audio offset value in the External Flash.
 * 
 */
#define TEST_AUDIO_OFFSET (TEST_API_DATA_AUDIO_CFG_TBL_SIZE + \
                           TEST_API_DATA_AUDIO_DEFAULT_SIZE)

/*******************************************************************************
 * Private Variables
 ******************************************************************************/
/**
 * @brief This array holds shifting audio data.
 * 
 */
static uint8_t test_cfg_tbl[2048U] = {};

/**
 * @brief This variable is used for calculate the maximum buffer size in test.
 * 
 */
static bool m_test_edit_cnt;

/**
 * @brief This variable is used for change the return value.
 * 
 */
static bool m_test_current_audio;

/**
 * @brief This variable is used to get the I2S function address.
 * 
 */
static bsp_i2s_cb_t m_test_audio_write_cb;

/**
 * @brief This variable is used to get the I2S function address.
 * 
 */
static api_data_cb_t m_test_data_cb;

/*******************************************************************************
 * Private Functions Prototype Declarations
 ******************************************************************************/

/**
 * @brief This stub function is used instead of I2S callback set function.
 * 
 */
static bool test_bsp_i2s_cb_set_stub(bsp_i2s_cb_t callback, 
                                     int cmock_num_calls);

/**
 * @brief This stub function is used instead of data callback set function.
 * 
 */
static bool test_bsp_data_cb_set_stub(api_data_cb_t callback, 
                                      int cmock_num_calls);

/**
 * @brief This helper function is used for audio initialization.
 * 
 */
static void helper_api_audio_init_success(void);

/**
 * @brief This helper function is used for buffer ready state.
 * 
 */
static void helper_buffer_state_ready(void);

/**
 * @brief This helper function is used for audio initialization with stubs.
 * 
 */
static void helper_api_audio_init_with_stub_success(void);

/**
 * @brief This helper function is used for read finish with stubs.
 * 
 */
static void helper_api_audio_read_finish_stub(void);

/**
 * @brief This helper function is used for read finish success.
 * 
 */
static void helper_api_audio_read_finish_success(void);

/**
 * @brief This helper function is used for the audio run state.
 * 
 */
static void helper_api_audio_run_state(void);

/**
 * @brief This helper function is used for the audio mute state.
 * 
 */
static void helper_api_audio_mute_success(void);

/**
 * @brief This helper function is used for the audio unmute state.
 * 
 */
static void helper_api_audio_unmute_success(void);

/**
 * @brief This helper function is used for audio deinitialization.
 * 
 */
static void helper_api_audio_deinit_success(void);

/**
 * @brief This stub function is used instead of the data read process.
 * 
 */
static bool test_api_data_read_stub(uint32_t data_label, 
                                    uint8_t* p_data, 
                                    size_t size, 
                                    uint32_t offset, 
                                    int cmock_num_calls);

/**
 * @brief This stub function is used instead of the data read process.
 * 
 */
static bool test_api_data_read_speed_update_stub(uint32_t data_label, 
                                                 uint8_t* p_data, 
                                                 size_t size, 
                                                 uint32_t offset, 
                                                 int cmock_num_calls);

/**
 * @brief This stub function is used instead of the I2S write process.
 * 
 */
static bool test_i2s_write_stub(const uint8_t* data, 
                                size_t size, 
                                int cmock_num_calls);
/**
 * @brief This stub function is used instead of the data read process.
 * 
 */
static bool test_api_data_read(uint32_t data_label, 
                               uint8_t* p_data, 
                               size_t size, 
                               uint32_t offset, 
                               int cmock_num_calls);

/*******************************************************************************
 * Setup & Teardown 
 ******************************************************************************/

void setUp()
{
    log_print_Ignore();

    m_test_edit_cnt = false;
    m_test_current_audio = false;
}

void tearDown()
{
}

void suiteSetUp(void)
{
}

int suiteTearDown(int num_failures)
{
    (void)num_failures;
    return 0;
}

/*******************************************************************************
 * Test Functions
 ******************************************************************************/

/**
 * @brief The test verifies the api_audio_init() function by calling the related 
 * api_data and bsp_i2s functions successfully.
 * 
 * https://codebeamer.daiichi.com/item/195496
 *  
 */
void test_api_audio_init__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    uint8_t test_curr_audio = 0U;
    api_data_read_ExpectAndReturn(TEST_API_DATA_LABEL_CURR_AUDIO, 
                                 &test_curr_audio,
                                 1U,
                                 0U,
                                 true);
    api_data_read_IgnoreArg_p_data();

    uint32_t test_adrr = (TEST_AUDIO_OFFSET * test_curr_audio) + 
        TEST_API_DATA_EEPROM_LAST_DATA;

    api_data_read_ExpectAndReturn(test_adrr, 
                                 &test_curr_audio,
                                 TEST_API_DATA_AUDIO_CFG_TBL_SIZE,
                                 0U,
                                 true);

    api_data_read_IgnoreArg_p_data();

    // get i2s callback adress
    bsp_i2s_cb_set_StubWithCallback(test_bsp_i2s_cb_set_stub);

    // get data callback adress
    api_data_cb_set_StubWithCallback(test_bsp_data_cb_set_stub);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_init();    

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);

    TEST_STEP(3);
    m_test_audio_write_cb(BSP_I2S_ERROR); 
}

/**
 * @brief The test verifies the api_audio_init() function when the current audio 
 * data is not read.
 * 
 * https://codebeamer.daiichi.com/item/195506
 *  
 */
void test_api_audio_init__when_current_audio_not_read__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    api_data_read_ExpectAnyArgsAndReturn(false);

    api_data_read_ExpectAnyArgsAndReturn(false);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_init();    

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_deinit() function succesfully. 
 * Note: In addition, it is tested that the callback function has been not 
 * deleted.
 * 
 * https://codebeamer.daiichi.com/item/195507
 *  
 */
void test_api_audio_deinit__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    api_audio_deinit(); 

    TEST_STEP(2);
    m_test_audio_write_cb(BSP_I2S_TX_EMPTY);  
}

/**
 * @brief The test verifies the api_audio_process function when audio init is 
 * successful.
 * 
 * https://codebeamer.daiichi.com/item/195510
 *  
 */
void test_api_audio_process__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_api_audio_deinit_success();

    helper_api_audio_init_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    api_audio_process();
}

/**
 * @brief The test verifies the api_audio_process function when the audio is 
 * ready state. 
 * 
 * https://codebeamer.daiichi.com/item/195516
 *  
 */
void test_api_audio_process__when_audio_state_ready__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    helper_api_audio_read_finish_stub();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    api_audio_process();

    TEST_STEP(2);
    api_audio_process();

    TEST_STEP(3);
    api_audio_process(); // when state ready but audio play parameter is false
}

/**
 * @brief The test verifies the api_audio_process function when the 
 * api_audio_deinit function success. 
 * 
 * https://codebeamer.daiichi.com/item/195526
 *  
 */
void test_api_audio_process__when_init_err__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_api_audio_deinit_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    api_audio_process();
}

/**
 * @brief The test verifies the api_audio_mute function when audio init is 
 * successful. 
 * 
 * https://codebeamer.daiichi.com/item/195527
 *  
 */
void test_api_audio_mute__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    // ************************** Expected Calls **************************** //

    drv_amplifier_mute_ExpectAndReturn(true);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_mute();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief The test verifies the api_audio_mute function when the audio deinit is 
 * successful. 
 * 
 * https://codebeamer.daiichi.com/item/195528
 *  
 */
void test_api_audio_mute__when_api_audio_not_init__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_api_audio_deinit_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_mute();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_mute function according to the 
 * amplifier mute state.  
 * 
 * https://codebeamer.daiichi.com/item/195542
 *  
 */
void test_api_audio_mute__amplifier_mute_state__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    // ************************** Expected Calls **************************** //

    drv_amplifier_mute_ExpectAndReturn(false);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_mute();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_mute function according to the 
 * amplifier mute state. 
 * 
 * https://codebeamer.daiichi.com/item/195543
 *  
 */
void test_api_audio_mute__when_buffer_state_ready__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    helper_api_audio_read_finish_stub();

    // ************************** Expected Calls **************************** //

    drv_amplifier_mute_ExpectAndReturn(true);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    api_audio_process();

    TEST_STEP(2);
    bool ret = api_audio_mute();

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief The test verifies the api_audio_unmute function when audio init is 
 * successful. 
 * 
 * https://codebeamer.daiichi.com/item/195544
 *  
 */
void test_api_audio_unmute__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    // ************************** Expected Calls **************************** //

    drv_amplifier_unmute_ExpectAndReturn(true);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_unmute();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief The test verifies the api_audio_unmute function when the audio deinit 
 * function is successful. 
 * 
 * https://codebeamer.daiichi.com/item/195545
 *  
 */
void test_api_audio_unmute__when_api_audio_not_init__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_api_audio_deinit_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_unmute();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_unmute function when the audio unmute 
 * function is fail. 
 * 
 * https://codebeamer.daiichi.com/item/195546
 *  
 */
void test_api_audio_unmute__amplifier_unmute_state__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    // ************************** Expected Calls **************************** //

    drv_amplifier_unmute_ExpectAndReturn(false);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_unmute();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_unmute function when the buffer state 
 * is ready. 
 * 
 * https://codebeamer.daiichi.com/item/195547
 *  
 */
void test_api_audio_unmute__when_buffer_state_ready__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    helper_api_audio_read_finish_stub();

    // ************************** Expected Calls **************************** //

    drv_amplifier_unmute_ExpectAndReturn(true);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    api_audio_process();

    TEST_STEP(2);
    bool ret = api_audio_unmute();

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief The test verifies the api_audio_play function when the audio state is 
 * not ready. 
 * 
 * https://codebeamer.daiichi.com/item/195563
 *  
 */
void test_api_audio_play__when_audio_state_not_ready__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_play();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_play function when the 
 * api_audio_deinit function is successful. 
 * 
 * https://codebeamer.daiichi.com/item/195564
 *  
 */
void test_api_audio_play__when_api_audio_not_init__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_api_audio_deinit_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_play();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_pause function when the 
 * api_audio_deinit function is successful. 
 * 
 * https://codebeamer.daiichi.com/issue/197521
 *  
 */
void test_api_audio_pause__when_api_audio_not_init__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_api_audio_deinit_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_pause();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_pause function when the 
 * api_audio_init function is successful. 
 * 
 * https://codebeamer.daiichi.com/item/197522
 *  
 */
void test_api_audio_pause__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_pause();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief The test verifies the api_audio_play function is successful. 
 * 
 * https://codebeamer.daiichi.com/item/195565
 *  
 */
void test_api_audio_play__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    helper_api_audio_read_finish_stub();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    api_audio_process();

    TEST_STEP(2);
    api_audio_process();

    TEST_STEP(3);
    bool ret = api_audio_play();

    TEST_STEP(4);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief The test verifies the api_audio_play function when the audio play 
 * parameter is set to true.
 * 
 * https://codebeamer.daiichi.com/item/195566
 *  
 */
void test_api_audio_play__set_audio_play_param__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    helper_api_audio_read_finish_stub();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    api_audio_process();

    TEST_STEP(2);
    bool ret = api_audio_play(); 

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(false, ret);

    TEST_STEP(4);
    api_audio_process(); 
    
    TEST_STEP(5);
    api_audio_process(); 
}

/**
 * @brief The test verifies the api_audio_param_update function is successful. 
 * 
 * https://codebeamer.daiichi.com/item/195567
 *  
 */
void test_api_audio_param_update__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint16_t speed = 10U;

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    api_audio_param_update(speed);
}

/**
 * @brief The test verifies the api_audio_status_get function is successful. 
 * 
 * https://codebeamer.daiichi.com/item/197526
 *  
 */
void test_api_audio_status_get__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    // The audio module is paused
    helper_api_audio_mute_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool status = api_audio_status_get();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, status);
}

/**
 * @brief The test verifies the api_audio_status_get function is successful. 
 * 
 * https://codebeamer.daiichi.com/item/197529
 *  
 */
void test_api_audio_status_get__audio_play__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    helper_api_audio_read_finish_stub();

    // Set the audio play to true
    helper_api_audio_run_state();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool status = api_audio_status_get();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, status);
}

/**
 * @brief The test verifies the api_audio_param_update function when the 
 * api_audio_deinit function is successful. 
 * 
 * https://codebeamer.daiichi.com/item/195568
 *  
 */
void test_api_audio_param_update__when_api_audio_not_init__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint16_t speed = 10U;

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_deinit_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    api_audio_param_update(speed);
}

/**
 * @brief The test verifies the api_audio_switch function is successful. 
 * 
 * https://codebeamer.daiichi.com/item/195569
 *  
 */
void test_api_audio_switch__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint8_t audio_num = 1U;

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_switch(audio_num);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief The test verifies the api_audio_switch function with the invalid audio 
 * number.
 * 
 * http
 *  
 */
void test_api_audio_switch__invalid_audio__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint8_t audio_num = 1U;

    // **************************** Pre-Actions ***************************** //

    m_test_current_audio = true;

    helper_api_audio_init_with_stub_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_switch(audio_num);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_switch function when audio is not 
 * init.  
 * 
 * https://codebeamer.daiichi.com/item/195573
 *  
 */
void test_api_audio_switch__when_api_audio_not_init__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint8_t audio_num = 1U;

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_deinit_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_switch(audio_num);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_switch function when the audio number 
 * is the same. 
 * 
 * https://codebeamer.daiichi.com/item/195575
 *  
 */
void test_api_audio_switch__when_audio_num_is_same___success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint8_t audio_num = 0U;

    // **************************** Pre-Actions ***************************** //

    // init function sets audio_num value to 0U
    helper_api_audio_init_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_switch(audio_num);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);

    TEST_STEP(3);
    ret = api_audio_switch(audio_num);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief The test verifies the api_audio_name_get function is successful. 
 * 
 * https://codebeamer.daiichi.com/item/195577
 *  
 */
void test_api_audio_name_get__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    char_t test_audio_name[TEST_CFG_TBL_NAME_PARAM_SIZE];

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    helper_buffer_state_ready();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_name_get(test_audio_name);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief The test verifies the api_audio_name_get function is failing according 
 * to the buffer state ready.  
 * 
 * https://codebeamer.daiichi.com/item/197176
 *  
 */
void test_api_audio_name_get__buffer_state__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    char_t test_audio_name[TEST_CFG_TBL_NAME_PARAM_SIZE];

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_name_get(test_audio_name);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_name_get function when deinit function 
 * is successful. 
 * 
 * https://codebeamer.daiichi.com/item/195578
 *  
 */
void test_api_audio_name_get__when_api_audio_not_init__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    char_t test_audio_name[TEST_CFG_TBL_NAME_PARAM_SIZE];

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_deinit_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_name_get(test_audio_name);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_name_get function when using an 
 * invalid argument. 
 * 
 * https://codebeamer.daiichi.com/item/195579
 *  
 */
void test_api_audio_name_get__when_use_invalid_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_deinit_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_name_get(NULL);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_size_get function is successful. 
 * 
 * https://codebeamer.daiichi.com/item/195582
 *  
 */
void test_api_audio_size_get__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint32_t test_size = 99U;

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    helper_buffer_state_ready();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_size_get(&test_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(0U, test_size);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief The test verifies the api_audio_size_get function is failing 
 * according to the buffer state ready.
 * 
 * https://codebeamer.daiichi.com/item/197177
 *  
 */
void test_api_audio_size_get__buffer_state__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint32_t test_size = 99U;

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_size_get(&test_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(99U, test_size);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_size_get function when the 
 * api_audio_deinit() function is successful. 
 * 
 * https://codebeamer.daiichi.com/item/195583
 *  
 */
void test_api_audio_size_get__when_api_audio_not_init__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint32_t test_size = 99U;

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_deinit_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_size_get(&test_size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(99U, test_size);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_size_get function when using an 
 * invalid argument. 
 * 
 * https://codebeamer.daiichi.com/item/195584
 *  
 */
void test_api_audio_size_get__when_use_invalid_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_size_get(NULL);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_calib_date_get function is successful. 
 * 
 * https://codebeamer.daiichi.com/item/195585
 *  
 */
void test_api_audio_calib_date_get__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    char_t test_audio_name[TEST_CFG_TBL_CALIB_DATE_PARAM_SIZE];

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();
    
    helper_buffer_state_ready();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_calib_date_get(test_audio_name);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief The test verifies the api_audio_calib_date_get function is failing 
 * according to the buffer state ready.
 * 
 * https://codebeamer.daiichi.com/item/197178
 *  
 */
void test_api_audio_calib_date_get__buffer_state__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    char_t test_audio_name[TEST_CFG_TBL_CALIB_DATE_PARAM_SIZE];

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();
    
    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_calib_date_get(test_audio_name);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_calib_date_get function when 
 * api_data_deinit is successful. 
 * 
 * https://codebeamer.daiichi.com/item/195589
 *  
 */
void test_api_audio_calib_date_get__when_api_audio_not_init__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    char_t test_audio_name[TEST_CFG_TBL_CALIB_DATE_PARAM_SIZE];

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_deinit_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_calib_date_get(test_audio_name);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_calib_date_get function when 
 * api_data_deinit is successful. 
 * 
 * https://codebeamer.daiichi.com/item/195590
 *  
 */
void test_api_audio_calib_date_get__when_use_invalid_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_deinit_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_calib_date_get(NULL);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_calib_tool_get function is successful.
 * 
 * https://codebeamer.daiichi.com/item/195591
 *  
 */
void test_api_audio_calib_tool_get__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    char_t test_version = 99U;

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    helper_buffer_state_ready();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_calib_tool_get(&test_version);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(0U, test_version);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief The test verifies the api_audio_calib_tool_get function is failing 
 * according to the buffer state ready.
 * 
 * https://codebeamer.daiichi.com/item/197179
 *  
 */
void test_api_audio_calib_tool_get__buffer_state__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    char_t test_version = 99U;

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_calib_tool_get(&test_version);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(99U, test_version);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_calib_tool_get function when the 
 * api_audio_deinit function is successful. 
 * 
 * https://codebeamer.daiichi.com/item/195592
 *  
 */
void test_api_audio_calib_tool_get__when_api_audio_not_init__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    char_t test_version = 99U;

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_deinit_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_calib_tool_get(&test_version);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(99U, test_version);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_calib_tool_get function when using an 
 * invalid argument. 
 * 
 * https://codebeamer.daiichi.com/issue/195593
 *  
 */
void test_api_audio_calib_tool_get__when_use_invalid_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_calib_tool_get(NULL);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_calib_ver_get function is successful.
 * 
 * https://codebeamer.daiichi.com/item/195594
 *  
 */
void test_api_api_audio_calib_ver_get__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    char_t test_version = 99U;

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    helper_buffer_state_ready();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_calib_ver_get(&test_version);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(0U, test_version);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief The test verifies the api_audio_calib_ver_get function is failing 
 * according to the buffer state ready.
 * 
 * https://codebeamer.daiichi.com/item/197180
 *  
 */
void test_api_api_audio_calib_ver_get__buffer_state__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    char_t test_version = 99U;

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_calib_ver_get(&test_version);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(99U, test_version);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_calib_ver_get function when the 
 * api_audio_deinit function is successful. 
 * 
 * https://codebeamer.daiichi.com/item/195595
 *  
 */
void test_api_audio_calib_ver_get__when_api_audio_not_init__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    char_t test_version = 99U;

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_deinit_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_calib_ver_get(&test_version);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(99U, test_version);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_calib_ver_get function when using an 
 * invalid argument. 
 * 
 * https://codebeamer.daiichi.com/item/195596
 *  
 */
void test_api_audio_calib_ver_get__when_use_invalid_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_calib_ver_get(NULL);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_freq_get function is successful. 
 * 
 * https://codebeamer.daiichi.com/item/195597
 *  
 */
void test_api_audio_freq_get__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint16_t test_freq = 99U;

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    helper_buffer_state_ready();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_freq_get(&test_freq, 1U);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(0U, test_freq);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief The test verifies the api_audio_freq_get function is failing according 
 * to the buffer state ready.
 * 
 * https://codebeamer.daiichi.com/item/197197
 *  
 */
void test_api_audio_freq_get__buffer_state__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint16_t test_freq = 99U;

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_freq_get(&test_freq, 1U);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(99U, test_freq);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_freq_get function when the 
 * api_audio_deinit function is successful. 
 * 
 * https://codebeamer.daiichi.com/item/195598
 *  
 */
void test_api_audio_freq_get__when_api_audio_not_init__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint16_t test_freq = 99U;

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_deinit_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_freq_get(&test_freq, 1U);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(99U, test_freq);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_freq_get function when using an 
 * invalid argument. 
 * 
 * https://codebeamer.daiichi.com/item/195600
 *  
 */
void test_api_audio_freq_get__when_use_invalid_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_freq_get(NULL, 1U);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_gain_get function is successful. 
 * 
 * https://codebeamer.daiichi.com/item/195601
 *  
 */
void test_api_audio_gain_get__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint16_t test_gain = 99U;

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    helper_buffer_state_ready();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_gain_get(&test_gain, 1U);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(0U, test_gain);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief The test verifies the api_audio_gain_get function is failing according 
 * to the buffer state ready.
 * 
 * https://codebeamer.daiichi.com/item/197198
 *  
 */
void test_api_audio_gain_get__buffer_state__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint16_t test_gain = 99U;

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_gain_get(&test_gain, 1U);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(99U, test_gain);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_gain_get function when the 
 * api_audio_deinit function is successful. 
 * 
 * https://codebeamer.daiichi.com/item/195602
 *  
 */
void test_api_audio_gain_get__when_api_audio_not_init__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint16_t test_gain = 99U;

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_deinit_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_gain_get(&test_gain, 1U);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(99U, test_gain);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_gain_get function when using an 
 * invalid argument. 
 * 
 * https://codebeamer.daiichi.com/item/195603
 *  
 */
void test_api_audio_gain_get__when_use_invalid_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_gain_get(NULL, 1U);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_ver_get function is successful. 
 * 
 * https://codebeamer.daiichi.com/item/195608
 *  
 */
void test_api_audio_ver_get__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    char_t test_audio_version = 99U;

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    helper_buffer_state_ready();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_ver_get(&test_audio_version);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(0U, test_audio_version);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief The test verifies the api_audio_ver_get function is failing according 
 * to the buffer state ready.
 * 
 * https://codebeamer.daiichi.com/item/197199
 *  
 */
void test_api_audio_ver_get__buffer_state__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    char_t test_audio_version = 99U;

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_ver_get(&test_audio_version);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(99U, test_audio_version);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_ver_get function when the 
 * api_audio_deinit function is successful. 
 * 
 * https://codebeamer.daiichi.com/item/195609
 *  
 */
void test_api_audio_ver_get__when_api_audio_not_init__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    char_t test_audio_version = 99U;

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_deinit_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_ver_get(&test_audio_version);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(99U, test_audio_version);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_ver_get function when using an 
 * invalid argument. 
 * 
 * https://codebeamer.daiichi.com/item/195610
 *  
 */
void test_api_audio_ver_get__when_use_invalid_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_audio_ver_get(NULL);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief The test verifies the api_audio_process function sets audio and flash 
 * state when the current audio state switches.
 * 
 * https://codebeamer.daiichi.com/item/195952
 *  
 */
void test_api_audio_process__when_curr_audio_state_switch__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_success();

    helper_api_audio_read_finish_stub();

    api_data_read_ExpectAnyArgsAndReturn(true);

    uint8_t audio_num = 1U;
    // SET current audio state to AUDIO_STATE_SWITCH
    api_audio_switch(audio_num);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    api_audio_process();
}

/**
 * @brief The test verifies the api_audio_process function when set flash state 
 * to FLASH_READ_STATE_INIT when the audio cfg table could not read. 
 * 
 * https://codebeamer.daiichi.com/item/195974
 *  
 */
void test_api_audio_process__when_curr_audio_state_switch__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //
    
    helper_api_audio_deinit_success();

    helper_api_audio_init_success();

    uint8_t audio_num = 1U;
    // SET current audio state to AUDIO_STATE_SWITCH
    api_audio_switch(audio_num); 

    // ************************** Expected Calls **************************** //

    api_data_read_ExpectAnyArgsAndReturn(false);

    api_data_read_ExpectAnyArgsAndReturn(true);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    api_audio_process();

    TEST_STEP(2);
    api_audio_process(); 
}

/**
 * @brief The test verifies the api_audio_process function when set flash state 
 * to FLASH_READ_STATE_INIT when the audio cfg table could not read. 
 * 
 * Note: Also sets read process return value is false in this test.
 * 
 * https://codebeamer.daiichi.com/item/196204
 *  
 */
void test_api_audio_process__read_err_when_curr_audio_state_switch__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //
    
    helper_api_audio_deinit_success();

    helper_api_audio_init_success();

    uint8_t audio_num = 1U;
    // SET current audio state to AUDIO_STATE_SWITCH
    api_audio_switch(audio_num); 

    // ************************** Expected Calls **************************** //

    api_data_read_ExpectAnyArgsAndReturn(false);

    // read funtion returns error 
    api_data_read_ExpectAnyArgsAndReturn(false);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    api_audio_process();

    TEST_STEP(2);
    api_audio_process(); 
}

/**
 * @brief  The test verifies the api_audio_process function when the I2S write 
 * process error. 
 * 
 * https://codebeamer.daiichi.com/item/196004
 *  
 */
void test_api_audio_process__when_i2s_write_err__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // Set invalid variable for write callback
    uint8_t test_invalid_status = 5U;

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_deinit_success();

    helper_api_audio_init_success();

    helper_api_audio_read_finish_stub();

    helper_api_audio_run_state();

    // ************************** Expected Calls **************************** //

    bsp_i2s_write_ExpectAnyArgsAndReturn(false);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    api_audio_process(); 

    TEST_STEP(2);
    m_test_audio_write_cb((bsp_i2s_status_t)BSP_I2S_RX_FULL);

    TEST_STEP(3);
    m_test_audio_write_cb((bsp_i2s_status_t)BSP_I2S_END_TRANSFER);

    TEST_STEP(4);
    m_test_audio_write_cb(test_invalid_status);
}

/**
 * @brief  The test verifies the api_audio_process function if I2S write process 
 * error when the audio state is AUDIO_STATE_RUN and mute.
 * 
 * https://codebeamer.daiichi.com/item/196005
 *  
 */
void test_api_audio_process__when_audio_state_run_mute__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_deinit_success();

    helper_api_audio_init_success();

    helper_api_audio_read_finish_stub();

    helper_api_audio_run_state();

    helper_api_audio_mute_success();

    // ************************** Expected Calls **************************** //

    bsp_i2s_write_ExpectAnyArgsAndReturn(false);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    api_audio_process();
    
    TEST_STEP(2);
    m_test_audio_write_cb((bsp_i2s_status_t)BSP_I2S_TX_EMPTY);
}

/**
 * @brief  The test verifies the api_audio_process function if I2S write process 
 * error when the audio state is AUDIO_STATE_RUN and unmute.
 * 
 * https://codebeamer.daiichi.com/item/196013
 *  
 */
void test_api_audio_process__when_audio_state_run_and_unmute__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_deinit_success();

    helper_api_audio_init_success();

    helper_api_audio_read_finish_stub();

    helper_api_audio_run_state();

    helper_api_audio_unmute_success();

    // ************************** Expected Calls **************************** //

    bsp_i2s_write_ExpectAnyArgsAndReturn(false);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    api_audio_process(); 
    
    TEST_STEP(2);
    m_test_audio_write_cb((bsp_i2s_status_t)BSP_I2S_TX_EMPTY);
}

/**
 * @brief The test verifies the api_audio_process function according to 
 * the calculated gain value.
 * 
 * https://codebeamer.daiichi.com/item/196201
 *  
 */
void test_api_audio_process__calculated_gain__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_init_with_stub_success();

    helper_api_audio_read_finish_success();

    uint16_t speed = 0U;
    api_audio_param_update(speed);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //
   
    TEST_STEP(1);
    api_audio_process();
}

/**
 * @brief The test verifies the api_audio_process function when the shifting 
 * rate value is bigger than the value of the maximum edit parts of the buffer.
 * 
 * https://codebeamer.daiichi.com/item/196202
 *  
 */
void test_api_audio_process__max_edit_cnt_varibale__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    // calculates the maximum buffer size 
    m_test_edit_cnt = true;

    helper_api_audio_deinit_success();
    helper_api_audio_init_with_stub_success();

    helper_api_audio_read_finish_success();

    uint16_t speed = 10U;
    api_audio_param_update(speed);

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //
   
    TEST_STEP(1);
    api_audio_process();
}

/**
 * @brief  The test verifies the api_audio_process function when the audio run 
 * state is checked. 
 * 
 * Note:In the second test step, it is seen that the buffer state is not ready.
 * 
 * https://codebeamer.daiichi.com/issue/196245
 *  
 */
void test_api_audio_process__when_buff_state_not_ready__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_deinit_success();

    helper_api_audio_init_success();

    helper_api_audio_read_finish_stub();

    helper_api_audio_run_state();

    // ************************** Expected Calls **************************** //

    bsp_i2s_write_ExpectAndReturn(NULL, TEST_AUDIO_BUFF_SIZE, true);
    bsp_i2s_write_IgnoreArg_data();

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    api_audio_process();
    
    TEST_STEP(2);
    api_audio_process();
}

/**
 * @brief  The test verifies the api_audio_process function when the audio play  
 * process is continued. 
 *   
 */
void test_api_audio_process__audio_play__success(void)
{
    TEST_CASE();
    
    m_test_edit_cnt = true;

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_deinit_success();
    
    // Initialize audio with stubs 
    api_data_read_StubWithCallback(test_api_data_read);
    bsp_i2s_cb_set_StubWithCallback(test_bsp_i2s_cb_set_stub);
    api_data_cb_set_StubWithCallback(test_bsp_data_cb_set_stub);
    (void)api_audio_init();

    // Update Speed 
    uint16_t speed = 4U;
    api_audio_param_update(speed);

    m_test_data_cb(false);
    m_test_data_cb(false);
    m_test_data_cb(false);

    helper_api_audio_run_state();

    // ************************** Expected Calls **************************** //

    bsp_i2s_write_StubWithCallback(test_i2s_write_stub);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    api_audio_process();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(100U, test_cfg_tbl[92U]);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(100U, test_cfg_tbl[191U]);

    TEST_STEP(4);
    TEST_ASSERT_EQUAL(100U, test_cfg_tbl[288U]);

    TEST_STEP(5);
    TEST_ASSERT_EQUAL(100U, test_cfg_tbl[384U]);
}

/**
 * @brief  The test verifies the api_audio_process function for different gain 
 * value.
 * 
 * https://codebeamer.daiichi.com/item/198562
 *   
 */
void test_api_audio_process__gain_value__success(void)
{
    TEST_CASE();
    
    m_test_edit_cnt = true;

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_deinit_success();
    
    // Init
    api_data_read_StubWithCallback(test_api_data_read);
    bsp_i2s_cb_set_StubWithCallback(test_bsp_i2s_cb_set_stub);
    api_data_cb_set_StubWithCallback(test_bsp_data_cb_set_stub);
    (void)api_audio_init();

    // Update Speed for different gain value
    uint16_t speed = 3U;
    api_audio_param_update(speed);

    m_test_data_cb(false);
    m_test_data_cb(false);
    m_test_data_cb(false);

    helper_api_audio_run_state();

    // ************************** Expected Calls **************************** //

    bsp_i2s_write_StubWithCallback(test_i2s_write_stub);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    api_audio_process();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(90U, test_cfg_tbl[92U]);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(90U, test_cfg_tbl[191U]);

    TEST_STEP(4);
    TEST_ASSERT_EQUAL(90U, test_cfg_tbl[288U]);

    TEST_STEP(5);
    TEST_ASSERT_EQUAL(90U, test_cfg_tbl[384U]);
}

/**
 * @brief  The test verifies the api_audio_process function when the buffer is 
 * shifted. 
 * 
 * https://codebeamer.daiichi.com/item/198565
 *   
 */
void test_api_audio_process__buff_shift__success(void)
{
    TEST_CASE();
    
    m_test_edit_cnt = true;

    // **************************** Input Params **************************** //

    // **************************** Pre-Actions ***************************** //

    helper_api_audio_deinit_success();
    
    // Init
    api_data_read_StubWithCallback(test_api_data_read);
    bsp_i2s_cb_set_StubWithCallback(test_bsp_i2s_cb_set_stub);
    api_data_cb_set_StubWithCallback(test_bsp_data_cb_set_stub);
    (void)api_audio_init();

    // Update Speed 
    uint16_t speed = 4U;
    api_audio_param_update(speed);

    m_test_data_cb(false);
    m_test_data_cb(false);
    m_test_data_cb(false);

    helper_api_audio_run_state();

    // ************************** Expected Calls **************************** //

    bsp_i2s_write_StubWithCallback(test_i2s_write_stub);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    api_audio_process();

    TEST_STEP(2);
    api_audio_param_update((uint16_t)3U);

    TEST_STEP(3);
    m_test_audio_write_cb((bsp_i2s_status_t)BSP_I2S_TX_EMPTY);
   
    TEST_STEP(4);
    api_audio_process();
        
    TEST_STEP(5);
    m_test_data_cb(false);

    TEST_STEP(6);
    api_audio_process();
    
    TEST_STEP(7);
    m_test_audio_write_cb((bsp_i2s_status_t)BSP_I2S_TX_EMPTY);

    TEST_STEP(8);
    api_audio_process();

    TEST_STEP(9);
    TEST_ASSERT_EQUAL(54U, test_cfg_tbl[111U]);

    TEST_STEP(10);
    TEST_ASSERT_EQUAL(54U, test_cfg_tbl[210U]);

    TEST_STEP(11);
    TEST_ASSERT_EQUAL(54U, test_cfg_tbl[307U]);

    TEST_STEP(12);
    TEST_ASSERT_EQUAL(54U, test_cfg_tbl[403U]);
}

/*******************************************************************************
 * Private Functions Definitions
 ******************************************************************************/

static bool test_api_data_read(uint32_t data_label, 
                               uint8_t* p_data, 
                               size_t size, 
                               uint32_t offset, 
                               int cmock_num_calls)
{
    (void)offset;
    (void)data_label;
    (void)cmock_num_calls;

    if(0U == cmock_num_calls)
    {
        *p_data = API_AUDIO_1;

        return true;
    }
    else if(1U == cmock_num_calls)
    {
        uint8_t cfg_tbl[500U] = {
            // audio name
            0x41, 0x55, 0x44, 0x49, 0x4F, 0x2D, 0x31,
            // audio size   0x40, 0x13, 0x03, 0x00,
            [32] = 0x40, 0x13, 0x03, 0x00,
            // audio file version
            0x30, 0x31, 0x2E, 0x30,
            // Calibration version
            0x30, 0x31, 0x2E, 0x30,
            // Calibration tool version
            0x30, 0x31, 0x2E, 0x30,
            // Calibration Date
            0x30, 0x36, 0x2E, 0x30, 0x37, 0x2E, 0x32, 0x30, 0x32, 0x33, 0x2D,
            0x30, 0x39, 0x3A, 0x30, 0x32,
            // Audio Gain - 20 adet.
            [100] = 0xE8, 0x03, 0xE8, 0x03, 0xE8, 0x03, 0x84, 0x03, 0xE8, 0x03,
            0xE8, 0x03, 0xE8, 0x03, 0xE8, 0x03, 0xE8, 0x03, 0xE8, 0x03, 0xE8,
            0x03, 0xE8, 0x03, 0xE8, 0x03, 0xE8, 0x03, 0xE8, 0x03, 0xE8, 0x03,
            0xE8, 0x03, 0xE8, 0x03, 0xE8, 0x03, 0xE8, 0x03,
            // Frequency - 2 adet. 44100 - 48510
            [300] = 0x44, 0xAC, 0xFD, 0xAD,
        };

        if (NULL != p_data)
        {
            memcpy(p_data, cfg_tbl, size);
        }
        return true;
    }
    else
    {
        uint8_t test_data[20U] = {
         // 171,  5,    120,  225,  187,   51,   9,   202,  153,  250,
            100, 110, 120, 130, 140, 150, 160, 170, 180, 190,
         // 204,  16,    65,  25,   5,    38,   6,    236,  3,    32,
            200, 210, 220, 230, 240, 250, 90, 80, 70, 60,
        };

        memcpy(&p_data[92], test_data, 20U);    // 100 - 4

        memcpy(&p_data[191], test_data, 20U);   // 200 - 8

        memcpy(&p_data[288], test_data, 20U);   // 300 - 12

        memcpy(&p_data[384], test_data, 20U);   // 400 - 16

        return true;
    }

    return false;
}

static bool test_i2s_write_stub(const uint8_t* data, size_t size, 
                                int cmock_num_calls)
{
    (void)cmock_num_calls;

    for (size_t i = 0; i < size; i++) {
        test_cfg_tbl[i] = data[i];
    }
    
    return true;
}

static bool test_api_data_read_speed_update_stub(uint32_t data_label, 
                                                 uint8_t* p_data, 
                                                 size_t size, 
                                                 uint32_t offset, 
                                                 int cmock_num_calls)
{
    (void)offset;
    (void)data_label;
    (void)cmock_num_calls;

    if(0U == cmock_num_calls)
    {
        if(true == m_test_current_audio)
        {
            p_data[0U] = API_AUDIO_3 + 1U;
        }

        return true;
    }
    else if(1U == cmock_num_calls)
    {
        for (size_t i = 0; i < size; i++) 
        {
            if(i < 255)
            {
                p_data[i] = i;
            }
            p_data[i] = 0x01;
        }

        uint16_t gain_value = 1000U;
        p_data[TEST_CFG_TBL_GAIN_OFFSET] = (uint8_t)(gain_value & 0xFF);
        p_data[TEST_CFG_TBL_GAIN_OFFSET + 1U] = (uint8_t)(gain_value >> 8U);

        if (true == m_test_edit_cnt)
        {
            uint16_t edit_cnt = 1U;
            p_data[TEST_CFG_TBL_FREQ_OFFSET] = (uint8_t)(edit_cnt & 0xFF);
            p_data[TEST_CFG_TBL_FREQ_OFFSET  + 1U] = (uint8_t)(edit_cnt >> 8U);
        }

        return true;
    }

    return false;
}

static void helper_api_audio_mute_success(void)
{
    drv_amplifier_mute_ExpectAndReturn(true);
    (void)api_audio_mute();
}

static void helper_api_audio_unmute_success(void)
{
    drv_amplifier_unmute_ExpectAndReturn(true);
    (void)api_audio_unmute();
}

static bool test_bsp_i2s_cb_set_stub(bsp_i2s_cb_t callback, int cmock_num_calls)
{
    (void)cmock_num_calls;

    m_test_audio_write_cb = callback;

    return true;
}

static bool test_bsp_data_cb_set_stub(api_data_cb_t callback, 
                                      int cmock_num_calls)
{
    (void)cmock_num_calls;

    m_test_data_cb = callback;

    return true;
}

static void helper_api_audio_run_state(void)
{
    api_audio_process();

    api_audio_process();

    // Sets state to RUN
    (void)api_audio_play(); 
}
    
static void helper_api_audio_init_success(void)
{
    api_data_read_ExpectAnyArgsAndReturn(true);

    api_data_read_ExpectAnyArgsAndReturn(true);

    bsp_i2s_cb_set_StubWithCallback(test_bsp_i2s_cb_set_stub);

    api_data_cb_set_StubWithCallback(test_bsp_data_cb_set_stub);

    (void)api_audio_init();
}

static void helper_buffer_state_ready(void)
{
    api_data_read_ExpectAnyArgsAndReturn(true);

    m_test_data_cb(false);
}

static void helper_api_audio_deinit_success(void)
{
    api_audio_deinit(); 
}

static void helper_api_audio_read_finish_stub(void)
{
    api_data_read_StubWithCallback(test_api_data_read_stub);

    m_test_data_cb(false);
    m_test_data_cb(false);
    m_test_data_cb(false);
}

static void helper_api_audio_read_finish_success(void)
{
    api_data_read_ExpectAnyArgsAndReturn(true);

    m_test_data_cb(false);
    m_test_data_cb(false);
    m_test_data_cb(false);
}

static bool test_api_data_read_stub(uint32_t data_label, 
                                    uint8_t* p_data, 
                                    size_t size, 
                                    uint32_t offset, 
                                    int cmock_num_calls)
{
    (void)offset;
    (void)data_label;
    (void)cmock_num_calls;

    for (size_t i = 0; i < size; i++) {
        p_data[i] = 0x01;
    }

    return true;
}

static void helper_api_audio_init_with_stub_success(void)
{
    api_data_read_StubWithCallback(test_api_data_read_speed_update_stub);

    bsp_i2s_cb_set_StubWithCallback(test_bsp_i2s_cb_set_stub);

    api_data_cb_set_StubWithCallback(test_bsp_data_cb_set_stub);

    (void)api_audio_init();
}

// end of file
