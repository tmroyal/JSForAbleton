//
//  JSValueToTAtom.c
//  quickjs
//
//  Created by Thomas Royal on 8/26/22.
//

#include "JSValueToTAtom.h"

t_atom JSValueToAtom(JSContext* ctx, JSValueConst value){
    t_atom result;
    int64_t int_res;
    double float_res;
    
    switch(value.tag){
        case JS_TAG_BIG_DECIMAL:
        case JS_TAG_BIG_INT:
        case JS_TAG_BIG_FLOAT:
        case JS_TAG_STRING:
            atom_setsym(&result, gensym(JS_ToCString(ctx,  value)));
            break;
        case JS_TAG_INT:
            JS_ToInt64(ctx, &int_res, value);
            atom_setlong(&result, int_res);
            break;
        case JS_TAG_FLOAT64:
            JS_ToFloat64(ctx, &float_res, value);
            atom_setfloat(&result, float_res);;
            break;
        case JS_TAG_BOOL:
            atom_setlong(&result, JS_ToBool(ctx, value));
            break;
        case JS_TAG_OBJECT:
            atom_setsym(&result, gensym("[object]"));
            break;
        case JS_TAG_SYMBOL:
            atom_setsym(&result, gensym("symbol"));
        default:
            atom_setsym(&result, gensym("null"));
    
    }
    
    return result;
}
