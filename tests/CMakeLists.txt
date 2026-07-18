find_package(GTest QUIET)
if(NOT GTest_FOUND)
    message(STATUS "GTest not found — skipping ds_core_tests target. Install libgtest-dev to enable.")
    return()
endif()

add_executable(ds_core_tests
    test_clip.cpp
    test_track.cpp
    test_timeline.cpp
)
target_link_libraries(ds_core_tests PRIVATE ds_core GTest::gtest GTest::gtest_main)
add_test(NAME ds_core_tests COMMAND ds_core_tests)
