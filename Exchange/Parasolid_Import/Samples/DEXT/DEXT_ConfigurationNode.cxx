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

#include <DEXT_ConfigurationNode.hxx>

#include <DE_ConfigurationNode.hxx>
#include <DE_ConfigurationContext.hxx>
#include <DE_PluginHolder.hxx>
#include <DEXT_Provider.hxx>

#include <TKDEXT.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DEXT_ConfigurationNode, DE_ConfigurationNode)

namespace
{
static const TCollection_AsciiString& THE_CONFIGURATION_SCOPE()
{
  static const TCollection_AsciiString aScope = "provider";
  return aScope;
}

// Wrapper to auto-load DE component
DE_PluginHolder<DEXT_ConfigurationNode> THE_OCCT_IGES_COMPONENT_PLUGIN;
} // namespace

//=======================================================================
// function : DEJT_ConfigurationNode
// purpose  :
//=======================================================================
DEXT_ConfigurationNode::DEXT_ConfigurationNode()
    : DE_ConfigurationNode(),
      InternalParameters(XTCAFControl_InternalSection()),
      myIsValidReadLicense(Standard_True)
{
}

//=======================================================================
// function : DEJT_ConfigurationNode
// purpose  :
//=======================================================================
DEXT_ConfigurationNode::DEXT_ConfigurationNode(const Handle(DEXT_ConfigurationNode)& theNode)
    : DE_ConfigurationNode(theNode),
      InternalParameters(theNode->InternalParameters),
      myIsValidReadLicense(theNode->myIsValidReadLicense)
{
}

//=======================================================================
// function : Load
// purpose  :
//=======================================================================
bool DEXT_ConfigurationNode::Load(const Handle(DE_ConfigurationContext)& theResource)
{
  TCollection_AsciiString aScope =
    THE_CONFIGURATION_SCOPE() + "." + GetFormat() + "." + GetVendor();

  InternalParameters.ReadColorMode =
    theResource->BooleanVal("read.color.mode", InternalParameters.ReadColorMode, aScope);
  InternalParameters.ReadNameMode =
    theResource->BooleanVal("read.name.mode", InternalParameters.ReadNameMode, aScope);
  InternalParameters.ReadResourceName =
    theResource->StringVal("read.xt.resource.name", InternalParameters.ReadResourceName, aScope);
  InternalParameters.ReadSequence =
    theResource->StringVal("read.xt.sequence", InternalParameters.ReadSequence, aScope);

  return Standard_True;
}

//=======================================================================
// function : Save
// purpose  :
//=======================================================================
TCollection_AsciiString DEXT_ConfigurationNode::Save() const
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
  aResult += "!Set read.color.mode for indicate read Colors or not\n";
  aResult += "!Default value: 1. Available values: 0, 1\n";
  aResult += aScope + "read.color.mode :\t " + InternalParameters.ReadColorMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Set read.name.mode for indicate read Names or not\n";
  aResult += "!Default value: 1. Available values: 0, 1\n";
  aResult += aScope + "read.name.mode :\t " + InternalParameters.ReadNameMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the name of the resource file\n";
  aResult += "!Default value: XT. Available values: <string>\n";
  aResult += aScope + "read.xt.resource.name :\t " + InternalParameters.ReadResourceName + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the name of the sequence of operators\n";
  aResult += "!Default value: FromXT. Available values: <string>\n";
  aResult += aScope + "read.xt.sequence :\t " + InternalParameters.ReadSequence + "\n";
  aResult += "!\n";

  aResult += "!*****************************************************************************\n";
  return aResult;
}

//=======================================================================
// function : Copy
// purpose  :
//=======================================================================
Handle(DE_ConfigurationNode) DEXT_ConfigurationNode::Copy() const
{
  return new DEXT_ConfigurationNode(*this);
}

//=======================================================================
// function : BuildProvider
// purpose  :
//=======================================================================
Handle(DE_Provider) DEXT_ConfigurationNode::BuildProvider()
{
  return new DEXT_Provider(this);
}

//=======================================================================
// function : UpdateLoad
// purpose  : Update loading status. Checking for the license.
//=======================================================================
bool DEXT_ConfigurationNode::UpdateLoad(const Standard_Boolean theToImport,
                                        const Standard_Boolean theToKeep)
{
  // Checking for the license and activate if needed
}

//=======================================================================
// function : CustomActivation
// purpose  :
//=======================================================================
void DEXT_ConfigurationNode::CustomActivation(const TColStd_ListOfAsciiString& theSequence)
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
bool DEXT_ConfigurationNode::IsImportSupported() const
{
  return myIsValidReadLicense;
}

//=======================================================================
// function : IsExportSupported
// purpose  :
//=======================================================================
bool DEXT_ConfigurationNode::IsExportSupported() const
{
  return false;
}

//=======================================================================
// function : GetFormat
// purpose  :
//=======================================================================
TCollection_AsciiString DEXT_ConfigurationNode::GetFormat() const
{
  return TCollection_AsciiString("XT");
}

//=======================================================================
// function : GetVendor
// purpose  :
//=======================================================================
TCollection_AsciiString DEXT_ConfigurationNode::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}

//=======================================================================
// function : GetExtensions
// purpose  :
//=======================================================================
TColStd_ListOfAsciiString DEXT_ConfigurationNode::GetExtensions() const
{
  TColStd_ListOfAsciiString anExt;
  anExt.Append("x_t");
  return anExt;
}
