/**
 * @file working_test.cpp
 * @brief Working test for ExclusionParser library using correct API
 * 
 * This file contains a basic test that validates core functionality
 * using the actual API methods available in the ExclusionParser library.
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
 * @brief Working test runner that validates basic functionality
 */
class WorkingTest {
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
# Working test exclusion file
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
     * @brief Test basic data structure functionality
     */
    void testDataStructures() {
        cout << "\n=== Testing Data Structures ===" << endl;

        try {
            // Test ExclusionData creation
            ExclusionParser::ExclusionData data("test_file.el");
            logResult("ExclusionData creation", true);

            // Test scope creation using getOrCreateScope
            auto& instanceScope = data.getOrCreateScope("tb.test.instance", "123456789", false);
            instanceScope.scopeName = "tb.test.instance";
            instanceScope.isModule = false;
            instanceScope.checksum = "123456789";

            auto& moduleScope = data.getOrCreateScope("tb.test.module", "987654321", true);
            moduleScope.scopeName = "tb.test.module";
            moduleScope.isModule = true;
            moduleScope.checksum = "987654321";

            // Check scope count
            size_t scopeCount = data.getScopeCount();
            logResult("Scope count", scopeCount == 2,
                     "Expected 2, got " + to_string(scopeCount));

            // Test adding exclusions to scope
            ExclusionParser::BlockExclusion block;
            block.blockId = "101";
            block.checksum = "111111111";
            block.sourceCode = "test_signal = 1'b0;";
            block.annotation = "Test block exclusion";

            instanceScope.blockExclusions[block.blockId] = block;

            ExclusionParser::ToggleExclusion toggle;
            toggle.direction = ExclusionParser::ToggleDirection::ONE_TO_ZERO;
            toggle.signalName = "test_clock";
            toggle.netDescription = "net test_clock";

            instanceScope.toggleExclusions[toggle.signalName].push_back(toggle);

            // Check total exclusions
            size_t totalExclusions = data.getTotalExclusionCount();
            logResult("Total exclusions", totalExclusions >= 2,
                     "Expected >=2, got " + to_string(totalExclusions));

            cout << "  Created " << scopeCount << " scopes with " 
                 << totalExclusions << " exclusions" << endl;

        } catch (const exception& e) {
            logResult("Data structures exception", false, e.what());
        }
    }

    /**
     * @brief Test basic parsing functionality
     */
    void testBasicParsing() {
        cout << "\n=== Testing Basic Parsing ===" << endl;

        try {
            ExclusionParser::ExclusionParser parser;
            string testContent = createTestContent();
            
            auto result = parser.parseString(testContent, "working_test");
            
            logResult("String parsing", result.success, 
                     result.success ? "" : result.errorMessage);

            if (result.success) {
                auto data = parser.getData();
                
                size_t scopeCount = data->getScopeCount();
                logResult("Scope count", scopeCount == 2,
                         "Expected 2, got " + to_string(scopeCount));

                size_t totalExclusions = data->getTotalExclusionCount();
                logResult("Exclusion count", totalExclusions > 0,
                         "Got " + to_string(totalExclusions) + " exclusions");

                // Test direct access to scopes
                auto scopeIt = data->scopes.find("tb.test.core");
                logResult("Instance scope access", scopeIt != data->scopes.end());

                auto moduleIt = data->scopes.find("tb.test.wrapper");
                logResult("Module scope access", moduleIt != data->scopes.end());

                if (scopeIt != data->scopes.end()) {
                    const auto& scope = scopeIt->second;
                    logResult("Block exclusions present", !scope.blockExclusions.empty());
                    logResult("Toggle exclusions present", !scope.toggleExclusions.empty());
                    logResult("FSM exclusions present", !scope.fsmExclusions.empty());
                    logResult("Condition exclusions present", !scope.conditionExclusions.empty());
                }

                // Test exclusion counts by type
                auto countsByType = data->getExclusionCountsByType();
                logResult("Block count", countsByType[ExclusionParser::ExclusionType::BLOCK] > 0);
                logResult("Toggle count", countsByType[ExclusionParser::ExclusionType::TOGGLE] > 0);
                logResult("FSM count", countsByType[ExclusionParser::ExclusionType::FSM] > 0);
                logResult("Condition count", countsByType[ExclusionParser::ExclusionType::CONDITION] > 0);

                cout << "  Parsed " << scopeCount << " scopes with " 
                     << totalExclusions << " total exclusions" << endl;
                cout << "    Blocks: " << countsByType[ExclusionParser::ExclusionType::BLOCK]
                     << ", Toggles: " << countsByType[ExclusionParser::ExclusionType::TOGGLE]
                     << ", FSMs: " << countsByType[ExclusionParser::ExclusionType::FSM]
                     << ", Conditions: " << countsByType[ExclusionParser::ExclusionType::CONDITION] << endl;
            }

        } catch (const exception& e) {
            logResult("Basic parsing exception", false, e.what());
        }
    }

    /**
     * @brief Test writing functionality and round-trip
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
            
            logResult("Write to string", !writtenContent.empty());

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
            string testFileName = "working_test_output.el";
            auto fileResult = writer.writeFile(testFileName, *data);
            
            logResult("Write to file", fileResult.success,
                     fileResult.success ? "" : fileResult.errorMessage);

            if (fileResult.success) {
                // Check if file exists and has content
                ifstream testFile(testFileName);
                if (testFile.good()) {
                    testFile.seekg(0, ios::end);
                    size_t fileSize = testFile.tellg();
                    logResult("Output file content", fileSize > 0,
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
     * @brief Test real exclusion files if available
     */
    void testRealFiles() {
        cout << "\n=== Testing Real Files ===" << endl;

        try {
            string exclusionDir = "../exclusion";
            
            // Check if we can find any .el files
            bool foundElFiles = false;
            int filesProcessed = 0;
            int successfulParses = 0;
            
            // Try to process the first few .el files we can find
            ExclusionParser::ExclusionParser parser;
            
            // Sample file names from the workspace
            vector<string> sampleFiles = {
                "../exclusion/dcio.el",
                "../exclusion/dlpc.el", 
                "../exclusion/pwrseq.el"
            };
            
            for (const auto& filename : sampleFiles) {
                ifstream file(filename);
                if (file.good()) {
                    foundElFiles = true;
                    filesProcessed++;
                    
                    cout << "  Testing file: " << filename << endl;
                    
                    auto result = parser.parseFile(filename);
                    if (result.success) {
                        successfulParses++;
                        cout << "    Parsed " << result.exclusionsParsed << " exclusions" << endl;
                    } else {
                        cout << "    Parse failed: " << result.errorMessage << endl;
                    }
                    
                    // Limit to first 3 files for testing
                    if (filesProcessed >= 3) break;
                }
                file.close();
            }
            
            logResult("Real .el files found", foundElFiles);
            
            if (foundElFiles) {
                logResult("Real file parsing", successfulParses > 0,
                         to_string(successfulParses) + "/" + to_string(filesProcessed) + " files parsed");
                
                if (successfulParses > 0) {
                    auto data = parser.getData();
                    auto& manager = parser.getDataManager();
                    auto stats = manager.getStatistics();
                    
                    cout << "  Real file statistics:" << endl;
                    cout << "    Total scopes: " << stats.totalScopes << endl;
                    cout << "    Total exclusions: " << stats.totalExclusions << endl;
                    cout << "    Files processed: " << successfulParses << endl;
                }
            } else {
                cout << "  No .el files found in expected locations" << endl;
            }

        } catch (const exception& e) {
            logResult("Real file test exception", false, e.what());
        }
    }

    /**
     * @brief Run all tests and display results
     */
    void runAllTests() {
        cout << "======================================" << endl;
        cout << "    WORKING EXCLUSION PARSER TEST" << endl;
        cout << "======================================" << endl;

        testDataStructures();
        testBasicParsing();
        testBasicWriting();
        testDataManager();
        testRealFiles();

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
            cout << "Successfully validated:" << endl;
            cout << "  ✓ Data structure creation and manipulation" << endl;
            cout << "  ✓ Parsing of all exclusion types (Block, Toggle, FSM, Condition)" << endl;
            cout << "  ✓ Writing exclusions back to .el format" << endl;
            cout << "  ✓ Round-trip parsing integrity" << endl;
            cout << "  ✓ Data manager search and statistics" << endl;
            if (passedTests > 20) {
                cout << "  ✓ Real exclusion file processing" << endl;
            }
        } else {
            cout << "\n⚠️ Some tests failed. Check the results above." << endl;
        }
        
        cout << "\nLibrary compiled successfully with Visual Studio " << _MSC_VER << endl;
        cout << "Using C++ standard: " << __cplusplus << endl;
    }
};

/**
 * @brief Main function - entry point for working test
 */
int main() {
    try {
        cout << "ExclusionParser Library - Working Test Suite" << endl;
        cout << "Compiled with Visual Studio " << _MSC_VER << endl;
        cout << "C++ Standard: " << __cplusplus << endl << endl;

        WorkingTest tester;
        tester.runAllTests();
        
        cout << "\nPress Enter to continue...";
        cin.get();
        
        return 0;
    } catch (const exception& e) {
        cerr << "Fatal test error: " << e.what() << endl;
        return 1;
    }
}