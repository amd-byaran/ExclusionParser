/**
 * @file ExclusionParserTests.NET.cs
 * @brief Unit tests for the ExclusionParser .NET assembly
 * 
 * This file contains comprehensive unit tests for the managed .NET wrapper
 * classes of the ExclusionParser library. Tests validate functionality,
 * error handling, and interoperability with the native implementation.
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
using Microsoft.VisualStudio.TestTools.UnitTesting;
using ExclusionParser.Managed;

namespace ExclusionParser.Tests
{
    /// <summary>
    /// Test class for ExclusionParser .NET assembly functionality
    /// </summary>
    [TestClass]
    public class ExclusionParserNETTests
    {
        private string testDataDirectory;
        private string tempTestFile;

        /// <summary>
        /// Test initialization - create test data directory and sample files
        /// </summary>
        [TestInitialize]
        public void Initialize()
        {
            testDataDirectory = Path.Combine(Path.GetTempPath(), "ExclusionParserTests");
            Directory.CreateDirectory(testDataDirectory);
            
            tempTestFile = Path.Combine(testDataDirectory, "test_exclusions.el");
            CreateSampleExclusionFile(tempTestFile);
        }

        /// <summary>
        /// Test cleanup - remove temporary files and directories
        /// </summary>
        [TestCleanup]
        public void Cleanup()
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
        /// Test basic parser construction and properties
        /// </summary>
        [TestMethod]
        public void TestParserConstruction()
        {
            using (var parser = new ExclusionParserNet())
            {
                Assert.IsNotNull(parser);
                Assert.AreEqual(0, parser.ErrorCount);
                Assert.IsTrue(string.IsNullOrEmpty(parser.LastError));
                Assert.IsNull(parser.Data);
            }
        }

        /// <summary>
        /// Test parsing a valid exclusion file
        /// </summary>
        [TestMethod]
        public void TestParseValidFile()
        {
            using (var parser = new ExclusionParserNet())
            {
                var data = parser.ParseFile(tempTestFile);
                
                Assert.IsNotNull(data);
                Assert.IsTrue(data.ScopeCount > 0);
                Assert.AreEqual(tempTestFile, data.FileName);
                Assert.AreEqual(0, parser.ErrorCount);
                
                // Verify we have the expected sample data
                Assert.IsTrue(data.Scopes.ContainsKey("test_module"));
                var scope = data.Scopes["test_module"];
                Assert.IsTrue(scope.Enabled);
                Assert.IsTrue(scope.BlockExclusions.Count > 0);
                Assert.IsTrue(scope.ToggleExclusions.Count > 0);
            }
        }

        /// <summary>
        /// Test parsing from string content
        /// </summary>
        [TestMethod]
        public void TestParseFromString()
        {
            string content = CreateSampleExclusionContent();
            
            using (var parser = new ExclusionParserNet())
            {
                var data = parser.ParseString(content);
                
                Assert.IsNotNull(data);
                Assert.IsTrue(data.ScopeCount > 0);
                Assert.AreEqual(0, parser.ErrorCount);
                
                // Verify structure
                Assert.IsTrue(data.Scopes.ContainsKey("test_module"));
                var scope = data.Scopes["test_module"];
                Assert.IsNotNull(scope);
            }
        }

        /// <summary>
        /// Test parsing with invalid file path
        /// </summary>
        [TestMethod]
        [ExpectedException(typeof(FileNotFoundException))]
        public void TestParseInvalidFile()
        {
            using (var parser = new ExclusionParserNet())
            {
                parser.ParseFile("nonexistent_file.el");
            }
        }

        /// <summary>
        /// Test parsing with null parameters
        /// </summary>
        [TestMethod]
        [ExpectedException(typeof(ArgumentNullException))]
        public void TestParseNullFileName()
        {
            using (var parser = new ExclusionParserNet())
            {
                parser.ParseFile(null);
            }
        }

        /// <summary>
        /// Test parsing with null string content
        /// </summary>
        [TestMethod]
        [ExpectedException(typeof(ArgumentNullException))]
        public void TestParseNullString()
        {
            using (var parser = new ExclusionParserNet())
            {
                parser.ParseString(null);
            }
        }

        /// <summary>
        /// Test file validation functionality
        /// </summary>
        [TestMethod]
        public void TestFileValidation()
        {
            using (var parser = new ExclusionParserNet())
            {
                Assert.IsTrue(parser.ValidateFile(tempTestFile));
                Assert.IsFalse(parser.ValidateFile("nonexistent.el"));
                Assert.IsFalse(parser.ValidateFile(null));
            }
        }

        /// <summary>
        /// Test parsing statistics generation
        /// </summary>
        [TestMethod]
        public void TestParsingStatistics()
        {
            using (var parser = new ExclusionParserNet())
            {
                var data = parser.ParseFile(tempTestFile);
                var stats = parser.GetStatistics();
                
                Assert.IsNotNull(stats);
                Assert.IsTrue(stats.ContainsKey("TotalScopes"));
                Assert.IsTrue(stats.ContainsKey("TotalExclusions"));
                Assert.IsTrue(stats.ContainsKey("BlockExclusions"));
                Assert.IsTrue(stats.ContainsKey("ToggleExclusions"));
                Assert.IsTrue(stats.ContainsKey("ErrorCount"));
                
                Assert.AreEqual(data.ScopeCount, (int)stats["TotalScopes"]);
                Assert.AreEqual(0, (int)stats["ErrorCount"]);
            }
        }

        /// <summary>
        /// Test writer construction and basic operations
        /// </summary>
        [TestMethod]
        public void TestWriterConstruction()
        {
            using (var writer = new ExclusionWriterNet())
            {
                Assert.IsNotNull(writer);
            }
        }

        /// <summary>
        /// Test writing exclusion data to file
        /// </summary>
        [TestMethod]
        public void TestWriteToFile()
        {
            string outputFile = Path.Combine(testDataDirectory, "output_test.el");
            
            try
            {
                var data = ExclusionUtilities.CreateSampleData();
                
                using (var writer = new ExclusionWriterNet())
                {
                    Assert.IsTrue(writer.Open(outputFile));
                    writer.WriteHeader("ExclusionParser.NET Tests", "2.0.0", "verification");
                    writer.WriteData(data);
                    writer.Close();
                }
                
                Assert.IsTrue(File.Exists(outputFile));
                Assert.IsTrue(new FileInfo(outputFile).Length > 0);
                
                // Verify we can parse the written file
                using (var parser = new ExclusionParserNet())
                {
                    var parsedData = parser.ParseFile(outputFile);
                    Assert.IsNotNull(parsedData);
                    Assert.IsTrue(parsedData.ScopeCount > 0);
                }
            }
            finally
            {
                if (File.Exists(outputFile))
                    File.Delete(outputFile);
            }
        }

        /// <summary>
        /// Test writing exclusion data to string
        /// </summary>
        [TestMethod]
        public void TestWriteToString()
        {
            var data = ExclusionUtilities.CreateSampleData();
            
            using (var writer = new ExclusionWriterNet())
            {
                string result = writer.WriteToString(data);
                
                Assert.IsNotNull(result);
                Assert.IsTrue(result.Length > 0);
                Assert.IsTrue(result.Contains("test_module"));
                Assert.IsTrue(result.Contains("block_001"));
                
                // Verify we can parse the written string
                using (var parser = new ExclusionParserNet())
                {
                    var parsedData = parser.ParseString(result);
                    Assert.IsNotNull(parsedData);
                    Assert.IsTrue(parsedData.ScopeCount > 0);
                }
            }
        }

        /// <summary>
        /// Test writer error handling with invalid parameters
        /// </summary>
        [TestMethod]
        [ExpectedException(typeof(ArgumentNullException))]
        public void TestWriterNullData()
        {
            using (var writer = new ExclusionWriterNet())
            {
                writer.WriteData(null);
            }
        }

        /// <summary>
        /// Test utility functions
        /// </summary>
        [TestMethod]
        public void TestUtilities()
        {
            // Test version property
            Assert.IsNotNull(ExclusionUtilities.Version);
            Assert.AreEqual("2.0.0", ExclusionUtilities.Version);
            
            // Test file validation
            Assert.IsTrue(ExclusionUtilities.IsValidExclusionFile(tempTestFile));
            Assert.IsFalse(ExclusionUtilities.IsValidExclusionFile("nonexistent.el"));
            
            // Test sample data creation
            var sampleData = ExclusionUtilities.CreateSampleData();
            Assert.IsNotNull(sampleData);
            Assert.IsTrue(sampleData.ScopeCount > 0);
            Assert.IsTrue(sampleData.TotalExclusions > 0);
        }

        /// <summary>
        /// Test managed data structure operations
        /// </summary>
        [TestMethod]
        public void TestManagedDataStructures()
        {
            var data = new ExclusionData("test.el");
            data.SetMetadata("tool", "test");
            data.SetMetadata("version", "1.0");
            
            var scope = new ExclusionScope("test_scope", "checksum123", true);
            
            var blockEx = new BlockExclusion("block1", "checksum1", "always @(posedge clk)", "Test block");
            scope.AddBlockExclusion(blockEx);
            
            var toggleEx = new ToggleExclusion(ToggleDirection.Both, "test_signal", null, "test_path", "Test toggle");
            scope.AddToggleExclusion(toggleEx);
            
            data.AddScope(scope);
            
            // Verify structure
            Assert.AreEqual(1, data.ScopeCount);
            Assert.AreEqual(2, data.TotalExclusions);
            Assert.AreEqual("test", data.GetMetadata("tool"));
            Assert.IsTrue(data.Scopes.ContainsKey("test_scope"));
            
            var retrievedScope = data.Scopes["test_scope"];
            Assert.AreEqual(1, retrievedScope.BlockExclusions.Count);
            Assert.AreEqual(1, retrievedScope.ToggleExclusions.Count);
        }

        /// <summary>
        /// Test LINQ operations on exclusion data
        /// </summary>
        [TestMethod]
        public void TestLinqOperations()
        {
            using (var parser = new ExclusionParserNet())
            {
                var data = parser.ParseFile(tempTestFile);
                
                // Test LINQ queries
                var enabledScopes = data.Scopes.Values.Where(s => s.Enabled).ToList();
                Assert.IsTrue(enabledScopes.Count > 0);
                
                var blockExclusions = data.GetExclusionsByType(ExclusionType.Block);
                Assert.IsNotNull(blockExclusions);
                
                var toggleExclusions = data.GetExclusionsByType(ExclusionType.Toggle);
                Assert.IsNotNull(toggleExclusions);
                
                // Test filtering and aggregation
                var totalExclusions = data.Scopes.Values
                    .Sum(s => s.BlockExclusions.Count + s.ToggleExclusions.Count + 
                             s.FsmExclusions.Count + s.ConditionExclusions.Count);
                Assert.AreEqual(data.TotalExclusions, totalExclusions);
            }
        }

        /// <summary>
        /// Test round-trip parsing and writing
        /// </summary>
        [TestMethod]
        public void TestRoundTripOperation()
        {
            string outputFile = Path.Combine(testDataDirectory, "roundtrip_test.el");
            
            try
            {
                // Parse original file
                ExclusionData originalData;
                using (var parser = new ExclusionParserNet())
                {
                    originalData = parser.ParseFile(tempTestFile);
                }
                
                // Write to new file
                using (var writer = new ExclusionWriterNet())
                {
                    Assert.IsTrue(writer.Open(outputFile));
                    writer.WriteHeader("Roundtrip Test", "2.0.0", "verification");
                    writer.WriteData(originalData);
                    writer.Close();
                }
                
                // Parse the written file
                ExclusionData roundtripData;
                using (var parser = new ExclusionParserNet())
                {
                    roundtripData = parser.ParseFile(outputFile);
                }
                
                // Compare data structures
                Assert.AreEqual(originalData.ScopeCount, roundtripData.ScopeCount);
                Assert.AreEqual(originalData.TotalExclusions, roundtripData.TotalExclusions);
                
                foreach (var scopeName in originalData.Scopes.Keys)
                {
                    Assert.IsTrue(roundtripData.Scopes.ContainsKey(scopeName));
                    var originalScope = originalData.Scopes[scopeName];
                    var roundtripScope = roundtripData.Scopes[scopeName];
                    
                    Assert.AreEqual(originalScope.Enabled, roundtripScope.Enabled);
                    Assert.AreEqual(originalScope.BlockExclusions.Count, roundtripScope.BlockExclusions.Count);
                    Assert.AreEqual(originalScope.ToggleExclusions.Count, roundtripScope.ToggleExclusions.Count);
                }
            }
            finally
            {
                if (File.Exists(outputFile))
                    File.Delete(outputFile);
            }
        }

        /// <summary>
        /// Creates a sample exclusion file for testing
        /// </summary>
        /// <param name="filePath">Path where to create the file</param>
        private void CreateSampleExclusionFile(string filePath)
        {
            string content = CreateSampleExclusionContent();
            File.WriteAllText(filePath, content);
        }

        /// <summary>
        /// Creates sample exclusion file content
        /// </summary>
        /// <returns>Sample exclusion content as string</returns>
        private string CreateSampleExclusionContent()
        {
            return @"// ExclusionParser Test File
// Generated by: ExclusionParser.NET Tests
// Version: 2.0.0
// Mode: verification

scope test_module {
    checksum: test_checksum_123
    enabled: true
    
    block_exclusion block_001 {
        checksum: block_checksum_456
        content: ""always @(posedge clk) begin""
        comment: ""Sample block exclusion for testing""
    }
    
    toggle_exclusion {
        direction: both
        signal: test_signal
        hierarchy: test_module.test_signal
        comment: ""Sample toggle exclusion for testing""
    }
    
    fsm_exclusion {
        fsm: test_fsm
        state: idle_state
        transition: idle_to_active
        hierarchy: test_module.test_fsm
        comment: ""Sample FSM exclusion for testing""
    }
    
    condition_exclusion {
        condition: ""(reset == 1'b1)""
        hierarchy: test_module.reset_condition
        comment: ""Sample condition exclusion for testing""
    }
}

scope disabled_module {
    checksum: disabled_checksum_789
    enabled: false
    
    block_exclusion block_002 {
        checksum: block_checksum_789
        content: ""initial begin""
        comment: ""Disabled block exclusion""
    }
}";
        }
    }
}