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

#ifndef _DEJT_ConfigurationNode_HeaderFile
#define _DEJT_ConfigurationNode_HeaderFile

#include <DE_ConfigurationNode.hxx>
#include <JTCAFControl_XcafToJT.hxx>
#include <JtTools_IBRepProvider.hxx>

//! The purpose of this class is to configure the transfer process for JT format
//! Stores the necessary settings for DEJT_Provider.
//! Configures and creates special provider to transfer JT files.
//!
//! Nodes grouped by Vendor name and Format type.
//! The Vendor name is "OCC"
//! The Format type is "JT"
//! The supported CAD extension is ".jt"
//! The import process is supported.
//! The export process is supported.
class DEJT_ConfigurationNode : public DE_ConfigurationNode
{
  DEFINE_STANDARD_RTTIEXT(DEJT_ConfigurationNode, DE_ConfigurationNode)
public:
  //! Initializes all fields by default
  Standard_EXPORT DEJT_ConfigurationNode();

  //! Copies values of all fields
  //! @param[in] theNode object to copy
  Standard_EXPORT DEJT_ConfigurationNode(const Handle(DEJT_ConfigurationNode)& theNode);

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

  //! Checks the file content to verify a format
  //! @param[in] theBuffer read stream buffer to check content
  //! @return Standard_True if file is supported by a current provider
  Standard_EXPORT virtual bool CheckContent(const Handle(NCollection_Buffer)& theBuffer) const
    Standard_OVERRIDE;

  //! Sets the Xt provider to read BRep data
  //! @param[in] theProvider provider to set
  void SetProviderXt(const Handle(JtTools_IBRepProvider)& theProvider)
  {
    myXtProvider = theProvider;
  }

  //! Gets the Xt provider to read BRep data
  //! @return Xt BRep provider to read
  Handle(JtTools_IBRepProvider) GetProviderXt() { return myXtProvider; }

  //! Checks the XT import supporting
  //! @return true if XT import is supported
  bool IsImportXTSupported() { return myIsValidXtLicense; }

public:
  enum ReadMode_PrefGeomRepr
  {
    ReadMode_PrefGeomRepr_Default = 0,
    ReadMode_PrefGeomRepr_Both,
    ReadMode_PrefGeomRepr_Mesh,
    ReadMode_PrefGeomRepr_BRep
  };

  enum WriteMode_NameFormat
  {
    WriteMode_NameFormat_InstanceName = 0,
    WriteMode_NameFormat_ProductName
  };

  struct JTCAFControl_InternalSection
  {
    // Read parameters
    bool ReadCheckBRepTypes = false; //<! Set ReadOutBRepTypeVar if BRep types should be find or not
    TCollection_AsciiString ReadDumpBRepProvider; //<! Creating new BRepProvider by the string value
    int  ReadLodIndex = 0;     //<! Set triangulation LOD to be loaded during translation
    bool ReadParallel = false; //<! Setup multithreaded execution.
    bool ReadSkipDegenerateTriangles =
      true; //<! Set if degenerate triangles should be skipped or not
    bool ReadDeduplicatePrims =
      true; //<! Set if translator should try generating single for JT primitives
    bool ReadDeferMetadata = false; //<! Set if Metadata reading should be deferred
    bool ReadCacheFileContent =
      false; //<! Set if JtData_Model should cache file content in internal buffer
    bool ReadHiddenMetadata = true; //<! Sets the flag controlling the loading of hidden Metadata
    bool ReadSkipLateDataLoading = false; //<! Sets flag to skip data loading
    bool ReadKeepLateData =
      true; //<! Sets flag to keep information about deferred storage to load/unload data later
    bool ReadLoadStrictly =
      false; //<! Sets flag to load and activate only exactly specified triangulation LOD
    bool                  ReadPrintDebugInfo = false; //<! Sets flag to print debug information
    bool                  ReadFilterLayers   = false; //<! Sets whether to filter the layers
    bool                  ReadWireframe      = false; //<! If true setting JtWirefram_BRepProvider
    bool                  ReadXt             = false; //<! Setting XSDRAWJT_XTBRepProvider
    ReadMode_PrefGeomRepr ReadPrefGeomRepr =
      ReadMode_PrefGeomRepr_Default;   //<! Sets preferred geometry representation
    bool ReadTranslateMetadata = true; //<! Set if Metadata should be translated into XDE document
    bool ReadTranslatePMI      = true; //<! Set if PMI data should be translated into XDE document

    // Write parameters
    WriteMode_NameFormat WriteNameFormat =
      WriteMode_NameFormat_InstanceName; //<! Define the export name format for JT nodes
    JtData_LengthUnit WriteJTLengthUnit =
      JtData_LengthUnit_MM; //<! Setting up JT length unit using in Data Exchange
  };

public:
  JTCAFControl_InternalSection InternalParameters; //<! Customize transfer process parameters

private:
  Standard_Boolean              myIsValidJtLicense; //!< Valid flag of Jt license
  Standard_Boolean              myIsValidXtLicense; //!< Valid flag of Parasolid license
  Handle(JtTools_IBRepProvider) myXtProvider;       //!< Provider to read XT BRep data
};

#endif // _DEJT_ConfigurationNode_HeaderFile
