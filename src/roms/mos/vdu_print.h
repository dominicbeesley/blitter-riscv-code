#ifndef __VDU_PRINT_H__
#define __VDU_PRINT_H__

#include <stdint.h>

extern void vdu_str(const char *str);
extern void vdu_hexnyb(unsigned hex);
extern void vdu_hex8(unsigned hex);
extern void vdu_hex32(unsigned hex);


#endif