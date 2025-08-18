set(Qt6_ROOT "D:/Qt/6.9.0/msvc2022_64")
set(Qt6_DIR "${Qt6_ROOT}/lib/cmake/Qt6") 

set(ENV{http_proxy} "http://127.0.0.1:10808")
set(ENV{https_proxy} "http://127.0.0.1:10808")

include(FetchContent)

FetchContent_Declare(
    Mortis
    GIT_REPOSITORY  https://github.com/masoame/Mortis.git
)

FetchContent_MakeAvailable(Mortis)

message("---------------------------------start_curl---------------------------------")

#set(OPENSSL_ROOT_DIR "D:/code/make/openssl")
#find_package(OpenSSL REQUIRED COMPONENTS Crypto SSL )

FetchContent_Declare(
    zlib
    GIT_REPOSITORY  https://github.com/madler/zlib.git
    GIT_TAG v1.3.1
)
FetchContent_MakeAvailable(zlib)


set(ZLIB_INCLUDE_DIR ${CMAKE_BINARY_DIR}/_deps/zlib-src/)
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(ZLIB_LIBRARY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib/zlibd.lib)
else()
    set(ZLIB_LIBRARY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib/zlib.lib)
endif()

file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/zconf.h)
file(COPY ${CMAKE_BINARY_DIR}/_deps/zlib-build/zconf.h DESTINATION ${ZLIB_INCLUDE_DIR})

FetchContent_Declare(
    libssh2
    GIT_REPOSITORY https://github.com/libssh2/libssh2.git
    GIT_TAG libssh2-1.11.1
)
FetchContent_MakeAvailable(libssh2)



set(LIBSSH2_INCLUDE_DIR ${CMAKE_BINARY_DIR}/_deps/libssh2-src/include/)
set(LIBSSH2_LIBRARY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib/libssh2.lib)


FetchContent_Declare(
    curl
    GIT_REPOSITORY  https://github.com/curl/curl.git
    GIT_TAG curl-8_11_0
    CMAKE_ARGS -DCMAKE_USE_OPENSSL=ON
)

FetchContent_MakeAvailable(curl)

message("----------------------------------end_curl----------------------------------")

FetchContent_Declare(
    SDL
    GIT_REPOSITORY  https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.30.9
)

FetchContent_MakeAvailable(SDL)


add_subdirectory(external_library)
