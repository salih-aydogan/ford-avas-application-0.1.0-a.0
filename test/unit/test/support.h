/**
 * @file support.h
 * @author Oğuzhan MUTLU (oguzhan.mutlu@daiichi.com)
 * @brief 
 * @version 0.1
 * @date 2021-03-02
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef __SUPPORT_H
#define __SUPPORT_H

#include <stdio.h>

#define TEST_STEP(x)    printf("\nTEST STEP:%d:", x)
#define TEST_CASE()       printf("\nTEST CASE:%s:", __func__)

#endif //__SUPPORT_H
