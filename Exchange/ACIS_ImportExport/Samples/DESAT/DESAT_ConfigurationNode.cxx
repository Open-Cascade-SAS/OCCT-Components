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

#include <DESAT_ConfigurationNode.hxx>

#include <DE_ConfigurationNode.hxx>
#include <DE_ConfigurationContext.hxx>
#include <DE_PluginHolder.hxx>
#include <DESAT_Provider.hxx>

#include <TKDESAT.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DESAT_ConfigurationNode, DE_ConfigurationNode)

namespace
{
static const TCollection_AsciiString& THE_CONFIGURATION_SCOPE()
{
  static const TCollection_AsciiString aScope = "provider";
  return aScope;
}

// Wrapper to auto-load DE component
DE_PluginHolder<DESAT_ConfigurationNode> THE_OCCT_IGES_COMPONENT_PLUGIN;
} // namespace

//=======================================================================
// function : DESAT_ConfigurationNode
// purpose  :
//=======================================================================
DESAT_ConfigurationNode::DESAT_ConfigurationNode()
    : DE_ConfigurationNode(),
      InternalParameters(AcisData_ConfParameters()),
      myIsValidReadLicense(Standard_True),
      myIsValidWriteLicense(Standard_True)
{
}

//=======================================================================
// function : DESAT_ConfigurationNode
// purpose  :
//=======================================================================
DESAT_ConfigurationNode::DESAT_ConfigurationNode(const Handle(DESAT_ConfigurationNode)& theNode)
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
bool DESAT_ConfigurationNode::Load(const Handle(DE_ConfigurationContext)& theResource)
{
  TCollection_AsciiString aScope =
    THE_CONFIGURATION_SCOPE() + "." + GetFormat() + "." + GetVendor();

  InternalParameters.ReadColorMode =
    theResource->BooleanVal("read.color.mode", InternalParameters.ReadColorMode, aScope);
  InternalParameters.ReadNameMode =
    theResource->BooleanVal("read.name.mode", InternalParameters.ReadNameMode, aScope);
  InternalParameters.ReadParallelMode =
    theResource->BooleanVal("read.parallel.mode", InternalParameters.ReadParallelMode, aScope);
  InternalParameters.ReadMaxPrecisionVal =
    theResource->RealVal("read.maxprecision.val", InternalParameters.ReadMaxPrecisionVal, aScope);
  InternalParameters.ReadResourceName =
    theResource->StringVal("read.resource.name", InternalParameters.ReadResourceName, aScope);
  InternalParameters.ReadSequence =
    theResource->StringVal("read.sequence", InternalParameters.ReadSequence, aScope);

  InternalParameters.WriteColorMode =
    theResource->BooleanVal("write.color.mode", InternalParameters.WriteColorMode, aScope);
  InternalParameters.WriteNameMode =
    theResource->BooleanVal("write.name.mode", InternalParameters.WriteNameMode, aScope);
  InternalParameters.WritePrecisionMode =
    (AcisData_ConfParameters::Write_PrecisionMode)theResource->IntegerVal(
      "write.precision.mode",
      InternalParameters.WritePrecisionMode,
      aScope);
  InternalParameters.WritePrecisionVal =
    theResource->RealVal("write.precision.val", InternalParameters.WritePrecisionVal, aScope);
  InternalParameters.WriteHeaderVersion =
    theResource->IntegerVal("write.header.version", InternalParameters.WriteHeaderVersion, aScope);
  InternalParameters.WriteResourceName =
    theResource->StringVal("write.resource.name", InternalParameters.WriteResourceName, aScope);
  InternalParameters.WriteSequence =
    theResource->StringVal("write.sequence", InternalParameters.WriteSequence, aScope);

  return Standard_True;
}

//=======================================================================
// function : Save
// purpose  :
//=======================================================================
TCollection_AsciiString DESAT_ConfigurationNode::Save() const
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
  aResult += "!Setting up parallel processing mode\n";
  aResult += "!Default value: 0. Available values: 0, 1\n";
  aResult += aScope + "read.parallel.mode :\t " + InternalParameters.ReadParallelMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the maximum allowable tolerance (in internal units, which are specified in "
             "xstep.cascade.unit)";
  aResult += " of the shape\n";
  aResult += "!Default value: 1. Available values: any real positive (non null) value\n";
  aResult += aScope + "read.maxprecision.val :\t " + InternalParameters.ReadMaxPrecisionVal + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the name of the resource file\n";
  aResult += "!Default value: SAT. Available values: <string>\n";
  aResult += aScope + "read.resource.name :\t " + InternalParameters.ReadResourceName + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the name of the sequence of operators\n";
  aResult += "!Default value: FromSAT. Available values: <string>\n";
  aResult += aScope + "read.sequence :\t " + InternalParameters.ReadSequence + "\n";
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
  aResult += "!Specifies the mode of writing the resolution value into the SAT file\n";
  aResult += "!Default value: Average(0). Available values: \"Least\"(-1), \"Average\"(0), ";
  aResult += "\"Greatest\"(1), \"Session\"(2)\n";
  aResult += aScope + "write.precision.mode :\t " + InternalParameters.WritePrecisionMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!This parameter gives the resolution value for a SAT file when the "
             "write.precision.mode parameter value is 1\n";
  aResult += "!Default value: 0.0001. Available values: any real positive (non null) value\n";
  aResult += aScope + "write.precision.val :\t " + InternalParameters.WritePrecisionVal + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Setting up Header Version\n";
  aResult += "!Default value: 700. Available values: <int>\n";
  aResult += aScope + "write.header.version :\t " + InternalParameters.WriteHeaderVersion + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the name of the resource file\n";
  aResult += "!Default value: SAT. Available values: <string>\n";
  aResult += aScope + "write.resource.name :\t " + InternalParameters.WriteResourceName + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the name of the sequence of operators\n";
  aResult += "!Default value: ToSAT. Available values: <string>\n";
  aResult += aScope + "write.sequence :\t " + InternalParameters.WriteSequence + "\n";
  aResult += "!\n";

  aResult += "!*****************************************************************************\n";
  return aResult;
}

//=======================================================================
// function : Copy
// purpose  :
//=======================================================================
Handle(DE_ConfigurationNode) DESAT_ConfigurationNode::Copy() const
{
  return new DESAT_ConfigurationNode(*this);
}

//=======================================================================
// function : BuildProvider
// purpose  :
//=======================================================================
Handle(DE_Provider) DESAT_ConfigurationNode::BuildProvider()
{
  return new DESAT_Provider(this);
}

//=======================================================================
// function : UpdateLoad
// purpose  : Update loading status. Checking for the license.
//=======================================================================
bool DESAT_ConfigurationNode::UpdateLoad(const Standard_Boolean theToImport,
                                         const Standard_Boolean theToKeep)
{
  // Checking for the license and activate if needed
}

//=======================================================================
// function : CustomActivation
// purpose  :
//=======================================================================
void DESAT_ConfigurationNode::CustomActivation(const TColStd_ListOfAsciiString& theSequence)
{
  if (theSequence.IsEmpty())
  {
    return;
  }
  auto aValue = theSequence.begin();
  if (!aValue->IsEmpty())
  {
    TKDESAT::Activate(*aValue);
  }
  UpdateLoad(true, false);
}

//=======================================================================
// function : IsImportSupported
// purpose  :
//=======================================================================
bool DESAT_ConfigurationNode::IsImportSupported() const
{
  return myIsValidReadLicense;
}

//=======================================================================
// function : IsExportSupported
// purpose  :
//=======================================================================
bool DESAT_ConfigurationNode::IsExportSupported() const
{
  return myIsValidWriteLicense;
}

//=======================================================================
// function : GetFormat
// purpose  :
//=======================================================================
TCollection_AsciiString DESAT_ConfigurationNode::GetFormat() const
{
  return TCollection_AsciiString("SAT");
}

//=======================================================================
// function : GetVendor
// purpose  :
//=======================================================================
TCollection_AsciiString DESAT_ConfigurationNode::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}

//=======================================================================
// function : GetExtensions
// purpose  :
//=======================================================================
TColStd_ListOfAsciiString DESAT_ConfigurationNode::GetExtensions() const
{
  TColStd_ListOfAsciiString anExt;
  anExt.Append("sat");
  anExt.Append("sab");
  return anExt;
}
