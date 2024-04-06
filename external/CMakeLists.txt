include(FetchContent)

FetchContent_Declare(
        etl
        GIT_REPOSITORY "https://github.com/ETLCPP/etl"
        GIT_TAG "20.35.14"
)
FetchContent_Declare(
        fpm
        GIT_REPOSITORY "https://github.com/MikeLankamp/fpm"
        GIT_TAG "v1.1.0"
)
FetchContent_Declare(
        ggpo
        GIT_REPOSITORY "https://github.com/pond3r/ggpo"
        GIT_TAG 7ddadef8546a7d99ff0b3530c6056bc8ee4b9c0a
)
FetchContent_Declare(
        raylib
        GIT_REPOSITORY "https://github.com/raysan5/raylib"
        GIT_TAG "4.5.0"
)
FetchContent_Declare(
        tomlplusplus
        GIT_REPOSITORY "https://github.com/marzer/tomlplusplus"
        GIT_TAG "v3.3.0"
)
FetchContent_MakeAvailable(etl fpm ggpo raylib tomlplusplus)

add_library(RBST_deps INTERFACE)
target_link_libraries(RBST_deps INTERFACE
    etl
    fpm
    GGPO
    raylib
    tomlplusplus::tomlplusplus
)