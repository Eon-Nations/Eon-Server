cmake_minimum_required(VERSION 3.0.0)
project(Eon-Server VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()

add_executable(Eon-Server src/server.c src/varint/varint.c src/packets/packet_reader.h src/packets/status_packet.h)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
