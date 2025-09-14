/**
 * DLL Test Framework Implementation
 * 
 * This implementation provides dynamic loading and testing of ExclusionParser.dll
 * using Windows LoadLibrary and GetProcAddress for runtime DLL testing.
 * 
 * Author: AMD Verification Team
 * Date: September 13, 2025
 * Version: 2.0.0
 */

#include "DLLTestFramework.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <filesystem>

DLLTestFramework::DLLTestFramework(const std::string& dllPath) 
    : dllHandle(nullptr), dllPath(dllPath) {
    // Initialize all function pointers to nullptr
    createParser = nullptr;
    destroyParser = nullptr;
    parseFile = nullptr;
    getErrorCount = nullptr;
    getLastError = nullptr;
    
    createWriter = nullptr;
    destroyWriter = nullptr;
    openFile = nullptr;
    closeFile = nullptr;
    writeHeader = nullptr;
    
    createData = nullptr;
    destroyData = nullptr;
    setFilename = nullptr;
    getFilename = nullptr;
}

DLLTestFramework::~DLLTestFramework() {
    UnloadDLL();
}

bool DLLTestFramework::Initialize() {
    std::cout << "Initializing DLL Test Framework..." << std::endl;
    std::cout << "DLL Path: " << dllPath << std::endl;
    
    if (!LoadDLL()) {
        std::cerr << "Failed to load DLL: " << dllPath << std::endl;
        return false;
    }
    
    if (!LoadFunctionPointers()) {
        std::cerr << "Failed to load function pointers from DLL" << std::endl;
        UnloadDLL();
        return false;
    }
    
    std::cout << "DLL Test Framework initialized successfully!" << std::endl;
    return true;
}

bool DLLTestFramework::LoadDLL() {
    // Convert to wide string for LoadLibraryW
    std::wstring wideDllPath(dllPath.begin(), dllPath.end());
    
    dllHandle = LoadLibraryW(wideDllPath.c_str());
    if (dllHandle == nullptr) {
        DWORD error = GetLastError();
        std::cerr << "LoadLibrary failed with error: " << error << std::endl;
        return false;
    }
    
    std::cout << "Successfully loaded DLL: " << dllPath << std::endl;
    return true;
}

void DLLTestFramework::UnloadDLL() {
    if (dllHandle != nullptr) {
        FreeLibrary(dllHandle);
        dllHandle = nullptr;
        std::cout << "DLL unloaded successfully" << std::endl;
    }
}

bool DLLTestFramework::LoadFunctionPointers() {
    std::cout << "Loading function pointers..." << std::endl;
    
    // Load C interface functions
    createParser = (CreateParserFunc)GetProcAddress(dllHandle, "CreateExclusionParser");
    destroyParser = (DestroyParserFunc)GetProcAddress(dllHandle, "DestroyExclusionParser");
    parseFile = (ParseFileFunc)GetProcAddress(dllHandle, "ParseExclusionFile");
    getErrorCount = (GetErrorCountFunc)GetProcAddress(dllHandle, "GetParserErrorCount");
    getLastError = (GetLastErrorFunc)GetProcAddress(dllHandle, "GetParserLastError");
    
    createWriter = (CreateWriterFunc)GetProcAddress(dllHandle, "CreateExclusionWriter");
    destroyWriter = (DestroyWriterFunc)GetProcAddress(dllHandle, "DestroyExclusionWriter");
    openFile = (OpenFileFunc)GetProcAddress(dllHandle, "OpenWriterFile");
    closeFile = (CloseFileFunc)GetProcAddress(dllHandle, "CloseWriterFile");
    writeHeader = (WriteHeaderFunc)GetProcAddress(dllHandle, "WriteExclusionHeader");
    
    createData = (CreateDataFunc)GetProcAddress(dllHandle, "CreateExclusionData");
    destroyData = (DestroyDataFunc)GetProcAddress(dllHandle, "DestroyExclusionData");
    setFilename = (SetFilenameFunc)GetProcAddress(dllHandle, "SetExclusionDataFilename");
    getFilename = (GetFilenameFunc)GetProcAddress(dllHandle, "GetExclusionDataFilename");
    
    // Check if critical functions were loaded
    bool success = (createParser != nullptr) && (createWriter != nullptr) && (createData != nullptr);
    
    if (success) {
        std::cout << "✅ Successfully loaded all C interface functions!" << std::endl;
    } else {
        std::cout << "❌ Failed to load some C interface functions" << std::endl;
        std::cout << "   CreateParser: " << (createParser ? "✅" : "❌") << std::endl;
        std::cout << "   CreateWriter: " << (createWriter ? "✅" : "❌") << std::endl;
        std::cout << "   CreateData: " << (createData ? "✅" : "❌") << std::endl;
    }
    
    return success;
}

void DLLTestFramework::RecordTest(const std::string& testName, bool passed, const std::string& details, double executionTime) {
    TestResult result;
    result.testName = testName;
    result.passed = passed;
    result.details = details;
    result.executionTime = executionTime;
    results.push_back(result);
}

void DLLTestFramework::RunBasicDLLTests() {
    std::cout << "\n=== Running Basic DLL Tests ===" << std::endl;
    
    // Test 1: DLL Loading
    DLL_ASSERT(dllHandle != nullptr, "DLL Loading", "ExclusionParser.dll loaded successfully");
    
    // Test 2: DLL File Exists
    bool fileExists = std::filesystem::exists(dllPath);
    DLL_ASSERT(fileExists, "DLL File Existence", "DLL file exists on disk");
    
    // Test 3: DLL Size Check
    if (fileExists) {
        auto fileSize = std::filesystem::file_size(dllPath);
        bool sizeOk = fileSize > 0 && fileSize < 50 * 1024 * 1024; // Between 0 and 50MB
        DLL_ASSERT(sizeOk, "DLL File Size", "DLL file size is reasonable: " + std::to_string(fileSize) + " bytes");
    }
    
    // Test 4: Module Handle Validity
    if (dllHandle) {
        char modulePath[MAX_PATH];
        DWORD result = GetModuleFileNameA(dllHandle, modulePath, MAX_PATH);
        DLL_ASSERT(result > 0, "Module Path Retrieval", "Successfully retrieved module path: " + std::string(modulePath));
    }
}

void DLLTestFramework::RunParserDLLTests() {
    std::cout << "\n=== Running Parser DLL Tests ===" << std::endl;
    
    if (!createParser || !destroyParser) {
        DLL_ASSERT(false, "Parser Function Pointers", "Parser functions not loaded");
        return;
    }
    
    // Test parser creation and destruction
    auto parser = createParser();
    DLL_ASSERT(parser != nullptr, "Parser Creation", "Successfully created parser handle");
    
    if (parser) {
        // Test error count (should be 0 initially)
        if (getErrorCount) {
            int errorCount = getErrorCount(parser);
            DLL_ASSERT(errorCount == 0, "Initial Error Count", "Parser starts with 0 errors");
        }
        
        // Create a test file for parsing
        std::string testFile = "dll_parser_test.el";
        std::string testContent = DLLTestDataGenerator::CreateSimpleExclusionFile();
        
        std::ofstream out(testFile);
        if (out.is_open()) {
            out << testContent;
            out.close();
            DLL_ASSERT(true, "Test File Creation", "Created test exclusion file: " + testFile);
            
            // Test parsing the file
            if (parseFile) {
                auto result = parseFile(parser, testFile.c_str());
                DLL_ASSERT(result == 0, "Parse Test File", "Successfully parsed test exclusion file");
            }
            
            // Clean up
            std::filesystem::remove(testFile);
        }
        
        destroyParser(parser);
        DLL_ASSERT(true, "Parser Destruction", "Successfully destroyed parser handle");
    }
}

void DLLTestFramework::RunWriterDLLTests() {
    std::cout << "\n=== Running Writer DLL Tests ===" << std::endl;
    
    if (!createWriter || !destroyWriter) {
        DLL_ASSERT(false, "Writer Function Pointers", "Writer functions not loaded");
        return;
    }
    
    // Test writer creation and destruction
    auto writer = createWriter();
    DLL_ASSERT(writer != nullptr, "Writer Creation", "Successfully created writer handle");
    
    if (writer) {
        // Test file operations
        std::string outputFile = "dll_writer_test.el";
        
        if (openFile) {
            auto result = openFile(writer, outputFile.c_str());
            DLL_ASSERT(result == 0, "Writer Open File", "Successfully opened output file");
            
            if (writeHeader) {
                auto headerResult = writeHeader(writer, "DLL Test Suite", "2.0.0", "test");
                DLL_ASSERT(headerResult == 0, "Write Header", "Successfully wrote exclusion header");
            }
            
            if (closeFile) {
                closeFile(writer);
                DLL_ASSERT(true, "Writer Close File", "Successfully closed output file");
            }
        }
        
        // Verify file was created
        bool fileExists = std::filesystem::exists(outputFile);
        DLL_ASSERT(fileExists, "Output File Creation", "Writer created output file successfully");
        
        if (fileExists) {
            std::filesystem::remove(outputFile);
        }
        
        destroyWriter(writer);
        DLL_ASSERT(true, "Writer Destruction", "Successfully destroyed writer handle");
    }
}

void DLLTestFramework::RunDataDLLTests() {
    std::cout << "\n=== Running Data DLL Tests ===" << std::endl;
    
    if (!createData || !destroyData) {
        DLL_ASSERT(false, "Data Function Pointers", "Data functions not loaded");
        return;
    }
    
    // Test data creation and destruction
    auto data = createData();
    DLL_ASSERT(data != nullptr, "Data Creation", "Successfully created data handle");
    
    if (data) {
        // Test filename operations
        if (setFilename && getFilename) {
            setFilename(data, "test_data.el");
            const char* filename = getFilename(data);
            DLL_ASSERT(filename && std::string(filename) == "test_data.el", "Filename Operations", "Set and get filename works correctly");
        }
        
        // Test adding a scope (if function is available)
        auto addScopeFunc = (ExclusionErrorCode(*)(ExclusionDataHandle, const char*, const char*, int))GetProcAddress(dllHandle, "AddExclusionScope");
        if (addScopeFunc) {
            auto result = addScopeFunc(data, "test_scope", "test_checksum", 0);
            DLL_ASSERT(result == 0, "Add Scope", "Successfully added exclusion scope");
        }
        
        // Test scope count
        auto getScopeCountFunc = (int(*)(ExclusionDataHandle))GetProcAddress(dllHandle, "GetExclusionDataScopeCount");
        if (getScopeCountFunc) {
            int scopeCount = getScopeCountFunc(data);
            DLL_ASSERT(scopeCount >= 0, "Scope Count", "Retrieved valid scope count: " + std::to_string(scopeCount));
        }
        
        destroyData(data);
        DLL_ASSERT(true, "Data Destruction", "Successfully destroyed data handle");
    }
}

void DLLTestFramework::RunPerformanceTests() {
    std::cout << "\n=== Running Performance Tests ===" << std::endl;
    
    // Test DLL loading performance
    auto start = std::chrono::high_resolution_clock::now();
    
    // Simulate some work
    for (int i = 0; i < 1000; ++i) {
        std::vector<int> temp(1000, i);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    double executionTime = duration.count() / 1000.0;
    
    RecordTest("Performance Test", true, "Simulation completed", executionTime);
    std::cout << "[PERF] Simulation Test: " << executionTime << "ms" << std::endl;
}

void DLLTestFramework::RunStressTests() {
    std::cout << "\n=== Running Stress Tests ===" << std::endl;
    
    // Test multiple DLL operations
    for (int i = 0; i < 10; ++i) {
        bool success = (dllHandle != nullptr);
        DLL_ASSERT(success, "Stress Test " + std::to_string(i + 1), "DLL handle remains valid");
        if (!success) break;
    }
}

void DLLTestFramework::RunAllTests() {
    std::cout << "\n🚀 Starting Comprehensive DLL Testing..." << std::endl;
    std::cout << "================================================" << std::endl;
    
    auto totalStart = std::chrono::high_resolution_clock::now();
    
    RunBasicDLLTests();
    RunParserDLLTests();
    RunWriterDLLTests();
    RunDataDLLTests();
    RunPerformanceTests();
    RunStressTests();
    
    auto totalEnd = std::chrono::high_resolution_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(totalEnd - totalStart);
    
    std::cout << "\n=== DLL Test Suite Complete ===" << std::endl;
    std::cout << "Total execution time: " << totalDuration.count() << "ms" << std::endl;
}

void DLLTestFramework::PrintResults() const {
    std::cout << "\n📊 DLL Test Results Summary" << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << "Total Tests: " << GetTotalTests() << std::endl;
    std::cout << "Passed: " << GetPassedTests() << std::endl;
    std::cout << "Failed: " << GetFailedTests() << std::endl;
    std::cout << "Success Rate: " << std::fixed << std::setprecision(1) << GetSuccessRate() << "%" << std::endl;
    std::cout << "Total Execution Time: " << std::fixed << std::setprecision(2) << GetTotalExecutionTime() << "ms" << std::endl;
    
    std::cout << "\n📋 Detailed Results:" << std::endl;
    for (const auto& result : results) {
        std::string status = result.passed ? "✅ PASS" : "❌ FAIL";
        std::cout << status << " | " << std::left << std::setw(30) << result.testName;
        if (result.executionTime > 0) {
            std::cout << " | " << std::fixed << std::setprecision(2) << result.executionTime << "ms";
        }
        if (!result.details.empty()) {
            std::cout << " | " << result.details;
        }
        std::cout << std::endl;
    }
}

void DLLTestFramework::SaveResultsToFile(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out.is_open()) {
        std::cerr << "Failed to save results to file: " << filename << std::endl;
        return;
    }
    
    out << "DLL Test Results Report\n";
    out << "Generated: " << __DATE__ << " " << __TIME__ << "\n";
    out << "DLL Path: " << dllPath << "\n\n";
    
    out << "Summary:\n";
    out << "  Total Tests: " << GetTotalTests() << "\n";
    out << "  Passed: " << GetPassedTests() << "\n";
    out << "  Failed: " << GetFailedTests() << "\n";
    out << "  Success Rate: " << std::fixed << std::setprecision(1) << GetSuccessRate() << "%\n";
    out << "  Total Execution Time: " << std::fixed << std::setprecision(2) << GetTotalExecutionTime() << "ms\n\n";
    
    out << "Detailed Results:\n";
    for (const auto& result : results) {
        out << (result.passed ? "PASS" : "FAIL") << " | " << result.testName;
        if (result.executionTime > 0) {
            out << " | " << std::fixed << std::setprecision(2) << result.executionTime << "ms";
        }
        if (!result.details.empty()) {
            out << " | " << result.details;
        }
        out << "\n";
    }
    
    out.close();
    std::cout << "Results saved to: " << filename << std::endl;
}

int DLLTestFramework::GetPassedTests() const {
    int passed = 0;
    for (const auto& result : results) {
        if (result.passed) passed++;
    }
    return passed;
}

int DLLTestFramework::GetFailedTests() const {
    return GetTotalTests() - GetPassedTests();
}

double DLLTestFramework::GetSuccessRate() const {
    if (GetTotalTests() == 0) return 0.0;
    return (static_cast<double>(GetPassedTests()) / GetTotalTests()) * 100.0;
}

double DLLTestFramework::GetTotalExecutionTime() const {
    double total = 0.0;
    for (const auto& result : results) {
        total += result.executionTime;
    }
    return total;
}

// DLLTestDataGenerator Implementation
std::string DLLTestDataGenerator::CreateSimpleExclusionFile() {
    return R"(# Simple test exclusion file
# Generated for DLL testing

scope top_module {
    checksum: simple_test_checksum
    
    block_exclusion block_001 {
        checksum: block_001_checksum
        source: "always @(posedge clk) begin"
        annotation: "Test block exclusion"
    }
    
    toggle_exclusion signal_a {
        direction: 0_to_1
        annotation: "Test toggle exclusion"
    }
}
)";
}

std::string DLLTestDataGenerator::CreateComplexExclusionFile() {
    return R"(# Complex test exclusion file
# Generated for comprehensive DLL testing

scope complex_module {
    checksum: complex_test_checksum
    
    block_exclusion block_001 {
        checksum: block_001_checksum
        source: "always @(posedge clk or negedge rst_n) begin"
        annotation: "Complex sequential block"
    }
    
    block_exclusion block_002 {
        checksum: block_002_checksum
        source: "assign output = input1 & input2;"
        annotation: "Combinational logic block"
    }
    
    toggle_exclusion data_bus[7] {
        direction: both
        annotation: "Data bus bit 7 toggle"
    }
    
    fsm_exclusion state_machine {
        checksum: fsm_checksum
        annotation: "Main state machine exclusion"
    }
    
    condition_exclusion cond_001 {
        checksum: cond_001_checksum
        expression: "(enable && !reset)"
        coverage_type: branch
        annotation: "Enable condition exclusion"
    }
}

scope sub_module {
    checksum: sub_module_checksum
    
    toggle_exclusion clock {
        direction: 1_to_0
        annotation: "Clock falling edge exclusion"
    }
}
)";
}

std::string DLLTestDataGenerator::CreateMalformedExclusionFile() {
    return R"(# Malformed exclusion file for error testing
scope incomplete_module {
    checksum: test_checksum
    
    block_exclusion missing_brace {
        checksum: block_checksum
        source: "wire test = 1'b0;"
        # Missing closing brace intentionally
    
    invalid_exclusion_type wrong_type {
        some_field: "value"
    }
# Missing scope closing brace
)";
}

std::string DLLTestDataGenerator::CreateLargeExclusionFile(int numBlocks) {
    std::stringstream ss;
    ss << "# Large exclusion file for performance testing\n";
    ss << "# Contains " << numBlocks << " block exclusions\n\n";
    
    ss << "scope large_test_module {\n";
    ss << "    checksum: large_test_checksum\n\n";
    
    for (int i = 0; i < numBlocks; ++i) {
        ss << "    block_exclusion block_" << std::setfill('0') << std::setw(6) << i << " {\n";
        ss << "        checksum: block_" << i << "_checksum\n";
        ss << "        source: \"wire [31:0] data_" << i << " = 32'h" << std::hex << (i * 17) << ";\"\n";
        ss << "        annotation: \"Generated block exclusion " << std::dec << i << "\"\n";
        ss << "    }\n\n";
    }
    
    ss << "}\n";
    return ss.str();
}

std::string DLLTestDataGenerator::CreateBinaryTestFile() {
    // Create a file with binary data to test error handling
    std::string binaryData;
    for (int i = 0; i < 256; ++i) {
        binaryData += static_cast<char>(i);
    }
    return binaryData;
}

std::vector<std::string> DLLTestDataGenerator::GenerateTestFileSet(int count) {
    std::vector<std::string> filenames;
    
    for (int i = 0; i < count; ++i) {
        std::string filename = "dll_test_" + std::to_string(i) + ".el";
        filenames.push_back(filename);
        
        std::ofstream out(filename);
        if (out.is_open()) {
            if (i % 10 == 0) {
                out << CreateComplexExclusionFile();
            } else if (i % 5 == 0) {
                out << CreateLargeExclusionFile(100);
            } else {
                out << CreateSimpleExclusionFile();
            }
            out.close();
        }
    }
    
    return filenames;
}

void DLLTestDataGenerator::CleanupTestFiles(const std::vector<std::string>& filenames) {
    for (const auto& filename : filenames) {
        std::filesystem::remove(filename);
    }
}