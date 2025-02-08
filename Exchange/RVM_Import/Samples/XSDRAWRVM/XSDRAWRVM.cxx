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

#include <BRep_Builder.hxx>
#include <DBRep.hxx>
#include <DDocStd.hxx>
#include <DDocStd_DrawDocument.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_PluginMacro.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <Interface_Static.hxx>
#include <Message.hxx>
#include <OSD_File.hxx>
#include <OSD_FileIterator.hxx>
#include <DERVM_ConfigurationNode.hxx>
#include <RVMCAFControl_Controller.hxx>
#include <RVMCAFControl_Model.hxx>
#include <RVMCAFControl_Reader.hxx>
#include <TColStd_MapOfAsciiString.hxx>
#include <TDataStd_Name.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>
#include <UnitsMethods.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XSAlgo.hxx>
#include <XSAlgo_AlgoContainer.hxx>
#include <XSControl_WorkSession.hxx>
#include <XSDRAW.hxx>
#include <XSDRAWRVM.hxx>
#include <UnitsAPI.hxx>

#include <vector>

// #define ENABLE_ADDITIONAL_FUNCTIONAL

namespace
{
#ifdef ENABLE_ADDITIONAL_FUNCTIONAL
//! Check if on thePath found as a file
bool IsFile(const std::string& thePath)
{
  OSD_File aFile(OSD_Path(thePath.c_str()));

  return aFile.KindOfFile() == OSD_KindFile::OSD_FILE;
}
#endif // ENABLE_ADDITIONAL_FUNCTIONAL

//! Check if on thePath found as a directory
bool IsDirectory(const std::string& thePath)
{
  OSD_File aFile(OSD_Path(thePath.c_str()));

  return aFile.KindOfFile() == OSD_KindFile::OSD_DIRECTORY;
}

//! Get Files with theExtension from directory thePath.
//! Return files with absolute path.
std::vector<std::string> DirectoryFiles(const std::string& thePath, const char* theExtension)
{
  if (thePath.size() == 0 || !IsDirectory(thePath))
  {
    return std::vector<std::string>();
  }

  std::vector<std::string> aFiles;
  // Get all files from directory
  TCollection_AsciiString aSampleExt = theExtension;
  aSampleExt.LowerCase();
  for (OSD_FileIterator aFileIterator(OSD_Path(thePath.c_str()), "*"); aFileIterator.More();
       aFileIterator.Next())
  {
    OSD_Path aFilePath;
    aFileIterator.Values().Path(aFilePath);

    TCollection_AsciiString aFileName;
    aFilePath.SystemName(aFileName);
    Standard_Integer        aDotPos = aFileName.SearchFromEnd(".");
    TCollection_AsciiString anExt;
    if (aDotPos != -1)
    {
      anExt = aFileName.SubString(aDotPos, aFileName.Length());
      anExt.LowerCase();
    }

    if (anExt == aSampleExt)
    {
      aFiles.emplace_back(thePath + aFileName.ToCString());
    }
  }
  return aFiles;
}

} // namespace

//=======================================================================
// function : ReadRvm
//=======================================================================
static Standard_Integer ReadRvm(Draw_Interpretor& theInterpretor,
                                Standard_Integer  argc,
                                const char**      argv)
{
  TCollection_AsciiString    aDocumentName;
  TCollection_AsciiString    aFileName;
  Standard_Boolean           toSetSkipDegeneratedObjects = Standard_False;
  TCollection_ExtendedString aTagFileName;
  Standard_Boolean           toReadTagRulesFlag = Standard_False;
  Standard_Boolean           toDefineUnit       = Standard_False;
  XSAlgo::AlgoContainer()->PrepareForTransfer(); // update unit info
  Standard_Real aUnit = UnitsMethods::GetCasCadeLengthUnit(UnitsMethods_LengthUnit_Meter);

  for (Standard_Integer anArgIter = 1; anArgIter < argc; ++anArgIter)
  {
    TCollection_AsciiString anArg(argv[anArgIter]);
    anArg.LowerCase();

    if (anArg == "-skipdeg")
    {
      toSetSkipDegeneratedObjects = Standard_True;
    }
    else if (anArg == "-tagrules")
    {
      toReadTagRulesFlag = Standard_True;

      if (anArgIter + 1 < argc)
      {
        aTagFileName = TCollection_ExtendedString(argv[anArgIter + 1], true);
        ++anArgIter;
      }
    }
    else if (anArg == "-unit")
    {
      if (anArgIter + 1 < argc)
      {
        const TCollection_AsciiString aUnitStr(argv[anArgIter + 1]);
        aUnit        = UnitsAPI::AnyToSI(1, aUnitStr.ToCString());
        toDefineUnit = Standard_True;
        ++anArgIter;
      }
    }
    else if (aDocumentName.IsEmpty())
    {
      aDocumentName = argv[anArgIter];
    }
    else if (aFileName.IsEmpty())
    {
      aFileName = argv[anArgIter];
    }
    else
    {
      std::cout << "Syntax error at '" << argv[anArgIter] << "'\n";
      return 0;
    }
  }

  if (aDocumentName.IsEmpty() || aFileName.IsEmpty())
  {
    std::cout << "Syntax error: wrong number of arguments.\n";
    return 0;
  }

  XSDRAW::LoadSession();

  Handle(RVMCAFControl_Controller) aController =
    Handle(RVMCAFControl_Controller)::DownCast(XSDRAW::Controller());

  if (aController.IsNull())
  {
    XSDRAW::SetNorm("RVM");
  }

  RVMCAFControl_Reader aReader(XSDRAW::Session(), Standard_True);

  if (toReadTagRulesFlag)
  {
    aReader.SetTagRules(aTagFileName);
  }

  aReader.SetSkipDegeneratedObjects(toSetSkipDegeneratedObjects);

  Handle(Draw_ProgressIndicator) aProgress = new Draw_ProgressIndicator(theInterpretor, 1);

  Handle(TDocStd_Document) aDoc;
  Standard_CString         aDocNameStr = aDocumentName.ToCString();

  if (!DDocStd::GetDocument(aDocNameStr, aDoc, Standard_False))
  {
    Handle(TDocStd_Application) anApp = DDocStd::GetApplication();
    anApp->NewDocument("BinXCAF", aDoc);

    TDF_Label aRootLabel = aDoc->GetData()->Root();
    TDataStd_Name::Set(aRootLabel, argv[1]);

    Handle(DDocStd_DrawDocument) aDrawDocument = new DDocStd_DrawDocument(aDoc);
    Draw::Set(aDocumentName.ToCString(), aDrawDocument);
  }
  if (!toDefineUnit)
  {
    XCAFDoc_DocumentTool::GetLengthUnit(aDoc, aUnit, UnitsMethods_LengthUnit_Meter);
  }
  aReader.SetSystemLengthUnit(aUnit);
  if (IsDirectory(aFileName.ToCString()))
  {
    // It is a directory

    std::vector<std::string> aFileNames = DirectoryFiles(aFileName.ToCString(), ".rvm");

    if (!aReader.Perform(aFileNames, aDoc, aProgress->Start()))
    {
      return 0;
    }
  }
  else
  {
    if (!aReader.Perform(aFileName, aDoc, aProgress->Start()))
    {
      return 0;
    }
  }

  // Next scope for testing result of reading
  {
    std::stringstream stream;
    aReader.Dump(stream);
    theInterpretor << stream.str().c_str();
  }

  theInterpretor << "Document saved with name " << aDocumentName << "\n";

  return 0;
}

//=======================================================================
// function : Init
//=======================================================================
void XSDRAWRVM::Init()
{
  RVMCAFControl_Controller::Init();
}

//=======================================================================
// function : InitCommands
//=======================================================================
void XSDRAWRVM::InitCommands(Draw_Interpretor& theCommands)
{
  RVMCAFControl_Controller::Init();

  theCommands.Add("ReadRvm",
                  "Doc filename [-skipdeg] [-tagrules path-to-xml-tag-rules-file.xml] [-unit unit]"
                  "\n\t\t:   -skipdeg        skip degenerated objects;"
                  "\n\t\t:   -tagrules       path to XML tag rules file;"
                  "\n\t\t:   -unit           system length unit (in mm by default, can use mm, cm, "
                  "dm, m, ft, etc)."
                  "\n\t\t: Read RVM file into DECAF document.",
                  __FILE__,
                  ReadRvm,
                  "DECAF");
}

//==============================================================================
// XSDRAWRVM::Factory
//==============================================================================
void XSDRAWRVM::Factory(Draw_Interpretor& theInterpretor)
{
  XSDRAWRVM::InitCommands(theInterpretor);
}

// Declare entry point PLUGINFACTORY
DPLUGIN(XSDRAWRVM)
