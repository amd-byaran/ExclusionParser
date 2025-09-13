/**
 * @file test_data_structures.cpp
 * @brief Tests for data structures and basic functionality
 * 
 * This file contains unit tests for the core data structures
 * used in the Exclusion Coverage Parser.
 * 
 * @author ExclusionCoverageParser
 * @version 1.0.0
 * @date 2025
 */

#include <gtest/gtest.h>
#include "ExclusionTypes.h"
#include "ExclusionData.h"

using namespace ExclusionParser;

/**
 * @brief Test fixture for data structure tests
 */
class DataStructureTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up test data
        data = std::make_shared<ExclusionData>("test.el");
        data->generatedBy = "test_user";
        data->formatVersion = "2";
        data->generationDate = "Mon Jan 01 00:00:00 2025";
        data->exclusionMode = "test";
    }
    
    void TearDown() override {
        // Clean up
    }
    
    std::shared_ptr<ExclusionData> data;
};

/**
 * @brief Test BlockExclusion structure
 */
TEST_F(DataStructureTest, BlockExclusionBasic) {
    BlockExclusion block("123", "456789", "test_code = 1'b0;", "Test annotation");
    
    EXPECT_EQ(block.blockId, "123");
    EXPECT_EQ(block.checksum, "456789");
    EXPECT_EQ(block.sourceCode, "test_code = 1'b0;");
    EXPECT_EQ(block.annotation, "Test annotation");
}

/**
 * @brief Test ToggleExclusion structure
 */
TEST_F(DataStructureTest, ToggleExclusionBasic) {
    ToggleExclusion toggle(ToggleDirection::ZERO_TO_ONE, "clock_signal", 5, 
                          "net clock_signal[7:0]", "Clock toggle test");
    
    EXPECT_EQ(toggle.direction, ToggleDirection::ZERO_TO_ONE);
    EXPECT_EQ(toggle.signalName, "clock_signal");
    EXPECT_TRUE(toggle.bitIndex.has_value());
    EXPECT_EQ(toggle.bitIndex.value(), 5);
    EXPECT_EQ(toggle.netDescription, "net clock_signal[7:0]");
    EXPECT_EQ(toggle.annotation, "Clock toggle test");
}

/**
 * @brief Test FsmExclusion structure for states
 */
TEST_F(DataStructureTest, FsmExclusionState) {
    FsmExclusion fsm("test_fsm", "12345", "FSM state test");
    
    EXPECT_EQ(fsm.fsmName, "test_fsm");
    EXPECT_EQ(fsm.checksum, "12345");
    EXPECT_EQ(fsm.annotation, "FSM state test");
    EXPECT_FALSE(fsm.isTransition);
}

/**
 * @brief Test FsmExclusion structure for transitions
 */
TEST_F(DataStructureTest, FsmExclusionTransition) {
    FsmExclusion fsm("test_fsm", "IDLE", "ACTIVE", "0->1", "FSM transition test");
    
    EXPECT_EQ(fsm.fsmName, "test_fsm");
    EXPECT_EQ(fsm.fromState, "IDLE");
    EXPECT_EQ(fsm.toState, "ACTIVE");
    EXPECT_EQ(fsm.transitionId, "0->1");
    EXPECT_EQ(fsm.annotation, "FSM transition test");
    EXPECT_TRUE(fsm.isTransition);
}

/**
 * @brief Test ConditionExclusion structure
 */
TEST_F(DataStructureTest, ConditionExclusionBasic) {
    ConditionExclusion condition("2", "789123", "signal_a && signal_b", "1 -1", "1 \"01\"", "Condition test");
    
    EXPECT_EQ(condition.conditionId, "2");
    EXPECT_EQ(condition.checksum, "789123");
    EXPECT_EQ(condition.expression, "signal_a && signal_b");
    EXPECT_EQ(condition.parameters, "1 -1");
    EXPECT_EQ(condition.coverage, "1 \"01\"");
    EXPECT_EQ(condition.annotation, "Condition test");
}

/**
 * @brief Test ExclusionScope functionality
 */
TEST_F(DataStructureTest, ExclusionScopeOperations) {
    ExclusionScope scope("tb.test.module", "scope_checksum", false);
    
    EXPECT_EQ(scope.scopeName, "tb.test.module");
    EXPECT_EQ(scope.checksum, "scope_checksum");
    EXPECT_FALSE(scope.isModule);
    EXPECT_EQ(scope.getTotalExclusionCount(), 0);
    
    // Add exclusions
    BlockExclusion block("1", "123", "code", "annotation");
    scope.addBlockExclusion(block);
    
    ToggleExclusion toggle(ToggleDirection::BOTH, "signal", std::nullopt, "net signal", "");
    scope.addToggleExclusion(toggle);
    
    FsmExclusion fsm("fsm", "456", "");
    scope.addFsmExclusion(fsm);
    
    ConditionExclusion condition("1", "789", "expr", "params", "cov", "");
    scope.addConditionExclusion(condition);
    
    EXPECT_EQ(scope.getTotalExclusionCount(), 4);
    EXPECT_EQ(scope.blockExclusions.size(), 1);
    EXPECT_EQ(scope.toggleExclusions.size(), 1);
    EXPECT_EQ(scope.fsmExclusions.size(), 1);
    EXPECT_EQ(scope.conditionExclusions.size(), 1);
}

/**
 * @brief Test ExclusionData functionality
 */
TEST_F(DataStructureTest, ExclusionDataOperations) {
    EXPECT_EQ(data->fileName, "test.el");
    EXPECT_EQ(data->generatedBy, "test_user");
    EXPECT_EQ(data->getScopeCount(), 0);
    EXPECT_EQ(data->getTotalExclusionCount(), 0);
    
    // Add a scope
    auto& scope = data->getOrCreateScope("test.scope", "checksum", false);
    EXPECT_EQ(data->getScopeCount(), 1);
    
    // Add exclusions to scope
    BlockExclusion block("1", "123", "code", "annotation");
    scope.addBlockExclusion(block);
    
    EXPECT_EQ(data->getTotalExclusionCount(), 1);
    
    // Test exclusion counts by type
    auto counts = data->getExclusionCountsByType();
    EXPECT_EQ(counts[ExclusionType::BLOCK], 1);
    EXPECT_EQ(counts[ExclusionType::TOGGLE], 0);
    EXPECT_EQ(counts[ExclusionType::FSM], 0);
    EXPECT_EQ(counts[ExclusionType::CONDITION], 0);
}

/**
 * @brief Test ExclusionData merge functionality
 */
TEST_F(DataStructureTest, ExclusionDataMerge) {
    // Create first data set
    auto& scope1 = data->getOrCreateScope("test.scope1", "checksum1", false);
    BlockExclusion block1("1", "123", "code1", "annotation1");
    scope1.addBlockExclusion(block1);
    
    // Create second data set
    ExclusionData other("other.el");
    auto& scope2 = other.getOrCreateScope("test.scope2", "checksum2", true);
    BlockExclusion block2("2", "456", "code2", "annotation2");
    scope2.addBlockExclusion(block2);
    
    // Also add to same scope as first data
    auto& scope1_other = other.getOrCreateScope("test.scope1", "checksum1", false);
    BlockExclusion block3("3", "789", "code3", "annotation3");
    scope1_other.addBlockExclusion(block3);
    
    EXPECT_EQ(data->getScopeCount(), 1);
    EXPECT_EQ(data->getTotalExclusionCount(), 1);
    
    // Merge
    data->merge(other, false);
    
    EXPECT_EQ(data->getScopeCount(), 2);
    EXPECT_EQ(data->getTotalExclusionCount(), 3);
    
    // Check that both blocks exist in scope1
    auto& mergedScope1 = data->scopes["test.scope1"];
    EXPECT_EQ(mergedScope1.blockExclusions.size(), 2);
    EXPECT_TRUE(mergedScope1.blockExclusions.find("1") != mergedScope1.blockExclusions.end());
    EXPECT_TRUE(mergedScope1.blockExclusions.find("3") != mergedScope1.blockExclusions.end());
}

/**
 * @brief Test utility functions
 */
TEST_F(DataStructureTest, UtilityFunctions) {
    // Test toggle direction conversion
    EXPECT_EQ(toggleDirectionToString(ToggleDirection::ZERO_TO_ONE), "0to1");
    EXPECT_EQ(toggleDirectionToString(ToggleDirection::ONE_TO_ZERO), "1to0");
    EXPECT_EQ(toggleDirectionToString(ToggleDirection::BOTH), "");
    
    EXPECT_EQ(stringToToggleDirection("0to1"), ToggleDirection::ZERO_TO_ONE);
    EXPECT_EQ(stringToToggleDirection("1to0"), ToggleDirection::ONE_TO_ZERO);
    EXPECT_EQ(stringToToggleDirection(""), ToggleDirection::BOTH);
    EXPECT_EQ(stringToToggleDirection("invalid"), ToggleDirection::BOTH);
    
    // Test exclusion type conversion
    EXPECT_EQ(exclusionTypeToString(ExclusionType::BLOCK), "Block");
    EXPECT_EQ(exclusionTypeToString(ExclusionType::TOGGLE), "Toggle");
    EXPECT_EQ(exclusionTypeToString(ExclusionType::FSM), "FSM");
    EXPECT_EQ(exclusionTypeToString(ExclusionType::CONDITION), "Condition");
}

/**
 * @brief Test ExclusionDataManager basic functionality
 */
TEST_F(DataStructureTest, ExclusionDataManagerBasic) {
    ExclusionDataManager manager;
    manager.setData(data);
    
    EXPECT_EQ(manager.getData(), data);
    EXPECT_FALSE(manager.isEmpty());
    
    // Add some data
    auto& scope = data->getOrCreateScope("test.scope", "checksum", false);
    BlockExclusion block("1", "123", "code", "annotation with keyword");
    scope.addBlockExclusion(block);
    
    // Test statistics
    auto stats = manager.getStatistics();
    EXPECT_EQ(stats.totalScopes, 1);
    EXPECT_EQ(stats.instanceScopes, 1);
    EXPECT_EQ(stats.moduleScopes, 0);
    EXPECT_EQ(stats.totalExclusions, 1);
    EXPECT_EQ(stats.annotatedExclusions, 1);
    
    // Test search
    SearchCriteria criteria;
    criteria.annotation = "keyword";
    auto results = manager.search(criteria);
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(results[0].first, "test.scope");
    EXPECT_EQ(results[0].second, ExclusionType::BLOCK);
    
    // Test clear
    manager.clear();
    EXPECT_TRUE(manager.isEmpty());
}

/**
 * @brief Test PatternMatcher functionality
 */
TEST_F(DataStructureTest, PatternMatcher) {
    // Test basic pattern matching
    EXPECT_TRUE(PatternMatcher::matches("test", "test"));
    EXPECT_FALSE(PatternMatcher::matches("test", "Test"));
    EXPECT_TRUE(PatternMatcher::matches("test", "Test", false)); // case insensitive
    
    // Test wildcard patterns
    EXPECT_TRUE(PatternMatcher::matches("test*", "test123"));
    EXPECT_TRUE(PatternMatcher::matches("*test", "123test"));
    EXPECT_TRUE(PatternMatcher::matches("*test*", "123test456"));
    
    EXPECT_TRUE(PatternMatcher::matches("test?", "test1"));
    EXPECT_FALSE(PatternMatcher::matches("test?", "test12"));
    
    // Test pattern escaping
    std::string escaped = PatternMatcher::escape("test.*[abc]");
    EXPECT_TRUE(escaped.find("\\") != std::string::npos); // Should contain escape characters
}