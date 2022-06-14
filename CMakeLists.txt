cmake_minimum_required(VERSION 3.13)
project(hwtree VERSION 0.1.0.0
  DESCRIPTION "hamming space data structure to store binary patterns for nearest neighbor retrieval")




set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(LIB_SOURCES src/hwt.cpp src/hwtnode.cpp src/hwtree.cpp)


add_library(hwtree STATIC ${LIB_SOURCES} )
target_compile_options(hwtree PUBLIC -g -Ofast -Wall)
target_include_directories(hwtree PUBLIC include/)


add_executable(testhwtree tests/test_hwtree.cpp)
target_compile_options(testhwtree PUBLIC -g -O0 -Wall)
target_link_libraries(testhwtree hwtree)

add_executable(testhwt tests/test_hwt.cpp)
target_compile_options(testhwt PUBLIC -g -O0 -Wall)
target_link_libraries(testhwt hwtree)

add_executable(runhwtree tests/run_hwtree.cpp)
target_compile_options(runhwtree PUBLIC -g -Ofast -Wall)
target_link_libraries(runhwtree hwtree)

include(CTest)
add_test(NAME test1 COMMAND testhwtree)
add_test(NAME test2 COMMAND testhwt)

install(TARGETS hwtree
  ARCHIVE DESTINATION lib
  PUBLIC_HEADER DESTINATION include)
