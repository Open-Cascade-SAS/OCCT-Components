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

#include <DEXT_Provider.hxx>

#include <BinXCAFDrivers.hxx>
#include <BRep_Builder.hxx>
#include <DE_ConfigurationNode.hxx>
#include <Interface_Static.hxx>
#include <Message.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XSControl_WorkSession.hxx>
#include <DEXT_ConfigurationNode.hxx>
#include <XtCAFControl_Reader.hxx>
#include <XtControl_Controller.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DEXT_Provider, DE_Provider)

//=======================================================================
// function : DEXT_Provider
// purpose  :
//=======================================================================
DEXT_Provider::DEXT_Provider() {}

//=======================================================================
// function : DEXT_Provider
// purpose  :
//=======================================================================
DEXT_Provider::DEXT_Provider(const Handle(DE_ConfigurationNode)& theNode)
    : DE_Provider(theNode)
{
}

//=======================================================================
// function : initStatic
// purpose  :
//=======================================================================
void DEXT_Provider::initStatic(const Handle(DE_ConfigurationNode)& theNode)
{
  Handle(DEXT_ConfigurationNode) aNode = Handle(DEXT_ConfigurationNode)::DownCast(theNode);
  XtControl_Controller::Init();
  myOldValues.ReadResourceName = Interface_Static::CVal("read.xt.resource.name");
  myOldValues.ReadSequence     = Interface_Static::CVal("read.xt.sequence");

  // Set new values
  setStatic(aNode->InternalParameters);
}

//=======================================================================
// function : setStatic
// purpose  :
//=======================================================================
void DEXT_Provider::setStatic(
  const DEXT_ConfigurationNode::XTCAFControl_InternalSection theParameter)
{
  Interface_Static::SetCVal("read.xt.resource.name", theParameter.ReadResourceName.ToCString());
  Interface_Static::SetCVal("read.xt.sequence", theParameter.ReadSequence.ToCString());
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool DEXT_Provider::Read(const TCollection_AsciiString&  thePath,
                         const Handle(TDocStd_Document)& theDocument,
                         Handle(XSControl_WorkSession)&  theWS,
                         const Message_ProgressRange&    theProgress)
{
  if (theDocument.IsNull())
  {
    Message::SendFail() << "Error in the DEXT_Provider during reading the file " << thePath
                        << "\t: theDocument shouldn't be null";
    return false;
  }
  if (!GetNode()->IsKind(STANDARD_TYPE(DEXT_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the DEXT_Provider during reading the file " << thePath
                        << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(DEXT_ConfigurationNode) aNode = Handle(DEXT_ConfigurationNode)::DownCast(GetNode());
  initStatic(aNode);
  personizeWS(theWS);
  XCAFDoc_DocumentTool::SetLengthUnit(theDocument,
                                      aNode->GlobalParameters.LengthUnit,
                                      UnitsMethods_LengthUnit_Millimeter);
  XtCAFControl_Reader aReader;
  aReader.SetWS(theWS);

  aReader.SetColorMode(aNode->InternalParameters.ReadColorMode);
  aReader.SetNameMode(aNode->InternalParameters.ReadNameMode);
  IFSelect_ReturnStatus aReadStat = IFSelect_RetVoid;
  aReadStat                       = aReader.ReadFile(thePath.ToCString());
  if (aReadStat != IFSelect_RetDone)
  {
    Message::SendFail() << "Error in the DEXT_Provider during reading the file " << thePath
                        << "\t: abandon, no model loaded";
    setStatic(myOldValues);
    return false;
  }

  if (!aReader.Transfer(theDocument, theProgress))
  {
    Message::SendFail() << "Error in the DEXT_Provider during reading the file " << thePath
                        << "\t: Cannot read any relevant data from the XT file";
    setStatic(myOldValues);
    return false;
  }
  setStatic(myOldValues);
  return true;
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool DEXT_Provider::Read(const TCollection_AsciiString&  thePath,
                         const Handle(TDocStd_Document)& theDocument,
                         const Message_ProgressRange&    theProgress)
{
  Handle(XSControl_WorkSession) aWS = new XSControl_WorkSession();
  return Read(thePath, theDocument, aWS, theProgress);
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool DEXT_Provider::Read(const TCollection_AsciiString& thePath,
                         TopoDS_Shape&                  theShape,
                         Handle(XSControl_WorkSession)& theWS,
                         const Message_ProgressRange&   theProgress)
{
  (void)theProgress;
  if (!GetNode()->IsKind(STANDARD_TYPE(DEXT_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the DEXT_Provider during reading the file " << thePath
                        << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(DEXT_ConfigurationNode) aNode = Handle(DEXT_ConfigurationNode)::DownCast(GetNode());
  initStatic(aNode);
  personizeWS(theWS);
  XtControl_Reader aReader;
  aReader.SetWS(theWS);

  IFSelect_ReturnStatus aReadStat = IFSelect_RetVoid;
  aReadStat = aReader.ReadFile(thePath.ToCString(), 1000. / aNode->GlobalParameters.LengthUnit);
  if (aReadStat != IFSelect_RetDone)
  {
    Message::SendFail() << "Error in the DEXT_Provider during reading the file " << thePath
                        << "\t: abandon, no model loaded";
    setStatic(myOldValues);
    return false;
  }
  if (aReader.TransferRoots() <= 0)
  {
    Message::SendFail() << "Error in the DEXT_Provider during reading the file " << thePath
                        << "\t: Cannot read any relevant data from the IGES file";
    setStatic(myOldValues);
    return false;
  }
  theShape = aReader.OneShape();
  setStatic(myOldValues);
  return true;
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool DEXT_Provider::Read(const TCollection_AsciiString& thePath,
                         TopoDS_Shape&                  theShape,
                         const Message_ProgressRange&   theProgress)
{
  Handle(XSControl_WorkSession) aWS = new XSControl_WorkSession();
  return Read(thePath, theShape, aWS, theProgress);
}

//=======================================================================
// function : GetFormat
// purpose  :
//=======================================================================
TCollection_AsciiString DEXT_Provider::GetFormat() const
{
  return TCollection_AsciiString("XT");
}

//=======================================================================
// function : GetVendor
// purpose  :
//=======================================================================
TCollection_AsciiString DEXT_Provider::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}

//=======================================================================
// function : personizeWS
// purpose  :
//=======================================================================
void DEXT_Provider::personizeWS(Handle(XSControl_WorkSession)& theWS)
{
  if (theWS.IsNull())
  {
    Message::SendWarning() << "Warning: DEXT_Provider :"
                           << " Null work session, use internal temporary session";
    theWS = new XSControl_WorkSession();
  }
  Handle(XtControl_Controller) aCntrl =
    Handle(XtControl_Controller)::DownCast(theWS->NormAdaptor());
  if (aCntrl.IsNull())
  {
    theWS->SelectNorm("Xt");
  }
}
