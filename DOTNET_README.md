# ExclusionParser .NET Assembly

## Overview

The ExclusionParser .NET Assembly provides managed .NET interoperability for the ExclusionParser native C++ library. This assembly enables seamless integration with C#, VB.NET, F#, and other .NET languages while maintaining the performance and functionality of the native implementation.

## Features

- **Managed Data Structures**: Full .NET-compatible data models with property support
- **LINQ Compatibility**: Support for LINQ queries and lambda expressions
- **Garbage Collection**: Automatic memory management through .NET garbage collector
- **Exception Handling**: .NET-style exception handling with typed exceptions
- **Interoperability**: Seamless conversion between native C++ and managed .NET types
- **Performance**: Optimized C++/CLI bridge minimizes marshaling overhead

## Architecture

### Core Components

1. **Managed Data Types** (`ExclusionTypes.NET.h`)
   - `BlockExclusion` - Represents block-level coverage exclusions
   - `ToggleExclusion` - Represents signal toggle exclusions
   - `FsmExclusion` - Represents finite state machine exclusions
   - `ConditionExclusion` - Represents conditional coverage exclusions

2. **Data Containers** (`ExclusionData.NET.h`)
   - `ExclusionScope` - Groups exclusions within a hierarchical scope
   - `ExclusionData` - Top-level container for all exclusion information

3. **Parser and Writer** (`ExclusionParser.NET.h`)
   - `ExclusionParserNet` - Managed wrapper for parsing exclusion files
   - `ExclusionWriterNet` - Managed wrapper for writing exclusion data
   - `ExclusionUtilities` - Static utility methods and helpers

4. **C++/CLI Bridge** (`ExclusionParser.NET.cpp`)
   - Type conversion functions between native and managed types
   - Memory management and resource cleanup
   - Exception translation and error handling

## Installation

### Prerequisites

- .NET Framework 4.8 or .NET 8.0+
- Visual Studio 2022 with C++/CLI support
- ExclusionParser native C++ library (ExclusionParser.dll)

### Building from Source

```batch
# Build native library first
build_dll.bat

# Build .NET assembly
build-dotnet.bat
```

### NuGet Package

```xml
<PackageReference Include="AMD.ExclusionParser.NET" Version="2.0.0" />
```

## Usage Examples

### C# Example

```csharp
using ExclusionParser.Managed;

// Parse an exclusion file
using (var parser = new ExclusionParserNet())
{
    var data = parser.ParseFile("exclusions.el");
    
    Console.WriteLine($"Parsed {data.ScopeCount} scopes");
    Console.WriteLine($"Total exclusions: {data.TotalExclusions}");
    
    // Use LINQ to analyze data
    var enabledScopes = data.Scopes.Values
        .Where(s => s.Enabled)
        .ToList();
    
    var blockExclusions = data.GetExclusionsByType(ExclusionType.Block);
    Console.WriteLine($"Block exclusions: {blockExclusions.Count()}");
}

// Create exclusion data programmatically
var newData = new ExclusionData("new_exclusions.el");
var scope = new ExclusionScope("cpu_core", "checksum123", true);

var blockEx = new BlockExclusion(
    "hazard_detection",
    "block_hash",
    "always @(posedge clk) begin if (stall) ...",
    "Pipeline hazard detection excluded during reset"
);
scope.AddBlockExclusion(blockEx);

newData.AddScope(scope);

// Write to file
using (var writer = new ExclusionWriterNet())
{
    writer.Open("output.el");
    writer.WriteHeader("My Tool", "1.0", "verification");
    writer.WriteData(newData);
    writer.Close();
}
```

### VB.NET Example

```vb.net
Imports ExclusionParser.Managed

' Parse exclusion file
Using parser As New ExclusionParserNet()
    Dim data As ExclusionData = parser.ParseFile("exclusions.el")
    
    Console.WriteLine($"Parsed {data.ScopeCount} scopes")
    Console.WriteLine($"Total exclusions: {data.TotalExclusions}")
    
    ' Use LINQ with VB.NET syntax
    Dim enabledScopes = From scope In data.Scopes.Values
                       Where scope.Enabled
                       Select scope.Name
    
    Console.WriteLine($"Enabled scopes: {String.Join(", ", enabledScopes)}")
End Using

' Create new exclusion data
Dim newData As New ExclusionData("vb_exclusions.el")
Dim scope As New ExclusionScope("gpu_core", "vb_checksum", True)

Dim toggleEx As New ToggleExclusion(
    ToggleDirection.Both,
    "enable_signal",
    Nothing,
    "gpu_core.control",
    "Enable signal toggle excluded"
)
scope.AddToggleExclusion(toggleEx)

newData.AddScope(scope)
```

## API Reference

### ExclusionParserNet Class

#### Methods

- `ExclusionData ParseFile(string fileName)` - Parse exclusion file
- `ExclusionData ParseString(string content)` - Parse exclusion content from string
- `bool ValidateFile(string fileName)` - Validate file format
- `Dictionary<string, object> GetStatistics()` - Get parsing statistics

#### Properties

- `int ErrorCount` - Number of parsing errors
- `string LastError` - Last error message
- `ExclusionData Data` - Parsed data

### ExclusionWriterNet Class

#### Methods

- `bool Open(string fileName)` - Open output file
- `void Close()` - Close output file
- `void WriteHeader(string generatedBy, string version, string mode)` - Write file header
- `void WriteData(ExclusionData data)` - Write exclusion data
- `string WriteToString(ExclusionData data)` - Serialize to string

### ExclusionData Class

#### Properties

- `string FileName` - Source file name
- `int ScopeCount` - Number of scopes
- `int TotalExclusions` - Total exclusion count
- `Dictionary<string, ExclusionScope> Scopes` - Scope collection
- `Dictionary<string, string> Metadata` - File metadata

#### Methods

- `void AddScope(ExclusionScope scope)` - Add scope
- `void SetMetadata(string key, string value)` - Set metadata
- `string GetMetadata(string key)` - Get metadata value
- `IEnumerable<object> GetExclusionsByType(ExclusionType type)` - Filter by type

### ExclusionScope Class

#### Properties

- `string Name` - Scope name
- `string Checksum` - Scope checksum
- `bool Enabled` - Whether scope is enabled
- `Dictionary<string, BlockExclusion> BlockExclusions` - Block exclusions
- `Dictionary<string, ToggleExclusion> ToggleExclusions` - Toggle exclusions
- `Dictionary<string, FsmExclusion> FsmExclusions` - FSM exclusions
- `Dictionary<string, ConditionExclusion> ConditionExclusions` - Condition exclusions

#### Methods

- `void AddBlockExclusion(BlockExclusion exclusion)` - Add block exclusion
- `void AddToggleExclusion(ToggleExclusion exclusion)` - Add toggle exclusion
- `void AddFsmExclusion(FsmExclusion exclusion)` - Add FSM exclusion
- `void AddConditionExclusion(ConditionExclusion exclusion)` - Add condition exclusion
- `int GetTotalExclusionCount()` - Get total exclusions in scope
- `IEnumerable<object> GetAllExclusions()` - Get all exclusions

### Exclusion Types

#### BlockExclusion

```csharp
public ref class BlockExclusion
{
    property String^ Id;
    property String^ Checksum;
    property String^ Content;
    property String^ Comment;
    
    BlockExclusion(String^ id, String^ checksum, String^ content, String^ comment);
}
```

#### ToggleExclusion

```csharp
public ref class ToggleExclusion  
{
    property ToggleDirection Direction;
    property String^ SignalName;
    property Nullable<int> BitIndex;
    property String^ HierarchyPath;
    property String^ Comment;
    
    ToggleExclusion(ToggleDirection direction, String^ signalName, 
                   Nullable<int> bitIndex, String^ hierarchyPath, String^ comment);
}
```

#### FsmExclusion

```csharp
public ref class FsmExclusion
{
    property String^ FsmName;
    property String^ StateName;
    property String^ TransitionName;
    property String^ HierarchyPath;
    property String^ Comment;
    
    FsmExclusion(String^ fsmName, String^ stateName, String^ transitionName,
                String^ hierarchyPath, String^ comment);
}
```

#### ConditionExclusion

```csharp
public ref class ConditionExclusion
{
    property String^ ConditionText;
    property String^ HierarchyPath;
    property String^ Comment;
    
    ConditionExclusion(String^ conditionText, String^ hierarchyPath, String^ comment);
}
```

### Enumerations

#### ExclusionType

```csharp
public enum class ExclusionType
{
    Block,
    Toggle,
    FSM,
    Condition
};
```

#### ToggleDirection

```csharp
public enum class ToggleDirection
{
    High,
    Low,
    Both
};
```

## Error Handling

The assembly uses standard .NET exception handling patterns:

```csharp
try
{
    using (var parser = new ExclusionParserNet())
    {
        var data = parser.ParseFile("exclusions.el");
        // Process data...
    }
}
catch (FileNotFoundException ex)
{
    Console.WriteLine($"File not found: {ex.Message}");
}
catch (ExclusionParserException ex)
{
    Console.WriteLine($"Parsing error: {ex.Message}");
}
catch (ArgumentNullException ex)
{
    Console.WriteLine($"Invalid argument: {ex.ParamName}");
}
```

### Exception Types

- `ExclusionParserException` - General parsing and processing errors
- `ArgumentNullException` - Null parameter errors
- `FileNotFoundException` - File access errors
- `IOException` - I/O operation errors

## Performance Considerations

### Memory Management

- All managed objects are handled by .NET garbage collector
- Native resources are automatically cleaned up via finalizers
- Use `using` statements for deterministic resource disposal

### Best Practices

```csharp
// ✓ Good: Use using statements
using (var parser = new ExclusionParserNet())
{
    var data = parser.ParseFile("file.el");
    // Process data
} // Automatic cleanup

// ✓ Good: Batch operations
var data = new ExclusionData("batch.el");
var scope = new ExclusionScope("module", "checksum", true);
// Add multiple exclusions to scope
scope.AddBlockExclusion(ex1);
scope.AddBlockExclusion(ex2);
scope.AddToggleExclusion(ex3);
data.AddScope(scope); // Add scope once

// ✓ Good: Use LINQ efficiently
var results = data.Scopes.Values
    .Where(s => s.Enabled)
    .SelectMany(s => s.BlockExclusions.Values)
    .Where(b => b.Comment.Contains("keyword"))
    .ToList(); // Materialize once
```

### Performance Metrics

Typical performance characteristics:

- **Parsing**: ~1-5 MB/s depending on file complexity
- **Writing**: ~2-10 MB/s for serialization
- **Memory**: ~10-50 bytes per exclusion object overhead
- **LINQ**: Near-native performance for simple queries

## Integration Examples

### ASP.NET Core Web API

```csharp
[ApiController]
[Route("api/[controller]")]
public class ExclusionController : ControllerBase
{
    [HttpPost("parse")]
    public IActionResult ParseExclusions(IFormFile file)
    {
        if (file == null || file.Length == 0)
            return BadRequest("No file provided");
            
        try
        {
            using var stream = file.OpenReadStream();
            using var reader = new StreamReader(stream);
            var content = reader.ReadToEnd();
            
            using var parser = new ExclusionParserNet();
            var data = parser.ParseString(content);
            
            var result = new
            {
                FileName = data.FileName,
                ScopeCount = data.ScopeCount,
                TotalExclusions = data.TotalExclusions,
                Statistics = parser.GetStatistics()
            };
            
            return Ok(result);
        }
        catch (ExclusionParserException ex)
        {
            return BadRequest($"Parsing error: {ex.Message}");
        }
    }
}
```

### Windows Forms Application

```csharp
public partial class ExclusionViewer : Form
{
    private ExclusionData currentData;
    
    private void LoadFileButton_Click(object sender, EventArgs e)
    {
        using var dialog = new OpenFileDialog();
        dialog.Filter = "Exclusion Files (*.el)|*.el|All Files (*.*)|*.*";
        
        if (dialog.ShowDialog() == DialogResult.OK)
        {
            try
            {
                using var parser = new ExclusionParserNet();
                currentData = parser.ParseFile(dialog.FileName);
                
                // Populate UI controls
                ScopeListBox.DataSource = currentData.Scopes.Values.ToList();
                ScopeListBox.DisplayMember = "Name";
                
                StatisticsLabel.Text = $"Scopes: {currentData.ScopeCount}, " +
                                     $"Exclusions: {currentData.TotalExclusions}";
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error loading file: {ex.Message}", "Error");
            }
        }
    }
}
```

### PowerShell Integration

```powershell
# Load the assembly
Add-Type -Path "ExclusionParser.NET.dll"

# Parse exclusion file
$parser = New-Object ExclusionParser.Managed.ExclusionParserNet
try {
    $data = $parser.ParseFile("exclusions.el")
    
    Write-Host "Parsed $($data.ScopeCount) scopes"
    Write-Host "Total exclusions: $($data.TotalExclusions)"
    
    # Get statistics
    $stats = $parser.GetStatistics()
    $stats.GetEnumerator() | ForEach-Object {
        Write-Host "$($_.Key): $($_.Value)"
    }
}
finally {
    $parser.Dispose()
}
```

## Building and Deployment

### Build Requirements

- Visual Studio 2022 (17.0 or later)
- Windows SDK 10.0.22621.0 or later
- .NET Framework 4.8 or .NET 8.0+
- C++/CLI compiler support

### Build Configuration

The project supports both Debug and Release configurations:

- **Debug**: Full debug symbols, no optimization, detailed error reporting
- **Release**: Optimized code, minimal debug info, production-ready

### Deployment

Include the following files in your deployment:

1. `ExclusionParser.NET.dll` - Managed assembly
2. `ExclusionParser.dll` - Native dependency
3. `ExclusionParser.NET.pdb` - Debug symbols (optional)

### Redistribution

The assembly can be redistributed as:

- **NuGet Package**: `AMD.ExclusionParser.NET`
- **Xcopy Deployment**: Copy DLLs to target directory
- **Global Assembly Cache**: Install using `gacutil` (optional)

## Testing

The assembly includes comprehensive unit tests:

```batch
# Run tests
dotnet test tests\ExclusionParser.NET.Tests.csproj
```

Test coverage includes:

- ✅ Basic parsing and writing operations
- ✅ Error handling and validation
- ✅ LINQ query operations
- ✅ Round-trip data integrity
- ✅ Performance benchmarks
- ✅ Memory leak detection
- ✅ Multi-threading safety

## Troubleshooting

### Common Issues

1. **Assembly not found**
   - Ensure `ExclusionParser.dll` is in the same directory
   - Check target framework compatibility

2. **Parsing errors**
   - Verify file encoding (UTF-8 recommended)
   - Check file format and syntax

3. **Performance issues**
   - Use `using` statements for resource cleanup
   - Avoid frequent small operations, batch when possible

4. **Memory issues**
   - Monitor garbage collection behavior
   - Dispose of large data structures explicitly

### Debug Mode

Enable detailed logging in debug builds:

```csharp
#if DEBUG
    parser.EnableVerboseLogging = true;
#endif
```

## Version History

### Version 2.0.0 (Current)
- Complete .NET assembly with C++/CLI wrappers
- Full LINQ compatibility and .NET integration
- Comprehensive unit test suite
- Performance optimizations
- Enhanced error handling

### Version 1.0.0
- Native C++ library with dynamic loading support
- Basic parsing and writing functionality
- Console-based testing framework

## License

Copyright © 2025 Advanced Micro Devices Inc. All rights reserved.

This software is provided under the MIT License. See LICENSE file for details.

## Support

For technical support and questions:

- **Issues**: Report bugs and feature requests on GitHub
- **Documentation**: Additional examples and tutorials available online
- **Contact**: AMD Verification Team

---

*Generated by ExclusionParser .NET Assembly v2.0.0*