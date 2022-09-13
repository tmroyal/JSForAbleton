//
//  Scheduler.c
//  quickjs
//
//  Created by Thomas Royal on 9/6/22.
//

#include "Scheduler.h"

// TODO: critical regions
// TODO: ensure sched free when freeing in main obj
// TODO: clear on interpret

scheduler* createScheduler(t_object* time_object){
    scheduler* scheduler;
    scheduler = malloc(sizeof(scheduler));
    
    scheduler->secondsSchedule = createFunctionSchedule(4, ER_SECONDS);
    scheduler->ticksSchedule = createFunctionSchedule(4, ER_TICKS);
    
    scheduler->time_object = time_object;
    scheduler->js_ctx = NULL;
    return scheduler;
}

void clearScheduler(scheduler* sch){
    clearSchedule(sch->secondsSchedule);
    clearSchedule(sch->ticksSchedule);
}

void schedulerSeekTo(scheduler* sch){
    size_t ignore;
    double time_seconds = time_getms((t_timeobject*)sch->time_object)/1000.0;
    long time_ticks = time_getticks((t_timeobject*)sch->time_object);
    
    event_time ts;
    event_time tt;
    
    ts.seconds = time_seconds;
    tt.ticks = time_ticks;
    
    fs_pop(sch->ticksSchedule, tt, &ignore);
    fs_pop(sch->secondsSchedule, ts, &ignore);
}

void set_js_ctx(schedule* sch, JSContext* ctx){
    sch->js_ctx = ctx;
}

void freeScheduler(scheduler* sch){
    freeFunctionSchedule(sch->ticksSchedule);
    freeFunctionSchedule(sch->secondsSchedule);
    free(sch);
}

void sch_inSeconds(scheduler* sch, event_time time, JSValueConst func){
    double current_time = time_getms((t_timeobject*)sch->time_object)/1000.0;
    double sched_time = current_time + time.seconds;
    insertFunction(sch->secondsSchedule, func, time);
}

void sch_atSeconds(scheduler* sch, event_time time, JSValueConst func){
    double current_time = time_getms((t_timeobject*)sch->time_object)/1000.0;
    if (time.seconds > current_time){ // event must be in future
        insertFunction(sch->secondsSchedule, func, time);
    }
}

void sch_inTicks(scheduler* sch, event_time time, JSValueConst func){
    long current_time = time_getticks((t_timeobject*)sch->time_object)/1000.0;
    long sched_time = current_time + time.ticks;
    insertFunction(sch->secondsSchedule, func, time);
}

void sch_atTicks(scheduler* sch, event_time time, JSValueConst func){
    long current_time = time_getticks((t_timeobject*)sch->time_object);
    if (time.ticks > current_time){
        insertFunction(sch->ticksSchedule, func, time);
    }
}

void execute_scheduler(scheduler* sch){
    double time_seconds = time_getms((t_timeobject*)sch->time_object)/1000.0;
    long time_ticks = time_getticks((t_timeobject*)sch->time_object);
    size_t n_sec_events, n_tick_events;
    
    JSValue args[] = {
        JS_NewInt32(sch->js_ctx, time_tick),
        JS_NewFloat64(sch->js_ctx, time_seconds)
    };
    
    fs_pop(sch->secondsSchedule, time_seconds, &n_sec_events);
    fs_pop(sch->ticksSchedule, time_ticks, &n_tick_events);
    
    if (!sch->js_ctx){ return; }

    call_output_funcs(sch, sch->ticksSchedule, n_tick_events, args);
    call_output_funcs(sch, sch->secondsSchedule, n_sec_events, args);
}

void call_output_funcs(scheduler* sch, functionSchedule* fs, size_t n, JSValueConst* args){
    
    for (size_t i = 0; i < n; i++){
        JS_Call(sch->js_ctx, fs->outputs[i].function, JS_UNDEFINED, N_JS_ARGS, args);
    }
    
}
