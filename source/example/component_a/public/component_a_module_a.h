/**
 * @file component_a_module_a.h
 * @author your name (you@domain.com)
 * @brief The purpose of the file (interface)
 * @version 0.1
 * @date 2021-08-08
 * 
 * @copyright Copyright (c) 2021
 *
 * https://...
 * 
 */

#ifndef COMPONENT_A_MODULE_A_H
#define COMPONENT_A_MODULE_A_H

#include "daiichi_types.h"

// Don't include unnecessary header files

/*******************************************************************************
 * Macros
 ******************************************************************************/

/**
 * @brief Short description. Short description SHOULD NOT exceed single
 *        paragraph. Lines SHOULD be aligned.
 * 
 * Long description. Long description MAY include multiple paragraphs.
 * 
 * Each paragraph SHOULD be started after one blank line. Each line SHOULD
 * be aligned to left. It is good to make each line shorter than the
 * line before for better readability.
 * 
 * https://...
 * 
 */
#define COMPONENT_A_MODULE_B_MY_MACRO 0U

/**
 * @brief List count macros SHOULD end with _CNT.
 * 
 * https://...
 * 
 */
#define COMPONENT_A_MODULE_B_MY_LIST_CNT 10U

/**
 * @brief Time indicating macros SHOULD end with time unit like _MS.
 * 
 * https://...
 * 
 */
#define COMPONENT_A_MODULE_B_MY_TIMEOUT_MS 100U

/**
 * @brief Configuration macros SHOULD start with _CONF.
 * 
 * https://...
 * 
 */
#define COMPONENT_A_MODULE_B_CONF_MY_SETTING 5U

/*******************************************************************************
 * Type Definitions
 ******************************************************************************/

/**
 * @brief ...
 * 
 * https://...
 * 
 */
typedef uint32_t component_a_module_a_my_type_t;

/*******************************************************************************
 * Enumerations
 ******************************************************************************/

/**
 * @brief Enumerations SHOULD be named. Names SHOULD end with _e.
 * 
 * https://...
 * 
 */
typedef enum component_a_module_a_my_enum_e
{
    /**
     * @brief Each enum item SHOULD be commented.
     * 
     */
    COMPONENT_A_MODULE_A_MY_ENUM_1 = 0U,

    /**
     * @brief Comment style SHOULD be the same for every item.
     * 
     */
    COMPONENT_A_MODULE_A_MY_ENUM_2 = 1U,

} component_a_module_a_my_enum_t;

/**
 * @brief Enumerations SHOULD be typed. Types name SHOULD be the same but
 *        ending with _t.
 * 
 * https://...
 * 
 */
typedef enum component_a_module_a_my_enum_e
{
    COMPONENT_A_MODULE_A_MY_ENUM_1 = 0U, /**< First item SHOULD always indicate 
                                              a value. */

    COMPONENT_A_MODULE_A_MY_ENUM_2 = 1U, /**< Last item SHOULD always be
                                              followed by a comma. */

} component_a_module_a_my_enum_t;

/**
 * @brief ...
 * 
 * https://...
 * 
 */
typedef enum component_a_module_a_my_enum_e
{
    /// There SHOULD be a blank line between items.
    COMPONENT_A_MODULE_A_MY_ENUM_1 = 0U,

    /// There SHOULD be an empty line after last item.
    /// Multiple lines are allowed.
    COMPONENT_A_MODULE_A_MY_ENUM_2 = 1U,

} component_a_module_a_my_enum_t;

/*******************************************************************************
 * Structs
 ******************************************************************************/

/**
 * @brief Structs SHOULD be named. Names SHOULD end with _s.
 * 
 * https://...
 * 
 */
typedef struct component_a_module_a_my_struct_s
{
    uint32_t my_integer;    /**< All struct elements SHOULD be commented. */

    bool my_bool;           /**< Comments SHOUDL be aligned. Extra lines
                                 SHOULD be aligned. */

    uint8_t b_my_bool;      /**< ... */

    uint32_t *p_my_pointer; /**< ... */

} component_a_module_a_my_struct_t;

/**
 * @brief Structs SHOULD be typed. Types name SHOULD be the same but
 *        ending with _t.
 * 
 * https://...
 * 
 */
typedef struct component_a_module_a_my_struct_s
{
    /// There SHOULD be a blank line between items.
    uint32_t my_integer;

    /// ...
    bool my_bool;

    /// ...
    uint8_t b_my_bool;

    /// There SHOULD be an empty line after last item.
    /// Multiple lines are allowed.
    uint32_t *p_my_pointer;

} component_a_module_a_my_struct_t;

/*******************************************************************************
 * Public Variables
 ******************************************************************************/

/**
 * @brief ...
 * 
 * Main component header component_a.h SHOULD be included whereever this
 * variable is used.
 * 
 * https://...
 * 
 */
extern uint32_t g_component_a_module_a_my_public_variable;

/*******************************************************************************
 * Public Functions 
 ******************************************************************************/

/**
 * @brief Setter function example.
 * 
 * https://...
 * 
 * @param value 
 * 
 */
void component_a_module_a_my_object_my_value_set(uint32_t value);

/**
 * @brief Getter function example.
 * 
 * https://...
 * 
 * @return uin32_t 
 * 
 * @retval -1 Important return values SHOULD be commented.
 * @retval 0  Descriptions SHOULD be aligned.
 * 
 */
uint32_t component_a_module_a_my_object_my_value_get();

/**
 * @brief Check function example.
 * 
 * https://...
 * 
 * @return bool Short description. There SHOULD be a one blank line after
 *              return type description.
 * 
 * @retval true  Short description.
 * 
 * @retval false Possible Errors:
 *               - Errors SHOUDL be displayed as list.
 *               - Multi line list items SHOULD be aligned with
 *                 first letter.
 * 
 */
bool component_a_module_a_my_object_my_bool_check();

/**
 * @brief Action function example.
 * 
 * https://...
 * 
 * @param value      Comments SHOULD be started 1 space after longest param name.
 * @param long_value Comments SHOUDL be aligned.
 * 
 * @return true  Bool values COULD be showed with return instead of retval.
 *               There SHOULD be a blank line after multi line comments.
 * 
 * @return false Main return SHOULD be ommited in this case. Only values
 *               are shown.
 * 
 */
bool component_a_module_a_my_object_my_action(uint32_t value, uint32_t long_value);

/**
 * @brief Action function example.
 * 
 * https://...
 * 
 * @param[in]  value   All direction SHOULD be written if there is any
 *                     outout params. Parameter names should be aligned
 *                     to the first letter.
 * 
 * @param[out] p_value Short description.
 *                     Important Values:
 *                     - -1 Error
 *                     - 0  Empty list.
 * 
 * @return true  Bool values COULD be showed with return instead of retval.
 *               There SHOULD be a blank line after multi line comments.
 * 
 * @return false Main return SHOULD be ommited in this case. Only values
 *               are shown.
 * 
 */
bool component_a_module_a_my_object_my_action(uint32_t value, uint32_t *p_value);

/**
 * @brief Action function example.
 * 
 * https://...
 * 
 * @return component_a_module_a_my_enum_t Short description. There SHOULD be no
 *                                        blank line in an item comment. Single
 *                                        paragraph only.
 * 
 * @retval COMPONENT_A_MODULE_A_MY_ENUM_1 Short description about why this
 *                                        value is returned.
 * 
 * @retval COMPONENT_A_MODULE_A_MY_ENUM_1 There should be 1 blank line before and 
 *                                        after multi line comments.
 * 
 * @retval COMPONENT_A_MODULE_A_MY_ENUM_1 Single line comment.
 * @retval COMPONENT_A_MODULE_A_MY_ENUM_1 Single line comment.
 * 
 * @retval COMPONENT_A_MODULE_A_MY_ENUM_1 Possible erros:
 *                                        - Error 1 description.
 *                                        - Error 2 description with multi
 *                                          line comment.
 *                                        - Error 3 descriotion.
 * 
 */
component_a_module_a_my_enum_t component_a_module_a_my_feature_my_action();

#endif /* COMPONENT_A_MODULE_A_H  */

// end of file
