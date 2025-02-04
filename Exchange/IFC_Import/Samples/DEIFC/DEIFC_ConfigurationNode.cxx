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

#include <DEIFC_ConfigurationNode.hxx>

#include <DE_ConfigurationContext.hxx>
#include <DE_PluginHolder.hxx>
#include <DEIFC_Provider.hxx>

#include <TKDEIFC.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DEIFC_ConfigurationNode, DE_ConfigurationNode)

namespace
{
static const TCollection_AsciiString& THE_CONFIGURATION_SCOPE()
{
  static const TCollection_AsciiString aScope = "provider";
  return aScope;
}

// Wrapper to auto-load DE component
DE_PluginHolder<DEIFC_ConfigurationNode> THE_OCCT_IGES_COMPONENT_PLUGIN;
} // namespace

//=======================================================================
// function : DEIFC_ConfigurationNode
// purpose  :
//=======================================================================
DEIFC_ConfigurationNode::DEIFC_ConfigurationNode()
    : DE_ConfigurationNode(),
      InternalParameters(IFCCAFControl_InternalSection()),
      myIsValidReadLicense(Standard_True)
{
}

//=======================================================================
// function : DEIFC_ConfigurationNode
// purpose  :
//=======================================================================
DEIFC_ConfigurationNode::DEIFC_ConfigurationNode(const Handle(DEIFC_ConfigurationNode)& theNode)
    : DE_ConfigurationNode(theNode),
      InternalParameters(theNode->InternalParameters),
      myIsValidReadLicense(theNode->myIsValidReadLicense)
{
}

//=======================================================================
// function : Load
// purpose  :
//=======================================================================
bool DEIFC_ConfigurationNode::Load(const Handle(DE_ConfigurationContext)& theResource)
{
  TCollection_AsciiString aScope =
    THE_CONFIGURATION_SCOPE() + "." + GetFormat() + "." + GetVendor();

  InternalParameters.ReadDiscardProjTrsf =
    theResource->BooleanVal("read.discard.proj.trsf",
                            InternalParameters.ReadDiscardProjTrsf,
                            aScope);
  InternalParameters.ReadExpandProdRepr =
    theResource->BooleanVal("read.expand.prod.repr", InternalParameters.ReadExpandProdRepr, aScope);
  InternalParameters.ReadUseTypeAsName =
    theResource->BooleanVal("read.use.type.as.name", InternalParameters.ReadUseTypeAsName, aScope);
  InternalParameters.ReadAngleDeflection =
    theResource->RealVal("read.ifc.angledeflection",
                         InternalParameters.ReadAngleDeflection,
                         aScope);

  return Standard_True;
}

//=======================================================================
// function : Save
// purpose  :
//=======================================================================
TCollection_AsciiString DEIFC_ConfigurationNode::Save() const
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
  aResult += "!Set flag to ignore project (root) transformation\n";
  aResult += "!Default value: 0. Available values: 0, 1\n";
  aResult += aScope + "read.discard.proj.trsf :\t " + InternalParameters.ReadDiscardProjTrsf + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult +=
    "!Sets the option for enabling/disabling the expansion of the Product Representation items, ";
  aResult += "usually consisting of Geometric Representation items which do not usually have their "
             "own names.\n";
  aResult += "!Default value: 1. Available values: 0, 1\n";
  aResult += aScope + "read.expand.prod.repr :\t " + InternalParameters.ReadExpandProdRepr + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Sets the option which allows using type of the IFC entity as a prefix for its name "
             "in assembly structure\n";
  aResult += "!Default value: 0. Available values: 0, 1\n";
  aResult += aScope + "read.use.type.as.name :\t " + InternalParameters.ReadUseTypeAsName + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Set default value for angle deflection (used for analys of tangentiality ";
  aResult += "of faces during creation of Poly_Triangulation)";
  aResult += "usually consisting of Geometric Representation items which do not usually have their "
             "own names.\n";
  aResult += "!Default value (in degrees): 45. Available values: <real>\n";
  aResult +=
    aScope + "read.ifc.angledeflection :\t " + InternalParameters.ReadAngleDeflection + "\n";
  aResult += "!\n";

  aResult += "!*****************************************************************************\n";
  return aResult;
}

//=======================================================================
// function : Copy
// purpose  :
//=======================================================================
Handle(DE_ConfigurationNode) DEIFC_ConfigurationNode::Copy() const
{
  return new DEIFC_ConfigurationNode(*this);
}

//=======================================================================
// function : BuildProvider
// purpose  :
//=======================================================================
Handle(DE_Provider) DEIFC_ConfigurationNode::BuildProvider()
{
  return new DEIFC_Provider(this);
}

//=======================================================================
// function : UpdateLoad
// purpose  : Update loading status. Checking for the license.
//=======================================================================
bool DEIFC_ConfigurationNode::UpdateLoad(const Standard_Boolean theToImport,
                                         const Standard_Boolean theToKeep)
{
  // Checking for the license and activate if needed
}

//=======================================================================
// function : CustomActivation
// purpose  :
//=======================================================================
void DEIFC_ConfigurationNode::CustomActivation(const TColStd_ListOfAsciiString& theSequence)
{
  if (theSequence.IsEmpty())
  {
    return;
  }
  auto aValue = theSequence.begin();
  if (!aValue->IsEmpty())
  {
    TKDEIFC::Activate(*aValue);
  }
  UpdateLoad(true, false);
}

//=======================================================================
// function : IsImportSupported
// purpose  :
//=======================================================================
bool DEIFC_ConfigurationNode::IsImportSupported() const
{
  return myIsValidReadLicense;
}

//=======================================================================
// function : IsExportSupported
// purpose  :
//=======================================================================
bool DEIFC_ConfigurationNode::IsExportSupported() const
{
  return false;
}

//=======================================================================
// function : GetFormat
// purpose  :
//=======================================================================
TCollection_AsciiString DEIFC_ConfigurationNode::GetFormat() const
{
  return TCollection_AsciiString("IFC");
}

//=======================================================================
// function : GetVendor
// purpose  :
//=======================================================================
TCollection_AsciiString DEIFC_ConfigurationNode::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}

//=======================================================================
// function : GetExtensions
// purpose  :
//=======================================================================
TColStd_ListOfAsciiString DEIFC_ConfigurationNode::GetExtensions() const
{
  TColStd_ListOfAsciiString anExt;
  anExt.Append("ifc");
  return anExt;
}
