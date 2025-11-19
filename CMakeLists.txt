cmake_minimum_required(VERSION 3.15)
project(embedded_motor_pid_controller VERSION 1.0.0 LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")
set(CMAKE_C_FLAGS_DEBUG "-g -O0")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")

# Option to build tests
option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_DEMO "Build PID demo application" ON)

# PID Controller library
add_library(pid_controller STATIC
    firmware/src/pid.c
)

target_include_directories(pid_controller PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/firmware/include
)

# Motor model library (for simulation)
add_library(motor_model STATIC
    firmware/src/motor.c
)

target_include_directories(motor_model PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/firmware/include
)

# Demo application
if(BUILD_DEMO)
    add_executable(pid_demo
        firmware/src/main.c
    )

    target_link_libraries(pid_demo PRIVATE
        pid_controller
        motor_model
    )

    # Link math library on Unix systems
    if(UNIX)
        target_link_libraries(pid_demo PRIVATE m)
    endif()
endif()

# Unit tests
if(BUILD_TESTS)
    # Unity testing framework
    add_library(unity STATIC
        tests/Unity/src/unity.c
    )

    target_include_directories(unity PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/Unity/src
    )

    # PID unit tests
    add_executable(test_pid
        tests/test_pid.c
    )

    target_link_libraries(test_pid PRIVATE
        pid_controller
        unity
    )

    # Link math library on Unix systems
    if(UNIX)
        target_link_libraries(test_pid PRIVATE m)
    endif()

    # Enable testing
    enable_testing()
    add_test(NAME PID_Tests COMMAND test_pid)

    # Add custom target to run tests
    add_custom_target(run_tests
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        DEPENDS test_pid
        COMMENT "Running unit tests..."
    )
endif()

# Installation
install(TARGETS pid_controller
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES
    firmware/include/pid.h
    DESTINATION include
)

# Print configuration summary
message(STATUS "")
message(STATUS "Configuration Summary:")
message(STATUS "  Project: ${PROJECT_NAME} ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "  Build tests: ${BUILD_TESTS}")
message(STATUS "  Build demo: ${BUILD_DEMO}")
message(STATUS "")
