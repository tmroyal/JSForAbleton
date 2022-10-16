//
//  AtomToJSValue.c
//  quickjs
//
//  Created by Thomas Royal on 10/15/22.
//

#include "AtomToJSValue.h"

JSValue AtomToJSValue(JSContext* ctx, t_atom atom){
    JSValue result;
    
    switch(atom_gettype(&atom)){
        case A_SYM:
            result = JS_NewString(ctx, atom_getsym(&atom)->s_name);
            break;
        case A_LONG:
            result = JS_NewInt64(ctx, atom_getfloat(&atom));
            break;
        case A_FLOAT:
            result = JS_NewFloat64(ctx, atom_getfloat(&atom));
            break;
        default:
            result = JS_UNDEFINED;
    }
    
    return result;
}
