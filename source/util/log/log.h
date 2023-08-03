/*PRQA S ALL EOF*/

/**
 * @file    log.h
 * @author  Enes AYDIN (enes.aydin@daiichi.com)
 * @brief   Interface of the Log component.
 * @version 0.1
 * @date    2021-02-25
 * 
 * @copyright Copyright (c) 2021
 * 
 * This interface has been developed according to the log architecture design
 * 
 *      https://codebeamer.daiichi.com/issue/14454
 *
 * Example Usage:
 * 
 * @code
 *    
 *    LOG_VERBOSE("Deneme");               // writes '[...]Deneme\r\n'
 *    LOG_DEBUG("%% %c", a);               // writes '[...]% a\r\n'
 *    LOG_WARNING("%6.3d", 13);            // writes '[...]   013\r\n'
 *    LOG_INFO("%*.s", 5, "EnEs13");       // writes '[...]   En\r\n'
 *    LOG_FATAL("%.*s", 4, "EnEs13");      // writes '[...]EnEs\r\n'
 *    LOG_ERROR("%d", 13);                 // writes '[...]13\r\n'
 *    LOG_DEBUG("%d", -13);                // writes '[...]-13\r\n'
 *    LOG_ERROR("% d", -13);               // writes '[...]-13\r\n'
 *    LOG_DEBUG("%d", 13);                 // writes '[...] 13\r\n'
 *    LOG_VERBOSE("%u", 13U);              // writes '[...]13\r\n'
 *    LOG_WARNING("%05d", 13);             // writes '[...]00013\r\n'
 *    LOG_DEBUG("%05d", 13);               // writes '[...]+0013\r\n'
 *    LOG_INFO("%x", 131313);              // writes '[...]200f1\r\n'
 *    LOG_INFO("%6b", 13);                 // writes '[...]  1101\r\n'
 *    LOG_FATAL("%p", &val);               // writes '[...]0064fe18\r\n'
 * 
 * @endcode
 */

#ifndef LOG_H
#define LOG_H

/*******************************************************************************
 * Includes 
 ******************************************************************************/

#include "daiichi_types.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/14462
 * 
 */
#define LOG_LEVEL_NONE          0u

/**
 * @brief https://codebeamer.daiichi.com/issue/14463
 * 
 */
#define LOG_LEVEL_VERBOSE       1u

/**
 * @brief https://codebeamer.daiichi.com/issue/14464
 * 
 */
#define LOG_LEVEL_DEBUG         2u

/**
 * @brief https://codebeamer.daiichi.com/issue/14465
 * 
 */
#define LOG_LEVEL_INFO          3u

/**
 * @brief https://codebeamer.daiichi.com/issue/14466
 * 
 */
#define LOG_LEVEL_WARNING       4u

/**
 * @brief https://codebeamer.daiichi.com/issue/14467
 * 
 */
#define LOG_LEVEL_ERROR         5u

/**
 * @brief https://codebeamer.daiichi.com/issue/14468
 * 
 */
#define LOG_LEVEL_FATAL         6u

/**
 * @brief https://codebeamer.daiichi.com/issue/14469
 * 
 */
#define LOG_LEVEL               LOG_LEVEL_INFO

/**
 * @brief https://codebeamer.daiichi.com/issue/14472
 * 
 */
#define LOG_POOL_SIZE           (uint32_t)4096u

/**
 * @brief https://codebeamer.daiichi.com/issue/14473
 * 
 */
#define LOG_FRAME_MAX_SIZE      256u

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Initialize the LOG module, clears the internal LOG buffer.
 * 
 */
void log_init(void);

/**
 * @brief the LOG module Process.
 * 
 * All LOGs are buffered and When the buffer is full, 
 * logs are sent to the output by the LOG process
 * 
 */
void log_process(void);

/**
 * @brief Sets the minimum log level to be printed.
 * 
 * https://codebeamer.daiichi.com/issue/14480
 * 
 * @param[in] level Log level to be printed.
 */
void log_set_level(uint8_t level);

/**
 * @brief Return the minimum log level to be printed.
 * 
 * https://codebeamer.daiichi.com/issue/14481
 * 
 * @return NONE The log is disabled.
 * @return VERBOSE Print all.
 * @return DEBUG Print FATAL, ERROR, WARNING, INFO, and DEBUG logs.
 * @return INFO Print FATAL, ERROR, WARNING, and INFO logs.
 * @return WARNING Print FATAL, ERROR, and WARNING logs.
 * @return ERROR Print FATAL and ERROR logs.
 * @return FATAL Print only FATAL logs.
 * 
 */
uint8_t log_get_level(void);

/**
 * @brief This variadic function writes the log frame to the memory pool or 
 *        directly to the output depending on the p_buffer parameter.
 * 
 * https://codebeamer.daiichi.com/issue/14482
 * 
 * @param[in] b_buffer To choose memory pool or output.
 * @param[in] level Log level.
 * @param[in] p_module Pointer to the module name string.
 * @param[in] line The number of rows where the function is called.
 * @param[in] p_str Pointer to printf style format string.
 * @param[in] variadic Format specifier according to the variadic params.
 */
void log_print(bool b_buffer,
               uint8_t level,
               const char *p_module,
               uint32_t line,
               const char *p_str,
               ...);

/**
 * @brief This function writes the log communication frame to the memory pool.
 * 
 * https://codebeamer.daiichi.com/issue/14483
 * 
 * @param[in] p_direction "RX" or "TX" as a string.
 * @param[in] p_module Pointer to the module name string.
 * @param[in] line The number of rows where the function is called.
 * @param[in] p_buf Pointer to the log data buffer.
 * @param[in] size Size of the log data buffer.
 */
void log_comm(const char *p_direction,
              const char *p_module,
              uint32_t line,
              const uint8_t *p_buf,
              uint32_t size);

/*******************************************************************************
 * Public Function Macros
 ******************************************************************************/

/**
 * @brief This function-like macro is used to register.
 * 
 * https://codebeamer.daiichi.com/issue/14474
 * 
 */
#define LOG_MODULE_NAME(name) static const char *mp_log_md_name = (name)

/**
 * @brief This function-like macro stores the log frame in memory pool to ben 
 *        sent to the output when the Log task runs. ##__VA_ARGS__ is GCC
 *        extension. A standard alternative to this trick is complex an limited. 
 *        There is no complete standard alternative.
 * 
 * https://codebeamer.daiichi.com/issue/14475
 * 
 */
#define LOG(level, payload, ...) log_print(true,                    \
                                           level,                   \
                                           mp_log_md_name,          \
                                           (uint32_t)__LINE__,      \
                                           payload,                 \
                                           ##__VA_ARGS__)

/**
 * @brief This function-like macro sends the log frame to output synchronously. 
 *        ##__VA_ARGS__ is GCC extension. A standard alternative to this trick 
 *        is complex and limited. There is no complete standard alternative.
 * 
 * https://codebeamer.daiichi.com/issue/14476
 * 
 */
#define LOG_NOW(level, payload, ...)  log_print(false,              \
                                                level,              \
                                                mp_log_md_name,     \
                                                (uint32_t)__LINE__, \
                                                payload,            \
                                                ##__VA_ARGS__)

/**
 * @brief This function-like macro stores the log communication frame in memory 
 *        pool to ben sent to the output when the Log task runs. 
 * 
 * https://codebeamer.daiichi.com/issue/14477
 * 
 */
#define LOG_RX(p_buf, buf_size)     log_comm("RX",                  \
                                             mp_log_md_name,        \
                                             (uint32_t)__LINE__,    \
                                             p_buf,                 \
                                             buf_size)

/**
 * @brief This function-like macro stores the log communication frame in memory 
 *        pool to ben sent to the output when the Log task runs. 
 * 
 * https://codebeamer.daiichi.com/issue/14478
 * 
 */
#define LOG_TX(p_buf, buf_size)     log_comm("TX",                  \
                                             mp_log_md_name,        \
                                             (uint32_t)__LINE__,    \
                                             p_buf,                 \
                                             buf_size)

#define LOG_VERBOSE(payload, ...)   log_print(true,                 \
                                              LOG_LEVEL_VERBOSE,    \
                                              mp_log_md_name,       \
                                              (uint32_t)__LINE__,   \
                                              payload,              \
                                              ##__VA_ARGS__)

#define LOG_DEBUG(payload, ...)     log_print(true,                 \
                                              LOG_LEVEL_DEBUG,      \
                                              mp_log_md_name,       \
                                              (uint32_t)__LINE__,   \
                                              payload,              \
                                              ##__VA_ARGS__)

#define LOG_INFO(payload, ...)      log_print(true,                 \
                                              LOG_LEVEL_INFO,       \
                                              mp_log_md_name,       \
                                              (uint32_t)__LINE__,   \
                                              payload,              \
                                              ##__VA_ARGS__)

#define LOG_WARNING(payload, ...)   log_print(true,                 \
                                              LOG_LEVEL_WARNING,    \
                                              mp_log_md_name,       \
                                              (uint32_t)__LINE__,   \
                                              payload,              \
                                              ##__VA_ARGS__)

#define LOG_ERROR(payload, ...)     log_print(true,                 \
                                              LOG_LEVEL_ERROR,      \
                                              mp_log_md_name,       \
                                              (uint32_t)__LINE__,   \
                                              payload,              \
                                              ##__VA_ARGS__)

#define LOG_FATAL(payload, ...)     log_print(true,                 \
                                              LOG_LEVEL_FATAL,      \
                                              mp_log_md_name,       \
                                              (uint32_t)__LINE__,   \
                                              payload,              \
                                              ##__VA_ARGS__)

#endif  /* LOG_H */

// end of file
