/**
 * @file ExclusionParserExample.cs
 * @brief C# usage examples for the ExclusionParser .NET assembly
 * 
 * This file demonstrates how to use the ExclusionParser .NET assembly
 * from C# for parsing and writing hardware verification exclusion files.
 * The examples show common usage patterns and best practices.
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
using ExclusionParser.Managed;

namespace ExclusionParser.Examples
{
    /// <summary>
    /// Comprehensive examples demonstrating ExclusionParser .NET assembly usage
    /// </summary>
    public class ExclusionParserExamples
    {
        /// <summary>
        /// Example 1: Basic file parsing and data access
        /// </summary>
        public static void BasicParsingExample()
        {
            Console.WriteLine("=== Basic Parsing Example ===");
            
            try
            {
                using (var parser = new ExclusionParserNet())
                {
                    // Parse an exclusion file
                    string fileName = "sample_exclusions.el";
                    var data = parser.ParseFile(fileName);
                    
                    Console.WriteLine($"File: {data.FileName}");
                    Console.WriteLine($"Scopes: {data.ScopeCount}");
                    Console.WriteLine($"Total Exclusions: {data.TotalExclusions}");
                    
                    // Access metadata
                    Console.WriteLine("Metadata:");
                    foreach (var meta in data.Metadata)
                    {
                        Console.WriteLine($"  {meta.Key}: {meta.Value}");
                    }
                    
                    // Iterate through scopes
                    foreach (var scope in data.Scopes.Values)
                    {
                        Console.WriteLine($"Scope: {scope.Name} (Enabled: {scope.Enabled})");
                        Console.WriteLine($"  Block exclusions: {scope.BlockExclusions.Count}");
                        Console.WriteLine($"  Toggle exclusions: {scope.ToggleExclusions.Count}");
                        Console.WriteLine($"  FSM exclusions: {scope.FsmExclusions.Count}");
                        Console.WriteLine($"  Condition exclusions: {scope.ConditionExclusions.Count}");
                    }
                }
            }
            catch (FileNotFoundException)
            {
                Console.WriteLine("Sample file not found. Creating sample data instead...");
                CreateSampleDataExample();
            }
            catch (ExclusionParserException ex)
            {
                Console.WriteLine($"Parsing error: {ex.Message}");
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Unexpected error: {ex.Message}");
            }
        }
        
        /// <summary>
        /// Example 2: Creating sample data programmatically
        /// </summary>
        public static void CreateSampleDataExample()
        {
            Console.WriteLine("=== Creating Sample Data Example ===");
            
            // Create exclusion data structure
            var data = new ExclusionData("programmatic_sample.el");
            data.SetMetadata("generated_by", "ExclusionParser.NET Example");
            data.SetMetadata("version", "2.0.0");
            data.SetMetadata("creation_date", DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss"));
            
            // Create a scope for a CPU module
            var cpuScope = new ExclusionScope("cpu_core", "cpu_checksum_12345", true);
            
            // Add block exclusions
            var blockEx1 = new BlockExclusion(
                "pipeline_hazard_detection", 
                "block_hash_567", 
                "always @(posedge clk) begin if (stall) ...", 
                "Exclude pipeline hazard detection during reset"
            );
            cpuScope.AddBlockExclusion(blockEx1);
            
            var blockEx2 = new BlockExclusion(
                "cache_miss_handler",
                "block_hash_890",
                "always_comb begin case (cache_state) ...",
                "Cache miss handling not tested in this configuration"
            );
            cpuScope.AddBlockExclusion(blockEx2);
            
            // Add toggle exclusions
            var toggleEx1 = new ToggleExclusion(
                ToggleDirection.High,
                "interrupt_pending",
                null,
                "cpu_core.interrupt_controller",
                "Interrupt pending signal - high coverage not achievable"
            );
            cpuScope.AddToggleExclusion(toggleEx1);
            
            var toggleEx2 = new ToggleExclusion(
                ToggleDirection.Both,
                "debug_enable",
                0, // bit index
                "cpu_core.debug_unit",
                "Debug enable bit - both transitions excluded"
            );
            cpuScope.AddToggleExclusion(toggleEx2);
            
            // Add FSM exclusions
            var fsmEx = new FsmExclusion(
                "power_management_fsm",
                "deep_sleep",
                "active_to_deep_sleep",
                "cpu_core.power_mgmt",
                "Deep sleep transition not testable in current environment"
            );
            cpuScope.AddFsmExclusion(fsmEx);
            
            // Add condition exclusions
            var condEx = new ConditionExclusion(
                "(reset_n == 1'b0) && (power_good == 1'b0)",
                "cpu_core.reset_logic",
                "Reset condition during power-off not testable"
            );
            cpuScope.AddConditionExclusion(condEx);
            
            data.AddScope(cpuScope);
            
            // Create a scope for memory controller
            var memScope = new ExclusionScope("memory_controller", "mem_checksum_54321", true);
            
            var memBlockEx = new BlockExclusion(
                "ecc_correction",
                "ecc_hash_111",
                "if (ecc_error) begin correct_data();",
                "ECC correction logic - errors not injected in test"
            );
            memScope.AddBlockExclusion(memBlockEx);
            
            data.AddScope(memScope);
            
            // Display the created data
            Console.WriteLine($"Created exclusion data with {data.ScopeCount} scopes:");
            Console.WriteLine($"Total exclusions: {data.TotalExclusions}");
            
            // Use LINQ to analyze the data
            var enabledScopes = data.Scopes.Values.Where(s => s.Enabled).ToList();
            Console.WriteLine($"Enabled scopes: {enabledScopes.Count}");
            
            var blockExclusions = data.GetExclusionsByType(ExclusionType.Block);
            Console.WriteLine($"Block exclusions across all scopes: {blockExclusions.Count()}");
            
            var toggleExclusions = data.GetExclusionsByType(ExclusionType.Toggle);
            Console.WriteLine($"Toggle exclusions across all scopes: {toggleExclusions.Count()}");
        }
        
        /// <summary>
        /// Example 3: Writing exclusion data to files and strings
        /// </summary>
        public static void WritingExample()
        {
            Console.WriteLine("=== Writing Example ===");
            
            try
            {
                // Create sample data
                var data = ExclusionUtilities.CreateSampleData();
                
                using (var writer = new ExclusionWriterNet())
                {
                    // Write to string first
                    string exclusionContent = writer.WriteToString(data);
                    Console.WriteLine("Generated exclusion content:");
                    Console.WriteLine(exclusionContent);
                    Console.WriteLine($"Content length: {exclusionContent.Length} characters");
                    
                    // Write to file
                    string outputFile = "generated_exclusions.el";
                    if (writer.Open(outputFile))
                    {
                        writer.WriteHeader("ExclusionParser.NET C# Example", "2.0.0", "verification");
                        writer.WriteData(data);
                        writer.Close();
                        
                        Console.WriteLine($"Written to file: {outputFile}");
                        
                        // Verify by reading the file size
                        if (File.Exists(outputFile))
                        {
                            var fileInfo = new FileInfo(outputFile);
                            Console.WriteLine($"File size: {fileInfo.Length} bytes");
                        }
                    }
                    else
                    {
                        Console.WriteLine("Failed to open output file");
                    }
                }
                
                // Verify by parsing the written file
                using (var parser = new ExclusionParserNet())
                {
                    if (File.Exists("generated_exclusions.el"))
                    {
                        var parsedData = parser.ParseFile("generated_exclusions.el");
                        Console.WriteLine($"Verification: Parsed {parsedData.ScopeCount} scopes from written file");
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Writing error: {ex.Message}");
            }
        }
        
        /// <summary>
        /// Example 4: LINQ queries and data analysis
        /// </summary>
        public static void LinqAnalysisExample()
        {
            Console.WriteLine("=== LINQ Analysis Example ===");
            
            // Create comprehensive test data
            var data = new ExclusionData("analysis_sample.el");
            
            // Create multiple scopes with various exclusions
            for (int i = 1; i <= 3; i++)
            {
                var scope = new ExclusionScope($"module_{i}", $"checksum_{i}", i % 2 == 1);
                
                // Add some block exclusions
                for (int j = 1; j <= i * 2; j++)
                {
                    var blockEx = new BlockExclusion(
                        $"block_{i}_{j}",
                        $"block_hash_{i}_{j}",
                        $"always @(posedge clk) begin // Module {i} Block {j}",
                        $"Block exclusion {j} in module {i}"
                    );
                    scope.AddBlockExclusion(blockEx);
                }
                
                // Add toggle exclusions
                for (int j = 1; j <= i; j++)
                {
                    var toggleEx = new ToggleExclusion(
                        (ToggleDirection)(j % 3),
                        $"signal_{i}_{j}",
                        j % 2 == 0 ? j : (int?)null,
                        $"module_{i}.signal_path",
                        $"Toggle exclusion {j} in module {i}"
                    );
                    scope.AddToggleExclusion(toggleEx);
                }
                
                data.AddScope(scope);
            }
            
            Console.WriteLine($"Analysis data: {data.ScopeCount} scopes, {data.TotalExclusions} total exclusions");
            
            // LINQ Analysis Examples
            
            // 1. Find enabled scopes
            var enabledScopes = data.Scopes.Values
                .Where(s => s.Enabled)
                .Select(s => s.Name)
                .ToList();
            Console.WriteLine($"Enabled scopes: {string.Join(", ", enabledScopes)}");
            
            // 2. Count exclusions by type
            var exclusionsByType = data.Scopes.Values
                .SelectMany(s => s.GetAllExclusions())
                .GroupBy(e => e.GetType().Name)
                .ToDictionary(g => g.Key, g => g.Count());
            
            Console.WriteLine("Exclusions by type:");
            foreach (var kvp in exclusionsByType)
            {
                Console.WriteLine($"  {kvp.Key}: {kvp.Value}");
            }
            
            // 3. Find scopes with high exclusion counts
            var highExclusionScopes = data.Scopes.Values
                .Where(s => s.GetTotalExclusionCount() > 3)
                .OrderByDescending(s => s.GetTotalExclusionCount())
                .ToList();
            
            Console.WriteLine("Scopes with high exclusion counts:");
            foreach (var scope in highExclusionScopes)
            {
                Console.WriteLine($"  {scope.Name}: {scope.GetTotalExclusionCount()} exclusions");
            }
            
            // 4. Find toggle exclusions with bit indices
            var indexedToggles = data.Scopes.Values
                .SelectMany(s => s.ToggleExclusions.Values)
                .Where(t => t.BitIndex.HasValue)
                .ToList();
            
            Console.WriteLine($"Toggle exclusions with bit indices: {indexedToggles.Count}");
            foreach (var toggle in indexedToggles)
            {
                Console.WriteLine($"  {toggle.SignalName}[{toggle.BitIndex}] - {toggle.Direction}");
            }
            
            // 5. Search by comment content
            var searchTerm = "module";
            var exclusionsWithSearchTerm = data.Scopes.Values
                .SelectMany(s => s.GetAllExclusions())
                .Where(e => e.ToString().ToLowerInvariant().Contains(searchTerm.ToLowerInvariant()))
                .ToList();
            
            Console.WriteLine($"Exclusions containing '{searchTerm}': {exclusionsWithSearchTerm.Count}");
        }
        
        /// <summary>
        /// Example 5: Error handling and validation
        /// </summary>
        public static void ErrorHandlingExample()
        {
            Console.WriteLine("=== Error Handling Example ===");
            
            using (var parser = new ExclusionParserNet())
            {
                // Test file not found
                try
                {
                    parser.ParseFile("nonexistent_file.el");
                }
                catch (FileNotFoundException ex)
                {
                    Console.WriteLine($"Expected error - File not found: {ex.Message}");
                }
                
                // Test null parameter
                try
                {
                    parser.ParseFile(null);
                }
                catch (ArgumentNullException ex)
                {
                    Console.WriteLine($"Expected error - Null parameter: {ex.ParameterName}");
                }
                
                // Test invalid content parsing
                try
                {
                    string invalidContent = "This is not valid exclusion file content!";
                    parser.ParseString(invalidContent);
                    
                    if (parser.ErrorCount > 0)
                    {
                        Console.WriteLine($"Parser errors: {parser.ErrorCount}");
                        Console.WriteLine($"Last error: {parser.LastError}");
                    }
                }
                catch (ExclusionParserException ex)
                {
                    Console.WriteLine($"Expected parsing error: {ex.Message}");
                }
                
                // File validation
                string[] testFiles = { "valid.el", "invalid.txt", null, "" };
                foreach (string file in testFiles)
                {
                    bool isValid = ExclusionUtilities.IsValidExclusionFile(file);
                    Console.WriteLine($"File '{file ?? "null"}' is valid: {isValid}");
                }
            }
        }
        
        /// <summary>
        /// Example 6: Performance and statistics
        /// </summary>
        public static void PerformanceExample()
        {
            Console.WriteLine("=== Performance Example ===");
            
            var stopwatch = System.Diagnostics.Stopwatch.StartNew();
            
            // Create large dataset for performance testing
            var data = new ExclusionData("performance_test.el");
            data.SetMetadata("test_type", "performance");
            
            // Add many scopes and exclusions
            for (int i = 0; i < 10; i++)
            {
                var scope = new ExclusionScope($"perf_module_{i:D3}", $"checksum_{i:D8}", true);
                
                // Add many exclusions to each scope
                for (int j = 0; j < 50; j++)
                {
                    if (j % 4 == 0)
                    {
                        var blockEx = new BlockExclusion(
                            $"block_{i}_{j}",
                            $"hash_{i}_{j}",
                            $"// Performance test block {i}-{j}",
                            $"Generated block exclusion {j}"
                        );
                        scope.AddBlockExclusion(blockEx);
                    }
                    else if (j % 4 == 1)
                    {
                        var toggleEx = new ToggleExclusion(
                            ToggleDirection.Both,
                            $"perf_signal_{j}",
                            j % 8 == 1 ? j : (int?)null,
                            $"perf_module_{i:D3}",
                            $"Performance toggle {j}"
                        );
                        scope.AddToggleExclusion(toggleEx);
                    }
                }
                
                data.AddScope(scope);
            }
            
            stopwatch.Stop();
            Console.WriteLine($"Data creation time: {stopwatch.ElapsedMilliseconds} ms");
            Console.WriteLine($"Created data with {data.ScopeCount} scopes and {data.TotalExclusions} exclusions");
            
            // Test LINQ performance
            stopwatch.Restart();
            
            var stats = new Dictionary<string, object>();
            stats["EnabledScopes"] = data.Scopes.Values.Count(s => s.Enabled);
            stats["BlockExclusions"] = data.GetExclusionsByType(ExclusionType.Block).Count();
            stats["ToggleExclusions"] = data.GetExclusionsByType(ExclusionType.Toggle).Count();
            stats["AverageExclusionsPerScope"] = data.TotalExclusions / (double)data.ScopeCount;
            
            stopwatch.Stop();
            Console.WriteLine($"Statistics calculation time: {stopwatch.ElapsedMilliseconds} ms");
            
            Console.WriteLine("Performance statistics:");
            foreach (var stat in stats)
            {
                Console.WriteLine($"  {stat.Key}: {stat.Value}");
            }
            
            // Test serialization performance
            stopwatch.Restart();
            
            using (var writer = new ExclusionWriterNet())
            {
                string serialized = writer.WriteToString(data);
                Console.WriteLine($"Serialization time: {stopwatch.ElapsedMilliseconds} ms");
                Console.WriteLine($"Serialized size: {serialized.Length:N0} characters");
            }
        }
        
        /// <summary>
        /// Main entry point for running all examples
        /// </summary>
        public static void Main(string[] args)
        {
            Console.WriteLine("ExclusionParser .NET Assembly - C# Examples");
            Console.WriteLine("===========================================");
            Console.WriteLine($"Assembly Version: {ExclusionUtilities.Version}");
            Console.WriteLine();
            
            try
            {
                BasicParsingExample();
                Console.WriteLine();
                
                CreateSampleDataExample();
                Console.WriteLine();
                
                WritingExample();
                Console.WriteLine();
                
                LinqAnalysisExample();
                Console.WriteLine();
                
                ErrorHandlingExample();
                Console.WriteLine();
                
                PerformanceExample();
                Console.WriteLine();
                
                Console.WriteLine("All examples completed successfully!");
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Example execution error: {ex.Message}");
                Console.WriteLine($"Stack trace: {ex.StackTrace}");
            }
            
            Console.WriteLine("\nPress any key to exit...");
            Console.ReadKey();
        }
    }
}