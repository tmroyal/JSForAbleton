//
//  Scheduler.h
//  quickjs
//
//  Created by Thomas Royal on 9/6/22.
//

#ifndef Scheduler_h
#define Scheduler_h

#include <stdio.h>

#include "ext.h"
#include "ext_obex.h"
#include "ext_time.h"
#include "ext_itm.h"

#include "functionSchedule.h"

#define N_JS_ARGS 2

typedef struct _scheduler {
    functionSchedule* secondsSchedule;
    functionSchedule* ticksSchedule;
    t_object* time_object;
    JSContext* js_ctx;
} scheduler;

scheduler* createScheduler(t_object* time_object);
void freeScheduler(scheduler* sch);

void set_js_ctx(scheduler* sch, JSContext* ctx);

void inSeconds(scheduler* sch, event_time time, JSValueConst func);
void atSeconds(scheduler* sch, event_time time, JSValueConst func);
void inTicks(scheduler* sch, event_time time, JSValueConst func);
void atTicks(scheduler* sch, event_time time, JSValueConst func);

void execute_scheduler(scheduler* sch); // gets time and calls an functions
void call_output_funcs(scheduler* sch, functionSchedule* fs, size_t n, JSValueConst* args);

#endif /* Scheduler_h */
