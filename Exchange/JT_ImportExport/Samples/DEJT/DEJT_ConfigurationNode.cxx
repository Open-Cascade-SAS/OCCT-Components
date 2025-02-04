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

#include <DEJT_ConfigurationNode.hxx>

#include <DE_ConfigurationContext.hxx>
#include <DE_PluginHolder.hxx>
#include <DEJT_Provider.hxx>
#include <NCollection_Buffer.hxx>

#include <TKDEJT.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DEJT_ConfigurationNode, DE_ConfigurationNode)

namespace
{
static const TCollection_AsciiString& THE_CONFIGURATION_SCOPE()
{
  static const TCollection_AsciiString aScope = "provider";
  return aScope;
}

// Wrapper to auto-load DE component
DE_PluginHolder<DEJT_ConfigurationNode> THE_OCCT_IGES_COMPONENT_PLUGIN;
} // namespace

//=======================================================================
// function : DEJT_ConfigurationNode
// purpose  :
//=======================================================================
DEJT_ConfigurationNode::DEJT_ConfigurationNode()
    : DE_ConfigurationNode(),
      InternalParameters(JTCAFControl_InternalSection()),
      myIsValidJtLicense(Standard_True),
      myIsValidXtLicense(Standard_True)
{
}

//=======================================================================
// function : DEJT_ConfigurationNode
// purpose  :
//=======================================================================
DEJT_ConfigurationNode::DEJT_ConfigurationNode(const Handle(DEJT_ConfigurationNode)& theNode)
    : DE_ConfigurationNode(theNode),
      InternalParameters(theNode->InternalParameters),
      myIsValidJtLicense(theNode->myIsValidJtLicense),
      myIsValidXtLicense(theNode->myIsValidXtLicense)
{
}

//=======================================================================
// function : Load
// purpose  :
//=======================================================================
bool DEJT_ConfigurationNode::Load(const Handle(DE_ConfigurationContext)& theResource)
{
  TCollection_AsciiString aScope =
    THE_CONFIGURATION_SCOPE() + "." + GetFormat() + "." + GetVendor();

  InternalParameters.ReadCheckBRepTypes =
    theResource->BooleanVal("read.check.brep.types", InternalParameters.ReadCheckBRepTypes, aScope);
  InternalParameters.ReadDumpBRepProvider =
    theResource->StringVal("read.dump.brep.provider",
                           InternalParameters.ReadDumpBRepProvider,
                           aScope);
  InternalParameters.ReadLodIndex =
    theResource->IntegerVal("read.lod.index", InternalParameters.ReadLodIndex, aScope);
  InternalParameters.ReadParallel =
    theResource->BooleanVal("read.parallel", InternalParameters.ReadParallel, aScope);
  InternalParameters.ReadSkipDegenerateTriangles =
    theResource->BooleanVal("read.skip.degenerate.triangles",
                            InternalParameters.ReadSkipDegenerateTriangles,
                            aScope);
  InternalParameters.ReadDeduplicatePrims =
    theResource->BooleanVal("read.deduplicate.prims",
                            InternalParameters.ReadDeduplicatePrims,
                            aScope);
  InternalParameters.ReadDeferMetadata =
    theResource->BooleanVal("read.defer.metadata", InternalParameters.ReadDeferMetadata, aScope);
  InternalParameters.ReadCacheFileContent =
    theResource->BooleanVal("read.cache.file.content",
                            InternalParameters.ReadCacheFileContent,
                            aScope);
  InternalParameters.ReadHiddenMetadata =
    theResource->BooleanVal("read.hidden.metadata", InternalParameters.ReadHiddenMetadata, aScope);
  InternalParameters.ReadSkipLateDataLoading =
    theResource->BooleanVal("read.skip.late.data.loading",
                            InternalParameters.ReadSkipLateDataLoading,
                            aScope);
  InternalParameters.ReadKeepLateData =
    theResource->BooleanVal("read.keep.late.data", InternalParameters.ReadKeepLateData, aScope);
  InternalParameters.ReadLoadStrictly =
    theResource->BooleanVal("read.load.strictly", InternalParameters.ReadLoadStrictly, aScope);
  InternalParameters.ReadPrintDebugInfo =
    theResource->BooleanVal("read.print.debug.info", InternalParameters.ReadPrintDebugInfo, aScope);
  InternalParameters.ReadFilterLayers =
    theResource->BooleanVal("read.filter.layers", InternalParameters.ReadFilterLayers, aScope);
  InternalParameters.ReadWireframe =
    theResource->BooleanVal("read.wireframe", InternalParameters.ReadWireframe, aScope);
  InternalParameters.ReadXt = theResource->BooleanVal("read.xt", InternalParameters.ReadXt, aScope);
  InternalParameters.ReadPrefGeomRepr =
    (ReadMode_PrefGeomRepr)theResource->IntegerVal("read.pref.geom.repr",
                                                   InternalParameters.ReadPrefGeomRepr,
                                                   aScope);
  InternalParameters.ReadTranslateMetadata =
    theResource->BooleanVal("read.translate.metadata",
                            InternalParameters.ReadTranslateMetadata,
                            aScope);
  InternalParameters.ReadTranslatePMI =
    theResource->BooleanVal("read.translate.pmi", InternalParameters.ReadTranslatePMI, aScope);

  InternalParameters.WriteNameFormat =
    (WriteMode_NameFormat)theResource->IntegerVal("write.name.format",
                                                  InternalParameters.WriteNameFormat,
                                                  aScope);
  InternalParameters.WriteJTLengthUnit =
    (JtData_LengthUnit)theResource->IntegerVal("write.jt.length.unit",
                                               InternalParameters.WriteJTLengthUnit,
                                               aScope);

  return Standard_True;
}

//=======================================================================
// function : Save
// purpose  :
//=======================================================================
TCollection_AsciiString DEJT_ConfigurationNode::Save() const
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
  aResult += "!Set read.check.brep.types if BRep types should be find or not\n";
  aResult += "!Default value: 0. Available values: 0, 1\n";
  aResult += aScope + "read.check.brep.types :\t " + InternalParameters.ReadCheckBRepTypes + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Creating new BRepProvider by the string value\n";
  aResult += "!Default value: empty. Available values: <string>\n";
  aResult +=
    aScope + "read.dump.brep.provider :\t " + InternalParameters.ReadDumpBRepProvider + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Set triangulation LOD to be loaded during translation\n";
  aResult += "!Default value: 0. Available values: <int>\n";
  aResult += aScope + "read.lod.index :\t " + InternalParameters.ReadLodIndex + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Setting up parallel reading\n";
  aResult += "!Default value: 0. Available values: 0, 1\n";
  aResult += aScope + "read.parallel :\t " + InternalParameters.ReadParallel + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Set if degenerate triangles should be skipped or not\n";
  aResult += "!Default value: 1. Available values: 0, 1\n";
  aResult += aScope + "read.skip.degenerate.triangles :\t "
             + InternalParameters.ReadSkipDegenerateTriangles + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult +=
    "!Set if translator should try generating single geometry (TopoDS_Shape) for JT primitives";
  aResult += "(cylinders, boxes, spheres) having the same parameters (but different location)\n";
  aResult += "!Default value: 1. Available values: 0, 1\n";
  aResult +=
    aScope + "read.deduplicate.prims :\t " + InternalParameters.ReadDeduplicatePrims + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Set if Metadata reading should be deferred\n";
  aResult += "!Default value: 0. Available values: 0, 1\n";
  aResult += aScope + "read.defer.metadata :\t " + InternalParameters.ReadDeferMetadata + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Set if JtData_Model should cache file content in internal buffer";
  aResult += " for later reusage (e.g. for loading LODs)\n";
  aResult += "!Default value: 0. Available values: 0, 1\n";
  aResult +=
    aScope + "read.cache.file.content :\t " + InternalParameters.ReadCacheFileContent + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Sets the flag controlling the loading of hidden Metadata\n";
  aResult += "!Default value: 1. Available values: 0, 1\n";
  aResult += aScope + "read.hidden.metadata :\t " + InternalParameters.ReadHiddenMetadata + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Sets flag to skip data loading\n";
  aResult += "!Default value: 0. Available values: 0, 1\n";
  aResult +=
    aScope + "read.skip.late.data.loading :\t " + InternalParameters.ReadSkipLateDataLoading + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Sets flag to keep information about deferred storage to load/unload data later\n";
  aResult += "!Default value: 1. Available values: 0, 1\n";
  aResult += aScope + "read.keep.late.data :\t " + InternalParameters.ReadKeepLateData + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Sets flag to load and activate only exactly specified triangulation LOD\n";
  aResult += "!Default value: 0. Available values: 0, 1\n";
  aResult += aScope + "read.load.strictly :\t " + InternalParameters.ReadLoadStrictly + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Sets flag to print debug information\n";
  aResult += "!Default value: 0. Available values: 0, 1\n";
  aResult += aScope + "read.print.debug.info :\t " + InternalParameters.ReadPrintDebugInfo + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Sets whether to filter the layers\n";
  aResult += "!Default value: 0. Available values: 0, 1\n";
  aResult += aScope + "read.filter.layers :\t " + InternalParameters.ReadFilterLayers + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!If true setting JtWirefram_BRepProvider\n";
  aResult += "!Default value: 0. Available values: 0, 1\n";
  aResult += aScope + "read.wireframe :\t " + InternalParameters.ReadWireframe + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Setting XSDRAWJT_XTBRepProvider\n";
  aResult += "!Default value: 0. Available values: 0, 1\n";
  aResult += aScope + "read.xt :\t " + InternalParameters.ReadXt + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Sets preferred geometry representation\n";
  aResult += "!Default value: 0 (take mesh representation, when both BRep and Mesh are available, "
             "and take BRep otherwise).";
  aResult += "Available values :  0(take mesh representation, when both BRep and Mesh are "
             "available, and take BRep otherwise), ";
  aResult +=
    "1(take BRep representation, when both BRep and Mesh are available, and take Mesh otherwise), ";
  aResult += "2((for diagnostics) accept only mesh representation and ignore any BRep), ";
  aResult += "3((for diagnostics) accept only B-Rep and ignore any mesh representation)\n";
  aResult += aScope + "read.pref.geom.repr :\t " + InternalParameters.ReadPrefGeomRepr + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Set if Metadata should be translated into XDE document\n";
  aResult += "!Default value: 1. Available values: 0, 1\n";
  aResult +=
    aScope + "read.translate.metadata :\t " + InternalParameters.ReadTranslateMetadata + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Set if PMI data should be translated into XDE document\n";
  aResult += "!Default value: 1. Available values: 0, 1\n";
  aResult += aScope + "read.translate.pmi :\t " + InternalParameters.ReadTranslatePMI + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Write parameters:\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Define the export name format for JT nodes\n";
  aResult +=
    "!Default value: 0(Instance Name). Available values: 0(Instance Name), 1(Product Name)\n";
  aResult += aScope + "write.name.format :\t " + InternalParameters.WriteNameFormat + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Setting up JT length unit using in Data Exchange\n";
  aResult += "!Default value: 0. Available values: 0(MM), 1(CM), 2(M), 3(INCH), 4(FT), 5(YARDS), "
             "6(UM), 7(DM), ";
  aResult += "8(KM), 9(MIL = 0.001 inch), 10(MILE)\n";
  aResult += aScope + "write.jt.length.unit :\t " + InternalParameters.WriteJTLengthUnit + "\n";
  aResult += "!\n";

  aResult += "!*****************************************************************************\n";
  return aResult;
}

//=======================================================================
// function : Copy
// purpose  :
//=======================================================================
Handle(DE_ConfigurationNode) DEJT_ConfigurationNode::Copy() const
{
  return new DEJT_ConfigurationNode(*this);
}

//=======================================================================
// function : BuildProvider
// purpose  :
//=======================================================================
Handle(DE_Provider) DEJT_ConfigurationNode::BuildProvider()
{
  return new DEJT_Provider(this);
}

//=======================================================================
// function : UpdateLoad
// purpose  : Update loading status. Checking for the license.
//=======================================================================
bool DEJT_ConfigurationNode::UpdateLoad(const Standard_Boolean theToImport,
                                        const Standard_Boolean theToKeep)
{
  // Checking for the license and activate if needed
}

//=======================================================================
// function : CustomActivation
// purpose  :
//=======================================================================
void DEJT_ConfigurationNode::CustomActivation(const TColStd_ListOfAsciiString& theSequence)
{
  if (theSequence.IsEmpty())
  {
    return;
  }
  auto aValue = theSequence.begin();
  if (!aValue->IsEmpty())
  {
    TKDEJT::Activate(*aValue);
  }
  UpdateLoad(true, false);
}

//=======================================================================
// function : IsImportSupported
// purpose  :
//=======================================================================
bool DEJT_ConfigurationNode::IsImportSupported() const
{
  return myIsValidJtLicense;
}

//=======================================================================
// function : IsExportSupported
// purpose  :
//=======================================================================
bool DEJT_ConfigurationNode::IsExportSupported() const
{
  return myIsValidJtLicense;
}

//=======================================================================
// function : GetFormat
// purpose  :
//=======================================================================
TCollection_AsciiString DEJT_ConfigurationNode::GetFormat() const
{
  return TCollection_AsciiString("JT");
}

//=======================================================================
// function : GetVendor
// purpose  :
//=======================================================================
TCollection_AsciiString DEJT_ConfigurationNode::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}

//=======================================================================
// function : GetExtensions
// purpose  :
//=======================================================================
TColStd_ListOfAsciiString DEJT_ConfigurationNode::GetExtensions() const
{
  TColStd_ListOfAsciiString anExt;
  anExt.Append("jt");
  return anExt;
}

//=======================================================================
// function : CheckContent
// purpose  :
//=======================================================================
bool DEJT_ConfigurationNode::CheckContent(const Handle(NCollection_Buffer)& theBuffer) const
{
  if (theBuffer.IsNull() || theBuffer->Size() < 20)
  {
    return false;
  }
  const char* aBytes = (const char*)theBuffer->Data();
  if (!::strncmp(aBytes, "Version ", 8)
      && ((aBytes[12] == 'J' && aBytes[13] == 'T') || // Version 9.5 JT
          (aBytes[13] == 'J' && aBytes[14] == 'T')))  // Version 10.0 JT
  {
    return true;
  }
  // old versions without JT keyword
  if ((!::strncmp(aBytes, "Version 8.0", 11) || // Version 8.0
       !::strncmp(aBytes, "Version 8.1", 11)))  // Version 8.1
  {
    return true;
  }
  return false;
}
