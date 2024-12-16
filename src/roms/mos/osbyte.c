#include "osbyte.h"
#include "mos.h"
#include "buffers.h"
#include "mos_shared.h"
#include "interrupts.h"
#include <stddef.h>


#define XYSET(X,V) {if(X) *X = V;}
#define XYGET(X) ((X)?*X:0)

uint8_t osbyte_15_FLUSH_BUFFER_CLASS(uint8_t *X, uint8_t *Y);
uint8_t osbyte_21_FLUSH_BUFFER(uint8_t *X, uint8_t *Y);
uint8_t osbyte_124_ESCAPE_CLEAR(uint8_t *X, uint8_t *Y);
uint8_t osbyte_125_ESCAPE_SET(uint8_t *X, uint8_t *Y);
uint8_t osbyte_126_ESCAPE_ACK(uint8_t *X, uint8_t *Y);

uint8_t osbyte_default_BYTEV(uint8_t A, uint8_t *X, uint8_t *Y) {
	
	switch (A) {
		case OSBYTE_15_FLUSH_BUFFER_CLASS:	return osbyte_15_FLUSH_BUFFER_CLASS(X, Y);
		case OSBYTE_21_FLUSH_BUFFER:		return osbyte_21_FLUSH_BUFFER(X, Y);
		case OSBYTE_124_ESCAPE_CLEAR:		return osbyte_124_ESCAPE_CLEAR(X, Y);
		case OSBYTE_125_ESCAPE_SET:			return osbyte_125_ESCAPE_SET(X, Y);
		case OSBYTE_126_ESCAPE_ACK:			return osbyte_126_ESCAPE_ACK(X, Y);
		default:
			//TODO: check what should happen here
			return FLAG_C;
	}

}

uint8_t osbyte_15_FLUSH_BUFFER_CLASS(uint8_t *X, uint8_t *Y) {
	if (XYGET(X)) {
		XYSET(X, OSB_IN_STREAM);
		osbyte_21_FLUSH_BUFFER(X,Y);
	} else {
		for (uint8_t XX = 0; XX <= BUFNOMAX; XX++) {
			osbyte_21_FLUSH_BUFFER(&XX, NULL);
		}
	}
	return 0;
}

uint8_t osbyte_21_FLUSH_BUFFER(uint8_t *X, uint8_t *Y) {
	buffers_flush(0, XYGET(X));
	return 0;
}


uint8_t osbyte_124_ESCAPE_CLEAR(uint8_t *X, uint8_t *Y) {
	ESCAPE_FLAG = ESCAPE_FLAG >> 1; // clear
	//TODO: TUBE
	return 0;
}
uint8_t osbyte_125_ESCAPE_SET(uint8_t *X, uint8_t *Y) {
	ESCAPE_FLAG = 0x80 | (ESCAPE_FLAG >> 1); // clear
	//TODO: TUBE
	return 0;
}
uint8_t osbyte_126_ESCAPE_ACK(uint8_t *X, uint8_t *Y) {
	if (!(ESCAPE_FLAG & FLAG_N))
		return osbyte_124_ESCAPE_CLEAR(X, Y);

	if (!OSB_ESC_EFFECTS) {
		interrupts_disable(0);
		OSB_HALT_LINES = 0;
		//TODO: EXEC close
		uint8_t XX = 0;
		osbyte_15_FLUSH_BUFFER_CLASS(&XX, NULL);
	}

	XYSET(X, 0xFF);
	return osbyte_124_ESCAPE_CLEAR(X, Y);
}


