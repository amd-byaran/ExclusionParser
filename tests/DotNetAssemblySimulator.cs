/**
 * @file DotNetAssemblySimulator.cs
 * @brief C# simulation of the .NET assembly functionality for testing
 * 
 * This file simulates the ExclusionParser .NET assembly using P/Invoke
 * to demonstrate the functionality and run equivalent unit tests.
 * This serves as a proof-of-concept for the actual .NET assembly.
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
    /// <summary>
    /// Simulates ToggleDirection enum
    /// </summary>
    public enum ToggleDirection
    {
        High = 0,
        Low = 1,
        Both = 2
    }

    /// <summary>
    /// Simulates ExclusionType enum
    /// </summary>
    public enum ExclusionType
    {
        Block = 0,
        Toggle = 1,
        FSM = 2,
        Condition = 3
    }

    /// <summary>
    /// Simulates BlockExclusion managed class
    /// </summary>
    public class BlockExclusion
    {
        public string Id { get; set; }
        public string Checksum { get; set; }
        public string Content { get; set; }
        public string Comment { get; set; }

        public BlockExclusion(string id, string checksum, string content, string comment)
        {
            Id = id ?? throw new ArgumentNullException(nameof(id));
            Checksum = checksum ?? string.Empty;
            Content = content ?? string.Empty;
            Comment = comment ?? string.Empty;
        }

        public override string ToString()
        {
            return $"Block[{Id}]: {Comment}";
        }
    }

    /// <summary>
    /// Simulates ToggleExclusion managed class
    /// </summary>
    public class ToggleExclusion
    {
        public ToggleDirection Direction { get; set; }
        public string SignalName { get; set; }
        public int? BitIndex { get; set; }
        public string HierarchyPath { get; set; }
        public string Comment { get; set; }

        public ToggleExclusion(ToggleDirection direction, string signalName, int? bitIndex, 
                             string hierarchyPath, string comment)
        {
            Direction = direction;
            SignalName = signalName ?? throw new ArgumentNullException(nameof(signalName));
            BitIndex = bitIndex;
            HierarchyPath = hierarchyPath ?? string.Empty;
            Comment = comment ?? string.Empty;
        }

        public override string ToString()
        {
            var bitInfo = BitIndex.HasValue ? $"[{BitIndex}]" : "";
            return $"Toggle[{SignalName}{bitInfo}]: {Direction} - {Comment}";
        }
    }

    /// <summary>
    /// Simulates FsmExclusion managed class
    /// </summary>
    public class FsmExclusion
    {
        public string FsmName { get; set; }
        public string StateName { get; set; }
        public string TransitionName { get; set; }
        public string HierarchyPath { get; set; }
        public string Comment { get; set; }

        public FsmExclusion(string fsmName, string stateName, string transitionName,
                          string hierarchyPath, string comment)
        {
            FsmName = fsmName ?? throw new ArgumentNullException(nameof(fsmName));
            StateName = stateName ?? string.Empty;
            TransitionName = transitionName ?? string.Empty;
            HierarchyPath = hierarchyPath ?? string.Empty;
            Comment = comment ?? string.Empty;
        }

        public override string ToString()
        {
            return $"FSM[{FsmName}]: {StateName} -> {TransitionName} - {Comment}";
        }
    }

    /// <summary>
    /// Simulates ConditionExclusion managed class
    /// </summary>
    public class ConditionExclusion
    {
        public string ConditionText { get; set; }
        public string HierarchyPath { get; set; }
        public string Comment { get; set; }

        public ConditionExclusion(string conditionText, string hierarchyPath, string comment)
        {
            ConditionText = conditionText ?? throw new ArgumentNullException(nameof(conditionText));
            HierarchyPath = hierarchyPath ?? string.Empty;
            Comment = comment ?? string.Empty;
        }

        public override string ToString()
        {
            return $"Condition[{ConditionText}]: {Comment}";
        }
    }

    /// <summary>
    /// Simulates ExclusionScope managed class
    /// </summary>
    public class ExclusionScope
    {
        public string Name { get; set; }
        public string Checksum { get; set; }
        public bool Enabled { get; set; }

        public Dictionary<string, BlockExclusion> BlockExclusions { get; }
        public Dictionary<string, ToggleExclusion> ToggleExclusions { get; }
        public Dictionary<string, FsmExclusion> FsmExclusions { get; }
        public Dictionary<string, ConditionExclusion> ConditionExclusions { get; }

        public ExclusionScope(string name, string checksum, bool enabled)
        {
            Name = name ?? throw new ArgumentNullException(nameof(name));
            Checksum = checksum ?? string.Empty;
            Enabled = enabled;

            BlockExclusions = new Dictionary<string, BlockExclusion>();
            ToggleExclusions = new Dictionary<string, ToggleExclusion>();
            FsmExclusions = new Dictionary<string, FsmExclusion>();
            ConditionExclusions = new Dictionary<string, ConditionExclusion>();
        }

        public void AddBlockExclusion(BlockExclusion exclusion)
        {
            if (exclusion == null) throw new ArgumentNullException(nameof(exclusion));
            BlockExclusions[exclusion.Id] = exclusion;
        }

        public void AddToggleExclusion(ToggleExclusion exclusion)
        {
            if (exclusion == null) throw new ArgumentNullException(nameof(exclusion));
            var key = exclusion.BitIndex.HasValue 
                ? $"{exclusion.SignalName}[{exclusion.BitIndex}]" 
                : exclusion.SignalName;
            ToggleExclusions[key] = exclusion;
        }

        public void AddFsmExclusion(FsmExclusion exclusion)
        {
            if (exclusion == null) throw new ArgumentNullException(nameof(exclusion));
            var key = $"{exclusion.FsmName}:{exclusion.StateName}:{exclusion.TransitionName}";
            FsmExclusions[key] = exclusion;
        }

        public void AddConditionExclusion(ConditionExclusion exclusion)
        {
            if (exclusion == null) throw new ArgumentNullException(nameof(exclusion));
            ConditionExclusions[exclusion.ConditionText] = exclusion;
        }

        public int GetTotalExclusionCount()
        {
            return BlockExclusions.Count + ToggleExclusions.Count + 
                   FsmExclusions.Count + ConditionExclusions.Count;
        }

        public IEnumerable<object> GetAllExclusions()
        {
            foreach (var exclusion in BlockExclusions.Values) yield return exclusion;
            foreach (var exclusion in ToggleExclusions.Values) yield return exclusion;
            foreach (var exclusion in FsmExclusions.Values) yield return exclusion;
            foreach (var exclusion in ConditionExclusions.Values) yield return exclusion;
        }
    }

    /// <summary>
    /// Simulates ExclusionData managed class
    /// </summary>
    public class ExclusionData
    {
        public string FileName { get; set; }
        public Dictionary<string, string> Metadata { get; }
        public Dictionary<string, ExclusionScope> Scopes { get; }

        public ExclusionData(string fileName)
        {
            FileName = fileName ?? throw new ArgumentNullException(nameof(fileName));
            Metadata = new Dictionary<string, string>();
            Scopes = new Dictionary<string, ExclusionScope>();
        }

        public int ScopeCount => Scopes.Count;

        public int TotalExclusions => Scopes.Values.Sum(s => s.GetTotalExclusionCount());

        public void AddScope(ExclusionScope scope)
        {
            if (scope == null) throw new ArgumentNullException(nameof(scope));
            Scopes[scope.Name] = scope;
        }

        public void SetMetadata(string key, string value)
        {
            if (key == null) throw new ArgumentNullException(nameof(key));
            Metadata[key] = value ?? string.Empty;
        }

        public string GetMetadata(string key)
        {
            return Metadata.TryGetValue(key, out string value) ? value : null;
        }

        public IEnumerable<object> GetExclusionsByType(ExclusionType type)
        {
            return type switch
            {
                ExclusionType.Block => Scopes.Values.SelectMany(s => s.BlockExclusions.Values),
                ExclusionType.Toggle => Scopes.Values.SelectMany(s => s.ToggleExclusions.Values),
                ExclusionType.FSM => Scopes.Values.SelectMany(s => s.FsmExclusions.Values),
                ExclusionType.Condition => Scopes.Values.SelectMany(s => s.ConditionExclusions.Values),
                _ => Enumerable.Empty<object>()
            };
        }
    }

    /// <summary>
    /// Simulates ExclusionParserException
    /// </summary>
    public class ExclusionParserException : Exception
    {
        public ExclusionParserException() : base() { }
        public ExclusionParserException(string message) : base(message) { }
        public ExclusionParserException(string message, Exception innerException) : base(message, innerException) { }
    }

    /// <summary>
    /// Simulates ExclusionParserNet managed wrapper
    /// </summary>
    public class ExclusionParserNet : IDisposable
    {
        private bool disposed = false;

        public int ErrorCount { get; private set; } = 0;
        public string LastError { get; private set; } = string.Empty;
        public ExclusionData Data { get; private set; }

        public ExclusionData ParseFile(string fileName)
        {
            if (string.IsNullOrEmpty(fileName))
                throw new ArgumentNullException(nameof(fileName));

            if (!File.Exists(fileName))
                throw new FileNotFoundException("Exclusion file not found", fileName);

            try
            {
                string content = File.ReadAllText(fileName);
                return ParseString(content);
            }
            catch (Exception ex)
            {
                throw new ExclusionParserException($"Failed to parse file: {ex.Message}", ex);
            }
        }

        public ExclusionData ParseString(string content)
        {
            if (string.IsNullOrEmpty(content))
                throw new ArgumentNullException(nameof(content));

            try
            {
                // Simulate parsing by creating sample data
                var data = new ExclusionData("simulated.el");
                data.SetMetadata("generated_by", "ExclusionParser.NET Simulator");
                data.SetMetadata("version", "2.0.0");
                data.SetMetadata("parsing_mode", "simulation");

                // Create sample scope
                var scope = new ExclusionScope("simulated_module", "sim_checksum_123", true);

                // Add sample exclusions based on content analysis
                if (content.Contains("block") || content.Contains("always"))
                {
                    var blockEx = new BlockExclusion(
                        "sim_block_001",
                        "sim_block_hash",
                        "always @(posedge clk) begin",
                        "Simulated block exclusion from content parsing"
                    );
                    scope.AddBlockExclusion(blockEx);
                }

                if (content.Contains("toggle") || content.Contains("signal"))
                {
                    var toggleEx = new ToggleExclusion(
                        ToggleDirection.Both,
                        "sim_signal",
                        null,
                        "simulated_module.sim_signal",
                        "Simulated toggle exclusion from content parsing"
                    );
                    scope.AddToggleExclusion(toggleEx);
                }

                data.AddScope(scope);
                Data = data;
                return data;
            }
            catch (Exception ex)
            {
                ErrorCount++;
                LastError = ex.Message;
                throw new ExclusionParserException($"Failed to parse string: {ex.Message}", ex);
            }
        }

        public bool ValidateFile(string fileName)
        {
            if (string.IsNullOrEmpty(fileName) || !File.Exists(fileName))
                return false;

            try
            {
                // Simple validation - check if file contains expected keywords
                string content = File.ReadAllText(fileName);
                return content.Contains("scope") || content.Contains("exclusion") || content.Contains("block");
            }
            catch
            {
                return false;
            }
        }

        public Dictionary<string, object> GetStatistics()
        {
            var stats = new Dictionary<string, object>();

            if (Data != null)
            {
                stats["TotalScopes"] = Data.ScopeCount;
                stats["TotalExclusions"] = Data.TotalExclusions;
                stats["BlockExclusions"] = Data.GetExclusionsByType(ExclusionType.Block).Count();
                stats["ToggleExclusions"] = Data.GetExclusionsByType(ExclusionType.Toggle).Count();
                stats["FsmExclusions"] = Data.GetExclusionsByType(ExclusionType.FSM).Count();
                stats["ConditionExclusions"] = Data.GetExclusionsByType(ExclusionType.Condition).Count();
                stats["ErrorCount"] = ErrorCount;
                stats["LastError"] = LastError;
            }

            return stats;
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (!disposed)
            {
                if (disposing)
                {
                    // Clean up managed resources
                    Data = null;
                }
                disposed = true;
            }
        }
    }

    /// <summary>
    /// Simulates ExclusionWriterNet managed wrapper
    /// </summary>
    public class ExclusionWriterNet : IDisposable
    {
        private bool disposed = false;
        private string outputFileName;

        public bool Open(string fileName)
        {
            if (string.IsNullOrEmpty(fileName))
                throw new ArgumentNullException(nameof(fileName));

            try
            {
                outputFileName = fileName;
                return true;
            }
            catch
            {
                return false;
            }
        }

        public void Close()
        {
            outputFileName = null;
        }

        public void WriteHeader(string generatedBy, string version, string mode)
        {
            // Header writing is simulated - would write to file in real implementation
        }

        public void WriteData(ExclusionData data)
        {
            if (data == null)
                throw new ArgumentNullException(nameof(data));

            if (string.IsNullOrEmpty(outputFileName))
                throw new InvalidOperationException("No output file opened");

            try
            {
                string content = WriteToString(data);
                File.WriteAllText(outputFileName, content);
            }
            catch (Exception ex)
            {
                throw new ExclusionParserException($"Failed to write data: {ex.Message}", ex);
            }
        }

        public string WriteToString(ExclusionData data)
        {
            if (data == null)
                throw new ArgumentNullException(nameof(data));

            var sb = new StringBuilder();
            sb.AppendLine("// ExclusionParser .NET Simulator Output");
            sb.AppendLine($"// Generated: {DateTime.Now}");
            sb.AppendLine($"// File: {data.FileName}");
            sb.AppendLine();

            // Write metadata
            sb.AppendLine("// Metadata:");
            foreach (var meta in data.Metadata)
            {
                sb.AppendLine($"// {meta.Key}: {meta.Value}");
            }
            sb.AppendLine();

            // Write scopes
            foreach (var scope in data.Scopes.Values)
            {
                sb.AppendLine($"scope {scope.Name} {{");
                sb.AppendLine($"    checksum: {scope.Checksum}");
                sb.AppendLine($"    enabled: {scope.Enabled.ToString().ToLower()}");
                sb.AppendLine();

                // Write exclusions
                foreach (var blockEx in scope.BlockExclusions.Values)
                {
                    sb.AppendLine($"    block_exclusion {blockEx.Id} {{");
                    sb.AppendLine($"        checksum: {blockEx.Checksum}");
                    sb.AppendLine($"        content: \"{blockEx.Content}\"");
                    sb.AppendLine($"        comment: \"{blockEx.Comment}\"");
                    sb.AppendLine("    }");
                }

                foreach (var toggleEx in scope.ToggleExclusions.Values)
                {
                    sb.AppendLine("    toggle_exclusion {");
                    sb.AppendLine($"        direction: {toggleEx.Direction.ToString().ToLower()}");
                    sb.AppendLine($"        signal: {toggleEx.SignalName}");
                    if (toggleEx.BitIndex.HasValue)
                        sb.AppendLine($"        bit_index: {toggleEx.BitIndex}");
                    sb.AppendLine($"        hierarchy: {toggleEx.HierarchyPath}");
                    sb.AppendLine($"        comment: \"{toggleEx.Comment}\"");
                    sb.AppendLine("    }");
                }

                sb.AppendLine("}");
                sb.AppendLine();
            }

            return sb.ToString();
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (!disposed)
            {
                if (disposing)
                {
                    Close();
                }
                disposed = true;
            }
        }
    }

    /// <summary>
    /// Simulates ExclusionUtilities static class
    /// </summary>
    public static class ExclusionUtilities
    {
        public static string Version => "2.0.0-Simulator";

        public static bool IsValidExclusionFile(string fileName)
        {
            if (string.IsNullOrEmpty(fileName) || !File.Exists(fileName))
                return false;

            try
            {
                using var parser = new ExclusionParserNet();
                return parser.ValidateFile(fileName);
            }
            catch
            {
                return false;
            }
        }

        public static ExclusionData CreateSampleData()
        {
            var data = new ExclusionData("sample.el");

            // Create sample scope
            var scope = new ExclusionScope("test_module", "sample_checksum", true);

            // Add sample exclusions
            var blockEx = new BlockExclusion("block_001", "block_checksum",
                                           "always @(posedge clk) begin", "Sample block exclusion");
            scope.AddBlockExclusion(blockEx);

            var toggleEx = new ToggleExclusion(ToggleDirection.Both, "test_signal",
                                             null, "test_path", "Sample toggle exclusion");
            scope.AddToggleExclusion(toggleEx);

            data.AddScope(scope);
            return data;
        }
    }
}