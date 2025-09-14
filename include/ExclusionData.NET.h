/**
 * @file ExclusionData.NET.h
 * @brief Managed .NET data container classes for Hardware Coverage Exclusion Parser
 * 
 * This file contains managed C++/CLI data container classes that provide hierarchical
 * organization of exclusion data compatible with .NET frameworks. These classes
 * enable seamless integration with C#, VB.NET applications and provide LINQ
 * capabilities, serialization support, and property-based access patterns.
 * 
 * @author AMD Advanced Micro Devices Inc. - Verification Team
 * @version 2.0.0
 * @date September 13, 2025
 * @copyright (c) 2025 Advanced Micro Devices Inc. All rights reserved.
 */

#pragma once
#include "ExclusionTypes.NET.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Linq;

namespace ExclusionParser {
namespace Managed {

    /// <summary>
    /// Managed class representing a hierarchical scope for exclusion organization.
    /// Scopes can represent modules, instances, or other hierarchical containers
    /// in hardware verification environments.
    /// </summary>
    public ref class ExclusionScope
    {
    private:
        String^ _scopeName;
        String^ _checksum;
        bool _isModule;
        Dictionary<String^, BlockExclusion^>^ _blockExclusions;
        Dictionary<String^, List<ToggleExclusion^>^>^ _toggleExclusions;
        Dictionary<String^, List<FsmExclusion^>^>^ _fsmExclusions;
        Dictionary<String^, ConditionExclusion^>^ _conditionExclusions;

    public:
        /// <summary>
        /// Initializes a new instance of the ExclusionScope class
        /// </summary>
        ExclusionScope()
        {
            _scopeName = String::Empty;
            _checksum = String::Empty;
            _isModule = false;
            _blockExclusions = gcnew Dictionary<String^, BlockExclusion^>();
            _toggleExclusions = gcnew Dictionary<String^, List<ToggleExclusion^>^>();
            _fsmExclusions = gcnew Dictionary<String^, List<FsmExclusion^>^>();
            _conditionExclusions = gcnew Dictionary<String^, ConditionExclusion^>();
        }

        /// <summary>
        /// Initializes a new instance of the ExclusionScope class with specified parameters
        /// </summary>
        /// <param name="scopeName">Name of the scope (module or instance)</param>
        /// <param name="checksum">Verification checksum for scope integrity</param>
        /// <param name="isModule">True if this represents a module, false for instance</param>
        ExclusionScope(String^ scopeName, String^ checksum, bool isModule)
        {
            _scopeName = scopeName ? scopeName : String::Empty;
            _checksum = checksum ? checksum : String::Empty;
            _isModule = isModule;
            _blockExclusions = gcnew Dictionary<String^, BlockExclusion^>();
            _toggleExclusions = gcnew Dictionary<String^, List<ToggleExclusion^>^>();
            _fsmExclusions = gcnew Dictionary<String^, List<FsmExclusion^>^>();
            _conditionExclusions = gcnew Dictionary<String^, ConditionExclusion^>();
        }

        /// <summary>Gets or sets the scope name</summary>
        property String^ ScopeName
        {
            String^ get() { return _scopeName; }
            void set(String^ value) { _scopeName = value ? value : String::Empty; }
        }

        /// <summary>Gets or sets the verification checksum</summary>
        property String^ Checksum
        {
            String^ get() { return _checksum; }
            void set(String^ value) { _checksum = value ? value : String::Empty; }
        }

        /// <summary>Gets or sets whether this is a module scope</summary>
        property bool IsModule
        {
            bool get() { return _isModule; }
            void set(bool value) { _isModule = value; }
        }

        /// <summary>Gets the collection of block exclusions</summary>
        property Dictionary<String^, BlockExclusion^>^ BlockExclusions
        {
            Dictionary<String^, BlockExclusion^>^ get() { return _blockExclusions; }
        }

        /// <summary>Gets the collection of toggle exclusions</summary>
        property Dictionary<String^, List<ToggleExclusion^>^>^ ToggleExclusions
        {
            Dictionary<String^, List<ToggleExclusion^>^>^ get() { return _toggleExclusions; }
        }

        /// <summary>Gets the collection of FSM exclusions</summary>
        property Dictionary<String^, List<FsmExclusion^>^>^ FsmExclusions
        {
            Dictionary<String^, List<FsmExclusion^>^>^ get() { return _fsmExclusions; }
        }

        /// <summary>Gets the collection of condition exclusions</summary>
        property Dictionary<String^, ConditionExclusion^>^ ConditionExclusions
        {
            Dictionary<String^, ConditionExclusion^>^ get() { return _conditionExclusions; }
        }

        /// <summary>
        /// Adds a block exclusion to this scope
        /// </summary>
        /// <param name="blockExclusion">The block exclusion to add</param>
        void AddBlockExclusion(BlockExclusion^ blockExclusion)
        {
            if (blockExclusion != nullptr && !String::IsNullOrEmpty(blockExclusion->BlockId))
            {
                _blockExclusions[blockExclusion->BlockId] = blockExclusion;
            }
        }

        /// <summary>
        /// Adds a toggle exclusion to this scope
        /// </summary>
        /// <param name="toggleExclusion">The toggle exclusion to add</param>
        void AddToggleExclusion(ToggleExclusion^ toggleExclusion)
        {
            if (toggleExclusion != nullptr && !String::IsNullOrEmpty(toggleExclusion->SignalName))
            {
                if (!_toggleExclusions->ContainsKey(toggleExclusion->SignalName))
                {
                    _toggleExclusions[toggleExclusion->SignalName] = gcnew List<ToggleExclusion^>();
                }
                _toggleExclusions[toggleExclusion->SignalName]->Add(toggleExclusion);
            }
        }

        /// <summary>
        /// Adds an FSM exclusion to this scope
        /// </summary>
        /// <param name="fsmExclusion">The FSM exclusion to add</param>
        void AddFsmExclusion(FsmExclusion^ fsmExclusion)
        {
            if (fsmExclusion != nullptr && !String::IsNullOrEmpty(fsmExclusion->FsmName))
            {
                if (!_fsmExclusions->ContainsKey(fsmExclusion->FsmName))
                {
                    _fsmExclusions[fsmExclusion->FsmName] = gcnew List<FsmExclusion^>();
                }
                _fsmExclusions[fsmExclusion->FsmName]->Add(fsmExclusion);
            }
        }

        /// <summary>
        /// Adds a condition exclusion to this scope
        /// </summary>
        /// <param name="conditionExclusion">The condition exclusion to add</param>
        void AddConditionExclusion(ConditionExclusion^ conditionExclusion)
        {
            if (conditionExclusion != nullptr && !String::IsNullOrEmpty(conditionExclusion->ConditionId))
            {
                _conditionExclusions[conditionExclusion->ConditionId] = conditionExclusion;
            }
        }

        /// <summary>
        /// Gets the total number of exclusions in this scope
        /// </summary>
        property int TotalExclusions
        {
            int get()
            {
                int total = _blockExclusions->Count + _conditionExclusions->Count;
                total += _toggleExclusions->Values->Sum(gcnew Func<List<ToggleExclusion^>^, int>(
                    [](List<ToggleExclusion^>^ list) { return list->Count; }));
                total += _fsmExclusions->Values->Sum(gcnew Func<List<FsmExclusion^>^, int>(
                    [](List<FsmExclusion^>^ list) { return list->Count; }));
                return total;
            }
        }

        /// <summary>
        /// Returns a string representation of the scope
        /// </summary>
        virtual String^ ToString() override
        {
            String^ type = _isModule ? "Module" : "Instance";
            return String::Format("{0}[{1}]: {2} exclusions", type, _scopeName, TotalExclusions);
        }
    };

    /// <summary>
    /// Managed class representing the complete exclusion data container.
    /// This is the top-level container that holds all scopes and exclusions
    /// parsed from hardware verification exclusion files.
    /// </summary>
    public ref class ExclusionData
    {
    private:
        String^ _fileName;
        String^ _generatedBy;
        String^ _formatVersion;
        String^ _generationDate;
        String^ _exclusionMode;
        Dictionary<String^, ExclusionScope^>^ _scopes;

    public:
        /// <summary>
        /// Initializes a new instance of the ExclusionData class
        /// </summary>
        ExclusionData()
        {
            _fileName = String::Empty;
            _generatedBy = "ExclusionParser.NET";
            _formatVersion = "2.0.0";
            _generationDate = DateTime::Now.ToString("yyyy-MM-dd HH:mm:ss");
            _exclusionMode = "verification";
            _scopes = gcnew Dictionary<String^, ExclusionScope^>();
        }

        /// <summary>
        /// Initializes a new instance of the ExclusionData class with filename
        /// </summary>
        /// <param name="fileName">Name of the exclusion file</param>
        ExclusionData(String^ fileName) : ExclusionData()
        {
            _fileName = fileName ? fileName : String::Empty;
        }

        /// <summary>Gets or sets the exclusion file name</summary>
        property String^ FileName
        {
            String^ get() { return _fileName; }
            void set(String^ value) { _fileName = value ? value : String::Empty; }
        }

        /// <summary>Gets or sets the generator identification</summary>
        property String^ GeneratedBy
        {
            String^ get() { return _generatedBy; }
            void set(String^ value) { _generatedBy = value ? value : String::Empty; }
        }

        /// <summary>Gets or sets the format version</summary>
        property String^ FormatVersion
        {
            String^ get() { return _formatVersion; }
            void set(String^ value) { _formatVersion = value ? value : String::Empty; }
        }

        /// <summary>Gets or sets the generation date</summary>
        property String^ GenerationDate
        {
            String^ get() { return _generationDate; }
            void set(String^ value) { _generationDate = value ? value : String::Empty; }
        }

        /// <summary>Gets or sets the exclusion mode</summary>
        property String^ ExclusionMode
        {
            String^ get() { return _exclusionMode; }
            void set(String^ value) { _exclusionMode = value ? value : String::Empty; }
        }

        /// <summary>Gets the collection of scopes</summary>
        property Dictionary<String^, ExclusionScope^>^ Scopes
        {
            Dictionary<String^, ExclusionScope^>^ get() { return _scopes; }
        }

        /// <summary>
        /// Adds a scope to the exclusion data
        /// </summary>
        /// <param name="scope">The scope to add</param>
        void AddScope(ExclusionScope^ scope)
        {
            if (scope != nullptr && !String::IsNullOrEmpty(scope->ScopeName))
            {
                _scopes[scope->ScopeName] = scope;
            }
        }

        /// <summary>
        /// Gets a scope by name
        /// </summary>
        /// <param name="scopeName">Name of the scope to retrieve</param>
        /// <returns>The scope if found, nullptr otherwise</returns>
        ExclusionScope^ GetScope(String^ scopeName)
        {
            if (!String::IsNullOrEmpty(scopeName) && _scopes->ContainsKey(scopeName))
            {
                return _scopes[scopeName];
            }
            return nullptr;
        }

        /// <summary>
        /// Gets all exclusions of a specific type across all scopes
        /// </summary>
        /// <param name="exclusionType">Type of exclusions to retrieve</param>
        /// <returns>List of exclusions of the specified type</returns>
        List<Object^>^ GetExclusionsByType(ExclusionType exclusionType)
        {
            auto result = gcnew List<Object^>();

            for each(auto scope in _scopes->Values)
            {
                switch (exclusionType)
                {
                case ExclusionType::Block:
                    for each(auto block in scope->BlockExclusions->Values)
                        result->Add(block);
                    break;
                case ExclusionType::Toggle:
                    for each(auto toggleList in scope->ToggleExclusions->Values)
                        for each(auto toggle in toggleList)
                            result->Add(toggle);
                    break;
                case ExclusionType::FSM:
                    for each(auto fsmList in scope->FsmExclusions->Values)
                        for each(auto fsm in fsmList)
                            result->Add(fsm);
                    break;
                case ExclusionType::Condition:
                    for each(auto condition in scope->ConditionExclusions->Values)
                        result->Add(condition);
                    break;
                }
            }

            return result;
        }

        /// <summary>
        /// Gets the total number of exclusions across all scopes
        /// </summary>
        property int TotalExclusions
        {
            int get()
            {
                return _scopes->Values->Sum(gcnew Func<ExclusionScope^, int>(
                    [](ExclusionScope^ scope) { return scope->TotalExclusions; }));
            }
        }

        /// <summary>
        /// Gets the total number of scopes
        /// </summary>
        property int ScopeCount
        {
            int get() { return _scopes->Count; }
        }

        /// <summary>
        /// Returns a string representation of the exclusion data
        /// </summary>
        virtual String^ ToString() override
        {
            return String::Format("ExclusionData[{0}]: {1} scopes, {2} exclusions", 
                                _fileName, ScopeCount, TotalExclusions);
        }
    };

} // namespace Managed
} // namespace ExclusionParser