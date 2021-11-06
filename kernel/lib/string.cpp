/**
 * @file string.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Standard string and memory utility library
 * @version 0.3
 * @date 2020-06-17
 *
 * @copyright Copyright the Xyris Contributors (c) 2020
 *
 */

#include <lib/string.hpp>

int strlen(const char* s)
{
    int i = 0;
    while (s[i] != '\0') {
        ++i;
    }
    return i;
}

char* strcat(char* dest, const char* src)
{
    const size_t len_dest = strlen(dest);
    const size_t len_src = strlen(src);
    memcpy(dest + len_dest, src, len_src + 1);
    return dest;
}

void strcpy(char* dest, const char* src)
{
    while (*src) {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
}

void strncpy(char* dest, const char* src, size_t len)
{
    while (*src && len--) {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
}

int strcmp(const char *s1, const char *s2)
{
    while (*s1)
    {
        if (*s1 != *s2)
            break;

        s1++;
        s2++;
    }

    return (int)*s1 - (int)*s2;
}

const char* strstr(const char* haystack, const char* needle)
{
    while (*haystack)
    {
        if ((*haystack == *needle) && (strcmp(haystack, needle) == 0)) {
            return haystack;
        }
        haystack++;
    }

    return NULL;
}

void reverse(char* s)
{
    int c;
    int j = strlen(s) - 1;
    for (int i = 0; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = (char)c;
    }
}

void itoa(int n, char str[])
{
    int i, sign;
    if ((sign = n) < 0)
        n = -n;
    i = 0;
    do {
        str[i++] = (char)(n % 10 + (int)'0');
    } while ((n /= 10) > 0);

    if (sign < 0)
        str[i++] = '-';
    str[i] = '\0';

    reverse(str);
}

void* memset(void* bufptr, int value, size_t size)
{
    unsigned char* buf = (unsigned char*)bufptr;
    for (size_t i = 0; i < size; i++)
        buf[i] = (unsigned char)value;
    return bufptr;
}

int memcmp(const void* ptr1, const void* ptr2, size_t num)
{
    const unsigned char* a = (const unsigned char*)ptr1;
    const unsigned char* b = (const unsigned char*)ptr2;
    for (size_t i = 0; i < num; i++) {
        if (a[i] < b[i])
            return (int)-i;
        else if (a[i] > b[i])
            return (int)i;
    }
    return 0;
}

void* memmove(void* destptr, const void* srcptr, size_t size)
{
    unsigned char* dst = (unsigned char*)destptr;
    const unsigned char* src = (const unsigned char*)srcptr;
    if (dst < src) {
        for (size_t i = 0; i < size; i++)
            dst[i] = src[i];
    } else {
        for (size_t i = size; i != 0; i--)
            dst[i - 1] = src[i - 1];
    }
    return destptr;
}

void* memcpy(void* dstptr, const void* srcptr, size_t size)
{
    unsigned char* dst = (unsigned char*)dstptr;
    const unsigned char* src = (const unsigned char*)srcptr;
    for (size_t i = 0; i < size; i++)
        dst[i] = src[i];
    return dstptr;
}
