/**
 * @file ExclusionParser.NET.h
 * @brief Managed .NET wrapper classes for Hardware Coverage Exclusion Parser
 * 
 * This file contains C++/CLI managed wrapper classes that provide .NET interop
 * for the ExclusionParser native C++ library. These wrappers enable seamless
 * integration with C#, VB.NET, and other .NET languages while maintaining
 * the performance and functionality of the native implementation.
 * 
 * @author AMD Advanced Micro Devices Inc. - Verification Team
 * @version 2.0.0
 * @date September 13, 2025
 * @copyright (c) 2025 Advanced Micro Devices Inc. All rights reserved.
 */

#pragma once
#include "ExclusionData.NET.h"
#include "ExclusionParser.h"
#include "ExclusionWriter.h"
#include "ExclusionData.h"
#include <msclr/marshal_cppstd.h>
#include <msclr/auto_gcroot.h>

using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;
using namespace msclr::interop;

namespace ExclusionParser {
namespace Managed {

    /// <summary>
    /// Managed exception class for ExclusionParser .NET operations
    /// </summary>
    public ref class ExclusionParserException : public Exception
    {
    public:
        ExclusionParserException() : Exception() {}
        ExclusionParserException(String^ message) : Exception(message) {}
        ExclusionParserException(String^ message, Exception^ innerException) 
            : Exception(message, innerException) {}
    };

    /// <summary>
    /// Managed wrapper for the native ExclusionParser class.
    /// Provides .NET-compatible interface for parsing hardware verification exclusion files.
    /// </summary>
    public ref class ExclusionParserNet
    {
    private:
        std::unique_ptr<ExclusionParser::ExclusionParser>* m_nativeParser;
        ExclusionData^ m_managedData;

        // Helper method to convert native exclusion data to managed
        ExclusionData^ ConvertToManaged(const ExclusionParser::ExclusionData& nativeData);
        
        // Helper method to convert managed exclusion data to native
        std::unique_ptr<ExclusionParser::ExclusionData> ConvertToNative(ExclusionData^ managedData);

    public:
        /// <summary>
        /// Initializes a new instance of the ExclusionParserNet class
        /// </summary>
        ExclusionParserNet()
        {
            m_nativeParser = new std::unique_ptr<ExclusionParser::ExclusionParser>(
                std::make_unique<ExclusionParser::ExclusionParser>());
            m_managedData = gcnew ExclusionData();
        }

        /// <summary>
        /// Finalizer for proper cleanup of native resources
        /// </summary>
        ~ExclusionParserNet()
        {
            this->!ExclusionParserNet();
        }

        /// <summary>
        /// Destructor for cleanup of native resources
        /// </summary>
        !ExclusionParserNet()
        {
            if (m_nativeParser)
            {
                delete m_nativeParser;
                m_nativeParser = nullptr;
            }
        }

        /// <summary>
        /// Parses an exclusion file and returns the managed data structure
        /// </summary>
        /// <param name="fileName">Path to the exclusion file</param>
        /// <returns>Parsed exclusion data</returns>
        /// <exception cref="ExclusionParserException">Thrown when parsing fails</exception>
        ExclusionData^ ParseFile(String^ fileName)
        {
            if (String::IsNullOrEmpty(fileName))
                throw gcnew ArgumentNullException("fileName");

            if (!File::Exists(fileName))
                throw gcnew FileNotFoundException("Exclusion file not found", fileName);

            try
            {
                std::string nativeFileName = marshal_as<std::string>(fileName);
                
                if (!(*m_nativeParser)->parseFile(nativeFileName))
                {
                    String^ error = gcnew String((*m_nativeParser)->getLastError().c_str());
                    throw gcnew ExclusionParserException("Failed to parse file: " + error);
                }

                // Convert the parsed data to managed format
                const auto& nativeData = (*m_nativeParser)->getData();
                m_managedData = ConvertToManaged(nativeData);
                m_managedData->FileName = fileName;

                return m_managedData;
            }
            catch (const std::exception& e)
            {
                String^ error = gcnew String(e.what());
                throw gcnew ExclusionParserException("Native parsing error: " + error);
            }
        }

        /// <summary>
        /// Parses exclusion data from a string
        /// </summary>
        /// <param name="content">Exclusion file content as string</param>
        /// <returns>Parsed exclusion data</returns>
        /// <exception cref="ExclusionParserException">Thrown when parsing fails</exception>
        ExclusionData^ ParseString(String^ content)
        {
            if (String::IsNullOrEmpty(content))
                throw gcnew ArgumentNullException("content");

            try
            {
                std::string nativeContent = marshal_as<std::string>(content);
                
                if (!(*m_nativeParser)->parseString(nativeContent))
                {
                    String^ error = gcnew String((*m_nativeParser)->getLastError().c_str());
                    throw gcnew ExclusionParserException("Failed to parse string: " + error);
                }

                // Convert the parsed data to managed format
                const auto& nativeData = (*m_nativeParser)->getData();
                m_managedData = ConvertToManaged(nativeData);

                return m_managedData;
            }
            catch (const std::exception& e)
            {
                String^ error = gcnew String(e.what());
                throw gcnew ExclusionParserException("Native parsing error: " + error);
            }
        }

        /// <summary>
        /// Gets the number of parsing errors encountered
        /// </summary>
        property int ErrorCount
        {
            int get()
            {
                return (*m_nativeParser)->getErrorCount();
            }
        }

        /// <summary>
        /// Gets the last error message from the parser
        /// </summary>
        property String^ LastError
        {
            String^ get()
            {
                std::string nativeError = (*m_nativeParser)->getLastError();
                return gcnew String(nativeError.c_str());
            }
        }

        /// <summary>
        /// Gets the parsed exclusion data
        /// </summary>
        property ExclusionData^ Data
        {
            ExclusionData^ get() { return m_managedData; }
        }

        /// <summary>
        /// Validates the syntax and structure of an exclusion file
        /// </summary>
        /// <param name="fileName">Path to the exclusion file</param>
        /// <returns>True if file is valid, false otherwise</returns>
        bool ValidateFile(String^ fileName)
        {
            if (String::IsNullOrEmpty(fileName) || !File::Exists(fileName))
                return false;

            try
            {
                std::string nativeFileName = marshal_as<std::string>(fileName);
                return (*m_nativeParser)->validateFile(nativeFileName);
            }
            catch (...)
            {
                return false;
            }
        }

        /// <summary>
        /// Gets parsing statistics for the last parsed file
        /// </summary>
        /// <returns>Dictionary containing parsing statistics</returns>
        Dictionary<String^, Object^>^ GetStatistics()
        {
            auto stats = gcnew Dictionary<String^, Object^>();
            
            if (m_managedData != nullptr)
            {
                stats["TotalScopes"] = m_managedData->ScopeCount;
                stats["TotalExclusions"] = m_managedData->TotalExclusions;
                stats["BlockExclusions"] = m_managedData->GetExclusionsByType(ExclusionType::Block)->Count;
                stats["ToggleExclusions"] = m_managedData->GetExclusionsByType(ExclusionType::Toggle)->Count;
                stats["FsmExclusions"] = m_managedData->GetExclusionsByType(ExclusionType::FSM)->Count;
                stats["ConditionExclusions"] = m_managedData->GetExclusionsByType(ExclusionType::Condition)->Count;
                stats["ErrorCount"] = ErrorCount;
                stats["LastError"] = LastError;
            }

            return stats;
        }
    };

    /// <summary>
    /// Managed wrapper for the native ExclusionWriter class.
    /// Provides .NET-compatible interface for writing hardware verification exclusion files.
    /// </summary>
    public ref class ExclusionWriterNet
    {
    private:
        std::unique_ptr<ExclusionParser::ExclusionWriter>* m_nativeWriter;

    public:
        /// <summary>
        /// Initializes a new instance of the ExclusionWriterNet class
        /// </summary>
        ExclusionWriterNet()
        {
            m_nativeWriter = new std::unique_ptr<ExclusionParser::ExclusionWriter>(
                std::make_unique<ExclusionParser::ExclusionWriter>());
        }

        /// <summary>
        /// Finalizer for proper cleanup of native resources
        /// </summary>
        ~ExclusionWriterNet()
        {
            this->!ExclusionWriterNet();
        }

        /// <summary>
        /// Destructor for cleanup of native resources
        /// </summary>
        !ExclusionWriterNet()
        {
            if (m_nativeWriter)
            {
                delete m_nativeWriter;
                m_nativeWriter = nullptr;
            }
        }

        /// <summary>
        /// Opens an output file for writing exclusion data
        /// </summary>
        /// <param name="fileName">Path to the output file</param>
        /// <returns>True if file opened successfully</returns>
        /// <exception cref="ExclusionParserException">Thrown when file cannot be opened</exception>
        bool Open(String^ fileName)
        {
            if (String::IsNullOrEmpty(fileName))
                throw gcnew ArgumentNullException("fileName");

            try
            {
                std::string nativeFileName = marshal_as<std::string>(fileName);
                return (*m_nativeWriter)->open(nativeFileName);
            }
            catch (const std::exception& e)
            {
                String^ error = gcnew String(e.what());
                throw gcnew ExclusionParserException("Failed to open file: " + error);
            }
        }

        /// <summary>
        /// Closes the output file
        /// </summary>
        void Close()
        {
            try
            {
                (*m_nativeWriter)->close();
            }
            catch (...)
            {
                // Ignore exceptions during close
            }
        }

        /// <summary>
        /// Writes the header information to the exclusion file
        /// </summary>
        /// <param name="generatedBy">Tool that generated the file</param>
        /// <param name="version">Format version</param>
        /// <param name="mode">Exclusion mode</param>
        void WriteHeader(String^ generatedBy, String^ version, String^ mode)
        {
            try
            {
                std::string nativeGeneratedBy = marshal_as<std::string>(generatedBy ? generatedBy : "ExclusionParser.NET");
                std::string nativeVersion = marshal_as<std::string>(version ? version : "2.0.0");
                std::string nativeMode = marshal_as<std::string>(mode ? mode : "verification");
                
                (*m_nativeWriter)->writeHeader(nativeGeneratedBy, nativeVersion, nativeMode);
            }
            catch (const std::exception& e)
            {
                String^ error = gcnew String(e.what());
                throw gcnew ExclusionParserException("Failed to write header: " + error);
            }
        }

        /// <summary>
        /// Writes exclusion data to the file
        /// </summary>
        /// <param name="data">Exclusion data to write</param>
        /// <exception cref="ExclusionParserException">Thrown when writing fails</exception>
        void WriteData(ExclusionData^ data)
        {
            if (data == nullptr)
                throw gcnew ArgumentNullException("data");

            try
            {
                // Convert managed data to native format and write
                auto nativeData = ConvertToNative(data);
                (*m_nativeWriter)->writeData(*nativeData);
            }
            catch (const std::exception& e)
            {
                String^ error = gcnew String(e.what());
                throw gcnew ExclusionParserException("Failed to write data: " + error);
            }
        }

        /// <summary>
        /// Writes exclusion data to a string
        /// </summary>
        /// <param name="data">Exclusion data to write</param>
        /// <returns>String representation of the exclusion data</returns>
        String^ WriteToString(ExclusionData^ data)
        {
            if (data == nullptr)
                throw gcnew ArgumentNullException("data");

            try
            {
                auto nativeData = ConvertToNative(data);
                std::string result = (*m_nativeWriter)->writeToString(*nativeData);
                return gcnew String(result.c_str());
            }
            catch (const std::exception& e)
            {
                String^ error = gcnew String(e.what());
                throw gcnew ExclusionParserException("Failed to write to string: " + error);
            }
        }

    private:
        // Helper method to convert managed exclusion data to native
        std::unique_ptr<ExclusionParser::ExclusionData> ConvertToNative(ExclusionData^ managedData);
    };

    /// <summary>
    /// Managed utility class for ExclusionParser .NET operations
    /// </summary>
    public ref class ExclusionUtilities abstract sealed
    {
    public:
        /// <summary>
        /// Gets the version of the ExclusionParser .NET assembly
        /// </summary>
        static property String^ Version
        {
            String^ get() { return "2.0.0"; }
        }

        /// <summary>
        /// Validates the format of an exclusion file without full parsing
        /// </summary>
        /// <param name="fileName">Path to the file to validate</param>
        /// <returns>True if format appears valid</returns>
        static bool IsValidExclusionFile(String^ fileName)
        {
            if (String::IsNullOrEmpty(fileName) || !File::Exists(fileName))
                return false;

            try
            {
                auto parser = gcnew ExclusionParserNet();
                return parser->ValidateFile(fileName);
            }
            catch (...)
            {
                return false;
            }
        }

        /// <summary>
        /// Creates a sample exclusion data structure for testing
        /// </summary>
        /// <returns>Sample exclusion data</returns>
        static ExclusionData^ CreateSampleData()
        {
            auto data = gcnew ExclusionData("sample.el");
            
            // Create a sample scope
            auto scope = gcnew ExclusionScope("test_module", "sample_checksum", true);
            
            // Add sample exclusions
            auto blockEx = gcnew BlockExclusion("block_001", "block_checksum", 
                                              "always @(posedge clk) begin", "Sample block exclusion");
            scope->AddBlockExclusion(blockEx);
            
            auto toggleEx = gcnew ToggleExclusion(ToggleDirection::Both, "test_signal", 
                                                Nullable<int>(), "Test signal", "Sample toggle exclusion");
            scope->AddToggleExclusion(toggleEx);
            
            data->AddScope(scope);
            return data;
        }
    };

} // namespace Managed
} // namespace ExclusionParser