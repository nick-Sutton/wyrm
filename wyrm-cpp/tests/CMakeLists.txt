add_executable(wyrm-cpp-tests
    test_serialization.cpp
)

target_link_libraries(wyrm-cpp-tests
    PRIVATE
        wyrm-cpp
        Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(wyrm-cpp-tests)
