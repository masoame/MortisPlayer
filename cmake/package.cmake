set(Qt6_ROOT "D:/Qt/6.9.0/msvc2022_64")
set(Qt6_DIR "${Qt6_ROOT}/lib/cmake/Qt6") 

set(ENV{http_proxy} "http://127.0.0.1:10808")
set(ENV{https_proxy} "http://127.0.0.1:10808")

include(FetchContent)

FetchContent_Declare(
    MortisUtils
    GIT_REPOSITORY  https://github.com/masoame/MortisUtils.git
)
FetchContent_MakeAvailable(MortisUtils)

FetchContent_Declare(
    curl
    GIT_REPOSITORY  https://github.com/curl/curl.git
    GIT_TAG curl-8_11_0
    CMAKE_ARGS -DCMAKE_USE_OPENSSL=OFF
)

FetchContent_MakeAvailable(curl)

FetchContent_Declare(
    SDL
    GIT_REPOSITORY  https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.30.9
)
FetchContent_MakeAvailable(SDL)
include(cmake/external_library/ffmpeg_config.cmake)
