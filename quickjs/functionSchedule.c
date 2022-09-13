//
//  functionSchedule.c
//  quickjs
//
//  Created by Thomas Royal on 8/30/22.
//

#include "functionSchedule.h"

functionSchedule* createFunctionSchedule(size_t init_size, event_resolution
                                          resolution){
    functionSchedule* newFS = malloc(sizeof(functionSchedule));
    newFS->n_events = 0;
    newFS->capacity = init_size;
    newFS->resolution = resolution;
    newFS->outputs = malloc(sizeof(sEvent)*MAX_OUTPUT_EVENTS);
    resizeSchedule(newFS, init_size);
    return newFS;
}

void freeFunctionSchedule(functionSchedule* fsp){
    free(fsp->sEvents);
    free(fsp->outputs);
    free(fsp);
}

void resizeSchedule(functionSchedule* fs, size_t new_size){
    if (new_size < fs->capacity){
        printf("WARNING: CANNOT DECREASE SIZE OF FS: ignoring\n");
        return;
    }
    fs->capacity = new_size;
    sEvent* newSEvents = malloc(sizeof(sEvent)*new_size);
    
    if (fs->n_events > 0){
        for (size_t i = 0; i < fs->n_events; i++){
            newSEvents[i] = fs->sEvents[i];
        }
        free(fs->sEvents);
    }
    fs->sEvents = newSEvents;
}

void clearSchedule(functionSchedule* fs){
    fs->n_events = 0; // we don't delete anything, just overwrite
}

// TODO: optimize this
void insertFunction(functionSchedule *fs, JSValue function, event_time time){
    size_t index, i;
    
    index = getInsertionIndex(fs, time);
    
    fs->n_events++;
    if(fs->capacity < fs->n_events){
        resizeSchedule(fs, fs->capacity*2);
    }
    
    for (i = fs->n_events - 1; i > index; i--){
        fs->sEvents[i] = fs->sEvents[i-1];
    }
    fs->sEvents[index].function = &function;
    fs->sEvents[index].time = time;
}

bool time_lt(event_time self, event_time other, event_resolution res){
    switch (res){
        case ER_TICKS:
            return self.ticks < other.ticks;
            break;
        case ER_SECONDS:
            return self.seconds < other.seconds;
            break;
    }
}

bool time_gt(event_time self, event_time other, event_resolution res){
    switch (res){
        case ER_TICKS:
            return self.ticks > other.ticks;
            break;
        case ER_SECONDS:
            return self.seconds > other.seconds;
            break;
    }
}

bool time_eq(event_time self, event_time other, event_resolution res){
    switch (res){
        case ER_TICKS:
            return self.ticks == other.ticks;
            break;
        case ER_SECONDS:
            return self.seconds == other.seconds;
            break;
    }
}

bool time_lteq(event_time self, event_time other, event_resolution res){
    return time_lt(self, other, res) || time_eq(self, other, res);
}

#ifdef DEBUG
void printTimes(functionSchedule* fs){
    for (int i = 0; i < fs->n_events; i++){
        printf("%d ", fs->sEvents[i].time.ticks);
    }
    printf("");
}
#endif


size_t getInsertionIndex(functionSchedule *fs, event_time time){
    // Start here
    size_t low = 0;
    size_t high = fs->n_events - 1;
    size_t mid;
    
    event_time m_time;
    
    if (fs->n_events == 0){ return 0; }
    
    // binary search
    do {
        mid = (low+high)/2;
        m_time = fs->sEvents[mid].time;
        if (time_eq(time, m_time, fs->resolution)){
            return mid;
        } else if (time_gt(time, m_time, fs->resolution)){
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    } while (low < high);
    
    mid = low;
    m_time = fs->sEvents[mid].time;
    
    if (time_lt(time, m_time, fs->resolution)){
        return mid;
    } else {
        return mid+1;
    }
}

void fs_pop(functionSchedule *fs, event_time cur_time, size_t *n_events){
    size_t n_ev_match = 0;
    sEvent* testEv = fs->sEvents + 1;
    
    // increment i based on conditions
    while (n_ev_match < fs->n_events &&
           time_lteq(fs->sEvents[n_ev_match].time, cur_time, fs->resolution))
    {
        n_ev_match++;
    }

    if (n_ev_match >= MAX_OUTPUT_EVENTS){
        (*n_events) = MAX_OUTPUT_EVENTS;
    } else {
        (*n_events) = n_ev_match;
    }

    if (n_ev_match == 0){ return NULL; }
    
    // copy number of events to the output, discard excess
    memcpy(fs->outputs, fs->sEvents, sizeof(sEvent)*(*n_events));
    
    // move non matching to beginning of array
    fs->n_events -= n_ev_match;
    memmove(fs->sEvents, fs->sEvents + n_ev_match, sizeof(sEvent)*fs->n_events);
}

