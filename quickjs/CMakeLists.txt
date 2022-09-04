cmake_minimum_required(VERSION 3.19)
project(quickjs_max)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../max-sdk-base/script/max-pretarget.cmake)

#############################################################
# MAX EXTERNAL
#############################################################

include_directories( 
	"${MAX_SDK_INCLUDES}"
	"${MAX_SDK_MSP_INCLUDES}"
	"${MAX_SDK_JIT_INCLUDES}"
  "/usr/local/include/quickjs"
)

file(GLOB PROJECT_SRC
		"[!test_]*.h"
		"[!test_]*.c"
		"*.cpp"
)


add_library( 
	${PROJECT_NAME} 
	MODULE
	${PROJECT_SRC}
)

find_library(QJS_LOC NAMES quickjs/libquickjs.a)
target_link_libraries(${PROJECT_NAME} PRIVATE ${QJS_LOC})

add_executable(TestFS 
	test_functionSchedule.c
	test_functionSchedule.h
	functionSchedule.c
	functionSchedule.h
)
target_link_libraries(TestFS PRIVATE ${QJS_LOC})

include(${CMAKE_CURRENT_SOURCE_DIR}/../../max-sdk-base/script/max-posttarget.cmake)
