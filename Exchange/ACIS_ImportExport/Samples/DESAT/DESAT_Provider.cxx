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

#include <DESAT_Provider.hxx>

#include <AcisData.hxx>
#include <AcisData_ConfParameters.hxx>
#include <BinXCAFDrivers.hxx>
#include <BRep_Builder.hxx>
#include <DE_ConfigurationNode.hxx>
#include <Interface_Static.hxx>
#include <Message.hxx>
#include <DESAT_ConfigurationNode.hxx>
#include <SATCAFControl_Reader.hxx>
#include <SATCAFControl_Writer.hxx>
#include <SatControl_Controller.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XSControl_WorkSession.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DESAT_Provider, DE_Provider)

//=======================================================================
// function : DESAT_Provider
// purpose  :
//=======================================================================
DESAT_Provider::DESAT_Provider() {}

//=======================================================================
// function : DESAT_Provider
// purpose  :
//=======================================================================
DESAT_Provider::DESAT_Provider(const Handle(DE_ConfigurationNode)& theNode)
    : DE_Provider(theNode)
{
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool DESAT_Provider::Read(const TCollection_AsciiString&  thePath,
                          const Handle(TDocStd_Document)& theDocument,
                          Handle(XSControl_WorkSession)&  theWS,
                          const Message_ProgressRange&    theProgress)
{
  if (theDocument.IsNull())
  {
    Message::SendFail() << "Error in the DESAT_Provider during reading the file " << thePath
                        << "\t: theDocument shouldn't be null";
    return false;
  }
  if (!GetNode()->IsKind(STANDARD_TYPE(DESAT_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the DESAT_Provider during reading the file " << thePath
                        << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(DESAT_ConfigurationNode) aNode = Handle(DESAT_ConfigurationNode)::DownCast(GetNode());

  personizeWS(theWS);
  SATCAFControl_Reader aReader;
  aReader.SetWS(theWS);

  aReader.SetColorMode(aNode->InternalParameters.ReadColorMode);
  aReader.SetNameMode(aNode->InternalParameters.ReadNameMode);

  IFSelect_ReturnStatus   aReadStat = IFSelect_RetVoid;
  AcisData_ConfParameters aParameters;
  aReadStat = aReader.ReadFile(thePath.ToCString(), aParameters);
  if (aReadStat != IFSelect_RetDone)
  {
    Message::SendFail() << "Error in the DESAT_Provider during reading the file " << thePath
                        << "\t: abandon, no model loaded";
    aNode->InternalParameters.Reset();
    return false;
  }

  aReader.ClearShapes();
  aReader.NbRootsForTransfer(); // to initialize list of roots
  if (!aReader.Transfer(theDocument, theProgress))
  {
    Message::SendFail() << "Error in the DESAT_Provider during reading the file " << thePath
                        << "\t: Cannot read any relevant data from the SAT file";
    aNode->InternalParameters.Reset();
    return false;
  }
  aNode->InternalParameters.Reset();
  return true;
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
bool DESAT_Provider::Write(const TCollection_AsciiString&  thePath,
                           const Handle(TDocStd_Document)& theDocument,
                           Handle(XSControl_WorkSession)&  theWS,
                           const Message_ProgressRange&    theProgress)
{
  if (!GetNode()->IsKind(STANDARD_TYPE(DESAT_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the DESAT_Provider during reading the file " << thePath
                        << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(DESAT_ConfigurationNode) aNode = Handle(DESAT_ConfigurationNode)::DownCast(GetNode());
  if (aNode->GlobalParameters.LengthUnit != 1.0)
  {
    Message::SendWarning()
      << "Warning in the DESAT_Provider during writing the file " << thePath
      << "\t: Target Units for writing were changed, but current format doesn't support scaling";
  }
  personizeWS(theWS);
  SATCAFControl_Writer aWriter;
  aWriter.SetWS(theWS);
  aWriter.SetColorMode(aNode->InternalParameters.WriteColorMode);
  aWriter.SetNameMode(aNode->InternalParameters.WriteNameMode);
  AcisData_ConfParameters aParameters;
  if (!aWriter.Transfer(theDocument, aParameters, theProgress))
  {
    Message::SendFail() << "Error in the DESAT_Provider during writing the file " << thePath
                        << "\t: Cannot write any relevant data to the SAT file";
    aNode->InternalParameters.Reset();
    return false;
  }
  if (!aWriter.WriteFile(thePath.ToCString()))
  {
    Message::SendFail() << "Error in the DESAT_Provider during writing the file " << thePath;
    aNode->InternalParameters.Reset();
    return false;
  }
  aNode->InternalParameters.Reset();
  return true;
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool DESAT_Provider::Read(const TCollection_AsciiString&  thePath,
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
bool DESAT_Provider::Write(const TCollection_AsciiString&  thePath,
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
bool DESAT_Provider::Read(const TCollection_AsciiString& thePath,
                          TopoDS_Shape&                  theShape,
                          Handle(XSControl_WorkSession)& theWS,
                          const Message_ProgressRange&   theProgress)
{
  (void)theProgress;
  if (!GetNode()->IsKind(STANDARD_TYPE(DESAT_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the DESAT_Provider during reading the file " << thePath
                        << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(DESAT_ConfigurationNode) aNode = Handle(DESAT_ConfigurationNode)::DownCast(GetNode());
  personizeWS(theWS);
  SatControl_Reader aReader;
  aReader.SetWS(theWS);

  IFSelect_ReturnStatus   aReadStat = IFSelect_RetVoid;
  AcisData_ConfParameters aParameters;
  aReadStat = aReader.ReadFile(thePath.ToCString(), aParameters);
  if (aReadStat != IFSelect_RetDone)
  {
    Message::SendFail() << "Error in the DESAT_Provider during reading the file " << thePath
                        << "\t: abandon, no model loaded";
    aNode->InternalParameters.Reset();
    return false;
  }
  if (aReader.TransferRoots() <= 0)
  {
    Message::SendFail() << "Error in the DESAT_Provider during reading the file " << thePath
                        << "\t: Cannot read any relevant data from the IGES file";
    aNode->InternalParameters.Reset();
    return false;
  }
  theShape = aReader.OneShape();

  aNode->InternalParameters.Reset();
  return true;
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
bool DESAT_Provider::Write(const TCollection_AsciiString& thePath,
                           const TopoDS_Shape&            theShape,
                           Handle(XSControl_WorkSession)& theWS,
                           const Message_ProgressRange&   theProgress)
{
  (void)theProgress;
  personizeWS(theWS);
  SatControl_Writer aWriter;
  aWriter.SetWS(theWS);

  Handle(DESAT_ConfigurationNode) aNode = Handle(DESAT_ConfigurationNode)::DownCast(GetNode());
  if (aNode->GlobalParameters.LengthUnit != 1.0)
  {
    Message::SendWarning()
      << "Warning in the DESAT_Provider during writing the file " << thePath
      << "\t: Target Units for writing were changed, but current format doesn't support scaling";
  }
  AcisData_ConfParameters aParameters;
  IFSelect_ReturnStatus   aStat = aWriter.TransferShape(theShape, aParameters);
  aStat                         = aWriter.WriteFile(thePath.ToCString());
  if (aStat != IFSelect_RetDone)
  {
    Message::SendFail() << "IGESCAFControl_Provider: Error on writing file " << thePath;
    return false;
  }
  return true;
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool DESAT_Provider::Read(const TCollection_AsciiString& thePath,
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
bool DESAT_Provider::Write(const TCollection_AsciiString& thePath,
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
TCollection_AsciiString DESAT_Provider::GetFormat() const
{
  return TCollection_AsciiString("SAT");
}

//=======================================================================
// function : GetVendor
// purpose  :
//=======================================================================
TCollection_AsciiString DESAT_Provider::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}

//=======================================================================
// function : personizeWS
// purpose  :
//=======================================================================
void DESAT_Provider::personizeWS(Handle(XSControl_WorkSession)& theWS)
{
  if (theWS.IsNull())
  {
    Message::SendWarning() << "Warning: DESAT_Provider :"
                           << " Null work session, use internal temporary session";
    theWS = new XSControl_WorkSession();
  }
  SatControl_Controller::Init();
  Handle(SatControl_Controller) aCntrl =
    Handle(SatControl_Controller)::DownCast(theWS->NormAdaptor());
  if (aCntrl.IsNull())
  {
    theWS->SelectNorm("SAT");
  }
}
