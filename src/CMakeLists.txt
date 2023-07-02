add_subdirectory(networking)
add_subdirectory(packets)

add_compile_options(-fmessage-length=80 -fno-diagnostics-show-option
-fstack-protector-all -g -O3 -Walloc-zero -Walloca
-Wduplicated-cond -Wduplicated-branches
-Werror -Wfatal-errors -Winit-self -Wlogical-op
-Wshadow -Wunused-const-variable=1)
add_executable(${PROJECT_NAME} main.c)

target_link_libraries(${PROJECT_NAME} networking)
target_link_libraries(${PROJECT_NAME} serverbound_packets)
target_link_libraries(${PROJECT_NAME} clientbound_packets)