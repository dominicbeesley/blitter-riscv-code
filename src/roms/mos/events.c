#include "events.h"
#include "mos_shared.h"
#include "mos.h"

extern void printch(char c);
extern void hexbyte(unsigned int n);


uint8_t event_raise(uint8_t no, uint8_t X, uint8_t Y) {

//	printch('E');
//	printch('!');
//	hexbyte(no);
//	hexbyte(X);
//	hexbyte(Y);

	return FLAG_C;
}