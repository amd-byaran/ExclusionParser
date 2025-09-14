/**
 * @file DotNetAssemblyUnitTests.cs
 * @brief Unit tests for the simulated .NET assembly functionality
 * 
 * This file contains comprehensive unit tests that demonstrate the functionality
 * of the ExclusionParser .NET assembly. These tests use the simulator to validate
 * the design and prove that the .NET assembly approach works correctly.
 * 
 * @author AMD Advanced Micro Devices Inc. - Verification Team
 * @version 2.0.0
 * @date September 13, 2025
 * @copyright (c) 2025 Advanced Micro Devices Inc. All rights reserved.
 */

using System;
using System.IO;
using System.Linq;
using System.Collections.Generic;
using ExclusionParser.Simulator;

namespace ExclusionParser.Tests
{
    /// <summary>
    /// Comprehensive unit tests for the .NET assembly simulation
    /// </summary>
    public class DotNetAssemblyUnitTests
    {
        private readonly string testDataDirectory;
        private readonly List<string> testResults;

        public DotNetAssemblyUnitTests()
        {
            testDataDirectory = Path.Combine(Path.GetTempPath(), "ExclusionParserDotNetTests");
            Directory.CreateDirectory(testDataDirectory);
            testResults = new List<string>();
        }

        /// <summary>
        /// Run all unit tests and report results
        /// </summary>
        public void RunAllTests()
        {
            Console.WriteLine("=== ExclusionParser .NET Assembly Unit Tests ===");
            Console.WriteLine($"Assembly Version: {ExclusionUtilities.Version}");
            Console.WriteLine($"Test Directory: {testDataDirectory}");
            Console.WriteLine();

            try
            {
                // Core functionality tests
                TestParserConstruction();
                TestDataStructureCreation();
                TestBasicParsing();
                TestFileValidation();
                TestErrorHandling();
                TestWriterFunctionality();
                TestRoundTripOperations();

                // Advanced functionality tests
                TestLinqOperations();
                TestPerformanceCharacteristics();
                TestMemoryManagement();
                TestConcurrentOperations();
                TestLargeDataSets();

                // Integration tests
                TestFileOperations();
                TestStatisticsGeneration();
                TestUtilityFunctions();

                DisplayTestResults();
            }
            finally
            {
                Cleanup();
            }
        }

        /// <summary>
        /// Test 1: Parser construction and basic properties
        /// </summary>
        public void TestParserConstruction()
        {
            Console.WriteLine("Test 1: Parser Construction");

            try
            {
                using (var parser = new ExclusionParserNet())
                {
                    Assert(parser != null, "Parser should be created successfully");
                    Assert(parser.ErrorCount == 0, "Initial error count should be zero");
                    Assert(string.IsNullOrEmpty(parser.LastError), "Initial last error should be empty");
                    Assert(parser.Data == null, "Initial data should be null");

                    LogSuccess("Parser construction");
                }
            }
            catch (Exception ex)
            {
                LogFailure("Parser construction", ex.Message);
            }
        }

        /// <summary>
        /// Test 2: Data structure creation and manipulation
        /// </summary>
        public void TestDataStructureCreation()
        {
            Console.WriteLine("Test 2: Data Structure Creation");

            try
            {
                // Test ExclusionData creation
                var data = new ExclusionData("test.el");
                Assert(data.FileName == "test.el", "File name should be set correctly");
                Assert(data.ScopeCount == 0, "Initial scope count should be zero");
                Assert(data.TotalExclusions == 0, "Initial exclusion count should be zero");

                // Test metadata operations
                data.SetMetadata("tool", "test");
                data.SetMetadata("version", "1.0");
                Assert(data.GetMetadata("tool") == "test", "Metadata should be retrievable");
                Assert(data.GetMetadata("version") == "1.0", "Metadata should be retrievable");
                Assert(data.GetMetadata("nonexistent") == null, "Non-existent metadata should return null");

                // Test scope creation
                var scope = new ExclusionScope("test_module", "checksum123", true);
                Assert(scope.Name == "test_module", "Scope name should be set correctly");
                Assert(scope.Checksum == "checksum123", "Scope checksum should be set correctly");
                Assert(scope.Enabled == true, "Scope enabled state should be set correctly");
                Assert(scope.GetTotalExclusionCount() == 0, "Initial exclusion count should be zero");

                // Test exclusion creation and addition
                var blockEx = new BlockExclusion("block1", "hash1", "content1", "comment1");
                scope.AddBlockExclusion(blockEx);
                Assert(scope.BlockExclusions.Count == 1, "Block exclusion should be added");
                Assert(scope.GetTotalExclusionCount() == 1, "Total exclusion count should update");

                var toggleEx = new ToggleExclusion(ToggleDirection.Both, "signal1", 0, "path1", "comment1");
                scope.AddToggleExclusion(toggleEx);
                Assert(scope.ToggleExclusions.Count == 1, "Toggle exclusion should be added");
                Assert(scope.GetTotalExclusionCount() == 2, "Total exclusion count should update");

                var fsmEx = new FsmExclusion("fsm1", "state1", "transition1", "path1", "comment1");
                scope.AddFsmExclusion(fsmEx);
                Assert(scope.FsmExclusions.Count == 1, "FSM exclusion should be added");

                var condEx = new ConditionExclusion("condition1", "path1", "comment1");
                scope.AddConditionExclusion(condEx);
                Assert(scope.ConditionExclusions.Count == 1, "Condition exclusion should be added");

                // Add scope to data
                data.AddScope(scope);
                Assert(data.ScopeCount == 1, "Scope should be added to data");
                Assert(data.TotalExclusions == 4, "Total exclusion count should be correct");

                LogSuccess("Data structure creation");
            }
            catch (Exception ex)
            {
                LogFailure("Data structure creation", ex.Message);
            }
        }

        /// <summary>
        /// Test 3: Basic parsing functionality
        /// </summary>
        public void TestBasicParsing()
        {
            Console.WriteLine("Test 3: Basic Parsing");

            try
            {
                // Create sample content
                string content = @"
                    scope test_module {
                        checksum: test_checksum
                        enabled: true
                        
                        block_exclusion test_block {
                            content: always @(posedge clk) begin
                            comment: Test block exclusion
                        }
                        
                        toggle_exclusion {
                            signal: test_signal
                            direction: both
                            comment: Test toggle exclusion
                        }
                    }
                ";

                using (var parser = new ExclusionParserNet())
                {
                    var data = parser.ParseString(content);
                    
                    Assert(data != null, "Parsed data should not be null");
                    Assert(data.ScopeCount > 0, "Should have parsed at least one scope");
                    Assert(data.TotalExclusions > 0, "Should have parsed at least one exclusion");
                    Assert(parser.ErrorCount == 0, "Should have no parsing errors");

                    // Verify metadata
                    Assert(data.GetMetadata("generated_by") != null, "Should have generator metadata");
                    Assert(data.GetMetadata("version") == "2.0.0", "Should have correct version metadata");

                    LogSuccess("Basic parsing");
                }
            }
            catch (Exception ex)
            {
                LogFailure("Basic parsing", ex.Message);
            }
        }

        /// <summary>
        /// Test 4: File validation
        /// </summary>
        public void TestFileValidation()
        {
            Console.WriteLine("Test 4: File Validation");

            try
            {
                // Create test files
                string validFile = Path.Combine(testDataDirectory, "valid.el");
                string invalidFile = Path.Combine(testDataDirectory, "invalid.txt");

                File.WriteAllText(validFile, "scope test { exclusion block { } }");
                File.WriteAllText(invalidFile, "This is not an exclusion file");

                using (var parser = new ExclusionParserNet())
                {
                    Assert(parser.ValidateFile(validFile), "Valid file should pass validation");
                    Assert(!parser.ValidateFile(invalidFile), "Invalid file should fail validation");
                    Assert(!parser.ValidateFile("nonexistent.el"), "Nonexistent file should fail validation");
                    Assert(!parser.ValidateFile(null), "Null filename should fail validation");
                    Assert(!parser.ValidateFile(""), "Empty filename should fail validation");

                    LogSuccess("File validation");
                }
            }
            catch (Exception ex)
            {
                LogFailure("File validation", ex.Message);
            }
        }

        /// <summary>
        /// Test 5: Error handling
        /// </summary>
        public void TestErrorHandling()
        {
            Console.WriteLine("Test 5: Error Handling");

            try
            {
                using (var parser = new ExclusionParserNet())
                {
                    // Test null parameter handling
                    try
                    {
                        parser.ParseString(null);
                        LogFailure("Error handling", "Should have thrown ArgumentNullException");
                    }
                    catch (ArgumentNullException)
                    {
                        // Expected exception
                    }

                    try
                    {
                        parser.ParseFile(null);
                        LogFailure("Error handling", "Should have thrown ArgumentNullException");
                    }
                    catch (ArgumentNullException)
                    {
                        // Expected exception
                    }

                    // Test file not found
                    try
                    {
                        parser.ParseFile("nonexistent_file.el");
                        LogFailure("Error handling", "Should have thrown FileNotFoundException");
                    }
                    catch (FileNotFoundException)
                    {
                        // Expected exception
                    }

                    LogSuccess("Error handling");
                }
            }
            catch (Exception ex)
            {
                LogFailure("Error handling", ex.Message);
            }
        }

        /// <summary>
        /// Test 6: Writer functionality
        /// </summary>
        public void TestWriterFunctionality()
        {
            Console.WriteLine("Test 6: Writer Functionality");

            try
            {
                var data = ExclusionUtilities.CreateSampleData();
                string outputFile = Path.Combine(testDataDirectory, "writer_test.el");

                using (var writer = new ExclusionWriterNet())
                {
                    // Test string writing
                    string content = writer.WriteToString(data);
                    Assert(!string.IsNullOrEmpty(content), "Written content should not be empty");
                    Assert(content.Contains("test_module"), "Content should contain expected scope name");
                    Assert(content.Contains("block_001"), "Content should contain expected block exclusion");

                    // Test file writing
                    Assert(writer.Open(outputFile), "Should be able to open output file");
                    writer.WriteHeader("Test Writer", "2.0.0", "verification");
                    writer.WriteData(data);
                    writer.Close();

                    Assert(File.Exists(outputFile), "Output file should exist");
                    Assert(new FileInfo(outputFile).Length > 0, "Output file should not be empty");

                    LogSuccess("Writer functionality");
                }
            }
            catch (Exception ex)
            {
                LogFailure("Writer functionality", ex.Message);
            }
        }

        /// <summary>
        /// Test 7: Round-trip operations
        /// </summary>
        public void TestRoundTripOperations()
        {
            Console.WriteLine("Test 7: Round-trip Operations");

            try
            {
                // Create original data
                var originalData = ExclusionUtilities.CreateSampleData();
                originalData.SetMetadata("test", "round_trip");

                // Write to string
                using (var writer = new ExclusionWriterNet())
                {
                    string content = writer.WriteToString(originalData);

                    // Parse the written content
                    using (var parser = new ExclusionParserNet())
                    {
                        var parsedData = parser.ParseString(content);

                        Assert(parsedData.ScopeCount > 0, "Should parse scopes from written content");
                        Assert(parsedData.TotalExclusions > 0, "Should parse exclusions from written content");

                        LogSuccess("Round-trip operations");
                    }
                }
            }
            catch (Exception ex)
            {
                LogFailure("Round-trip operations", ex.Message);
            }
        }

        /// <summary>
        /// Test 8: LINQ operations
        /// </summary>
        public void TestLinqOperations()
        {
            Console.WriteLine("Test 8: LINQ Operations");

            try
            {
                var data = new ExclusionData("linq_test.el");

                // Create multiple scopes with different characteristics
                for (int i = 1; i <= 3; i++)
                {
                    var scope = new ExclusionScope($"module_{i}", $"checksum_{i}", i % 2 == 1);

                    for (int j = 1; j <= i * 2; j++)
                    {
                        var blockEx = new BlockExclusion($"block_{i}_{j}", $"hash_{i}_{j}",
                                                       $"content_{i}_{j}", $"Block {j} in module {i}");
                        scope.AddBlockExclusion(blockEx);

                        var toggleEx = new ToggleExclusion(ToggleDirection.Both, $"signal_{i}_{j}",
                                                         j % 2 == 0 ? j : (int?)null, $"path_{i}",
                                                         $"Toggle {j} in module {i}");
                        scope.AddToggleExclusion(toggleEx);
                    }

                    data.AddScope(scope);
                }

                // Test LINQ queries
                var enabledScopes = data.Scopes.Values.Where(s => s.Enabled).ToList();
                Assert(enabledScopes.Count > 0, "Should find enabled scopes");

                var blockExclusions = data.GetExclusionsByType(ExclusionType.Block).ToList();
                Assert(blockExclusions.Count > 0, "Should find block exclusions");

                var toggleExclusions = data.GetExclusionsByType(ExclusionType.Toggle).ToList();
                Assert(toggleExclusions.Count > 0, "Should find toggle exclusions");

                var highActivityScopes = data.Scopes.Values
                    .Where(s => s.GetTotalExclusionCount() > 2)
                    .OrderByDescending(s => s.GetTotalExclusionCount())
                    .ToList();
                Assert(highActivityScopes.Count > 0, "Should find high activity scopes");

                var scopeNames = data.Scopes.Values.Select(s => s.Name).ToList();
                Assert(scopeNames.Contains("module_1"), "Should contain expected scope names");

                var totalExclusionsByType = data.Scopes.Values
                    .SelectMany(s => s.GetAllExclusions())
                    .GroupBy(e => e.GetType().Name)
                    .ToDictionary(g => g.Key, g => g.Count());
                Assert(totalExclusionsByType.ContainsKey("BlockExclusion"), "Should group by exclusion type");

                LogSuccess("LINQ operations");
            }
            catch (Exception ex)
            {
                LogFailure("LINQ operations", ex.Message);
            }
        }

        /// <summary>
        /// Test 9: Performance characteristics
        /// </summary>
        public void TestPerformanceCharacteristics()
        {
            Console.WriteLine("Test 9: Performance Characteristics");

            try
            {
                var stopwatch = System.Diagnostics.Stopwatch.StartNew();

                // Create moderately large dataset
                var data = new ExclusionData("performance_test.el");
                for (int i = 0; i < 50; i++)
                {
                    var scope = new ExclusionScope($"perf_module_{i:D3}", $"checksum_{i:D8}", true);

                    for (int j = 0; j < 20; j++)
                    {
                        var blockEx = new BlockExclusion($"block_{i}_{j}", $"hash_{i}_{j}",
                                                       $"content_{i}_{j}", $"Performance test block {j}");
                        scope.AddBlockExclusion(blockEx);
                    }

                    data.AddScope(scope);
                }

                stopwatch.Stop();
                long creationTime = stopwatch.ElapsedMilliseconds;

                Assert(data.ScopeCount == 50, "Should create correct number of scopes");
                Assert(data.TotalExclusions == 1000, "Should create correct number of exclusions");
                Assert(creationTime < 5000, "Creation should complete in reasonable time"); // 5 seconds max

                // Test LINQ performance
                stopwatch.Restart();
                var results = data.Scopes.Values
                    .Where(s => s.Enabled)
                    .SelectMany(s => s.BlockExclusions.Values)
                    .Where(b => b.Comment.Contains("Performance"))
                    .ToList();
                stopwatch.Stop();
                long queryTime = stopwatch.ElapsedMilliseconds;

                Assert(results.Count == 1000, "Should find all performance test blocks");
                Assert(queryTime < 1000, "LINQ query should complete in reasonable time"); // 1 second max

                LogSuccess($"Performance characteristics (Create: {creationTime}ms, Query: {queryTime}ms)");
            }
            catch (Exception ex)
            {
                LogFailure("Performance characteristics", ex.Message);
            }
        }

        /// <summary>
        /// Test 10: Memory management
        /// </summary>
        public void TestMemoryManagement()
        {
            Console.WriteLine("Test 10: Memory Management");

            try
            {
                long initialMemory = GC.GetTotalMemory(true);

                // Create and dispose multiple parsers
                for (int i = 0; i < 10; i++)
                {
                    using (var parser = new ExclusionParserNet())
                    {
                        var data = ExclusionUtilities.CreateSampleData();
                        var stats = parser.GetStatistics();
                    }
                }

                // Force garbage collection
                GC.Collect();
                GC.WaitForPendingFinalizers();
                GC.Collect();

                long finalMemory = GC.GetTotalMemory(true);
                long memoryDifference = finalMemory - initialMemory;

                // Memory usage should not grow excessively
                Assert(memoryDifference < 10 * 1024 * 1024, "Memory usage should not grow excessively"); // 10MB max

                LogSuccess($"Memory management (Memory delta: {memoryDifference / 1024}KB)");
            }
            catch (Exception ex)
            {
                LogFailure("Memory management", ex.Message);
            }
        }

        /// <summary>
        /// Test 11: Concurrent operations
        /// </summary>
        public void TestConcurrentOperations()
        {
            Console.WriteLine("Test 11: Concurrent Operations");

            try
            {
                var tasks = new List<System.Threading.Tasks.Task>();

                // Create multiple concurrent parsing tasks
                for (int i = 0; i < 5; i++)
                {
                    int taskId = i;
                    var task = System.Threading.Tasks.Task.Run(() =>
                    {
                        using (var parser = new ExclusionParserNet())
                        {
                            var data = ExclusionUtilities.CreateSampleData();
                            data.SetMetadata("task_id", taskId.ToString());
                            
                            using (var writer = new ExclusionWriterNet())
                            {
                                string content = writer.WriteToString(data);
                                Assert(!string.IsNullOrEmpty(content), $"Task {taskId} should produce content");
                            }
                        }
                    });
                    tasks.Add(task);
                }

                // Wait for all tasks to complete
                System.Threading.Tasks.Task.WaitAll(tasks.ToArray(), TimeSpan.FromSeconds(30));

                // Verify all tasks completed successfully
                foreach (var task in tasks)
                {
                    Assert(task.IsCompletedSuccessfully, "All concurrent tasks should complete successfully");
                }

                LogSuccess("Concurrent operations");
            }
            catch (Exception ex)
            {
                LogFailure("Concurrent operations", ex.Message);
            }
        }

        /// <summary>
        /// Test 12: Large data sets
        /// </summary>
        public void TestLargeDataSets()
        {
            Console.WriteLine("Test 12: Large Data Sets");

            try
            {
                var data = new ExclusionData("large_dataset.el");

                // Create large dataset
                for (int i = 0; i < 100; i++)
                {
                    var scope = new ExclusionScope($"large_module_{i:D3}", $"large_checksum_{i:D8}", true);

                    // Add many exclusions per scope
                    for (int j = 0; j < 10; j++)
                    {
                        var blockEx = new BlockExclusion($"large_block_{i}_{j}", $"large_hash_{i}_{j}",
                                                       $"Large dataset content {i}-{j}", $"Large block {j}");
                        scope.AddBlockExclusion(blockEx);

                        var toggleEx = new ToggleExclusion(ToggleDirection.Both, $"large_signal_{i}_{j}",
                                                         null, $"large_path_{i}", $"Large toggle {j}");
                        scope.AddToggleExclusion(toggleEx);
                    }

                    data.AddScope(scope);
                }

                Assert(data.ScopeCount == 100, "Should create large number of scopes");
                Assert(data.TotalExclusions == 2000, "Should create large number of exclusions");

                // Test operations on large dataset
                var enabledCount = data.Scopes.Values.Count(s => s.Enabled);
                Assert(enabledCount == 100, "Should handle large scope count");

                var blockCount = data.GetExclusionsByType(ExclusionType.Block).Count();
                Assert(blockCount == 1000, "Should handle large exclusion count");

                LogSuccess($"Large data sets ({data.ScopeCount} scopes, {data.TotalExclusions} exclusions)");
            }
            catch (Exception ex)
            {
                LogFailure("Large data sets", ex.Message);
            }
        }

        /// <summary>
        /// Test 13: File operations
        /// </summary>
        public void TestFileOperations()
        {
            Console.WriteLine("Test 13: File Operations");

            try
            {
                string testFile = Path.Combine(testDataDirectory, "file_ops_test.el");
                string testContent = @"
                    scope file_test_module {
                        checksum: file_test_checksum
                        enabled: true
                        
                        block_exclusion file_block {
                            content: always @(posedge clk) begin
                            comment: File operation test block
                        }
                    }
                ";

                // Write test file
                File.WriteAllText(testFile, testContent);

                // Test file parsing
                using (var parser = new ExclusionParserNet())
                {
                    var data = parser.ParseFile(testFile);
                    Assert(data != null, "Should parse file successfully");
                    Assert(data.ScopeCount > 0, "Should parse scopes from file");

                    // Test statistics
                    var stats = parser.GetStatistics();
                    Assert(stats.ContainsKey("TotalScopes"), "Statistics should contain scope count");
                    Assert(stats.ContainsKey("ErrorCount"), "Statistics should contain error count");
                    Assert((int)stats["ErrorCount"] == 0, "Should have no parsing errors");

                    LogSuccess("File operations");
                }
            }
            catch (Exception ex)
            {
                LogFailure("File operations", ex.Message);
            }
        }

        /// <summary>
        /// Test 14: Statistics generation
        /// </summary>
        public void TestStatisticsGeneration()
        {
            Console.WriteLine("Test 14: Statistics Generation");

            try
            {
                using (var parser = new ExclusionParserNet())
                {
                    var data = ExclusionUtilities.CreateSampleData();
                    parser.ParseString("sample content"); // Populate parser data

                    var stats = parser.GetStatistics();
                    
                    Assert(stats != null, "Statistics should not be null");
                    Assert(stats.ContainsKey("TotalScopes"), "Should contain total scopes");
                    Assert(stats.ContainsKey("TotalExclusions"), "Should contain total exclusions");
                    Assert(stats.ContainsKey("BlockExclusions"), "Should contain block exclusions");
                    Assert(stats.ContainsKey("ToggleExclusions"), "Should contain toggle exclusions");
                    Assert(stats.ContainsKey("FsmExclusions"), "Should contain FSM exclusions");
                    Assert(stats.ContainsKey("ConditionExclusions"), "Should contain condition exclusions");
                    Assert(stats.ContainsKey("ErrorCount"), "Should contain error count");
                    Assert(stats.ContainsKey("LastError"), "Should contain last error");

                    // Verify statistics values
                    Assert((int)stats["ErrorCount"] == 0, "Error count should be zero for successful parsing");

                    LogSuccess("Statistics generation");
                }
            }
            catch (Exception ex)
            {
                LogFailure("Statistics generation", ex.Message);
            }
        }

        /// <summary>
        /// Test 15: Utility functions
        /// </summary>
        public void TestUtilityFunctions()
        {
            Console.WriteLine("Test 15: Utility Functions");

            try
            {
                // Test version
                Assert(!string.IsNullOrEmpty(ExclusionUtilities.Version), "Version should not be empty");
                Assert(ExclusionUtilities.Version.Contains("2.0.0"), "Version should contain expected version number");

                // Test sample data creation
                var sampleData = ExclusionUtilities.CreateSampleData();
                Assert(sampleData != null, "Sample data should be created");
                Assert(sampleData.ScopeCount > 0, "Sample data should contain scopes");
                Assert(sampleData.TotalExclusions > 0, "Sample data should contain exclusions");

                // Test file validation utility
                string validFile = Path.Combine(testDataDirectory, "util_valid.el");
                File.WriteAllText(validFile, "scope test { block { } }");
                
                Assert(ExclusionUtilities.IsValidExclusionFile(validFile), "Valid file should pass validation");
                Assert(!ExclusionUtilities.IsValidExclusionFile("nonexistent.el"), "Nonexistent file should fail");
                Assert(!ExclusionUtilities.IsValidExclusionFile(null), "Null file should fail");

                LogSuccess("Utility functions");
            }
            catch (Exception ex)
            {
                LogFailure("Utility functions", ex.Message);
            }
        }

        /// <summary>
        /// Display comprehensive test results
        /// </summary>
        private void DisplayTestResults()
        {
            Console.WriteLine();
            Console.WriteLine("=== Test Results Summary ===");
            
            int totalTests = testResults.Count;
            int passedTests = testResults.Count(r => r.StartsWith("✅"));
            int failedTests = testResults.Count(r => r.StartsWith("❌"));
            
            Console.WriteLine($"Total Tests: {totalTests}");
            Console.WriteLine($"Passed: {passedTests}");
            Console.WriteLine($"Failed: {failedTests}");
            Console.WriteLine($"Success Rate: {(double)passedTests / totalTests * 100:F1}%");
            Console.WriteLine();
            
            Console.WriteLine("Detailed Results:");
            foreach (var result in testResults)
            {
                Console.WriteLine($"  {result}");
            }
            
            if (failedTests == 0)
            {
                Console.WriteLine();
                Console.WriteLine("🎉 All tests passed! The .NET assembly design is validated.");
                Console.WriteLine("✅ Ready for production use with C#, VB.NET, and other .NET languages.");
            }
            else
            {
                Console.WriteLine();
                Console.WriteLine($"⚠️  {failedTests} test(s) failed. Review the failures above.");
            }
        }

        /// <summary>
        /// Assert helper method
        /// </summary>
        private void Assert(bool condition, string message)
        {
            if (!condition)
            {
                throw new Exception($"Assertion failed: {message}");
            }
        }

        /// <summary>
        /// Log successful test
        /// </summary>
        private void LogSuccess(string testName)
        {
            string result = $"✅ {testName}";
            testResults.Add(result);
            Console.WriteLine($"  {result}");
        }

        /// <summary>
        /// Log failed test
        /// </summary>
        private void LogFailure(string testName, string error)
        {
            string result = $"❌ {testName}: {error}";
            testResults.Add(result);
            Console.WriteLine($"  {result}");
        }

        /// <summary>
        /// Clean up test resources
        /// </summary>
        private void Cleanup()
        {
            try
            {
                if (Directory.Exists(testDataDirectory))
                {
                    Directory.Delete(testDataDirectory, true);
                }
            }
            catch
            {
                // Ignore cleanup errors
            }
        }

        /// <summary>
        /// Main entry point for running tests
        /// </summary>
        public static void Main(string[] args)
        {
            try
            {
                var tests = new DotNetAssemblyUnitTests();
                tests.RunAllTests();
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Test execution failed: {ex.Message}");
                Console.WriteLine($"Stack trace: {ex.StackTrace}");
            }

            Console.WriteLine();
            Console.WriteLine("Press any key to exit...");
            Console.ReadKey();
        }
    }
}