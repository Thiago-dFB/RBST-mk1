cmake_minimum_required(VERSION 3.29)

project( RBST_mk1
    LANGUAGES
        CXX
    VERSION
        1.0.1
)

add_subdirectory(src)
add_subdirectory(external)

target_link_libraries(RBST PRIVATE RBST_deps)

install(TARGETS RBST)

install( DIRECTORY "${CMAKE_SOURCE_DIR}/res/"
    DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}
)