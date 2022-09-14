cmake_minimum_required(VERSION 3.19)
project(quickjs_max)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../max-sdk-base/script/max-pretarget.cmake)

#############################################################
# MAX EXTERNAL
#############################################################

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

target_include_directories( 
	${PROJECT_NAME}
	PRIVATE
	"${MAX_SDK_INCLUDES}"
	"${MAX_SDK_MSP_INCLUDES}"
	"${MAX_SDK_JIT_INCLUDES}"
  "/usr/local/include/quickjs"
)

find_library(QJS_LOC NAMES quickjs/libquickjs.a)
target_link_libraries(${PROJECT_NAME} PRIVATE ${QJS_LOC})

include(${CMAKE_CURRENT_SOURCE_DIR}/../../max-sdk-base/script/max-posttarget.cmake)
