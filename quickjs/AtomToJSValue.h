//
//  AtomToJSValue.h
//  quickjs
//
//  Created by Thomas Royal on 10/15/22.
//

#ifndef AtomToJSValue_h
#define AtomToJSValue_h

#include <stdio.h>

#include "quickjs.h"
#include "ext.h"
#include "ext_obex.h"

JSValue AtomToJSValue(JSContext* ctx, t_atom atom);
#endif /* AtomToJSValue_h */
