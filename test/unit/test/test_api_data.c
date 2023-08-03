/**
 * @file test_api_data.c
 * @author Muhammed Salih AYDOGAN
 * @brief These tests verify the units of the API DATA module.
 * 
 * @version 0.1
 * 
 * @date 19/05/2023
 * 
 * @copyright Copyright (c) 2023
 * 
 * https://codebeamer.daiichi.com/item/192891
 * 
 */

/*******************************************************************************
 * Includes 
 ******************************************************************************/

//First - include test framework headers
#include "unity.h"
#include "support.h"

// Second - include the module to be tested
#include "api_data.h"

// Third - include the mocks of required modules
#include "mock_bsp_eeprom.h"
#include "mock_bsp_mcu.h"
#include "mock_drv_flash.h"

#include "mock_log.h"

/*******************************************************************************
 * Test Files
 ******************************************************************************/

TEST_FILE("api_data.c");

/*******************************************************************************             
 * Defines
 ******************************************************************************/
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
 * @brief This macro indicates the audio file - 1 configuration table address to 
 *        be saved in Flash.
 * 
 */
#define TEST_API_DATA_LABEL_AUDIO_1_CFG (TEST_API_DATA_EEPROM_LAST_DATA)

/**
 * @brief This macro indicates the audio file - 1 data address.
 * 
 */
#define TEST_API_DATA_LABEL_AUDIO_1 \
    (TEST_API_DATA_LABEL_AUDIO_1_CFG + TEST_API_DATA_AUDIO_CFG_TBL_SIZE)

/**
 * @brief This macro indicates the audio file - 2 configuration table address to 
 *        be saved in Flash.
 * 
 */
#define TEST_API_DATA_LABEL_AUDIO_2_CFG \
    (TEST_API_DATA_LABEL_AUDIO_1 + TEST_API_DATA_AUDIO_DEFAULT_SIZE)

/**
 * @brief This macro indicates the audio file - 2 data address.
 * 
 */
#define TEST_API_DATA_LABEL_AUDIO_2 \
    (TEST_API_DATA_LABEL_AUDIO_2_CFG + TEST_API_DATA_AUDIO_CFG_TBL_SIZE)

/**
 * @brief This macro indicates the audio file - 3 configuration table address to 
 *        be saved in Flash.
 */
#define TEST_API_DATA_LABEL_AUDIO_3_CFG \
    (TEST_API_DATA_LABEL_AUDIO_2 + TEST_API_DATA_AUDIO_DEFAULT_SIZE)

/**
 * @brief This macro indicates the audio file - 3 data address.
 * 
 */
#define TEST_API_DATA_LABEL_AUDIO_3 \
    (TEST_API_DATA_LABEL_AUDIO_3_CFG + TEST_API_DATA_AUDIO_CFG_TBL_SIZE)

/**
 * @brief This macro indicates the last data address of external flash.
 * 
 */
#define TEST_API_DATA_FLASH_LAST_DATA \
    (TEST_API_DATA_LABEL_AUDIO_3 + TEST_API_DATA_AUDIO_DEFAULT_SIZE)

/*******************************************************************************
 * Private Functions Prototype Declarations
 ******************************************************************************/
/**
 * @brief This helper function uses for flash initialization.
*/
static void helper_flash_init(void);

/**
 * @brief This helper function uses for flash initialization wtih fail.
*/
static void helper_flash_init_with_fail(void);

/**
 * @brief This test function for using as the callback.
*/
static void test_api_data_cb(bool err);

/**
 * @brief This stub function use for bsp_eeprom_read function. 
*/
bsp_eeprom_status_t test_bsp_eeprom_read_stub(uint32_t addr, 
                                              uint8_t* p_data, 
                                              size_t size, 
                                              int cmock_num_calls);

/**
 * @brief This stub function use for bsp_eeprom_read function. 
*/
static bsp_eeprom_status_t test_api_data_write_stub(uint32_t addr, 
                                                    const uint8_t* p_data, 
                                                    size_t size, 
                                                    int cmock_num_calls);

/**
 * @brief This stub function use for bsp_eeprom_read function. 
*/
static bsp_eeprom_status_t test_api_data_write(uint32_t addr, 
                                                    const uint8_t* p_data, 
                                                    size_t size, 
                                                    int cmock_num_calls);
/**
 * @brief This stub function use for bsp_eeprom_read function. 
*/
static bsp_eeprom_status_t test_api_data_read_stub(uint32_t addr, 
                                                   uint8_t* p_data, 
                                                   size_t size, 
                                                   int cmock_num_calls);
/**
 * @brief This variable uses for create fake area.
*/
static uint8_t m_test_area[2048] = {99U};

/**
 * @brief This variable uses compare default table. 
*/
uint32_t m_test_eeprom_default_tbl[2U][3U] = {0U};

/*******************************************************************************
 * Setup & Teardown 
 ******************************************************************************/

void setUp()
{
    log_print_Ignore();
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
 * @brief Verifies the api_data_init function when initializing the component's 
 * variables and the flash driver is successfully done.
 * 
 * https://codebeamer.daiichi.com/issue/192910
 *  
 */
void test_api_data_init__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    const uint8_t test_delay = 10U;
    int8_t test_cnt = 5;

    drv_flash_init_ExpectAndReturn(true);
    bsp_eeprom_read_StubWithCallback(test_bsp_eeprom_read_stub);

    for(int8_t cnt = 0; cnt < test_cnt; cnt++)
    {
        bsp_mcu_delay_Expect(test_delay);
    }

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_data_init();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief Verifies the api_data_init function when initializing the flash driver 
 * is failed.
 * 
 * https://codebeamer.daiichi.com/item/192913
 *  
 */
void test_api_data_init__when_external_flash_initialize_fail__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    drv_flash_init_ExpectAndReturn(false);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_data_init();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies the api_data_init function when the default set process is 
 * failed.
 * 
 * https://codebeamer.daiichi.com/item/192933
 *  
 */
void test_api_data_init__when_default_set_fail__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    uint8_t eeprom_auth = 0x00U;
    uint8_t test_delay = 10U;

    drv_flash_init_ExpectAndReturn(true);

    const uint32_t max_try_cnt = 10U;
    uint32_t try_idx;

    for (try_idx = 0U; 
         (max_try_cnt > try_idx); 
         try_idx++)
    {
        bsp_eeprom_read_ExpectAndReturn(TEST_API_DATA_LABEL_AUTH, 
                                        &eeprom_auth, 
                                        1U, 
                                        BSP_EEPROM_STATUS_SUCCESS);
        bsp_mcu_delay_Expect(test_delay);
    }

    uint32_t eeprom_addr = TEST_API_DATA_LABEL_CAR_SPEED;
    const uint8_t data_tbl[250U] = {0U};
    size_t data_tbl_size = 250U;

    // eeprom reset
    bsp_eeprom_write_ExpectAndReturn(eeprom_addr, 
                                     data_tbl, 
                                     data_tbl_size, 
                                     true);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_data_init();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies the api_data_init function when the default set process is 
 * failed.
 * 
 * https://codebeamer.daiichi.com/issue/192937
 *  
 */
void test_api_data_init__when_default_set_success__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
 
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    uint8_t eeprom_auth = 0x00U;
    uint8_t test_delay = 10U;

    drv_flash_init_ExpectAndReturn(true);

    const uint32_t max_try_cnt = 10U;
    uint32_t try_idx;

    for (try_idx = 0U; 
         (max_try_cnt > try_idx); 
         try_idx++)
    {
        bsp_eeprom_read_ExpectAndReturn(TEST_API_DATA_LABEL_AUTH, 
                                        &eeprom_auth, 
                                        1U, 
                                        BSP_EEPROM_STATUS_SUCCESS);
        bsp_mcu_delay_Expect(test_delay);
    }

    bsp_eeprom_write_StubWithCallback(test_api_data_write);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_data_init();

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);

    TEST_STEP(3);
    TEST_ASSERT_EQUAL(0xAAU, m_test_eeprom_default_tbl[0][0U]);

    TEST_STEP(4);
    TEST_ASSERT_EQUAL(TEST_API_DATA_LABEL_AUTH, 
                      m_test_eeprom_default_tbl[0][1U]);
    
    TEST_STEP(5);
    TEST_ASSERT_EQUAL(1U, m_test_eeprom_default_tbl[0][2U]);
    
    TEST_STEP(6);
    TEST_ASSERT_EQUAL(1U, m_test_eeprom_default_tbl[1][0U]);
    
    TEST_STEP(7);
    TEST_ASSERT_EQUAL(TEST_API_DATA_LABEL_CURR_AUDIO, 
                      m_test_eeprom_default_tbl[1][1U]);
    
    TEST_STEP(8);
    TEST_ASSERT_EQUAL(1U, m_test_eeprom_default_tbl[1][2U]);
}

/**
 * @brief Verifies the api_data_read function when reading data at EEPROM memory 
 * location successfully.
 * 
 * https://codebeamer.daiichi.com/issue/192939
 *  
 */
void test_api_data_read__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    
    // set data label for return EEPROM memmory location
    uint32_t data_label = TEST_API_DATA_EEPROM_LAST_DATA - 1U;

    uint8_t data = 10U;

    size_t size = 1U;
    uint32_t offset = 1U;

    // **************************** Pre-Actions ***************************** //

    helper_flash_init();

    // ************************** Expected Calls **************************** //

    bsp_eeprom_read_ExpectAndReturn(data_label, 
                                    &data, 
                                    size, 
                                    BSP_EEPROM_STATUS_SUCCESS);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_data_read(data_label, &data, size, offset);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief Verifies the api_data_cb_set function successfully.
 * 
 * https://codebeamer.daiichi.com/item/198372
 *  
 */
void test_api_data_cb_set__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    
    api_data_cb_t test_cb = test_api_data_cb;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    drv_flash_cb_set_ExpectAndReturn((drv_flash_cb_t)test_cb, true);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_data_cb_set(test_cb);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief Verifies the api_data_cb_set function when flash callback set function 
 * error.
 * 
 * https://codebeamer.daiichi.com/item/198373
 *  
 */
void test_api_data_cb_set__flash_err__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    drv_flash_cb_set_ExpectAnyArgsAndReturn(false);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_data_cb_set(NULL);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies the api_data_process function successfully.
 * 
 * https://codebeamer.daiichi.com/item/198374
 *  
 */
void test_api_data_process__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    drv_flash_process_Expect();
    
    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    api_data_process();
}

/**
 * @brief Verifies the api_data_read function when reading data at EEPROM memory 
 * location failed.
 * 
 * https://codebeamer.daiichi.com/item/192940
 *  
 */
void test_api_data_read__when_eeprom_read_fail__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    
    uint32_t data_label = TEST_API_DATA_EEPROM_LAST_DATA - 1U;

    uint8_t data = 10U;

    size_t size = 1U;
    uint32_t offset = 1U;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    bsp_eeprom_read_ExpectAnyArgsAndReturn(BSP_EEPROM_STATUS_NOT_RDY);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_data_read(data_label, &data, size, offset);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies the api_data_read function when the sum of the given address 
 * and data size exceeds the FlexRAM size.
 * 
 * https://codebeamer.daiichi.com/item/193166
 *  
 */
void test_api_data_read__when_eeprom_read_addr_size_err__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    
    uint32_t data_label = TEST_API_DATA_EEPROM_LAST_DATA - 1U;

    uint8_t data = 10U;

    size_t size = 1U;
    uint32_t offset = 1U;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    //The sum of the given address and data size exceeds the FlexRAM size.
    bsp_eeprom_read_ExpectAnyArgsAndReturn(BSP_EEPROM_STATUS_ERR_ADDR_SIZE);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_data_read(data_label, &data, size, offset);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies the api_data_read function when reading data at external 
 * flash memory location successfully.
 * 
 * https://codebeamer.daiichi.com/item/192941
 *  
 */
void test_api_data_read__when_use_ext_flash_mem_location__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    
    // set data label for return external flash memory location
    uint32_t data_label = TEST_API_DATA_EEPROM_LAST_DATA + 1U;

    uint8_t data = 10U;

    size_t size = 1U;
    uint32_t offset = 1U;

    // **************************** Pre-Actions ***************************** //

    helper_flash_init();

    // ************************** Expected Calls **************************** //

    uint32_t p_addr = (data_label - TEST_API_DATA_EEPROM_LAST_DATA) + offset;

    drv_flash_read_ExpectAndReturn(p_addr, &data, size, true);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_data_read(data_label, &data, size, offset);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief Verifies the api_data_read function when using an external flash 
 * memory limit. 
 * 
 * https://codebeamer.daiichi.com/item/193171
 *  
 */
void test_api_data_read__external_flash_limit__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    
    // set data label for return external flash memory location
    uint32_t data_label = TEST_API_DATA_EEPROM_LAST_DATA + 100U;

    uint8_t data = 10U;

    size_t size = 1U;

    // set big offset value 
    uint32_t offset = 4290067294U;

    // **************************** Pre-Actions ***************************** //

    helper_flash_init();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_data_read(data_label, &data, size, offset);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief Verifies the api_data_read function when using an unknown memory 
 * location. 
 * 
 * https://codebeamer.daiichi.com/item/193184
 *  
 */
void test_api_data_read__when_use_unknown_mem_location__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    
    // set data label for return external flash memory location
    uint32_t data_label = TEST_API_DATA_FLASH_LAST_DATA + 1U;

    uint8_t data = 10U;

    size_t size = 1U;
    uint32_t offset = 1U;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_data_read(data_label, &data, size, offset);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies the api_data_read function when the drv_flash_read() function 
 * failed. 
 * 
 * https://codebeamer.daiichi.com/item/193185
 *  
 */
void test_api_data_read__when_ext_flash_read_err__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    
    // set data label for return external flash memory location
    uint32_t data_label = TEST_API_DATA_EEPROM_LAST_DATA + 1U;

    uint8_t data = 10U;

    size_t size = 1U;
    uint32_t offset = 1U;

    // **************************** Pre-Actions ***************************** //

    helper_flash_init();

    // ************************** Expected Calls **************************** //

    uint32_t p_addr = (data_label - TEST_API_DATA_EEPROM_LAST_DATA) + offset;

    // external flash read process with error
    drv_flash_read_ExpectAndReturn(p_addr, &data, size, false);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_data_read(data_label, &data, size, offset);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies the api_data_read function with an invalid data argument. 
 * 
 * https://codebeamer.daiichi.com/item/192973
 *  
 */
void test_api_data_read__when_use_invalid_data_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint32_t data_label = TEST_API_DATA_EEPROM_LAST_DATA + 1U;
    size_t size = 1U;
    uint32_t offset = 1U;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_data_read(data_label, NULL, size, offset);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies the api_data_read function when external flash not 
 * initialize.
 * 
 * https://codebeamer.daiichi.com/item/193186
 *  
 */
void test_api_data_read__when_external_flash_not_init__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    
    // set data label for return external flash memory location
    uint32_t data_label = TEST_API_DATA_EEPROM_LAST_DATA + 1U;

    uint8_t data = 10U;

    size_t size = 1U;
    uint32_t offset = 1U;

    // **************************** Pre-Actions ***************************** //

    helper_flash_init_with_fail();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_data_read(data_label, &data, size, offset);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies the api_data_write function with an invalid data argument. 
 * 
 * https://codebeamer.daiichi.com/item/193064
 *  
 */
void test_api_data_write__when_use_invalid_data_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //

    uint32_t data_label = TEST_API_DATA_EEPROM_LAST_DATA + 1U;
    size_t size = 1U;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_data_write(data_label, NULL, size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies the api_data_write function when select EEPROM memory 
 * location.
 * 
 * https://codebeamer.daiichi.com/item/193187
 *  
 */
void test_api_data_write__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    // Select EEPROM memory location
    uint32_t data_label = TEST_API_DATA_EEPROM_LAST_DATA - 1U;

    uint8_t data = 1U;

    size_t size = 1U;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    bsp_eeprom_write_ExpectAndReturn(data_label, 
                                     &data, 
                                     size, 
                                     BSP_EEPROM_STATUS_SUCCESS);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_data_write(data_label, &data, size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief Verifies the api_data_read function after writing on the related 
 * memory location with the write function.
 * 
 * https://codebeamer.daiichi.com/item/194057
 * 
 */
void test_api_data_read__write_related_memory_location__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    // Select EEPROM memory location
    uint32_t data_label = TEST_API_DATA_EEPROM_LAST_DATA - 1U;

    uint8_t data = 77U;

    uint8_t test_data_read = 0U;

    uint32_t offset = 1U;

    size_t size = sizeof(data);

    // **************************** Pre-Actions ***************************** //

    helper_flash_init();

    // ************************** Expected Calls **************************** //

    bsp_eeprom_write_StubWithCallback(test_api_data_write_stub);

    bsp_eeprom_read_StubWithCallback(test_api_data_read_stub);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_data_write(data_label, &data, size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);

    TEST_STEP(3);
    ret = api_data_read(data_label, &test_data_read, size, offset);

    TEST_STEP(4);
    TEST_ASSERT_EQUAL(true, ret);

    TEST_STEP(5);
    TEST_ASSERT_EQUAL(data, test_data_read);
}

/**
 * @brief Verifies the api_data_write function when the sum of the given address 
 * and data size exceeds the FlexRAM size.
 * 
 * https://codebeamer.daiichi.com/item/193188
 *  
 */
void test_api_data_write__when_eeprom_status_err_addr_size__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    // Select EEPROM memory location
    uint32_t data_label = TEST_API_DATA_EEPROM_LAST_DATA - 1U;

    size_t size = 1U;

    uint8_t data = 1U;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // The sum of the given address and data size exceeds the FlexRAM size.
    bsp_eeprom_write_ExpectAndReturn(data_label, 
                                     &data, 
                                     size, 
                                     BSP_EEPROM_STATUS_ERR_ADDR_SIZE);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_data_write(data_label, &data, size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies the api_data_write function when the EEPROM write process 
 * gives the generic error. 
 * 
 * https://codebeamer.daiichi.com/item/193189
 *  
 */
void test_api_data_write__when_eeprom_status_err__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    // Select EEPROM memory location
    uint32_t data_label = TEST_API_DATA_EEPROM_LAST_DATA - 1U;

    size_t size = 1U;

    uint8_t data = 1U;

    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    //The EEPROM write generic error.
    bsp_eeprom_write_ExpectAndReturn(data_label, 
                                     &data, 
                                     size, 
                                     BSP_EEPROM_STATUS_ERR);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_data_write(data_label, &data, size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies the api_data_write function when the external flash write 
 * process is successful.
 * 
 * https://codebeamer.daiichi.com/item/193190
 *  
 */
void test_api_data_write__when_external_flash_select__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    // Select external flash memory location
    uint32_t data_label = TEST_API_DATA_EEPROM_LAST_DATA + 1U;

    size_t size = 1U;

    uint8_t data = 1U;

    // **************************** Pre-Actions ***************************** //

    helper_flash_init();

    // ************************** Expected Calls **************************** //

    uint32_t p_addr = (data_label - TEST_API_DATA_EEPROM_LAST_DATA);
    drv_flash_write_ExpectAndReturn(p_addr, &data, size, true);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_data_write(data_label, &data, size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief Verifies the api_data_write function when the external flash write 
 * process is fail.
 * 
 * https://codebeamer.daiichi.com/item/193191
 *  
 */
void test_api_data_write__when_external_flash_write_err__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    // Select external flash memory location
    uint32_t data_label = TEST_API_DATA_EEPROM_LAST_DATA + 1U;

    size_t size = 1U;

    uint8_t data = 1U;

    // **************************** Pre-Actions ***************************** //

    helper_flash_init();

    // ************************** Expected Calls **************************** //

    uint32_t p_addr = (data_label - TEST_API_DATA_EEPROM_LAST_DATA);

    // write process fail
    drv_flash_write_ExpectAndReturn(p_addr, &data, size, false);

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_data_write(data_label, &data, size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies the api_data_write function when the memory location is 
 * unknown.
 * 
 * https://codebeamer.daiichi.com/item/193192
 *  
 */
void test_api_data_write__when_memory_location_unknown__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    // Select external flash memory location
    uint32_t data_label = TEST_API_DATA_FLASH_LAST_DATA + 1U;

    size_t size = 1U;

    uint8_t data = 1U;

    // **************************** Pre-Actions ***************************** //

    helper_flash_init();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_data_write(data_label, &data, size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies the api_data_write function when the external flash is not 
 * init.
 * 
 * https://codebeamer.daiichi.com/item/193193
 *  
 */
void test_api_data_write__when_external_flash_not_init__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    // Select external flash memory location
    uint32_t data_label = TEST_API_DATA_EEPROM_LAST_DATA + 1U;

    size_t size = 1U;

    uint8_t data = 1U;

    // **************************** Pre-Actions ***************************** //

    // External flash not init
    helper_flash_init_with_fail();

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_data_write(data_label, &data, size);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/**
 * @brief Verifies the api_data_dtc_set function with a valid DTC argument.
 * 
 * https://codebeamer.daiichi.com/item/193152
 *  
 */
void test_api_data_dtc_set__usual_run__success(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    uint32_t data_label = TEST_API_DATA_EEPROM_LAST_DATA + 1U;
    uint32_t dtc = 1U;
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_data_dtc_set(data_label, dtc);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(true, ret);
}

/**
 * @brief Verifies the api_data_dtc_set function with an invalid DTC argument.
 * 
 * https://codebeamer.daiichi.com/item/193159
 *  
 */
void test_api_data_dtc_set__when_use_invalid_dtc_argument__fail(void)
{
    TEST_CASE();

    // **************************** Input Params **************************** //
    uint32_t data_label = TEST_API_DATA_EEPROM_LAST_DATA + 1U;

    // invalid dtc
    uint32_t dtc = 0U;
    // **************************** Pre-Actions ***************************** //

    // ************************** Expected Calls **************************** //

    // **************************** Test Steps ****************************** //

    TEST_STEP(1);
    bool ret = api_data_dtc_set(data_label, dtc);

    TEST_STEP(2);
    TEST_ASSERT_EQUAL(false, ret);
}

/*******************************************************************************
 * Private Functions Definitions
 ******************************************************************************/
static void helper_flash_init()
{
    uint8_t eeprom_auth = 0x00U;
    uint8_t test_delay = 10U;

    drv_flash_init_ExpectAndReturn(true);

    const uint32_t max_try_cnt = 10U;
    uint32_t try_idx;

    for (try_idx = 0U; 
         (max_try_cnt > try_idx); 
         try_idx++)
    {
        bsp_eeprom_read_ExpectAndReturn(TEST_API_DATA_LABEL_AUTH, 
                                        &eeprom_auth, 
                                        1U, 
                                        BSP_EEPROM_STATUS_SUCCESS);
        bsp_mcu_delay_Expect(test_delay);
    }

    // eeprom reset
    bsp_eeprom_write_IgnoreAndReturn(false);
    
    (void)api_data_init();
}

static void helper_flash_init_with_fail()
{
    drv_flash_init_ExpectAndReturn(false);

    (void)api_data_init();
}

bsp_eeprom_status_t test_bsp_eeprom_read_stub(uint32_t addr, 
                                              uint8_t* p_data, 
                                              size_t size, 
                                              int cmock_num_calls)
{
    (void)addr;
    (void)p_data;
    (void)size;
    (void)cmock_num_calls;

    if (5 == cmock_num_calls)
    {
        *p_data = 0xAAU;
    }

    return BSP_EEPROM_STATUS_SUCCESS;
}

static bsp_eeprom_status_t test_api_data_write_stub(uint32_t addr, 
                                                    const uint8_t* p_data, 
                                                    size_t size, 
                                                    int cmock_num_calls)
{
    (void)cmock_num_calls;

    memcpy(&m_test_area[addr], p_data, size);

    return BSP_EEPROM_STATUS_SUCCESS;
}

static bsp_eeprom_status_t test_api_data_write(uint32_t addr, 
                                               const uint8_t* p_data, 
                                               size_t size, 
                                               int cmock_num_calls)
{
    if(3 == cmock_num_calls)
    {
        m_test_eeprom_default_tbl[0][0U] = *p_data;
        m_test_eeprom_default_tbl[0][1U] = addr;
        m_test_eeprom_default_tbl[0][2U] = size;

        return BSP_EEPROM_STATUS_SUCCESS;
    }
    if (4 == cmock_num_calls)
    {
        m_test_eeprom_default_tbl[1][0U] = *p_data;
        m_test_eeprom_default_tbl[1][1U] = addr;
        m_test_eeprom_default_tbl[1][2U] = size;
        return BSP_EEPROM_STATUS_SUCCESS;
    }
    else
    {
        return BSP_EEPROM_STATUS_SUCCESS;
    }
}

static bsp_eeprom_status_t test_api_data_read_stub(uint32_t addr,
                                                   uint8_t* p_data, 
                                                   size_t size, 
                                                   int cmock_num_calls)
{
    (void)cmock_num_calls;
    
    memcpy(p_data, &m_test_area[addr], size);

    return BSP_EEPROM_STATUS_SUCCESS;
}

static void test_api_data_cb(bool err)
{
    (void)err;
}

// end of file
