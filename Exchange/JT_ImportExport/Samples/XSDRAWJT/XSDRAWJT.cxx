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

#include <XSDRAWJT.hxx>

#include <BinDrivers.hxx>
#include <BinLDrivers.hxx>
#include <BinXCAFDrivers.hxx>
#include <DDocStd.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <DDocStd_DrawDocument.hxx>
#include <DE_Wrapper.hxx>
#include <Draw.hxx>
#include <Draw_PluginMacro.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <NCollection_AccAllocator.hxx>
#include <JtData_Model.hxx>
#include <JtTools_FileDumpBRepProvider.hxx>
#include <JtWireframe_BRepProvider.hxx>
#include <DEJT_ConfigurationNode.hxx>
#include <JTCAFControl_Reader.hxx>
#include <JTCAFControl_XcafToJT.hxx>
#include <JtNode_Partition.hxx>
#include <JtTools_Assembly.hxx>
#include <UnitsMethods.hxx>
#include <OSD_File.hxx>
#include <OSD_OpenFile.hxx>
#include <OSD_Protection.hxx>
#include <StdDrivers.hxx>
#include <StdLDrivers.hxx>
#include <TDataStd_Name.hxx>
#include <ViewerTest.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XmlDrivers.hxx>
#include <XmlLDrivers.hxx>
#include <XmlXCAFDrivers.hxx>
#include <XSAlgo.hxx>
#include <XSAlgo_AlgoContainer.hxx>
#include <Jt_SceneGraph.hxx>

//! Return global instance of XT B-Rep provider to be loaded by another DRAW plugin linked to TKDEXT.
static Handle(JtTools_IBRepProvider) & XSDRAWJT_XTBRepProvider()
{
  static Handle(JtTools_IBRepProvider) THE_INSTANCE;
  return THE_INSTANCE;
}

//! Print B-Rep type within JT file.
void printBRepJtType(const unsigned int theTypes, TCollection_AsciiString &theStr)
{
  if (theTypes)
  {
    if (theTypes & JtTools_IBRepProvider::BRepSegmentFlag(JtData_SegmentType_JtBRep))
      theStr += "jtbrep";
    if (theTypes & JtTools_IBRepProvider::BRepSegmentFlag(JtData_SegmentType_XtBRep))
    {
      if (!theStr.IsEmpty())
        theStr += "_";
      theStr += "xt";
    }
    if (theTypes & JtTools_IBRepProvider::BRepSegmentFlag(JtData_SegmentType_Wireframe))
    {
      if (!theStr.IsEmpty())
        theStr += "_";
      theStr += "wireframe";
    }
    if (theTypes & JtTools_IBRepProvider::BRepSegmentFlag(JtData_SegmentType_ULP))
    {
      if (!theStr.IsEmpty())
        theStr += "_";
      theStr += "ulp";
    }
    if (theTypes & JtTools_IBRepProvider::BRepSegmentFlag(JtData_SegmentType_LWPA))
    {
      if (!theStr.IsEmpty())
        theStr += "_";
      theStr += "lwpa";
    }
    if (theTypes & JtTools_IBRepProvider::BRepSegmentFlag(JtData_SegmentType_MultiXtBRep))
    {
      if (!theStr.IsEmpty())
        theStr += "_";
      theStr += "multixt";
    }
    if (theTypes & JtTools_IBRepProvider::BRepSegmentFlag(JtData_SegmentType_StepBRep) ||
        theTypes & JtTools_IBRepProvider::BRepSegmentFlag(JtData_SegmentType_JT10_5_StepBRep))
    {
      if (!theStr.IsEmpty())
        theStr += "_";
      theStr += "step";
    }
  }
  else
  {
    theStr = "none";
  }
}
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
// function : SetXTBRepProvider
// purpose  :
//=======================================================================
void XSDRAWJT::SetXTBRepProvider(const Handle(JtTools_IBRepProvider) & theBRepProvider)
{
  XSDRAWJT_XTBRepProvider() = theBRepProvider;
  Handle(DEJT_ConfigurationNode) aNode;
  if (DE_Wrapper::GlobalWrapper()->Find("JT", "OCC", aNode))
  {
    aNode->SetProviderXt(theBRepProvider);
  }
}

//=======================================================================
// function : LoadLSG
// purpose  :
//=======================================================================
static Standard_Integer LoadLSG(
    Draw_Interpretor & /*theDI*/,
    Standard_Integer theNbArgs,
    const char **theArgVec)
{
  if (theNbArgs != 2)
  {
    std::cout << "Usage: LoadLSG jtFile\n";
    return 1;
  }

  TCollection_AsciiString aFileName(theArgVec[1]);
  Handle(Jt_SceneGraph) aLSG = new Jt_SceneGraph;
  aLSG->LoadFile(aFileName);

  return 0;
}

//=======================================================================
// function : ReadJt
// purpose  :
//=======================================================================
static Standard_Integer ReadJt(
    Draw_Interpretor &theDI,
    Standard_Integer theNbArgs,
    const char **theArgVec)
{
  TCollection_AsciiString aDocName;
  TCollection_AsciiString aFilePath;
  TCollection_AsciiString anOutNbLodsVar, anOutBRepTypeVar, anOutHasMixedNbLodsVar;
  Standard_Integer aLodIndex = 0;
  bool toUseExistingDoc = false;
  bool toParallel = false;
  bool toSkipDegenerateTriangles = true;
  bool toDeduplicatePrims = true;
  bool toDeferMetadata = false;
  bool toCacheFileContent = false;
  bool loadHiddenMetadata = true;
  bool toSkipLateDataLoading = false;
  bool toKeepLateData = true;
  bool toLoadStrictly = false;
  bool toPrintDebugInfo = false;
  bool toFilterLayers = false;
  bool toReadWireframe = false;
  bool toReadXt = false;
  JTCAFControl_Representation aPrefGeomRepr = JTCAFControl_Representation_PreferMesh;
  Handle(JtTools_IBRepProvider) aDumpBRepProvider;
  Handle(TDocStd_Document) aDoc;
  Handle(TDocStd_Application) anApp = DDocStd::GetApplication();
  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    if ((anArg == "-lod") && (anArgIter + 1 < theNbArgs))
    {
      ++anArgIter;
      aLodIndex = Draw::Atoi(theArgVec[anArgIter]);
    }
    else if (anArg == "-nocreate" || anArg == "-nocreatedoc")
    {
      toUseExistingDoc = true;
      if (anArgIter + 1 < theNbArgs && Draw::ParseOnOff(theArgVec[anArgIter + 1], toUseExistingDoc))
      {
        ++anArgIter;
      }
    }
    else if (anArg == "-parallel")
    {
      toParallel = true;
      if (anArgIter + 1 < theNbArgs && Draw::ParseOnOff(theArgVec[anArgIter + 1], toParallel))
      {
        ++anArgIter;
      }
    }
    else if (anArg == "-skipdegeneratetriangles" || anArg == "-skipdegenerate")
    {
      toSkipDegenerateTriangles = true;
      if (anArgIter + 1 < theNbArgs && Draw::ParseOnOff(theArgVec[anArgIter + 1], toSkipDegenerateTriangles))
      {
        ++anArgIter;
      }
    }
    else if (anArg == "-deduplicateprimitives" || anArg == "-deduplicateprims")
    {
      toDeduplicatePrims = true;
      if (anArgIter + 1 < theNbArgs && Draw::ParseOnOff(theArgVec[anArgIter + 1], toDeduplicatePrims))
      {
        ++anArgIter;
      }
    }
    else if (anArg == "-defermetadata")
    {
      toDeferMetadata = true;
      if (anArgIter + 1 < theNbArgs && Draw::ParseOnOff(theArgVec[anArgIter + 1], toDeferMetadata))
      {
        ++anArgIter;
      }
    }
    else if (anArg == "-loadhiddenmetadata")
    {
      if (anArgIter + 1 < theNbArgs && Draw::ParseOnOff(theArgVec[anArgIter + 1], loadHiddenMetadata))
      {
        ++anArgIter;
      }
    }
    else if (anArg == "-tocachefilecontent" || anArg == "-tocachefile" || anArg == "-tocachecontent" || anArg == "-tocache")
    {
      toCacheFileContent = true;
      if (anArgIter + 1 < theNbArgs && Draw::ParseOnOff(theArgVec[anArgIter + 1], toCacheFileContent))
      {
        ++anArgIter;
      }
    }
    else if (anArg == "-skiplateloading")
    {
      toSkipLateDataLoading = true;
      if (anArgIter + 1 < theNbArgs && Draw::ParseOnOff(theArgVec[anArgIter + 1], toSkipLateDataLoading))
      {
        ++anArgIter;
      }
    }
    else if (anArg == "-keeplate")
    {
      toKeepLateData = true;
      if (anArgIter + 1 < theNbArgs && Draw::ParseOnOff(theArgVec[anArgIter + 1], toKeepLateData))
      {
        ++anArgIter;
      }
    }
    else if (anArg == "-strictly" || anArg == "-exact")
    {
      toLoadStrictly = true;
      if (anArgIter + 1 < theNbArgs && Draw::ParseOnOff(theArgVec[anArgIter + 1], toLoadStrictly))
      {
        ++anArgIter;
      }
    }
    else if (anArg == "-toprintinfo" || anArg == "-toprintdebuginfo")
    {
      toPrintDebugInfo = true;
      if (anArgIter + 1 < theNbArgs && Draw::ParseOnOff(theArgVec[anArgIter + 1], toPrintDebugInfo))
      {
        ++anArgIter;
      }
    }
    else if (anArg == "-outnblods" && anArgIter + 1 < theNbArgs)
    {
      anOutNbLodsVar = theArgVec[++anArgIter];
    }
    else if ((anArg == "-outbreptype" || anArg == "-outbrep") && anArgIter + 1 < theNbArgs)
    {
      anOutBRepTypeVar = theArgVec[++anArgIter];
    }
    else if ((anArg == "-geom" || anArg == "-geometry" || anArg == "-repr" || anArg == "-representation") && anArgIter + 1 < theNbArgs)
    {
      TCollection_AsciiString aReprStr(theArgVec[++anArgIter]);
      aReprStr.LowerCase();
      if (aReprStr == "preferbrep")
      {
        aPrefGeomRepr = JTCAFControl_Representation_PreferBRep;
      }
      else if (aReprStr == "prefermesh")
      {
        aPrefGeomRepr = JTCAFControl_Representation_PreferMesh;
      }
      else if (aReprStr == "mesh" || aReprStr == "meshonly")
      {
        aPrefGeomRepr = JTCAFControl_Representation_MeshOnly;
      }
      else if (aReprStr == "brep" || aReprStr == "breponly")
      {
        aPrefGeomRepr = JTCAFControl_Representation_BRepOnly;
      }
      else
      {
        std::cout << "Syntax error at '" << theArgVec[anArgIter] << "'\n";
        return 1;
      }
      if (aPrefGeomRepr != JTCAFControl_Representation_MeshOnly)
      {
        Standard_Boolean isDefault = true;
        for (Standard_Integer aBRepArgIter = anArgIter + 1;
             aBRepArgIter < theNbArgs; ++aBRepArgIter)
        {
          TCollection_AsciiString aBRepArg(theArgVec[aBRepArgIter]);
          aBRepArg.LowerCase();
          if (aBRepArg == "xt")
          {
            toReadXt = true;
          }
          else if (aBRepArg == "wireframe")
          {
            toReadWireframe = true;
          }
          else if (aBRepArg == "all")
          {
            toReadXt = true;
            toReadWireframe = true;
          }
          else
          {
            // the main argument index is not incremented
            break;
          }
          isDefault = false;
          anArgIter++;
        }
        if (isDefault)
        {
          toReadXt = true;
          toReadWireframe = true;
        }
      }
    }
    else if (anArg == "-dumpbrep" && anArgIter + 1 < theNbArgs)
    {
      aDumpBRepProvider = new JtTools_FileDumpBRepProvider(theArgVec[++anArgIter]);
    }
    else if (anArg == "-filterlayers")
    {
      toFilterLayers = true;
    }
    else if (anArg == "-outhasmixednblods" && anArgIter + 1 < theNbArgs)
    {
      anOutHasMixedNbLodsVar = theArgVec[++anArgIter];
    }
    else if (aDocName.IsEmpty())
    {
      aDocName = theArgVec[anArgIter];
      Standard_CString aNameVar = aDocName.ToCString();
      DDocStd::GetDocument(aNameVar, aDoc, Standard_False);
    }
    else if (aFilePath.IsEmpty())
    {
      aFilePath = theArgVec[anArgIter];
    }
    else
    {
      std::cout << "Syntax error at argument '" << theArgVec[anArgIter] << "'\n";
      return 1;
    }
  }
  if (aDocName.IsEmpty() || aFilePath.IsEmpty())
  {
    std::cout << "Syntax error: wrong number of arguments\n";
    return 1;
  }

  Handle(Draw_ProgressIndicator) aProgress = new Draw_ProgressIndicator(theDI, 1);
  if (aDoc.IsNull())
  {
    if (toUseExistingDoc)
    {
      std::cout << "Error: document with name " << aDocName << " does not exist\n";
      return 1;
    }
    anApp->NewDocument(TCollection_ExtendedString("BinXCAF"), aDoc);
  }
  else if (!toUseExistingDoc)
  {
    std::cout << "Error: document with name " << aDocName << " already exists\n";
    return 1;
  }

  {
    JTCAFControl_Reader aReader;
    aReader.SetParallel(toParallel);
    aReader.SetTriangulationLod(aLodIndex);
    aReader.SetSkipDegenerateTriangles(toSkipDegenerateTriangles);
    aReader.SetDeduplicatePrimitives(toDeduplicatePrims);
    aReader.SetDeferMetadata(toDeferMetadata);
    aReader.SetCacheFileContent(toCacheFileContent);
    aReader.SetPreferredRepresentation(aPrefGeomRepr);
    aReader.SetLoadHiddenMetadata(loadHiddenMetadata);
    aReader.SetToSkipLateDataLoading(toSkipLateDataLoading);
    aReader.SetToKeepLateData(toKeepLateData);
    aReader.SetToLoadStrictly(toLoadStrictly);
    aReader.SetToPrintDebugMessages(toPrintDebugInfo);
    aReader.SetFilterLayers(toFilterLayers);

    if (!anOutBRepTypeVar.IsEmpty())
    {
      aReader.SetCheckBRepTypes(Standard_True);
    }

    if (!aDumpBRepProvider.IsNull())
    {
      aReader.SetBRepProvider(aDumpBRepProvider);
    }
    else
    {
      const Handle(JtTools_IBRepProvider) &aXTBRepProvider = XSDRAWJT_XTBRepProvider();
      if (toReadXt && !aXTBRepProvider.IsNull())
      {
        aReader.SetBRepProvider(aXTBRepProvider, JtData_SegmentType_XtBRep);
        aReader.SetBRepProvider(aXTBRepProvider, JtData_SegmentType_MultiXtBRep);
      }
      if (toReadWireframe)
      {
        Handle(JtTools_IBRepProvider) aWFP = new JtWireframe_BRepProvider();
        aReader.SetBRepProvider(aWFP, JtData_SegmentType_Wireframe);
      }
    }

    Standard_Real aLengthUnitM = 1.;
    if (!XCAFDoc_DocumentTool::GetLengthUnit(aDoc, aLengthUnitM))
    {
      XSAlgo::AlgoContainer()->PrepareForTransfer();
      aLengthUnitM = UnitsMethods::GetCasCadeLengthUnit(UnitsMethods_LengthUnit_Meter);
    }
    aReader.SetSystemLengthUnit(aLengthUnitM);

    if (!aReader.Perform(aDoc, aFilePath.ToCString(), aProgress->Start()))
    {
      std::cout << "Error: file reading failed '" << aFilePath << "'\n";
      return 1;
    }

    if (aLodIndex >= aReader.NbTriangulationLods())
    {
      std::cout << "Warning: JT file does not define LOD " << aLodIndex << "\n";
    }
    TCollection_AsciiString aTypeStr;
    printBRepJtType(aReader.BRepGeometryTypes(), aTypeStr);
    theDI << "Nb.LODs: " << aReader.NbTriangulationLods() << "\n";
    theDI << "B-Rep:   " << aTypeStr << "\n";
    if (!anOutNbLodsVar.IsEmpty())
    {
      Draw::Set(anOutNbLodsVar.ToCString(), aReader.NbTriangulationLods());
    }
    if (!anOutBRepTypeVar.IsEmpty())
    {
      Draw::Set(anOutBRepTypeVar.ToCString(), aTypeStr.ToCString());
    }
    if (!anOutHasMixedNbLodsVar.IsEmpty())
    {
      Draw::Set(anOutHasMixedNbLodsVar.ToCString(), aReader.HasMixedNbLods() ? "true" : "false");
    }
  }

  Handle(DDocStd_DrawDocument) aDrawDoc = new DDocStd_DrawDocument(aDoc);
  TDataStd_Name::Set(aDoc->GetData()->Root(), aDocName.ToCString());
  Draw::Set(aDocName.ToCString(), aDrawDoc);
  return 0;
}

//=======================================================================
// function : WriteJt
// purpose  :
//=======================================================================

static Standard_Integer WriteJt(
    Draw_Interpretor &theDI,
    const Standard_Integer theCount,
    const char **theArgs)
{
  if (theCount < 3)
  {
    theDI.PrintHelp(theArgs[0]);
    return 1;
  }

  // LOD parameters
  Standard_Boolean isDeflection = Standard_False;
  JtData_Array<IMeshTools_Parameters> aLODParameters;
  // Max file count
  Standard_Integer aFileCountMax = 1;
  // Length unit parameters
  Standard_Real aCafLengthUnitM = 1.;
  JtData_LengthUnit aJtLengthUnit = JtData_LengthUnit_UNKNOWN;
  // Name format
  JTCAFControl_NameFormat aNameFormat = JTCAFControl_InstanceName;
  for (Standard_Integer i = 3; i < theCount; ++i)
  {
    if (isDeflection)
    {
      if (theArgs[i][0] == '-')
        isDeflection = Standard_False;
      else
      {
        aLODParameters.push_back(IMeshTools_Parameters());
        aLODParameters.back().Deflection = Draw::Atof(theArgs[i]);
        continue;
      }
    }

    if (!strcmp(theArgs[i], "-deflections"))
    {
      isDeflection = Standard_True;
    }
    else if (!strcmp(theArgs[i], "-partition"))
    {
      if (theCount > i + 1)
      {
        aFileCountMax = Draw::Atoi(theArgs[++i]);
      }
    }
    else if (!strcmp(theArgs[i], "-name"))
    {
      if (theCount > i + 1)
      {
        if (!strcmp(theArgs[++i], "product"))
        {
          aNameFormat = JTCAFControl_ProductName;
        }
      }
    }
    else if (!strcmp(theArgs[i], "-jtLengthUnit") && theCount > i + 1)
    {
      i++;
      Standard_Integer aK = JtData_LengthUnit_MM;
      Standard_Boolean isCorrectParameter = Standard_False;
      while (aK <= JtData_LengthUnit_MI)
      {
        if (strcmp(JtData_Model::GetJtLengthUnitName(static_cast<JtData_LengthUnit>(aK)),
                   theArgs[i]) == 0)
        {
          aJtLengthUnit = static_cast<JtData_LengthUnit>(aK);
          isCorrectParameter = Standard_True;
          break;
        }
        ++aK;
      }
      if (!isCorrectParameter)
      {
        std::cout << "Error: invalid key parameter -lu \'" << theArgs[i] << "\', use"
                  << "millimeters, centimeters, meters, inches, feet, "
                  << "yards, micrometers, decimeters, kilometers, mils , miles\n";
        return 1;
      }
    }
    else
    {
      std::cout << "Error: invalid key - \'" << theArgs[i] << "\'\n";
      return 1;
    }
  }

  Handle(TDocStd_Document) aDoc;
  Handle(TDocStd_Application) aTmpDocApp;
  Standard_CString aDocName = theArgs[1];
  if (!DDocStd::GetDocument(aDocName, aDoc))
  {
    // Read the XBF file.
    aTmpDocApp = new TDocStd_Application();
    StdLDrivers::DefineFormat(aTmpDocApp);
    BinLDrivers::DefineFormat(aTmpDocApp);
    XmlLDrivers::DefineFormat(aTmpDocApp);
    StdDrivers::DefineFormat(aTmpDocApp);
    BinDrivers::DefineFormat(aTmpDocApp);
    XmlDrivers::DefineFormat(aTmpDocApp);
    BinXCAFDrivers::DefineFormat(aTmpDocApp);
    XmlXCAFDrivers::DefineFormat(aTmpDocApp);

    aTmpDocApp->NewDocument("BinXCAF", aDoc);
    if (aTmpDocApp->Open(aDocName, aDoc) != PCDM_RS_OK)
    {
      std::cout << "Error: failed to read file " << aDocName << ".\n";
      return 0;
    }
  }
  if (!XCAFDoc_DocumentTool::GetLengthUnit(aDoc, aCafLengthUnitM))
  {
    XSAlgo::AlgoContainer()->PrepareForTransfer();
    aCafLengthUnitM = UnitsMethods::GetCasCadeLengthUnit(UnitsMethods_LengthUnit_Meter);
  }

  // Create and save the JT model.
  Jt_GUID::Value(Standard_True);
  JTCAFControl_XcafToJT aConverter;
  const Handle(JtNode_Partition) aJTModelStructure =
      aConverter.Convert(aDoc, aFileCountMax, aLODParameters, aNameFormat, aCafLengthUnitM, aJtLengthUnit);
  if (!aTmpDocApp.IsNull())
  {
    aTmpDocApp->Close(aDoc);
  }

  Handle(JtData_Model) aJTModel = new JtData_Model(theArgs[2]);
  if (!JtData_Model::Store(aJTModel, aJTModelStructure))
  {
    std::cout << "Error: failed to write file " << theArgs[2] << ".\n";
  }

  return 0;
}
//=======================================================================
// function : readwritejt
// purpose  :
//=======================================================================
static Standard_Integer ReadWriteJt(Draw_Interpretor &theDI,
                                    Standard_Integer theCount,
                                    const char **theArgs)
{
  if (theCount != 3)
  {
    theDI.PrintHelp(theArgs[0]);
    return 1;
  }

  Handle(NCollection_BaseAllocator) anAllocator = new NCollection_AccAllocator(16 * 1024);
  Handle(Draw_ProgressIndicator) aProgress = new Draw_ProgressIndicator(theDI, 1);

  Handle(JtData_Model) aReadModel = new JtData_Model(theArgs[1]);
  Handle(JtNode_Partition) aRootNode = aReadModel->Init(anAllocator, aProgress->Start(), Standard_False);

  Handle(JtData_Model) aModelsave = new JtData_Model(theArgs[2]);
  JtData_Model::Store(aModelsave, aRootNode);

  return 0;
}

//=======================================================================
// function : AddJtModel
// purpose  :
//=======================================================================

// A holder for JT model file paths.
static JtData_Array<TCollection_AsciiString> ModelPaths;

static Standard_Integer AddJtModel(
    Draw_Interpretor &,
    Standard_Integer theCount,
    const char **theArgs)
{
  if (theCount > 2)
  {
    Message::SendFail() << "Error: invalid count of arguments.\n";
    return 1;
  }

  if (theCount == 2)
  {
    ModelPaths.push_back(theArgs[1]);
  }
  else
  {
    ModelPaths.clear();
  }

  return 0;
}

//=======================================================================
// function : AssembleJt
// purpose  :
//=======================================================================

static Standard_Integer AssembleJt(
    Draw_Interpretor &theDI,
    Standard_Integer theCount,
    const char **theArgs)
{
  if (theCount < 2)
  {
    Message::SendFail() << "Error: invalid count of arguments.\n";
    return 1;
  }

  const Standard_Boolean isShattered = (strcmp(theArgs[1], "-s") == 0);
  if ((isShattered && theCount == 2) || (!isShattered && theCount == 3))
  {
    Message::SendFail() << "Error: invalid count of arguments.\n";
    return 1;
  }

  JtData_Array<TCollection_AsciiString> *aModelPathPointer = &ModelPaths;
  JtData_Array<TCollection_AsciiString> aModelPaths;
  if ((isShattered && theCount != 3) || (!isShattered && theCount != 2))
  {
    for (Jt_I32 aI = (isShattered ? 3 : 2); aI < theCount; ++aI)
    {
      aModelPaths.push_back(theArgs[aI]);
    }

    aModelPathPointer = &aModelPaths;
  }

  Handle(JtNode_Partition) aAssembly;
  Handle(Draw_ProgressIndicator) aProgressI = new Draw_ProgressIndicator(theDI, 1);
  if (isShattered)
  {
    if (!JtTools_Assembly::Shattered(aAssembly, *aModelPathPointer, aProgressI->Start()))
    {
      Message::SendFail() << "Error: failed to assemble.\n";
    }

    if (aAssembly.IsNull())
    {
      return 0;
    }
  }
  else
  {
    if (!JtTools_Assembly::Monolithic(aAssembly, *aModelPathPointer, aProgressI->Start()))
    {
      Message::SendFail() << "Error: failed to assemble.\n";
      return 0;
    }
  }

  Handle(JtData_Model) aJTModel = new JtData_Model(theArgs[isShattered ? 2 : 1]);
  if (!JtData_Model::Store(aJTModel, aAssembly, isShattered))
  {
    Message::SendFail() << "Error: failed to store the assembly.\n";
  }

  return 0;
}

//=======================================================================
// function : Init
// purpose  :
//=======================================================================

void XSDRAWJT::Init()
{
}

//=======================================================================
// function : InitCommands
// purpose  :
//=======================================================================

void XSDRAWJT::InitCommands(Draw_Interpretor &theCommands)
{
  const char *aGroup = "TKDEJT";
  theCommands.Add("ReadJt",
                  "ReadJt cafDoc jtFile"
                  "\n\t\t:   [-noCreateDoc {on|off}=off]"
                  "\n\t\t:   [-lod LOD=0]"
                  "\n\t\t:   [-parallel {on|off}=off]"
                  "\n\t\t:   [-skipDegenerate {on|off}=on]"
                  "\n\t\t:   [-deduplicatePrims {on|off}=on]"
                  "\n\t\t:   [-deferMetadata {on|off}=off]"
                  "\n\t\t:   [-loadhiddenmetadata {on|off}=on]"
                  "\n\t\t:   [-toCacheFileContent {on|off}=off]"
                  "\n\t\t:   [-skipLateLoading {on|off}=on]"
                  "\n\t\t:   [-keepLate {on|off}=on]]"
                  "\n\t\t:   [-toPrintDebugInfo {on|off}=on]]"
                  "\n\t\t:   [-outNbLods Var] [-outBRepType Var]"
                  "\n\t\t:   [-geometry {{preferBRep|preferMesh|BRep}[BRep types = all]|mesh}]"
                  "\n\t\t:   [-dumpBRep filePrefix]"
                  "\n\t\t:   [-outHasMixedNbLods Var]"
                  "\n\t\t:   [-filterLayers]"
                  "\n\t\t: Reads the JT file and translates it into the CAF document."
                  "\n\t\t:   '-lod' - load specific triangulation (0 - highest quality)"
                  "\n\t\t:   '-strictly' - load and activate ONLY exactly specified triangulation."
                  "\n\t\t:       Full file structure will be loaded in any case."
                  "\n\t\t:   '-parallel' - use multithreaded optimizations"
                  "\n\t\t:   '-skipDegenerate' - skip degenerate triangles"
                  "\n\t\t:   '-deduplicatePrims' - share BRep geometry defining JT"
                  "\n\t\t:       primitives with the same parameters"
                  "\n\t\t:   '-deferMetadata' - do not read late-load Metadata straight away"
                  "\n\t\t:   '-loadhiddenmetadata' - controls loading of hidden Metadata"
                  "\n\t\t:   '-noCreateDoc' - use the existing document instead of a new"
                  "\n\t\t:       one"
                  "\n\t\t:   '-toCacheFileContent' - cache entire file content within JT"
                  "\n\t\t:       Model"
                  "\n\t\t:   '-skipLateLoading' - skip data loading. It can be performed later (false by default)"
                  "\n\t\t:   '-keepLate' - load data into itself with preservation of information"
                  "\n\t\t:       about deferred storage to load/unload this data later"
                  "\n\t\t:   '-toPrintDebugInfo' - print additional debug inforamtion during data reading"
                  "\n\t\t:   '-outNbLods' - set the TCL variable to the number of LODs"
                  "\n\t\t:       in the JT file"
                  "\n\t\t:   '-outBRepType' - set the TCL variable to the type of B-Rep"
                  "\n\t\t:       geometry in the JT file"
                  "\n\t\t:   '-geometry' - define preferred geometry representation:"
                  "\n\t\t:       when alternatives are available in JT file;"
                  "\n\t\t:       preferBRep for reading B-Rep first and mesh as fallback,"
                  "\n\t\t:       preferMesh for reading mesh first and B-Rep as fallback,"
                  "\n\t\t:       mesh, BRep for reading only one representation without fallback;"
                  "\n\t\t:       [BRep types] for define preferred B-Rep types:"
                  "\n\t\t:       all for reading all existing B-Rep types"
                  "\n\t\t:       Xt for reading Xt B-Rep type, if its provider exists,"
                  "\n\t\t:       Wireframe for reading Wireframe-Rep type, if its provider exists,"
                  "\n\t\t:   '-dumpBRep' - dump B-Rep representations to files with specified prefix,"
                  "\n\t\t:       for testing purpose"
                  "\n\t\t:   '-outHasMixedNbLods' - set the TCL variable to the 'true' value if there are"
                  "\n\t\t:        different numbers of LODs in the JT file else to 'false' value"
                  "\n\t\t:   '-filterLayers' - filter the layers by properties \"ACTIVELAYERFILTER\","
                  "\n\t\t:       \"LAYERFILTER<ID>\" and \"LAYER\"",
                  __FILE__, ReadJt, aGroup);
  theCommands.Add("WriteJt",
                  "WriteJt cafDoc jtFile [-partition [<max file count>]]\n"
                  "    [-deflections <LOD0 deflection> [<LOD1 deflection> ...]]\n"
                  "    [-name {product/instance}]\n"
                  "    [-lu <CAF length unit in mm> <JT length unit>]\n"
                  "  Writes the pre-triangulated model or the model with the required deflections\n"
                  "  from the CAF document to the JT file:\n"
                  "    '-partition' - write part nodes outside of the file containing theirs"
                  "      parents.\n"
                  "    '-deflections' - deflections of the LODs (as many deflections are given,"
                  "      as many LODs will be created).\n"
                  "    '-name' - format of the node names (instance name is used by default).\n"
                  "    '-jtLengthUnit' - sets the length units for the JT model, valid names:,\n"
                  "      'millimeters, centimeters, meters, inches, "
                  "      'feet, yards, micrometers, decimeters, kilometers, mils, miles\n",
                  __FILE__, WriteJt, aGroup);
  theCommands.Add("LoadLSG",
                  "LoadLSG jtFile\n"
                  "  Iterates over scene graph of loaded jt file",
                  __FILE__, LoadLSG, aGroup);

  theCommands.Add("ReadWriteJt", "Reads and writes jt file via Jt data model without xcaf conversion."
                                 "\n\t\t:   Usage: ReadWriteJt in_file out_file",
                  __FILE__, ReadWriteJt, aGroup);

  theCommands.Add("AddJtModel",
                  "AddJtModel [<model file path>]\n"
                  "  Adds the path to the list used by command AssembleJt.\n"
                  "  Clears the list if the path is not set",
                  __FILE__, AddJtModel, aGroup);
  theCommands.Add("AssembleJt",
                  "\n"
                  "  AssembleJt [-s] <supermodel file path>\n"
                  "    [<model file path 1> ... <model file path n>]\n"
                  "\n"
                  "  With option '-s' assembles the JT models to a single shattered JT supermodel\n"
                  "  that uses the original models without any copying. All file paths are\n"
                  "  considered relative to the directory of the supermodel except its own path.\n"
                  "\n"
                  "  Without option '-s' assembles the JT models to a single monolithic JT\n"
                  "  supermodel.\n"
                  "\n"
                  "  The list created by command 'AddJtModel' is used\n"
                  "  if the file paths are not set.",
                  __FILE__, AssembleJt, aGroup);
}

//==============================================================================
// XSDRAWJT::Factory
//==============================================================================
void XSDRAWJT::Factory(Draw_Interpretor &theDI)
{
  XSDRAWJT::InitCommands(theDI);

#ifdef DEB
  std::cout << "Draw Plugin : All TKXSDRAWJT commands are loaded" << std::endl;
#endif
}

// Declare entry point PLUGINFACTORY
DPLUGIN(XSDRAWJT)
