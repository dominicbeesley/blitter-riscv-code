#ifndef __HANDLERS_H__
#define __HANDLERS_H__

#include <stdint.h>
#include "mos.h"

#define HANDLER_EXIT -1
#define HANDLER_ESCAPE -2
#define HANDLER_ERROR -3
#define HANDLER_EVENT -4
#define HANDLER_UKIRQ -5
#define HANDLER_UKECALL -6
#define HANDLER_UKEXCEPT -7

typedef void (*handlers_exit_fn)(uint32_t exitcode);
typedef void (*handlers_escape_fn)(uint8_t flag);
typedef void (*handlers_error_fn)(mos_error *error_block);
typedef void (*handlers_event_fn)(uint8_t *A, uint8_t *X, uint8_t *Y);
typedef void (*handlers_ukirq_fn)();
typedef mos_error * (*handlers_ukecall_fnd)(struct mos_args *args, uint32_t a7);
typedef void (*handlers_ukexcept_fn)();		//TODO: Discuss types of exception

extern const mos_error *handlers_set(int handler_number, void *fn, void *data, void **oldfn, void**olddata);
extern void* handlers_fn(int handler_number);
extern void* handlers_data(int handler_number);
extern void handlers_init(void);

//quick access to handler functions

#define HANDLER_EXIT_FN ((handlers_exit_fn)handlers_fn(HANDLER_EXIT))
#define HANDLER_ESCAPE_FN ((handlers_escape_fn)handlers_fn(HANDLER_ESCAPE))
#define HANDLER_ERROR_FN ((handlers_error_fn)handlers_fn(HANDLER_ERROR))
#define HANDLER_EVENT_FN ((handlers_event_fn)handlers_fn(HANDLER_EVENT))
#define HANDLER_UKIRQ_FN ((handlers_ukirq_fn)handlers_fn(HANDLER_UKIRQ))
#define HANDLER_UKECALL_FN ((handlers_ukecall_fn)handlers_fn(HANDLER_UKECALL))
#define HANDLER_UKEXCEPT_FN ((handlers_ukexcept_fn)handlers_fn(HANDLER_UKEXCEPT))

#define HANDLER_EXIT_DATAPTR ((uint8_t *)handlers_data(HANDLER_EXIT))
#define HANDLER_ESCAPE_DATAPTR ((uint8_t *)handlers_data(HANDLER_ESCAPE))
#define HANDLER_ERROR_DATAPTR ((mos_error *)handlers_data(HANDLER_ERROR))



#endif