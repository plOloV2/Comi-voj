cmake_minimum_required(VERSION 3.28)
project(comi_voj C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    xoshiro256
    GIT_REPOSITORY https://github.com/plOloV2/Xoshiro256.git
    GIT_TAG        main
)

FetchContent_MakeAvailable(xoshiro256)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pipe")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -DDEBUG_MSG")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -march=native -flto -fomit-frame-pointer")

find_package(OpenMP REQUIRED)

add_executable(${PROJECT_NAME}
    src/data_operations/data_prepare.c
    src/data_operations/stack_op.c
    src/algorithms/nearest_neighbour_fast.c
    src/algorithms/nearest_neighbour_stack.c
    src/algorithms/repetitive_nearest_neighbour.c
    src/algorithms/brute_force.c
    src/UI/TUI_func.c
    src/main.c
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)

target_include_directories(${PROJECT_NAME} PRIVATE inc)

target_link_libraries(${PROJECT_NAME} OpenMP::OpenMP_C m xoshiro256)
