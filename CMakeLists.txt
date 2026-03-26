cmake_minimum_required(VERSION 3.28)
project(comi_voj C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(USE_NATIVE_ARCH "Compile with -march=native" ON)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -fomit-frame-pointer)
    if(USE_NATIVE_ARCH)
        add_compile_options(-march=native)
    else()
        add_compile_options(-mtune=generic)
    endif()
endif()

include(FetchContent)
FetchContent_Declare(
    xoshiro256
    GIT_REPOSITORY https://github.com/plOloV2/Xoshiro256.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(xoshiro256)

find_package(OpenMP REQUIRED)

add_executable(${PROJECT_NAME}
    src/UI/TUI_func.c
    src/data_operations/data_prepare.c
    src/data_operations/stack_op.c
    src/data_operations/save_data.c
    src/algorithms/rand_seq.c
    src/algorithms/nearest_neighbour_fast.c
    src/algorithms/nearest_neighbour_stack.c
    src/algorithms/repetitive_nearest_neighbour.c
    src/algorithms/brute_force.c
    src/runner.c
    src/main.c
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)

target_include_directories(${PROJECT_NAME} PRIVATE inc)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pipe)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported)
    if(ipo_supported)
        set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
        set_property(TARGET xoshiro256 PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
endif()

if(WIN32)

    target_compile_options(${PROJECT_NAME} PRIVATE -fopenmp)

    target_link_libraries(${PROJECT_NAME} PRIVATE 
        -static
        -static-libgcc
        -lgomp 
        -lwinpthread 
        xoshiro256
    )
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenMP::OpenMP_C xoshiro256)
endif()
