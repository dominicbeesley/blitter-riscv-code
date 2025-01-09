#include "deice.h"

void deice_enter_hazard3(uint32_t * registers) {

	uint8_t host_status;

	uint32_t mcause = registers[deice_REG_IX_mcause];

	//we deduce target state from interrupt register
	if (mcause & 0x80000000) {
		switch (mcause & 0xF) {
			case 3:
				host_status = TS_H3_SOFT;
				break;
			case 7:
				host_status = TS_H3_TIMER;
				break;
			case 11:
				host_status = TS_H3_DEBUG;		// assume DEBUG button
				break;
			default:
				host_status = TS_H3_UNKNOWN + (mcause & 0xF);
				break;
		}
	} else {
		switch (mcause & 0xF) {
			case 3:
				host_status = TS_BP;
				break;
			default:
				host_status = TS_H3_UNKNOWN_EX + (mcause & 0x0F);
				break;
		}
	}

	deice_enter(host_status, registers);
}
