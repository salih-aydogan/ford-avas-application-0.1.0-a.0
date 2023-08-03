/**
 * @file    dcs_rules.hpp
 * @author  Enes AYDIN (enes.aydin@daiichi.com)
 * @brief   Interface of the dcs_rules
 * @version 0.1
 * @date    2021-08-20
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <cstddef>
#include <string>

enum class DCS_RULE
{
    DCS_1_3,
    DCS_3_1,
    DCS_3_2,
    DCS_3_3,
    DCS_3_4,
    DCS_3_5,
    DCS_4_3,
    DCS_4_4,
    DCS_4_5,
    DCS_4_6,
    DCS_4_7,
    DCS_5_1,
    DCS_6_2,
    DCS_7_2,
};

void dcs_setfilenamewithpath(std::string f);

void dcs_rule_print(DCS_RULE rule, std::size_t line);
