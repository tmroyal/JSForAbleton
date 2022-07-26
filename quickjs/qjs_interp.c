//
//  js_interp.c
//  quickjs
//
//  Created by Thomas Royal on 7/25/22.
//

#include "qjs_interp.h"

void create_interp(qjs_interp* interp){
    interp = malloc(sizeof(qjs_interp*));
    if (interp == NULL){ return; }
    
    interp->rt = JS_NewRuntime();
    interp->ctx = JS_NewContext(interp->rt);
}

void setup_console(
        qjs_interp* interp,
        JSCFunction*  log,
        JSCFunction*  error,
        JSCFunction*  warn
                   ){
    
    JSValue global_obj, console;
    
    global_obj = JS_GetGlobalObject(interp->ctx);
    console = JS_NewObject(interp->ctx);
    
    JS_SetProperty(interp->ctx, console, "log",
                   JS_NewCFunction(interp->ctx, log, "log", 1));
    JS_SetProperty(interp->ctx, console, "warn",
                   JS_NewCFunction(interp->ctx, warn, "warn", 1));
    JS_SetProperty(interp->ctx, console, "error",
                   JS_NewCFunction(interp->ctx, error, "error", 1));
                   
    JS_SetProperty(interp->ctx, global_obj, "console", console);
}

void destroy_interp(qjs_interp* interp){
    JS_FreeContext(interp->ctx);
    JS_FreeRuntime(interp->rt);
    free(interp);
}
