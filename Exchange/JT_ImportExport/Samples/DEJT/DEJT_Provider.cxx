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

#include <DEJT_Provider.hxx>

#include <BinXCAFDrivers.hxx>
#include <BRep_Builder.hxx>
#include <DEJT_ConfigurationNode.hxx>
#include <JtTools_FileDumpBRepProvider.hxx>
#include <JtWireframe_BRepProvider.hxx>
#include <JTCAFControl_Reader.hxx>
#include <JtNode_Partition.hxx>
#include <TDocStd_Application.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DEJT_Provider, DE_Provider)

//=======================================================================
// function : DEJT_Provider
// purpose  :
//=======================================================================
DEJT_Provider::DEJT_Provider() {}

//=======================================================================
// function : DEJT_Provider
// purpose  :
//=======================================================================
DEJT_Provider::DEJT_Provider(const Handle(DE_ConfigurationNode)& theNode)
    : DE_Provider(theNode)
{
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool DEJT_Provider::Read(const TCollection_AsciiString&  thePath,
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
bool DEJT_Provider::Write(const TCollection_AsciiString&  thePath,
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
bool DEJT_Provider::Read(const TCollection_AsciiString&  thePath,
                         const Handle(TDocStd_Document)& theDocument,
                         const Message_ProgressRange&    theProgress)
{
  if (theDocument.IsNull())
  {
    Message::SendFail() << "Error in the DEJT_Provider during reading the file " << thePath
                        << "\t: Error: document with name does not exist\n";
    return false;
  }
  if (!GetNode()->IsKind(STANDARD_TYPE(DEJT_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the DEJT_Provider during reading the file " << thePath
                        << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(DEJT_ConfigurationNode) aNode = Handle(DEJT_ConfigurationNode)::DownCast(GetNode());

  JTCAFControl_Representation aPrefGeomRepr =
    static_cast<JTCAFControl_Representation>(aNode->InternalParameters.ReadPrefGeomRepr);
  Standard_Real                 aLengthUnitM = aNode->GlobalParameters.LengthUnit / 1000;
  Handle(JtTools_IBRepProvider) aDumpBRepProvider;
  if (!aNode->InternalParameters.ReadDumpBRepProvider.IsEmpty())
  {
    aDumpBRepProvider =
      new JtTools_FileDumpBRepProvider(aNode->InternalParameters.ReadDumpBRepProvider);
  }

  {
    JTCAFControl_Reader aReader;
    aReader.SetParallel(aNode->InternalParameters.ReadParallel);
    aReader.SetTriangulationLod(aNode->InternalParameters.ReadLodIndex);
    aReader.SetSkipDegenerateTriangles(aNode->InternalParameters.ReadSkipDegenerateTriangles);
    aReader.SetDeduplicatePrimitives(aNode->InternalParameters.ReadDeduplicatePrims);
    aReader.SetDeferMetadata(aNode->InternalParameters.ReadDeferMetadata);
    aReader.SetCacheFileContent(aNode->InternalParameters.ReadCacheFileContent);
    aReader.SetPreferredRepresentation(aPrefGeomRepr);
    aReader.SetLoadHiddenMetadata(aNode->InternalParameters.ReadHiddenMetadata);
    aReader.SetToSkipLateDataLoading(aNode->InternalParameters.ReadSkipLateDataLoading);
    aReader.SetToKeepLateData(aNode->InternalParameters.ReadKeepLateData);
    aReader.SetToLoadStrictly(aNode->InternalParameters.ReadLoadStrictly);
    aReader.SetToPrintDebugMessages(aNode->InternalParameters.ReadPrintDebugInfo);
    aReader.SetFilterLayers(aNode->InternalParameters.ReadFilterLayers);
    aReader.SetTranslateMetadata(aNode->InternalParameters.ReadTranslateMetadata);
    aReader.SetTranslatePMI(aNode->InternalParameters.ReadTranslatePMI);
    aReader.SetCheckBRepTypes(aNode->InternalParameters.ReadCheckBRepTypes);

    if (!aDumpBRepProvider.IsNull())
    {
      aReader.SetBRepProvider(aDumpBRepProvider);
    }
    else
    {
      const Handle(JtTools_IBRepProvider)& aXTBRepProvider = aNode->GetProviderXt();
      if (aNode->InternalParameters.ReadXt && !aXTBRepProvider.IsNull())
      {
        if (!aNode->IsImportXTSupported())
        {
          Message::SendFail() << "Error in the DEJT_Provider during reading the file " << thePath
                              << "\t: Error: no XT license detected";
          return false;
        }
        aReader.SetBRepProvider(aXTBRepProvider, JtData_SegmentType_XtBRep);
        aReader.SetBRepProvider(aXTBRepProvider, JtData_SegmentType_MultiXtBRep);
      }
      if (aNode->InternalParameters.ReadWireframe)
      {
        Handle(JtTools_IBRepProvider) aWFP = new JtWireframe_BRepProvider();
        aReader.SetBRepProvider(aWFP, JtData_SegmentType_Wireframe);
      }
    }

    aReader.SetSystemLengthUnit(aLengthUnitM);
    if (!aReader.Perform(theDocument, thePath.ToCString(), theProgress))
    {
      Message::SendFail() << "Error in the DEJT_Provider during reading the file " << thePath
                          << "\t: Error: file reading failed";
      return false;
    }

    if (aNode->InternalParameters.ReadLodIndex >= aReader.NbTriangulationLods())
    {
      Message::SendWarning() << "Error in the DEJT_Provider during reading the file " << thePath
                             << "\t: Warning: JT file does not define LOD "
                             << aNode->InternalParameters.ReadLodIndex;
    }
  }

  return true;
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
bool DEJT_Provider::Write(const TCollection_AsciiString&  thePath,
                          const Handle(TDocStd_Document)& theDocument,
                          const Message_ProgressRange&    theProgress)
{
  if (!GetNode()->IsKind(STANDARD_TYPE(DEJT_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the DEJT_Provider during writing the file " << thePath
                        << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(DEJT_ConfigurationNode) aNode = Handle(DEJT_ConfigurationNode)::DownCast(GetNode());

  JtData_Array<IMeshTools_Parameters> aLODParameters;
  JtData_LengthUnit                   aJtLengthUnit = aNode->InternalParameters.WriteJTLengthUnit;
  Standard_Real           aCafLengthUnitM = JtData_Model::GetJtLengthUnitScale(aJtLengthUnit);
  JTCAFControl_NameFormat aNameFormat =
    static_cast<JTCAFControl_NameFormat>(aNode->InternalParameters.WriteNameFormat);
  // Create and save the JT model.
  Jt_GUID::Value(Standard_True);
  JTCAFControl_XcafToJT          aConverter;
  const Handle(JtNode_Partition) aJTModelStructure =
    aConverter.Convert(theDocument, 1, aLODParameters, aNameFormat, aCafLengthUnitM, aJtLengthUnit);

  Handle(JtData_Model) aJTModel = new JtData_Model(thePath.ToCString());
  if (!JtData_Model::Store(aJTModel, aJTModelStructure, false, theProgress))
  {
    Message::SendFail() << "Error in the DEJT_Provider during writing the file " << thePath
                        << "\t: Error: failed to write file";
    return false;
  }
  return true;
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool DEJT_Provider::Read(const TCollection_AsciiString& thePath,
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
bool DEJT_Provider::Write(const TCollection_AsciiString& thePath,
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
bool DEJT_Provider::Read(const TCollection_AsciiString& thePath,
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
    Message::SendFail() << "Error in the DEJT_Provider during reading the file " << thePath
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
bool DEJT_Provider::Write(const TCollection_AsciiString& thePath,
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
TCollection_AsciiString DEJT_Provider::GetFormat() const
{
  return TCollection_AsciiString("JT");
}

//=======================================================================
// function : GetVendor
// purpose  :
//=======================================================================
TCollection_AsciiString DEJT_Provider::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}
