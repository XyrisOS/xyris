/**
 * @file string.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Standard string and memory utility library
 * @version 0.3
 * @date 2020-06-17
 *
 * @copyright Copyright the Panix Contributors (c) 2020
 *
 */

#include <lib/string.hpp>

int strlen(const char* s) {
    int i = 0;
    while (s[i] != '\0') {
        ++i;
    }
    return i;
}

char* strcat(const char *s1, const char *s2) {
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    char *result = "";
    // = malloc(len1 + len2 + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    memcpy(result, s1, len1);
    memcpy(result + len1, s2, len2 + 1); // +1 to copy the null-terminator
    return result;
}

void strcpy(const char* source, char* destination) {
    int i = 0;
    while (source[i] != '\0') {
        destination[i] = source[i];
        ++i;
    }
    destination[i] = '\0';
}

void reverse(char* s) {
    int c;
    int j = strlen(s) - 1;
    for (int i = 0; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

void itoa(int n, char str[]) {
    int i, sign;
    if ((sign = n) < 0) n = -n;
    i = 0;
    do {
        str[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0) str[i++] = '-';
    str[i] = '\0';

    reverse(str);
}

void* memset(void* bufptr, int value, size_t size) {
	unsigned char* buf = (unsigned char*) bufptr;
	for (size_t i = 0; i < size; i++)
		buf[i] = (unsigned char) value;
	return bufptr;
}

int memcmp(const void* ptr1, const void* ptr2, size_t size) {
	const unsigned char* a = (const unsigned char*) ptr1;
	const unsigned char* b = (const unsigned char*) ptr2;
	for (size_t i = 0; i < size; i++) {
		if (a[i] < b[i])
			return -1;
		else if (a[i] > b[i])
			return 1;
	}
	return 0;
}

void* memmove(void* destination, const void* source, size_t size) {
	unsigned char* dst = (unsigned char*) destination;
	const unsigned char* src = (const unsigned char*) source;
	if (dst < src) {
		for (size_t i = 0; i < size; i++)
			dst[i] = src[i];
	} else {
		for (size_t i = size; i != 0; i--)
			dst[i-1] = src[i-1];
	}
	return destination;
}

void* memcpy(void* dstptr, const void* srcptr, size_t size) {
	unsigned char* dst = (unsigned char*) dstptr;
	const unsigned char* src = (const unsigned char*) srcptr;
	for (size_t i = 0; i < size; i++)
		dst[i] = src[i];
	return dstptr;
}