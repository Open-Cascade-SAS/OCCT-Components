// Copyright (c) 2025 OPEN CASCADE SAS
//
// This is a code sample provided for preview and evaluation purposes.
//
// Permission is granted hereby to use this sample code for evaluation
// and learning purposes only. This code may not be used in production
// environments without proper licensing.
//
// THIS CODE SAMPLE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
// EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIMS, DAMAGES OR
// OTHER LIABILITIES.

#include <DERVM_ConfigurationNode.hxx>

#include <DE_ConfigurationNode.hxx>
#include <DE_ConfigurationContext.hxx>
#include <DE_PluginHolder.hxx>
#include <DERVM_Provider.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DERVM_ConfigurationNode, DE_ConfigurationNode)

namespace
{
static const TCollection_AsciiString& THE_CONFIGURATION_SCOPE()
{
  static const TCollection_AsciiString aScope = "provider";
  return aScope;
}

// Wrapper to auto-load DE component
DE_PluginHolder<DERVM_ConfigurationNode> THE_OCCT_IGES_COMPONENT_PLUGIN;
} // namespace

//=======================================================================
// function : DERVM_ConfigurationNode
// purpose  :
//=======================================================================
DERVM_ConfigurationNode::DERVM_ConfigurationNode()
    : DE_ConfigurationNode(),
      InternalParameters(RVMCAFControl_InternalSection()),
      myIsValidReadLicense(Standard_True)
{
}

//=======================================================================
// function : DERVM_ConfigurationNode
// purpose  :
//=======================================================================
DERVM_ConfigurationNode::DERVM_ConfigurationNode(const Handle(DERVM_ConfigurationNode)& theNode)
    : DE_ConfigurationNode(theNode),
      InternalParameters(theNode->InternalParameters),
      myIsValidReadLicense(theNode->myIsValidReadLicense)
{
}

//=======================================================================
// function : Load
// purpose  :
//=======================================================================
bool DERVM_ConfigurationNode::Load(const Handle(DE_ConfigurationContext)& theResource)
{
  TCollection_AsciiString aScope =
    THE_CONFIGURATION_SCOPE() + "." + GetFormat() + "." + GetVendor();

  InternalParameters.ReadSkipDegenerated =
    theResource->BooleanVal("read.skip.degenerated",
                            InternalParameters.ReadSkipDegenerated,
                            aScope);
  InternalParameters.ReadTagRulesFilename =
    theResource->StringVal("read.tag.rules.filename",
                           InternalParameters.ReadTagRulesFilename,
                           aScope);

  return Standard_True;
}

//=======================================================================
// function : Save
// purpose  :
//=======================================================================
TCollection_AsciiString DERVM_ConfigurationNode::Save() const
{
  TCollection_AsciiString aResult;
  aResult += "!*****************************************************************************\n";
  aResult =
    aResult + "!Configuration Node " + " Vendor: " + GetVendor() + " Format: " + GetFormat() + "\n";
  TCollection_AsciiString aScope =
    THE_CONFIGURATION_SCOPE() + "." + GetFormat() + "." + GetVendor() + ".";

  aResult += "!\n";
  aResult += "!Read parameters:\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Update Skip Degenerated Objects Flag(if this flag is true, then skip all "
             "degenerated objects\n";
  aResult += "!Default value: false. Available values: 0, 1\n";
  aResult += aScope + "read.skip.degenerated :\t " + InternalParameters.ReadSkipDegenerated + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Setup name of the tag rules file\n";
  aResult += "!Default value: empty. Available values: <string>\n";
  aResult +=
    aScope + "read.tag.rules.filename :\t " + InternalParameters.ReadTagRulesFilename + "\n";
  aResult += "!\n";

  aResult += "!*****************************************************************************\n";
  return aResult;
}

//=======================================================================
// function : Copy
// purpose  :
//=======================================================================
Handle(DE_ConfigurationNode) DERVM_ConfigurationNode::Copy() const
{
  return new DERVM_ConfigurationNode(*this);
}

//=======================================================================
// function : BuildProvider
// purpose  :
//=======================================================================
Handle(DE_Provider) DERVM_ConfigurationNode::BuildProvider()
{
  return new DERVM_Provider(this);
}

//=======================================================================
// function : UpdateLoad
// purpose  : Update loading status. Checking for the license.
//=======================================================================
bool DERVM_ConfigurationNode::UpdateLoad(const Standard_Boolean theToImport,
                                         const Standard_Boolean theToKeep)
{
}

//=======================================================================
// function : CustomActivation
// purpose  :
//=======================================================================
void DERVM_ConfigurationNode::CustomActivation(const TColStd_ListOfAsciiString& theSequence)
{
  if (theSequence.IsEmpty())
  {
    return;
  }
  auto aValue = theSequence.begin();
  if (!aValue->IsEmpty())
  {
    TKDEXT::Activate(*aValue);
  }
  UpdateLoad(true, false);
}

//=======================================================================
// function : IsImportSupported
// purpose  :
//=======================================================================
bool DERVM_ConfigurationNode::IsImportSupported() const
{
  return myIsValidReadLicense;
}

//=======================================================================
// function : IsExportSupported
// purpose  :
//=======================================================================
bool DERVM_ConfigurationNode::IsExportSupported() const
{
  return false;
}

//=======================================================================
// function : GetFormat
// purpose  :
//=======================================================================
TCollection_AsciiString DERVM_ConfigurationNode::GetFormat() const
{
  return TCollection_AsciiString("RVM");
}

//=======================================================================
// function : GetVendor
// purpose  :
//=======================================================================
TCollection_AsciiString DERVM_ConfigurationNode::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}

//=======================================================================
// function : GetExtensions
// purpose  :
//=======================================================================
TColStd_ListOfAsciiString DERVM_ConfigurationNode::GetExtensions() const
{
  TColStd_ListOfAsciiString anExt;
  anExt.Append("rvm");
  return anExt;
}
