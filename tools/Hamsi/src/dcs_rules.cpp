/**
 * @file    dcs_rules.cpp
 * @author  Enes AYDIN (enes.aydin@daiichi.com)
 * @brief   Implementation of the dcs_rules
 * @version 0.1
 * @date    2021-08-20
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "dcs_rules.hpp"

#include <iostream>

static std::string filenamewithpath;

void dcs_setfilenamewithpath(std::string f)
{
    filenamewithpath = f;
}

void dcs_rule_print(DCS_RULE rule, std::size_t line)
{
    auto &out = std::cout;
    
    out << filenamewithpath << " LINE : " << line << " ";
    
    if (DCS_RULE::DCS_1_3 == rule)
    {
        out << "DCS-1.3: Each parameter check shall be performed with a different if statement.";
    }
    else if (DCS_RULE::DCS_3_1 == rule)
    {
        out << "DCS-3.1: A ternary operator expression shall be divided into 3 lines if the expression exceeds the 80 "
        "char limit. Each operand shall be placed into a separate line. Operators shall be placed at the beginning of the "
        "last 2 lines. Operators shall be aligned with the beginning of the ternary expression.";
    }
    else if (DCS_RULE::DCS_3_2 == rule)
    {
        out << "DCS-3.2: Each function parameter shall be in a separate line and shall be aligned with the first "
        "parameter if the function signature exceeds the 80 char limit. The first parameter shall be in the same line with "
        "the function name if it fits in 80 chars.";
    }
    else if (DCS_RULE::DCS_3_3 == rule)
    {
        out << "DCS-3.3: When switching to a new line; the \"=\" operator shall be placed in the first line and the "
        "second line shall be indented 4 spaces after the starting point of the first line.";
    }
    else if (DCS_RULE::DCS_3_4 == rule)
    {
        out << "DCS-3.4: When dividing an expression into multiple lines; each operand shall be placed in a new line "
        "and operators shall be placed at the end of each line. Each line shall be aligned with the first operand.";
    }
    else if (DCS_RULE::DCS_3_5 == rule)
    {
        out << "DCS-3.5: When switching to a new line in an if statement; each line shall contain only one expression"
        " and the logical operators shall be at the end of each line. Each line shall be aligned to the right of the "
        "opening \"(\" of the if statement.";
    }
    else if (DCS_RULE::DCS_4_3 == rule)
    {
        out << "DCS-4.3: The first and last lines of a block shall not be blank.";
    }
    else if (DCS_RULE::DCS_4_4 == rule)
    {
        out << "DCS-4.4: An empty line shall be placed before and after \"case:\", \"default:\", and \"break;\" "
        "keywords in a switch statement.";
    }
    else if (DCS_RULE::DCS_4_5 == rule)
    {
        out << "DCS-4.5: The last element of an enum definition shall end with a comma followed by a blank line.";
    }
    else if (DCS_RULE::DCS_4_6 == rule)
    {
        out << "DCS-4.6: There shall be a blank line after the last variable of a struct.";
    }
    else if (DCS_RULE::DCS_4_7 == rule)
    {
        out << "DCS-4.7: The opening \"{\" symbol of an array definition shall be placed after the \"=\" sign at the"
        " same line followed by a blank line. There shall be no blank line before the closing \"}\" symbol.";
    }
    else if (DCS_RULE::DCS_5_1 == rule)
    {
        out << "DCS-5.1: The last element of an array shall be followed by a comma if elements are placed in new "
        "lines.";
    }
    else if (DCS_RULE::DCS_6_2 == rule)
    {
        out << "DCS-6.2: All standard library headers shall be included with <> syntax. Other headers shall be "
        "included with \"\" syntax.";
    }
    else if (DCS_RULE::DCS_7_2 == rule)
    {
        out << "DCS-7.2: // commenting shall be used instead of /**/ style even with multi line comments.";
    }

    out << "\n\n";
}
