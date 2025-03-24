#
# Builds 2 targets (1) async_logger and (2) async_logger_test
#

cmake_minimum_required(VERSION 3.0)

project(async_logger)

set(CMAKE_CXX_STANDARD 17)

# Add the source files
set( LIB_SRC 
    src/async_logger.cpp
    src/async_logger.h )
    
# Add the test files
set( TEST_SRC
    test/async_logger_test.cpp )


# Add the library target
add_library( async_logger ${LIB_SRC} )

# Add the test target
add_executable( async_logger_test ${TEST_SRC} )
target_include_directories( async_logger_test PRIVATE src )
target_link_libraries( async_logger_test async_logger )

