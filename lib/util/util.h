#ifndef _UTIL_H
#define _UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "compiler_abstraction.h"

/**@brief
 *
 * @param
 */
static __INLINE void print_dump(uint8_t* p_buffer, uint32_t len)
{
    for(uint32_t index = 0; index <  len; index++)
    {
        printf("%02X", p_buffer[index]);
    }
    printf("\r\n");
}

/**@brief
 *
 * @param
 */
static __INLINE void print_dump_f(uint8_t* p_buffer, uint32_t len)
{
    for(uint32_t index = 0; index <  len; index++)
    {
        printf("0x%02X, ", p_buffer[index]);
    }
    printf("\r\n");
}

/*!
 * \brief Returns 2 raised to the power of n
 *
 * \param [IN] n power value
 * \retval result of raising 2 to the power n
 */
#define POW2( n ) ( 1 << n )

/*!
 * \brief Computes a random number between min and max
 *
 * \param [IN] min range minimum value
 * \param [IN] max range maximum value
 * \retval random random value in range min..max
 */
int32_t randr(int32_t min, int32_t max);

/**@brief
 *
 * @param
 */
void srand(unsigned int seed);

/*!
 * \brief Copies size elements of src array to dst array
 *
 * \remark STM32 Standard memcpy function only works on pointers that are aligned
 *
 * \param [OUT] dst  Destination array
 * \param [IN]  src  Source array
 * \param [IN]  size Number of bytes to be copied
 */
void memcpy1(uint8_t* dst, uint8_t* src, uint16_t size);

/*!
 * \brief Set size elements of dst array with value
 *
 * \remark STM32 Standard memset function only works on pointers that are aligned
 *
 * \param [OUT] dst   Destination array
 * \param [IN]  value Default value
 * \param [IN]  size  Number of bytes to be copied
 */
void memset1(uint8_t* dst, uint8_t value, uint16_t size);

/**@brief
 *
 * @param
 */
void memcpyr( uint8_t *dst, const uint8_t *src, uint16_t size );

/*!
 * \brief Converts a nibble to an hexadecimal character
 *
 * \param [IN] a   Nibble to be converted
 * \retval hexChar Converted hexadecimal character
 */
int8_t Nibble2HexChar(uint8_t a);


/*!
 * \brief  Find First Set
 *         This function identifies the least significant index or position of the
 *         bits set to one in the word
 *
 * \param [in]  value  Value to find least significant index
 * \retval bitIndex    Index of least significat bit at one
 */
//__STATIC_INLINE uint8_t __ffs( uint32_t value )
//static __inline uint8_t __ffs( uint32_t value )
//{
//    return( uint32_t )( 32 - __CLZ( value & ( -value ) ) );
//}

/**@brief
 *
 * @param
 */
bool is_buff_empty(uint8_t* p_buff, uint16_t len);

/**@brief Validating string is hex, eg. 11:22:33:44:55
 *
 * @param
 *
 * @retval     true     String is hex
 *             false    String is not hex
 */
bool hex_check(const char *str);

/**@brief Validating string is hexstring, eg. 11223344
 *
 * @param
 *
 * @retval     true     String is hexstring
 *             false    String is not hexstring
 */
bool hexstr_check(const char *str);

/**@brief Validating string is number, eg. 1234
 *
 * @param
 *
 * @retval     true     String is number
 *             false    String is not number
 */
bool integer_check(const char *str);

bool uinteger_check(const char *str);


/**@brief Decodeing a uuid string to a buffer
 *
 * @param
 */
void uuid_decode(char* str_in, uint8_t* p_decoded_data);

/**@brief Decodeing a hex string to a buffer
 *
 * @param
 */
uint16_t hexstr_decode(char* str_in, uint8_t* p_decoded_data);

/**@brief Function for encoding a buffer to a hex string
 *
 * @param
 */
void hex_encode(uint8_t *p_in, uint16_t len_in, char *p_out);

/**@brief Function for limiting the value
 */
#define LIMIT(x, MIN, MAX)  \
{                           \
    if(x < MIN)x = MIN;     \
    if(x > MAX)x = MAX;     \
}

#ifdef _cplusplus
}
#endif

#endif
