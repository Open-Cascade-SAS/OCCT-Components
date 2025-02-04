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

#ifndef _DEXT_ConfigurationNode_HeaderFile
#define _DEXT_ConfigurationNode_HeaderFile

#include <DE_ConfigurationNode.hxx>

//! The purpose of this class is to configure the transfer process for XT format
//! Stores the necessary settings for DEXT_Provider.
//! Configures and creates special provider to transfer XT files.
//!
//! Nodes grouped by Vendor name and Format type.
//! The Vendor name is "OCC"
//! The Format type is "XT"
//! The supported CAD extension is ".x_t"
//! The import process is supported.
//! The export process isn't supported.
class DEXT_ConfigurationNode : public DE_ConfigurationNode
{
  DEFINE_STANDARD_RTTIEXT(DEXT_ConfigurationNode, DE_ConfigurationNode)
public:
  //! Initializes all fields by default
  Standard_EXPORT DEXT_ConfigurationNode();

  //! Copies values of all fields
  //! @param[in] theNode object to copy
  Standard_EXPORT DEXT_ConfigurationNode(const Handle(DEXT_ConfigurationNode)& theNode);

  //! Updates values according the resource
  //! @param[in] theResource input resource to use
  //! @return true if theResource loading has ended correctly
  Standard_EXPORT virtual bool Load(const Handle(DE_ConfigurationContext)& theResource)
    Standard_OVERRIDE;

  //! Writes configuration to the string
  //! @return result resource string
  Standard_EXPORT virtual TCollection_AsciiString Save() const Standard_OVERRIDE;

  //! Copies values of all fields
  //! @return new object with the same field values
  Standard_EXPORT virtual Handle(DE_ConfigurationNode) Copy() const Standard_OVERRIDE;

  //! Creates new provider for the own format
  //! @return new created provider
  Standard_EXPORT virtual Handle(DE_Provider) BuildProvider() Standard_OVERRIDE;

  //! Update loading status. Checking for the license.
  //! @param[in] theToImport flag to updates for import. Standard_True-import, Standard_False-export
  //! @param[in] theToKeep flag to save update result, disable if not license
  //! @return Standard_True, if node can be used
  Standard_EXPORT virtual bool UpdateLoad(const Standard_Boolean theToImport,
                                          const Standard_Boolean theToKeep) Standard_OVERRIDE;

  //! Custom function to activate DE component.
  //! The next sequence of keys are used with the single argument:
  //! 1 - General OCC component key for DE (required);
  //!
  //! The main goal - real-time loading plug-in activation.
  //! After applying of licenses status can be updated of
  //! - IsImportSupported
  //! - IsExportSupported
  Standard_EXPORT void virtual CustomActivation(const TColStd_ListOfAsciiString& theSequence)
    Standard_OVERRIDE;

public:
  //! Checks the import supporting
  //! @return true if import is supported
  Standard_EXPORT virtual bool IsImportSupported() const Standard_OVERRIDE;

  //! Checks to export supporting
  //! @return true if export is supported
  Standard_EXPORT virtual bool IsExportSupported() const Standard_OVERRIDE;

  //! Gets CAD format name of associated provider
  //! @return provider CAD format
  Standard_EXPORT virtual TCollection_AsciiString GetFormat() const Standard_OVERRIDE;

  //! Gets provider's vendor name of associated provider
  //! @return provider's vendor name
  Standard_EXPORT virtual TCollection_AsciiString GetVendor() const Standard_OVERRIDE;

  //! Gets list of supported file extensions
  //! @return list of extensions
  Standard_EXPORT virtual TColStd_ListOfAsciiString GetExtensions() const Standard_OVERRIDE;

public:
  struct XTCAFControl_InternalSection
  {
    // Read parameters
    bool                    ReadColorMode = true; //<! Used to indicate read Colors or not
    bool                    ReadNameMode  = true; //<! Used to indicate read Names or not
    TCollection_AsciiString ReadResourceName =
      "XT"; //<! Defines the name of the resource file to read
    TCollection_AsciiString ReadSequence =
      "FromXT"; //<! Defines the name of the sequence of operators to read
  };

public:
  XTCAFControl_InternalSection InternalParameters; //<! Customize transfer process parameters

private:
  Standard_Boolean myIsValidReadLicense; //!< Valid flag of the license to read
};

#endif // _DEXT_ConfigurationNode_HeaderFile
