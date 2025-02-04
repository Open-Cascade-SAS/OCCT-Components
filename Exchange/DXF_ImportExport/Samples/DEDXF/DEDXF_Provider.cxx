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

#include <DEDXF_Provider.hxx>

#include <BinXCAFDrivers.hxx>
#include <DXFCAFControl_Reader.hxx>
#include <DXFCAFControl_Writer.hxx>
#include <DxfControl_Controller.hxx>
#include <DxfFile.hxx>
#include <DxfSection_ConfParameters.hxx>
#include <DxfSection_Model.hxx>
#include <Message.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XSControl_WorkSession.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DEDXF_Provider, DE_Provider)

//=======================================================================
// function : DEDXF_Provider
// purpose  :
//=======================================================================
DEDXF_Provider::DEDXF_Provider() {}

//=======================================================================
// function : DEDXF_Provider
// purpose  :
//=======================================================================
DEDXF_Provider::DEDXF_Provider(const Handle(DE_ConfigurationNode)& theNode)
    : DE_Provider(theNode)
{
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool DEDXF_Provider::Read(const TCollection_AsciiString&  thePath,
                          const Handle(TDocStd_Document)& theDocument,
                          Handle(XSControl_WorkSession)&  theWS,
                          const Message_ProgressRange&    theProgress)
{
  if (theDocument.IsNull())
  {
    Message::SendFail() << "Error in the DEDXF_Provider during reading the file " << thePath
                        << "\t: theDocument shouldn't be null";
    return false;
  }
  if (!GetNode()->IsKind(STANDARD_TYPE(DEDXF_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the DEDXF_Provider during reading the file " << thePath
                        << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(DEDXF_ConfigurationNode) aNode = Handle(DEDXF_ConfigurationNode)::DownCast(GetNode());
  personizeWS(theWS);
  XCAFDoc_DocumentTool::SetLengthUnit(theDocument,
                                      aNode->GlobalParameters.LengthUnit,
                                      UnitsMethods_LengthUnit_Millimeter);
  DXFCAFControl_Reader aReader;
  aReader.SetWS(theWS);
  aReader.SetColorMode(aNode->InternalParameters.ReadColorMode);
  aReader.SetNameMode(aNode->InternalParameters.WriteColorMode);
  IFSelect_ReturnStatus aReadStat = IFSelect_RetVoid;
  aReadStat = aReader.ReadFile(thePath.ToCString(), aNode->InternalParameters);
  if (aReadStat != IFSelect_RetDone)
  {
    Message::SendFail() << "Error in the DEDXF_Provider during reading the file " << thePath
                        << "\t: abandon, no model loaded";
    return false;
  }
  aReader.ClearShapes();
  aReader.NbRootsForTransfer();
  if (!aReader.Transfer(theDocument, theProgress))
  {
    Message::SendFail() << "Error in the DEDXF_Provider during reading the file " << thePath
                        << "\t: Cannot read any relevant data from the DXF file";
    return false;
  }
  return true;
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
bool DEDXF_Provider::Write(const TCollection_AsciiString&  thePath,
                           const Handle(TDocStd_Document)& theDocument,
                           Handle(XSControl_WorkSession)&  theWS,
                           const Message_ProgressRange&    theProgress)
{
  (void)theProgress;
  if (!GetNode()->IsKind(STANDARD_TYPE(DEDXF_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the DEDXF_Provider during reading the file " << thePath
                        << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(DEDXF_ConfigurationNode) aNode = Handle(DEDXF_ConfigurationNode)::DownCast(GetNode());
  personizeWS(theWS);
  if (aNode->GlobalParameters.LengthUnit != 1.0)
  {
    Message::SendWarning()
      << "Warning in the DEDXF_Provider during writing the file " << thePath
      << "\t: Target Units for writing were changed, but writer doesn't currently support scaling";
  }
  DXFCAFControl_Writer aWriter;
  aWriter.SetWS(theWS);
  Handle(DxfSection_Model) aDxfModel = Handle(DxfSection_Model)::DownCast(aWriter.Model());
  aWriter.SetColorMode(aNode->InternalParameters.WriteColorMode);
  aWriter.SetNameMode(aNode->InternalParameters.WriteNameMode);
  DxfSection_ConfParameters aParameters;
  if (!aWriter.Transfer(theDocument, aParameters))
  {
    Message::SendFail() << "Error in the DEDXF_Provider during writing the file " << thePath
                        << "\t: Transfer error";
    aDxfModel->GetConfParameters().Reset();
    return false;
  }
  if (!aWriter.WriteFile(thePath.ToCString()))
  {
    Message::SendFail() << "Error in the DEDXF_Provider during writing the file " << thePath
                        << "\t: Writing error";
    aDxfModel->GetConfParameters().Reset();
    return false;
  }
  aDxfModel->GetConfParameters().Reset();
  return true;
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool DEDXF_Provider::Read(const TCollection_AsciiString&  thePath,
                          const Handle(TDocStd_Document)& theDocument,
                          const Message_ProgressRange&    theProgress)
{
  Handle(XSControl_WorkSession) aWS = new XSControl_WorkSession();
  return Read(thePath, theDocument, aWS, theProgress);
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
bool DEDXF_Provider::Write(const TCollection_AsciiString&  thePath,
                           const Handle(TDocStd_Document)& theDocument,
                           const Message_ProgressRange&    theProgress)
{
  Handle(XSControl_WorkSession) aWS = new XSControl_WorkSession();
  return Write(thePath, theDocument, aWS, theProgress);
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool DEDXF_Provider::Read(const TCollection_AsciiString& thePath,
                          TopoDS_Shape&                  theShape,
                          Handle(XSControl_WorkSession)& theWS,
                          const Message_ProgressRange&   theProgress)
{
  (void)theProgress;
  if (!GetNode()->IsKind(STANDARD_TYPE(DEDXF_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the DEDXF_Provider during reading the file " << thePath
                        << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(DEDXF_ConfigurationNode) aNode = Handle(DEDXF_ConfigurationNode)::DownCast(GetNode());
  personizeWS(theWS);
  DxfControl_Reader aReader;
  aReader.SetWS(theWS);
  IFSelect_ReturnStatus    aStat = aReader.ReadFile(thePath.ToCString(), aNode->InternalParameters);
  Handle(DxfSection_Model) aDxfModel = Handle(DxfSection_Model)::DownCast(aReader.Model());
  switch (aStat)
  {
    case IFSelect_RetVoid: {
      Message::SendFail() << "Error in the DEDXF_Provider during reading the file " << thePath
                          << "\t: empty file";
      return false;
    }
    case IFSelect_RetDone: {
      break;
    }
    case IFSelect_RetError: {
      Message::SendFail() << "Error in the DEDXF_Provider during reading the file " << thePath
                          << "\t: file not found";
      aDxfModel->GetConfParameters().Reset();
      return false;
    }
    case IFSelect_RetFail: {
      Message::SendFail() << "Error in the DEDXF_Provider during reading the file " << thePath
                          << "\t: error during read";
      aDxfModel->GetConfParameters().Reset();
      return false;
    }
    default: {
      Message::SendFail() << "Error in the DEDXF_Provider during reading the file " << thePath
                          << "\t: failure";
      aDxfModel->GetConfParameters().Reset();
      return false;
    }
  }

  // Set target units for Unit Scaling. Initial units are taken from the file if present or
  // set to millimeters otherwise.
  aDxfModel->SetSystemLengthUnit(aNode->GlobalParameters.LengthUnit);
  if (aReader.TransferRoots() <= 0)
  {
    Message::SendFail() << "Error in the DEDXF_Provider during reading the file " << thePath
                        << "\t: Cannot read any relevant data from the DXF file";
    aDxfModel->GetConfParameters().Reset();
    return false;
  }
  theShape = aReader.OneShape();
  aDxfModel->GetConfParameters().Reset();
  return true;
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
bool DEDXF_Provider::Write(const TCollection_AsciiString& thePath,
                           const TopoDS_Shape&            theShape,
                           Handle(XSControl_WorkSession)& theWS,
                           const Message_ProgressRange&   theProgress)
{
  (void)theProgress;
  personizeWS(theWS);
  DxfControl_Writer aWriter;
  aWriter.SetWS(theWS);

  Handle(DEDXF_ConfigurationNode) aNode = Handle(DEDXF_ConfigurationNode)::DownCast(GetNode());
  if (aNode->GlobalParameters.LengthUnit != 1.0)
  {
    Message::SendWarning()
      << "Warning in the DEDXF_Provider during writing the file " << thePath
      << "\t: Target Units for writing were changed, but writer doesn't currently support scaling";
  }

  Handle(DxfSection_Model)  aDxfModel = Handle(DxfSection_Model)::DownCast(aWriter.Model());
  DxfSection_ConfParameters aParameters;
  IFSelect_ReturnStatus     aStat = aWriter.TransferShape(theShape, aParameters);
  aStat                           = aWriter.WriteFile(thePath.ToCString());
  if (aStat != IFSelect_RetDone)
  {
    Message::SendFail() << "Error in the DEDXF_Provider during writing the file " << thePath;
    aDxfModel->GetConfParameters().Reset();
    return false;
  }
  aDxfModel->GetConfParameters().Reset();
  return true;
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool DEDXF_Provider::Read(const TCollection_AsciiString& thePath,
                          TopoDS_Shape&                  theShape,
                          const Message_ProgressRange&   theProgress)
{
  Handle(XSControl_WorkSession) aWS = new XSControl_WorkSession();
  return Read(thePath, theShape, aWS, theProgress);
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
bool DEDXF_Provider::Write(const TCollection_AsciiString& thePath,
                           const TopoDS_Shape&            theShape,
                           const Message_ProgressRange&   theProgress)
{
  Handle(XSControl_WorkSession) aWS = new XSControl_WorkSession();
  return Write(thePath, theShape, aWS, theProgress);
}

//=======================================================================
// function : GetFormat
// purpose  :
//=======================================================================
TCollection_AsciiString DEDXF_Provider::GetFormat() const
{
  return TCollection_AsciiString("DXF");
}

//=======================================================================
// function : GetVendor
// purpose  :
//=======================================================================
TCollection_AsciiString DEDXF_Provider::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}

//=======================================================================
// function : personizeWS
// purpose  :
//=======================================================================
void DEDXF_Provider::personizeWS(Handle(XSControl_WorkSession)& theWS)
{
  if (theWS.IsNull())
  {
    Message::SendWarning() << "Warning: DEDXF_Provider :"
                           << " Null work session, use internal temporary session";
    theWS = new XSControl_WorkSession();
  }
  DxfControl_Controller::Init();
  Handle(DxfControl_Controller) aCntrl =
    Handle(DxfControl_Controller)::DownCast(theWS->NormAdaptor());
  if (aCntrl.IsNull())
  {
    theWS->SelectNorm("DXF");
  }
}
