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

set (HEADERS
    lnethttp.h
    httputil.h
    http_parser.h
    httpparser.h
    httpconnection.h
    httprequest.h
    httpresponse.h
    httpserver.h
    httpclient.h
    )


set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

add_definitions(-DHTTP_PARSER_STRICT=0)

include_directories(../)

add_library(lnethttp ${SRCS})
target_link_libraries(lnethttp lnet)

install(TARGETS lnethttp DESTINATION lib)
install(FILES ${HEADERS} DESTINATION include/lnet/http)
