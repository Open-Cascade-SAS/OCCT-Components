#     JT SDK User's Guide 

<h1><a id="products_user_guides__jt_interface_1">Introduction</a></h1>

<h2><a id="products_user_guides__jt_interface_1_1">Overview</a></h2>

<h3><a id="products_user_guides__jt_interface_1_1_1">This manual</a></h3>

This manual provides technical documentation on Open CASCADE JT Import-Export SDK and explains how to use this SDK for working with JT format from within Open CASCADE Technology based software.

The SDK provides data structure to store a JT model. It allows to:
* read a JT model from its files,
* write a JT model into a JT file,
* create, explore and modify JT models.

The following kinds of data can be stored in a JT model:
* name, color and transparency of the whole JT model and its parts;
* mesh representation (triangulation) of the whole JT model and its parts;
* different representations at different Levels Of Detail (LOD) of the same JT model and its parts.

Open CASCADE JT SDK also provides:
* Late Loading mechanism to delay loading of data associated with parts of a JT model until they are actually needed;
* data compression by different algorithms for JT files.

The SDK provides converters:
* from an *XCAF document* to a JT model, to transfer the hierarchy of names, colors and meshes from that XCAF document to a JT model;
* from the files of a JT model to an *XCAF document*, to transfer the hierarchy of names, colors and meshes from that JT model to an XCAF document.
*XCAF* is part of eXtended data exchange interface in Open CASCADE Technology, based on OCAF (Open CASCADE Application Framework).
It provides a universal document structure across Open Cascade data exchange components, including STEP, IGES, DXF, ACIS, Parasolid and JT formats.

This SDK contains examples (Draw commands) of using these converters.

<h3><a id="products_user_guides__jt_interface_1_1_2">Reference documentation</a></h3>

It is not the purpose of this manual to document the JT file format. Information on this file format is only provided insofar as necessary to explain the actual operations of the SDK.

For more information on the JT format, consult the appropriate external documentation.
Please contact ISO consortium for requesting *ISO 14306* specifications.

<h3><a id="products_user_guides__jt_interface_1_1_3">SDK features</a></h3>

* Reading of JT format major versions 8, 9 and 10;
* Writing in JT format version 8.1.

This SDK supports the following capabilities of JT format:
* LOD by elements "Vertex", "Tri-Strip Set" and "Primitive Set";
* attributes "Geometric Transform" and "Material";
* properties "Late Loaded" and "String";
* all node elements except for "Switch";
* "Data Compression and Encoding" by all "Encoding Algorithms", by compressions "ZLIB" and "LZMA".

<h2><a id="products_user_guides__jt_interface_1_2">Testing with Draw</a></h2>

The standard DRAWEXE executable from Open CASCADE Technology can be used for testing the JT SDK. TKJTTest plugin library provides specific commands to work with JT format and demonstrates how to use the JT SDK.

For more information on standard DRAW commands, see _Test Harness User's Guide_ available within Open CASCADE Technology.

For more information on testing the JT SDK with Draw see chapter [Draw](#products_user_guides__jt_interface_6).

<h1><a id="products_user_guides__jt_interface_2">Converting the files of a JT model to a CAF document</a></h1>

The files of a JT model can be converted to an XCAF document as follows:

~~~{.cpp}
TCollection_AsciiString theRootJtFilePath = ...;
Handle(TDocStd_Document) theBinXcafDoc = ...;

Standard_Integer aLodIndex = 0;
bool toSkipDegenerateTriangles = true;
bool toDeduplicatePrimitives = true;
bool toFilterLayers = ...;

JTCAFControl_Reader aConverter;
aConverter.SetTriangulationLod (aLodIndex);
aConverter.SetSkipDegenerateTriangles (toSkipDegenerateTriangles);
aConverter.SetDeduplicatePrimitives (toDeduplicatePrimitives);
aConverter.SetFilterLayers (toFilterLayers);
if (aConverter.Perform (theBinXcafDoc, theRootJtFilePath.ToCString()))
{
  // use the filled document
}
~~~

where:
* *theRootJtFilePath* is the full path to the root JT file of the model;
* *theBinXcafDoc* is an initialized XCAF document to store the converted model;
* *aLodIndex* is the index of the required LOD; 0 refers to most detailed LOD;
* *toSkipDegenerateTriangles* defines whether the degenerate triangles of the JT model should be skipped during the conversion;
* *toDeduplicatePrimitives* defines whether the same geometry *TopoDS_Shape* should be used for JT primitives (cylinders, boxes, spheres) with the same parameters but different locations;
* *toFilterLayers* defines whether layers of the model are filtered as described in the following properties of the nodes of the LSG segment:
    - "ACTIVELAYERFILTER"
    - "LAYERFILTER<ID>"
    - "LAYER"
* *aConverter* is an object to perform the conversion.

Type *JTCAFControl_Reader* is intended to perform the conversion.
The conversion behavior can be set by the following methods:

Method

~~~{.cpp}
bool JTCAFControl_Reader::Perform (Handle(TDocStd_Document)& theBinXcafDoc,
                                   const Standard_CString theModelFilePath,
                                   const Message_ProgressRange& theProgress);
~~~

performs the conversion: fills the document according to the model.
The structure of the model, the triangulations, the names and the colors are transferred to the document.
The resulting XCAF document can be visualized and written by means of Open CASCADE Technology.
*JTCAFControl_Reader* also provides a set of parameters altering conversion process, like *JTCAFControl_Reader::SetTriangulationLod()*.

<h1><a id="products_user_guides__jt_interface_3">Converting a XCAF document to a JT model</a></h1>

An XCAF document with a triangulated model can be converted to a JT model as follows:

~~~{.cpp}
Handle(TDocStd_Document) aBinXcafDoc = ...;
Jt_GUID::Value (Standard_True);
const Handle(JtNode_Partition) aRootPartition = JTCAFControl_XcafToJT::Convert(aBinXcafDoc);
~~~

where:
* *aBinXcafDoc* is the binary XCAF document;
* *aRootPartition* is the root of the structure of the resulting JT model.

Method

~~~{.cpp}
static Jt_GUID Jt_GUID::Value (Standard_Boolean theReset = Standard_False);
~~~

called with "true" as the argument reinitializes generation of unique identifiers for JT entities.

Method

~~~{.cpp}
static Handle(JtNode_Partition) JTCAFControl_XcafToJT::Convert(
  const Handle(TDocStd_Document)& theDocument);
~~~

converts the document to a JT structure and returns its root partition.

The structure of the document, the triangulations, the names, the colors and the length unit are transferred to the JT model.

The length unit is stored for each "Meta Data", Instance and Part node is accessible by its methods:

~~~{.cpp}
JtData_LengthUnit LengthUnit() const;

void SetLengthUnit(const JtData_LengthUnit theLengthUnit);
~~~

The JT model can be written to a file as described below.

<h1><a id="products_user_guides__jt_interface_4">Writing a JT model</a></h1>

A JT model can be written to a file as follows:

~~~{.cpp}
Handle(JtNode_Partition) aRootPartition = ...;
TCollection_AsciiString aFilePath = ...;
Handle(JtData_Model) aJTModel = new JtData_Model(aFilePath);
if (!aJTModel->Store(aRootPartition))
{
  // Process the error.
}
~~~

where:
* *aRootPartition* is the root of the structure of the JT model;
* *aFilePath* is the full path of the file to store the JT model;
* *aJtModel* is an object to write the JT model.

Method

~~~{.cpp}
JtData_Model::JtData_Model (const TCollection_AsciiString& theFilePath,
                            const Handle(JtData_Model)& theParent = Handle(JtData_Model)());
~~~

is intended to create an object to work with the JT model stored in the given file.

Method

~~~{.cpp}
Standard_Boolean JtData_Model::Store (const Handle(JtData_Object)& theRootPartition,
                                      const Message_ProgressRange& theProgress);
~~~

writes the structure of the JT model defined by the root partition to the file.

<h1><a id="products_user_guides__jt_interface_5">Reading a JT model</a></h1>

A JT model can be read from its files to an SDK structure as follows:

~~~{.cpp}
TCollection_AsciiString aRootFilePath = ...;
Handle(JtData_Model) aJTModel = new JtData_Model(aRootFilePath);
Handle(JtNode_Partition) aRootPartition = aJTModel->Init();
if (!aRootPartition.IsNull())
{
  // Use the JT model.
}
~~~

where:
* *aRootFilePath* is the full path to the root file of the model;
* *aModel* is an object to perform the reading;
* *aRootPartition* is the root partition of the model.

Method

~~~{.cpp}
Handle(JtNode_Partition) JtData_Model::Init(
  const Handle(NCollection_BaseAllocator)& theAllocator,
  const Message_ProgressRange& theProgress,
  const Standard_Boolean theToCacheFileContent = Standard_False);
~~~

reads the part of the JT file, which does not use the late loading mechanism, and returns the root partition of the JT model.
On success the partition is not null.

A non-root JT partition can be read from a JT file in a way similar to the reading of the root JT partition:

~~~{.cpp}
const TCollection_AsciiString aPartitionFilePath = ...;
Handle(JtData_Model) aJTSubModel = new JtData_Model(aPartitionFilePath);
Handle(JtNode_Partition) aPartition = aJTSubModel->Init();
if (!aPartition.IsNull())
{
  // Use the JT submodel.
}
~~~

where:
* *aPartitionFilePath* is the full path to the file of the non-root partition;
* *aJTSubModel* is an object to read the non-root partition;
* *aPartition* is the non-root partition.

The full path to the non-root partition file could be obtained using the full
path to the root JT partition file and the relative paths to the files of the
non-root partitions between the root partition and the required non-root
partition. The following method returns the path for a partition already read
from a file:

~~~{.cpp}
const TCollection_AsciiString& JtNode_Partition::RelativePath() const;
~~~

A JT "Tri-Strip Set Shape" node using the late loading mechanism can be read from its file as follows:

~~~{.cpp}
Handle(JtNode_Shape_TriStripSet) aTriStripSetShape = ...;
Handle(JtProperty_LateLoaded) aTriStripSetShapeLateLoad = (*aTriStripSetShape->LateLoads())[0];
aTriStripSetShapeLateLoad->Load();
Handle(JtElement_ShapeLOD_TriStripSet) aTriStripSetShapeLodSegment =
  Handle(JtElement_ShapeLOD_TriStripSet)::DownCast(aTriStripSetShapeLateLoad->DefferedObject());
if (!aTriStripSetShapeLodSegment.IsNull())
{
  // Use the data.
  ...

  // Release the data.
  aTriStripSetShapeLateLoad->Unload();
}
~~~

The methods used in the last example are

~~~{.cpp}
virtual const JtData_Array<Handle(JtProperty_LateLoaded)>*
  JtNode_Shape_TriStripSet::LateLoads() const
~~~

to access the late loaded properties of the node;

~~~{.cpp}
Standard_Boolean JtProperty_LateLoaded::Load(
  const Handle(NCollection_BaseAllocator)& theAllocator,
  const Message_ProgressRange& theProgress,
  JtData_SharedIStream* theSharedFile = NULL);
~~~

to read the "Tri-Strip Set Shape LOD" segment from the file for the node;

~~~{.cpp}
const Handle(JtData_Object)& JtProperty_LateLoaded::DefferedObject() const;
~~~

to access the read segment;

~~~{.cpp}
JtProperty_LateLoaded::Unload()
~~~

to release the memory allocated for the segment.

JT is a *visualization format* first of all, so that triangulation is the main geometry representation in the file.
STEP (ISO 10303) file format is more suitable for vendor-neutral data exchange of exact B-Rep geometry (boundary representation) across CAD systems, which is directly supported by Open CASCADE Technology.
JT format, however, allows putting optional B-Rep in form of Parasolid XT format (as blob data chunks) in addition to mesh representation.
JT SDK may unpack these data chunks from file, but doesn't interpret them.
Uncompressed data can be accessed from *JtElement_XTBRep* element in form of array of bytes:

~~~{.cpp}
Handle(JtElement_XTBRep) aJtElement_XtBRep = ...
const JtData_Array<Standard_Byte>& aXtData = aJtElement_XtBRep->RawData();
~~~

The data in XT format can be processed by one of Open Cascade Xt readers: *XtBRep* for a single solid, 
*MultiXtBRep* for a multiple solids, and *WireframeBRep* for curves. 
These parameters are defined by *SetBRepProvider* after calling *ReadJT*.
The output is controlled by *SetPreferredRepresentation*, which should be set as one of the following values:

~~~{.cpp}
if (aReprStr == "preferbrep")
{
  aPrefGeomRepr = JTCAFControl_Representation_PreferBRep;
}
else if (aReprStr == "prefermesh")
{
  aPrefGeomRepr = JTCAFControl_Representation_PreferMesh;
}
else if (aReprStr == "mesh"
      || aReprStr == "meshonly")
{
  aPrefGeomRepr = JTCAFControl_Representation_MeshOnly;
}
else if (aReprStr == "brep"
      || aReprStr == "breponly")
{
  aPrefGeomRepr = JTCAFControl_Representation_BRepOnly;
}
~~~

Other parameters are stored within JTCAFControl_Reader.hxx; the rest of them are boolean.
Initialization example:

~~~{.cpp}
static Standard_Integer ReadJt()
{
  TCollection_AsciiString aDocName;
  TCollection_AsciiString aFilePath;
  Standard_Integer aLodIndex = 0;
  bool toUseExistingDoc   = false;
  bool toParallel         = false;
  bool toSkipDegenerateTriangles = true;
  bool toDeduplicatePrims = true;
  bool toDeferMetadata = false;
  bool toCacheFileContent = false;
  bool loadHiddenMetadata = true;
  bool toFilterLayers = false;
  bool toReadWireframe = false;
  bool toReadXt = false;
  bool toCheckCheckBRepTypes = false
  JTCAFControl_Representation aPrefGeomRepr = JTCAFControl_Representation_PreferMesh;
  Handle(JtTools_IBRepProvider) aDumpBRepProvider;
  Handle(TDocStd_Document) aDoc;
  Handle(TDocStd_Application) anApp = DDocStd::GetApplication();
  anApp->NewDoc(aDoc);
  {
    JTCAFControl_Reader aReader;
    aReader.SetParallel (toParallel);
    aReader.SetTriangulationLod (aLodIndex);
    aReader.SetSkipDegenerateTriangles (toSkipDegenerateTriangles);
    aReader.SetDeduplicatePrimitives (toDeduplicatePrims);
    aReader.SetDeferMetadata (toDeferMetadata);
    aReader.SetCacheFileContent (toCacheFileContent);
    aReader.SetPreferredRepresentation (aPrefGeomRepr);
    aReader.SetLoadHiddenMetadata (loadHiddenMetadata);
    aReader.SetFilterLayers (toFilterLayers);
~~~

<h1><a id="products_user_guides__jt_interface_6">Draw</a></h1>

<h2><a id="products_user_guides__jt_interface_6_1">Overview</a></h2>

The Draw plugin with JT commands can be loaded as follows:

~~~{.tcl}
pload -DrawPluginProducts JT
~~~

Auxiliary plugins of Open CASCADE Technology can be loaded as follows:

~~~{.tcl}
pload MODELING VISUALIZATION DCAF XDE
~~~

Draw plugin "JT" contains commands *ReadJt* and *WriteJt* to demonstrate the work of the JT SDK.

<h2><a id="products_user_guides__jt_interface_6_2">ReadJt</a></h2>

Command *ReadJt* reads a JT model from its files and converts it to a CAF document:

~~~{.tcl}
ReadJt <cafDoc> <rootJtFile>"
  [-lod LOD=0]"
  [-skipDegenerate {on|off}=on]"
  [-deduplicatePrims {on|off}=on]"
  [-parallel {on|off}=off]"
  [-filterLayers]"
~~~

where:
*  *-lod* - loads a specific triangulation (0 - highest quality);
*  *-skipDegenerate* - skips degenerate triangles;
*  *-deduplicatePrims* - shares *BRep* geometry defining JT primitives with the same parameters;
*  *-parallel* - uses multithreaded optimizations;
*  *-filterLayers* - filters the layers by properties "ACTIVELAYERFILTER", "LAYERFILTER<ID>" and "LAYER".

<i><b>Example</b></i>

~~~{.tcl}
# Read and convert the JT model to document "doc".
ReadJt  doc  c:/model.jt  -lod 1  -parallel

# Show document "doc".
vinit
XDisplay -dispMode 1 doc
~~~

<h2><a id="products_user_guides__jt_interface_6_3">WriteJt</a></h2>

Command *WriteJt* converts a CAF document to a JT model and writes it to a JT file:

~~~{.tcl}
WriteJt <cafDoc> <jtFile>
~~~

<i><b>Example</b></i>

~~~{.tcl}
# Convert document "doc" to a JT model and write it to a JT file.
WriteJt doc c:/model_8_1.jt
~~~

where `<file>` is the full path to the JT file.

<h1><a id="products_user_guides__jt_requisites">System Requirements</a></h1>

Component is supported on Windows (IA-32 and x86-64), Linux (x86-64), Mac OS X (x86-
64, arm64), Android (ARMv7, arm64 and x86), and iOS (arm64) platforms.
The table below lists Component's library dependencies and its system requirements.


| System Requirements | |
| --------- | ----------- |
| <b>Operating System</b>   | |
| Windows  | MS Windows 10 / 8 |
| Linux   | Arch Linux, CentOS 6.4, CentOS 7.3, Fedora 24, Fedora 26, Ubuntu-1804, Debian 7.0, Debian 8.0, Debian 9.0, Debian 10.0 |
| OS X/macOS | OS X/macOS 10.10 and above, x86_64 and arm64 |
| iOS | iOS 7 and above |
| Android | Android 4.2 and above |
| | |
| <b>C++</b> | |
| Windows    | Microsoft Visual Studio 2015, 2017, 2019<br> GCC 7.1.0 (Mingw-w64) |
| Linux      | GNU gcc 4.3 - 8.3.0 (in framework of listed above Linux OS)<br> LLVM Clang 3+ |     
| Mac OS X   | XCode 6 or newer |
| Android  | GCC 7.1.0+ (android-ndk-r15+) - Windows<br> LLVM Clang 5.0.300080+ (android-ndk-r15+) - Linux |
| | |
| <b>Library dependencies</b> | |
| Open CASCADE Technology |  Open CASCADE Technology 7.9.0<br> https://dev.opencascade.org/release |
| Intel oneTBB (optional tool<br>for multithreaded algorithms) | oneTBB 2021.5.0<br> https://github.com/oneapi-src/oneTBB |
| lzma (Library and command <br>line tools for XZ and LZMA<br>compressed files) | lzma-5.2.2<br> https://sourceforge.net/projects/lzmautils/files/ |
| zlib (free cross-platform <br>library for data compression) | zlib-1.2.8<br> https://www.zlib.net/ |
