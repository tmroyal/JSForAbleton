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
#include "ext.h"
#include "ext_obex.h"

#include "quickjs.h"
#include "quickjs-libc.h"

typedef struct _qjs_interp {
    JSRuntime *rt;
    JSContext *ctx;
} qjs_interp;

/*
 Bellard uses global variables in
 quickjs-libc when setting up a JS
 function. Be like Bellard!
 */
t_object* glob_obj;

qjs_interp *create_interp();
void destroy_interp(qjs_interp* interp);
void set_glob_obj(t_object* obj);

JSValue interp_code(qjs_interp* interp, const char* code);

void setup_console( qjs_interp* interp );

JSValue con_log(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);
JSValue con_warn(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);
JSValue con_error(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);
JSValue post_val(JSContext *ctx, int argc, JSValueConst *argv, void (*post_func)(t_object *, C74_CONST char *, ...));


#endif /* js_interp_h */
