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

JSValue con_log(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv){

    return post_val(ctx, argc, argv, object_post);
}

JSValue con_warn(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv){
    
    return post_val(ctx, argc, argv, object_warn);
}

JSValue con_error(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv){
    
    return post_val(ctx, argc, argv, object_error);
}

JSValue interp_code(qjs_interp* interp, const char* code){
    // TODO: handle errors
    int len = (int)strlen(code);
    return JS_Eval(ctx, code, len, "<none>", JS_EVAL_TYPE_GLOBAL);
}


/*
    Based on js_print from quickjs-libc.
    Be like Bellard!
 */
JSValue post_val(JSContext *ctx, int argc, JSValueConst *argv, void (*post_func)(t_object *, C74_CONST char *, ...)){
    
    int i;
    const char *str;
    size_t len;

    for(i = 0; i < argc; i++) {
        if (i != 0){
            (*post_func)(glob_obj, " ");
        }
        str = JS_ToCStringLen(ctx, &len, argv[i]);
        if (!str){
            (*post_func)(glob_obj, "\n");
            return JS_EXCEPTION;
        }
        (*post_func)(glob_obj, str);
        JS_FreeCString(ctx, str);
    }
    (*post_func)(glob_obj, "\n");
    return JS_UNDEFINED;
}

void setup_console( qjs_interp* interp){
    JSValue this, console;
    
    this = JS_GetGlobalObject(interp->ctx);
    console = JS_NewObject(interp->ctx);
    
    JS_SetProperty(interp->ctx, console, "log",
                   JS_NewCFunction(interp->ctx, con_log, "log", 1));
    JS_SetProperty(interp->ctx, console, "warn",
                   JS_NewCFunction(interp->ctx, con_warn, "warn", 1));
    JS_SetProperty(interp->ctx, console, "error",
                   JS_NewCFunction(interp->ctx, con_error, "error", 1));
                   
    JS_SetProperty(interp->ctx, this, "console", console);
    JS_FreeValue(interp->ctx, this);
}

void set_glob_obj(t_object* x){
    glob_obj = x;
}

void destroy_interp(qjs_interp* interp){
    JS_FreeContext(interp->ctx);
    JS_FreeRuntime(interp->rt);
    free(interp);
}
