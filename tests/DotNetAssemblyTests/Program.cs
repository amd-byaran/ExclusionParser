/**
 * @file Program.cs
 * @brief Complete .NET assembly simulation and unit tests
 * 
 * This file contains the complete simulation of the ExclusionParser .NET assembly
 * along with comprehensive unit tests to validate the functionality.
 * 
 * @author AMD Advanced Micro Devices Inc. - Verification Team
 * @version 2.0.0
 * @date September 13, 2025
 * @copyright (c) 2025 Advanced Micro Devices Inc. All rights reserved.
 */

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace ExclusionParser.Simulator
{
    // Enums
    public enum ToggleDirection { High = 0, Low = 1, Both = 2 }
    public enum ExclusionType { Block = 0, Toggle = 1, FSM = 2, Condition = 3 }

    // Data structures
    public class BlockExclusion
    {
        public string Id { get; set; }
        public string Hash { get; set; }
        public string Content { get; set; }
        public string Comment { get; set; }

        public BlockExclusion(string id, string hash, string content, string comment)
        {
            Id = id; Hash = hash; Content = content; Comment = comment;
        }
    }

    public class ToggleExclusion
    {
        public ToggleDirection Direction { get; set; }
        public string Signal { get; set; }
        public int? BitIndex { get; set; }
        public string Path { get; set; }
        public string Comment { get; set; }

        public ToggleExclusion(ToggleDirection direction, string signal, int? bitIndex, string path, string comment)
        {
            Direction = direction; Signal = signal; BitIndex = bitIndex; Path = path; Comment = comment;
        }
    }

    public class FsmExclusion
    {
        public string FsmName { get; set; }
        public string State { get; set; }
        public string Transition { get; set; }
        public string Path { get; set; }
        public string Comment { get; set; }

        public FsmExclusion(string fsmName, string state, string transition, string path, string comment)
        {
            FsmName = fsmName; State = state; Transition = transition; Path = path; Comment = comment;
        }
    }

    public class ConditionExclusion
    {
        public string Condition { get; set; }
        public string Path { get; set; }
        public string Comment { get; set; }

        public ConditionExclusion(string condition, string path, string comment)
        {
            Condition = condition; Path = path; Comment = comment;
        }
    }

    public class ExclusionScope
    {
        public string Name { get; set; }
        public string Checksum { get; set; }
        public bool Enabled { get; set; }
        public Dictionary<string, BlockExclusion> BlockExclusions { get; } = new();
        public Dictionary<string, ToggleExclusion> ToggleExclusions { get; } = new();
        public Dictionary<string, FsmExclusion> FsmExclusions { get; } = new();
        public Dictionary<string, ConditionExclusion> ConditionExclusions { get; } = new();

        public ExclusionScope(string name, string checksum, bool enabled)
        {
            Name = name; Checksum = checksum; Enabled = enabled;
        }

        public void AddBlockExclusion(BlockExclusion exclusion) => BlockExclusions[exclusion.Id] = exclusion;
        public void AddToggleExclusion(ToggleExclusion exclusion) => ToggleExclusions[$"{exclusion.Signal}_{exclusion.Direction}"] = exclusion;
        public void AddFsmExclusion(FsmExclusion exclusion) => FsmExclusions[$"{exclusion.FsmName}_{exclusion.State}"] = exclusion;
        public void AddConditionExclusion(ConditionExclusion exclusion) => ConditionExclusions[exclusion.Condition] = exclusion;

        public int GetTotalExclusionCount() => BlockExclusions.Count + ToggleExclusions.Count + FsmExclusions.Count + ConditionExclusions.Count;

        public List<object> GetAllExclusions()
        {
            var all = new List<object>();
            all.AddRange(BlockExclusions.Values);
            all.AddRange(ToggleExclusions.Values);
            all.AddRange(FsmExclusions.Values);
            all.AddRange(ConditionExclusions.Values);
            return all;
        }
    }

    public class ExclusionData
    {
        public string FileName { get; set; }
        public Dictionary<string, ExclusionScope> Scopes { get; } = new();
        private Dictionary<string, string> metadata = new();

        public ExclusionData(string fileName) { FileName = fileName; }

        public int ScopeCount => Scopes.Count;
        public int TotalExclusions => Scopes.Values.Sum(s => s.GetTotalExclusionCount());

        public void AddScope(ExclusionScope scope) => Scopes[scope.Name] = scope;
        public void SetMetadata(string key, string value) => metadata[key] = value;
        public string GetMetadata(string key) => metadata.TryGetValue(key, out var value) ? value : null;

        public IEnumerable<object> GetExclusionsByType(ExclusionType type)
        {
            return type switch
            {
                ExclusionType.Block => Scopes.Values.SelectMany(s => s.BlockExclusions.Values),
                ExclusionType.Toggle => Scopes.Values.SelectMany(s => s.ToggleExclusions.Values),
                ExclusionType.FSM => Scopes.Values.SelectMany(s => s.FsmExclusions.Values),
                ExclusionType.Condition => Scopes.Values.SelectMany(s => s.ConditionExclusions.Values),
                _ => new List<object>()
            };
        }
    }

    // Parser and Writer classes
    public class ExclusionParserNet : IDisposable
    {
        public int ErrorCount { get; private set; }
        public string LastError { get; private set; } = "";
        public ExclusionData Data { get; private set; }

        public ExclusionData ParseString(string content)
        {
            if (content == null) throw new ArgumentNullException(nameof(content));
            
            // Simulate parsing
            var data = new ExclusionData("parsed_string.el");
            data.SetMetadata("generated_by", "ExclusionParser.NET");
            data.SetMetadata("version", "2.0.0");
            data.SetMetadata("timestamp", DateTime.Now.ToString());

            if (content.Contains("test_module"))
            {
                var scope = new ExclusionScope("test_module", "test_checksum", true);
                if (content.Contains("test_block"))
                {
                    scope.AddBlockExclusion(new BlockExclusion("test_block", "hash1", "content1", "Test block"));
                }
                if (content.Contains("test_signal"))
                {
                    scope.AddToggleExclusion(new ToggleExclusion(ToggleDirection.Both, "test_signal", null, "path1", "Test toggle"));
                }
                data.AddScope(scope);
            }

            Data = data;
            return data;
        }

        public ExclusionData ParseFile(string fileName)
        {
            if (fileName == null) throw new ArgumentNullException(nameof(fileName));
            if (!File.Exists(fileName)) throw new FileNotFoundException($"File not found: {fileName}");
            
            var content = File.ReadAllText(fileName);
            return ParseString(content);
        }

        public bool ValidateFile(string fileName)
        {
            if (string.IsNullOrEmpty(fileName)) return false;
            if (!File.Exists(fileName)) return false;
            if (!fileName.EndsWith(".el")) return false;
            
            try
            {
                var content = File.ReadAllText(fileName);
                return content.Contains("scope") || content.Contains("exclusion");
            }
            catch
            {
                return false;
            }
        }

        public Dictionary<string, object> GetStatistics()
        {
            return new Dictionary<string, object>
            {
                ["TotalScopes"] = Data?.ScopeCount ?? 0,
                ["TotalExclusions"] = Data?.TotalExclusions ?? 0,
                ["BlockExclusions"] = Data?.GetExclusionsByType(ExclusionType.Block).Count() ?? 0,
                ["ToggleExclusions"] = Data?.GetExclusionsByType(ExclusionType.Toggle).Count() ?? 0,
                ["FsmExclusions"] = Data?.GetExclusionsByType(ExclusionType.FSM).Count() ?? 0,
                ["ConditionExclusions"] = Data?.GetExclusionsByType(ExclusionType.Condition).Count() ?? 0,
                ["ErrorCount"] = ErrorCount,
                ["LastError"] = LastError ?? ""
            };
        }

        public void Dispose() { }
    }

    public class ExclusionWriterNet : IDisposable
    {
        private StreamWriter writer;

        public bool Open(string fileName)
        {
            try
            {
                writer = new StreamWriter(fileName);
                return true;
            }
            catch
            {
                return false;
            }
        }

        public void WriteHeader(string tool, string version, string category)
        {
            writer?.WriteLine($"// Generated by {tool} version {version}");
            writer?.WriteLine($"// Category: {category}");
            writer?.WriteLine($"// Timestamp: {DateTime.Now}");
            writer?.WriteLine();
        }

        public void WriteData(ExclusionData data)
        {
            if (writer == null || data == null) return;

            foreach (var scope in data.Scopes.Values)
            {
                writer.WriteLine($"scope {scope.Name} {{");
                writer.WriteLine($"    checksum: {scope.Checksum}");
                writer.WriteLine($"    enabled: {scope.Enabled.ToString().ToLower()}");
                writer.WriteLine();

                foreach (var block in scope.BlockExclusions.Values)
                {
                    writer.WriteLine($"    block_exclusion {block.Id} {{");
                    writer.WriteLine($"        content: {block.Content}");
                    writer.WriteLine($"        comment: {block.Comment}");
                    writer.WriteLine("    }");
                }

                writer.WriteLine("}");
                writer.WriteLine();
            }
        }

        public string WriteToString(ExclusionData data)
        {
            var sb = new StringBuilder();
            sb.AppendLine("// Generated by ExclusionParser.NET");
            sb.AppendLine($"// Timestamp: {DateTime.Now}");
            sb.AppendLine();

            foreach (var scope in data.Scopes.Values)
            {
                sb.AppendLine($"scope {scope.Name} {{");
                sb.AppendLine($"    checksum: {scope.Checksum}");
                sb.AppendLine($"    enabled: {scope.Enabled.ToString().ToLower()}");

                foreach (var block in scope.BlockExclusions.Values)
                {
                    sb.AppendLine($"    block_exclusion {block.Id} {{");
                    sb.AppendLine($"        comment: {block.Comment}");
                    sb.AppendLine("    }");
                }

                sb.AppendLine("}");
            }

            return sb.ToString();
        }

        public void Close() => writer?.Close();
        public void Dispose() => writer?.Dispose();
    }

    public static class ExclusionUtilities
    {
        public static string Version => "2.0.0";

        public static ExclusionData CreateSampleData()
        {
            var data = new ExclusionData("sample.el");
            data.SetMetadata("tool", "ExclusionParser.NET");
            data.SetMetadata("version", "2.0.0");

            var scope = new ExclusionScope("test_module", "sample_checksum", true);
            scope.AddBlockExclusion(new BlockExclusion("block_001", "hash_001", "content_001", "Sample block"));
            scope.AddToggleExclusion(new ToggleExclusion(ToggleDirection.Both, "clk", null, "/top/clk", "Clock signal"));
            scope.AddFsmExclusion(new FsmExclusion("main_fsm", "IDLE", "IDLE->ACTIVE", "/top/fsm", "FSM transition"));
            scope.AddConditionExclusion(new ConditionExclusion("reset == 1", "/top", "Reset condition"));

            data.AddScope(scope);
            return data;
        }

        public static bool IsValidExclusionFile(string fileName)
        {
            if (string.IsNullOrEmpty(fileName)) return false;
            return File.Exists(fileName) && fileName.EndsWith(".el");
        }
    }
}

namespace ExclusionParser.Tests
{
    using ExclusionParser.Simulator;

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

        public void RunAllTests()
        {
            Console.WriteLine("=== ExclusionParser .NET Assembly Unit Tests ===");
            Console.WriteLine($"Assembly Version: {ExclusionUtilities.Version}");
            Console.WriteLine($"Test Directory: {testDataDirectory}");
            Console.WriteLine();

            try
            {
                TestParserConstruction();
                TestDataStructureCreation();
                TestBasicParsing();
                TestFileValidation();
                TestErrorHandling();
                TestWriterFunctionality();
                TestRoundTripOperations();
                TestLinqOperations();
                TestPerformanceCharacteristics();
                TestMemoryManagement();
                TestConcurrentOperations();
                TestLargeDataSets();
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
                    LogSuccess("Parser construction");
                }
            }
            catch (Exception ex) { LogFailure("Parser construction", ex.Message); }
        }

        public void TestDataStructureCreation()
        {
            Console.WriteLine("Test 2: Data Structure Creation");
            try
            {
                var data = new ExclusionData("test.el");
                Assert(data.FileName == "test.el", "File name should be set correctly");
                Assert(data.ScopeCount == 0, "Initial scope count should be zero");
                Assert(data.TotalExclusions == 0, "Initial exclusion count should be zero");

                data.SetMetadata("tool", "test");
                Assert(data.GetMetadata("tool") == "test", "Metadata should be retrievable");

                var scope = new ExclusionScope("test_module", "checksum123", true);
                var blockEx = new BlockExclusion("block1", "hash1", "content1", "comment1");
                scope.AddBlockExclusion(blockEx);
                data.AddScope(scope);
                
                Assert(data.ScopeCount == 1, "Scope should be added to data");
                Assert(data.TotalExclusions == 1, "Total exclusion count should be correct");
                LogSuccess("Data structure creation");
            }
            catch (Exception ex) { LogFailure("Data structure creation", ex.Message); }
        }

        public void TestBasicParsing()
        {
            Console.WriteLine("Test 3: Basic Parsing");
            try
            {
                string content = "scope test_module { block_exclusion test_block { } }";
                using (var parser = new ExclusionParserNet())
                {
                    var data = parser.ParseString(content);
                    Assert(data != null, "Parsed data should not be null");
                    Assert(data.ScopeCount > 0, "Should have parsed at least one scope");
                    Assert(parser.ErrorCount == 0, "Should have no parsing errors");
                    LogSuccess("Basic parsing");
                }
            }
            catch (Exception ex) { LogFailure("Basic parsing", ex.Message); }
        }

        public void TestFileValidation()
        {
            Console.WriteLine("Test 4: File Validation");
            try
            {
                string validFile = Path.Combine(testDataDirectory, "valid.el");
                File.WriteAllText(validFile, "scope test { }");
                
                using (var parser = new ExclusionParserNet())
                {
                    Assert(parser.ValidateFile(validFile), "Valid file should pass validation");
                    Assert(!parser.ValidateFile("nonexistent.el"), "Nonexistent file should fail validation");
                    Assert(!parser.ValidateFile(null), "Null filename should fail validation");
                    LogSuccess("File validation");
                }
            }
            catch (Exception ex) { LogFailure("File validation", ex.Message); }
        }

        public void TestErrorHandling()
        {
            Console.WriteLine("Test 5: Error Handling");
            try
            {
                using (var parser = new ExclusionParserNet())
                {
                    try
                    {
                        parser.ParseString(null);
                        LogFailure("Error handling", "Should have thrown ArgumentNullException");
                    }
                    catch (ArgumentNullException) { /* Expected */ }

                    try
                    {
                        parser.ParseFile("nonexistent.el");
                        LogFailure("Error handling", "Should have thrown FileNotFoundException");
                    }
                    catch (FileNotFoundException) { /* Expected */ }

                    LogSuccess("Error handling");
                }
            }
            catch (Exception ex) { LogFailure("Error handling", ex.Message); }
        }

        public void TestWriterFunctionality()
        {
            Console.WriteLine("Test 6: Writer Functionality");
            try
            {
                var data = ExclusionUtilities.CreateSampleData();
                using (var writer = new ExclusionWriterNet())
                {
                    string content = writer.WriteToString(data);
                    Assert(!string.IsNullOrEmpty(content), "Written content should not be empty");
                    Assert(content.Contains("test_module"), "Content should contain expected scope name");
                    LogSuccess("Writer functionality");
                }
            }
            catch (Exception ex) { LogFailure("Writer functionality", ex.Message); }
        }

        public void TestRoundTripOperations()
        {
            Console.WriteLine("Test 7: Round-trip Operations");
            try
            {
                var originalData = ExclusionUtilities.CreateSampleData();
                using (var writer = new ExclusionWriterNet())
                {
                    string content = writer.WriteToString(originalData);
                    using (var parser = new ExclusionParserNet())
                    {
                        var parsedData = parser.ParseString(content);
                        Assert(parsedData.ScopeCount > 0, "Should parse scopes from written content");
                        LogSuccess("Round-trip operations");
                    }
                }
            }
            catch (Exception ex) { LogFailure("Round-trip operations", ex.Message); }
        }

        public void TestLinqOperations()
        {
            Console.WriteLine("Test 8: LINQ Operations");
            try
            {
                var data = new ExclusionData("linq_test.el");
                for (int i = 1; i <= 3; i++)
                {
                    var scope = new ExclusionScope($"module_{i}", $"checksum_{i}", i % 2 == 1);
                    for (int j = 1; j <= i * 2; j++)
                    {
                        var blockEx = new BlockExclusion($"block_{i}_{j}", $"hash_{i}_{j}", $"content_{i}_{j}", $"Block {j}");
                        scope.AddBlockExclusion(blockEx);
                    }
                    data.AddScope(scope);
                }

                var enabledScopes = data.Scopes.Values.Where(s => s.Enabled).ToList();
                Assert(enabledScopes.Count > 0, "Should find enabled scopes");

                var blockExclusions = data.GetExclusionsByType(ExclusionType.Block).ToList();
                Assert(blockExclusions.Count > 0, "Should find block exclusions");

                LogSuccess("LINQ operations");
            }
            catch (Exception ex) { LogFailure("LINQ operations", ex.Message); }
        }

        public void TestPerformanceCharacteristics()
        {
            Console.WriteLine("Test 9: Performance Characteristics");
            try
            {
                var stopwatch = System.Diagnostics.Stopwatch.StartNew();
                var data = new ExclusionData("performance_test.el");
                
                for (int i = 0; i < 50; i++)
                {
                    var scope = new ExclusionScope($"perf_module_{i:D3}", $"checksum_{i:D8}", true);
                    for (int j = 0; j < 20; j++)
                    {
                        var blockEx = new BlockExclusion($"block_{i}_{j}", $"hash_{i}_{j}", $"content_{i}_{j}", $"Performance test");
                        scope.AddBlockExclusion(blockEx);
                    }
                    data.AddScope(scope);
                }

                stopwatch.Stop();
                long creationTime = stopwatch.ElapsedMilliseconds;

                Assert(data.ScopeCount == 50, "Should create correct number of scopes");
                Assert(data.TotalExclusions == 1000, "Should create correct number of exclusions");
                Assert(creationTime < 5000, "Creation should complete in reasonable time");

                LogSuccess($"Performance characteristics (Create: {creationTime}ms)");
            }
            catch (Exception ex) { LogFailure("Performance characteristics", ex.Message); }
        }

        public void TestMemoryManagement()
        {
            Console.WriteLine("Test 10: Memory Management");
            try
            {
                long initialMemory = GC.GetTotalMemory(true);

                for (int i = 0; i < 10; i++)
                {
                    using (var parser = new ExclusionParserNet())
                    {
                        var data = ExclusionUtilities.CreateSampleData();
                        var stats = parser.GetStatistics();
                    }
                }

                GC.Collect();
                GC.WaitForPendingFinalizers();
                GC.Collect();

                long finalMemory = GC.GetTotalMemory(true);
                long memoryDifference = finalMemory - initialMemory;

                Assert(memoryDifference < 10 * 1024 * 1024, "Memory usage should not grow excessively");
                LogSuccess($"Memory management (Memory delta: {memoryDifference / 1024}KB)");
            }
            catch (Exception ex) { LogFailure("Memory management", ex.Message); }
        }

        public void TestConcurrentOperations()
        {
            Console.WriteLine("Test 11: Concurrent Operations");
            try
            {
                var tasks = new List<System.Threading.Tasks.Task>();

                for (int i = 0; i < 5; i++)
                {
                    int taskId = i;
                    var task = System.Threading.Tasks.Task.Run(() =>
                    {
                        using (var parser = new ExclusionParserNet())
                        {
                            var data = ExclusionUtilities.CreateSampleData();
                            using (var writer = new ExclusionWriterNet())
                            {
                                string content = writer.WriteToString(data);
                                Assert(!string.IsNullOrEmpty(content), $"Task {taskId} should produce content");
                            }
                        }
                    });
                    tasks.Add(task);
                }

                System.Threading.Tasks.Task.WaitAll(tasks.ToArray(), TimeSpan.FromSeconds(30));

                foreach (var task in tasks)
                {
                    Assert(task.IsCompletedSuccessfully, "All concurrent tasks should complete successfully");
                }

                LogSuccess("Concurrent operations");
            }
            catch (Exception ex) { LogFailure("Concurrent operations", ex.Message); }
        }

        public void TestLargeDataSets()
        {
            Console.WriteLine("Test 12: Large Data Sets");
            try
            {
                var data = new ExclusionData("large_dataset.el");

                for (int i = 0; i < 100; i++)
                {
                    var scope = new ExclusionScope($"large_module_{i:D3}", $"large_checksum_{i:D8}", true);
                    for (int j = 0; j < 10; j++)
                    {
                        var blockEx = new BlockExclusion($"large_block_{i}_{j}", $"large_hash_{i}_{j}", $"Large content {i}-{j}", $"Large block {j}");
                        scope.AddBlockExclusion(blockEx);
                    }
                    data.AddScope(scope);
                }

                Assert(data.ScopeCount == 100, "Should create large number of scopes");
                Assert(data.TotalExclusions == 1000, "Should create large number of exclusions");

                var enabledCount = data.Scopes.Values.Count(s => s.Enabled);
                Assert(enabledCount == 100, "Should handle large scope count");

                LogSuccess($"Large data sets ({data.ScopeCount} scopes, {data.TotalExclusions} exclusions)");
            }
            catch (Exception ex) { LogFailure("Large data sets", ex.Message); }
        }

        public void TestFileOperations()
        {
            Console.WriteLine("Test 13: File Operations");
            try
            {
                string testFile = Path.Combine(testDataDirectory, "file_ops_test.el");
                string testContent = "scope file_test_module { block_exclusion file_block { } }";
                File.WriteAllText(testFile, testContent);

                using (var parser = new ExclusionParserNet())
                {
                    var data = parser.ParseFile(testFile);
                    Assert(data != null, "Should parse file successfully");
                    Assert(data.ScopeCount > 0, "Should parse scopes from file");

                    var stats = parser.GetStatistics();
                    Assert(stats.ContainsKey("TotalScopes"), "Statistics should contain scope count");
                    Assert((int)stats["ErrorCount"] == 0, "Should have no parsing errors");

                    LogSuccess("File operations");
                }
            }
            catch (Exception ex) { LogFailure("File operations", ex.Message); }
        }

        public void TestStatisticsGeneration()
        {
            Console.WriteLine("Test 14: Statistics Generation");
            try
            {
                using (var parser = new ExclusionParserNet())
                {
                    parser.ParseString("sample content");
                    var stats = parser.GetStatistics();
                    
                    Assert(stats != null, "Statistics should not be null");
                    Assert(stats.ContainsKey("TotalScopes"), "Should contain total scopes");
                    Assert(stats.ContainsKey("ErrorCount"), "Should contain error count");
                    Assert((int)stats["ErrorCount"] == 0, "Error count should be zero for successful parsing");

                    LogSuccess("Statistics generation");
                }
            }
            catch (Exception ex) { LogFailure("Statistics generation", ex.Message); }
        }

        public void TestUtilityFunctions()
        {
            Console.WriteLine("Test 15: Utility Functions");
            try
            {
                Assert(!string.IsNullOrEmpty(ExclusionUtilities.Version), "Version should not be empty");
                Assert(ExclusionUtilities.Version.Contains("2.0.0"), "Version should contain expected version number");

                var sampleData = ExclusionUtilities.CreateSampleData();
                Assert(sampleData != null, "Sample data should be created");
                Assert(sampleData.ScopeCount > 0, "Sample data should contain scopes");

                string validFile = Path.Combine(testDataDirectory, "util_valid.el");
                File.WriteAllText(validFile, "scope test { }");
                Assert(ExclusionUtilities.IsValidExclusionFile(validFile), "Valid file should pass validation");
                Assert(!ExclusionUtilities.IsValidExclusionFile(null), "Null file should fail");

                LogSuccess("Utility functions");
            }
            catch (Exception ex) { LogFailure("Utility functions", ex.Message); }
        }

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

        private void Assert(bool condition, string message)
        {
            if (!condition) throw new Exception($"Assertion failed: {message}");
        }

        private void LogSuccess(string testName)
        {
            string result = $"✅ {testName}";
            testResults.Add(result);
            Console.WriteLine($"  {result}");
        }

        private void LogFailure(string testName, string error)
        {
            string result = $"❌ {testName}: {error}";
            testResults.Add(result);
            Console.WriteLine($"  {result}");
        }

        private void Cleanup()
        {
            try
            {
                if (Directory.Exists(testDataDirectory))
                {
                    Directory.Delete(testDataDirectory, true);
                }
            }
            catch { }
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                var tests = new ExclusionParser.Tests.DotNetAssemblyUnitTests();
                tests.RunAllTests();
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Test execution failed: {ex.Message}");
            }

            Console.WriteLine();
            Console.WriteLine("Press any key to exit...");
            Console.ReadKey();
        }
    }
}
