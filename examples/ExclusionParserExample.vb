' **
' * @file ExclusionParserExample.vb
' * @brief VB.NET usage examples for the ExclusionParser .NET assembly
' * 
' * This file demonstrates how to use the ExclusionParser .NET assembly
' * from VB.NET for parsing and writing hardware verification exclusion files.
' * The examples show common usage patterns and best practices.
' * 
' * @author AMD Advanced Micro Devices Inc. - Verification Team
' * @version 2.0.0
' * @date September 13, 2025
' * @copyright (c) 2025 Advanced Micro Devices Inc. All rights reserved.
' */

Imports System
Imports System.IO
Imports System.Linq
Imports System.Collections.Generic
Imports ExclusionParser.Managed

Namespace ExclusionParser.Examples

    ''' <summary>
    ''' VB.NET examples demonstrating ExclusionParser .NET assembly usage
    ''' </summary>
    Public Class ExclusionParserVBExamples

        ''' <summary>
        ''' Example 1: Basic file parsing with VB.NET syntax
        ''' </summary>
        Public Shared Sub BasicParsingExample()
            Console.WriteLine("=== VB.NET Basic Parsing Example ===")

            Try
                Using parser As New ExclusionParserNet()
                    ' Parse an exclusion file
                    Dim fileName As String = "sample_exclusions.el"
                    Dim data As ExclusionData = parser.ParseFile(fileName)

                    Console.WriteLine($"File: {data.FileName}")
                    Console.WriteLine($"Scopes: {data.ScopeCount}")
                    Console.WriteLine($"Total Exclusions: {data.TotalExclusions}")

                    ' Access metadata using VB.NET syntax
                    Console.WriteLine("Metadata:")
                    For Each meta In data.Metadata
                        Console.WriteLine($"  {meta.Key}: {meta.Value}")
                    Next

                    ' Iterate through scopes
                    For Each scope In data.Scopes.Values
                        Console.WriteLine($"Scope: {scope.Name} (Enabled: {scope.Enabled})")
                        Console.WriteLine($"  Block exclusions: {scope.BlockExclusions.Count}")
                        Console.WriteLine($"  Toggle exclusions: {scope.ToggleExclusions.Count}")
                        Console.WriteLine($"  FSM exclusions: {scope.FsmExclusions.Count}")
                        Console.WriteLine($"  Condition exclusions: {scope.ConditionExclusions.Count}")
                    Next
                End Using

            Catch ex As FileNotFoundException
                Console.WriteLine("Sample file not found. Creating sample data instead...")
                CreateSampleDataExample()
            Catch ex As ExclusionParserException
                Console.WriteLine($"Parsing error: {ex.Message}")
            Catch ex As Exception
                Console.WriteLine($"Unexpected error: {ex.Message}")
            End Try
        End Sub

        ''' <summary>
        ''' Example 2: Creating exclusion data with VB.NET object initialization
        ''' </summary>
        Public Shared Sub CreateSampleDataExample()
            Console.WriteLine("=== VB.NET Sample Data Creation Example ===")

            ' Create exclusion data structure
            Dim data As New ExclusionData("vb_sample.el")
            data.SetMetadata("generated_by", "ExclusionParser.NET VB.NET Example")
            data.SetMetadata("version", "2.0.0")
            data.SetMetadata("language", "VB.NET")
            data.SetMetadata("creation_date", DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss"))

            ' Create a scope for graphics processing unit
            Dim gpuScope As New ExclusionScope("graphics_processor", "gpu_checksum_vb123", True)

            ' Add block exclusions using VB.NET With statement
            Dim renderBlockEx As New BlockExclusion(
                "vertex_shader_pipeline",
                "vertex_hash_abc",
                "always @(posedge gpu_clk) begin if (vertex_valid) ...",
                "Vertex shader pipeline - certain paths not reachable in test"
            )
            gpuScope.AddBlockExclusion(renderBlockEx)

            Dim pixelBlockEx As New BlockExclusion(
                "pixel_shader_blend",
                "pixel_hash_def",
                "always_comb begin case (blend_mode) ...",
                "Pixel blending modes - alpha blending not tested"
            )
            gpuScope.AddBlockExclusion(pixelBlockEx)

            ' Add toggle exclusions
            Dim vSyncToggle As New ToggleExclusion(
                ToggleDirection.Low,
                "vsync_interrupt",
                Nothing, ' VB.NET null equivalent
                "graphics_processor.display_controller",
                "VSync interrupt - low transition excluded"
            )
            gpuScope.AddToggleExclusion(vSyncToggle)

            Dim memoryBusToggle As New ToggleExclusion(
                ToggleDirection.Both,
                "memory_bus_enable",
                0, ' bit index
                "graphics_processor.memory_interface",
                "Memory bus enable - both transitions excluded during reset"
            )
            gpuScope.AddToggleExclusion(memoryBusToggle)

            ' Add FSM exclusion
            Dim powerFsmEx As New FsmExclusion(
                "gpu_power_management",
                "performance_mode",
                "idle_to_performance",
                "graphics_processor.power_mgmt",
                "Performance mode transition not tested in low-power config"
            )
            gpuScope.AddFsmExclusion(powerFsmEx)

            ' Add condition exclusion
            Dim thermalCondEx As New ConditionExclusion(
                "(thermal_limit == 1'b1) && (emergency_shutdown == 1'b1)",
                "graphics_processor.thermal_mgmt",
                "Emergency thermal shutdown not testable in simulation"
            )
            gpuScope.AddConditionExclusion(thermalCondEx)

            data.AddScope(gpuScope)

            ' Create another scope for display controller
            Dim displayScope As New ExclusionScope("display_controller", "display_checksum_vb456", True)

            Dim hdmiBlockEx As New BlockExclusion(
                "hdmi_transmitter",
                "hdmi_hash_ghi",
                "if (hdmi_active) begin transmit_data(); end",
                "HDMI transmitter - high-resolution modes not tested"
            )
            displayScope.AddBlockExclusion(hdmiBlockEx)

            data.AddScope(displayScope)

            ' Display results using VB.NET string interpolation
            Console.WriteLine($"Created VB.NET exclusion data with {data.ScopeCount} scopes")
            Console.WriteLine($"Total exclusions: {data.TotalExclusions}")

            ' Use LINQ with VB.NET syntax
            Dim enabledScopeCount = data.Scopes.Values.Where(Function(s) s.Enabled).Count()
            Console.WriteLine($"Enabled scopes: {enabledScopeCount}")

            Dim blockExclusionCount = data.GetExclusionsByType(ExclusionType.Block).Count()
            Console.WriteLine($"Block exclusions: {blockExclusionCount}")
        End Sub

        ''' <summary>
        ''' Example 3: File writing with VB.NET error handling
        ''' </summary>
        Public Shared Sub WritingExample()
            Console.WriteLine("=== VB.NET Writing Example ===")

            Try
                ' Create sample data
                Dim data As ExclusionData = ExclusionUtilities.CreateSampleData()

                Using writer As New ExclusionWriterNet()
                    ' Write to string
                    Dim exclusionContent As String = writer.WriteToString(data)
                    Console.WriteLine("Generated exclusion content:")
                    Console.WriteLine(exclusionContent)
                    Console.WriteLine($"Content length: {exclusionContent.Length} characters")

                    ' Write to file
                    Dim outputFile As String = "vb_generated_exclusions.el"
                    If writer.Open(outputFile) Then
                        writer.WriteHeader("ExclusionParser.NET VB.NET Example", "2.0.0", "verification")
                        writer.WriteData(data)
                        writer.Close()

                        Console.WriteLine($"Written to file: {outputFile}")

                        ' Check file information
                        If File.Exists(outputFile) Then
                            Dim fileInfo As New FileInfo(outputFile)
                            Console.WriteLine($"File size: {fileInfo.Length} bytes")
                            Console.WriteLine($"Created: {fileInfo.CreationTime}")
                        End If
                    Else
                        Console.WriteLine("Failed to open output file")
                    End If
                End Using

                ' Verify by parsing the written file
                Using parser As New ExclusionParserNet()
                    If File.Exists("vb_generated_exclusions.el") Then
                        Dim parsedData As ExclusionData = parser.ParseFile("vb_generated_exclusions.el")
                        Console.WriteLine($"Verification: Parsed {parsedData.ScopeCount} scopes from written file")
                    End If
                End Using

            Catch ex As Exception
                Console.WriteLine($"Writing error: {ex.Message}")
            End Try
        End Sub

        ''' <summary>
        ''' Example 4: LINQ queries with VB.NET query syntax
        ''' </summary>
        Public Shared Sub LinqAnalysisExample()
            Console.WriteLine("=== VB.NET LINQ Analysis Example ===")

            ' Create test data
            Dim data As New ExclusionData("vb_analysis.el")

            ' Create multiple scopes
            For i As Integer = 1 To 4
                Dim scope As New ExclusionScope($"vb_module_{i}", $"vb_checksum_{i}", i Mod 2 = 1)

                ' Add exclusions using VB.NET For loop
                For j As Integer = 1 To i * 2
                    Dim blockEx As New BlockExclusion(
                        $"vb_block_{i}_{j}",
                        $"vb_hash_{i}_{j}",
                        $"// VB.NET generated block {i}-{j}",
                        $"VB.NET block exclusion {j} in module {i}"
                    )
                    scope.AddBlockExclusion(blockEx)
                Next

                For j As Integer = 1 To i
                    Dim toggleEx As New ToggleExclusion(
                        CType(j Mod 3, ToggleDirection),
                        $"vb_signal_{i}_{j}",
                        If(j Mod 2 = 0, j, Nothing),
                        $"vb_module_{i}.signal_path",
                        $"VB.NET toggle exclusion {j}"
                    )
                    scope.AddToggleExclusion(toggleEx)
                Next

                data.AddScope(scope)
            Next

            Console.WriteLine($"VB.NET analysis data: {data.ScopeCount} scopes, {data.TotalExclusions} exclusions")

            ' VB.NET LINQ Query Syntax Examples

            ' 1. Query enabled scopes
            Dim enabledScopeNames = From scope In data.Scopes.Values
                                   Where scope.Enabled
                                   Select scope.Name

            Console.WriteLine($"Enabled scopes: {String.Join(", ", enabledScopeNames)}")

            ' 2. Group exclusions by type using VB.NET query syntax
            Dim exclusionCounts = From scope In data.Scopes.Values
                                 From exclusion In scope.GetAllExclusions()
                                 Group exclusion By exclusion.GetType().Name Into grouped = Group
                                 Select New With {
                                     .Type = exclusion.GetType().Name,
                                     .Count = grouped.Count()
                                 }

            Console.WriteLine("Exclusions by type (VB.NET query):")
            For Each item In exclusionCounts
                Console.WriteLine($"  {item.Type}: {item.Count}")
            Next

            ' 3. Find scopes with specific characteristics
            Dim highActivityScopes = From scope In data.Scopes.Values
                                    Where scope.GetTotalExclusionCount() > 2
                                    Order By scope.GetTotalExclusionCount() Descending
                                    Select New With {
                                        .Name = scope.Name,
                                        .Count = scope.GetTotalExclusionCount(),
                                        .Enabled = scope.Enabled
                                    }

            Console.WriteLine("High activity scopes:")
            For Each scopeInfo In highActivityScopes
                Console.WriteLine($"  {scopeInfo.Name}: {scopeInfo.Count} exclusions (Enabled: {scopeInfo.Enabled})")
            Next

            ' 4. Complex LINQ with VB.NET lambda expressions
            Dim togglesWithIndices = data.Scopes.Values _
                                   .SelectMany(Function(s) s.ToggleExclusions.Values) _
                                   .Where(Function(t) t.BitIndex.HasValue) _
                                   .OrderBy(Function(t) t.SignalName) _
                                   .ToList()

            Console.WriteLine($"Toggle exclusions with bit indices: {togglesWithIndices.Count}")
            For Each toggle In togglesWithIndices
                Console.WriteLine($"  {toggle.SignalName}[{toggle.BitIndex}] - {toggle.Direction}")
            Next
        End Sub

        ''' <summary>
        ''' Example 5: VB.NET error handling patterns
        ''' </summary>
        Public Shared Sub ErrorHandlingExample()
            Console.WriteLine("=== VB.NET Error Handling Example ===")

            Using parser As New ExclusionParserNet()
                ' Test various error conditions with VB.NET Try-Catch

                ' File not found
                Try
                    parser.ParseFile("nonexistent_vb_file.el")
                Catch ex As FileNotFoundException
                    Console.WriteLine($"Expected VB.NET error - File not found: {ex.Message}")
                End Try

                ' Null parameter with VB.NET Nothing
                Try
                    parser.ParseFile(Nothing)
                Catch ex As ArgumentNullException
                    Console.WriteLine($"Expected VB.NET error - Nothing parameter: {ex.ParamName}")
                End Try

                ' Invalid content parsing
                Try
                    Dim invalidContent As String = "This is not valid exclusion content in VB.NET!"
                    parser.ParseString(invalidContent)

                    If parser.ErrorCount > 0 Then
                        Console.WriteLine($"VB.NET Parser errors: {parser.ErrorCount}")
                        Console.WriteLine($"Last error: {parser.LastError}")
                    End If
                Catch ex As ExclusionParserException
                    Console.WriteLine($"Expected VB.NET parsing error: {ex.Message}")
                End Try

                ' File validation using VB.NET array
                Dim testFiles() As String = {"valid.el", "invalid.txt", Nothing, ""}
                For Each file In testFiles
                    Dim isValid As Boolean = ExclusionUtilities.IsValidExclusionFile(file)
                    Dim displayFile As String = If(file, "Nothing")
                    Console.WriteLine($"VB.NET File '{displayFile}' is valid: {isValid}")
                Next
            End Using
        End Sub

        ''' <summary>
        ''' Example 6: VB.NET performance monitoring and statistics
        ''' </summary>
        Public Shared Sub PerformanceExample()
            Console.WriteLine("=== VB.NET Performance Example ===")

            Dim stopwatch As New System.Diagnostics.Stopwatch()
            stopwatch.Start()

            ' Create large dataset
            Dim data As New ExclusionData("vb_performance_test.el")
            data.SetMetadata("test_type", "vb_performance")
            data.SetMetadata("language", "VB.NET")

            ' Generate test data using VB.NET For loops
            For i As Integer = 0 To 9
                Dim scope As New ExclusionScope($"vb_perf_module_{i:D3}", $"vb_checksum_{i:D8}", True)

                For j As Integer = 0 To 49
                    Select Case j Mod 4
                        Case 0
                            Dim blockEx As New BlockExclusion(
                                $"vb_block_{i}_{j}",
                                $"vb_hash_{i}_{j}",
                                $"' VB.NET performance test block {i}-{j}",
                                $"VB.NET generated block exclusion {j}"
                            )
                            scope.AddBlockExclusion(blockEx)
                        Case 1
                            Dim toggleEx As New ToggleExclusion(
                                ToggleDirection.Both,
                                $"vb_perf_signal_{j}",
                                If(j Mod 8 = 1, j, Nothing),
                                $"vb_perf_module_{i:D3}",
                                $"VB.NET performance toggle {j}"
                            )
                            scope.AddToggleExclusion(toggleEx)
                    End Select
                Next

                data.AddScope(scope)
            Next

            stopwatch.Stop()
            Console.WriteLine($"VB.NET data creation time: {stopwatch.ElapsedMilliseconds} ms")
            Console.WriteLine($"Created {data.ScopeCount} scopes with {data.TotalExclusions} exclusions")

            ' VB.NET statistics calculation
            stopwatch.Restart()

            Dim stats As New Dictionary(Of String, Object) From {
                {"EnabledScopes", data.Scopes.Values.Count(Function(s) s.Enabled)},
                {"BlockExclusions", data.GetExclusionsByType(ExclusionType.Block).Count()},
                {"ToggleExclusions", data.GetExclusionsByType(ExclusionType.Toggle).Count()},
                {"AverageExclusionsPerScope", data.TotalExclusions / CDbl(data.ScopeCount)}
            }

            stopwatch.Stop()
            Console.WriteLine($"VB.NET statistics calculation time: {stopwatch.ElapsedMilliseconds} ms")

            Console.WriteLine("VB.NET performance statistics:")
            For Each stat In stats
                Console.WriteLine($"  {stat.Key}: {stat.Value}")
            Next

            ' Test serialization
            stopwatch.Restart()
            Using writer As New ExclusionWriterNet()
                Dim serialized As String = writer.WriteToString(data)
                Console.WriteLine($"VB.NET serialization time: {stopwatch.ElapsedMilliseconds} ms")
                Console.WriteLine($"Serialized size: {serialized.Length:N0} characters")
            End Using
        End Sub

        ''' <summary>
        ''' Main entry point for VB.NET examples
        ''' </summary>
        Public Shared Sub Main(args As String())
            Console.WriteLine("ExclusionParser .NET Assembly - VB.NET Examples")
            Console.WriteLine("===============================================")
            Console.WriteLine($"Assembly Version: {ExclusionUtilities.Version}")
            Console.WriteLine()

            Try
                BasicParsingExample()
                Console.WriteLine()

                CreateSampleDataExample()
                Console.WriteLine()

                WritingExample()
                Console.WriteLine()

                LinqAnalysisExample()
                Console.WriteLine()

                ErrorHandlingExample()
                Console.WriteLine()

                PerformanceExample()
                Console.WriteLine()

                Console.WriteLine("All VB.NET examples completed successfully!")

            Catch ex As Exception
                Console.WriteLine($"VB.NET example execution error: {ex.Message}")
                Console.WriteLine($"Stack trace: {ex.StackTrace}")
            End Try

            Console.WriteLine()
            Console.WriteLine("Press any key to exit...")
            Console.ReadKey()
        End Sub

    End Class

End Namespace