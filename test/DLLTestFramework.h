/**
 * DLL Interface Definition for ExclusionParser Dynamic Testing
 * 
 * This header defines the C interface for dynamically loading and testing
 * the ExclusionParser.dll at runtime using LoadLibrary/GetProcAddress.
 * 
 * Author: AMD Verification Team
 * Date: September 13, 2025
 * Version: 2.0.0
 */

#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <memory>

// Forward declarations for C interface structures
struct ExclusionParserHandle;
struct ExclusionWriterHandle;
struct ExclusionDataHandle;

// DLL Function Pointer Types
typedef ExclusionParserHandle* (*CreateParserFunc)();
typedef void (*DestroyParserFunc)(ExclusionParserHandle*);
typedef bool (*ParseFileFunc)(ExclusionParserHandle*, const char*);
typedef int (*GetErrorCountFunc)(ExclusionParserHandle*);
typedef const char* (*GetLastErrorFunc)(ExclusionParserHandle*);

typedef ExclusionWriterHandle* (*CreateWriterFunc)();
typedef void (*DestroyWriterFunc)(ExclusionWriterHandle*);
typedef bool (*OpenFileFunc)(ExclusionWriterHandle*, const char*);
typedef void (*CloseFileFunc)(ExclusionWriterHandle*);
typedef void (*WriteHeaderFunc)(ExclusionWriterHandle*, const char*, const char*, const char*);

typedef ExclusionDataHandle* (*CreateDataFunc)();
typedef void (*DestroyDataFunc)(ExclusionDataHandle*);
typedef void (*SetFilenameFunc)(ExclusionDataHandle*, const char*);
typedef const char* (*GetFilenameFunc)(ExclusionDataHandle*);

// Test result structure
struct TestResult {
    std::string testName;
    bool passed;
    std::string details;
    double executionTime; // in milliseconds
};

class DLLTestFramework {
private:
    HMODULE dllHandle;
    std::string dllPath;
    std::vector<TestResult> results;
    
    // Function pointers
    CreateParserFunc createParser;
    DestroyParserFunc destroyParser;
    ParseFileFunc parseFile;
    GetErrorCountFunc getErrorCount;
    GetLastErrorFunc getLastError;
    
    CreateWriterFunc createWriter;
    DestroyWriterFunc destroyWriter;
    OpenFileFunc openFile;
    CloseFileFunc closeFile;
    WriteHeaderFunc writeHeader;
    
    CreateDataFunc createData;
    DestroyDataFunc destroyData;
    SetFilenameFunc setFilename;
    GetFilenameFunc getFilename;
    
    bool LoadDLL();
    void UnloadDLL();
    bool LoadFunctionPointers();
    void RecordTest(const std::string& testName, bool passed, const std::string& details = "", double executionTime = 0.0);
    
public:
    DLLTestFramework(const std::string& dllPath);
    ~DLLTestFramework();
    
    bool Initialize();
    bool IsLoaded() const { return dllHandle != nullptr; }
    
    // Test categories
    void RunBasicDLLTests();
    void RunParserDLLTests();
    void RunWriterDLLTests();
    void RunDataDLLTests();
    void RunPerformanceTests();
    void RunStressTests();
    
    // Test execution and reporting
    void RunAllTests();
    void PrintResults() const;
    void SaveResultsToFile(const std::string& filename) const;
    
    // Statistics
    int GetTotalTests() const { return static_cast<int>(results.size()); }
    int GetPassedTests() const;
    int GetFailedTests() const;
    double GetSuccessRate() const;
    double GetTotalExecutionTime() const;
};

// Utility functions for creating test data
class DLLTestDataGenerator {
public:
    static std::string CreateSimpleExclusionFile();
    static std::string CreateComplexExclusionFile();
    static std::string CreateMalformedExclusionFile();
    static std::string CreateLargeExclusionFile(int numBlocks = 1000);
    static std::string CreateBinaryTestFile();
    
    // Performance testing data
    static std::vector<std::string> GenerateTestFileSet(int count = 50);
    static void CleanupTestFiles(const std::vector<std::string>& filenames);
};

// Test macros for DLL testing
#define DLL_ASSERT(condition, testName, details) \
    do { \
        bool result = (condition); \
        RecordTest(testName, result, details); \
        if (!result) { \
            std::cout << "[FAIL] " << testName << ": " << details << std::endl; \
        } else { \
            std::cout << "[PASS] " << testName << std::endl; \
        } \
    } while(0)

#define DLL_PERFORMANCE_TEST(testName, codeBlock) \
    do { \
        auto start = std::chrono::high_resolution_clock::now(); \
        codeBlock; \
        auto end = std::chrono::high_resolution_clock::now(); \
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start); \
        double executionTime = duration.count() / 1000.0; \
        RecordTest(testName, true, "Performance test completed", executionTime); \
        std::cout << "[PERF] " << testName << ": " << executionTime << "ms" << std::endl; \
    } while(0)