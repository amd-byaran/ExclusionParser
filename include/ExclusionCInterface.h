/**
 * C Interface Wrapper for ExclusionParser DLL
 * 
 * This header provides C-compatible function exports for the ExclusionParser DLL
 * to enable dynamic loading and testing through LoadLibrary/GetProcAddress.
 * 
 * Author: AMD Verification Team
 * Date: September 13, 2025
 * Version: 2.0.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// DLL Export macro
#ifdef EXCLUSION_PARSER_EXPORTS
#define EXCLUSION_API __declspec(dllexport)
#else
#define EXCLUSION_API __declspec(dllimport)
#endif

// Opaque handle types for C interface
typedef struct ExclusionParserHandle_* ExclusionParserHandle;
typedef struct ExclusionWriterHandle_* ExclusionWriterHandle;
typedef struct ExclusionDataHandle_* ExclusionDataHandle;

// Error codes
typedef enum {
    EXCLUSION_SUCCESS = 0,
    EXCLUSION_ERROR_NULL_POINTER = -1,
    EXCLUSION_ERROR_FILE_NOT_FOUND = -2,
    EXCLUSION_ERROR_PARSE_FAILED = -3,
    EXCLUSION_ERROR_WRITE_FAILED = -4,
    EXCLUSION_ERROR_INVALID_FORMAT = -5,
    EXCLUSION_ERROR_MEMORY_ALLOCATION = -6
} ExclusionErrorCode;

// ExclusionParser C Interface
EXCLUSION_API ExclusionParserHandle CreateExclusionParser();
EXCLUSION_API void DestroyExclusionParser(ExclusionParserHandle parser);
EXCLUSION_API ExclusionErrorCode ParseExclusionFile(ExclusionParserHandle parser, const char* filename);
EXCLUSION_API int GetParserErrorCount(ExclusionParserHandle parser);
EXCLUSION_API const char* GetParserLastError(ExclusionParserHandle parser);
EXCLUSION_API ExclusionDataHandle GetParserData(ExclusionParserHandle parser);

// ExclusionWriter C Interface  
EXCLUSION_API ExclusionWriterHandle CreateExclusionWriter();
EXCLUSION_API void DestroyExclusionWriter(ExclusionWriterHandle writer);
EXCLUSION_API ExclusionErrorCode OpenWriterFile(ExclusionWriterHandle writer, const char* filename);
EXCLUSION_API void CloseWriterFile(ExclusionWriterHandle writer);
EXCLUSION_API ExclusionErrorCode WriteExclusionHeader(ExclusionWriterHandle writer, const char* generatedBy, const char* version, const char* mode);
EXCLUSION_API ExclusionErrorCode WriteExclusionData(ExclusionWriterHandle writer, ExclusionDataHandle data);

// ExclusionData C Interface
EXCLUSION_API ExclusionDataHandle CreateExclusionData();
EXCLUSION_API void DestroyExclusionData(ExclusionDataHandle data);
EXCLUSION_API void SetExclusionDataFilename(ExclusionDataHandle data, const char* filename);
EXCLUSION_API const char* GetExclusionDataFilename(ExclusionDataHandle data);
EXCLUSION_API int GetExclusionDataScopeCount(ExclusionDataHandle data);

// Scope operations
EXCLUSION_API ExclusionErrorCode AddExclusionScope(ExclusionDataHandle data, const char* scopeName, const char* checksum, int isModule);
EXCLUSION_API int GetScopeBlockCount(ExclusionDataHandle data, const char* scopeName);
EXCLUSION_API int GetScopeToggleCount(ExclusionDataHandle data, const char* scopeName);
EXCLUSION_API int GetScopeFsmCount(ExclusionDataHandle data, const char* scopeName);
EXCLUSION_API int GetScopeConditionCount(ExclusionDataHandle data, const char* scopeName);

// Block exclusion operations
EXCLUSION_API ExclusionErrorCode AddBlockExclusion(ExclusionDataHandle data, const char* scopeName, 
                                                  const char* blockId, const char* checksum, 
                                                  const char* sourceCode, const char* annotation);

// Toggle exclusion operations
EXCLUSION_API ExclusionErrorCode AddToggleExclusion(ExclusionDataHandle data, const char* scopeName,
                                                   const char* signalName, int direction, int bitIndex,
                                                   const char* description, const char* annotation);

// FSM exclusion operations
EXCLUSION_API ExclusionErrorCode AddFsmStateExclusion(ExclusionDataHandle data, const char* scopeName,
                                                     const char* fsmName, const char* checksum, const char* annotation);
EXCLUSION_API ExclusionErrorCode AddFsmTransitionExclusion(ExclusionDataHandle data, const char* scopeName,
                                                          const char* fsmName, const char* fromState, const char* toState,
                                                          const char* checksum, const char* annotation);

// Condition exclusion operations
EXCLUSION_API ExclusionErrorCode AddConditionExclusion(ExclusionDataHandle data, const char* scopeName,
                                                      const char* conditionId, const char* checksum,
                                                      const char* expression, const char* parameters,
                                                      const char* coverage, const char* annotation);

// Utility functions
EXCLUSION_API const char* GetExclusionErrorString(ExclusionErrorCode code);
EXCLUSION_API const char* GetExclusionParserVersion();
EXCLUSION_API int IsExclusionFileValid(const char* filename);

// Testing support functions
EXCLUSION_API ExclusionErrorCode CreateTestExclusionFile(const char* filename, int complexity);
EXCLUSION_API ExclusionErrorCode ValidateExclusionData(ExclusionDataHandle data);
EXCLUSION_API void PrintExclusionDataSummary(ExclusionDataHandle data);

#ifdef __cplusplus
}
#endif