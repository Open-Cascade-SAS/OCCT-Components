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

#include <DEUSD_Provider.hxx>

#include <BinXCAFDrivers.hxx>
#include <BRep_Builder.hxx>
#include <Message.hxx>
#include <TDF_Tool.hxx>
#include <TDocStd_Application.hxx>
#include <DEUSD_ConfigurationNode.hxx>
#include <USD_XCAFReader.hxx>
#include <USD_XCAFWriter.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DEUSD_Provider, DE_Provider)

//=======================================================================
// function : DEUSD_Provider
// purpose  :
//=======================================================================
DEUSD_Provider::DEUSD_Provider() {}

//=======================================================================
// function : DEUSD_Provider
// purpose  :
//=======================================================================
DEUSD_Provider::DEUSD_Provider(const Handle(DE_ConfigurationNode)& theNode)
    : DE_Provider(theNode)
{
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool DEUSD_Provider::Read(const TCollection_AsciiString&  thePath,
                          const Handle(TDocStd_Document)& theDocument,
                          Handle(XSControl_WorkSession)&  theWS,
                          const Message_ProgressRange&    theProgress)
{
  (void)theWS;
  return Read(thePath, theDocument, theProgress);
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
bool DEUSD_Provider::Write(const TCollection_AsciiString&  thePath,
                           const Handle(TDocStd_Document)& theDocument,
                           Handle(XSControl_WorkSession)&  theWS,
                           const Message_ProgressRange&    theProgress)
{
  (void)theWS;
  return Write(thePath, theDocument, theProgress);
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool DEUSD_Provider::Read(const TCollection_AsciiString&  thePath,
                          const Handle(TDocStd_Document)& theDocument,
                          const Message_ProgressRange&    theProgress)
{
  if (theDocument.IsNull())
  {
    Message::SendFail() << "Error in the DEUSD_Provider during reading the file " << thePath
                        << "\t: Error: document with name does not exist\n";
    return false;
  }
  if (!GetNode()->IsKind(STANDARD_TYPE(DEUSD_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the DEUSD_Provider during reading the file " << thePath
                        << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(DEUSD_ConfigurationNode) aNode = Handle(DEUSD_ConfigurationNode)::DownCast(GetNode());
  USD_XCAFReader                  aReader;
  aReader.SetDocument(theDocument);
  aReader.SetUseScaling(aNode->InternalParameters.ReadUseScaling);
  aReader.SetReadOnlyVisible(aNode->InternalParameters.ReadOnlyVisible);
  aReader.SetSystemCoordinateSystem(aNode->InternalParameters.SystemCS);
  aReader.SetFileCoordinateSystem(aNode->InternalParameters.FileCS);
  aReader.SetSystemLengthUnit(aNode->GlobalParameters.LengthUnit / 1000);
  TDF_Label anAnimLabel;
  TDF_Tool::Label(theDocument->Main().Data(),
                  aNode->InternalParameters.AnimLabel,
                  anAnimLabel,
                  true);
  aReader.SetAnimationLabel(anAnimLabel);
  XCAFDoc_DocumentTool::SetLengthUnit(theDocument,
                                      aNode->GlobalParameters.LengthUnit,
                                      UnitsMethods_LengthUnit_Millimeter);
  if (!aReader.Perform(thePath, theProgress))
  {
    Message::SendFail() << "Error in the DEUSD_Provider during reading the file";
    return false;
  }
  return true;
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
bool DEUSD_Provider::Write(const TCollection_AsciiString&  thePath,
                           const Handle(TDocStd_Document)& theDocument,
                           const Message_ProgressRange&    theProgress)
{
  (void)theProgress;
  if (!GetNode()->IsKind(STANDARD_TYPE(DEUSD_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the DEUSD_Provider during writing the file " << thePath
                        << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(DEUSD_ConfigurationNode) aNode = Handle(DEUSD_ConfigurationNode)::DownCast(GetNode());

  USD_XCAFWriter                   aWriter;
  RWMesh_CoordinateSystemConverter aCSConverter;
  aCSConverter.SetInputCoordinateSystem(aNode->InternalParameters.SystemCS);
  aCSConverter.SetOutputCoordinateSystem(aNode->InternalParameters.FileCS);
  Standard_Real aScaleFactorMM = 1.;
  if (XCAFDoc_DocumentTool::GetLengthUnit(theDocument,
                                          aScaleFactorMM,
                                          UnitsMethods_LengthUnit_Millimeter))
  {
    aCSConverter.SetInputLengthUnit(aScaleFactorMM);
    aWriter.SetSystemLengthUnit(aScaleFactorMM / 1000.);
  }
  else
  {
    aCSConverter.SetInputLengthUnit(aNode->GlobalParameters.SystemUnit);
    aWriter.SetSystemLengthUnit(aNode->GlobalParameters.SystemUnit / 1000.);
    Message::SendWarning()
      << "Warning in the DEUSD_Provider during writing the file " << thePath
      << "\t: The document has no information on Units. Using global parameter as initial Unit.";
  }
  aCSConverter.SetOutputLengthUnit(aNode->GlobalParameters.LengthUnit);
  aWriter.SetNamingType(
    static_cast<USD_XCAFWriter::USD_NamingType>(aNode->InternalParameters.WriteNamingType));
  aWriter.SetCoordinateSystemConverter(aCSConverter);
  aWriter.SetTargetLengthUnit(aNode->GlobalParameters.LengthUnit / 1000);
  TDF_Label anAnimLabel;
  TDF_Tool::Label(theDocument->Main().Data(),
                  aNode->InternalParameters.AnimLabel,
                  anAnimLabel,
                  true);
  aWriter.SetAnimationLabel(anAnimLabel);
  (void)aNode;
  if (!aWriter.Perform(theDocument, thePath.ToCString(), theProgress))
  {
    Message::SendFail() << "Error in the DEUSD_Provider during writings the file";
    return false;
  }
  return true;
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool DEUSD_Provider::Read(const TCollection_AsciiString& thePath,
                          TopoDS_Shape&                  theShape,
                          Handle(XSControl_WorkSession)& theWS,
                          const Message_ProgressRange&   theProgress)
{
  (void)theWS;
  return Read(thePath, theShape, theProgress);
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
bool DEUSD_Provider::Write(const TCollection_AsciiString& thePath,
                           const TopoDS_Shape&            theShape,
                           Handle(XSControl_WorkSession)& theWS,
                           const Message_ProgressRange&   theProgress)
{
  (void)theWS;
  return Write(thePath, theShape, theProgress);
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool DEUSD_Provider::Read(const TCollection_AsciiString& thePath,
                          TopoDS_Shape&                  theShape,
                          const Message_ProgressRange&   theProgress)
{
  Handle(TDocStd_Document)    aDoc;
  Handle(TDocStd_Application) anApp = new TDocStd_Application();
  BinXCAFDrivers::DefineFormat(anApp);
  anApp->NewDocument("BinXCAF", aDoc);

  if (!Read(thePath, aDoc, theProgress))
  {
    return false;
  }

  TDF_LabelSequence         aLabels;
  Handle(XCAFDoc_ShapeTool) aSTool = XCAFDoc_DocumentTool::ShapeTool(aDoc->Main());
  aSTool->GetFreeShapes(aLabels);
  if (aLabels.Length() <= 0)
  {
    Message::SendFail() << "Error in the DEUSD_Provider during reading the file " << thePath
                        << "\t: Shape is empty";
    return false;
  }

  if (aLabels.Length() == 1)
  {
    theShape = aSTool->GetShape(aLabels.Value(1));
  }
  else
  {
    TopoDS_Compound aComp;
    BRep_Builder    aBuilder;
    aBuilder.MakeCompound(aComp);
    for (Standard_Integer anIndex = 1; anIndex <= aLabels.Length(); anIndex++)
    {
      TopoDS_Shape aS = aSTool->GetShape(aLabels.Value(anIndex));
      aBuilder.Add(aComp, aS);
    }
    theShape = aComp;
  }
  return true;
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
bool DEUSD_Provider::Write(const TCollection_AsciiString& thePath,
                           const TopoDS_Shape&            theShape,
                           const Message_ProgressRange&   theProgress)
{
  Handle(TDocStd_Document)  aDoc    = new TDocStd_Document("BinXCAF");
  Handle(XCAFDoc_ShapeTool) aShTool = XCAFDoc_DocumentTool::ShapeTool(aDoc->Main());
  aShTool->AddShape(theShape);
  return Write(thePath, aDoc, theProgress);
}

//=======================================================================
// function : GetFormat
// purpose  :
//=======================================================================
TCollection_AsciiString DEUSD_Provider::GetFormat() const
{
  return TCollection_AsciiString("USD");
}

//=======================================================================
// function : GetVendor
// purpose  :
//=======================================================================
TCollection_AsciiString DEUSD_Provider::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}
