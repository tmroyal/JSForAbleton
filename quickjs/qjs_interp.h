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
#include "qjs_struct.h"
#include "JSValueToTAtom.h"


typedef struct _qjs_interp {
    JSRuntime *rt;
    JSContext *ctx;
} qjs_interp;


qjs_interp *create_interp();
void destroy_interp(qjs_interp* interp);

JSValue interp_code(t_quickjs* obj, qjs_interp* interp, const char* code, int len);
void outlet_single(t_quickjs* obj, t_atom value);

JSValue setup_outlet(qjs_interp* interp);
void setup_console(qjs_interp* interp );
void setup_context(t_quickjs* obj, qjs_interp* interp);

JSValue con_log(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);
JSValue con_warn(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);
JSValue con_error(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);
JSValue post_val(JSContext *ctx, int argc, JSValueConst *argv, void (*post_func)(t_object *, C74_CONST char *, ...));
void print_exception(qjs_interp* interp);
JSValue outlet(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);

JSValue interp_handle_bang(qjs_interp* interp);

// RESUME HERE
// JSValue atSeconds(JSCont

#endif /* js_interp_h */
