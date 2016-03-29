set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")

include_directories(../src)
include_directories(../src/http)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

add_executable(lnet_ab lnet_ab.cpp)
target_link_libraries(lnet_ab lnet lnethttp)

