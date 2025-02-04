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

#include <XSDRAWUSD_ExportTest.hxx>

#include <DDocStd.hxx>
#include <DDocStd_DrawDocument.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_PluginMacro.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <Message.hxx>
#include <OSD_FileSystem.hxx>
#include <TDataStd_Name.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TopoDS_Shape.hxx>
#include <UnitsMethods.hxx>
#include <USD_XCAFWriter.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XSAlgo.hxx>
#include <XSAlgo_AlgoContainer.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_WorkSession.hxx>
#include <XSDRAW.hxx>

//=======================================================================
// function : WriteFile
// purpose  :
//=======================================================================
static Standard_Integer WriteFile(Draw_Interpretor &theDI,
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
  USD_XCAFWriter::USD_NamingType aNamingType = USD_XCAFWriter::USD_NamingType::USD_NamingType_InstanceName;
  Handle(TDocStd_Application) anApp = DDocStd::GetApplication();
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
    }
    else if (anArg == "-namingtype")
    {
      TCollection_AsciiString aType = theArgVec[++anArgIter];
      if (aType == "product")
      {
        aNamingType = USD_XCAFWriter::USD_NamingType_ProductName;
      }
      else if (aType, "instance")
      {
        aNamingType = USD_XCAFWriter::USD_NamingType_InstanceName;
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

  Handle(Draw_ProgressIndicator) aProgress = new Draw_ProgressIndicator(theDI);
  aProgress->SetConsoleMode(true);

  Standard_Real aLengthUnit = 1.;
  if (!XCAFDoc_DocumentTool::GetLengthUnit(aDoc, aLengthUnit))
  {
    aLengthUnit = UnitsMethods::GetCasCadeLengthUnit(UnitsMethods_LengthUnit_Meter);
  }

  RWMesh_CoordinateSystemConverter aCSConverter;
  aCSConverter.SetInputCoordinateSystem(RWMesh_CoordinateSystem_Zup);
  aCSConverter.SetOutputCoordinateSystem(RWMesh_CoordinateSystem_Yup);

  USD_XCAFWriter aWriter;
  aWriter.SetNamingType(aNamingType);
  aWriter.SetSystemLengthUnit(aLengthUnit);
  aWriter.SetCoordinateSystemConverter(aCSConverter);
  aWriter.Perform(aDoc, aFilePath.ToCString(), aProgress->Start());

  return 0;
}

//=======================================================================
// function : InitCommands
// purpose  :
//=======================================================================
void XSDRAWUSD_ExportTest::InitCommands(Draw_Interpretor &theCommands)
{
  const char *aGroup = "USD";
  theCommands.Add("WriteUSD", "WriteUSD docName filepath\n"
                              "\n\t\t:   '-namingtype [instance, product]' - product name or instance name usage ('false' by default for instance name)",
                  __FILE__, WriteFile, aGroup);
}
