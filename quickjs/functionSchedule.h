//
//  functionSchedule.h
//  quickjs
//
//  Adds quickjs javascripts to an array of functions
//
//  Created by Thomas Royal on 8/30/22.
//

#ifndef functionSchedule_h
#define functionSchedule_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "quickjs.h"

typedef enum er { ER_TICKS, ER_SECONDS } event_resolution;

typedef union et {
    uint32_t ticks;
    double seconds;
} event_time;

typedef struct _sEvent {
    event_time time;
    JSValue* function;
} sEvent;

typedef struct _funcSched {
    event_resolution resolution;
    sEvent*  sEvents;
    size_t n_events;
    size_t capacity;
} functionSchedule;


functionSchedule* createFunctionSchedule(size_t init_size, event_resolution);
void freeFunctionSchedule(functionSchedule* fsp);
void resizeSchedule(functionSchedule* fs, size_t new_size);
void insertFunction(functionSchedule *fs, JSValue function, event_time time);
size_t getInsertionIndex(functionSchedule *fs, event_time time);
bool time_gt(event_time self, event_time other, event_resolution res);
bool time_lt(event_time self, event_time other, event_resolution res);
bool time_eq(event_time self, event_time other, event_resolution res);


#ifdef DEBUG
void printTimes(functionSchedule* fs);
#endif


// get events, passing in event pointer and number events
// remove events before n
// we do not need to free the sEvent, but may need to free the JSValue

// JS_Call
// JS_FreeValue
// JS_Invoke

#endif /* functionSchedule_h */
