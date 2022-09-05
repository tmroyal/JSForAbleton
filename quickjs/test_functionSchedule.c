//
//  test_functionSchedule.c
//  quickjs
//
//  Created by Thomas Royal on 9/4/22.
//

#include "test_functionSchedule.h"


bool noSmoke(){
    PF("Smoke test");
    functionSchedule* fs = createFunctionSchedule(2, ER_TICKS);
    freeFunctionSchedule(fs);
    PRINT("PASS");
    return true;
}

bool testResizesSchedule(){
    PF("Resize Schedule");
    SETUP();
    
    fs->sEvents[1].time.ticks = 1;
    // This is not how insertions work, but is adequate to testing
    fs->n_events = 2;
    
    resizeSchedule(fs, 8);
    TEST(fs->capacity == 8, "Capcity must be set");
    TEST(fs->sEvents[1].time.ticks == 1, "Resize keeps data");
    
    resizeSchedule(fs, 4);
    TEST(fs->capacity == 8, "Shrinking does nothing");
    
    TEARDOWN();
    RETURN();
}


bool testTimeSecondsComparisons(){
    bool result = true;
    PF("Time Comparison (SECONDS)");
    event_time s1;
    event_time s2;
    
    s1.seconds = 0;
    s2.seconds = 1;
    TEST(time_lt(s1, s2, ER_SECONDS), "time lt");
    
    s1.seconds = 1;
    TEST(time_eq(s1, s2, ER_SECONDS), "time eq");
    
    s1.seconds = 2;
    TEST(time_gt(s1, s2, ER_SECONDS), "time gt");
    
    RETURN();
}


bool testTimeTicksComparisons(){
    bool result = true;
    PF("Time Comparison (TICKS)");
    event_time s1;
    event_time s2;
    
    s1.ticks = 0;
    s2.ticks = 1;
    TEST(time_lt(s1, s2, ER_TICKS), "time lt");
    
    s1.ticks = 1;
    TEST(time_eq(s1, s2, ER_TICKS), "time eq");
    
    s1.ticks = 2;
    TEST(time_gt(s1, s2, ER_TICKS), "time gt");
    
    RETURN();
}

bool insertionTest(const char* title, size_t expected, uint32_t tick){
    size_t ind;
    event_time t;
    
    t.ticks = tick;
    
    PF(title);
    SETUP();
    
    resizeSchedule(fs, 4);
    // we do this during testing
    fs->n_events = 4;
    
    fs->sEvents[0].time.ticks = 1;
    fs->sEvents[1].time.ticks = 2;
    fs->sEvents[2].time.ticks = 4;
    fs->sEvents[3].time.ticks = 5;
    
    ind = getInsertionIndex(fs, t);
    
    TEST(ind == expected, "insertion index");
    
    TEARDOWN();
    RETURN();
}

bool testInsertionIndexMid(){
    return insertionTest("Mid Insertion", 2, 3);
}

bool testInsertionIndexBefore(){
    return insertionTest("Before Insertion", 0, 0);
}

bool testInsertionIndexAfter(){
    return insertionTest("After Insertion", 4, 8);
}

bool testInsert(){
    JSValue func;
    event_time t;
    bool in_order;
    t.ticks = 0;
    
    PF("Test insert");
    SETUP();
    
    insertFunction(fs, func, t);
    t.ticks = 2;
    insertFunction(fs, func, t);
    t.ticks = 1;
    insertFunction(fs, func, t);
    
    in_order = fs->sEvents[0].time.ticks == 0 &&
            fs->sEvents[1].time.ticks == 1 &&
            fs->sEvents[2].time.ticks == 2;
    TEST(in_order, "Events inserted correct order");
    TEST(fs->n_events == 3, "Correct number of events");
    
    TEARDOWN();
    RETURN();
}

bool testPopOneEv(){
    JSValue func;
    event_time t;
    bool in_order;
    t.ticks = 0;
    
    PF("Test pop one ev");
    SETUP();
    
    insertFunction(fs, func, t);
    t.ticks = 2;
    insertFunction(fs, func, t);
    t.ticks = 1;
    insertFunction(fs, func, t);
    
    TODO("Implement pop one tests");
    
    TEARDOWN();
    RETURN();
}

bool testPopTwoEv(){
    JSValue func;
    event_time t;
    bool in_order;
    t.ticks = 0;
    
    PF("Test pop two ev");
    SETUP();
    
    insertFunction(fs, func, t);
    t.ticks = 2;
    insertFunction(fs, func, t);
    t.ticks = 1;
    insertFunction(fs, func, t);
    
    TODO("Implement pop two tests");
    
    TEARDOWN();
    RETURN();
}

bool runTests(){
    bool (*tests[])() = {
        noSmoke,
        testResizesSchedule,
        testTimeSecondsComparisons,
        testTimeTicksComparisons,
        testInsertionIndexMid,
        testInsertionIndexBefore,
        testInsertionIndexAfter,
        testInsert,
        testPopOneEv,
        testPopTwoEv,
        NULL
    };
    
    bool result = true;
    bool inter;
    size_t ind;
    
    for (ind = 0; tests[ind] != NULL; ind++){
        inter = (*tests[ind])();
        result &= inter;
    }
    
    return result;
}


int main(){
    bool result;
    PRINT("Function Schedule Tests");
    PRINT("-----------------------");
    
    result = runTests();
    
    PRINT("");
    if (result){
        PRINT("ALL PASS");
    } else {
        PRINT("SOME FAIL");
    }
    PRINT("");
    
    if (result){
        return 0;
    } else {
        return EIO;
    }
    
    return 0;
}

