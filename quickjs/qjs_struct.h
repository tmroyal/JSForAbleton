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
    struct qjs_interp   *qjs;
    char filename[MAX_PATH_CHARS];
    char fqn[MAX_PATH_CHARS];
    char **code;
    long code_size;
    bool code_loaded;
    short path;
    void *filewatcher;
    void *outlet;
} t_quickjs;

#endif /* qjs_struct_h */
