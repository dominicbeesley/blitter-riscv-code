#ifndef __BUFFERS_H__
#define __BUFFERS_H__

#include <stdint.h>

extern uint8_t buffers_insert_check_ESC(uint8_t buffer_num, uint8_t data);
extern uint8_t buffers_insert(uint8_t buffer_num, uint8_t data);

extern uint8_t buffers_default_INSV(uint8_t buffer, uint8_t data);
extern uint8_t buffers_default_REMV(uint8_t buffer_num, uint8_t flags, uint8_t *c);
extern int buffers_default_RDCHV(void);

extern void buffers_init(void);

#endif