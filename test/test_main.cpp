/**
 * @file test_main.cpp
 * @brief Main test file for Google Test
 * 
 * This file contains the main entry point for running all tests
 * in the Exclusion Coverage Parser test suite.
 * 
 * @author ExclusionCoverageParser
 * @version 1.0.0
 * @date 2025
 */

#include <gtest/gtest.h>
#include <iostream>

/**
 * @brief Main entry point for tests
 * 
 * Initializes Google Test and runs all test cases.
 * 
 * @param argc Number of command line arguments
 * @param argv Command line arguments
 * @return Test execution result (0 for success)
 */
int main(int argc, char** argv) {
    std::cout << "Starting Exclusion Coverage Parser Test Suite..." << std::endl;
    
    // Initialize Google Test
    ::testing::InitGoogleTest(&argc, argv);
    
    // Run all tests
    int result = RUN_ALL_TESTS();
    
    if (result == 0) {
        std::cout << "All tests passed successfully!" << std::endl;
    } else {
        std::cout << "Some tests failed. Please check the output above." << std::endl;
    }
    
    return result;
}