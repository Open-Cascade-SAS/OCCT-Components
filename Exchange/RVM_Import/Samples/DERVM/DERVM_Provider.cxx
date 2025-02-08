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

#include <DERVM_Provider.hxx>

#include <BinXCAFDrivers.hxx>
#include <BRep_Builder.hxx>
#include <DE_ConfigurationNode.hxx>
#include <Message.hxx>
#include <OSD_File.hxx>
#include <OSD_FileIterator.hxx>
#include <DERVM_ConfigurationNode.hxx>
#include <RVMCAFControl_Reader.hxx>
#include <UnitsMethods.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DERVM_Provider, DE_Provider)

namespace
{
static bool IsDirectory(const std::string& thePath)
{
  OSD_File aFile(OSD_Path(thePath.c_str()));

  return aFile.KindOfFile() == OSD_KindFile::OSD_DIRECTORY;
}

//! Get Files with theExtension from directory thePath.
//! Return files with absolute path.
static std::vector<std::string> DirectoryFiles(const std::string& thePath, const char* theExtension)
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
// function : DERVM_Provider
// purpose  :
//=======================================================================
DERVM_Provider::DERVM_Provider() {}

//=======================================================================
// function : DERVM_Provider
// purpose  :
//=======================================================================
DERVM_Provider::DERVM_Provider(const Handle(DE_ConfigurationNode)& theNode)
    : DE_Provider(theNode)
{
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool DERVM_Provider::Read(const TCollection_AsciiString&  thePath,
                          const Handle(TDocStd_Document)& theDocument,
                          Handle(XSControl_WorkSession)&  theWS,
                          const Message_ProgressRange&    theProgress)
{
  if (theDocument.IsNull())
  {
    Message::SendFail() << "Error in the DERVM_Provider during reading the file " << thePath
                        << "\t: theDocument shouldn't be null";
    return false;
  }
  if (!GetNode()->IsKind(STANDARD_TYPE(DERVM_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the DERVM_Provider during reading the file " << thePath
                        << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(DERVM_ConfigurationNode) aNode = Handle(DERVM_ConfigurationNode)::DownCast(GetNode());
  XCAFDoc_DocumentTool::SetLengthUnit(theDocument,
                                      aNode->GlobalParameters.LengthUnit,
                                      UnitsMethods_LengthUnit_Millimeter);
  RVMCAFControl_Reader aReader;
  Standard_Real        aUnit = aNode->GlobalParameters.LengthUnit / 1000.;
  aReader.SetSystemLengthUnit(aUnit);
  if (!theWS.IsNull())
  {
    aReader.SetWS(theWS);
  }
  if (!aNode->InternalParameters.ReadTagRulesFilename.IsEmpty())
  {
    aReader.SetTagRules(
      TCollection_ExtendedString(aNode->InternalParameters.ReadTagRulesFilename, true));
  }

  aReader.SetSkipDegeneratedObjects(aNode->InternalParameters.ReadSkipDegenerated);

  if (IsDirectory(thePath.ToCString()))
  {
    // It is a directory
    std::vector<std::string> aFileNames = DirectoryFiles(thePath.ToCString(), ".rvm");

    if (!aReader.Perform(aFileNames, theDocument, theProgress))
    {
      Message::SendFail() << "Error in the DERVM_Provider during reading the file " << thePath;
      return false;
    }
  }
  else
  {
    if (!aReader.Perform(thePath, theDocument, theProgress))
    {
      Message::SendFail() << "Error in the DERVM_Provider during reading the file " << thePath;
      return false;
    }
  }
  return true;
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool DERVM_Provider::Read(const TCollection_AsciiString&  thePath,
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
bool DERVM_Provider::Read(const TCollection_AsciiString& thePath,
                          TopoDS_Shape&                  theShape,
                          Handle(XSControl_WorkSession)& theWS,
                          const Message_ProgressRange&   theProgress)
{
  if (!GetNode()->IsKind(STANDARD_TYPE(DERVM_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the DERVM_Provider during reading the file " << thePath
                        << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(DERVM_ConfigurationNode) aNode = Handle(DERVM_ConfigurationNode)::DownCast(GetNode());
  Handle(TDocStd_Document)        aDoc  = new TDocStd_Document("BinXCAF");
  Read(thePath, aDoc, theWS, theProgress);

  TDF_LabelSequence         aLabels;
  Handle(XCAFDoc_ShapeTool) aSTool = XCAFDoc_DocumentTool::ShapeTool(aDoc->Main());
  aSTool->GetFreeShapes(aLabels);
  if (aLabels.Length() <= 0)
  {
    Message::SendFail() << "Error in the DERVM_Provider during reading the file " << thePath
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
// function : Read
// purpose  :
//=======================================================================
bool DERVM_Provider::Read(const TCollection_AsciiString& thePath,
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
TCollection_AsciiString DERVM_Provider::GetFormat() const
{
  return TCollection_AsciiString("RVM");
}

//=======================================================================
// function : GetVendor
// purpose  :
//=======================================================================
TCollection_AsciiString DERVM_Provider::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}
