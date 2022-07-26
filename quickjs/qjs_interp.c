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
    
    interp->rt = NULL;
    interp->ctx = NULL;
    
    return interp;
}

void scheduled_interp(t_quickjs* obj, t_symbol *s, short argc, t_atom *argv){
    JSValue res;
    qjs_interp *interp = (qjs_interp*)obj->qjs;
    
    setup_context(obj, interp);
    res = JS_Eval(interp->ctx, *(obj->code), obj->code_size, "<none>", JS_EVAL_TYPE_GLOBAL);
    
    if (JS_IsException(res)){
        print_exception((qjs_interp*)obj->qjs);
    }
    
    return res;
}

void interp_code(t_quickjs* obj, qjs_interp* interp, const char* code, int len){
    schedule(obj, (method)scheduled_interp, 0, NULL, 0, NULL);
}

void print_exception(qjs_interp* interp){
    JSValueConst exception;
    const char * er;
    t_object* obj = (t_object*) JS_GetContextOpaque(interp->ctx);
    
    exception = JS_GetException(interp->ctx);
    er = JS_ToCString(interp->ctx, exception);
    if (er){
        object_error(obj, er);
        JS_FreeCString(interp->ctx, er);
    } else {
        object_error(obj, "Error");
    }
    
    JS_FreeValue(interp->ctx, exception);
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

JSValue post_val(JSContext *ctx, int argc, JSValueConst *argv, void (*post_func)(t_object *, C74_CONST char *, ...)){
    
    t_object* obj = (t_object*)JS_GetContextOpaque(ctx);
    int i;
    const char *str;
    size_t len;

    for(i = 0; i < argc; i++) {
        str = JS_ToCStringLen(ctx, &len, argv[i]);
        if (!str){
            return JS_EXCEPTION;
        }
        (*post_func)(obj, str);
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

void setup_context(t_quickjs* obj, qjs_interp* interp){
    if (interp->ctx != NULL){ JS_FreeContext(interp->ctx); }
    if (interp->rt != NULL){ JS_FreeRuntime(interp->rt); }
    
    interp->rt = JS_NewRuntime();
    interp->ctx = JS_NewContext(interp->rt);
    JS_SetContextOpaque(interp->ctx, (void*)obj);
    
    setup_console(interp);
    setup_outlet(interp);
}

JSValue setup_outlet(qjs_interp* interp){
    JSValue this;
    
    this = JS_GetGlobalObject(interp->ctx);
    
    JS_SetPropertyStr(interp->ctx, this, "outlet",
                   JS_NewCFunction(interp->ctx, outlet, "outlet", 1));
    JS_FreeValue(interp->ctx, this);
}
                   
JSValue outlet(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv){
    JSValue none;
    t_atom atoms[argc];
    
    none = JS_UNDEFINED;
    
    t_quickjs* obj = (t_quickjs*) JS_GetContextOpaque(ctx);
    
    if (argc == 0){
        object_error((t_object*)obj, "outlet() requires at least one argument");
        return none;
    }
    
    for (int i = 0; i < argc; i++){
        atoms[i] = JSValueToAtom(ctx, argv[i]);
    }
    
    if (atoms[0].a_type == A_LONG || atoms[0].a_type == A_FLOAT){
        if (argc == 1){
            outlet_single(obj, atoms[0]);
        } else {
            outlet_list(obj->outlet, NULL, argc, atoms);
        }
    } else if (atoms[0].a_type == A_SYM){
        if (argc == 1){
            outlet_anything(obj->outlet, atom_getsym(&atoms[0]), 0, NULL);
            
        } else {
            outlet_anything(obj->outlet, atom_getsym(&atoms[0]), argc-1, atoms+1);
        }
    } else {
        outlet_anything(obj->outlet, gensym("Error: unexpected type"), 0, NULL);
    }

    return none;
}

void outlet_single(t_quickjs* obj, t_atom value){
    t_symbol *type;
    if (value.a_type == A_LONG){
        type = gensym("int");
    } else {
        type = gensym("float");
    }
    outlet_anything(obj->outlet, type, 1, &value);
}

void destroy_interp(qjs_interp* interp){
    if (interp->ctx != NULL){ JS_FreeContext(interp->ctx); }
    if (interp->rt != NULL){ JS_FreeRuntime(interp->rt); }
    free(interp);
}


void interp_handle_message(t_quickjs* obj, t_symbol *s, long argc, t_atom *argv){
    JSValue global, func;
    JSValue args[argc];
    qjs_interp* interp;
    
    interp = (qjs_interp*)obj->qjs;
    
    if (interp->ctx == NULL){
        object_error((t_object*)obj, "No JS context loaded");
        return;
    }
    
    global = JS_GetGlobalObject(interp->ctx);
    func = JS_GetPropertyStr(interp->ctx, global, s->s_name);
    
    if (JS_IsException(func)){
        object_error((t_object*)obj, "Error on function %s()", s->s_name);
    } else if (JS_IsUndefined(func)){
        object_warn((t_object*)obj, "%s() is not defined", s->s_name);
    } else {
        for (size_t i = 0; i < argc; i++){
            args[i] = AtomToJSValue(interp->ctx, argv[i]);
        }
        
        JS_Call(interp->ctx, func, global, argc, args); 
    }
    
    for (size_t i = 0; i < argc; i++){
        JS_FreeValue(interp->ctx, args[i]);
    }
    JS_FreeValue(interp->ctx, func);
    JS_FreeValue(interp->ctx,  global);
}
