/**
 * @file ExclusionParser.NET.cpp
 * @brief Implementation of managed .NET wrapper classes for Hardware Coverage Exclusion Parser
 * 
 * This file contains the implementation of C++/CLI managed wrapper classes that provide
 * .NET interop for the ExclusionParser native C++ library. These implementations handle
 * the conversion between native C++ data structures and managed .NET types.
 * 
 * @author AMD Advanced Micro Devices Inc. - Verification Team
 * @version 2.0.0
 * @date September 13, 2025
 * @copyright (c) 2025 Advanced Micro Devices Inc. All rights reserved.
 */

#include "ExclusionParser.NET.h"
#include <msclr/marshal_cppstd.h>

using namespace System;
using namespace System::Collections::Generic;
using namespace msclr::interop;

namespace ExclusionParser {
namespace Managed {

    // Helper method to convert native ToggleDirection to managed
    ToggleDirection ConvertToggleDirection(ExclusionParser::ToggleDirection nativeDir)
    {
        switch (nativeDir)
        {
        case ExclusionParser::ToggleDirection::HIGH:
            return ToggleDirection::High;
        case ExclusionParser::ToggleDirection::LOW:
            return ToggleDirection::Low;
        case ExclusionParser::ToggleDirection::BOTH:
            return ToggleDirection::Both;
        default:
            return ToggleDirection::Both;
        }
    }

    // Helper method to convert managed ToggleDirection to native
    ExclusionParser::ToggleDirection ConvertToggleDirection(ToggleDirection managedDir)
    {
        switch (managedDir)
        {
        case ToggleDirection::High:
            return ExclusionParser::ToggleDirection::HIGH;
        case ToggleDirection::Low:
            return ExclusionParser::ToggleDirection::LOW;
        case ToggleDirection::Both:
            return ExclusionParser::ToggleDirection::BOTH;
        default:
            return ExclusionParser::ToggleDirection::BOTH;
        }
    }

    // Helper method to convert native ExclusionType to managed
    ExclusionType ConvertExclusionType(ExclusionParser::ExclusionType nativeType)
    {
        switch (nativeType)
        {
        case ExclusionParser::ExclusionType::BLOCK:
            return ExclusionType::Block;
        case ExclusionParser::ExclusionType::TOGGLE:
            return ExclusionType::Toggle;
        case ExclusionParser::ExclusionType::FSM:
            return ExclusionType::FSM;
        case ExclusionParser::ExclusionType::CONDITION:
            return ExclusionType::Condition;
        default:
            return ExclusionType::Block;
        }
    }

    // Helper method to convert managed ExclusionType to native
    ExclusionParser::ExclusionType ConvertExclusionType(ExclusionType managedType)
    {
        switch (managedType)
        {
        case ExclusionType::Block:
            return ExclusionParser::ExclusionType::BLOCK;
        case ExclusionType::Toggle:
            return ExclusionParser::ExclusionType::TOGGLE;
        case ExclusionType::FSM:
            return ExclusionParser::ExclusionType::FSM;
        case ExclusionType::Condition:
            return ExclusionParser::ExclusionType::CONDITION;
        default:
            return ExclusionParser::ExclusionType::BLOCK;
        }
    }

    // Convert native BlockExclusion to managed
    BlockExclusion^ ConvertBlockExclusion(const ExclusionParser::BlockExclusion& nativeBlock)
    {
        String^ id = gcnew String(nativeBlock.id.c_str());
        String^ checksum = gcnew String(nativeBlock.checksum.c_str());
        String^ content = gcnew String(nativeBlock.content.c_str());
        String^ comment = gcnew String(nativeBlock.comment.c_str());
        
        return gcnew BlockExclusion(id, checksum, content, comment);
    }

    // Convert managed BlockExclusion to native
    ExclusionParser::BlockExclusion ConvertBlockExclusion(BlockExclusion^ managedBlock)
    {
        ExclusionParser::BlockExclusion nativeBlock;
        nativeBlock.id = marshal_as<std::string>(managedBlock->Id);
        nativeBlock.checksum = marshal_as<std::string>(managedBlock->Checksum);
        nativeBlock.content = marshal_as<std::string>(managedBlock->Content);
        nativeBlock.comment = marshal_as<std::string>(managedBlock->Comment);
        return nativeBlock;
    }

    // Convert native ToggleExclusion to managed
    ToggleExclusion^ ConvertToggleExclusion(const ExclusionParser::ToggleExclusion& nativeToggle)
    {
        ToggleDirection direction = ConvertToggleDirection(nativeToggle.direction);
        String^ signalName = gcnew String(nativeToggle.signalName.c_str());
        String^ hierarchyPath = gcnew String(nativeToggle.hierarchyPath.c_str());
        String^ comment = gcnew String(nativeToggle.comment.c_str());
        
        Nullable<int> bitIndex;
        if (nativeToggle.bitIndex >= 0)
            bitIndex = nativeToggle.bitIndex;
        
        return gcnew ToggleExclusion(direction, signalName, bitIndex, hierarchyPath, comment);
    }

    // Convert managed ToggleExclusion to native
    ExclusionParser::ToggleExclusion ConvertToggleExclusion(ToggleExclusion^ managedToggle)
    {
        ExclusionParser::ToggleExclusion nativeToggle;
        nativeToggle.direction = ConvertToggleDirection(managedToggle->Direction);
        nativeToggle.signalName = marshal_as<std::string>(managedToggle->SignalName);
        nativeToggle.hierarchyPath = marshal_as<std::string>(managedToggle->HierarchyPath);
        nativeToggle.comment = marshal_as<std::string>(managedToggle->Comment);
        nativeToggle.bitIndex = managedToggle->BitIndex.HasValue ? managedToggle->BitIndex.Value : -1;
        return nativeToggle;
    }

    // Convert native FsmExclusion to managed
    FsmExclusion^ ConvertFsmExclusion(const ExclusionParser::FsmExclusion& nativeFsm)
    {
        String^ fsmName = gcnew String(nativeFsm.fsmName.c_str());
        String^ stateName = gcnew String(nativeFsm.stateName.c_str());
        String^ transitionName = gcnew String(nativeFsm.transitionName.c_str());
        String^ hierarchyPath = gcnew String(nativeFsm.hierarchyPath.c_str());
        String^ comment = gcnew String(nativeFsm.comment.c_str());
        
        return gcnew FsmExclusion(fsmName, stateName, transitionName, hierarchyPath, comment);
    }

    // Convert managed FsmExclusion to native
    ExclusionParser::FsmExclusion ConvertFsmExclusion(FsmExclusion^ managedFsm)
    {
        ExclusionParser::FsmExclusion nativeFsm;
        nativeFsm.fsmName = marshal_as<std::string>(managedFsm->FsmName);
        nativeFsm.stateName = marshal_as<std::string>(managedFsm->StateName);
        nativeFsm.transitionName = marshal_as<std::string>(managedFsm->TransitionName);
        nativeFsm.hierarchyPath = marshal_as<std::string>(managedFsm->HierarchyPath);
        nativeFsm.comment = marshal_as<std::string>(managedFsm->Comment);
        return nativeFsm;
    }

    // Convert native ConditionExclusion to managed
    ConditionExclusion^ ConvertConditionExclusion(const ExclusionParser::ConditionExclusion& nativeCondition)
    {
        String^ conditionText = gcnew String(nativeCondition.conditionText.c_str());
        String^ hierarchyPath = gcnew String(nativeCondition.hierarchyPath.c_str());
        String^ comment = gcnew String(nativeCondition.comment.c_str());
        
        return gcnew ConditionExclusion(conditionText, hierarchyPath, comment);
    }

    // Convert managed ConditionExclusion to native
    ExclusionParser::ConditionExclusion ConvertConditionExclusion(ConditionExclusion^ managedCondition)
    {
        ExclusionParser::ConditionExclusion nativeCondition;
        nativeCondition.conditionText = marshal_as<std::string>(managedCondition->ConditionText);
        nativeCondition.hierarchyPath = marshal_as<std::string>(managedCondition->HierarchyPath);
        nativeCondition.comment = marshal_as<std::string>(managedCondition->Comment);
        return nativeCondition;
    }

    // Convert native ExclusionScope to managed
    ExclusionScope^ ConvertExclusionScope(const ExclusionParser::ExclusionScope& nativeScope)
    {
        String^ name = gcnew String(nativeScope.name.c_str());
        String^ checksum = gcnew String(nativeScope.checksum.c_str());
        
        auto managedScope = gcnew ExclusionScope(name, checksum, nativeScope.enabled);
        
        // Convert block exclusions
        for (const auto& nativeBlock : nativeScope.blockExclusions)
        {
            auto managedBlock = ConvertBlockExclusion(nativeBlock);
            managedScope->AddBlockExclusion(managedBlock);
        }
        
        // Convert toggle exclusions
        for (const auto& nativeToggle : nativeScope.toggleExclusions)
        {
            auto managedToggle = ConvertToggleExclusion(nativeToggle);
            managedScope->AddToggleExclusion(managedToggle);
        }
        
        // Convert FSM exclusions
        for (const auto& nativeFsm : nativeScope.fsmExclusions)
        {
            auto managedFsm = ConvertFsmExclusion(nativeFsm);
            managedScope->AddFsmExclusion(managedFsm);
        }
        
        // Convert condition exclusions
        for (const auto& nativeCondition : nativeScope.conditionExclusions)
        {
            auto managedCondition = ConvertConditionExclusion(nativeCondition);
            managedScope->AddConditionExclusion(managedCondition);
        }
        
        return managedScope;
    }

    // Convert managed ExclusionScope to native
    ExclusionParser::ExclusionScope ConvertExclusionScope(ExclusionScope^ managedScope)
    {
        ExclusionParser::ExclusionScope nativeScope;
        nativeScope.name = marshal_as<std::string>(managedScope->Name);
        nativeScope.checksum = marshal_as<std::string>(managedScope->Checksum);
        nativeScope.enabled = managedScope->Enabled;
        
        // Convert block exclusions
        for each (BlockExclusion^ managedBlock in managedScope->BlockExclusions->Values)
        {
            nativeScope.blockExclusions.push_back(ConvertBlockExclusion(managedBlock));
        }
        
        // Convert toggle exclusions
        for each (ToggleExclusion^ managedToggle in managedScope->ToggleExclusions->Values)
        {
            nativeScope.toggleExclusions.push_back(ConvertToggleExclusion(managedToggle));
        }
        
        // Convert FSM exclusions
        for each (FsmExclusion^ managedFsm in managedScope->FsmExclusions->Values)
        {
            nativeScope.fsmExclusions.push_back(ConvertFsmExclusion(managedFsm));
        }
        
        // Convert condition exclusions
        for each (ConditionExclusion^ managedCondition in managedScope->ConditionExclusions->Values)
        {
            nativeScope.conditionExclusions.push_back(ConvertConditionExclusion(managedCondition));
        }
        
        return nativeScope;
    }

    // Implementation of ExclusionParserNet::ConvertToManaged
    ExclusionData^ ExclusionParserNet::ConvertToManaged(const ExclusionParser::ExclusionData& nativeData)
    {
        String^ fileName = gcnew String(nativeData.fileName.c_str());
        auto managedData = gcnew ExclusionData(fileName);
        
        // Convert metadata
        for (const auto& pair : nativeData.metadata)
        {
            String^ key = gcnew String(pair.first.c_str());
            String^ value = gcnew String(pair.second.c_str());
            managedData->SetMetadata(key, value);
        }
        
        // Convert scopes
        for (const auto& nativeScope : nativeData.scopes)
        {
            auto managedScope = ConvertExclusionScope(nativeScope);
            managedData->AddScope(managedScope);
        }
        
        return managedData;
    }

    // Implementation of ExclusionParserNet::ConvertToNative
    std::unique_ptr<ExclusionParser::ExclusionData> ExclusionParserNet::ConvertToNative(ExclusionData^ managedData)
    {
        auto nativeData = std::make_unique<ExclusionParser::ExclusionData>();
        nativeData->fileName = marshal_as<std::string>(managedData->FileName);
        
        // Convert metadata
        for each (auto pair in managedData->Metadata)
        {
            std::string key = marshal_as<std::string>(pair.Key);
            std::string value = marshal_as<std::string>(pair.Value);
            nativeData->metadata[key] = value;
        }
        
        // Convert scopes
        for each (ExclusionScope^ managedScope in managedData->Scopes->Values)
        {
            nativeData->scopes.push_back(ConvertExclusionScope(managedScope));
        }
        
        return nativeData;
    }

    // Implementation of ExclusionWriterNet::ConvertToNative
    std::unique_ptr<ExclusionParser::ExclusionData> ExclusionWriterNet::ConvertToNative(ExclusionData^ managedData)
    {
        auto nativeData = std::make_unique<ExclusionParser::ExclusionData>();
        nativeData->fileName = marshal_as<std::string>(managedData->FileName);
        
        // Convert metadata
        for each (auto pair in managedData->Metadata)
        {
            std::string key = marshal_as<std::string>(pair.Key);
            std::string value = marshal_as<std::string>(pair.Value);
            nativeData->metadata[key] = value;
        }
        
        // Convert scopes
        for each (ExclusionScope^ managedScope in managedData->Scopes->Values)
        {
            nativeData->scopes.push_back(ConvertExclusionScope(managedScope));
        }
        
        return nativeData;
    }

} // namespace Managed
} // namespace ExclusionParser