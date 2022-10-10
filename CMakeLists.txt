cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
enable_testing()

project("depth" VERSION 1.0.0)
set(CMAKE_PROJECT_DESCRIPTION "Create Auto-Stereogram Images")
set(CMAKE_PROJECT_HOMEPAGE_URL "https://www.shaidin.com/depth")
set(CMAKE_PROJECT_VERSION_MAJOR 1)
set(CMAKE_PROJECT_VERSION_MINOR 0)
set(CMAKE_PROJECT_VERSION_PATCH 0)

set(CPACK_PACKAGE_VENDOR "virtualmatador")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}")
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_VENDOR}")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libpng16-16")
include(CPack)

add_custom_target("version" ALL
    COMMAND "${CMAKE_COMMAND}" -E echo ${CMAKE_PROJECT_VERSION} >
        "${CMAKE_CURRENT_BINARY_DIR}/version.txt"
)

add_subdirectory("extern/cli")
add_subdirectory("extern/stereogram")
add_subdirectory("src")
