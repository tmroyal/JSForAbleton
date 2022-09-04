//
//  test_functionSchedule.c
//  quickjs
//
//  Created by Thomas Royal on 9/4/22.
//

#include "test_functionSchedule.h"

int main(){
    JSValue bear;
    JSRuntime *rt;
    rt = JS_NewRuntime();
    JS_FreeRuntime(rt);
    printf("hell world\n");
    return 0;
}
