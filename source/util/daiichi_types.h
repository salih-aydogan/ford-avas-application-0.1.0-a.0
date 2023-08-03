/**
 * @file    daiichi_types.h
 * @author  Daiichi SW Team
 * @brief   All type definitions in the module
 * @version 0.2
 * @date    2020-11-23
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef DAIICHI_TYPES_H
#define DAIICHI_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

// char type Compiler implementation-defined
// Only use String literal otherwise, uint8_t or int8_t
typedef char char_t;

// As Floating-point type use always double
typedef double da_double_t;
typedef double da_float_t;

// Support C99
#if (__STDC_VERSION__ >= 199901L)

/**
 * @brief Shows CPU endiannes.
 * 
 * True if little endian.
 * False if big endian.
 */
#define CPU_ENDIANNESS (*(uint8_t *)&(uint32_t){1} == 1 ? true : false)

/**
 * @brief Check if the CPU is little endian.
 * 
 * True if little endian.
 * False if big endian.
 */
#define IS_LITTLE_ENDIAN CPU_ENDIANNESS

/**
 * @brief Check if the CPU is big endian.
 * 
 * True if big endian.
 * False if little endian.
 */
#define IS_BIG_ENDIAN (*(uint8_t *)&(uint32_t){1} == 1 ? false : true)

#endif

#endif /* DA_TYPES_H */

/*** end of file ***/
