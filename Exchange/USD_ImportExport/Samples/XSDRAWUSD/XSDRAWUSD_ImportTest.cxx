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

#include <XSDRAWUSD_ImportTest.hxx>

#include <DDocStd.hxx>
#include <DDocStd_DrawDocument.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_PluginMacro.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <Message.hxx>
#include <OSD_File.hxx>
#include <OSD_FileSystem.hxx>
#include <TDataStd_Name.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TopoDS_Shape.hxx>
#include <UnitsMethods.hxx>
#include <USD_XCAFReader.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XSAlgo.hxx>
#include <XSAlgo_AlgoContainer.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_WorkSession.hxx>
#include <XSDRAW.hxx>

//=======================================================================
// function : ReadFile
// purpose  :
//=======================================================================
static Standard_Integer ReadFile(Draw_Interpretor &theDI,
                                 Standard_Integer theNbArgs,
                                 const char **theArgVec)
{
  if (theNbArgs < 3)
  {
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  TCollection_AsciiString aDocName;
  TCollection_AsciiString aFilePath;
  Handle(TDocStd_Document) aDoc;
  Handle(TDocStd_Application) anApp = DDocStd::GetApplication();
  bool useScaling = true;
  USD_XCAFReader::USD_MetadataReadMode metadataMode = USD_XCAFReader::USD_OnlyCustomData;
  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    if (anArg.IsEmpty())
      continue;
    anArg.LowerCase();
    if (aDocName.IsEmpty())
    {
      aDocName = theArgVec[anArgIter];
      Standard_CString aNameVar = aDocName.ToCString();
      DDocStd::GetDocument(aNameVar, aDoc, Standard_False);
    }
    else if (aFilePath.IsEmpty())
    {
      aFilePath = theArgVec[anArgIter];
      OSD_File aFile(aFilePath);
      if (!aFile.Exists())
      {
        Message::SendFail() << "File '" << aFilePath << "' is not found";
        return 1;
      }
    }
    else if (anArg == "-scaling")
    {
      if (anArgIter + 1 < theNbArgs && Draw::ParseOnOff(theArgVec[anArgIter + 1], useScaling))
      {
        ++anArgIter;
      }
    }
    else if (anArg == "-metadatamode")
    {
      TCollection_AsciiString aMetaStr(theArgVec[++anArgIter]);
      aMetaStr.LowerCase();
      if (aMetaStr == "all")
      {
        metadataMode = USD_XCAFReader::USD_AllMetadata;
      }
      else if (aMetaStr == "custom")
      {
        metadataMode = USD_XCAFReader::USD_OnlyCustomData;
      }
      else if (aMetaStr == "without")
      {
        metadataMode = USD_XCAFReader::USD_WithoutMetadata;
      }
      else
      {
        std::cout << "Syntax error at '" << theArgVec[anArgIter] << "'\n";
        return 1;
      }
    }
    else
    {
      Message::SendFail() << "Syntax error at argument '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }

  if (aDocName.IsEmpty() || aFilePath.IsEmpty())
  {
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  if (aDoc.IsNull())
  {
    anApp->NewDocument(TCollection_ExtendedString("BinXCAF"), aDoc);
  }

  USD_XCAFReader aReader;
  aReader.SetDocument(aDoc);

  Standard_Real aLengthUnit = 1.;
  if (!XCAFDoc_DocumentTool::GetLengthUnit(aDoc, aLengthUnit))
  {
    aLengthUnit = UnitsMethods::GetCasCadeLengthUnit(UnitsMethods_LengthUnit_Meter);
  }

  aReader.SetSystemLengthUnit(aLengthUnit);
  aReader.SetUseScaling(useScaling);
  aReader.SetMetadataMode(metadataMode);
  aReader.SetSystemCoordinateSystem(RWMesh_CoordinateSystem_Zup);
  aReader.SetFileCoordinateSystem(RWMesh_CoordinateSystem_Yup);

  Handle(Draw_ProgressIndicator) aProgress = new Draw_ProgressIndicator(theDI);
  aProgress->SetConsoleMode(true);
  if (!aReader.Perform(aFilePath.ToCString(), aProgress->Start()))
  {
    Message::SendFail() << "Error: Cannot read any relevant data from the file";
    return 1;
  }

  Handle(DDocStd_DrawDocument) aDrawDoc = new DDocStd_DrawDocument(aDoc);
  TDataStd_Name::Set(aDoc->GetData()->Root(), aDocName.ToCString());
  Draw::Set(aDocName.ToCString(), aDrawDoc);

  return 0;
}

//=======================================================================
// function : InitCommands
// purpose  :
//=======================================================================
void XSDRAWUSD_ImportTest::InitCommands(Draw_Interpretor &theCommands)
{
  const char *aGroup = "USD";
  theCommands.Add("ReadUSD", "ReadUSD docName filepath [-scaling {on|off}=off] [-metadataMode {without|custom|all}]"
                             "\n\t\t:   '-scaling'      - scaling usage ('true' by default)"
                             "\n\t\t:   '-metadataMode' - mode of metadata reading from file. Can read all metadata, only custom data"
                             "\n\t\t:                     or don't read metadata ('custom' by default)",
                  __FILE__, ReadFile, aGroup);
}
