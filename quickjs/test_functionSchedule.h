//
//  test_functionSchedule.h
//  quickjs
//
//  Created by Thomas Royal on 9/4/22.
//

#ifndef test_functionSchedule_h
#define test_functionSchedule_h

#include <stdio.h>
#include <stdbool.h>
#include <errno.h>

#include "quickjs.h"

#include "functionSchedule.h"


#define PF(S) \
printf("%s", S); \
printf(" . . . ")

#define PRINT(S) \
printf(S); \
printf("\n")

#define SETUP() \
bool result = true; \
functionSchedule * fs = createFunctionSchedule(2, ER_TICKS)

#define TEARDOWN() \
freeFunctionSchedule(fs);

#define RETURN() \
if (result){ \
PRINT("TEST Pass"); \
} else {\
PRINT("TEST FAIL"); \
} \
return result;

#define TEST(COND,MSG) \
if (!(COND)){ \
    PRINT(""); \
    printf("FAIL: "); \
    PRINT(MSG); \
} \
result &= COND;

#define TODO(ITEM) \
PRINT(""); \
printf("TODO: "); \
PRINT(ITEM); \
result = false

#endif /* test_functionSchedule_h */
