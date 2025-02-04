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

#include <DEDXF_ConfigurationNode.hxx>

#include <DE_ConfigurationContext.hxx>
#include <DE_PluginHolder.hxx>
#include <DEDXF_Provider.hxx>
#include <Standard_Version.hxx>

#include <TKDEDXF.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DEDXF_ConfigurationNode, DE_ConfigurationNode)

namespace
{
static const TCollection_AsciiString& THE_CONFIGURATION_SCOPE()
{
  static const TCollection_AsciiString aScope = "provider";
  return aScope;
}

// Wrapper to auto-load DE component
DE_PluginHolder<DEDXF_ConfigurationNode> THE_OCCT_IGES_COMPONENT_PLUGIN;
} // namespace

//=======================================================================
// function : DEDXF_ConfigurationNode
// purpose  :
//=======================================================================
DEDXF_ConfigurationNode::DEDXF_ConfigurationNode()
    : DE_ConfigurationNode(),
      InternalParameters(DxfSection_ConfParameters()),
      myIsValidReadLicense(Standard_True),
      myIsValidWriteLicense(Standard_True)
{
}

//=======================================================================
// function : DEDXF_ConfigurationNode
// purpose  :
//=======================================================================
DEDXF_ConfigurationNode::DEDXF_ConfigurationNode(const Handle(DEDXF_ConfigurationNode)& theNode)
    : DE_ConfigurationNode(theNode),
      InternalParameters(theNode->InternalParameters),
      myIsValidReadLicense(theNode->myIsValidReadLicense),
      myIsValidWriteLicense(theNode->myIsValidWriteLicense)
{
}

//=======================================================================
// function : Load
// purpose  :
//=======================================================================
bool DEDXF_ConfigurationNode::Load(const Handle(DE_ConfigurationContext)& theResource)
{
  TCollection_AsciiString aScope =
    THE_CONFIGURATION_SCOPE() + "." + GetFormat() + "." + GetVendor();

  InternalParameters.ReadColorMode =
    theResource->BooleanVal("read.color.mode", InternalParameters.ReadColorMode, aScope);
  InternalParameters.ReadNameMode =
    theResource->BooleanVal("read.name.mode", InternalParameters.ReadNameMode, aScope);
  InternalParameters.ReadAnonymousBlocks =
    theResource->BooleanVal("read.dxf.anonymous.blocks",
                            InternalParameters.ReadAnonymousBlocks,
                            aScope);
  InternalParameters.ReadTextEntities =
    theResource->BooleanVal("read.dxf.textentities", InternalParameters.ReadTextEntities, aScope);
  InternalParameters.ReadHatchEntities =
    theResource->BooleanVal("read.dxf.hatchentities", InternalParameters.ReadHatchEntities, aScope);
  InternalParameters.ReadResourceName =
    theResource->StringVal("read.dxf.resource.name", InternalParameters.ReadResourceName, aScope);
  InternalParameters.ReadSequence =
    theResource->StringVal("read.dxf.sequence", InternalParameters.ReadSequence, aScope);

  InternalParameters.WriteColorMode =
    theResource->BooleanVal("write.color.mode", InternalParameters.WriteColorMode, aScope);
  InternalParameters.WriteNameMode =
    theResource->BooleanVal("write.name.mode", InternalParameters.WriteNameMode, aScope);
  InternalParameters.WriteVersion =
    (DxfSection_ConfParameters::WriteMode_Version)
      theResource->IntegerVal("write.dxf.version", InternalParameters.WriteVersion, aScope);
  InternalParameters.WriteAcisMode =
    theResource->BooleanVal("write.dxf.acis.mode", InternalParameters.WriteAcisMode, aScope);
  InternalParameters.WriteDeflectionFactor =
    theResource->RealVal("write.dxf.deflection.factor",
                         InternalParameters.WriteDeflectionFactor,
                         aScope);
  InternalParameters.WriteResourceName =
    theResource->StringVal("write.dxf.resource.name", InternalParameters.WriteResourceName, aScope);
  InternalParameters.WriteSequence =
    theResource->StringVal("write.dxf.sequence", InternalParameters.WriteSequence, aScope);

  return Standard_True;
}

//=======================================================================
// function : Save
// purpose  :
//=======================================================================
TCollection_AsciiString DEDXF_ConfigurationNode::Save() const
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
  aResult += "!Set read.dxf.anonymous.blocks to indicate read Anonymous Blocks or not\n";
  aResult += "!Default value: 1. Available values: 0, 1\n";
  aResult +=
    aScope + "read.dxf.anonymous.blocks :\t " + InternalParameters.ReadAnonymousBlocks + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Set read.dxf.textentities to indicate read DXF-TEXT entities or not\n";
  aResult += "!Default value: 0. Available values: 0, 1\n";
  aResult += aScope + "read.dxf.textentities :\t " + InternalParameters.ReadTextEntities + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Set read.dxf.hatchentities to indicate read DXF-HATCH entities or not\n";
  aResult += "!Default value: 0. Available values: 0, 1\n";
  aResult += aScope + "read.dxf.hatchentities :\t " + InternalParameters.ReadHatchEntities + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the name of the resource file\n";
  aResult += "!Default value: DXF. Available values: <string>\n";
  aResult += aScope + "read.dxf.resource.name :\t " + InternalParameters.ReadResourceName + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the name of the sequence of operators\n";
  aResult += "!Default value: FromDXF. Available values: <string>\n";
  aResult += aScope + "read.dxf.sequence :\t " + InternalParameters.ReadSequence + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Write parameters:\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Set write.color.mode for indicate write Colors or not\n";
  aResult += "!Default value: 1. Available values: 0, 1\n";
  aResult += aScope + "write.color.mode :\t " + InternalParameters.WriteColorMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Set write.name.mode for indicate write Names or not\n";
  aResult += "!Default value: 1. Available values: 0, 1\n";
  aResult += aScope + "write.name.mode :\t " + InternalParameters.WriteNameMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Setting up dxf version\n";
  aResult += "!Default value: 14. Available values: 11(Undef), 12(R12), 13(R13), 14(R14),";
  aResult += " 15(R2000), 16(R2004), 17(R2007), 18(R2010), 19(R2013)\n";
  aResult += aScope + "write.dxf.version :\t " + InternalParameters.WriteVersion + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Setting up DXF Acis Mode\n";
  aResult += "!Default value: 1. Available values: 0, 1\n";
  aResult += aScope + "write.dxf.acis.mode :\t " + InternalParameters.WriteAcisMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Setting up the deflection factor\n";
  aResult += "!Default value: 0.05. Available values: <real>\n";
  aResult +=
    aScope + "write.dxf.deflection.factor :\t " + InternalParameters.WriteDeflectionFactor + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the name of the resource file\n";
  aResult += "!Default value: DXF. Available values: <string>\n";
  aResult += aScope + "write.dxf.resource.name :\t " + InternalParameters.WriteResourceName + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the name of the sequence of operators\n";
  aResult += "!Default value: ToDXF. Available values: <string>\n";
  aResult += aScope + "write.dxf.sequence :\t " + InternalParameters.WriteSequence + "\n";
  aResult += "!\n";

  aResult += "!*****************************************************************************\n";
  return aResult;
}

//=======================================================================
// function : Copy
// purpose  :
//=======================================================================
Handle(DE_ConfigurationNode) DEDXF_ConfigurationNode::Copy() const
{
  return new DEDXF_ConfigurationNode(*this);
}

//=======================================================================
// function : BuildProvider
// purpose  :
//=======================================================================
Handle(DE_Provider) DEDXF_ConfigurationNode::BuildProvider()
{
  return new DEDXF_Provider(this);
}

//=======================================================================
// function : UpdateLoad
// purpose  : Update loading status. Checking for the license.
//=======================================================================
bool DEDXF_ConfigurationNode::UpdateLoad(const Standard_Boolean theToImport,
                                         const Standard_Boolean theToKeep)
{
  // Checking for the license and activate if needed
}

//=======================================================================
// function : CustomActivation
// purpose  :
//=======================================================================
void DEDXF_ConfigurationNode::CustomActivation(const TColStd_ListOfAsciiString& theSequence)
{
  if (theSequence.IsEmpty())
  {
    return;
  }
  auto aValue = theSequence.begin();
  if (!aValue->IsEmpty())
  {
    TKDEDXF::Activate(*aValue);
  }
  UpdateLoad(true, false);
}

//=======================================================================
// function : IsImportSupported
// purpose  :
//=======================================================================
bool DEDXF_ConfigurationNode::IsImportSupported() const
{
  return myIsValidReadLicense;
}

//=======================================================================
// function : IsExportSupported
// purpose  :
//=======================================================================
bool DEDXF_ConfigurationNode::IsExportSupported() const
{
  return myIsValidWriteLicense;
}

//=======================================================================
// function : GetFormat
// purpose  :
//=======================================================================
TCollection_AsciiString DEDXF_ConfigurationNode::GetFormat() const
{
  return TCollection_AsciiString("DXF");
}

//=======================================================================
// function : GetVendor
// purpose  :
//=======================================================================
TCollection_AsciiString DEDXF_ConfigurationNode::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}

//=======================================================================
// function : GetExtensions
// purpose  :
//=======================================================================
TColStd_ListOfAsciiString DEDXF_ConfigurationNode::GetExtensions() const
{
  TColStd_ListOfAsciiString anExt;
  anExt.Append("dxf");
  return anExt;
}
