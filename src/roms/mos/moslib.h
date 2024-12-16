#ifndef __MOSLIB_H__
#define __MOSLIB_H__

#include <stdint.h>

extern void mos_oswrch(unsigned char vdu);
extern int mos_osrdch(void);
extern void mos_osbyte(int al, int *x, int *y, int *c);

#endif