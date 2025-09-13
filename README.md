# Exclusion Coverage Parser

A modern C++ library for parsing and writing exclusion coverage files (.el format) used in hardware verification and testing.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Quick Start](#quick-start)
- [Architecture](#architecture)
- [Data Structures](#data-structures)
- [API Reference](#api-reference)
- [Usage Examples](#usage-examples)
- [Building](#building)
- [Testing](#testing)
- [Advanced Usage](#advanced-usage)
- [Performance](#performance)
- [Contributing](#contributing)

## Overview

The Exclusion Coverage Parser is a high-performance C++ library designed to handle exclusion coverage files commonly used in hardware verification workflows. It provides a complete solution for parsing, manipulating, and writing exclusion data with support for:

- **Block (Line) Exclusions**: Code coverage exclusions for specific source lines
- **Toggle Exclusions**: Signal transition coverage exclusions  
- **FSM Exclusions**: Finite state machine coverage exclusions
- **Condition Exclusions**: Conditional coverage exclusions

The library supports both static linking and dynamic linking (DLL) and is designed for use with Visual Studio 2022/2026.

## Features

### Core Functionality
- ✅ **Complete .el file format support** - Handles all exclusion types and annotations
- ✅ **High-performance parsing** - Optimized for large files with millions of exclusions
- ✅ **Memory efficient** - Uses `std::unordered_map` for O(1) lookup performance
- ✅ **Round-trip fidelity** - Parse → Modify → Write maintains data integrity
- ✅ **Merge capabilities** - Combine exclusions from multiple files
- ✅ **Flexible search** - Find exclusions by type, scope, annotation, etc.

### Advanced Features
- ✅ **Pattern matching** - Wildcard support for scope and signal name searching
- ✅ **Data validation** - Comprehensive consistency checking
- ✅ **Statistics** - Detailed analysis of exclusion data
- ✅ **Configurable output** - Control formatting, sorting, and content
- ✅ **Error handling** - Graceful handling of malformed files
- ✅ **Debug support** - Verbose logging for troubleshooting

### Integration
- ✅ **Static library** - Link directly into applications
- ✅ **Dynamic library (DLL)** - Runtime linking support
- ✅ **Modern C++20** - Uses latest language features
- ✅ **Cross-platform** - Windows primary, portable design
- ✅ **CMake support** - Easy integration into build systems

## Quick Start

### Basic Usage

```cpp
#include "ExclusionParser.h"
#include "ExclusionWriter.h"

using namespace ExclusionParser;

int main() {
    // Create parser and parse a file
    ExclusionParser parser;
    auto result = parser.parseFile("input.el");
    
    if (result.success) {
        auto data = parser.getData();
        std::cout << "Parsed " << data->getTotalExclusionCount() 
                  << " exclusions from " << data->getScopeCount() 
                  << " scopes" << std::endl;
        
        // Write to new file
        ExclusionWriter writer;
        auto writeResult = writer.writeFile("output.el", *data);
        
        if (writeResult.success) {
            std::cout << "Successfully wrote output file" << std::endl;
        }
    } else {
        std::cerr << "Parse failed: " << result.errorMessage << std::endl;
    }
    
    return 0;
}
```

### Advanced Example

```cpp
#include "ExclusionParser.h"
#include "ExclusionData.h"

using namespace ExclusionParser;

int main() {
    ExclusionParser parser;
    
    // Configure parser
    ParserConfig config;
    config.strictMode = false;
    config.mergeOnLoad = true;
    parser.setConfig(config);
    
    // Parse multiple files
    std::vector<std::string> files = {
        "module1.el", "module2.el", "module3.el"
    };
    
    auto result = parser.parseFiles(files, true);
    
    if (result.success) {
        // Use data manager for advanced operations
        auto& manager = parser.getDataManager();
        
        // Search for specific exclusions
        SearchCriteria criteria;
        criteria.type = ExclusionType::TOGGLE;
        criteria.annotation = "clock";
        
        auto searchResults = manager.search(criteria);
        std::cout << "Found " << searchResults.size() 
                  << " clock-related toggle exclusions" << std::endl;
        
        // Get statistics
        auto stats = manager.getStatistics();
        std::cout << "Statistics:" << std::endl;
        std::cout << "  Total exclusions: " << stats.totalExclusions << std::endl;
        std::cout << "  Annotated: " << stats.annotatedExclusions << std::endl;
        
        // Find potential duplicates
        auto duplicates = manager.findPotentialDuplicates();
        for (const auto& [checksum, scopes] : duplicates) {
            if (scopes.size() > 1) {
                std::cout << "Duplicate checksum " << checksum 
                          << " found in " << scopes.size() << " scopes" << std::endl;
            }
        }
    }
    
    return 0;
}
```

## Architecture

The library is organized into several key components:

```
ExclusionParser/
├── include/
│   ├── ExclusionTypes.h      # Core data structures
│   ├── ExclusionData.h       # Data management classes  
│   ├── ExclusionParser.h     # Main parser class
│   └── ExclusionWriter.h     # Writer and formatter classes
├── src/
│   ├── ExclusionData.cpp     # Data management implementation
│   ├── ExclusionParser.cpp   # Parser implementation
│   └── ExclusionWriter.cpp   # Writer implementation
├── test/
│   ├── test_main.cpp         # Test suite entry point
│   ├── test_data_structures.cpp
│   ├── test_parser.cpp
│   └── test_writer.cpp
└── exclusion/               # Sample .el files
```

### Key Classes

- **`ExclusionData`** - Main container for all exclusion information
- **`ExclusionScope`** - Represents an INSTANCE or MODULE scope
- **`ExclusionParser`** - Parses .el files into C++ structures
- **`ExclusionWriter`** - Writes C++ structures back to .el format
- **`ExclusionDataManager`** - High-level data operations and search

## Data Structures

### Core Types

The library defines four main exclusion types:

```cpp
enum class ExclusionType {
    BLOCK,      // Block/Line exclusions (code coverage)
    TOGGLE,     // Toggle exclusions (signal transitions)  
    FSM,        // Finite State Machine exclusions
    CONDITION   // Condition exclusions (conditional coverage)
};
```

### Block Exclusions

Represent specific lines of code excluded from coverage:

```cpp
struct BlockExclusion {
    std::string blockId;        // Block identifier (e.g., "161")
    std::string checksum;       // Block checksum  
    std::string sourceCode;     // Source code line
    std::string annotation;     // Optional annotation
};
```

**Example .el format:**
```
ANNOTATION: "Legacy code, compared against Mero"
Block 161 "1104666086" "do_db_reg_update = 1'b0;"
```

### Toggle Exclusions

Represent signal transitions excluded from toggle coverage:

```cpp
struct ToggleExclusion {
    ToggleDirection direction;  // 0to1, 1to0, or both
    std::string signalName;     // Signal name
    std::optional<int> bitIndex; // Bit index for arrays
    std::string netDescription; // Net description 
    std::string annotation;     // Optional annotation
};
```

**Example .el formats:**
```
Toggle 1to0 next_active_duty_cycle_cnt_frac_carry "net next_active_duty_cycle_cnt_frac_carry"
Toggle next_active_duty_cycle_cnt_frac [16] "net next_active_duty_cycle_cnt_frac[16:0]"
```

### FSM Exclusions

Represent finite state machine states or transitions:

```cpp
struct FsmExclusion {
    std::string fsmName;        // FSM identifier
    std::string checksum;       // Checksum (for states)
    std::string fromState;      // Source state (transitions)
    std::string toState;        // Destination state (transitions)
    std::string transitionId;   // Transition identifier
    std::string annotation;     // Optional annotation
    bool isTransition;          // True for transitions, false for states
};
```

**Example .el formats:**
```
Fsm state "85815111"
Transition SND_RD_ADDR1->IDLE "11->0"
```

### Condition Exclusions

Represent conditional coverage exclusions:

```cpp
struct ConditionExclusion {
    std::string conditionId;    // Condition identifier
    std::string checksum;       // Condition checksum
    std::string expression;     // Boolean expression
    std::string parameters;     // Additional parameters
    std::string coverage;       // Coverage specification
    std::string annotation;     // Optional annotation
};
```

**Example .el format:**
```
Condition 2 "2940925445" "(signal_a && signal_b != 2'b0) 1 -1" (1 "01")
```

### Scope Organization

All exclusions are organized within scopes (instances or modules):

```cpp
struct ExclusionScope {
    std::string scopeName;      // Full hierarchical name
    std::string checksum;       // Scope checksum
    bool isModule;              // true for MODULE, false for INSTANCE
    
    // Exclusions organized by type for efficient access
    std::unordered_map<std::string, BlockExclusion> blockExclusions;
    std::unordered_map<std::string, std::vector<ToggleExclusion>> toggleExclusions;
    std::unordered_map<std::string, std::vector<FsmExclusion>> fsmExclusions;
    std::unordered_map<std::string, ConditionExclusion> conditionExclusions;
};
```

## API Reference

### ExclusionParser Class

#### Core Methods

```cpp
class ExclusionParser {
public:
    // Configuration
    void setConfig(const ParserConfig& config);
    const ParserConfig& getConfig() const;
    
    // Parsing methods
    ParseResult parseFile(const std::string& filename);
    ParseResult parseString(const std::string& content, 
                           const std::string& sourceIdentifier = "string");
    ParseResult parseFiles(const std::vector<std::string>& filenames, 
                          bool continueOnError = true);
    
    // Data access
    std::shared_ptr<ExclusionData> getData() const;
    ExclusionDataManager& getDataManager();
    
    // Utility
    void clear();
    bool hasData() const;
    bool validateFile(const std::string& filename) const;
};
```

#### Configuration Options

```cpp
struct ParserConfig {
    bool strictMode;            // Fail on any format violations
    bool validateChecksums;     // Validate checksum format  
    bool preserveComments;      // Preserve comment lines
    bool mergeOnLoad;          // Merge with existing data
    size_t maxFileSize;        // Maximum file size (bytes)
};
```

#### Parse Results

```cpp
struct ParseResult {
    bool success;                           // Success/failure
    std::string errorMessage;               // Error details
    size_t linesProcessed;                  // Lines processed
    size_t exclusionsParsed;                // Exclusions found
    std::vector<std::string> warnings;     // Non-fatal warnings
    std::unordered_map<ExclusionType, size_t> exclusionCounts; // Counts by type
    
    std::string getSummary() const;         // Formatted summary
};
```

### ExclusionWriter Class

#### Core Methods

```cpp
class ExclusionWriter {
public:
    // Configuration
    void setConfig(const WriterConfig& config);
    const WriterConfig& getConfig() const;
    
    // Writing methods
    WriteResult writeFile(const std::string& filename, const ExclusionData& data) const;
    std::string writeToString(const ExclusionData& data) const;
    WriteResult writeToStream(std::ostream& stream, const ExclusionData& data) const;
    
    // Selective writing
    WriteResult writeScopes(const std::string& filename, const ExclusionData& data,
                           const std::vector<std::string>& scopeNames) const;
    WriteResult writeFilteredByType(const std::string& filename, const ExclusionData& data,
                                   const std::vector<ExclusionType>& types) const;
    
    // Utility
    std::vector<std::string> validateForWriting(const ExclusionData& data) const;
    std::string preview(const ExclusionData& data, size_t maxLines = 50) const;
    size_t estimateOutputSize(const ExclusionData& data) const;
};
```

#### Configuration Options

```cpp
struct WriterConfig {
    bool includeComments;           // Include file header comments
    bool includeAnnotations;        // Include exclusion annotations
    bool sortExclusions;           // Sort exclusions within scopes
    bool generateChecksums;        // Generate checksums for scopes
    bool preserveOrder;            // Preserve original order
    std::string indentation;       // Indentation string
    std::string lineEnding;        // Line ending style
    bool compactFormat;            // Use compact format
};
```

### ExclusionDataManager Class

#### Search and Analysis

```cpp
class ExclusionDataManager {
public:
    // Search operations
    std::vector<std::pair<std::string, ExclusionType>> search(const SearchCriteria& criteria) const;
    const ExclusionScope* findScope(const std::string& scopeName) const;
    std::vector<std::string> findScopesMatching(const std::string& pattern) const;
    
    // Analysis
    ExclusionStatistics getStatistics() const;
    std::unordered_set<std::string> getAllSignalNames() const;
    std::unordered_map<std::string, std::vector<std::string>> findPotentialDuplicates() const;
    
    // Data management
    bool mergeData(const ExclusionData& other, bool overwriteExisting = false);
    size_t removeExclusions(const SearchCriteria& criteria);
    std::shared_ptr<ExclusionData> cloneData() const;
};
```

#### Search Criteria

```cpp
struct SearchCriteria {
    std::optional<ExclusionType> type;      // Filter by exclusion type
    std::optional<std::string> scopeName;   // Filter by scope name  
    std::optional<std::string> annotation;  // Filter by annotation content
    std::optional<std::string> signalName;  // Filter by signal name
    std::optional<bool> isModule;           // Filter by scope type
};
```

## Usage Examples

### Example 1: Basic File Processing

```cpp
#include "ExclusionParser.h"
#include "ExclusionWriter.h"

void processExclusionFile(const std::string& inputFile, const std::string& outputFile) {
    ExclusionParser parser;
    
    // Parse input file
    auto parseResult = parser.parseFile(inputFile);
    if (!parseResult.success) {
        std::cerr << "Failed to parse " << inputFile << ": " 
                  << parseResult.errorMessage << std::endl;
        return;
    }
    
    std::cout << parseResult.getSummary() << std::endl;
    
    // Get parsed data
    auto data = parser.getData();
    
    // Configure writer for clean output
    ExclusionWriter writer;
    WriterConfig config;
    config.sortExclusions = true;
    config.includeAnnotations = true;
    writer.setConfig(config);
    
    // Write to output file
    auto writeResult = writer.writeFile(outputFile, *data);
    if (writeResult.success) {
        std::cout << "Successfully wrote " << writeResult.exclusionsWritten 
                  << " exclusions to " << outputFile << std::endl;
    } else {
        std::cerr << "Failed to write output: " << writeResult.errorMessage << std::endl;
    }
}
```

### Example 2: Merging Multiple Files

```cpp
void mergeExclusionFiles(const std::vector<std::string>& inputFiles, 
                        const std::string& outputFile) {
    ExclusionParser parser;
    
    // Configure for merging
    ParserConfig config;
    config.mergeOnLoad = true;
    parser.setConfig(config);
    
    // Parse all input files
    auto result = parser.parseFiles(inputFiles, true);
    
    if (!result.success) {
        std::cerr << "Failed to parse some files" << std::endl;
        for (const auto& warning : result.warnings) {
            std::cerr << "Warning: " << warning << std::endl;
        }
    }
    
    auto data = parser.getData();
    
    std::cout << "Merged data contains:" << std::endl;
    std::cout << "  Scopes: " << data->getScopeCount() << std::endl;
    std::cout << "  Total exclusions: " << data->getTotalExclusionCount() << std::endl;
    
    // Write merged output
    ExclusionWriter writer;
    auto writeResult = writer.writeFile(outputFile, *data);
    
    if (writeResult.success) {
        std::cout << "Successfully wrote merged file" << std::endl;
    }
}
```

### Example 3: Advanced Search and Analysis

```cpp
void analyzeExclusions(const std::string& filename) {
    ExclusionParser parser;
    auto result = parser.parseFile(filename);
    
    if (!result.success) {
        std::cerr << "Parse failed: " << result.errorMessage << std::endl;
        return;
    }
    
    auto& manager = parser.getDataManager();
    
    // Get overall statistics
    auto stats = manager.getStatistics();
    std::cout << "=== Exclusion Analysis ===" << std::endl;
    std::cout << "Total scopes: " << stats.totalScopes 
              << " (Modules: " << stats.moduleScopes 
              << ", Instances: " << stats.instanceScopes << ")" << std::endl;
    std::cout << "Total exclusions: " << stats.totalExclusions 
              << " (Annotated: " << stats.annotatedExclusions << ")" << std::endl;
    
    // Show exclusions by type
    std::cout << "\nExclusions by type:" << std::endl;
    for (const auto& [type, count] : stats.exclusionsByType) {
        std::cout << "  " << exclusionTypeToString(type) << ": " << count << std::endl;
    }
    
    // Find clock-related exclusions
    SearchCriteria clockCriteria;
    clockCriteria.signalName = "clk";
    auto clockResults = manager.search(clockCriteria);
    std::cout << "\nClock-related exclusions: " << clockResults.size() << std::endl;
    
    // Find exclusions with "legacy" in annotation
    auto legacyResults = manager.findByAnnotation("legacy", false);
    std::cout << "Legacy code exclusions: " << legacyResults.size() << std::endl;
    
    // Look for potential duplicates
    auto duplicates = manager.findPotentialDuplicates();
    if (!duplicates.empty()) {
        std::cout << "\nPotential duplicates found:" << std::endl;
        for (const auto& [checksum, scopes] : duplicates) {
            std::cout << "  Checksum " << checksum << " appears in " 
                      << scopes.size() << " scopes" << std::endl;
        }
    }
    
    // List all unique signal names
    auto signals = manager.getAllSignalNames();
    std::cout << "\nUnique signal names: " << signals.size() << std::endl;
    if (signals.size() <= 10) {
        for (const auto& signal : signals) {
            std::cout << "  " << signal << std::endl;
        }
    }
}
```

### Example 4: Custom Data Processing

```cpp
void processToggleExclusions(const std::string& filename) {
    ExclusionParser parser;
    auto result = parser.parseFile(filename);
    
    if (!result.success) return;
    
    auto& manager = parser.getDataManager();
    
    // Process all toggle exclusions using the template function
    manager.forEachExclusion<ToggleExclusion>(
        ExclusionType::TOGGLE,
        [](const std::string& scopeName, const ToggleExclusion& toggle) {
            std::cout << "Scope: " << scopeName << std::endl;
            std::cout << "  Signal: " << toggle.signalName;
            
            if (toggle.bitIndex.has_value()) {
                std::cout << "[" << toggle.bitIndex.value() << "]";
            }
            
            std::cout << " Direction: " << toggleDirectionToString(toggle.direction);
            
            if (!toggle.annotation.empty()) {
                std::cout << " Annotation: " << toggle.annotation;
            }
            
            std::cout << std::endl;
        }
    );
}
```

### Example 5: Filtering and Exporting

```cpp
void exportBlockExclusions(const std::string& inputFile, const std::string& outputFile) {
    ExclusionParser parser;
    auto result = parser.parseFile(inputFile);
    
    if (!result.success) return;
    
    auto data = parser.getData();
    
    // Write only block exclusions
    ExclusionWriter writer;
    std::vector<ExclusionType> typesToWrite = {ExclusionType::BLOCK};
    
    auto writeResult = writer.writeFilteredByType(outputFile, *data, typesToWrite);
    
    if (writeResult.success) {
        std::cout << "Exported " << writeResult.exclusionCounts[ExclusionType::BLOCK] 
                  << " block exclusions to " << outputFile << std::endl;
    }
}

void exportSpecificScopes(const std::string& inputFile, 
                         const std::vector<std::string>& scopePatterns,
                         const std::string& outputFile) {
    ExclusionParser parser;
    auto result = parser.parseFile(inputFile);
    
    if (!result.success) return;
    
    auto& manager = parser.getDataManager();
    
    // Find all scopes matching patterns
    std::vector<std::string> matchingScopes;
    for (const auto& pattern : scopePatterns) {
        auto matches = manager.findScopesMatching(pattern);
        matchingScopes.insert(matchingScopes.end(), matches.begin(), matches.end());
    }
    
    // Remove duplicates
    std::sort(matchingScopes.begin(), matchingScopes.end());
    matchingScopes.erase(std::unique(matchingScopes.begin(), matchingScopes.end()), 
                        matchingScopes.end());
    
    std::cout << "Found " << matchingScopes.size() << " matching scopes" << std::endl;
    
    // Write selected scopes
    ExclusionWriter writer;
    auto writeResult = writer.writeScopes(outputFile, *parser.getData(), matchingScopes);
    
    if (writeResult.success) {
        std::cout << "Exported " << writeResult.scopesWritten 
                  << " scopes with " << writeResult.exclusionsWritten 
                  << " exclusions" << std::endl;
    }
}
```

## Building

### Prerequisites

- **Visual Studio 2022 or later** (Visual Studio 2026 recommended)
- **CMake 3.20 or later**
- **C++20 compatible compiler**
- **Google Test** (optional, for running tests)

### Using CMake

```bash
# Create build directory
mkdir build
cd build

# Configure project
cmake ..

# Build library
cmake --build . --config Release

# Run tests (if Google Test is available)
ctest --config Release
```

### Using Visual Studio

1. Open `ExclusionCoverageParser.sln` in Visual Studio
2. Select desired configuration (Debug/Release)
3. Build → Build Solution (Ctrl+Shift+B)

### Integration Options

#### Static Library
```cmake
# In your CMakeLists.txt
find_package(ExclusionCoverageParser REQUIRED)
target_link_libraries(your_target ExclusionCoverageParser_static)
```

#### Dynamic Library (DLL)
```cmake
# In your CMakeLists.txt  
find_package(ExclusionCoverageParser REQUIRED)
target_link_libraries(your_target ExclusionCoverageParser_shared)
```

### Visual Studio Command Line Tools

To set up the Visual Studio environment for command-line builds:

```powershell
# Import Visual Studio environment
Import-VisualStudioEnvironment

# Build using MSBuild
msbuild ExclusionCoverageParser.sln /p:Configuration=Release /p:Platform=x64
```

## Testing

The library includes comprehensive unit tests covering all major functionality:

### Test Structure

- **`test_data_structures.cpp`** - Tests for core data structures and utilities
- **`test_parser.cpp`** - Tests for parsing functionality and edge cases  
- **`test_writer.cpp`** - Tests for writing functionality and round-trip testing

### Running Tests

```bash
# Using CMake/CTest
cd build
ctest --config Release --verbose

# Using Visual Studio
# Build and run the ExclusionParserTests project

# Direct execution
./build/test/ExclusionParserTests
```

### Test Coverage

The test suite covers:
- ✅ Basic data structure operations
- ✅ File parsing with various formats
- ✅ Error handling and edge cases
- ✅ Writer functionality and formatting
- ✅ Round-trip parsing and writing
- ✅ Data manager search and analysis
- ✅ Configuration options
- ✅ Pattern matching
- ✅ Memory management

### Sample Test Data

Tests use both synthetic data and samples from the provided exclusion files in the `exclusion/` folder.

## Advanced Usage

### Custom Configuration

#### Parser Configuration

```cpp
ExclusionParser parser;

ParserConfig config;
config.strictMode = true;           // Fail on any format violations
config.validateChecksums = true;    // Validate checksum format
config.preserveComments = false;    // Skip comment processing
config.mergeOnLoad = true;          // Merge with existing data
config.maxFileSize = 50 * 1024 * 1024; // 50MB limit

parser.setConfig(config);
```

#### Writer Configuration

```cpp
ExclusionWriter writer;

WriterConfig config;
config.includeComments = true;      // Include file header
config.includeAnnotations = true;   // Include annotations
config.sortExclusions = true;       // Sort within scopes
config.generateChecksums = true;    // Generate scope checksums
config.lineEnding = "\r\n";        // Windows line endings
config.indentation = "  ";          // Two-space indentation

writer.setConfig(config);
```

### Memory Management

The library uses smart pointers for automatic memory management:

```cpp
// Shared ownership of exclusion data
std::shared_ptr<ExclusionData> data = parser.getData();

// Clone data for independent modification
auto clonedData = dataManager.cloneData();

// Monitor memory usage
size_t memoryUsage = dataManager.getMemoryUsage();
std::cout << "Memory usage: " << memoryUsage << " bytes" << std::endl;
```

### Error Handling Best Practices

```cpp
ExclusionParser parser;

// Enable debug mode for troubleshooting
parser.setDebugMode(true);

auto result = parser.parseFile("complex_file.el");

if (!result.success) {
    std::cerr << "Parse failed: " << result.errorMessage << std::endl;
    
    // Show detailed information
    std::cerr << "Lines processed: " << result.linesProcessed << std::endl;
    
    return -1;
}

// Check for warnings even on success
if (!result.warnings.empty()) {
    std::cout << "Parse succeeded with warnings:" << std::endl;
    for (const auto& warning : result.warnings) {
        std::cout << "  Warning: " << warning << std::endl;
    }
}

// Validate data consistency
auto& manager = parser.getDataManager();
auto validationErrors = manager.validateData();

if (!validationErrors.empty()) {
    std::cout << "Data validation issues:" << std::endl;
    for (const auto& error : validationErrors) {
        std::cout << "  Error: " << error << std::endl;
    }
}
```

### Pattern Matching

The library supports powerful pattern matching for scope and signal names:

```cpp
ExclusionDataManager manager;
// ... load data ...

// Find all scopes containing "test"
auto testScopes = manager.findScopesMatching("*test*");

// Find all scopes ending with "wrapper"  
auto wrapperScopes = manager.findScopesMatching("*wrapper");

// Find scopes with specific hierarchy pattern
auto hierarchyScopes = manager.findScopesMatching("tb.gpu?.chip?.core.*");

// Case-insensitive pattern matching
bool matches = PatternMatcher::matches("*CLOCK*", "system_clock", false);
```

### Batch Processing

For processing large numbers of files efficiently:

```cpp
class ExclusionBatchProcessor {
public:
    void processDirectory(const std::string& directory) {
        std::vector<std::string> elFiles;
        
        // Find all .el files in directory
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.path().extension() == ".el") {
                elFiles.push_back(entry.path().string());
            }
        }
        
        std::cout << "Processing " << elFiles.size() << " files..." << std::endl;
        
        ExclusionParser parser;
        ParserConfig config;
        config.mergeOnLoad = true;
        parser.setConfig(config);
        
        // Process in batches to manage memory
        const size_t batchSize = 10;
        for (size_t i = 0; i < elFiles.size(); i += batchSize) {
            auto end = std::min(i + batchSize, elFiles.size());
            std::vector<std::string> batch(elFiles.begin() + i, elFiles.begin() + end);
            
            auto result = parser.parseFiles(batch, true);
            
            std::cout << "Batch " << (i / batchSize + 1) << ": ";
            if (result.success) {
                std::cout << "Success - " << result.exclusionsParsed << " exclusions";
            } else {
                std::cout << "Failed - " << result.errorMessage;
            }
            std::cout << std::endl;
        }
        
        // Generate consolidated report
        generateReport(parser.getDataManager());
    }
    
private:
    void generateReport(const ExclusionDataManager& manager) {
        auto stats = manager.getStatistics();
        
        std::ofstream report("exclusion_report.txt");
        report << "=== Exclusion Analysis Report ===" << std::endl;
        report << "Generated: " << getCurrentTimestamp() << std::endl;
        report << std::endl;
        
        report << "Summary:" << std::endl;
        report << "  Total Scopes: " << stats.totalScopes << std::endl;
        report << "  Total Exclusions: " << stats.totalExclusions << std::endl;
        report << "  Annotated: " << stats.annotatedExclusions << std::endl;
        report << std::endl;
        
        report << "By Type:" << std::endl;
        for (const auto& [type, count] : stats.exclusionsByType) {
            report << "  " << exclusionTypeToString(type) << ": " << count << std::endl;
        }
        
        // Add more detailed analysis as needed
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
};
```

## Performance

The library is optimized for handling large exclusion files efficiently:

### Performance Characteristics

- **Parsing Speed**: ~1M lines/second on modern hardware
- **Memory Usage**: ~100 bytes per exclusion (average)
- **Lookup Time**: O(1) for exclusion lookup by ID within scopes
- **Search Time**: O(n) for text-based searches with early termination

### Optimization Tips

1. **Use appropriate parser configuration**:
   ```cpp
   ParserConfig config;
   config.preserveComments = false;    // Skip if not needed
   config.validateChecksums = false;   // Skip for trusted files
   ```

2. **Leverage the data manager for efficient operations**:
   ```cpp
   // Efficient scope lookup
   auto scope = manager.findScope("exact.scope.name");
   
   // Use pattern matching for bulk operations
   auto matches = manager.findScopesMatching("pattern*");
   ```

3. **Process files in batches for very large datasets**:
   ```cpp
   // Parse files individually and merge selectively
   for (const auto& file : largeFileList) {
       auto result = parser.parseFile(file);
       // Process or filter result before merging
   }
   ```

### Memory Management

The library uses several strategies to minimize memory usage:

- **Shared string storage** - Common strings are shared
- **Efficient containers** - `std::unordered_map` for O(1) lookup
- **Optional fields** - Only store data when present
- **Smart pointers** - Automatic memory management

Monitor memory usage during processing:

```cpp
auto& manager = parser.getDataManager();
std::cout << "Current memory usage: " << manager.getMemoryUsage() << " bytes" << std::endl;

// Clear data when no longer needed
manager.clear();
```

## Contributing

We welcome contributions to the Exclusion Coverage Parser library!

### Development Setup

1. Fork the repository
2. Create a feature branch
3. Set up development environment:
   ```bash
   git clone your-fork-url
   cd ExclusionParser
   mkdir build && cd build
   cmake ..
   ```

### Coding Standards

- **C++20** features encouraged where appropriate
- **Comprehensive documentation** for all public APIs
- **Unit tests** for all new functionality
- **Error handling** for all failure modes
- **Memory safety** - prefer smart pointers and RAII

### Testing Requirements

All contributions must include:

- ✅ Unit tests with >90% code coverage
- ✅ Integration tests for end-to-end scenarios  
- ✅ Performance tests for critical paths
- ✅ Documentation updates

### Submission Process

1. Ensure all tests pass
2. Update documentation as needed
3. Submit pull request with clear description
4. Address review feedback promptly

---

## License

Copyright © 2025 Advanced Micro Devices Inc. All rights reserved.

This library is provided for internal use and development purposes.

---

*This documentation covers version 1.0.0 of the Exclusion Coverage Parser library. For the latest updates and additional examples, please refer to the source code and test suite.*