//
//  qjs_struct.h
//  quickjs
//
//  Created by Thomas Royal on 8/25/22.
//

#ifndef qjs_struct_h
#define qjs_struct_h


struct qjs_interp;

typedef struct _quickjs
{
    t_object    ob;
    
    
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
    void *proxy;
    long inlet_num;
} t_quickjs;

#endif /* qjs_struct_h */
