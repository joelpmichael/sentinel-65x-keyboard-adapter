/*
 * utils.c
 *
 *  Created on: 5 Aug 2024
 *      Author: joel
 */

#include "utils.h"

// __EBREAK() implementation
__attribute__ ((section (".slowfunc"))) inline void __EBREAK(void) {
    asm("ebreak");
}

// 32-bit aligned memcpy/memset, in RV32I assembly
void *memcpy(void *dst, const void *src, size_t len) {
    void *o_dst = dst;
    asm volatile(
        "beqz %2, .L_memcpy_end\n" // len == 0, goto end
        "andi t1, %0, 0b11\n" // check for alignment of source: lowest 2 bits set
        "bnez t1, .L_memcpy1\n" // if set, use per-byte memcpy
        "andi t1, %1, 0b11\n" // check for alignment of dest: lowest 2 bits set
        "bnez t1, .L_memcpy1\n" // if set, use per-byte memcpy
        "li t1, 4\n" // 4 is the magic number!
        "blt %2, t1, .L_memcpy1\n" // len < 4, use per-byte memcpy
        ".L_memcpy4:\n"
        "   lw t0, (%0)\n" // load 4 byte word from source
        "   sw t0, (%1)\n" // save 4 byte word to dest
        "   addi %0, %0, 4\n" // increment source pointer
        "   addi %1, %1, 4\n" // increment dest pointer
        "   addi %2, %2, -4\n" // decrement bytes remaining
        "   bgeu %2, t1, .L_memcpy4\n" // if >= 4 bytes remaining, repeat loop
        "   beqz %2, .L_memcpy_end\n" // if 0 bytes remaining, goto end
        ".L_memcpy1:\n"
        "   lbu t0, (%0)\n" // load unsigned byte from source
        "   sb t0, (%1)\n" // save byte to dest
        "   addi %0, %0, 1\n" // increment source pointer
        "   addi %1, %1, 1\n" // increment dest pointer
        "   addi %2, %2, -1\n" // decrement bytes remaining
        "   bnez %2, .L_memcpy1\n" // if > 0 bytes remaining, repeat loop
        ".L_memcpy_end:\n"
        :"+r"(src),"+r"(dst),"+r"(len)
        : 
        :"t0", "t1"
    );
    return o_dst;
}

void *memset(void *dst, int val, size_t len) {
    void *o_dst = dst;
    asm volatile(
        "beqz %2, .L_memset_end\n" // len == 0, goto end
        "andi t1, %1, 0b11\n" // check for alignment of dest: lowest 2 bits set
        "bnez t1, .L_memset1\n" // if set, use per-byte memset
        "li t1, 4\n"
        "blt %2, t1, .L_memset1\n" // len < 4, use per-byte memset
        "andi t1, %0, 0xFF\n" // mask low byte of val
        "and t0, t1, t1\n" // copy low byte of val
        "slli t0, t0, 8\n" // left-shift 8
        "or t0, t0, t1\n" // or low byte of val
        "slli t0, t0, 8\n" // .. etc
        "or t0, t0, t1\n"
        "slli t0, t0, 8\n"
        "or t0, t0, t1\n" // word now contains val 4 times
        "li t1, 4\n"
        ".L_memset4:\n"
        "   sw t0, (%1)\n" // save 4 copies of val to dest
        "   addi %1, %1, 4\n" // increment dest pointer
        "   addi %2, %2, -4\n" // decrement bytes remaining
        "   bgeu %2, t1, .L_memset4\n" // if >= 4 bytes remaining, repeat loop
        "   beqz %2, .L_memset_end\n" // if 0 bytes remaining, goto end
        ".L_memset1:\n"
        "   sb %0, (%1)\n" // save val to dest
        "   addi %1, %1, 1\n" // increment dest pointer
        "   addi %2, %2, -1\n" // decrement bytes remaining
        "   bnez %2, .L_memset1\n" // if > 0 bytes remaining, repeat loop
        ".L_memset_end:\n"
        :"+r"(val),"+r"(dst),"+r"(len)
        : 
        :"t0", "t1"
    );
    return o_dst;
}
