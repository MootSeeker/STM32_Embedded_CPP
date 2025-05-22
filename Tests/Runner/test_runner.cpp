#define CATCH_CONFIG_RUNNER // We want to control main ourselves
#include "catch.hpp"        // Catch2 header
#include "mock_stm32l4xx.h" // For reset_all_mocks declaration

int main(int argc, char* argv[]) {
    // reset_all_mocks() is called before running tests.
    // This ensures a clean state for the mock hardware at the start of the test session.
    // For per-test setup/teardown, Catch2 fixtures or event listeners can be used.
#ifdef UNIT_TESTING
    reset_all_mocks();
#endif

    int result = Catch::Session().run(argc, argv);
    return result;
}