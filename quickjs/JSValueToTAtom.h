//
//  JSValueToTAtom.h
//  quickjs
//
//  Created by Thomas Royal on 8/26/22.
//

#ifndef JSValueToTAtom_h
#define JSValueToTAtom_h

#include <stdio.h>

#include "quickjs.h"
#include "ext.h"
#include "ext_obex.h"


t_atom JSValueToAtom(JSContext* ctx, JSValueConst value);

#endif /* JSValueToTAtom_h */
