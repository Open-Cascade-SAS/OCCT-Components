/// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <DEIFC_Provider.hxx>

#include <BinXCAFDrivers.hxx>
#include <BRep_Builder.hxx>
#include <IFCCAFControl_Controller.hxx>
#include <IFCCAFControl_Reader.hxx>
#include <Interface_Static.hxx>
#include <Message.hxx>
#include <TDocStd_Application.hxx>
#include <XCAFDoc_DocumentTool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DEIFC_Provider, DE_Provider)

//=======================================================================
// function : DEIFC_Provider
// purpose  :
//=======================================================================
DEIFC_Provider::DEIFC_Provider() {}

//=======================================================================
// function : DEIFC_Provider
// purpose  :
//=======================================================================
DEIFC_Provider::DEIFC_Provider(const Handle(DE_ConfigurationNode)& theNode)
    : DE_Provider(theNode)
{
}

//=======================================================================
// function : initStatic
// purpose  :
//=======================================================================
void DEIFC_Provider::initStatic(const Handle(DE_ConfigurationNode)& theNode)
{
  Handle(DEIFC_ConfigurationNode) aNode = Handle(DEIFC_ConfigurationNode)::DownCast(theNode);

  IFCCAFControl_Controller::Init();
  myOldValues.ReadAngleDeflection =
    Interface_Static::RVal("read.ifc.angledeflection") * 180.0 / M_PI;

  // Set new values
  setStatic(aNode->InternalParameters);
}

//=======================================================================
// function : setStatic
// purpose  :
//=======================================================================
void DEIFC_Provider::setStatic(
  const DEIFC_ConfigurationNode::IFCCAFControl_InternalSection theParameter)
{
  Interface_Static::SetRVal("read.ifc.angledeflection",
                            theParameter.ReadAngleDeflection * M_PI / 180.0);
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool DEIFC_Provider::Read(const TCollection_AsciiString&  thePath,
                          const Handle(TDocStd_Document)& theDocument,
                          Handle(XSControl_WorkSession)&  theWS,
                          const Message_ProgressRange&    theProgress)
{
  if (theDocument.IsNull())
  {
    Message::SendFail() << "Error in the DEIFC_Provider during reading the file " << thePath
                        << "\t: theDocument shouldn't be null";
    return false;
  }
  if (!GetNode()->IsKind(STANDARD_TYPE(DEIFC_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the DEIFC_Provider during reading the file " << thePath
                        << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(DEIFC_ConfigurationNode) aNode = Handle(DEIFC_ConfigurationNode)::DownCast(GetNode());
  initStatic(aNode);
  XCAFDoc_DocumentTool::SetLengthUnit(theDocument,
                                      aNode->GlobalParameters.LengthUnit,
                                      UnitsMethods_LengthUnit_Millimeter);
  IFCCAFControl_Reader aReader;
  if (!theWS.IsNull())
  {
    aReader.SetWS(theWS);
  }
  aReader.SetDiscardProjectTransformation(aNode->InternalParameters.ReadDiscardProjTrsf);
  aReader.SetExpandIfcProductRepresentation(aNode->InternalParameters.ReadExpandProdRepr);
  aReader.SetUseTypeAsName(aNode->InternalParameters.ReadUseTypeAsName);

  // Disable auto naming
  Standard_Boolean anAutoNaming = XCAFDoc_ShapeTool::AutoNaming();
  XCAFDoc_ShapeTool::SetAutoNaming(false);

  if (!aReader.Perform(thePath.ToCString(), theDocument, theProgress))
  {
    Message::SendFail() << "Error in the DEIFC_Provider during reading the file " << thePath;
    setStatic(myOldValues);
    return false;
  }

  // Restore the auto naming option
  XCAFDoc_ShapeTool::SetAutoNaming(anAutoNaming);
  setStatic(myOldValues);
  return true;
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool DEIFC_Provider::Read(const TCollection_AsciiString&  thePath,
                          const Handle(TDocStd_Document)& theDocument,
                          const Message_ProgressRange&    theProgress)
{
  Handle(XSControl_WorkSession) aWS;
  return Read(thePath, theDocument, aWS, theProgress);
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool DEIFC_Provider::Read(const TCollection_AsciiString& thePath,
                          TopoDS_Shape&                  theShape,
                          Handle(XSControl_WorkSession)& theWS,
                          const Message_ProgressRange&   theProgress)
{
  if (!GetNode()->IsKind(STANDARD_TYPE(DEIFC_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the DEIFC_Provider during reading the file " << thePath
                        << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(DEIFC_ConfigurationNode) aNode = Handle(DEIFC_ConfigurationNode)::DownCast(GetNode());
  initStatic(aNode);

  Handle(TDocStd_Document) aDoc = new TDocStd_Document("BinXCAF");
  if (!Read(thePath, aDoc, theWS, theProgress))
  {
    return false;
  }
  TDF_LabelSequence         aLabels;
  Handle(XCAFDoc_ShapeTool) aSTool = XCAFDoc_DocumentTool::ShapeTool(aDoc->Main());
  aSTool->GetFreeShapes(aLabels);
  if (aLabels.Length() <= 0)
  {
    Message::SendFail() << "Error in the DEIFC_Provider during reading the file " << thePath
                        << "\t: Shape is empty";
    setStatic(myOldValues);
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
  setStatic(myOldValues);
  return true;
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool DEIFC_Provider::Read(const TCollection_AsciiString& thePath,
                          TopoDS_Shape&                  theShape,
                          const Message_ProgressRange&   theProgress)
{
  Handle(XSControl_WorkSession) aWS;
  return Read(thePath, theShape, aWS, theProgress);
}

//=======================================================================
// function : GetFormat
// purpose  :
//=======================================================================
TCollection_AsciiString DEIFC_Provider::GetFormat() const
{
  return TCollection_AsciiString("IFC");
}

//=======================================================================
// function : GetVendor
// purpose  :
//=======================================================================
TCollection_AsciiString DEIFC_Provider::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}
