/*
 * utils.c
 *
 *  Created on: 5 Aug 2024
 *      Author: joel
 */

#include "utils.h"

// 32-bit aligned memset
void *memset32(void *addr, uint32_t pattern, size_t length) {
    // check 32-bit alignment
    /*
    if ((((uintptr_t)addr & 0x11) != 0) || ((length & 0x11) != 0)) {
        return NULL;
    }
    */
    do {
        length -= 4;
        *(uint32_t *)(addr + length) = pattern;
    } while (length > 0);
    return addr;
}

// 32-bit aligned memcpy
void *memcpy32(void *dest, const void *src, size_t length) {
    // check 32-bit alignment
    /*
    if ((((uintptr_t)dest & 0x11) != 0) || (((uintptr_t)src & 0x11) != 0) || ((length & 0x11) != 0)) {
        return NULL;
    }
    */
    do {
        length -= 4;
        *(uint32_t *)(dest + length) = *(volatile uint32_t *)(src + length);
    } while (length > 0);
    return dest;
}
