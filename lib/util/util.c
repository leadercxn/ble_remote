/* Copyright (c) 2016 SENSORO Co.,Ltd. All Rights Reserved.
 *
 */

#include "util.h"


/*!
 * Redefinition of rand() and srand() standard C functions.
 * These functions are redefined in order to get the same behavior across
 * different compiler toolchains implementations.
 */
// Standard random functions redefinition start
#define RAND_LOCAL_MAX 2147483647

static unsigned long next = 1;

int rand(void)
{
    return ((next = next * 1103515245 + 12345) % RAND_LOCAL_MAX);
}

void srand(unsigned int seed)
{
    next = seed;
}
// Standard random functions redefinition end

int32_t randr(int32_t min, int32_t max)
{
    return (int32_t)rand() % (max - min + 1) + min;
}

int8_t Nibble2HexChar(uint8_t a)
{
    if(a < 10)
    {
        return '0' + a;
    }
    else if(a < 16)
    {
        return 'A' + (a - 10);
    }
    else
    {
        return '?';
    }
}

void memcpyr( uint8_t *dst, const uint8_t *src, uint16_t size )
{
    dst = dst + ( size - 1 );
    while( size-- )
    {
        *dst-- = *src++;
    }
}

bool is_buff_empty(uint8_t* p_buff, uint16_t len)
{
    for(uint16_t i = 0; i < len; i++)
    {
        if(p_buff[i] != 0xff)
        {
            return false;
        }
    }
    return true;
}

bool hex_check(const char *str)
{
    uint32_t len = strlen(str);
    uint32_t i = 0;
    bool check = false;
    for(; i < len; i++)
    {
        check = (str[i] == ':') ||
                ((str[i] >= '0')&& (str[i] <= '9')) ||
                ((str[i] >= 'a') && (str[i] <= 'f')) ||
                ((str[i] >= 'A') && (str[i] <= 'F'));
        if(!check)
        {
            return false;
        }
    }
    if(str[i-1] == ':')
    {
        return false;
    }
    return true;
}

bool hexstr_check(const char *str)
{
    uint32_t len = strlen(str);
    uint32_t i = 0;
    bool check = false;

    if(len%2 != 0)
    {
        return false;
    }

    for(; i < len; i++)
    {
        check = ((str[i] >= '0')&& (str[i] <= '9')) ||
                ((str[i] >= 'a') && (str[i] <= 'f')) ||
                ((str[i] >= 'A') && (str[i] <= 'F'));
        if(!check)
        {
            return false;
        }
    }
    return true;
}

bool integer_check(const char *str)
{
    uint32_t len = strlen(str);
    bool check = false;

    check = ((str[0] >= '0') && (str[0] <= '9')) ||
            str[0] == '-' ||
            str[0] == '+';
    if(!check)
    {
        return false;
    }

    for(uint32_t i = 1; i < len; i++)
    {
        check = (str[i] >= '0') && (str[i] <= '9');
        if(!check)
        {
            return false;
        }
    }
    return true;
}

bool uinteger_check(const char *str)
{
    uint32_t len = strlen(str);
    bool check = false;

    for(uint32_t i = 0; i < len; i++)
    {
        check = (str[i] >= '0') && (str[i] <= '9');
        if(!check)
        {
            return false;
        }
    }
    return true;
}

void uuid_decode(char* str_in, uint8_t* p_decoded_data)
{
    char* str = str_in;
    char c;
    uint8_t* p = p_decoded_data;
    while(*str != '\0')
    {
        if(*str == '-')
        {
            str++;
            continue;
        }
        sscanf(str, "%02x", (int*)&c);
        *p = c;
        p++;
        str += 2;
    }
}

uint16_t hexstr_decode(char* str_in, uint8_t* p_decoded_data)
{
    char* str = str_in;
    uint16_t index = 0;
    char c;
    uint8_t* p = p_decoded_data;
    if(strlen(str) % 2 == 0)
    {
        while(*str != '\0')
        {
            sscanf(str, "%02x", (int*)&c);
            p[index++] = c;
            str += 2;
        }
    }
    else
    {
        sscanf(str, "%1x", (int*)&c);
        p[index++] = c;
        str += 1;
        while(*str != '\0')
        {
            sscanf(str, "%02x", (int*)&c);
            p[index++] = c;
            str += 2;
        }
    }
    return index;
}

void hex_encode(uint8_t *p_in, uint16_t len_in, char *p_out)
{
    for(uint16_t i = 0; i < len_in; i++)
    {
        sprintf(&p_out[i*2], "%02X", p_in[i]);
    }
}
