/**
 * @file simple_test.cpp
 * @brief Simple working test for ExclusionParser library
 * 
 * This file contains a basic test that validates core functionality
 * without complex dependencies that might cause compilation issues.
 * 
 * @author ExclusionParser Library
 * @date 2025-09-13
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>

// Include our library headers
#include "../include/ExclusionTypes.h"
#include "../include/ExclusionParser.h"
#include "../include/ExclusionWriter.h"
#include "../include/ExclusionData.h"

using namespace std;

/**
 * @brief Simple test runner that validates basic functionality
 */
class SimpleTest {
private:
    int totalTests = 0;
    int passedTests = 0;

    void logResult(const string& testName, bool passed, const string& details = "") {
        totalTests++;
        if (passed) {
            passedTests++;
            cout << "✓ PASS: " << testName << endl;
        } else {
            cout << "✗ FAIL: " << testName;
            if (!details.empty()) {
                cout << " - " << details;
            }
            cout << endl;
        }
    }

public:
    /**
     * @brief Creates simple test exclusion content
     */
    string createTestContent() {
        return R"(
# Simple test exclusion file
INSTANCE tb.test.core "123456789"

    ANNOTATION: "Test block exclusion"
    Block 101 "111111111" "test_signal = 1'b0;"
    
    Block 102 "222222222" "reset_state = IDLE;"

    Toggle 1to0 test_clock "net test_clock"
    Toggle test_data [7:0] "net test_data[7:0]"

    Fsm state "333333333"
    Transition IDLE->ACTIVE "0->1"

    Condition 1 "444444444" "(enable && !reset) 1 -1" (1 "01")

MODULE tb.test.wrapper "987654321"

    Block 201 "555555555" "output_enable = 1;"
    Toggle 0to1 ready_signal "net ready_signal"
)";
    }

    /**
     * @brief Test basic parsing functionality
     */
    void testBasicParsing() {
        cout << "\n=== Testing Basic Parsing ===" << endl;

        try {
            ExclusionParser::ExclusionParser parser;
            string testContent = createTestContent();
            
            auto result = parser.parseString(testContent, "simple_test");
            
            logResult("String parsing", result.success, 
                     result.success ? "" : result.errorMessage);

            if (result.success) {
                auto data = parser.getData();
                
                size_t scopeCount = data->getScopeCount();
                logResult("Scope count check", scopeCount == 2,
                         "Expected 2, got " + to_string(scopeCount));

                size_t totalExclusions = data->getTotalExclusionCount();
                logResult("Exclusion count check", totalExclusions > 0,
                         "Got " + to_string(totalExclusions) + " exclusions");

                // Test scope retrieval
                auto instanceScope = data->getScope("tb.test.core");
                logResult("Instance scope found", instanceScope != nullptr);

                auto moduleScope = data->getScope("tb.test.wrapper");
                logResult("Module scope found", moduleScope != nullptr);

                if (instanceScope) {
                    logResult("Block exclusions present", 
                             !instanceScope->blockExclusions.empty());
                    logResult("Toggle exclusions present", 
                             !instanceScope->toggleExclusions.empty());
                    logResult("FSM exclusions present", 
                             !instanceScope->fsmExclusions.empty());
                    logResult("Condition exclusions present", 
                             !instanceScope->conditionExclusions.empty());
                }

                cout << "  Parsed " << scopeCount << " scopes with " 
                     << totalExclusions << " total exclusions" << endl;
            }

        } catch (const exception& e) {
            logResult("Basic parsing exception", false, e.what());
        }
    }

    /**
     * @brief Test writing functionality
     */
    void testBasicWriting() {
        cout << "\n=== Testing Basic Writing ===" << endl;

        try {
            // First parse test content
            ExclusionParser::ExclusionParser parser;
            string testContent = createTestContent();
            auto parseResult = parser.parseString(testContent, "write_test");

            if (!parseResult.success) {
                logResult("Parse for writing test", false, parseResult.errorMessage);
                return;
            }

            auto data = parser.getData();

            // Test writing to string
            ExclusionParser::ExclusionWriter writer;
            string writtenContent = writer.writeToString(*data);
            
            logResult("Write to string", !writtenContent.empty(),
                     writtenContent.empty() ? "Output is empty" : "");

            if (!writtenContent.empty()) {
                cout << "  Written content length: " << writtenContent.length() 
                     << " characters" << endl;

                // Test round-trip parsing
                ExclusionParser::ExclusionParser roundTripParser;
                auto roundTripResult = roundTripParser.parseString(writtenContent, "round_trip");

                logResult("Round-trip parsing", roundTripResult.success,
                         roundTripResult.success ? "" : roundTripResult.errorMessage);

                if (roundTripResult.success) {
                    auto roundTripData = roundTripParser.getData();
                    
                    size_t originalScopes = data->getScopeCount();
                    size_t roundTripScopes = roundTripData->getScopeCount();
                    
                    logResult("Round-trip scope count", originalScopes == roundTripScopes,
                             "Original: " + to_string(originalScopes) + 
                             ", Round-trip: " + to_string(roundTripScopes));

                    size_t originalExclusions = data->getTotalExclusionCount();
                    size_t roundTripExclusions = roundTripData->getTotalExclusionCount();
                    
                    logResult("Round-trip exclusion count", originalExclusions == roundTripExclusions,
                             "Original: " + to_string(originalExclusions) + 
                             ", Round-trip: " + to_string(roundTripExclusions));

                    cout << "  Round-trip verified: " << originalScopes << " scopes, "
                         << originalExclusions << " exclusions" << endl;
                }
            }

            // Test file writing
            string testFileName = "simple_test_output.el";
            auto fileResult = writer.writeFile(testFileName, *data);
            
            logResult("Write to file", fileResult.success,
                     fileResult.success ? "" : fileResult.errorMessage);

            if (fileResult.success) {
                // Check if file exists and has content
                ifstream testFile(testFileName);
                if (testFile.good()) {
                    testFile.seekg(0, ios::end);
                    size_t fileSize = testFile.tellg();
                    logResult("Output file has content", fileSize > 0,
                             "File size: " + to_string(fileSize) + " bytes");
                    
                    cout << "  Output file created: " << testFileName 
                         << " (" << fileSize << " bytes)" << endl;
                } else {
                    logResult("Output file readable", false, "Could not read output file");
                }
                testFile.close();

                // Clean up
                remove(testFileName.c_str());
            }

        } catch (const exception& e) {
            logResult("Basic writing exception", false, e.what());
        }
    }

    /**
     * @brief Test data manager functionality
     */
    void testDataManager() {
        cout << "\n=== Testing Data Manager ===" << endl;

        try {
            ExclusionParser::ExclusionParser parser;
            string testContent = createTestContent();
            auto result = parser.parseString(testContent, "manager_test");

            if (!result.success) {
                logResult("Parse for manager test", false, result.errorMessage);
                return;
            }

            auto& manager = parser.getDataManager();

            // Test statistics
            auto stats = manager.getStatistics();
            logResult("Statistics generation", stats.totalScopes > 0 && stats.totalExclusions > 0,
                     "Scopes: " + to_string(stats.totalScopes) + 
                     ", Exclusions: " + to_string(stats.totalExclusions));

            // Test scope finding
            auto foundScope = manager.findScope("tb.test.core");
            logResult("Scope finding", foundScope != nullptr);

            // Test pattern matching
            auto matchingScopes = manager.findScopesMatching("tb.test.*");
            logResult("Pattern matching", matchingScopes.size() >= 2,
                     "Found " + to_string(matchingScopes.size()) + " matching scopes");

            cout << "  Manager statistics:" << endl;
            cout << "    Total scopes: " << stats.totalScopes << endl;
            cout << "    Total exclusions: " << stats.totalExclusions << endl;
            cout << "    Annotated exclusions: " << stats.annotatedExclusions << endl;

        } catch (const exception& e) {
            logResult("Data manager test exception", false, e.what());
        }
    }

    /**
     * @brief Test configuration options
     */
    void testConfiguration() {
        cout << "\n=== Testing Configuration ===" << endl;

        try {
            ExclusionParser::ExclusionParser parser;
            
            // Test parser configuration
            ExclusionParser::ParserConfig config;
            config.strictMode = false;
            config.preserveComments = true;
            parser.setConfig(config);

            string testContent = createTestContent();
            auto result = parser.parseString(testContent, "config_test");
            
            logResult("Parser configuration", result.success);

            if (result.success) {
                // Test writer configuration
                ExclusionParser::ExclusionWriter writer;
                ExclusionParser::WriterConfig writeConfig;
                writeConfig.includeComments = true;
                writeConfig.includeAnnotations = true;
                writeConfig.sortExclusions = true;
                writer.setConfig(writeConfig);

                auto data = parser.getData();
                string output = writer.writeToString(*data);
                
                logResult("Writer configuration", !output.empty());
                
                // Check that annotations are included
                bool hasAnnotations = output.find("ANNOTATION:") != string::npos;
                logResult("Annotations included", hasAnnotations);

                cout << "  Configuration test completed successfully" << endl;
            }

        } catch (const exception& e) {
            logResult("Configuration test exception", false, e.what());
        }
    }

    /**
     * @brief Run all tests and display results
     */
    void runAllTests() {
        cout << "======================================" << endl;
        cout << "    SIMPLE EXCLUSION PARSER TEST" << endl;
        cout << "======================================" << endl;

        testBasicParsing();
        testBasicWriting();
        testDataManager();
        testConfiguration();

        cout << "\n======================================" << endl;
        cout << "  TEST RESULTS SUMMARY" << endl;
        cout << "======================================" << endl;

        cout << "Total Tests: " << totalTests << endl;
        cout << "Passed: " << passedTests << endl;
        cout << "Failed: " << (totalTests - passedTests) << endl;
        
        if (totalTests > 0) {
            double successRate = (double(passedTests) / totalTests) * 100.0;
            cout << "Success Rate: " << successRate << "%" << endl;
        }

        cout << "======================================" << endl;

        if (passedTests == totalTests) {
            cout << "\n🎉 ALL TESTS PASSED! 🎉" << endl;
            cout << "The ExclusionParser library is working correctly!" << endl;
        } else {
            cout << "\n⚠️ Some tests failed. Check the results above." << endl;
        }
    }
};

/**
 * @brief Main function - entry point for simple testing
 */
int main() {
    try {
        cout << "ExclusionParser Library - Simple Test Suite" << endl;
        cout << "Built with Visual Studio " << _MSC_VER << endl;
        cout << "C++ Standard: " << __cplusplus << endl << endl;

        SimpleTest tester;
        tester.runAllTests();
        
        cout << "\nPress Enter to continue...";
        cin.get();
        
        return 0;
    } catch (const exception& e) {
        cerr << "Fatal test error: " << e.what() << endl;
        return 1;
    }
}