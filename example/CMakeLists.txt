set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

include_directories(../src)
include_directories(../src/http)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

add_executable(httpserver_example httpserver_example.cpp)
target_link_libraries(httpserver_example lnet lnethttp)

add_executable(httpclient_example httpclient_example.cpp)
target_link_libraries(httpclient_example lnet lnethttp)

