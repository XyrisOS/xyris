/**
 * @file arch.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief 
 * @version 0.1
 * @date 2020-06-01
 * 
 * @copyright Copyright Keeton Feavel et al (c) 2020
 * 
 */
#include <arch/arch.hpp>

static inline int px_arch_cpuid_vendor(int code, int str[4]) {
    __asm__ volatile ("cpuid":"=a"(*str),
                "=b"(*(str+0)),
                "=d"(*(str+1)),
                "=c"(*(str+2)):"a"(code));
    return (int)str[0];
}
 
const char* const px_cpu_get_vendor() {
	static char vendor[16] = "UNKNOWN CPU!";
	px_arch_cpuid_vendor(0, (int*)(vendor));
	return vendor;
}