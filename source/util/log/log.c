/*PRQA S ALL EOF*/

/**
 * @file    log.c
 * @author  Enes AYDIN (enes.aydin@daiichi.com)
 * @brief   Implementation of the Log component.
 * @version 0.1
 * @date    2021-02-25
 * 
 * @copyright Copyright (c) 2021
 * 
 * This module has been developed according to the log detailed design.
 * 
 *      https://codebeamer.daiichi.com/issue/26636
 */

#include "bsp_uart.h"
#include "bsp_mcu.h"

#include "log.h"

#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

/*******************************************************************************
 * Internal Defines
 ******************************************************************************/

/**
 * @brief If the flag used before specifiers is '0', this macro is used for the 
 *        flags variable.
 * 
 */
#define FLAGS_ZEROPAD       1u

/**
 * @brief If the flag used before specifiers is '+', this macro is used for the 
 *        flags variable.
 * 
 */
#define FLAGS_PLUS          2u

/**
 * @brief If the flag used before specifiers is ' ', this macro is used for the 
 *        flags variable.
 * 
 */
#define FLAGS_SPACE         4u

/**
 * @brief If precis is used before specifiers, this macro is used for flags 
 *        variable.
 * 
 */
#define FLAGS_PRECISION     8u

/*******************************************************************************
 * Internal Forward Declerations
 ******************************************************************************/

/**
 * @brief Infos for format of parsing in vstring_nprintf
 * 
 */
typedef struct frmt_infos_s frmt_infos_t;

/*******************************************************************************
 * Internal Type Definitions
 ******************************************************************************/

/**
 * @brief Enum to hold state information.
 * 
 */
typedef enum frmt_state_e {
    FRMT_BEGIN,
    FRMT_FLAGS,
    FRMT_WIDTH,
    FRMT_PREC,
    FRMT_SPEC,
    FRMT_FINISH
} frmt_state_t;

/**
 * @brief Struct to hold format information.
 * 
 */
struct frmt_infos_s {
    uint32_t flags;
    uint32_t width;
    uint32_t precision;
};
/*******************************************************************************
 * Local Variable Definitions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/29913
 * 
 */
static uint32_t m_std_idx = 0U;

/**
 * @brief https://codebeamer.daiichi.com/issue/29964
 * 
 */
static char m_std_pool[LOG_POOL_SIZE] = {0};

/**
 * @brief https://codebeamer.daiichi.com/issue/29966
 * 
 */
static uint8_t m_log_level = LOG_LEVEL;

/**
 * @brief https://codebeamer.daiichi.com/issue/29967
 * 
 */
static const char *mp_level_names[] = {"", "VRB", "DBG", "INF", "WRN", "ERR", "FTL"};

/**
 * @brief https://codebeamer.daiichi.com/issue/32747
 * 
 */
static const char *mp_uint8_t_to_hex[256] = {
"00","01","02","03","04","05","06","07","08","09","0A","0B","0C","0D","0E","0F",
"10","11","12","13","14","15","16","17","18","19","1A","1B","1C","1D","1E","1F",
"20","21","22","23","24","25","26","27","28","29","2A","2B","2C","2D","2E","2F",
"30","31","32","33","34","35","36","37","38","39","3A","3B","3C","3D","3E","3F",
"40","41","42","43","44","45","46","47","48","49","4A","4B","4C","4D","4E","4F",
"50","51","52","53","54","55","56","57","58","59","5A","5B","5C","5D","5E","5F",
"60","61","62","63","64","65","66","67","68","69","6A","6B","6C","6D","6E","6F",
"70","71","72","73","74","75","76","77","78","79","7A","7B","7C","7D","7E","7F",
"80","81","82","83","84","85","86","87","88","89","8A","8B","8C","8D","8E","8F",
"90","91","92","93","94","95","96","97","98","99","9A","9B","9C","9D","9E","9F",
"A0","A1","A2","A3","A4","A5","A6","A7","A8","A9","AA","AB","AC","AD","AE","AF",
"B0","B1","B2","B3","B4","B5","B6","B7","B8","B9","BA","BB","BC","BD","BE","BF",
"C0","C1","C2","C3","C4","C5","C6","C7","C8","C9","CA","CB","CC","CD","CE","CF",
"D0","D1","D2","D3","D4","D5","D6","D7","D8","D9","DA","DB","DC","DD","DE","DF",
"E0","E1","E2","E3","E4","E5","E6","E7","E8","E9","EA","EB","EC","ED","EE","EF",
"F0","F1","F2","F3","F4","F5","F6","F7","F8","F9","FA","FB","FC","FD","FE","FF"
};

/*******************************************************************************
 * static Functions
 ******************************************************************************/

/**
 * @brief https://codebeamer.daiichi.com/issue/29915
 * 
 */
static uint32_t vstring_nprintf(char *p_str, 
                                uint32_t max_len, 
                                const char *p_frmt, 
                                va_list vargs);

/**
 * @brief https://codebeamer.daiichi.com/issue/29969
 * 
 */
static uint32_t string_nprintf(char *p_str, 
                               uint32_t max_len, 
                               const char *p_frmt, 
                               ...);

/*******************************************************************************
 * Internal static Functions
 ******************************************************************************/

/**
 * @brief Writes the frame data to pool.
 * 
 * @param[in] frame The address of frame data.
 * @param[in] len   The length of frame data.
 */
static void write_std_pool_safety(const char *frame, uint32_t len);

/**
 * @brief Writes character to string buffer and index of string increases
 *        If index is equal to max_len, writing does not occur. 
 * 
 * @param[out] p_str The address of string buffer.
 * @param[out] p_idx The index of string buffer.
 * @param[in] max_len The boundary of the string.
 * @param[in] ch The character to write in string.
 * 
 * @return frmt_state_t The state of parsing.
 * 
 */
static frmt_state_t write_to_string(char *p_str, 
                                    uint32_t *p_idx, 
                                    uint32_t max_len, 
                                    char ch);

/**
 * @brief Writes spaces to string buffer and index of string increases.
 * 
 * @param[out] p_str The address of string buffer.
 * @param[out] p_idx The index of string buffer.
 * @param[in] len It holds how many space characters are to be written. 
 *                If it is 0 and less than 0, writing does not occur.
 */
static void fill_space(char *p_str, uint32_t *p_idx, int32_t len);

/**
 * @brief Writes the reverse of the string to the incoming string.
 * 
 * @param[out] p_str The address of string buffer.
 */
static void str_reverse(char *p_str);

/**
 * @brief Gets the number for the field from the varadic parameter or format 
 *        string.
 * 
 * @param[in] pp_iter The address of the pointer indicating the relevant location 
 *                    of the format string.
 * @param[out] vargs The address of ariadic argument parameter.
 * 
 * @return uint32_t number indicating how much space will be.
 * 
 */
static uint32_t get_area(const char **pp_iter, va_list *vargs);

/**
 * @brief Deletes the zeropad value if it has the precision value.
 * 
 * @param[in] flags The value for output formatting.
 * 
 * @return uint32_t The current version of the flag returns.
 */
static uint32_t if_there_prec_remove_zeropad(uint32_t flags);

/**
 * @brief Returns the base number by character.
 * 
 * @param[in] ch One of the values x, b d or u.
 * 
 * @return uint32_t The value of 16, 10 or 2 returns.
 * 
 */
static uint32_t get_base(char ch);

/**
 * @brief The length of the incoming string is updated according to the 
 *        precision and flag values
 * 
 * @param[in] p_str The address of string buffer.
 * @param[in] flags The value for output formatting.
 * @param[in] precision The another value for output formatting.
 * 
 * @return uint32_t The value of 16, 10 or 2 returns.
 */
static uint32_t set_len(const char *p_str, 
                         uint32_t flags, 
                         uint32_t precision);

/**
 * @brief Copies string to output string. It checks certain values while 
 *        copying.
 * 
 * @param[out] p_str The address of string buffer.
 * @param[out] p_idx The index of string buffer.
 * @param[in] max_len The boundary of the string.
 * @param[in] flags The value for output formatting.
 * @param[in] precision The another value for output formatting.
 * @param[in] p_source_str The address of the string to copy.
 * 
 * @return frmt_state_t The state of parsing.
 * 
 */
static frmt_state_t copy_string(char *p_str, 
                                 uint32_t *p_idx, 
                                 uint32_t max_len, 
                                 uint32_t flags, 
                                 uint32_t precision, 
                                 const char *p_source_str);

/**
 * @brief Writes a uint32 type number to the given string address with checks.
 * 
 * @param[out] p_str The address of string buffer.
 * @param[out] p_idx The index of string buffer.
 * @param[in] max_len The boundary of the string.
 * @param[in] value The number to be converted.
 * @param[in] b_negative The sign of the number.
 * @param[in] base The base of the number.
 * @param[in] flags The value for output formatting.
 * @param[in] precision The another value for output formatting.
 * 
 * @return frmt_state_t The state of parsing.
 * 
 */
static frmt_state_t uint32_t_to_str(char *p_str, 
                                    uint32_t *p_idx, 
                                    uint32_t max_len, 
                                    uint32_t value, 
                                    bool b_negative, 
                                    uint32_t base, 
                                    frmt_infos_t *info);

/**
 * @brief Writes a double type number to the given string address with 
 * checks.
 * 
 * @param[out] p_str The address of string buffer.
 * @param[out] p_idx The index of string buffer.
 * @param[in] max_len The boundary of the string.
 * @param[in] value The number to be converted.
 * @param[in] flags The value for output formatting.
 * @param[in] precision The another value for output formatting.
 * 
 * @return frmt_state_t The state of parsing.
 * 
 */
static frmt_state_t double_to_str(char *p_str, 
                                  uint32_t *p_idx, 
                                  uint32_t max_len, 
                                  double value, 
                                  frmt_infos_t *info);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

void log_init(void)
{
    // m_std_pool will not be cleared here since there may be logs before init
    
    // Placeholder
}

/**
 * @brief https://codebeamer.daiichi.com/issue/29995
 * 
 */
void log_set_level(uint8_t level)
{
    m_log_level = level;

    // TODO: CALL Write LOG LEVEL severity to EEPROM
}

/**
 * @brief https://codebeamer.daiichi.com/issue/29996
 * 
 */
uint8_t log_get_level(void)
{
    return m_log_level;
}

/**
 * @brief https://codebeamer.daiichi.com/issue/29997
 * 
 */
void log_print(bool b_buffer, 
               uint8_t level, 
               const char *p_module, 
               uint32_t line, 
               const char *p_str, 
               ...)
{
    if ((m_log_level > level) || (LOG_LEVEL_NONE == m_log_level))
    {
        return;
    }
    
    va_list args;
    va_start(args, p_str);
    char frame[LOG_FRAME_MAX_SIZE];

    uint32_t sys_tick = bsp_mcu_tick_get();

    /*
     * Head of frame format
     */
    uint32_t len = string_nprintf(frame, 
                                  LOG_FRAME_MAX_SIZE - 2u, 
                                  "[%u][%s][%s:%u]", 
                                  sys_tick, 
                                  mp_level_names[level], 
                                  p_module, 
                                  line);
    /*
     * Payload part of frame format
     */
    len += vstring_nprintf(&frame[len], 
                           LOG_FRAME_MAX_SIZE - 2u - len, 
                           p_str, 
                           args);

    len += string_nprintf(&frame[len], 2u, "\r\n");

    if (true == b_buffer)
    {
        /*
         * Write frame to the pool
         */
        write_std_pool_safety(frame, len);
    }
    else
    {
        bsp_uart_write((const uint8_t*)frame, (size_t)len);
    }
}

/**
 * @brief https://codebeamer.daiichi.com/issue/29998
 * 
 */
void log_comm(const char *p_direction, 
              const char *p_module, 
              uint32_t line, 
              const uint8_t *p_buf, 
              uint32_t size)
{
    if (LOG_LEVEL_VERBOSE == m_log_level)
    {
        char frame[LOG_FRAME_MAX_SIZE];

        uint32_t sys_tick = bsp_mcu_tick_get();

        /*
         * Head of frame format
         */
        uint32_t len = string_nprintf(frame, 
                                      LOG_FRAME_MAX_SIZE - 2u, 
                                      "[%u][VRB][%s:%u][%s]", 
                                      sys_tick,
                                      p_module, 
                                      line, 
                                      p_direction);
        /*
         * Each byte will be written in hexadecimal and there will be a space 
         * between bytes. Therefore, every byte except the last byte will 
         * occupy 3 bytes. So,
         * 3 * size - 1 + len + 2
         */
        uint32_t copy_data_len = (LOG_FRAME_MAX_SIZE < (3u * size + len + 1u)) 
                                 ? (LOG_FRAME_MAX_SIZE - len - 2u) / 3U
                                 : (size);

        /*
         * Payload of frame format
         */
        for (uint32_t idx = 0u; idx < copy_data_len; ++idx)
        {
            len += string_nprintf(&frame[len], 
                                  3u, 
                                  "%s ", 
                                  mp_uint8_t_to_hex[p_buf[idx]]);
        }

        --len; // to overwrite the last character (because it is space)
        len += string_nprintf(&frame[len], 2u, "\r\n");

        /*
         * Write frame to the pool
         */
        write_std_pool_safety(frame, len);
    }
}

void log_process(void)
{
    if (0U == m_std_idx)
    {
        return;
    }

    bool ret = bsp_uart_async_write((const uint8_t*)m_std_pool, 
                                    (size_t)m_std_idx);

    if(true == ret)
    {
        m_std_idx = 0U;
    }
}

/*******************************************************************************
 * static Functions 
 ******************************************************************************/

static uint32_t vstring_nprintf(char *p_str, 
                                uint32_t max_len, 
                                const char *p_frmt, 
                                va_list vargs)
{
    bool b_exit = false;
    const char *iter = p_frmt;
    frmt_state_t state = FRMT_BEGIN;
    uint32_t idx = 0u, flags = 0u, width = 0u, precision = 0u;
    
    while ((0 != *iter) && (true != b_exit))
    {
        switch (state)
        {
        case FRMT_BEGIN:
        {
            if ('%' != *iter)
            {
                state = write_to_string(p_str, &idx, max_len, *iter);
            }
            else
            {
                flags = 0u;
                state = FRMT_FLAGS;
            }
            iter++;
        }
        break;

        case FRMT_FLAGS:
        {    
            if ('0' == *iter)
            {
                iter++;
                flags |= FLAGS_ZEROPAD;
            }
            else if ('+' == *iter)
            {
                iter++;
                flags |= FLAGS_PLUS;
            }
            else if (' ' == *iter)
            {
                iter++;
                flags |= FLAGS_SPACE;
            }
            else
            {
                state = FRMT_WIDTH;
            }
        }
        break;

        case FRMT_WIDTH:
        {
            width = get_area(&iter, &vargs);
            state = FRMT_PREC;
        }
        break;

        case FRMT_PREC:
        {
            precision = 0u;
            
            if ('.' == *iter)
            {
                iter++;
                flags |= FLAGS_PRECISION;
                precision = get_area(&iter, &vargs);
            }
            state = FRMT_SPEC;
        }
        break;

        case FRMT_SPEC:
        {
            if ('d' == *iter)
            {
                const int32_t value = va_arg(vargs, int32_t);
                flags = if_there_prec_remove_zeropad(flags);
                
                state = uint32_t_to_str(p_str, 
                                        &idx, 
                                        max_len, 
                                        (uint32_t)abs(value),
                                        (0 > value), 
                                        10u, 
                                        &((frmt_infos_t){
                                            flags, 
                                            width, 
                                            precision
                                        }));
            }
            else if (('b' == *iter) || ('u' == *iter) || ('x' == *iter))
            {
                uint32_t base = get_base(*iter);
                                
                flags &= ~(FLAGS_PLUS | FLAGS_SPACE);
                flags = if_there_prec_remove_zeropad(flags);
                                
                const uint32_t value = va_arg(vargs, uint32_t);
                
                state = uint32_t_to_str(p_str, 
                                        &idx, 
                                        max_len,
                                        value, 
                                        false, 
                                        base, 
                                        &((frmt_infos_t){
                                            flags, 
                                            width, 
                                            precision
                                        }));
            }
            else if ('f' == *iter)
            {
                state = double_to_str(p_str, 
                                     &idx, 
                                     max_len, 
                                     va_arg(vargs, double), 
                                     &((frmt_infos_t){
                                         flags, 
                                         width, 
                                         precision
                                     }));
            }
            else if ('p' == *iter)
            {
                width = sizeof(void *) * 2u;
                flags |= FLAGS_ZEROPAD;
                state = uint32_t_to_str(p_str, 
                               &idx, 
                               max_len, 
                               (uint32_t)((uint32_t *)va_arg(vargs, void*)), 
                               false, 
                               16u, 
                               &((frmt_infos_t){
                                   flags, 
                                   width, 
                                   precision
                               }));
            }
            else if ('c' == *iter)
            {
                fill_space(p_str, &idx, (int32_t)width - 1);
                state = write_to_string(p_str, 
                                        &idx, 
                                        max_len, 
                                        (char)va_arg(vargs, int));
            }
            else if ('s' == *iter)
            {
                const char* p = va_arg(vargs, char*);
                uint32_t str_len = set_len(p, flags, precision);
                                    
                fill_space(p_str, &idx, (int32_t)width - (int32_t)str_len);
                
                state = copy_string(p_str, 
                                    &idx, 
                                    max_len, 
                                    flags, 
                                    precision, 
                                    p);
            }
            else
            {
                state = write_to_string(p_str, &idx, max_len, *iter);
            }
                                
            iter++;
        }
        break;

        case FRMT_FINISH:
        {
            // Do nothing
        }
        
        default:
        {
            b_exit = 1u;
        }
        break;
        }
    }

    return idx;
}

static uint32_t string_nprintf(char *p_str, 
                               uint32_t max_len, 
                               const char *p_frmt, 
                               ...)
{
    va_list args;
    
    va_start(args, p_frmt);
    uint32_t ret = vstring_nprintf(p_str, max_len, p_frmt, args);
    va_end(args);
    
    return ret;
}

static void write_std_pool_safety(const char *frame, uint32_t len)
{
    if (LOG_POOL_SIZE <= (m_std_idx + len))
    {
        log_print(false, LOG_LEVEL_FATAL, "LOG", __LINE__, "LOG POOL IS FULL");

        log_process();
    }

    if (LOG_POOL_SIZE > (m_std_idx + len))
    {
        m_std_idx += string_nprintf(&m_std_pool[m_std_idx], len, frame);
    }
}

static frmt_state_t write_to_string(char *p_str, 
                                    uint32_t *p_idx, 
                                    uint32_t max_len, 
                                    char ch)
{
	if (*p_idx >= max_len)
	{
		return FRMT_FINISH;
	}
	
	p_str[(*p_idx)++] = ch;
	
	return FRMT_BEGIN;
}

static void fill_space(char *p_str, uint32_t *p_idx, int32_t len)
{
	for (int32_t cnt = 0; cnt < len; cnt++)
	{
		p_str[(*p_idx)++] = ' ';
	}
}

static void str_reverse(char *p_str)
{
    char *p1, *p2;

    if (!p_str || !*p_str)
    {
        return;
    }

    for (p1 = p_str, p2 = p_str + strlen(p_str) - 1; p2 > p1; ++p1, --p2)
    {
        *p1 ^= *p2;
        *p2 ^= *p1;
        *p1 ^= *p2;
    }
}

static uint32_t get_area(const char **pp_iter, va_list *vargs)
{
	uint32_t area;
	
    if (0 != isdigit((int)**pp_iter))
	{
		area = (uint32_t) atoi(*pp_iter);
		while (isdigit((int)**pp_iter) && ++*pp_iter);
	}
    else if ('*' == **pp_iter)
	{
		const int32_t wid = va_arg(*vargs, int32_t);
		area = 0 < wid ? (uint32_t) wid : 0u;
		++*pp_iter;
	}
    else
    {
        area = 0u;
    }
	
	return area;
}

static uint32_t if_there_prec_remove_zeropad(uint32_t flags)
{
	if (FLAGS_PRECISION & flags)
	{
		return (flags & ~FLAGS_ZEROPAD);
	}
	
	return flags;
}

static uint32_t get_base(char ch)
{
    if ('x' == ch)
	{
    	return 16u;
    }
    
	if ('b' == ch)
	{
    	return 2u;
    }
	
	return 10u;
}

static uint32_t set_len(const char *p_str, 
                        uint32_t flags, 
                        uint32_t precision)
{
    if ((0u == precision) && (flags & FLAGS_PRECISION))
    {
        return 0u;
    }
    
    uint32_t len = (uint32_t) strlen(p_str);
	
	if ((0u < precision) && (len > precision))
    {
        return precision;
	}
	
	return len;
}

static frmt_state_t copy_string(char *p_str, 
                                uint32_t *p_idx, 
                                uint32_t max_len, 
                                uint32_t flags, 
                                uint32_t precision, 
                                const char *p_source_str)
{
	frmt_state_t state = FRMT_BEGIN;
	
	while ((*p_source_str != 0) && (!(flags & FLAGS_PRECISION) || precision--))
	{
		state = write_to_string(p_str, p_idx, max_len, *p_source_str++);
		
		if (FRMT_FINISH == state)
		{
			break;
		}
	}
	
	return state;
}

static frmt_state_t uint32_t_to_str(char *p_str, 
                                    uint32_t *p_idx, 
                                    uint32_t max_len, 
                                    uint32_t value, 
                                    bool b_negative, 
                                    uint32_t base, 
                                    frmt_infos_t *info)
{
	uint32_t len = 0u;
	char buf[32] = {0};
	uint32_t val = value;
    uint32_t flags = info->flags;
    uint32_t width = info->width;
    uint32_t precision = info->precision;
	
	do {
		const char digit = (char)(val % base);
		buf[len++] = (char)((10 > digit) ? ('0' + digit) : ('a' + digit - 10));
		val /= base;
	} while (0u != val);
	
	if ((0 != width) && 
        (flags & FLAGS_ZEROPAD) && 
        ((true == b_negative) || (flags & (FLAGS_PLUS | FLAGS_SPACE))))
	{
		width--;
    }
    
    while (len < precision)
	{
		buf[len++] = '0';
    }
    
    while ((flags & FLAGS_ZEROPAD) && (len < width))
	{
		buf[len++] = '0';
    }
    
    if (true == b_negative)
	{
		buf[len++] = '-';
	}
    else if (flags & FLAGS_PLUS)
	{
		buf[len++] = '+';
	}
    else if (flags & FLAGS_SPACE)
	{
		buf[len++] = ' ';
    }
    
	if (!(flags & FLAGS_ZEROPAD))
	{
		fill_space(p_str, p_idx, (int32_t)width - (int32_t)len);
	}
	
    str_reverse(buf);
    frmt_state_t state = copy_string(p_str, p_idx, max_len, flags, precision, buf);
	
	return state;
}

static frmt_state_t double_to_str(char *p_str, 
                                  uint32_t *p_idx, 
                                  uint32_t max_len, 
                                  double value, 
                                  frmt_infos_t *info)
{
	char buf[32] = {0};
	uint32_t len  = 0u;
	bool negative = false;
  	static const double pow10[] = {1, 
                                   10, 
                                   100, 
                                   1000, 
                                   10000, 
                                   100000, 
                                   1000000, 
                                   10000000,
                                   100000000, 
                                   1000000000};
    uint32_t flags = info->flags;
    uint32_t width = info->width;
    uint32_t precision = info->precision;
    
    
	if (value < 0)
	{
		negative = true;
		value = fabs(value);
	}

    if (!(flags & FLAGS_PRECISION))
    {
        precision = 6;
    }
    
    while ((len < 32u) && (precision > 9u))
    {
        buf[len++] = '0';
        precision--;
    }

    int32_t whole = (int32_t)value;
    double tmp = (value - whole) * pow10[precision];
    uint32_t frac = (uint32_t)tmp;
    double diff = tmp - frac;

    if (diff > 0.5)
    {
        ++frac;
        // handle rollover, e.g. case 0.99 with prec 1 is 1.0
        if (frac >= pow10[precision])
        {
            frac = 0;
            ++whole;
        }
    }
    else if ((diff == 0.5) && ((frac == 0u) || (frac & 1u)))
    {
        // if halfway, round up if odd OR if last digit is 0
        ++frac;
    }

    if (precision == 0u)
    {
        diff = value - (double)whole;

        if ((!(diff < 0.5) || (diff > 0.5)) && (whole & 1))
        {
            // exactly 0.5 and ODD, then round up
            // 1.5 -> 2, but 2.5 -> 2
            ++whole;
        }
    }
    else
    {
        uint32_t count = precision;
        // now do fractional part, as an unsigned number
        while (len < 32u)
        {
            --count;
            buf[len++] = (char)(48u + (frac % 10u));
            
            if (!(frac /= 10u))
            {
                break;
            }
        }
        
        // add extra 0s
        while ((len < 32u) && (count-- > 0u))
        {
            buf[len++] = '0';
        }

        if (len < 32u)
        {
            // add decimal
            buf[len++] = '.';
        }
    }

    do
    {
        buf[len++] = (char)(48 + (whole % 10));
        whole /= 10;
    } while (0 != whole);

    if (flags & FLAGS_ZEROPAD)
    {
        if (width && (negative || (flags & (FLAGS_PLUS | FLAGS_SPACE))))
        {
            width--;
        }

        while (len < width) 
        {
            buf[len++] = '0';
        }
    }

    if (true == negative)
    {
        buf[len++] = '-';
    }
    else if (flags & FLAGS_PLUS)
    {
        buf[len++] = '+';  // ignore the space if the '+' exists
    }
    else if (flags & FLAGS_SPACE)
    {
        buf[len++] = ' ';
    }

	if (!(flags & FLAGS_ZEROPAD))
	{
		fill_space(p_str, p_idx, (int32_t)width - (int32_t)len);
	}
	
    str_reverse(buf);
    frmt_state_t state = copy_string(p_str, p_idx, max_len, flags, len, buf);

	return state;
}

// end of file
