set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(SRCS
    http_parser.c
    httpclient.cpp
    httpconnection.cpp
    httpconnector.cpp
    httpparser.cpp
    httprequest.cpp
    httpresponse.cpp
    httpserver.cpp
    httputil.cpp
    )

add_definitions(-DHTTP_PARSER_STRICT=0)

include_directories(../)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

add_library(lnethttp ${SRCS})
target_link_libraries(lnethttp lnet)
