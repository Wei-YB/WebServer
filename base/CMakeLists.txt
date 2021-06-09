aux_source_directory(. baseCodeFile)

add_library(base STATIC ${baseCodeFile})

macro(add_base_test_target TEST_TARGET)
    add_executable("test_${TEST_TARGET}" "./test/test_${TEST_TARGET}.cpp")
    target_link_libraries("test_${TEST_TARGET}" base gtest gtest_main pthread)
endmacro(add_example_target)

add_base_test_target(Timestamp)
add_base_test_target(ThisThread)
add_base_test_target(Mutex)
add_base_test_target(LogFile)
