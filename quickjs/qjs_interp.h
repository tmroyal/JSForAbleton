//
//  js_interp.h
//  quickjs
//
//  Created by Thomas Royal on 7/25/22.
//

#ifndef qjs_interp_h
#define qjs_interp_h

#include <stdio.h>
#include "string.h"

#include "quickjs.h"
#include "quickjs-libc.h"

typedef struct _qjs_interp {
    JSRuntime *rt;
    JSContext *ctx;
} qjs_interp;

void create_interp(qjs_interp*  interp);
void destroy_interp(qjs_interp* interp);

void setup_console(
        qjs_interp* interp,
        JSCFunction*  log,
        JSCFunction*  error,
        JSCFunction*  warn
);


#endif /* js_interp_h */
