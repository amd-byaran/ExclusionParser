/**
 * C Interface Implementation for ExclusionParser DLL
 * 
 * This implementation wraps the C++ ExclusionParser classes in C functions
 * for dynamic loading and testing purposes.
 * 
 * Author: AMD Verification Team
 * Date: September 13, 2025
 * Version: 2.0.0
 */

#include "ExclusionCInterface.h"
#include "ExclusionParser.h"
#include "ExclusionWriter.h"
#include "ExclusionData.h"
#include <memory>
#include <string>
#include <map>
#include <fstream>
#include <sstream>

// Internal handle structures
struct ExclusionParserHandle_ {
    std::unique_ptr<ExclusionParser::ExclusionParser> parser;
    std::string lastError;
    
    ExclusionParserHandle_() : parser(std::make_unique<ExclusionParser::ExclusionParser>()) {}
};

struct ExclusionWriterHandle_ {
    std::unique_ptr<ExclusionParser::ExclusionWriter> writer;
    std::string lastError;
    
    ExclusionWriterHandle_() : writer(std::make_unique<ExclusionParser::ExclusionWriter>()) {}
};

struct ExclusionDataHandle_ {
    std::unique_ptr<ExclusionParser::ExclusionData> data;
    std::string lastError;
    
    ExclusionDataHandle_() : data(std::make_unique<ExclusionParser::ExclusionData>()) {}
};

// Error string mapping
static const std::map<ExclusionErrorCode, std::string> errorStrings = {
    {EXCLUSION_SUCCESS, "Success"},
    {EXCLUSION_ERROR_NULL_POINTER, "Null pointer provided"},
    {EXCLUSION_ERROR_FILE_NOT_FOUND, "File not found"},
    {EXCLUSION_ERROR_PARSE_FAILED, "Parse operation failed"},
    {EXCLUSION_ERROR_WRITE_FAILED, "Write operation failed"},
    {EXCLUSION_ERROR_INVALID_FORMAT, "Invalid file format"},
    {EXCLUSION_ERROR_MEMORY_ALLOCATION, "Memory allocation failed"}
};

// Helper function to convert C++ exceptions to error codes
template<typename Func>
ExclusionErrorCode SafeExecute(Func&& func) {
    try {
        return func();
    } catch (const std::bad_alloc&) {
        return EXCLUSION_ERROR_MEMORY_ALLOCATION;
    } catch (const std::exception&) {
        return EXCLUSION_ERROR_PARSE_FAILED;
    } catch (...) {
        return EXCLUSION_ERROR_PARSE_FAILED;
    }
}

// ExclusionParser C Interface Implementation
extern "C" {

EXCLUSION_API ExclusionParserHandle CreateExclusionParser() {
    return SafeExecute([&]() -> ExclusionParserHandle {
        return new ExclusionParserHandle_();
    }) ? nullptr : new ExclusionParserHandle_();
}

EXCLUSION_API void DestroyExclusionParser(ExclusionParserHandle parser) {
    delete parser;
}

EXCLUSION_API ExclusionErrorCode ParseExclusionFile(ExclusionParserHandle parser, const char* filename) {
    if (!parser || !filename) return EXCLUSION_ERROR_NULL_POINTER;
    
    return SafeExecute([&]() -> ExclusionErrorCode {
        bool success = parser->parser->parseFile(filename);
        return success ? EXCLUSION_SUCCESS : EXCLUSION_ERROR_PARSE_FAILED;
    });
}

EXCLUSION_API int GetParserErrorCount(ExclusionParserHandle parser) {
    if (!parser) return -1;
    
    try {
        return parser->parser->getErrorCount();
    } catch (...) {
        return -1;
    }
}

EXCLUSION_API const char* GetParserLastError(ExclusionParserHandle parser) {
    if (!parser) return "Invalid parser handle";
    
    try {
        parser->lastError = parser->parser->getLastError();
        return parser->lastError.c_str();
    } catch (...) {
        return "Exception occurred getting last error";
    }
}

EXCLUSION_API ExclusionDataHandle GetParserData(ExclusionParserHandle parser) {
    if (!parser) return nullptr;
    
    try {
        // This would need to be implemented to extract data from parser
        // For now, return a new data handle
        return CreateExclusionData();
    } catch (...) {
        return nullptr;
    }
}

// ExclusionWriter C Interface Implementation
EXCLUSION_API ExclusionWriterHandle CreateExclusionWriter() {
    try {
        return new ExclusionWriterHandle_();
    } catch (...) {
        return nullptr;
    }
}

EXCLUSION_API void DestroyExclusionWriter(ExclusionWriterHandle writer) {
    delete writer;
}

EXCLUSION_API ExclusionErrorCode OpenWriterFile(ExclusionWriterHandle writer, const char* filename) {
    if (!writer || !filename) return EXCLUSION_ERROR_NULL_POINTER;
    
    return SafeExecute([&]() -> ExclusionErrorCode {
        bool success = writer->writer->open(filename);
        return success ? EXCLUSION_SUCCESS : EXCLUSION_ERROR_WRITE_FAILED;
    });
}

EXCLUSION_API void CloseWriterFile(ExclusionWriterHandle writer) {
    if (writer) {
        try {
            writer->writer->close();
        } catch (...) {
            // Ignore exceptions on close
        }
    }
}

EXCLUSION_API ExclusionErrorCode WriteExclusionHeader(ExclusionWriterHandle writer, const char* generatedBy, const char* version, const char* mode) {
    if (!writer || !generatedBy || !version || !mode) return EXCLUSION_ERROR_NULL_POINTER;
    
    return SafeExecute([&]() -> ExclusionErrorCode {
        writer->writer->writeHeader(generatedBy, version, mode);
        return EXCLUSION_SUCCESS;
    });
}

EXCLUSION_API ExclusionErrorCode WriteExclusionData(ExclusionWriterHandle writer, ExclusionDataHandle data) {
    if (!writer || !data) return EXCLUSION_ERROR_NULL_POINTER;
    
    return SafeExecute([&]() -> ExclusionErrorCode {
        writer->writer->writeData(*data->data);
        return EXCLUSION_SUCCESS;
    });
}

// ExclusionData C Interface Implementation
EXCLUSION_API ExclusionDataHandle CreateExclusionData() {
    try {
        return new ExclusionDataHandle_();
    } catch (...) {
        return nullptr;
    }
}

EXCLUSION_API void DestroyExclusionData(ExclusionDataHandle data) {
    delete data;
}

EXCLUSION_API void SetExclusionDataFilename(ExclusionDataHandle data, const char* filename) {
    if (data && filename) {
        try {
            data->data->fileName = filename;
        } catch (...) {
            // Ignore exceptions
        }
    }
}

EXCLUSION_API const char* GetExclusionDataFilename(ExclusionDataHandle data) {
    if (!data) return "";
    
    try {
        return data->data->fileName.c_str();
    } catch (...) {
        return "";
    }
}

EXCLUSION_API int GetExclusionDataScopeCount(ExclusionDataHandle data) {
    if (!data) return -1;
    
    try {
        return static_cast<int>(data->data->scopes.size());
    } catch (...) {
        return -1;
    }
}

// Scope operations
EXCLUSION_API ExclusionErrorCode AddExclusionScope(ExclusionDataHandle data, const char* scopeName, const char* checksum, int isModule) {
    if (!data || !scopeName || !checksum) return EXCLUSION_ERROR_NULL_POINTER;
    
    return SafeExecute([&]() -> ExclusionErrorCode {
        ExclusionParser::ExclusionScope scope(scopeName, checksum, isModule != 0);
        data->data->scopes[scopeName] = scope;
        return EXCLUSION_SUCCESS;
    });
}

EXCLUSION_API int GetScopeBlockCount(ExclusionDataHandle data, const char* scopeName) {
    if (!data || !scopeName) return -1;
    
    try {
        auto it = data->data->scopes.find(scopeName);
        if (it != data->data->scopes.end()) {
            return static_cast<int>(it->second.blockExclusions.size());
        }
        return 0;
    } catch (...) {
        return -1;
    }
}

EXCLUSION_API int GetScopeToggleCount(ExclusionDataHandle data, const char* scopeName) {
    if (!data || !scopeName) return -1;
    
    try {
        auto it = data->data->scopes.find(scopeName);
        if (it != data->data->scopes.end()) {
            return static_cast<int>(it->second.toggleExclusions.size());
        }
        return 0;
    } catch (...) {
        return -1;
    }
}

EXCLUSION_API int GetScopeFsmCount(ExclusionDataHandle data, const char* scopeName) {
    if (!data || !scopeName) return -1;
    
    try {
        auto it = data->data->scopes.find(scopeName);
        if (it != data->data->scopes.end()) {
            return static_cast<int>(it->second.fsmExclusions.size());
        }
        return 0;
    } catch (...) {
        return -1;
    }
}

EXCLUSION_API int GetScopeConditionCount(ExclusionDataHandle data, const char* scopeName) {
    if (!data || !scopeName) return -1;
    
    try {
        auto it = data->data->scopes.find(scopeName);
        if (it != data->data->scopes.end()) {
            return static_cast<int>(it->second.conditionExclusions.size());
        }
        return 0;
    } catch (...) {
        return -1;
    }
}

// Block exclusion operations
EXCLUSION_API ExclusionErrorCode AddBlockExclusion(ExclusionDataHandle data, const char* scopeName, 
                                                  const char* blockId, const char* checksum, 
                                                  const char* sourceCode, const char* annotation) {
    if (!data || !scopeName || !blockId || !checksum || !sourceCode || !annotation) {
        return EXCLUSION_ERROR_NULL_POINTER;
    }
    
    return SafeExecute([&]() -> ExclusionErrorCode {
        ExclusionParser::BlockExclusion block(blockId, checksum, sourceCode, annotation);
        data->data->scopes[scopeName].blockExclusions[blockId] = block;
        return EXCLUSION_SUCCESS;
    });
}

// Toggle exclusion operations
EXCLUSION_API ExclusionErrorCode AddToggleExclusion(ExclusionDataHandle data, const char* scopeName,
                                                   const char* signalName, int direction, int bitIndex,
                                                   const char* description, const char* annotation) {
    if (!data || !scopeName || !signalName || !description || !annotation) {
        return EXCLUSION_ERROR_NULL_POINTER;
    }
    
    return SafeExecute([&]() -> ExclusionErrorCode {
        auto toggleDir = static_cast<ExclusionParser::ToggleDirection>(direction);
        std::optional<int> bitIdx = (bitIndex >= 0) ? std::optional<int>(bitIndex) : std::nullopt;
        ExclusionParser::ToggleExclusion toggle(toggleDir, signalName, bitIdx, description, annotation);
        data->data->scopes[scopeName].toggleExclusions[signalName].push_back(toggle);
        return EXCLUSION_SUCCESS;
    });
}

// FSM exclusion operations
EXCLUSION_API ExclusionErrorCode AddFsmStateExclusion(ExclusionDataHandle data, const char* scopeName,
                                                     const char* fsmName, const char* checksum, const char* annotation) {
    if (!data || !scopeName || !fsmName || !checksum || !annotation) {
        return EXCLUSION_ERROR_NULL_POINTER;
    }
    
    return SafeExecute([&]() -> ExclusionErrorCode {
        ExclusionParser::FsmExclusion fsm(fsmName, checksum, annotation);
        data->data->scopes[scopeName].fsmExclusions[fsmName].push_back(fsm);
        return EXCLUSION_SUCCESS;
    });
}

EXCLUSION_API ExclusionErrorCode AddFsmTransitionExclusion(ExclusionDataHandle data, const char* scopeName,
                                                          const char* fsmName, const char* fromState, const char* toState,
                                                          const char* checksum, const char* annotation) {
    if (!data || !scopeName || !fsmName || !fromState || !toState || !checksum || !annotation) {
        return EXCLUSION_ERROR_NULL_POINTER;
    }
    
    return SafeExecute([&]() -> ExclusionErrorCode {
        ExclusionParser::FsmExclusion fsm(fsmName, fromState, toState, checksum, annotation);
        data->data->scopes[scopeName].fsmExclusions[fsmName].push_back(fsm);
        return EXCLUSION_SUCCESS;
    });
}

// Condition exclusion operations
EXCLUSION_API ExclusionErrorCode AddConditionExclusion(ExclusionDataHandle data, const char* scopeName,
                                                      const char* conditionId, const char* checksum,
                                                      const char* expression, const char* parameters,
                                                      const char* coverage, const char* annotation) {
    if (!data || !scopeName || !conditionId || !checksum || !expression || !parameters || !coverage || !annotation) {
        return EXCLUSION_ERROR_NULL_POINTER;
    }
    
    return SafeExecute([&]() -> ExclusionErrorCode {
        ExclusionParser::ConditionExclusion condition(conditionId, checksum, expression, parameters, coverage, annotation);
        data->data->scopes[scopeName].conditionExclusions[conditionId] = condition;
        return EXCLUSION_SUCCESS;
    });
}

// Utility functions
EXCLUSION_API const char* GetExclusionErrorString(ExclusionErrorCode code) {
    auto it = errorStrings.find(code);
    return it != errorStrings.end() ? it->second.c_str() : "Unknown error";
}

EXCLUSION_API const char* GetExclusionParserVersion() {
    return "2.0.0";
}

EXCLUSION_API int IsExclusionFileValid(const char* filename) {
    if (!filename) return 0;
    
    try {
        std::ifstream file(filename);
        return file.is_open() ? 1 : 0;
    } catch (...) {
        return 0;
    }
}

// Testing support functions
EXCLUSION_API ExclusionErrorCode CreateTestExclusionFile(const char* filename, int complexity) {
    if (!filename) return EXCLUSION_ERROR_NULL_POINTER;
    
    return SafeExecute([&]() -> ExclusionErrorCode {
        std::ofstream file(filename);
        if (!file.is_open()) return EXCLUSION_ERROR_WRITE_FAILED;
        
        file << "# Test exclusion file (complexity: " << complexity << ")\n";
        file << "# Generated by ExclusionParser C Interface\n\n";
        
        file << "scope test_module {\n";
        file << "    checksum: test_checksum\n\n";
        
        for (int i = 0; i < complexity; ++i) {
            file << "    block_exclusion block_" << i << " {\n";
            file << "        checksum: block_" << i << "_checksum\n";
            file << "        source: \"wire [31:0] data_" << i << " = 32'h" << std::hex << (i * 17) << ";\"\n";
            file << "        annotation: \"Test block " << std::dec << i << "\"\n";
            file << "    }\n\n";
        }
        
        file << "}\n";
        file.close();
        
        return EXCLUSION_SUCCESS;
    });
}

EXCLUSION_API ExclusionErrorCode ValidateExclusionData(ExclusionDataHandle data) {
    if (!data) return EXCLUSION_ERROR_NULL_POINTER;
    
    try {
        // Basic validation
        if (data->data->fileName.empty()) return EXCLUSION_ERROR_INVALID_FORMAT;
        if (data->data->scopes.empty()) return EXCLUSION_ERROR_INVALID_FORMAT;
        
        return EXCLUSION_SUCCESS;
    } catch (...) {
        return EXCLUSION_ERROR_PARSE_FAILED;
    }
}

EXCLUSION_API void PrintExclusionDataSummary(ExclusionDataHandle data) {
    if (!data) return;
    
    try {
        std::cout << "ExclusionData Summary:\n";
        std::cout << "  Filename: " << data->data->fileName << "\n";
        std::cout << "  Scopes: " << data->data->scopes.size() << "\n";
        
        for (const auto& scope : data->data->scopes) {
            std::cout << "    " << scope.first << ": " 
                      << scope.second.blockExclusions.size() << " blocks, "
                      << scope.second.toggleExclusions.size() << " toggles, "
                      << scope.second.fsmExclusions.size() << " fsms, "
                      << scope.second.conditionExclusions.size() << " conditions\n";
        }
    } catch (...) {
        std::cout << "Error printing exclusion data summary\n";
    }
}

} // extern "C"