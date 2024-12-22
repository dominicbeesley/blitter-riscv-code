#ifndef __DEBUG_PRINT_H__
#define __DEBUG_PRINT_H__

#include <stdint.h>

extern void debug_print_init(void);
extern void debug_print_str(const char *s);
extern void debug_print_ch(char c);
extern void debug_print_hex_word(uint32_t n);
extern void debug_print_hex_halfword(uint32_t n);
extern void debug_print_hex_byte(uint8_t n);

#ifdef DEBUG_PRINT
#define DEBUG_PRINT_INIT debug_print_init()
#define DEBUG_PRINT_STR(X) debug_print_str(X)
#define DEBUG_PRINT_CH(X) debug_print_ch(X)
#define DEBUG_PRINT_HEX_WORD(X) debug_print_hex_word(X)
#define DEBUG_PRINT_HEX_HALFWORD(X) debug_print_hex_halfword(X)
#define DEBUG_PRINT_HEX_BYTE(X) debug_print_hex_byte(X)
#else
#define DEBUG_PRINT_INIT 
#define DEBUG_PRINT_STR(X)
#define DEBUG_PRINT_CH(X)
#define DEBUG_PRINT_HEX_WORD(X)
#define DEBUG_PRINT_HEX_HALFWORD(X)
#define DEBUG_PRINT_HEX_BYTE(X)
#endif

#endif