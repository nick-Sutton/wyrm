add_executable(wyrm-tests
    test.cpp
)

target_link_libraries(wyrm-tests
    PRIVATE
        Catch2::Catch2WithMain
        wyrm-common
)