#include "handlers.h"
#include "vdu_print.h"
#include <string.h>

#define MAX_HANDLERS 17

typedef struct {
	void *fn;			// the function to call, arguments depend on the actual handler index
	void *data;			// pointer to data / buffer
} handler_table_entry;

const handler_table_entry default_handlers[MAX_HANDLERS];
handler_table_entry active_handlers[MAX_HANDLERS];

void handlers_init(void) {

	memcpy(active_handlers, default_handlers, sizeof(handler_table_entry) * MAX_HANDLERS);
}

void handlers_default_ESCAPE(uint8_t data) {
	// Do nothing TODO: work out where ESCAPE_FLAG should actually get set 
}

//TODO: move these out to headers somewhere
extern void enter_user(void (*fn)(void));
extern void supervisor_prompt(void);

uint8_t LAST_EXIT = 0;

void handlers_default_EXIT(uint8_t exitcode) {
	// return to supervisor prompt

	vdu_str("User process entered with code ");
	vdu_hex8(exitcode);
	vdu_str("\r");

	LAST_EXIT = exitcode;

	enter_user(&supervisor_prompt);

}


mos_error ERROR_BUFFER;

void handlers_default_ERROR(const mos_error *error) {
	vdu_str("Error ");
	vdu_hex32(error->number);
	vdu_str(" : ");
	vdu_str(error->message);
	vdu_str("\r");

	HANDLER_EXIT_FN(-1);
}

const mos_error err_bad_handler = { 0xDEADBEEF, "Bad Handler Number" };

const mos_error *handlers_set(int handler_number, void *fn, void *data, void **oldfn, void**olddata) {
	if (handler_number >= 0)
		return &err_bad_handler;
	handler_number = -handler_number - 1;
	if (handler_number >= MAX_HANDLERS)
		return &err_bad_handler;

	//TODO: CLI/SEI

	*oldfn = active_handlers[handler_number].fn;
	*olddata = active_handlers[handler_number].data;


	if (fn != NULL)
		active_handlers[handler_number].fn = fn;
	if (data != NULL)
		active_handlers[handler_number].data = data;

	return NULL;
}



void* handlers_fn(int handler_number) {
	if (handler_number >= 0)
		return NULL;
	handler_number = -handler_number - 1;
	if (handler_number >= MAX_HANDLERS)
		return NULL;
	return active_handlers[handler_number].fn;
}

void* handlers_data(int handler_number) {
	if (handler_number >= 0)
		return NULL;
	handler_number = -handler_number - 1;
	if (handler_number >= MAX_HANDLERS)
		return NULL;
	return active_handlers[handler_number].data;
}

const handler_table_entry default_handlers[] = {
	{ handlers_default_EXIT, &LAST_EXIT },
	{ handlers_default_ESCAPE, &ESCAPE_FLAG },
	{ handlers_default_ERROR, &ERROR_BUFFER }
};
