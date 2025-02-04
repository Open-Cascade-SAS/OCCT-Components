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

#ifndef _DEXT_Provider_HeaderFile
#define _DEXT_Provider_HeaderFile

#include <DE_Provider.hxx>
#include <DEXT_ConfigurationNode.hxx>

class TopoDS_Shape;
class TDocStd_Document;

//! The class to transfer XT files.
//! Reads and Writes any XT files into/from OCCT.
//! Each operation needs configuration node.
//!
//! Providers grouped by Vendor name and Format type.
//! The Vendor name is "OCC"
//! The Format type is "XT"
//! The import process is supported.
//! The export process isn't supported.
class DEXT_Provider : public DE_Provider
{
public:
  DEFINE_STANDARD_RTTIEXT(DEXT_Provider, DE_Provider)

public:
  //! Default constructor
  //! Configure translation process with global configuration
  Standard_EXPORT DEXT_Provider();

  //! Configure translation process
  //! @param[in] theNode object for initializing Internal Parameters
  Standard_EXPORT DEXT_Provider(const Handle(DE_ConfigurationNode)& theNode);

public:
  //! Reads a CAD file, according internal configuration
  //! @param[in] thePath path to the import CAD file
  //! @param[out] theDocument document to save result
  //! @param[in] theWS current work session
  //! @param theProgress[in] progress indicator
  //! @return true if Read operation has ended correctly
  Standard_EXPORT virtual bool Read(
    const TCollection_AsciiString&  thePath,
    const Handle(TDocStd_Document)& theDocument,
    Handle(XSControl_WorkSession)&  theWS,
    const Message_ProgressRange&    theProgress = Message_ProgressRange()) Standard_OVERRIDE;

  //! Reads a CAD file, according internal configuration
  //! @param[in] thePath path to the import CAD file
  //! @param[out] theDocument document to save result
  //! @param theProgress[in] progress indicator
  //! @return true if Read operation has ended correctly
  Standard_EXPORT virtual bool Read(
    const TCollection_AsciiString&  thePath,
    const Handle(TDocStd_Document)& theDocument,
    const Message_ProgressRange&    theProgress = Message_ProgressRange()) Standard_OVERRIDE;

  //! Reads a CAD file, according internal configuration
  //! @param[in] thePath path to the import CAD file
  //! @param[out] theShape shape to save result
  //! @param[in] theWS current work session
  //! @param theProgress[in] progress indicator
  //! @return true if Read operation has ended correctly
  Standard_EXPORT virtual bool Read(
    const TCollection_AsciiString& thePath,
    TopoDS_Shape&                  theShape,
    Handle(XSControl_WorkSession)& theWS,
    const Message_ProgressRange&   theProgress = Message_ProgressRange()) Standard_OVERRIDE;

  //! Reads a CAD file, according internal configuration
  //! @param[in] thePath path to the import CAD file
  //! @param[out] theShape shape to save result
  //! @param theProgress[in] progress indicator
  //! @return true if Read operation has ended correctly
  Standard_EXPORT virtual bool Read(
    const TCollection_AsciiString& thePath,
    TopoDS_Shape&                  theShape,
    const Message_ProgressRange&   theProgress = Message_ProgressRange()) Standard_OVERRIDE;

public:
  //! Gets CAD format name of associated provider
  //! @return provider CAD format
  Standard_EXPORT virtual TCollection_AsciiString GetFormat() const Standard_OVERRIDE;

  //! Gets provider's vendor name of associated provider
  //! @return provider's vendor name
  Standard_EXPORT virtual TCollection_AsciiString GetVendor() const Standard_OVERRIDE;

private:
  //! Initialize static variables
  void initStatic(const Handle(DE_ConfigurationNode)& theNode);

  //! Initialize static variables
  void setStatic(const DEXT_ConfigurationNode::XTCAFControl_InternalSection theParameter);

  //! Personizes work session with current format.
  //! Creates new temporary session if current session is null
  //! @param[in] theWS current work session
  void personizeWS(Handle(XSControl_WorkSession)& theWS);

  DEXT_ConfigurationNode::XTCAFControl_InternalSection myOldValues;
};

#endif // _DEXT_Provider_HeaderFile
