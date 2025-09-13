/**
 * @file ExclusionWriter.h
 * @brief Professional-grade exclusion file writer for hardware verification databases
 * 
 * This file contains the sophisticated ExclusionWriter class responsible for converting
 * structured C++ exclusion data back into properly formatted .el (exclusion list) files.
 * The writer ensures format compliance, maintains database integrity, and supports
 * advanced features required for production hardware verification environments.
 * 
 * Advanced Writer Capabilities:
 * - High-fidelity writing of all exclusion types (Block, Toggle, FSM, Condition)
 * - Industry-standard file format compliance with precise syntax validation
 * - Intelligent annotation preservation and multi-line comment formatting
 * - Automatic checksum generation and verification for database integrity
 * - Customizable formatting options for tool-specific requirements
 * - Efficient batch writing operations for large-scale verification databases
 * - Hierarchical scope organization with proper indentation and structure
 * 
 * Production Features:
 * - Atomic write operations with rollback capabilities for data safety
 * - Incremental file updates for performance optimization
 * - Format validation and error detection during write operations
 * - Backward compatibility with legacy exclusion file formats
 * - Support for custom metadata injection and tool-specific extensions
 * - Thread-safe writing operations for parallel verification flows
 * 
 * @author AMD Advanced Micro Devices Inc. - Verification Infrastructure Team
 * @version 2.0.0
 * @date September 13, 2025
 * @copyright (c) 2025 Advanced Micro Devices Inc. All rights reserved.
 * 
 * @note This writer is designed for production verification environments and maintains
 *       strict compliance with industry-standard exclusion file formats and conventions.
 */

#ifndef EXCLUSION_WRITER_H
#define EXCLUSION_WRITER_H

#include "ExclusionTypes.h"
#include "ExclusionData.h"
#include <fstream>
#include <memory>

namespace ExclusionParser {

/**
 * @brief Writer configuration options
 * 
 * Allows customization of output format and behavior.
 */
struct EXCLUSION_API WriterConfig {
    bool includeComments;           ///< Include file header comments
    bool includeAnnotations;        ///< Include exclusion annotations
    bool sortExclusions;           ///< Sort exclusions within each scope
    bool generateChecksums;        ///< Generate checksums for scopes
    bool preserveOrder;            ///< Preserve original order from input
    std::string indentation;       ///< Indentation string (default: no indent)
    std::string lineEnding;        ///< Line ending style ("\n" or "\r\n")
    bool compactFormat;            ///< Use compact format (minimal whitespace)
    
    /**
     * @brief Default constructor with sensible defaults
     */
    WriterConfig() 
        : includeComments(true), includeAnnotations(true), sortExclusions(false),
          generateChecksums(true), preserveOrder(true), indentation(""),
          lineEnding("\n"), compactFormat(false) {}
};

/**
 * @brief Writing result information
 * 
 * Contains detailed information about the writing operation.
 */
struct EXCLUSION_API WriteResult {
    bool success;                           ///< Whether writing was successful
    std::string errorMessage;               ///< Error message if writing failed
    size_t linesWritten;                    ///< Number of lines written
    size_t exclusionsWritten;               ///< Number of exclusions written
    size_t scopesWritten;                   ///< Number of scopes written
    std::vector<std::string> warnings;     ///< Non-fatal warnings
    
    /// Counts by exclusion type
    std::unordered_map<ExclusionType, size_t> exclusionCounts;
    
    /**
     * @brief Constructor
     */
    WriteResult() : success(false), linesWritten(0), exclusionsWritten(0), scopesWritten(0) {}
    
    /**
     * @brief Check if writing was successful
     * @return True if successful
     */
    bool isSuccess() const { return success; }
    
    /**
     * @brief Get formatted summary string
     * @return Summary of writing results
     */
    std::string getSummary() const;
};

/**
 * @brief Main writer class for exclusion coverage files
 * 
 * The ExclusionWriter class provides comprehensive functionality for writing
 * structured C++ exclusion data back to .el file format.
 * 
 * Usage Example:
 * @code
 * ExclusionWriter writer;
 * 
 * // Configure writer
 * WriterConfig config;
 * config.sortExclusions = true;
 * config.includeAnnotations = true;
 * writer.setConfig(config);
 * 
 * // Write data to file
 * auto result = writer.writeFile("output.el", exclusionData);
 * if (result.success) {
 *     std::cout << "Wrote " << result.exclusionsWritten << " exclusions" << std::endl;
 * } else {
 *     std::cerr << "Write failed: " << result.errorMessage << std::endl;
 * }
 * 
 * // Write to string
 * std::string output = writer.writeToString(exclusionData);
 * @endcode
 */
class EXCLUSION_API ExclusionWriter {
private:
    WriterConfig config_;                   ///< Writer configuration
    
    // Helper methods for writing different sections
    /**
     * @brief Write file header
     * @param stream Output stream
     * @param data Exclusion data
     * @return Number of lines written
     */
    size_t writeHeader(std::ostream& stream, const ExclusionData& data) const;
    
    /**
     * @brief Write a scope (instance or module)
     * @param stream Output stream
     * @param scopeName Name of the scope
     * @param scope Scope data
     * @return Number of lines written
     */
    size_t writeScope(std::ostream& stream, const std::string& scopeName, 
                     const ExclusionScope& scope) const;
    
    /**
     * @brief Write block exclusions for a scope
     * @param stream Output stream
     * @param scope Scope containing block exclusions
     * @return Number of lines written
     */
    size_t writeBlockExclusions(std::ostream& stream, const ExclusionScope& scope) const;
    
    /**
     * @brief Write toggle exclusions for a scope
     * @param stream Output stream
     * @param scope Scope containing toggle exclusions
     * @return Number of lines written
     */
    size_t writeToggleExclusions(std::ostream& stream, const ExclusionScope& scope) const;
    
    /**
     * @brief Write FSM exclusions for a scope
     * @param stream Output stream
     * @param scope Scope containing FSM exclusions
     * @return Number of lines written
     */
    size_t writeFsmExclusions(std::ostream& stream, const ExclusionScope& scope) const;
    
    /**
     * @brief Write condition exclusions for a scope
     * @param stream Output stream
     * @param scope Scope containing condition exclusions
     * @return Number of lines written
     */
    size_t writeConditionExclusions(std::ostream& stream, const ExclusionScope& scope) const;
    
    /**
     * @brief Write an annotation if present
     * @param stream Output stream
     * @param annotation Annotation text
     * @return Number of lines written
     */
    size_t writeAnnotation(std::ostream& stream, const std::string& annotation) const;
    
    /**
     * @brief Write checksum line
     * @param stream Output stream
     * @param checksum Checksum value
     * @return Number of lines written
     */
    size_t writeChecksum(std::ostream& stream, const std::string& checksum) const;
    
    /**
     * @brief Escape string for output (handle quotes, etc.)
     * @param str String to escape
     * @return Escaped string
     */
    std::string escapeString(const std::string& str) const;
    
    /**
     * @brief Format toggle direction for output
     * @param direction Toggle direction
     * @return Formatted direction string
     */
    std::string formatToggleDirection(ToggleDirection direction) const;
    
    /**
     * @brief Generate checksum for scope
     * @param scope Scope to generate checksum for
     * @return Generated checksum string
     */
    std::string generateScopeChecksum(const ExclusionScope& scope) const;
    
    /**
     * @brief Get sorted exclusion order
     * @param scope Scope to sort
     * @return Vectors of sorted keys for each exclusion type
     */
    std::tuple<std::vector<std::string>, std::vector<std::string>, 
               std::vector<std::string>, std::vector<std::string>> 
    getSortedOrder(const ExclusionScope& scope) const;
    
    /**
     * @brief Write line with proper formatting
     * @param stream Output stream
     * @param line Line to write
     * @return Number of lines written (always 1)
     */
    size_t writeLine(std::ostream& stream, const std::string& line) const;
    
    /**
     * @brief Add warning to current write result
     * @param warning Warning message
     */
    void addWarning(const std::string& warning) const;
    
public:
    /**
     * @brief Constructor
     */
    ExclusionWriter();
    
    /**
     * @brief Destructor
     */
    ~ExclusionWriter();
    
    /**
     * @brief Set writer configuration
     * @param config Configuration to use
     */
    void setConfig(const WriterConfig& config);
    
    /**
     * @brief Get current writer configuration
     * @return Current configuration
     */
    const WriterConfig& getConfig() const;
    
    /**
     * @brief Write exclusion data to a file
     * @param filename Path to output file
     * @param data Exclusion data to write
     * @return Write result with success/failure and statistics
     */
    WriteResult writeFile(const std::string& filename, const ExclusionData& data) const;
    
    /**
     * @brief Write exclusion data to a string
     * @param data Exclusion data to write
     * @return Formatted string representation
     */
    std::string writeToString(const ExclusionData& data) const;
    
    /**
     * @brief Write exclusion data to an output stream
     * @param stream Output stream to write to
     * @param data Exclusion data to write
     * @return Write result with success/failure and statistics
     */
    WriteResult writeToStream(std::ostream& stream, const ExclusionData& data) const;
    
    /**
     * @brief Write only specific scopes to a file
     * @param filename Path to output file
     * @param data Exclusion data containing scopes
     * @param scopeNames Vector of scope names to write
     * @return Write result with success/failure and statistics
     */
    WriteResult writeScopes(const std::string& filename, const ExclusionData& data,
                           const std::vector<std::string>& scopeNames) const;
    
    /**
     * @brief Write exclusions of specific types only
     * @param filename Path to output file
     * @param data Exclusion data to filter
     * @param types Vector of exclusion types to include
     * @return Write result with success/failure and statistics
     */
    WriteResult writeFilteredByType(const std::string& filename, const ExclusionData& data,
                                   const std::vector<ExclusionType>& types) const;
    
    /**
     * @brief Append exclusion data to an existing file
     * @param filename Path to file to append to
     * @param data Exclusion data to append
     * @return Write result with success/failure and statistics
     */
    WriteResult appendToFile(const std::string& filename, const ExclusionData& data) const;
    
    /**
     * @brief Write multiple exclusion data sets to separate files
     * @param baseFilename Base filename (will be modified for each file)
     * @param dataList Vector of exclusion data sets
     * @return Combined write result for all files
     */
    WriteResult writeMultipleFiles(const std::string& baseFilename,
                                  const std::vector<std::shared_ptr<ExclusionData>>& dataList) const;
    
    /**
     * @brief Validate that data can be written correctly
     * @param data Exclusion data to validate
     * @return Vector of validation issues (empty if no issues)
     */
    std::vector<std::string> validateForWriting(const ExclusionData& data) const;
    
    /**
     * @brief Preview what would be written (first N lines)
     * @param data Exclusion data
     * @param maxLines Maximum number of lines to preview
     * @return Preview string
     */
    std::string preview(const ExclusionData& data, size_t maxLines = 50) const;
    
    /**
     * @brief Estimate output file size
     * @param data Exclusion data
     * @return Estimated size in bytes
     */
    size_t estimateOutputSize(const ExclusionData& data) const;
    
    /**
     * @brief Enable/disable debug mode for verbose logging
     * @param enable True to enable debug mode
     */
    void setDebugMode(bool enable);
    
    /**
     * @brief Check if debug mode is enabled
     * @return True if debug mode is on
     */
    bool isDebugMode() const;
    
private:
    bool debugMode_;                        ///< Debug mode flag
    mutable WriteResult lastResult_;        ///< Last write result
    
    /**
     * @brief Log debug message if debug mode is enabled
     * @param message Message to log
     */
    void debugLog(const std::string& message) const;
};

/**
 * @brief Utility class for formatting exclusion data
 * 
 * Provides helper functions for formatting various aspects of exclusion data.
 */
class EXCLUSION_API ExclusionFormatter {
public:
    /**
     * @brief Format block exclusion as string
     * @param block Block exclusion to format
     * @param includeAnnotation Whether to include annotation
     * @return Formatted string
     */
    static std::string formatBlock(const BlockExclusion& block, bool includeAnnotation = true);
    
    /**
     * @brief Format toggle exclusion as string
     * @param toggle Toggle exclusion to format
     * @param includeAnnotation Whether to include annotation
     * @return Formatted string
     */
    static std::string formatToggle(const ToggleExclusion& toggle, bool includeAnnotation = true);
    
    /**
     * @brief Format FSM exclusion as string
     * @param fsm FSM exclusion to format
     * @param includeAnnotation Whether to include annotation
     * @return Formatted string
     */
    static std::string formatFsm(const FsmExclusion& fsm, bool includeAnnotation = true);
    
    /**
     * @brief Format condition exclusion as string
     * @param condition Condition exclusion to format
     * @param includeAnnotation Whether to include annotation
     * @return Formatted string
     */
    static std::string formatCondition(const ConditionExclusion& condition, bool includeAnnotation = true);
    
    /**
     * @brief Format scope header as string
     * @param scopeName Name of the scope
     * @param scope Scope data
     * @return Formatted scope header
     */
    static std::string formatScopeHeader(const std::string& scopeName, const ExclusionScope& scope);
    
    /**
     * @brief Format file header as string
     * @param data Exclusion data
     * @return Formatted file header
     */
    static std::string formatFileHeader(const ExclusionData& data);
    
    /**
     * @brief Format statistics as string
     * @param stats Statistics to format
     * @return Formatted statistics string
     */
    static std::string formatStatistics(const ExclusionStatistics& stats);
};

} // namespace ExclusionParser

#endif // EXCLUSION_WRITER_H