/**
 * @file ExclusionTypes.h
 * @brief Comprehensive data structures for Hardware Coverage Exclusion Parser
 * 
 * This file contains all the C++ data structures used to represent and manage exclusion 
 * coverage data from .el (exclusion list) files commonly used in hardware verification 
 * workflows. The structures use std::unordered_map for efficient O(1) lookup and storage 
 * of coverage exclusions across large verification databases.
 * 
 * Hardware Coverage File Format Overview:
 * Exclusion files (.el) contain four main types of coverage exclusions commonly used
 * in ASIC/FPGA verification environments:
 * 
 * 1. Block (Functional) Exclusions - specific Verilog/SystemVerilog code blocks/lines
 *    that should be excluded from functional coverage analysis
 * 
 * 2. Toggle Exclusions - signal transition coverage exclusions for nets that should
 *    not be monitored for 0-to-1, 1-to-0, or bidirectional transitions
 * 
 * 3. FSM (Finite State Machine) Exclusions - state machine coverage exclusions for
 *    specific states or state transitions that are unreachable or uninteresting
 * 
 * 4. Condition (Expression) Exclusions - conditional/branch coverage exclusions for
 *    Boolean expressions and decision points that should not be analyzed
 * 
 * Each exclusion is hierarchically organized within MODULE or INSTANCE scopes,
 * allowing for fine-grained control over coverage analysis in complex hardware designs.
 * All exclusions include checksums for verification database integrity and optional
 * annotations for documentation and traceability.
 * 
 * @author AMD Advanced Micro Devices Inc. - Verification Team
 * @version 2.0.0
 * @date September 13, 2025
 * @copyright (c) 2025 Advanced Micro Devices Inc. All rights reserved.
 * 
 * @note This library supports DLL export/import for integration with external 
 *       verification tools and provides comprehensive validation and round-trip
 *       testing capabilities for production hardware verification workflows.
 */

#ifndef EXCLUSION_TYPES_H
#define EXCLUSION_TYPES_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <optional>

// Export/Import macros for DLL support
#ifdef _WIN32
    #ifdef EXCLUSION_PARSER_EXPORTS
        #define EXCLUSION_API __declspec(dllexport)
    #else
        #define EXCLUSION_API __declspec(dllimport)
    #endif
#else
    #define EXCLUSION_API
#endif

namespace ExclusionParser {

/**
 * @brief Enumeration for hardware coverage exclusion types
 * 
 * This enum represents the four main categories of coverage exclusions
 * commonly found in hardware verification environments. Each type corresponds
 * to different aspects of coverage analysis in ASIC/FPGA verification workflows.
 */
enum class ExclusionType {
    BLOCK,      ///< Block/Functional exclusions for specific Verilog/SystemVerilog code lines
    TOGGLE,     ///< Toggle exclusions for signal transition coverage (0->1, 1->0)
    FSM,        ///< Finite State Machine exclusions for state and transition coverage
    CONDITION   ///< Condition/Branch exclusions for Boolean expression coverage
};

/**
 * @brief Enumeration for signal toggle transition directions
 * 
 * Represents the specific direction of signal transitions that should be excluded
 * from toggle coverage analysis. Used extensively in digital signal verification
 * where certain transitions may be unreachable or uninteresting for coverage goals.
 */
enum class ToggleDirection {
    ZERO_TO_ONE,    ///< 0-to-1 signal transition (rising edge)
    ONE_TO_ZERO,    ///< 1-to-0 signal transition (falling edge)
    BOTH            ///< Both transition directions excluded (complete signal exclusion)
};

/**
 * @brief Structure representing a functional block (line) coverage exclusion
 * 
 * Block exclusions represent specific lines or blocks of Verilog/SystemVerilog code 
 * that should be excluded from functional coverage analysis. These are commonly used
 * to exclude unreachable code, debug statements, or verification-specific constructs
 * that don't represent actual hardware functionality.
 * 
 * Each block exclusion includes a unique identifier, cryptographic checksum for
 * database integrity verification, the actual source code being excluded, and
 * optional human-readable annotations for documentation and traceability.
 * 
 * File format examples:
 * - Block 161 "1104666086" "do_db_reg_update = 1'b0;"
 * - Block 42 "abc123def" "// synthesis translate_off" "Debug code exclusion"
 */
struct EXCLUSION_API BlockExclusion {
    std::string blockId;        ///< Unique block identifier within scope (e.g., "161", "block_42")
    std::string checksum;       ///< Cryptographic checksum for database integrity (e.g., "1104666086")
    std::string sourceCode;     ///< Complete Verilog/SystemVerilog source line being excluded
    std::string annotation;     ///< Optional human-readable annotation explaining exclusion rationale
    
    /**
     * @brief Default constructor for BlockExclusion
     * @param id Unique block identifier within the current scope
     * @param cs Cryptographic checksum for verification database integrity
     * @param code Complete source code line or block being excluded from coverage
     * @param annot Optional annotation explaining why this block is excluded (default empty)
     */
    BlockExclusion(const std::string& id = "", const std::string& cs = "", 
                   const std::string& code = "", const std::string& annot = "")
        : blockId(id), checksum(cs), sourceCode(code), annotation(annot) {}
};

/**
 * @brief Structure representing a signal toggle coverage exclusion
 * 
 * Toggle exclusions define specific digital signals and their transition directions
 * that should be excluded from toggle coverage analysis. These are essential in 
 * hardware verification where certain signal transitions may be unreachable due to
 * design constraints, clock domain crossings, or verification environment limitations.
 * 
 * Toggle exclusions support both scalar signals and bit-specific exclusions for
 * multi-bit buses and arrays. They can exclude specific transition directions
 * (0->1, 1->0) or completely exclude a signal from toggle analysis.
 * 
 * File format examples:
 * - Toggle 1to0 clk_gate_enable "net clk_gate_enable"
 * - Toggle data_bus [7] "net data_bus[31:0]" "MSB exclusion"
 * - Toggle reset_n BOTH "net reset_n" "Reset signal - always driven"
 */
struct EXCLUSION_API ToggleExclusion {
    ToggleDirection direction;   ///< Specific transition direction to exclude (0->1, 1->0, or both)
    std::string signalName;      ///< Full hierarchical signal name in the design
    std::optional<int> bitIndex; ///< Optional bit index for array/bus signals (std::nullopt for scalar)
    std::string netDescription;  ///< Descriptive net information from verification database
    std::string annotation;      ///< Optional human-readable exclusion rationale and documentation
    
    /**
     * @brief Default constructor for ToggleExclusion
     * @param dir Toggle transition direction to exclude (default: both directions)
     * @param name Full hierarchical signal name in the hardware design
     * @param bit Optional bit index for array signals (default: nullopt for scalar signals)
     * @param desc Descriptive net information from the verification database
     * @param annot Optional annotation explaining exclusion rationale (default empty)
     */
    ToggleExclusion(ToggleDirection dir = ToggleDirection::BOTH, 
                    const std::string& name = "", 
                    std::optional<int> bit = std::nullopt,
                    const std::string& desc = "", 
                    const std::string& annot = "")
        : direction(dir), signalName(name), bitIndex(bit), 
          netDescription(desc), annotation(annot) {}
};

/**
 * @brief Structure representing a Finite State Machine (FSM) coverage exclusion
 * 
 * FSM exclusions define specific states or state transitions within finite state
 * machines that should be excluded from FSM coverage analysis. These exclusions
 * are critical in hardware verification where certain states may be unreachable
 * due to design constraints, or certain transitions may be verification artifacts.
 * 
 * FSM exclusions support both individual state exclusions and specific state-to-state
 * transition exclusions. They include cryptographic checksums for database integrity
 * and can represent complex state machine hierarchies common in modern ASIC designs.
 * 
 * File format examples:
 * - Fsm state_machine "85815111" (state exclusion)
 * - Fsm req_state "4079565410" "Unreachable error state"
 * - Transition IDLE->ACTIVE "state_encode_01" "Normal operation transition"
 * - Transition SND_RD_ADDR1->IDLE "11->0" "Bus protocol state transition"
 */
struct EXCLUSION_API FsmExclusion {
    std::string fsmName;         ///< FSM instance name or identifier within the design hierarchy
    std::string checksum;        ///< Cryptographic checksum for verification database integrity
    std::string fromState;       ///< Source state name (for transition exclusions, empty for state exclusions)
    std::string toState;         ///< Destination state name (for transition exclusions, empty for state exclusions)
    std::string transitionId;    ///< Transition encoding or identifier (e.g., "11->0", "encode_01")
    std::string annotation;      ///< Optional human-readable exclusion rationale and documentation
    bool isTransition;           ///< True for state transition exclusions, false for individual state exclusions
    
    /**
     * @brief Constructor for FSM state exclusion (excludes an entire state)
     * @param name FSM instance name within the design hierarchy
     * @param cs Cryptographic checksum for database verification
     * @param annot Optional annotation explaining why this state is excluded (default empty)
     */
    FsmExclusion(const std::string& name = "", const std::string& cs = "", 
                 const std::string& annot = "")
        : fsmName(name), checksum(cs), annotation(annot), isTransition(false) {}
    
    /**
     * @brief Constructor for FSM state transition exclusion (excludes specific state-to-state transition)
     * @param name FSM instance name within the design hierarchy  
     * @param from Source state name in the state machine
     * @param to Destination state name in the state machine
     * @param transId Transition encoding or identifier (e.g., binary encoding, symbolic name)
     * @param annot Optional annotation explaining why this transition is excluded (default empty)
     */
    FsmExclusion(const std::string& name, const std::string& from, 
                 const std::string& to, const std::string& transId, 
                 const std::string& annot = "")
        : fsmName(name), fromState(from), toState(to), 
          transitionId(transId), annotation(annot), isTransition(true) {}
};

/**
 * @brief Structure representing a condition exclusion
 * 
 * Condition exclusions represent conditional coverage points that are
 * excluded from analysis, typically complex boolean expressions.
 * 
 * Example from file:
 * /**
 * @brief Structure representing a conditional/branch coverage exclusion
 * 
 * Condition exclusions define complex Boolean expressions and decision points
 * that should be excluded from conditional coverage analysis. These are essential
 * in hardware verification where certain combinations of conditions may be
 * unreachable due to design constraints, mutual exclusivity, or verification limitations.
 * 
 * Condition exclusions support sophisticated expression analysis including
 * parameter specifications, coverage type definitions, and detailed Boolean
 * expression parsing for accurate coverage exclusion in complex verification scenarios.
 * 
 * File format examples:
 * - Condition 2 "2940925445" "(enable && !reset)" "branch" "Enable control exclusion"
 * - Condition 15 "abc123def" "(clock_div != 2'b00)" "condition" "Clock divider analysis"
 * - Condition complex "xyz789" "((a && b) || (c && d))" "1 -1" "1 \"01\"" "Complex logic exclusion"
 */
struct EXCLUSION_API ConditionExclusion {
    std::string conditionId;     ///< Unique condition identifier within scope (e.g., "2", "complex_cond_1")
    std::string checksum;        ///< Cryptographic checksum for verification database integrity
    std::string expression;      ///< Complete Boolean expression being excluded from coverage
    std::string parameters;      ///< Additional coverage analysis parameters (e.g., "1 -1", "branch_weights")
    std::string coverage;        ///< Coverage type specification (e.g., "branch", "condition", "1 \"01\"")
    std::string annotation;      ///< Optional human-readable exclusion rationale and documentation
    
    /**
     * @brief Default constructor for ConditionExclusion
     * @param id Unique condition identifier within the current scope
     * @param cs Cryptographic checksum for verification database integrity
     * @param expr Complete Boolean expression string being excluded from coverage analysis
     * @param params Additional coverage parameters for analysis control (default empty)
     * @param cov Coverage type specification defining exclusion scope (default empty)
     * @param annot Optional annotation explaining exclusion rationale (default empty)
     */
    ConditionExclusion(const std::string& id = "", const std::string& cs = "",
                       const std::string& expr = "", const std::string& params = "",
                       const std::string& cov = "", const std::string& annot = "")
        : conditionId(id), checksum(cs), expression(expr), 
          parameters(params), coverage(cov), annotation(annot) {}
};

/**
 * @brief Structure representing a hierarchical exclusion scope (Instance or Module)
 * 
 * Exclusion scopes represent the hierarchical organization of exclusions within
 * hardware designs. Each scope corresponds to either a MODULE (design template)
 * or INSTANCE (instantiated module) in the verification environment, providing
 * fine-grained control over coverage exclusion management in complex ASIC/FPGA designs.
 * 
 * Scopes contain all exclusion types organized in efficient hash maps for
 * O(1) lookup performance in large verification databases. Each scope includes
 * integrity checksums and supports both module-level and instance-level exclusions.
 */
struct EXCLUSION_API ExclusionScope {
    std::string scopeName;      ///< Full hierarchical name of the scope
    std::string checksum;       ///< Scope checksum
    bool isModule;              ///< true for MODULE, false for INSTANCE
    
    // Exclusion containers using unordered_map for efficient lookup
    /// Block exclusions mapped by block ID
    std::unordered_map<std::string, BlockExclusion> blockExclusions;
    
    /// Toggle exclusions mapped by signal name + direction + bit index
    std::unordered_map<std::string, std::vector<ToggleExclusion>> toggleExclusions;
    
    /// FSM exclusions mapped by FSM name
    std::unordered_map<std::string, std::vector<FsmExclusion>> fsmExclusions;
    
    /// Condition exclusions mapped by condition ID
    std::unordered_map<std::string, ConditionExclusion> conditionExclusions;
    
    /**
     * @brief Constructor
     * @param name Scope name
     * @param cs Checksum
     * @param module True if this is a module scope
     */
    ExclusionScope(const std::string& name = "", const std::string& cs = "", 
                   bool module = false)
        : scopeName(name), checksum(cs), isModule(module) {}
    
    /**
     * @brief Add a block exclusion to this scope
     * @param exclusion Block exclusion to add
     */
    void addBlockExclusion(const BlockExclusion& exclusion) {
        blockExclusions[exclusion.blockId] = exclusion;
    }
    
    /**
     * @brief Add a toggle exclusion to this scope
     * @param exclusion Toggle exclusion to add
     */
    void addToggleExclusion(const ToggleExclusion& exclusion) {
        toggleExclusions[exclusion.signalName].push_back(exclusion);
    }
    
    /**
     * @brief Add an FSM exclusion to this scope
     * @param exclusion FSM exclusion to add
     */
    void addFsmExclusion(const FsmExclusion& exclusion) {
        fsmExclusions[exclusion.fsmName].push_back(exclusion);
    }
    
    /**
     * @brief Add a condition exclusion to this scope
     * @param exclusion Condition exclusion to add
     */
    void addConditionExclusion(const ConditionExclusion& exclusion) {
        conditionExclusions[exclusion.conditionId] = exclusion;
    }
    
    /**
     * @brief Get total number of exclusions in this scope
     * @return Total count of all exclusions
     */
    size_t getTotalExclusionCount() const {
        size_t total = blockExclusions.size() + conditionExclusions.size();
        for (const auto& pair : toggleExclusions) {
            total += pair.second.size();
        }
        for (const auto& pair : fsmExclusions) {
            total += pair.second.size();
        }
        return total;
    }
};

/**
 * @brief Main data structure for exclusion coverage data
 * 
 * This is the primary container for all exclusion data parsed from .el files.
 * It maintains file metadata and organizes exclusions by scope.
 */
struct EXCLUSION_API ExclusionData {
    // File metadata
    std::string fileName;           ///< Original filename
    std::string generatedBy;        ///< User who generated the file
    std::string formatVersion;      ///< Format version (e.g., "2")
    std::string generationDate;     ///< Date when file was generated
    std::string exclusionMode;      ///< Exclusion mode (e.g., "default")
    
    /// All scopes (instances and modules) mapped by scope name
    std::unordered_map<std::string, ExclusionScope> scopes;
    
    /**
     * @brief Constructor
     * @param filename Original filename
     */
    ExclusionData(const std::string& filename = "") : fileName(filename) {}
    
    /**
     * @brief Add or get a scope (instance or module)
     * @param scopeName Name of the scope
     * @param checksum Scope checksum
     * @param isModule True if this is a module
     * @return Reference to the scope
     */
    ExclusionScope& getOrCreateScope(const std::string& scopeName, 
                                     const std::string& checksum = "", 
                                     bool isModule = false) {
        auto it = scopes.find(scopeName);
        if (it == scopes.end()) {
            scopes[scopeName] = ExclusionScope(scopeName, checksum, isModule);
        }
        return scopes[scopeName];
    }
    
    /**
     * @brief Merge another ExclusionData into this one
     * @param other ExclusionData to merge
     * @param overwriteExisting If true, overwrite existing exclusions
     */
    void merge(const ExclusionData& other, bool overwriteExisting = false) {
        for (const auto& [scopeName, scope] : other.scopes) {
            if (scopes.find(scopeName) == scopes.end() || overwriteExisting) {
                scopes[scopeName] = scope;
            } else {
                // Merge individual exclusions
                auto& existingScope = scopes[scopeName];
                
                // Merge block exclusions
                for (const auto& [blockId, block] : scope.blockExclusions) {
                    if (overwriteExisting || existingScope.blockExclusions.find(blockId) == existingScope.blockExclusions.end()) {
                        existingScope.blockExclusions[blockId] = block;
                    }
                }
                
                // Merge toggle exclusions
                for (const auto& [signalName, toggles] : scope.toggleExclusions) {
                    for (const auto& toggle : toggles) {
                        existingScope.addToggleExclusion(toggle);
                    }
                }
                
                // Merge FSM exclusions
                for (const auto& [fsmName, fsms] : scope.fsmExclusions) {
                    for (const auto& fsm : fsms) {
                        existingScope.addFsmExclusion(fsm);
                    }
                }
                
                // Merge condition exclusions
                for (const auto& [condId, condition] : scope.conditionExclusions) {
                    if (overwriteExisting || existingScope.conditionExclusions.find(condId) == existingScope.conditionExclusions.end()) {
                        existingScope.conditionExclusions[condId] = condition;
                    }
                }
            }
        }
    }
    
    /**
     * @brief Clear all data (reset to empty state)
     */
    void clear() {
        fileName.clear();
        generatedBy.clear();
        formatVersion.clear();
        generationDate.clear();
        exclusionMode.clear();
        scopes.clear();
    }
    
    /**
     * @brief Get total number of scopes
     * @return Number of scopes (instances + modules)
     */
    size_t getScopeCount() const {
        return scopes.size();
    }
    
    /**
     * @brief Get total number of exclusions across all scopes
     * @return Total exclusion count
     */
    size_t getTotalExclusionCount() const {
        size_t total = 0;
        for (const auto& [scopeName, scope] : scopes) {
            total += scope.getTotalExclusionCount();
        }
        return total;
    }
    
    /**
     * @brief Get exclusion counts by type
     * @return Map of exclusion type to count
     */
    std::unordered_map<ExclusionType, size_t> getExclusionCountsByType() const {
        std::unordered_map<ExclusionType, size_t> counts;
        counts[ExclusionType::BLOCK] = 0;
        counts[ExclusionType::TOGGLE] = 0;
        counts[ExclusionType::FSM] = 0;
        counts[ExclusionType::CONDITION] = 0;
        
        for (const auto& [scopeName, scope] : scopes) {
            counts[ExclusionType::BLOCK] += scope.blockExclusions.size();
            counts[ExclusionType::CONDITION] += scope.conditionExclusions.size();
            
            for (const auto& [signalName, toggles] : scope.toggleExclusions) {
                counts[ExclusionType::TOGGLE] += toggles.size();
            }
            
            for (const auto& [fsmName, fsms] : scope.fsmExclusions) {
                counts[ExclusionType::FSM] += fsms.size();
            }
        }
        
        return counts;
    }
};

/**
 * @brief Utility function to convert ToggleDirection to string
 * @param direction Toggle direction enum
 * @return String representation
 */
EXCLUSION_API std::string toggleDirectionToString(ToggleDirection direction);

/**
 * @brief Utility function to convert string to ToggleDirection
 * @param str String representation
 * @return Toggle direction enum
 */
EXCLUSION_API ToggleDirection stringToToggleDirection(const std::string& str);

/**
 * @brief Utility function to convert ExclusionType to string
 * @param type Exclusion type enum
 * @return String representation
 */
EXCLUSION_API std::string exclusionTypeToString(ExclusionType type);

} // namespace ExclusionParser

#endif // EXCLUSION_TYPES_H