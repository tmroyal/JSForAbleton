//
//  js_interp.h
//  quickjs
//
//  Created by Thomas Royal on 7/25/22.
//

#ifndef js_interp_h
#define js_interp_h

#include <stdio.h>

#include "quickjs.h"
#include "quickjs-libc.h"

typedef struct _js_interp {
    JSRuntime *rt;
} js_interp;

void create_interp();
void destroy_interp();

#endif /* js_interp_h */
