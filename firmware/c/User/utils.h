/*
 * utils.h
 *
 *  Created on: 5 Aug 2024
 *      Author: joel
 */

#ifndef USER_UTILS_H_
#define USER_UTILS_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

void *memset32(void *, uint32_t, size_t);
void *memcpy32(void *, const void *, size_t);

inline int32_t MAX(int32_t a, int32_t b) { return ((a) > (b) ? a : b); }
inline int32_t MIN(int32_t a, int32_t b) { return ((a) < (b) ? a : b); }

#endif /* USER_UTILS_H_ */
