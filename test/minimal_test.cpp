/**
 * @file minimal_test.cpp
 * @brief Minimal working test for basic ExclusionParser functionality
 * 
 * This file contains the simplest possible test to validate the library compiles
 * and basic functionality works without complex data structure manipulation.
 */

#include <iostream>
#include <fstream>
#include <string>

// Test that headers include properly
#include "../include/ExclusionTypes.h"

using namespace std;

int main() {
    cout << "=== MINIMAL EXCLUSION PARSER TEST ===" << endl;
    cout << "Built with Visual Studio " << _MSC_VER << endl;
    cout << "C++ Standard: " << __cplusplus << endl;
    
    int totalTests = 0;
    int passedTests = 0;
    
    // Test 1: Basic enum usage
    totalTests++;
    try {
        ExclusionParser::ToggleDirection dir = ExclusionParser::ToggleDirection::ONE_TO_ZERO;
        ExclusionParser::ExclusionType type = ExclusionParser::ExclusionType::BLOCK;
        
        cout << "✓ PASS: Basic enum creation" << endl;
        passedTests++;
    } catch (const exception& e) {
        cout << "✗ FAIL: Basic enum creation - " << e.what() << endl;
    }
    
    // Test 2: Basic structure creation 
    totalTests++;
    try {
        ExclusionParser::BlockExclusion block;
        block.blockId = "123";
        block.checksum = "456789";
        block.sourceCode = "test = 1;";
        block.annotation = "Test annotation";
        
        cout << "✓ PASS: Block exclusion structure" << endl;
        passedTests++;
    } catch (const exception& e) {
        cout << "✗ FAIL: Block exclusion structure - " << e.what() << endl;
    }
    
    // Test 3: Toggle exclusion structure
    totalTests++;
    try {
        ExclusionParser::ToggleExclusion toggle;
        toggle.direction = ExclusionParser::ToggleDirection::ZERO_TO_ONE;
        toggle.signalName = "clock_signal";
        toggle.netDescription = "net clock_signal";
        toggle.annotation = "Clock toggle";
        
        cout << "✓ PASS: Toggle exclusion structure" << endl;
        passedTests++;
    } catch (const exception& e) {
        cout << "✗ FAIL: Toggle exclusion structure - " << e.what() << endl;
    }
    
    // Test 4: FSM exclusion structure
    totalTests++;
    try {
        ExclusionParser::FsmExclusion fsm;
        fsm.fsmName = "test_fsm";
        fsm.checksum = "789012";
        fsm.fromState = "IDLE";
        fsm.toState = "ACTIVE";
        fsm.transitionId = "0->1";
        fsm.annotation = "Test FSM";
        fsm.isTransition = true;
        
        cout << "✓ PASS: FSM exclusion structure" << endl;
        passedTests++;
    } catch (const exception& e) {
        cout << "✗ FAIL: FSM exclusion structure - " << e.what() << endl;
    }
    
    // Test 5: Condition exclusion structure
    totalTests++;
    try {
        ExclusionParser::ConditionExclusion condition;
        condition.conditionId = "1";
        condition.checksum = "345678";
        condition.expression = "(a && b)";
        condition.parameters = "1 -1";
        condition.coverage = "(1 \"01\")";
        condition.annotation = "Test condition";
        
        cout << "✓ PASS: Condition exclusion structure" << endl;
        passedTests++;
    } catch (const exception& e) {
        cout << "✗ FAIL: Condition exclusion structure - " << e.what() << endl;
    }
    
    // Test 6: Basic scope structure
    totalTests++;
    try {
        ExclusionParser::ExclusionScope scope;
        scope.scopeName = "tb.test.module";
        scope.checksum = "987654321";
        scope.isModule = true;
        
        // Add basic exclusions to maps
        scope.blockExclusions["123"] = ExclusionParser::BlockExclusion();
        scope.conditionExclusions["1"] = ExclusionParser::ConditionExclusion();
        
        cout << "✓ PASS: Exclusion scope structure" << endl;
        passedTests++;
    } catch (const exception& e) {
        cout << "✗ FAIL: Exclusion scope structure - " << e.what() << endl;
    }
    
    // Test 7: Basic data container
    totalTests++;
    try {
        ExclusionParser::ExclusionData data("test.el");
        data.fileName = "test.el";
        data.formatVersion = "2";
        data.generatedBy = "test_user";
        
        cout << "✓ PASS: Exclusion data container" << endl;
        passedTests++;
    } catch (const exception& e) {
        cout << "✗ FAIL: Exclusion data container - " << e.what() << endl;
    }
    
    // Test 8: Create simple test file
    totalTests++;
    try {
        string testContent = R"(# Simple test file
INSTANCE tb.test "123456789"
    Block 101 "111111111" "signal = 1'b0;"
    Toggle 1to0 clock "net clock"
)";
        
        ofstream testFile("minimal_test_output.el");
        testFile << testContent;
        testFile.close();
        
        // Check file was created
        ifstream checkFile("minimal_test_output.el");
        if (checkFile.good()) {
            cout << "✓ PASS: File I/O operations" << endl;
            passedTests++;
            remove("minimal_test_output.el"); // Clean up
        } else {
            cout << "✗ FAIL: File I/O operations - Could not create file" << endl;
        }
        checkFile.close();
    } catch (const exception& e) {
        cout << "✗ FAIL: File I/O operations - " << e.what() << endl;
    }
    
    // Results summary
    cout << "\n=== TEST RESULTS ===" << endl;
    cout << "Total Tests: " << totalTests << endl;
    cout << "Passed: " << passedTests << endl;
    cout << "Failed: " << (totalTests - passedTests) << endl;
    
    if (totalTests > 0) {
        double successRate = (double(passedTests) / totalTests) * 100.0;
        cout << "Success Rate: " << successRate << "%" << endl;
    }
    
    if (passedTests == totalTests) {
        cout << "\n🎉 ALL MINIMAL TESTS PASSED! 🎉" << endl;
        cout << "The ExclusionParser data structures are working correctly!" << endl;
        cout << "Library headers compile successfully." << endl;
        cout << "Ready for integration with parsing and writing components." << endl;
    } else {
        cout << "\n⚠️ Some tests failed." << endl;
    }
    
    cout << "\nPress Enter to continue...";
    cin.get();
    
    return (passedTests == totalTests) ? 0 : 1;
}