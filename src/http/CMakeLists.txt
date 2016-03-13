set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")

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

add_library(lnet_http ${SRCS})
target_link_libraries(lnet_http lnet)
