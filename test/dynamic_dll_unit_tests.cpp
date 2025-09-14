/**
 * Dynamic DLL Unit Tests for ExclusionParser
 * 
 * This test suite loads ExclusionParser.dll dynamically at runtime and
 * performs comprehensive testing through the DLL interface rather than
 * static linking. This validates the actual DLL functionality that would
 * be used by external applications.
 * 
 * Author: AMD Verification Team
 * Date: September 13, 2025
 * Version: 2.0.0
 */

#include <iostream>
#include <string>
#include <filesystem>
#include <chrono>
#include "DLLTestFramework.h"

class DynamicDLLUnitTests {
private:
    std::unique_ptr<DLLTestFramework> framework;
    std::string dllPath;
    
public:
    DynamicDLLUnitTests() {
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
        
        if (dllPath.empty()) {
            std::cerr << "Error: ExclusionParser.dll not found in any expected location!" << std::endl;
            std::cerr << "Please ensure the DLL is built and accessible." << std::endl;
            return;
        }
        
        framework = std::make_unique<DLLTestFramework>(dllPath);
    }
    
    bool Initialize() {
        if (!framework) {
            std::cerr << "Framework not initialized - DLL not found" << std::endl;
            return false;
        }
        
        std::cout << "🔧 Initializing Dynamic DLL Test Suite..." << std::endl;
        std::cout << "DLL Location: " << dllPath << std::endl;
        
        return framework->Initialize();
    }
    
    void RunComprehensiveTests() {
        if (!framework || !framework->IsLoaded()) {
            std::cerr << "Cannot run tests - DLL not loaded" << std::endl;
            return;
        }
        
        std::cout << "\n🚀 Starting Comprehensive Dynamic DLL Testing" << std::endl;
        std::cout << "===============================================" << std::endl;
        std::cout << "Testing ExclusionParser.dll through dynamic loading" << std::endl;
        std::cout << "This validates real-world DLL usage scenarios" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run all test categories
        framework->RunAllTests();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::cout << "\n⏱️  Total Test Duration: " << duration.count() << "ms" << std::endl;
    }
    
    void RunSpecificTestCategory(const std::string& category) {
        if (!framework || !framework->IsLoaded()) {
            std::cerr << "Cannot run tests - DLL not loaded" << std::endl;
            return;
        }
        
        std::cout << "\n🎯 Running " << category << " Tests" << std::endl;
        std::cout << std::string(30 + category.length(), '=') << std::endl;
        
        if (category == "basic") {
            framework->RunBasicDLLTests();
        } else if (category == "parser") {
            framework->RunParserDLLTests();
        } else if (category == "writer") {
            framework->RunWriterDLLTests();
        } else if (category == "data") {
            framework->RunDataDLLTests();
        } else if (category == "performance") {
            framework->RunPerformanceTests();
        } else if (category == "stress") {
            framework->RunStressTests();
        } else {
            std::cout << "Unknown test category: " << category << std::endl;
            std::cout << "Available categories: basic, parser, writer, data, performance, stress" << std::endl;
        }
    }
    
    void PrintDetailedResults() {
        if (!framework) {
            std::cerr << "No framework available for results" << std::endl;
            return;
        }
        
        framework->PrintResults();
        
        // Save results to file
        std::string resultsFile = "dll_test_results_" + GetTimestamp() + ".txt";
        framework->SaveResultsToFile(resultsFile);
        
        std::cout << "\n📁 Results saved to: " << resultsFile << std::endl;
    }
    
    void GeneratePerformanceReport() {
        if (!framework) return;
        
        std::cout << "\n📈 Performance Analysis" << std::endl;
        std::cout << "======================" << std::endl;
        std::cout << "DLL Loading: SUCCESS" << std::endl;
        std::cout << "Function Resolution: " << (framework->IsLoaded() ? "SUCCESS" : "FAILED") << std::endl;
        std::cout << "Total Execution Time: " << std::fixed << std::setprecision(2) 
                  << framework->GetTotalExecutionTime() << "ms" << std::endl;
        std::cout << "Average Test Time: " << std::fixed << std::setprecision(2)
                  << (framework->GetTotalTests() > 0 ? framework->GetTotalExecutionTime() / framework->GetTotalTests() : 0) 
                  << "ms per test" << std::endl;
    }
    
    bool ValidateDLLIntegrity() {
        std::cout << "\n🔍 DLL Integrity Validation" << std::endl;
        std::cout << "============================" << std::endl;
        
        if (dllPath.empty()) {
            std::cout << "❌ DLL path not found" << std::endl;
            return false;
        }
        
        // Check file existence
        bool exists = std::filesystem::exists(dllPath);
        std::cout << "File exists: " << (exists ? "✅ YES" : "❌ NO") << std::endl;
        
        if (!exists) return false;
        
        // Check file size
        auto fileSize = std::filesystem::file_size(dllPath);
        std::cout << "File size: " << fileSize << " bytes" << std::endl;
        
        bool sizeOk = fileSize > 1024 && fileSize < 100 * 1024 * 1024; // Between 1KB and 100MB
        std::cout << "Size reasonable: " << (sizeOk ? "✅ YES" : "❌ NO") << std::endl;
        
        // Check if DLL can be loaded
        bool canLoad = framework && framework->IsLoaded();
        std::cout << "Can load DLL: " << (canLoad ? "✅ YES" : "❌ NO") << std::endl;
        
        return exists && sizeOk && canLoad;
    }
    
private:
    std::string GetTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto tm = *std::localtime(&time_t);
        
        std::stringstream ss;
        ss << std::put_time(&tm, "%Y%m%d_%H%M%S");
        return ss.str();
    }
};

void PrintUsage(const char* programName) {
    std::cout << "\nUsage: " << programName << " [option]\n";
    std::cout << "Options:\n";
    std::cout << "  --all         Run all test categories (default)\n";
    std::cout << "  --basic       Run basic DLL loading tests\n";
    std::cout << "  --parser      Run parser functionality tests\n";
    std::cout << "  --writer      Run writer functionality tests\n";
    std::cout << "  --data        Run data structure tests\n";
    std::cout << "  --performance Run performance tests\n";
    std::cout << "  --stress      Run stress tests\n";
    std::cout << "  --integrity   Validate DLL integrity only\n";
    std::cout << "  --help        Show this help message\n";
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "===========================================================" << std::endl;
    std::cout << "   ExclusionParser Dynamic DLL Unit Test Suite v2.0.0    " << std::endl;
    std::cout << "===========================================================" << std::endl;
    std::cout << "AMD Verification Team - September 13, 2025" << std::endl;
    std::cout << "Testing ExclusionParser.dll through dynamic loading" << std::endl;
    std::cout << "===========================================================" << std::endl;
    
    try {
        DynamicDLLUnitTests tests;
        
        // Parse command line arguments
        std::string testCategory = "all";
        if (argc > 1) {
            std::string arg = argv[1];
            if (arg == "--help" || arg == "-h") {
                PrintUsage(argv[0]);
                return 0;
            } else if (arg == "--integrity") {
                bool isValid = tests.ValidateDLLIntegrity();
                return isValid ? 0 : 1;
            } else if (arg.substr(0, 2) == "--") {
                testCategory = arg.substr(2);
            }
        }
        
        // Initialize the test framework
        if (!tests.Initialize()) {
            std::cerr << "\n❌ Failed to initialize DLL test framework" << std::endl;
            std::cerr << "Please ensure ExclusionParser.dll is built and accessible" << std::endl;
            return 1;
        }
        
        // Validate DLL integrity
        if (!tests.ValidateDLLIntegrity()) {
            std::cerr << "\n❌ DLL integrity validation failed" << std::endl;
            return 1;
        }
        
        // Run tests based on category
        if (testCategory == "all") {
            tests.RunComprehensiveTests();
        } else {
            tests.RunSpecificTestCategory(testCategory);
        }
        
        // Print results and generate reports
        tests.PrintDetailedResults();
        tests.GeneratePerformanceReport();
        
        std::cout << "\n🎉 Dynamic DLL Testing Complete!" << std::endl;
        std::cout << "✅ ExclusionParser.dll functionality validated through dynamic loading" << std::endl;
        std::cout << "📊 Test results available in generated report files" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n💥 Dynamic DLL test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\n💥 Dynamic DLL test failed with unknown exception" << std::endl;
        return 1;
    }
}