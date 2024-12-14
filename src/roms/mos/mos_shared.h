#ifndef __MOS_SHARED__
#define __MOS_SHARED__

void mos_latch_write(uint8_t val);


#define DELAY1	*sheila_USRVIA_ora_nh;

// these are used in various internal calls to mimic
// flags being passed in 6502 version
#define FLAG_N 0x80
#define FLAG_V 0x40
#define FLAG_Z 0x02
#define FLAG_C 0x01


#endif