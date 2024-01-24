cmake_minimum_required(VERSION 3.0.0)
project(Eon-Server VERSION 0.1.0 LANGUAGES C)

add_compile_options(-g3 -Wall -Wextra -Wpedantic -Werror)

add_subdirectory(src)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
