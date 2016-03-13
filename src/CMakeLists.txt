set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")

set(SRCS
    acceptor.cpp
    address.cpp
    connection.cpp
    ioloop.cpp
    logger.cpp
    notifier.cpp
    poller.cpp
    process.cpp
    signaler.cpp
    sockutil.cpp
    stringutil.cpp
    tcpserver.cpp
    timer.cpp
    timingwheel.cpp
    polarssl/md5.c
    polarssl/sha1.c
    polarssl/base64.c
    )

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

add_library(lnet ${SRCS})
target_link_libraries(lnet rt)

add_subdirectory(http)
