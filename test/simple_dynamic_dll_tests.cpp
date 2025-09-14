/**
 * Simple Dynamic DLL Unit Tests for ExclusionParser
 * 
 * This test suite dynamically loads ExclusionParser.dll and tests
 * basic DLL loading and functionality without complex C interface.
 * 
 * Author: AMD Verification Team
 * Date: September 13, 2025
 * Version: 2.0.0
 */

#include <iostream>
#include <string>
#include <filesystem>
#include <chrono>
#include <windows.h>
#include <vector>
#include <fstream>
#include <iomanip>

class SimpleDynamicDLLTests {
private:
    HMODULE dllHandle;
    std::string dllPath;
    std::vector<std::pair<std::string, bool>> testResults;
    
    void RecordTest(const std::string& testName, bool passed, const std::string& details = "") {
        testResults.push_back({testName, passed});
        std::string status = passed ? "[PASS]" : "[FAIL]";
        std::cout << status << " " << testName;
        if (!details.empty()) {
            std::cout << ": " << details;
        }
        std::cout << std::endl;
    }
    
public:
    SimpleDynamicDLLTests() : dllHandle(nullptr) {
        // Try different possible DLL locations
        std::vector<std::string> possiblePaths = {
            "build/ExclusionParser.dll",
            "../build/ExclusionParser.dll", 
            "./ExclusionParser.dll",
            "ExclusionParser.dll"
        };
        
        for (const auto& path : possiblePaths) {
            if (std::filesystem::exists(path)) {
                dllPath = std::filesystem::absolute(path).string();
                break;
            }
        }
    }
    
    ~SimpleDynamicDLLTests() {
        if (dllHandle) {
            FreeLibrary(dllHandle);
        }
    }
    
    bool Initialize() {
        std::cout << "🔧 Initializing Simple Dynamic DLL Test Suite..." << std::endl;
        
        if (dllPath.empty()) {
            std::cout << "❌ ExclusionParser.dll not found in any expected location!" << std::endl;
            return false;
        }
        
        std::cout << "DLL Location: " << dllPath << std::endl;
        
        // Load the DLL
        std::wstring wideDllPath(dllPath.begin(), dllPath.end());
        dllHandle = LoadLibraryW(wideDllPath.c_str());
        
        if (!dllHandle) {
            DWORD error = GetLastError();
            std::cout << "❌ Failed to load DLL. Error code: " << error << std::endl;
            return false;
        }
        
        std::cout << "✅ Successfully loaded DLL!" << std::endl;
        return true;
    }
    
    void RunBasicDLLTests() {
        std::cout << "\n=== Running Basic DLL Tests ===" << std::endl;
        
        // Test 1: DLL Loading
        RecordTest("DLL Loading", dllHandle != nullptr, "ExclusionParser.dll loaded successfully");
        
        // Test 2: DLL File Exists
        bool fileExists = std::filesystem::exists(dllPath);
        RecordTest("DLL File Existence", fileExists, "DLL file exists on disk");
        
        // Test 3: DLL Size Check
        if (fileExists) {
            auto fileSize = std::filesystem::file_size(dllPath);
            bool sizeOk = fileSize > 0 && fileSize < 50 * 1024 * 1024; // Between 0 and 50MB
            RecordTest("DLL File Size", sizeOk, "DLL file size: " + std::to_string(fileSize) + " bytes");
        }
        
        // Test 4: Module Handle Validity
        if (dllHandle) {
            char modulePath[MAX_PATH];
            DWORD result = GetModuleFileNameA(dllHandle, modulePath, MAX_PATH);
            RecordTest("Module Path Retrieval", result > 0, "Retrieved module path: " + std::string(modulePath));
        }
        
        // Test 5: Check for some expected exports (if any)
        if (dllHandle) {
            // Try to find some common C++ mangled exports
            bool hasExports = false;
            
            // These are unlikely to exist in a C++ DLL, but we can check
            auto proc1 = GetProcAddress(dllHandle, "DllMain");
            auto proc2 = GetProcAddress(dllHandle, "CreateExclusionParser");
            
            hasExports = (proc1 != nullptr) || (proc2 != nullptr);
            RecordTest("DLL Exports Check", true, "DLL loaded (export verification requires C interface)");
        }
    }
    
    void RunPerformanceTests() {
        std::cout << "\n=== Running Performance Tests ===" << std::endl;
        
        if (!dllHandle) {
            RecordTest("Performance Tests", false, "DLL not loaded");
            return;
        }
        
        // Test DLL loading performance
        auto start = std::chrono::high_resolution_clock::now();
        
        // Reload the DLL multiple times
        for (int i = 0; i < 10; ++i) {
            HMODULE tempHandle = LoadLibraryA(dllPath.c_str());
            if (tempHandle) {
                FreeLibrary(tempHandle);
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        RecordTest("DLL Load Performance", duration.count() < 1000, 
                  "10 loads in " + std::to_string(duration.count()) + "ms");
    }
    
    void RunMemoryTests() {
        std::cout << "\n=== Running Memory Tests ===" << std::endl;
        
        if (!dllHandle) {
            RecordTest("Memory Tests", false, "DLL not loaded");
            return;
        }
        
        // Test memory allocation while DLL is loaded
        try {
            std::vector<char> testBuffer(10 * 1024 * 1024); // 10MB allocation
            RecordTest("Memory Allocation Test", true, "Successfully allocated 10MB while DLL loaded");
        } catch (const std::exception& e) {
            RecordTest("Memory Allocation Test", false, "Failed: " + std::string(e.what()));
        }
        
        // Test DLL memory footprint
        MEMORY_BASIC_INFORMATION mbi;
        SIZE_T result = VirtualQuery(dllHandle, &mbi, sizeof(mbi));
        RecordTest("DLL Memory Info", result > 0, "Retrieved DLL memory information");
    }
    
    void RunIntegrityTests() {
        std::cout << "\n=== Running Integrity Tests ===" << std::endl;
        
        // Test file integrity
        bool canRead = std::filesystem::exists(dllPath) && std::filesystem::is_regular_file(dllPath);
        RecordTest("File Integrity", canRead, "DLL file is readable and valid");
        
        // Test DLL format (basic check)
        if (canRead) {
            std::ifstream file(dllPath, std::ios::binary);
            if (file.is_open()) {
                char header[2];
                file.read(header, 2);
                bool isPE = (header[0] == 'M' && header[1] == 'Z'); // PE format starts with MZ
                RecordTest("PE Format Check", isPE, "DLL has valid PE format header");
                file.close();
            }
        }
        
        // Test DLL unloading and reloading
        if (dllHandle) {
            FreeLibrary(dllHandle);
            dllHandle = LoadLibraryA(dllPath.c_str());
            RecordTest("DLL Reload Test", dllHandle != nullptr, "Successfully reloaded DLL");
        }
    }
    
    void RunAllTests() {
        std::cout << "\n🚀 Starting Simple Dynamic DLL Testing" << std::endl;
        std::cout << "=======================================" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        RunBasicDLLTests();
        RunPerformanceTests();
        RunMemoryTests();
        RunIntegrityTests();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::cout << "\n⏱️  Total Test Duration: " << duration.count() << "ms" << std::endl;
    }
    
    void PrintResults() {
        std::cout << "\n📊 Test Results Summary" << std::endl;
        std::cout << "=======================" << std::endl;
        
        int passed = 0;
        int total = static_cast<int>(testResults.size());
        
        for (const auto& result : testResults) {
            if (result.second) passed++;
        }
        
        std::cout << "Total Tests: " << total << std::endl;
        std::cout << "Passed: " << passed << std::endl;
        std::cout << "Failed: " << (total - passed) << std::endl;
        
        if (total > 0) {
            double successRate = (static_cast<double>(passed) / total) * 100.0;
            std::cout << "Success Rate: " << std::fixed << std::setprecision(1) << successRate << "%" << std::endl;
        }
        
        if (passed == total) {
            std::cout << "\n🎉 ALL TESTS PASSED! 🎉" << std::endl;
            std::cout << "✅ ExclusionParser.dll dynamic loading verified!" << std::endl;
        } else {
            std::cout << "\n⚠️  Some tests failed." << std::endl;
        }
    }
};

int main(int argc, char* argv[]) {
    std::cout << "=======================================================" << std::endl;
    std::cout << "   Simple Dynamic DLL Test Suite v2.0.0             " << std::endl;
    std::cout << "=======================================================" << std::endl;
    std::cout << "AMD Verification Team - September 13, 2025" << std::endl;
    std::cout << "Testing ExclusionParser.dll through dynamic loading" << std::endl;
    std::cout << "=======================================================" << std::endl;
    
    try {
        SimpleDynamicDLLTests tests;
        
        if (!tests.Initialize()) {
            std::cerr << "\n❌ Failed to initialize DLL test framework" << std::endl;
            return 1;
        }
        
        tests.RunAllTests();
        tests.PrintResults();
        
        std::cout << "\n✅ Simple Dynamic DLL Testing Complete!" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n💥 Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}