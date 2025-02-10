#include "deice.h"

void deice_enter_picorv32(uint32_t * registers) {

	uint8_t status;

	//TODO: not sure this is the right place for this but we need
	//to readjust the PC pointer depending on the type of interrupt
	//adjust back to previous instruction if illegal op / break / bus error
	uint32_t irq_type = registers[32];
	if (irq_type & 0x6) {
		if (registers[31] & 1) {
			//16 bit instruction
			registers[31]-=2;
		} else {
			registers[31]-=4;
		}
	}

	//we deduce target state from interrupt register
	if (irq_type & 1) {
		status = TS_RV_TIMER;
		registers[32] &= ~1;
	}
	else if (irq_type & 2)
	{
		uint8_t *pc = (uint8_t *)(registers[31] & 0xFFFFFFFE);

		// check for a c.ebreak/break at PC
		if (
			((pc[0] == 0x02) && (pc[1] == 0x90))
			| ((pc[0] == 0x73) && (pc[1] == 0x00) && (pc[2] == 0x10) && (pc[1] == 0x00))
			)
			// break or c.break
			status = TS_BP;
		else if ((pc[0] == 0x73) && (pc[1] == 0x00) && (pc[2] == 0x00) && (pc[1] == 0x00))
			// ecall
			status = TS_RV_CALL;
		else
			status = TS_ILLEGAL;
		registers[32] &= ~2;
	}
	else if (irq_type & 4) {
		status = TS_RV_BUSERROR;
		registers[32] &= ~4;		
	}
	else if (irq_type & 8) {
		status = TS_RV_NMI;
		registers[32] &= ~8;		
	}
	else if (irq_type & 16) {
		status = TS_RV_IRQ;
		registers[32] &= ~16;		
	}
	else if (irq_type & 32) {
		status = TS_RV_DEBUG;
		registers[32] &= ~32;		
	} else {
		status = TS_RV_UNKNOWN;		
		uint32_t m = 1;
		while (m) {
			if (m & irq_type) {
				registers[32] &= m;
			} else {
				m = m << 1;
			}
		}
	}

	deice_enter(status, registers);
}
