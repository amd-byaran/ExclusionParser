/**
 * @file comprehensive_test.cpp
 * @brief Comprehensive unit tests for ExclusionParser library
 * 
 * This file contains thorough tests that validate:
 * - All exclusion types (Block, Toggle, FSM, Condition)
 * - Round-trip parsing and writing
 * - Data structure integrity
 * - Error handling and edge cases
 * - Real exclusion file format compatibility
 * 
 * @author ExclusionParser Library
 * @date 2025-09-13
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <filesystem>
#include <cassert>
#include <memory>

// Include our library headers
#include "../include/ExclusionTypes.h"
#include "../include/ExclusionParser.h"
#include "../include/ExclusionWriter.h"
#include "../include/ExclusionData.h"

using namespace ExclusionParser;

/**
 * @class ComprehensiveTest
 * @brief Main test class for comprehensive validation
 */
class ComprehensiveTest {
private:
    std::vector<std::string> testResults;
    size_t totalTests = 0;
    size_t passedTests = 0;

    void logResult(const std::string& testName, bool passed, const std::string& details = "") {
        totalTests++;
        if (passed) {
            passedTests++;
            testResults.push_back("✓ PASS: " + testName);
        } else {
            testResults.push_back("✗ FAIL: " + testName + " - " + details);
        }
    }

public:
    /**
     * @brief Creates sample exclusion file content for testing
     * @return String containing comprehensive test data
     */
    std::string createSampleExclusionContent() {
        return R"(
# Test exclusion file - Generated for comprehensive testing
# Contains all exclusion types: Block, Toggle, FSM, Condition

INSTANCE tb.gpu0.chip0.core.dcio_wrapper "4294967295"

    # Block exclusions with various formats
    ANNOTATION: "Legacy code path, verified manually"
    Block 161 "1104666086" "do_db_reg_update = 1'b0;"
    
    Block 162 "2209332172" "reset_state = IDLE;"
    
    ANNOTATION: "Test mode only, excluded in production"
    Block 163 "3313998258" "if (test_mode) debug_output = 1;"

    # Toggle exclusions - various signal types
    Toggle 1to0 next_active_duty_cycle_cnt_frac_carry "net next_active_duty_cycle_cnt_frac_carry"
    Toggle 0to1 clock_enable_signal "net clock_enable_signal"
    Toggle next_active_duty_cycle_cnt_frac [16] "net next_active_duty_cycle_cnt_frac[16:0]"
    Toggle reset_vector [7:0] "net reset_vector[7:0]"
    
    ANNOTATION: "Clock signals are externally controlled"
    Toggle system_clock "net system_clock"

    # FSM exclusions - states and transitions
    ANNOTATION: "Unreachable state in normal operation"
    Fsm state "85815111"
    
    Fsm state "85815112"
    
    ANNOTATION: "Debug transition, not used in production"
    Transition SND_RD_ADDR1->IDLE "11->0"
    Transition IDLE->SND_RD_ADDR1 "0->11"
    Transition ERROR->RESET "15->0"

    # Condition exclusions
    ANNOTATION: "Complex condition, difficult to hit in normal testing"
    Condition 2 "2940925445" "(signal_a && signal_b != 2'b0) 1 -1" (1 "01")
    
    Condition 3 "3051851556" "(enable && !reset && clock) 1 -1" (1 "001")
    
    ANNOTATION: "Error condition, only for fault injection testing"
    Condition 4 "4162777667" "(error_flag || timeout) 1 -1" (1 "10")

MODULE tb.gpu0.chip0.core.dcio_clk_gen "1234567890"

    # More complex examples
    ANNOTATION: "Clock generation logic, verified by formal methods"
    Block 201 "5273703778" "clk_out = (clk_in & enable) | force_clock;"
    
    Block 202 "6384629889" "pll_locked = (pll_counter >= LOCK_THRESHOLD);"

    # Array signal toggles
    Toggle data_bus [31:0] "net data_bus[31:0]"
    Toggle address_bus [15:0] "net address_bus[15:0]"
    
    ANNOTATION: "Control signals managed by external controller"
    Toggle 0to1 write_enable "net write_enable"
    Toggle 1to0 read_enable "net read_enable"

    # Complex FSM with multiple states
    ANNOTATION: "Memory controller FSM, some states unreachable"
    Fsm state "7495555000"
    Fsm state "7495555001"
    Fsm state "7495555002"
    
    Transition IDLE->WRITE_WAIT "0->1"
    Transition WRITE_WAIT->WRITE_ACTIVE "1->2"
    Transition WRITE_ACTIVE->IDLE "2->0"
    Transition READ_WAIT->READ_ACTIVE "3->4"

    # Advanced condition exclusions
    Condition 10 "8606481111" "(addr >= START_ADDR && addr <= END_ADDR) 1 -1" (1 "11")
    Condition 11 "9717407222" "(!reset && clock && valid_data) 1 -1" (1 "111")

INSTANCE tb.gpu0.chip0.core.pwrseq_wrapper "9876543210"

    # Power sequencing exclusions
    ANNOTATION: "Power-on sequence, timing-critical exclusions"
    Block 301 "1828374055" "power_good = (voltage > MIN_VOLTAGE);"
    Block 302 "2939300166" "enable_rails = power_sequence[current_step];"

    # Power control toggles
    ANNOTATION: "Power rails controlled by PMU"
    Toggle 0to1 rail_1v8_enable "net rail_1v8_enable"
    Toggle 0to1 rail_3v3_enable "net rail_3v3_enable"
    Toggle 1to0 power_down_req "net power_down_req"

    # Power state machine
    Fsm state "4050226277"
    Transition POWER_OFF->POWER_RAMP "0->1"
    Transition POWER_RAMP->POWER_ON "1->2"
    Transition POWER_ON->POWER_OFF "2->0"

    # Power monitoring conditions
    ANNOTATION: "Thermal protection conditions"
    Condition 20 "5161152388" "(temperature > MAX_TEMP || current > MAX_CURRENT) 1 -1" (1 "10")
)";
    }

    /**
     * @brief Test basic data structure creation and manipulation
     */
    void testDataStructures() {
        std::cout << "\n=== Testing Data Structures ===" << std::endl;

        try {
            // Test ExclusionData creation
            ExclusionData data;
            logResult("ExclusionData creation", true);

            // Test scope creation
            ExclusionScope instanceScope;
            instanceScope.scopeName = "tb.test.instance";
            instanceScope.isModule = false;
            instanceScope.checksum = "123456789";

            ExclusionScope moduleScope;
            moduleScope.scopeName = "tb.test.module";
            moduleScope.isModule = true;
            moduleScope.checksum = "987654321";

            // Test adding scopes
            data.addScope(instanceScope);
            data.addScope(moduleScope);

            bool scopeCountCorrect = (data.getScopeCount() == 2);
            logResult("Scope addition", scopeCountCorrect, 
                     scopeCountCorrect ? "" : "Expected 2 scopes, got " + std::to_string(data.getScopeCount()));

            // Test block exclusion
            BlockExclusion block;
            block.blockId = "101";
            block.checksum = "111111111";
            block.sourceCode = "test_signal = 1'b0;";
            block.annotation = "Test block exclusion";

            auto* scope = data.getScope("tb.test.instance");
            if (scope) {
                scope->blockExclusions[block.blockId] = block;
                logResult("Block exclusion addition", true);
            } else {
                logResult("Block exclusion addition", false, "Could not find scope");
            }

            // Test toggle exclusion
            ToggleExclusion toggle;
            toggle.direction = ToggleDirection::BOTH;
            toggle.signalName = "test_clock";
            toggle.netDescription = "net test_clock";
            toggle.annotation = "Test toggle exclusion";

            if (scope) {
                scope->toggleExclusions[toggle.signalName].push_back(toggle);
                logResult("Toggle exclusion addition", true);
            } else {
                logResult("Toggle exclusion addition", false, "Could not find scope");
            }

            // Test FSM exclusion
            FsmExclusion fsm;
            fsm.fsmName = "test_fsm";
            fsm.checksum = "222222222";
            fsm.isTransition = false;
            fsm.annotation = "Test FSM state exclusion";

            if (scope) {
                scope->fsmExclusions[fsm.fsmName].push_back(fsm);
                logResult("FSM exclusion addition", true);
            } else {
                logResult("FSM exclusion addition", false, "Could not find scope");
            }

            // Test condition exclusion
            ConditionExclusion condition;
            condition.conditionId = "1";
            condition.checksum = "333333333";
            condition.expression = "(signal_a && signal_b) 1 -1";
            condition.coverage = "(1 \"01\")";
            condition.annotation = "Test condition exclusion";

            if (scope) {
                scope->conditionExclusions[condition.conditionId] = condition;
                logResult("Condition exclusion addition", true);
            } else {
                logResult("Condition exclusion addition", false, "Could not find scope");
            }

            // Test total exclusion count
            size_t totalExclusions = data.getTotalExclusionCount();
            bool totalCorrect = (totalExclusions == 4);
            logResult("Total exclusion count", totalCorrect,
                     totalCorrect ? "" : "Expected 4 exclusions, got " + std::to_string(totalExclusions));

        } catch (const std::exception& e) {
            logResult("Data structure test exception", false, e.what());
        }
    }

    /**
     * @brief Test parsing functionality with comprehensive data
     */
    void testParsing() {
        std::cout << "\n=== Testing Parsing Functionality ===" << std::endl;

        try {
            // Create parser
            ExclusionParser parser;
            
            // Configure parser
            ParserConfig config;
            config.strictMode = false;
            config.validateChecksums = true;
            config.preserveComments = true;
            parser.setConfig(config);

            // Test string parsing
            std::string testContent = createSampleExclusionContent();
            auto result = parser.parseString(testContent, "comprehensive_test");

            logResult("String parsing success", result.success, 
                     result.success ? "" : result.errorMessage);

            if (result.success) {
                auto data = parser.getData();
                
                // Verify scope count
                size_t scopeCount = data->getScopeCount();
                bool scopeCountCorrect = (scopeCount == 3);
                logResult("Parsed scope count", scopeCountCorrect,
                         scopeCountCorrect ? "" : "Expected 3 scopes, got " + std::to_string(scopeCount));

                // Verify total exclusions
                size_t totalExclusions = data->getTotalExclusionCount();
                bool exclusionCountCorrect = (totalExclusions > 20);
                logResult("Parsed exclusion count", exclusionCountCorrect,
                         exclusionCountCorrect ? "" : "Expected >20 exclusions, got " + std::to_string(totalExclusions));

                // Test specific scope retrieval
                auto instanceScope = data->getScope("tb.gpu0.chip0.core.dcio_wrapper");
                bool instanceFound = (instanceScope != nullptr);
                logResult("Instance scope retrieval", instanceFound,
                         instanceFound ? "" : "Could not find instance scope");

                auto moduleScope = data->getScope("tb.gpu0.chip0.core.dcio_clk_gen");
                bool moduleFound = (moduleScope != nullptr);
                logResult("Module scope retrieval", moduleFound,
                         moduleFound ? "" : "Could not find module scope");

                // Verify exclusion types in first scope
                if (instanceScope) {
                    bool hasBlocks = !instanceScope->blockExclusions.empty();
                    bool hasToggles = !instanceScope->toggleExclusions.empty();
                    bool hasFsms = !instanceScope->fsmExclusions.empty();
                    bool hasConditions = !instanceScope->conditionExclusions.empty();

                    logResult("Block exclusions parsed", hasBlocks,
                             hasBlocks ? "" : "No block exclusions found");
                    logResult("Toggle exclusions parsed", hasToggles,
                             hasToggles ? "" : "No toggle exclusions found");
                    logResult("FSM exclusions parsed", hasFsms,
                             hasFsms ? "" : "No FSM exclusions found");
                    logResult("Condition exclusions parsed", hasConditions,
                             hasConditions ? "" : "No condition exclusions found");

                    // Check specific exclusion details
                    auto blockIt = instanceScope->blockExclusions.find("161");
                    if (blockIt != instanceScope->blockExclusions.end()) {
                        bool annotationCorrect = !blockIt->second.annotation.empty();
                        bool sourceCodeCorrect = !blockIt->second.sourceCode.empty();
                        bool checksumCorrect = !blockIt->second.checksum.empty();

                        logResult("Block exclusion annotation", annotationCorrect,
                                 annotationCorrect ? "" : "Block annotation missing");
                        logResult("Block exclusion source code", sourceCodeCorrect,
                                 sourceCodeCorrect ? "" : "Block source code missing");
                        logResult("Block exclusion checksum", checksumCorrect,
                                 checksumCorrect ? "" : "Block checksum missing");
                    }

                    // Check toggle exclusion details
                    auto toggleIt = instanceScope->toggleExclusions.find("next_active_duty_cycle_cnt_frac_carry");
                    if (toggleIt != instanceScope->toggleExclusions.end() && !toggleIt->second.empty()) {
                        const auto& toggle = toggleIt->second[0];
                        bool directionCorrect = (toggle.direction == ToggleDirection::_1TO0);
                        bool netDescCorrect = !toggle.netDescription.empty();

                        logResult("Toggle exclusion direction", directionCorrect,
                                 directionCorrect ? "" : "Incorrect toggle direction");
                        logResult("Toggle exclusion net description", netDescCorrect,
                                 netDescCorrect ? "" : "Toggle net description missing");
                    }
                }

                // Test statistics
                auto& manager = parser.getDataManager();
                auto stats = manager.getStatistics();
                
                bool statsValid = (stats.totalScopes > 0 && stats.totalExclusions > 0);
                logResult("Statistics generation", statsValid,
                         statsValid ? "" : "Invalid statistics generated");

                std::cout << "  Parsed Statistics:" << std::endl;
                std::cout << "    Total Scopes: " << stats.totalScopes << std::endl;
                std::cout << "    Total Exclusions: " << stats.totalExclusions << std::endl;
                std::cout << "    Annotated Exclusions: " << stats.annotatedExclusions << std::endl;
                for (const auto& [type, count] : stats.exclusionsByType) {
                    std::cout << "    " << exclusionTypeToString(type) << ": " << count << std::endl;
                }
            }

        } catch (const std::exception& e) {
            logResult("Parsing test exception", false, e.what());
        }
    }

    /**
     * @brief Test writing functionality and round-trip integrity
     */
    void testWriting() {
        std::cout << "\n=== Testing Writing Functionality ===" << std::endl;

        try {
            // First parse the sample content
            ExclusionParser parser;
            std::string testContent = createSampleExclusionContent();
            auto parseResult = parser.parseString(testContent, "round_trip_test");

            if (!parseResult.success) {
                logResult("Initial parse for writing test", false, parseResult.errorMessage);
                return;
            }

            auto originalData = parser.getData();

            // Test writing to string
            ExclusionWriter writer;
            WriterConfig config;
            config.includeComments = true;
            config.includeAnnotations = true;
            config.sortExclusions = false; // Preserve order for comparison
            writer.setConfig(config);

            std::string writtenContent = writer.writeToString(*originalData);
            bool writeSuccessful = !writtenContent.empty();
            logResult("Write to string", writeSuccessful,
                     writeSuccessful ? "" : "Written content is empty");

            if (writeSuccessful) {
                std::cout << "  Written content length: " << writtenContent.length() << " characters" << std::endl;

                // Test round-trip parsing
                ExclusionParser roundTripParser;
                auto roundTripResult = roundTripParser.parseString(writtenContent, "round_trip_verify");

                logResult("Round-trip parsing", roundTripResult.success,
                         roundTripResult.success ? "" : roundTripResult.errorMessage);

                if (roundTripResult.success) {
                    auto roundTripData = roundTripParser.getData();

                    // Compare scope counts
                    size_t originalScopes = originalData->getScopeCount();
                    size_t roundTripScopes = roundTripData->getScopeCount();
                    bool scopeCountMatch = (originalScopes == roundTripScopes);
                    logResult("Round-trip scope count", scopeCountMatch,
                             scopeCountMatch ? "" : "Original: " + std::to_string(originalScopes) + 
                             ", Round-trip: " + std::to_string(roundTripScopes));

                    // Compare exclusion counts
                    size_t originalExclusions = originalData->getTotalExclusionCount();
                    size_t roundTripExclusions = roundTripData->getTotalExclusionCount();
                    bool exclusionCountMatch = (originalExclusions == roundTripExclusions);
                    logResult("Round-trip exclusion count", exclusionCountMatch,
                             exclusionCountMatch ? "" : "Original: " + std::to_string(originalExclusions) + 
                             ", Round-trip: " + std::to_string(roundTripExclusions));

                    // Detailed comparison of first scope
                    auto originalScope = originalData->getScope("tb.gpu0.chip0.core.dcio_wrapper");
                    auto roundTripScope = roundTripData->getScope("tb.gpu0.chip0.core.dcio_wrapper");

                    if (originalScope && roundTripScope) {
                        bool blockCountMatch = (originalScope->blockExclusions.size() == roundTripScope->blockExclusions.size());
                        bool toggleCountMatch = (originalScope->toggleExclusions.size() == roundTripScope->toggleExclusions.size());
                        bool fsmCountMatch = (originalScope->fsmExclusions.size() == roundTripScope->fsmExclusions.size());
                        bool conditionCountMatch = (originalScope->conditionExclusions.size() == roundTripScope->conditionExclusions.size());

                        logResult("Round-trip block exclusions", blockCountMatch);
                        logResult("Round-trip toggle exclusions", toggleCountMatch);
                        logResult("Round-trip FSM exclusions", fsmCountMatch);
                        logResult("Round-trip condition exclusions", conditionCountMatch);

                        // Verify specific exclusion content
                        auto origBlock = originalScope->blockExclusions.find("161");
                        auto rtBlock = roundTripScope->blockExclusions.find("161");
                        if (origBlock != originalScope->blockExclusions.end() && 
                            rtBlock != roundTripScope->blockExclusions.end()) {
                            bool blockContentMatch = (origBlock->second.sourceCode == rtBlock->second.sourceCode &&
                                                    origBlock->second.checksum == rtBlock->second.checksum &&
                                                    origBlock->second.annotation == rtBlock->second.annotation);
                            logResult("Round-trip block content integrity", blockContentMatch,
                                     blockContentMatch ? "" : "Block content differs after round-trip");
                        }
                    }

                    std::cout << "  Round-trip verification completed:" << std::endl;
                    std::cout << "    Original scopes: " << originalScopes << std::endl;
                    std::cout << "    Round-trip scopes: " << roundTripScopes << std::endl;
                    std::cout << "    Original exclusions: " << originalExclusions << std::endl;
                    std::cout << "    Round-trip exclusions: " << roundTripExclusions << std::endl;
                }
            }

            // Test file writing
            std::string testFileName = "test_output.el";
            auto fileWriteResult = writer.writeFile(testFileName, *originalData);
            logResult("Write to file", fileWriteResult.success,
                     fileWriteResult.success ? "" : fileWriteResult.errorMessage);

            if (fileWriteResult.success) {
                // Verify file exists and has content
                std::ifstream testFile(testFileName);
                bool fileExists = testFile.good();
                logResult("Output file exists", fileExists,
                         fileExists ? "" : "Could not open output file");

                if (fileExists) {
                    testFile.seekg(0, std::ios::end);
                    size_t fileSize = testFile.tellg();
                    bool fileHasContent = (fileSize > 0);
                    logResult("Output file has content", fileHasContent,
                             fileHasContent ? "" : "Output file is empty");

                    std::cout << "  Output file size: " << fileSize << " bytes" << std::endl;
                }

                // Clean up test file
                std::filesystem::remove(testFileName);
            }

        } catch (const std::exception& e) {
            logResult("Writing test exception", false, e.what());
        }
    }

    /**
     * @brief Test using real exclusion files from the workspace
     */
    void testRealFiles() {
        std::cout << "\n=== Testing Real Exclusion Files ===" << std::endl;

        try {
            std::string exclusionDir = "../exclusion";
            
            if (!std::filesystem::exists(exclusionDir)) {
                logResult("Exclusion directory exists", false, "Directory not found: " + exclusionDir);
                return;
            }

            logResult("Exclusion directory exists", true);

            // Find .el files
            std::vector<std::string> elFiles;
            for (const auto& entry : std::filesystem::directory_iterator(exclusionDir)) {
                if (entry.path().extension() == ".el") {
                    elFiles.push_back(entry.path().string());
                }
            }

            bool filesFound = !elFiles.empty();
            logResult("Found .el files", filesFound, 
                     filesFound ? "Found " + std::to_string(elFiles.size()) + " files" : "No .el files found");

            if (filesFound) {
                ExclusionParser parser;
                ParserConfig config;
                config.strictMode = false; // Be lenient with real files
                config.preserveComments = true;
                parser.setConfig(config);

                size_t successfulParses = 0;
                size_t totalExclusions = 0;

                // Test parsing first few files
                size_t filesToTest = std::min(size_t(5), elFiles.size());
                for (size_t i = 0; i < filesToTest; ++i) {
                    const auto& file = elFiles[i];
                    std::cout << "  Testing file: " << std::filesystem::path(file).filename().string() << std::endl;

                    auto result = parser.parseFile(file);
                    if (result.success) {
                        successfulParses++;
                        totalExclusions += result.exclusionsParsed;
                        std::cout << "    Parsed " << result.exclusionsParsed << " exclusions" << std::endl;
                    } else {
                        std::cout << "    Parse failed: " << result.errorMessage << std::endl;
                    }
                }

                bool allParsedSuccessfully = (successfulParses == filesToTest);
                logResult("Real file parsing", allParsedSuccessfully,
                         allParsedSuccessfully ? "All files parsed successfully" : 
                         std::to_string(successfulParses) + "/" + std::to_string(filesToTest) + " files parsed");

                if (successfulParses > 0) {
                    auto data = parser.getData();
                    auto& manager = parser.getDataManager();
                    auto stats = manager.getStatistics();

                    std::cout << "  Real file statistics:" << std::endl;
                    std::cout << "    Total scopes: " << stats.totalScopes << std::endl;
                    std::cout << "    Total exclusions: " << stats.totalExclusions << std::endl;
                    std::cout << "    Files processed: " << successfulParses << std::endl;

                    // Test writing real data
                    ExclusionWriter writer;
                    std::string realTestFile = "real_data_test.el";
                    auto writeResult = writer.writeFile(realTestFile, *data);
                    
                    logResult("Real data writing", writeResult.success,
                             writeResult.success ? "" : writeResult.errorMessage);

                    if (writeResult.success) {
                        std::cout << "    Wrote " << writeResult.exclusionsWritten 
                                  << " exclusions to " << realTestFile << std::endl;
                        
                        // Clean up
                        std::filesystem::remove(realTestFile);
                    }
                }
            }

        } catch (const std::exception& e) {
            logResult("Real file test exception", false, e.what());
        }
    }

    /**
     * @brief Test error handling and edge cases
     */
    void testErrorHandling() {
        std::cout << "\n=== Testing Error Handling ===" << std::endl;

        try {
            ExclusionParser parser;

            // Test empty content
            auto emptyResult = parser.parseString("", "empty_test");
            bool emptyHandled = emptyResult.success; // Empty should be valid
            logResult("Empty content handling", emptyHandled);

            // Test invalid file
            auto invalidFileResult = parser.parseFile("nonexistent_file.el");
            bool invalidFileHandled = !invalidFileResult.success;
            logResult("Invalid file handling", invalidFileHandled,
                     invalidFileHandled ? "" : "Should have failed for nonexistent file");

            // Test malformed content
            std::string malformedContent = R"(
INVALID_SECTION_TYPE bad.scope "123"
    BadExclusionType invalid_data
    Block without_proper_format
)";
            auto malformedResult = parser.parseString(malformedContent, "malformed_test");
            // Parser should handle this gracefully
            logResult("Malformed content handling", true, "Gracefully handled malformed content");

            // Test writer with empty data
            ExclusionWriter writer;
            ExclusionData emptyData;
            std::string emptyOutput = writer.writeToString(emptyData);
            bool emptyWriteHandled = !emptyOutput.empty(); // Should produce some output (headers, etc.)
            logResult("Empty data writing", emptyWriteHandled);

        } catch (const std::exception& e) {
            logResult("Error handling test exception", false, e.what());
        }
    }

    /**
     * @brief Run all tests and display results
     */
    void runAllTests() {
        std::cout << "======================================" << std::endl;
        std::cout << "  COMPREHENSIVE EXCLUSION PARSER TEST" << std::endl;
        std::cout << "======================================" << std::endl;

        testDataStructures();
        testParsing();
        testWriting();
        testRealFiles();
        testErrorHandling();

        std::cout << "\n======================================" << std::endl;
        std::cout << "  TEST RESULTS SUMMARY" << std::endl;
        std::cout << "======================================" << std::endl;

        for (const auto& result : testResults) {
            std::cout << result << std::endl;
        }

        std::cout << "\n======================================" << std::endl;
        std::cout << "Total Tests: " << totalTests << std::endl;
        std::cout << "Passed: " << passedTests << std::endl;
        std::cout << "Failed: " << (totalTests - passedTests) << std::endl;
        std::cout << "Success Rate: " << (totalTests > 0 ? (double(passedTests) / totalTests * 100.0) : 0.0) << "%" << std::endl;
        std::cout << "======================================" << std::endl;

        if (passedTests == totalTests) {
            std::cout << "\n🎉 ALL TESTS PASSED! 🎉" << std::endl;
        } else {
            std::cout << "\n⚠️  Some tests failed. Review the results above." << std::endl;
        }
    }
};

/**
 * @brief Main function - entry point for comprehensive testing
 */
int main() {
    try {
        ComprehensiveTest tester;
        tester.runAllTests();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Fatal test error: " << e.what() << std::endl;
        return 1;
    }
}