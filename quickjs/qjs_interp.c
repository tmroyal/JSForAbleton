//
//  js_interp.c
//  quickjs
//
//  Created by Thomas Royal on 7/25/22.
//

#include "qjs_interp.h"

qjs_interp* create_interp(){
    qjs_interp* interp = malloc(sizeof(qjs_interp*));
    if (interp == NULL){ return; }
    
    interp->rt = JS_NewRuntime();
    interp->ctx = JS_NewContext(interp->rt);
    post("Interp: %d\n", interp->rt == NULL);
    post("Interp: %d\n", interp->ctx == NULL);
    
    return interp;
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

JSValue interp_code(qjs_interp* interp, const char* code, int len){
    // TODO: handle errors
    if (len <= 0){ len = (int)strlen(code); }
    return JS_Eval(interp->ctx, code, len, "<none>", JS_EVAL_TYPE_GLOBAL);
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
        str = JS_ToCStringLen(ctx, &len, argv[i]);
        if (!str){
            return JS_EXCEPTION;
        }
        (*post_func)(glob_obj, str);
        JS_FreeCString(ctx, str);
    }
    return JS_UNDEFINED;
}

void setup_console( qjs_interp* interp){
    JSValue this, console;
    
    this = JS_GetGlobalObject(interp->ctx);
    console = JS_NewObject(interp->ctx);
    
    JS_SetPropertyStr(interp->ctx, console, "log",
                   JS_NewCFunction(interp->ctx, con_log, "log", 1));
    JS_SetPropertyStr(interp->ctx, console, "warn",
                   JS_NewCFunction(interp->ctx, con_warn, "warn", 1));
    JS_SetPropertyStr(interp->ctx, console, "error",
                   JS_NewCFunction(interp->ctx, con_error, "error", 1));
                   
    JS_SetPropertyStr(interp->ctx, this, "console", console);
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