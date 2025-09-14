/**
 * @file ExclusionTypes.NET.h
 * @brief Managed .NET data structures for Hardware Coverage Exclusion Parser
 * 
 * This file contains all the C++/CLI managed data structures that provide .NET
 * interop for the ExclusionParser library. These managed types mirror the native
 * C++ structures and enable seamless integration with C#, VB.NET, and other
 * .NET languages.
 * 
 * The managed types provide:
 * - Automatic garbage collection and memory management
 * - .NET Framework/Core compatibility
 * - Serialization support for XML/JSON
 * - LINQ query capabilities
 * - Property-based access patterns
 * - Exception handling with .NET exceptions
 * 
 * @author AMD Advanced Micro Devices Inc. - Verification Team
 * @version 2.0.0
 * @date September 13, 2025
 * @copyright (c) 2025 Advanced Micro Devices Inc. All rights reserved.
 */

#pragma once
#include <msclr/marshal_cppstd.h>

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

namespace ExclusionParser {
namespace Managed {

    /// <summary>
    /// Managed enumeration for toggle direction types in hardware verification
    /// </summary>
    public enum class ToggleDirection : int
    {
        /// <summary>Signal transition from 0 to 1</summary>
        ZeroToOne = 0,
        /// <summary>Signal transition from 1 to 0</summary>
        OneToZero = 1,
        /// <summary>Both 0->1 and 1->0 transitions</summary>
        Both = 2
    };

    /// <summary>
    /// Managed enumeration for exclusion types in coverage analysis
    /// </summary>
    public enum class ExclusionType : int
    {
        /// <summary>Functional block exclusion</summary>
        Block = 0,
        /// <summary>Signal toggle exclusion</summary>
        Toggle = 1,
        /// <summary>Finite state machine exclusion</summary>
        FSM = 2,
        /// <summary>Conditional/branch exclusion</summary>
        Condition = 3
    };

    /// <summary>
    /// Managed class representing a functional block exclusion in hardware verification.
    /// Block exclusions specify Verilog/SystemVerilog code segments that should be
    /// excluded from functional coverage analysis.
    /// </summary>
    public ref class BlockExclusion
    {
    private:
        String^ _blockId;
        String^ _checksum;
        String^ _sourceCode;
        String^ _annotation;

    public:
        /// <summary>
        /// Initializes a new instance of the BlockExclusion class
        /// </summary>
        BlockExclusion()
        {
            _blockId = String::Empty;
            _checksum = String::Empty;
            _sourceCode = String::Empty;
            _annotation = String::Empty;
        }

        /// <summary>
        /// Initializes a new instance of the BlockExclusion class with specified parameters
        /// </summary>
        /// <param name="blockId">Unique identifier for the block exclusion</param>
        /// <param name="checksum">Verification checksum for database integrity</param>
        /// <param name="sourceCode">Verilog/SystemVerilog source code to exclude</param>
        /// <param name="annotation">Optional documentation and reasoning</param>
        BlockExclusion(String^ blockId, String^ checksum, String^ sourceCode, String^ annotation)
        {
            _blockId = blockId ? blockId : String::Empty;
            _checksum = checksum ? checksum : String::Empty;
            _sourceCode = sourceCode ? sourceCode : String::Empty;
            _annotation = annotation ? annotation : String::Empty;
        }

        /// <summary>Gets or sets the unique block identifier</summary>
        property String^ BlockId
        {
            String^ get() { return _blockId; }
            void set(String^ value) { _blockId = value ? value : String::Empty; }
        }

        /// <summary>Gets or sets the verification checksum</summary>
        property String^ Checksum
        {
            String^ get() { return _checksum; }
            void set(String^ value) { _checksum = value ? value : String::Empty; }
        }

        /// <summary>Gets or sets the source code to exclude</summary>
        property String^ SourceCode
        {
            String^ get() { return _sourceCode; }
            void set(String^ value) { _sourceCode = value ? value : String::Empty; }
        }

        /// <summary>Gets or sets the exclusion annotation</summary>
        property String^ Annotation
        {
            String^ get() { return _annotation; }
            void set(String^ value) { _annotation = value ? value : String::Empty; }
        }

        /// <summary>
        /// Returns a string representation of the block exclusion
        /// </summary>
        virtual String^ ToString() override
        {
            return String::Format("BlockExclusion[{0}]: {1}", _blockId, _annotation);
        }
    };

    /// <summary>
    /// Managed class representing a signal toggle exclusion in hardware verification.
    /// Toggle exclusions specify signals that should be excluded from transition coverage analysis.
    /// </summary>
    public ref class ToggleExclusion
    {
    private:
        ToggleDirection _direction;
        String^ _signalName;
        Nullable<int> _bitIndex;
        String^ _description;
        String^ _annotation;

    public:
        /// <summary>
        /// Initializes a new instance of the ToggleExclusion class
        /// </summary>
        ToggleExclusion()
        {
            _direction = ToggleDirection::Both;
            _signalName = String::Empty;
            _bitIndex = Nullable<int>();
            _description = String::Empty;
            _annotation = String::Empty;
        }

        /// <summary>
        /// Initializes a new instance of the ToggleExclusion class with specified parameters
        /// </summary>
        /// <param name="direction">Toggle direction to exclude</param>
        /// <param name="signalName">Name of the signal</param>
        /// <param name="bitIndex">Optional bit index for vector signals</param>
        /// <param name="description">Signal description</param>
        /// <param name="annotation">Optional documentation</param>
        ToggleExclusion(ToggleDirection direction, String^ signalName, Nullable<int> bitIndex, String^ description, String^ annotation)
        {
            _direction = direction;
            _signalName = signalName ? signalName : String::Empty;
            _bitIndex = bitIndex;
            _description = description ? description : String::Empty;
            _annotation = annotation ? annotation : String::Empty;
        }

        /// <summary>Gets or sets the toggle direction</summary>
        property ToggleDirection Direction
        {
            ToggleDirection get() { return _direction; }
            void set(ToggleDirection value) { _direction = value; }
        }

        /// <summary>Gets or sets the signal name</summary>
        property String^ SignalName
        {
            String^ get() { return _signalName; }
            void set(String^ value) { _signalName = value ? value : String::Empty; }
        }

        /// <summary>Gets or sets the bit index for vector signals</summary>
        property Nullable<int> BitIndex
        {
            Nullable<int> get() { return _bitIndex; }
            void set(Nullable<int> value) { _bitIndex = value; }
        }

        /// <summary>Gets or sets the signal description</summary>
        property String^ Description
        {
            String^ get() { return _description; }
            void set(String^ value) { _description = value ? value : String::Empty; }
        }

        /// <summary>Gets or sets the exclusion annotation</summary>
        property String^ Annotation
        {
            String^ get() { return _annotation; }
            void set(String^ value) { _annotation = value ? value : String::Empty; }
        }

        /// <summary>
        /// Returns a string representation of the toggle exclusion
        /// </summary>
        virtual String^ ToString() override
        {
            String^ bitStr = _bitIndex.HasValue ? String::Format("[{0}]", _bitIndex.Value) : "";
            return String::Format("ToggleExclusion[{0}{1}]: {2}", _signalName, bitStr, _annotation);
        }
    };

    /// <summary>
    /// Managed class representing a finite state machine exclusion in hardware verification.
    /// FSM exclusions can exclude either states or state transitions from coverage analysis.
    /// </summary>
    public ref class FsmExclusion
    {
    private:
        String^ _fsmName;
        String^ _fromState;
        String^ _toState;
        String^ _checksum;
        String^ _annotation;
        bool _isTransition;

    public:
        /// <summary>
        /// Initializes a new instance of the FsmExclusion class for state exclusion
        /// </summary>
        /// <param name="fsmName">Name of the finite state machine</param>
        /// <param name="checksum">Verification checksum</param>
        /// <param name="annotation">Optional documentation</param>
        FsmExclusion(String^ fsmName, String^ checksum, String^ annotation)
        {
            _fsmName = fsmName ? fsmName : String::Empty;
            _fromState = String::Empty;
            _toState = String::Empty;
            _checksum = checksum ? checksum : String::Empty;
            _annotation = annotation ? annotation : String::Empty;
            _isTransition = false;
        }

        /// <summary>
        /// Initializes a new instance of the FsmExclusion class for transition exclusion
        /// </summary>
        /// <param name="fsmName">Name of the finite state machine</param>
        /// <param name="fromState">Source state of the transition</param>
        /// <param name="toState">Target state of the transition</param>
        /// <param name="checksum">Verification checksum</param>
        /// <param name="annotation">Optional documentation</param>
        FsmExclusion(String^ fsmName, String^ fromState, String^ toState, String^ checksum, String^ annotation)
        {
            _fsmName = fsmName ? fsmName : String::Empty;
            _fromState = fromState ? fromState : String::Empty;
            _toState = toState ? toState : String::Empty;
            _checksum = checksum ? checksum : String::Empty;
            _annotation = annotation ? annotation : String::Empty;
            _isTransition = true;
        }

        /// <summary>Gets or sets the FSM name</summary>
        property String^ FsmName
        {
            String^ get() { return _fsmName; }
            void set(String^ value) { _fsmName = value ? value : String::Empty; }
        }

        /// <summary>Gets or sets the source state (for transitions)</summary>
        property String^ FromState
        {
            String^ get() { return _fromState; }
            void set(String^ value) { _fromState = value ? value : String::Empty; }
        }

        /// <summary>Gets or sets the target state (for transitions)</summary>
        property String^ ToState
        {
            String^ get() { return _toState; }
            void set(String^ value) { _toState = value ? value : String::Empty; }
        }

        /// <summary>Gets or sets the verification checksum</summary>
        property String^ Checksum
        {
            String^ get() { return _checksum; }
            void set(String^ value) { _checksum = value ? value : String::Empty; }
        }

        /// <summary>Gets or sets the exclusion annotation</summary>
        property String^ Annotation
        {
            String^ get() { return _annotation; }
            void set(String^ value) { _annotation = value ? value : String::Empty; }
        }

        /// <summary>Gets whether this is a transition exclusion</summary>
        property bool IsTransition
        {
            bool get() { return _isTransition; }
        }

        /// <summary>
        /// Returns a string representation of the FSM exclusion
        /// </summary>
        virtual String^ ToString() override
        {
            if (_isTransition)
                return String::Format("FsmTransition[{0}: {1}->{2}]: {3}", _fsmName, _fromState, _toState, _annotation);
            else
                return String::Format("FsmState[{0}]: {1}", _fsmName, _annotation);
        }
    };

    /// <summary>
    /// Managed class representing a conditional/branch exclusion in hardware verification.
    /// Condition exclusions specify Boolean expressions that should be excluded from
    /// conditional coverage analysis.
    /// </summary>
    public ref class ConditionExclusion
    {
    private:
        String^ _conditionId;
        String^ _checksum;
        String^ _expression;
        String^ _parameters;
        String^ _coverage;
        String^ _annotation;

    public:
        /// <summary>
        /// Initializes a new instance of the ConditionExclusion class
        /// </summary>
        ConditionExclusion()
        {
            _conditionId = String::Empty;
            _checksum = String::Empty;
            _expression = String::Empty;
            _parameters = String::Empty;
            _coverage = String::Empty;
            _annotation = String::Empty;
        }

        /// <summary>
        /// Initializes a new instance of the ConditionExclusion class with specified parameters
        /// </summary>
        /// <param name="conditionId">Unique identifier for the condition</param>
        /// <param name="checksum">Verification checksum</param>
        /// <param name="expression">Boolean expression to exclude</param>
        /// <param name="parameters">Coverage analysis parameters</param>
        /// <param name="coverage">Coverage type (branch, condition, etc.)</param>
        /// <param name="annotation">Optional documentation</param>
        ConditionExclusion(String^ conditionId, String^ checksum, String^ expression, 
                          String^ parameters, String^ coverage, String^ annotation)
        {
            _conditionId = conditionId ? conditionId : String::Empty;
            _checksum = checksum ? checksum : String::Empty;
            _expression = expression ? expression : String::Empty;
            _parameters = parameters ? parameters : String::Empty;
            _coverage = coverage ? coverage : String::Empty;
            _annotation = annotation ? annotation : String::Empty;
        }

        /// <summary>Gets or sets the condition identifier</summary>
        property String^ ConditionId
        {
            String^ get() { return _conditionId; }
            void set(String^ value) { _conditionId = value ? value : String::Empty; }
        }

        /// <summary>Gets or sets the verification checksum</summary>
        property String^ Checksum
        {
            String^ get() { return _checksum; }
            void set(String^ value) { _checksum = value ? value : String::Empty; }
        }

        /// <summary>Gets or sets the Boolean expression</summary>
        property String^ Expression
        {
            String^ get() { return _expression; }
            void set(String^ value) { _expression = value ? value : String::Empty; }
        }

        /// <summary>Gets or sets the coverage parameters</summary>
        property String^ Parameters
        {
            String^ get() { return _parameters; }
            void set(String^ value) { _parameters = value ? value : String::Empty; }
        }

        /// <summary>Gets or sets the coverage type</summary>
        property String^ Coverage
        {
            String^ get() { return _coverage; }
            void set(String^ value) { _coverage = value ? value : String::Empty; }
        }

        /// <summary>Gets or sets the exclusion annotation</summary>
        property String^ Annotation
        {
            String^ get() { return _annotation; }
            void set(String^ value) { _annotation = value ? value : String::Empty; }
        }

        /// <summary>
        /// Returns a string representation of the condition exclusion
        /// </summary>
        virtual String^ ToString() override
        {
            return String::Format("ConditionExclusion[{0}]: {1}", _conditionId, _annotation);
        }
    };

} // namespace Managed
} // namespace ExclusionParser