/*
 * utils.c
 *
 *  Created on: 5 Aug 2024
 *      Author: joel
 */

#include "utils.h"

// 32-bit aligned memset with fallback to compiler's memset
void *memset32(void *addr, uint32_t pattern, size_t length) {
    // check 32-bit alignment
    if (((uintptr_t)addr & 0x11) != 0) {
        return memset(addr, pattern, length);
    }
    if ((length & 0x11) != 0) {
        uint8_t remain = length & 0x11;
        length &= ~(0x11);
        if (memset(addr + length, pattern, remain) == NULL)
            return NULL;
    }
    if (length == 0)
        return addr;
    do {
        length -= 4;
        *(uint32_t *)(addr + length) = pattern;
    } while (length > 0);
    return addr;
}

// 32-bit aligned memcpy with fallback to compiler's memcpy
void *memcpy32(void *dest, const void *src, size_t length) {
    // check 32-bit alignment
    if ((((uintptr_t)dest & 0x11) != 0) || (((uintptr_t)src & 0x11) != 0)) {
        return memcpy(dest, src, length);
    }
    if ((length & 0x11) != 0) {
        uint8_t remain = length & 0x11;
        length &= ~(0x11);
        if (memcpy(dest + length, src + length, remain) == NULL)
            return NULL;
    }
    if (length == 0)
        return dest;
    do {
        length -= 4;
        *(uint32_t *)(dest + length) = *(volatile uint32_t *)(src + length);
    } while (length > 0);
    return dest;
}
