cmake_minimum_required(VERSION 2.8)
project(sylar)

include(cmake/utils.cmake)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")

include_directories(.)
include_directories(/apps/sylar/include)
include_directories(/apps/sylar/lib)
include_directories(/apps/sylar/lib64)
include_directories(/usr/local/include)
link_directories(/usr/local/lib)
link_directories(/usr/local/lib64)

set(LIB_SRC
    sylar/address.cpp
    sylar/bytearray.cpp
    sylar/config.cpp
    sylar/fd_manager.cpp
    sylar/fiber.cpp
    sylar/http/http.cpp
    sylar/http/http_parser.cpp
    sylar/http/http11_parser.rl.cpp
    sylar/http/httpclient_parser.rl.cpp
    sylar/hook.cpp
    sylar/IOManager.cpp
    sylar/log.cpp
    sylar/scheduler.cpp
    sylar/Socket.cpp
    sylar/tcp_server.cpp
    sylar/thread.cpp
    sylar/timer.cpp
    sylar/util.cpp
    )

add_library(sylar SHARED ${LIB_SRC})
force_redefine_file_macro_for_sources(sylar) # 重定义 宏 __FILE__
# add_library(sylar_static STATIC ${LIB_SRC})
# SET_TARGET_PROPERTIES (sylar_static PROPERTIES OUTPUT_NAME "sylar")

find_library(YAMLCPP yaml-cpp)


set(LIBS
    sylar
    pthread
    dl
    ${YAMLCPP}
    )

message("***", ${LIBS})

add_executable(test tests/test.cpp)
add_dependencies(test sylar)
force_redefine_file_macro_for_sources(test) # 重定义 宏 __FILE__
target_link_libraries(test ${LIBS})

add_executable(test_config tests/test_config.cpp)
add_dependencies(test_config sylar)
force_redefine_file_macro_for_sources(test_config) # 重定义 宏 __FILE__
target_link_libraries(test_config ${LIBS})

add_executable(test_thread tests/test_thread.cpp)
add_dependencies(test_thread sylar)
force_redefine_file_macro_for_sources(test_thread) # 重定义 宏 __FILE__
target_link_libraries(test_thread ${LIBS})

add_executable(test_util tests/test_util.cpp)
add_dependencies(test_util sylar)
force_redefine_file_macro_for_sources(test_util) # 重定义 宏 __FILE__
target_link_libraries(test_util ${LIBS})

add_executable(test_fiber tests/test_fiber.cpp)
add_dependencies(test_fiber sylar)
force_redefine_file_macro_for_sources(test_fiber) # 重定义 宏 __FILE__
target_link_libraries(test_fiber ${LIBS})


add_executable(test_scheduler tests/test_scheduler.cpp)
add_dependencies(test_scheduler sylar)
force_redefine_file_macro_for_sources(test_scheduler) # 重定义 宏 __FILE__
target_link_libraries(test_scheduler ${LIBS})

add_executable(test_iomanager tests/test_iomanager.cpp)
add_dependencies(test_iomanager sylar)
force_redefine_file_macro_for_sources(test_iomanager) # 重定义 宏 __FILE__
target_link_libraries(test_iomanager ${LIBS})

add_executable(test_timer tests/test_timer.cpp)
add_dependencies(test_timer sylar)
force_redefine_file_macro_for_sources(test_timer) # 重定义 宏 __FILE__
target_link_libraries(test_timer ${LIBS})

add_executable(test_hook tests/test_hook.cpp)
add_dependencies(test_hook sylar)
force_redefine_file_macro_for_sources(test_hook) # 重定义 宏 __FILE__
target_link_libraries(test_hook ${LIBS})

add_executable(test_address tests/test_address.cpp)
add_dependencies(test_address sylar)
force_redefine_file_macro_for_sources(test_address) # 重定义 宏 __FILE__
target_link_libraries(test_address ${LIBS})

add_executable(test_socket tests/test_socket.cpp)
add_dependencies(test_socket sylar)
force_redefine_file_macro_for_sources(test_socket) # 重定义 宏 __FILE__
target_link_libraries(test_socket ${LIBS})

add_executable(test_bytearray tests/test_bytearray.cpp)
add_dependencies(test_bytearray sylar)
force_redefine_file_macro_for_sources(test_bytearray) # 重定义 宏 __FILE__
target_link_libraries(test_bytearray ${LIBS})

add_executable(test_http tests/test_http.cpp)
add_dependencies(test_http sylar)
force_redefine_file_macro_for_sources(test_http) # 重定义 宏 __FILE__
target_link_libraries(test_http ${LIBS})

add_executable(test_http_parser tests/test_http_parser.cpp)
add_dependencies(test_http_parser sylar)
force_redefine_file_macro_for_sources(test_http_parser) # 重定义 宏 __FILE__
target_link_libraries(test_http_parser ${LIBS})

add_executable(test_tcpserver tests/test_tcpserver.cpp)
add_dependencies(test_tcpserver sylar)
force_redefine_file_macro_for_sources(test_tcpserver) # 重定义 宏 __FILE__
target_link_libraries(test_tcpserver ${LIBS})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)