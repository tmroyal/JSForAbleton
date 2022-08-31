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
    resizeSchedule(newFS, init_size);
    return newFS;
}

void freeFunctionSchedule(functionSchedule* fsp){
    free(fsp->sEvents);
    free(fsp);
}

void resizeSchedule(functionSchedule* fs, size_t new_size){
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


void insertFunction(functionSchedule *fs, JSValue function, event_time time){
    size_t index;
    
    fs->n_events++;
    if(fs->capacity < fs->n_events){
        resizeSchedule(fs, fs->capacity*2);
    }
        
    index = getInsertionIndex(fs, time);
}

// binary search
size_t getInsertionIndex(functionSchedule *fs, event_time time){
    // Start here
    //size_t index = fs->nE
}

