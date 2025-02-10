#include "events.h"
#include "mos_shared.h"
#include "mos.h"



uint8_t event_raise(uint8_t no, uint8_t X, uint8_t Y) {
	//TODO: raise an event - we need a way of restoring / setting up user's stack?
	//TODO: align with interrupts.S as restoring user's stack will cause interrupts/exceptions to bugger up mos stack by blindly restoring to top of stack!
	return FLAG_C;
}