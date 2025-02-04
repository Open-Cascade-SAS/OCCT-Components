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

#include <DEUSD_ConfigurationNode.hxx>

#include <DE_ConfigurationContext.hxx>
#include <NCollection_Buffer.hxx>
#include <DEUSD_Provider.hxx>
#include <TKDEUSD.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DEUSD_ConfigurationNode, DE_ConfigurationNode)

static const TCollection_AsciiString THE_CONFIGURATION_SCOPE = "provider";

//=======================================================================
// function : DEUSD_ConfigurationNode
// purpose  :
//=======================================================================
DEUSD_ConfigurationNode::DEUSD_ConfigurationNode()
    : DE_ConfigurationNode(),
      InternalParameters(USD_InternalSection()),
      myIsValidReadLicense(Standard_True),
      myIsValidWriteLicense(Standard_True)
{
}

//=======================================================================
// function : DEUSD_ConfigurationNode
// purpose  :
//=======================================================================
DEUSD_ConfigurationNode::DEUSD_ConfigurationNode(const Handle(DEUSD_ConfigurationNode)& theNode)
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
bool DEUSD_ConfigurationNode::Load(const Handle(DE_ConfigurationContext)& theResource)
{
  TCollection_AsciiString aScope = THE_CONFIGURATION_SCOPE + "." + GetFormat() + "." + GetVendor();

  InternalParameters.SystemCS =
    (RWMesh_CoordinateSystem)(theResource->IntegerVal("system.cs",
                                                      (int)InternalParameters.SystemCS,
                                                      aScope)
                              % 2);
  InternalParameters.FileCS =
    (RWMesh_CoordinateSystem)(theResource->IntegerVal("file.cs",
                                                      (int)InternalParameters.FileCS,
                                                      aScope)
                              % 2);
  InternalParameters.AnimLabel =
    (theResource->StringVal("anim.label", InternalParameters.AnimLabel, aScope));

  InternalParameters.ReadUseScaling =
    theResource->BooleanVal("read.use.scaling", InternalParameters.ReadUseScaling, aScope);

  InternalParameters.WriteNamingType =
    (WriteMode_NamingType)theResource->IntegerVal("write.naming.type",
                                                  InternalParameters.WriteNamingType,
                                                  aScope);

  return Standard_True;
}

//=======================================================================
// function : Save
// purpose  :
//=======================================================================
TCollection_AsciiString DEUSD_ConfigurationNode::Save() const
{
  TCollection_AsciiString aResult;
  aResult += "!*****************************************************************************\n";
  aResult =
    aResult + "!Configuration Node " + " Vendor: " + GetVendor() + " Format: " + GetFormat() + "\n";
  TCollection_AsciiString aScope =
    THE_CONFIGURATION_SCOPE + "." + GetFormat() + "." + GetVendor() + ".";

  aResult += "!\n";
  aResult += "!Common parameters:\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!System origin coordinate system to perform conversion into during read\n";
  aResult += "!Default value: 0(Zup). Available values: 0(Zup), 1(Yup)\n";
  aResult += aScope + "system.cs :\t " + InternalParameters.SystemCS + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!File origin coordinate system to perform conversion during read\n";
  aResult += "!Default value: 1(Yup). Available values: 0(Zup), 1(Yup)\n";
  aResult += aScope + "file.cs :\t " + InternalParameters.FileCS + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Label in XCAF document with animation to store/retrieve\n";
  aResult += "!Default value: 0:1:15. Available values 0:N:K\n";
  aResult += aScope + "anim.label :\t " + InternalParameters.AnimLabel + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Read parameters:\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!TODO\n";
  aResult += "!Default value: 1. Available values: 0, 1\n";
  aResult += aScope + "read.use.scaling :\t " + InternalParameters.ReadUseScaling + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Write parameters:\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Default value: 0(WriteMode_NamingType_InstanceName). Available values: "
             "0(WriteMode_NamingType_InstanceName), 1(WriteMode_NamingType_ProductName)\n";
  aResult += aScope + "write.naming.type :\t " + InternalParameters.WriteNamingType + "\n";
  aResult += "!\n";

  return aResult;
}

//=======================================================================
// function : Copy
// purpose  :
//=======================================================================
Handle(DE_ConfigurationNode) DEUSD_ConfigurationNode::Copy() const
{
  return new DEUSD_ConfigurationNode(*this);
}

//=======================================================================
// function : BuildProvider
// purpose  :
//=======================================================================
Handle(DE_Provider) DEUSD_ConfigurationNode::BuildProvider()
{
  return new DEUSD_Provider(this);
}

//=======================================================================
// function : UpdateLoad
// purpose  : Update loading status. Checking for the license.
//=======================================================================
bool DEUSD_ConfigurationNode::UpdateLoad(const Standard_Boolean theToImport,
                                         const Standard_Boolean theToKeep)
{
  // Checking for the license and activate if needed
}

//=======================================================================
// function : CustomActivation
// purpose  :
//=======================================================================
void DEUSD_ConfigurationNode::CustomActivation(const TColStd_ListOfAsciiString& theSequence)
{
  if (theSequence.IsEmpty())
  {
    return;
  }
  auto aValue = theSequence.begin();
  if (!aValue->IsEmpty())
  {
    TKDEUSD::Activate(*aValue);
  }
  UpdateLoad(true, false);
}

//=======================================================================
// function : IsImportSupported
// purpose  :
//=======================================================================
bool DEUSD_ConfigurationNode::IsImportSupported() const
{
  return myIsValidReadLicense;
}

//=======================================================================
// function : IsExportSupported
// purpose  :
//=======================================================================
bool DEUSD_ConfigurationNode::IsExportSupported() const
{
  return myIsValidWriteLicense;
}

//=======================================================================
// function : GetFormat
// purpose  :
//=======================================================================
TCollection_AsciiString DEUSD_ConfigurationNode::GetFormat() const
{
  return TCollection_AsciiString("USD");
}

//=======================================================================
// function : GetVendor
// purpose  :
//=======================================================================
TCollection_AsciiString DEUSD_ConfigurationNode::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}

//=======================================================================
// function : GetExtensions
// purpose  :
//=======================================================================
TColStd_ListOfAsciiString DEUSD_ConfigurationNode::GetExtensions() const
{
  TColStd_ListOfAsciiString anExt;
  anExt.Append("usd");
  anExt.Append("usda");
  anExt.Append("usdc");
  anExt.Append("usdz");
  return anExt;
}

//=======================================================================
// function : CheckContent
// purpose  :
//=======================================================================
bool DEUSD_ConfigurationNode::CheckContent(const Handle(NCollection_Buffer)& theBuffer) const
{
  (void)theBuffer;
  return false;
}
