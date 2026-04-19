cmake_minimum_required(VERSION 3.14)
project(banking)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(banking STATIC
    Account.cpp
    Transaction.cpp
)

option(BUILD_TESTS "Build tests" ON)
option(COVERAGE "Enable coverage reporting" OFF)

if(COVERAGE)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options(-O0 -g --coverage -fprofile-arcs -ftest-coverage)
        add_link_options(--coverage -lgcov)
    endif()
endif()

if(BUILD_TESTS)
    enable_testing()
    include(FetchContent)
    
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    FetchContent_MakeAvailable(googletest)
    
    add_executable(banking_tests 
        tests/test_account.cpp 
        tests/test_transaction.cpp
        tests/test_mocks.cpp
    )
    target_link_libraries(banking_tests banking gtest_main gmock)
    target_include_directories(banking_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    
    add_test(NAME banking_tests COMMAND banking_tests)
endif()
