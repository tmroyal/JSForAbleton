//
//  qjs_struct.h
//  quickjs
//
//  Created by Thomas Royal on 8/25/22.
//

#ifndef qjs_struct_h
#define qjs_struct_h

#include "Scheduler.h"

struct qjs_interp;

typedef struct _quickjs
{
    t_object    ob;
    
    // time stuff
    scheduler* scheduler;
    t_object* time_obj;
    
    // code stuff
    struct qjs_interp   *qjs;
    char **code;
    long code_size;
    bool code_loaded;
    
    // filename stuff
    char filename[MAX_PATH_CHARS];
    char fqn[MAX_PATH_CHARS];
    short path;
    void *filewatcher;
    
    // outlets and inlets
    void *outlet;
} t_quickjs;

#endif /* qjs_struct_h */
