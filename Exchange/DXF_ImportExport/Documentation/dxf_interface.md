#     DXF Interface User's Guide 

<h1><a id="products_user_guides__dxf_interface_1">Introduction</a></h1>

<h2><a id="products_user_guides__dxf_interface_1_1">Overview</a></h2>

<h3><a id="products_user_guides__dxf_interface_1_1_1">This manual</a></h3>

This manual provides technical documentation on the OPEN CASCADE DXF Interface and explains how to use this interface for reading and writing DXF files from within Open CASCADE Technology based software.

The DXF format is featured by AutoCAD ® software. DXF files that are produced by this interface conform to DXF; all the stable versions of DXF from R13 are supported:

* On reading: DXF R13, DXF R14, DXF 2000/2000i, DXF 2002, DXF 2004, DXF 2007, DXF 2010, DXF 2013.
* On writing: DXF R13, DXF R14, DXF 2000, DXF 2002, DXF 2004, DXF 2007, DXF 2010, DXF 2013.

DXF entities wrapping proprietary modeler data of AutoCAD (ACIS bodies in SAT format) are also supported. For more information on mapping SAT entities to Open CASCADE Technology, refer to the _OPEN CASCADE ACIS Layer User’s Guide_.

The interface offers two levels of DXF data support:

* The _basic interface_ supports only geometrical and topological DXF entities that are translated to and from Open CASCADE Technology shapes.
* The _XDE interface_ extends the basic interface with translation of attributes (colors, names, layers and assembly structure) attached to entities, to and from XDE data structures.

The interface loads all data from the DXF file including data that cannot be translated. During the translation process, the interface finds and translates entities that have correspondence in Open CASCADE Technology representation.

<h3><a id="products_user_guides__dxf_interface_1_1_2">Reference documentation</a></h3>

It is not the purpose of this manual to document the file format that will be converted to the representation used by Open CASCADE Technology. Information on this file format is only provided insofar as necessary to explain the actual operations of the interface.

For more information on the DXF format, consult the appropriate external documentation.

<h3><a id="products_user_guides__dxf_interface_1_1_3">What the DXF Interface does</a></h3>

The DXF Interface provides tools for bidirectional translation of geometrical and topological data between DXF files and Open CASCADE Technology native BRep models (known as shapes). It also supports attributes (names, colors and layers) that can be translated to Open CASCADE Technology using XDE.

<h2><a id="products_user_guides__dxf_interface_1_2">Testing with Draw</a></h2>

The standard DRAWEXE executable from Open CASCADE Technology can be used for testing DXF interface. The plugin library TKXSDRAWDXF provides specific commands for reading and writing DXF files.

For more information on standard DRAW commands, see the _Test Harness User’s Guide_ available within Open CASCADE Technology.

For more information on testing DXF interface with Draw see chapter [Draw commands interface](#products_user_guides__dxf_interface_4).

<h1><a id="products_user_guides__dxf_interface_2">Reading DXF files</a></h1>

<h2><a id="products_user_guides__dxf_interface_2_1">Overview</a></h2>

This chapter explains what is read from DXF files, how it is done and the methods available to get the most out of the process.

<h2><a id="products_user_guides__dxf_interface_2_2">Domains covered</a></h2>

The following kinds of entities can be read from DXF files:

* Geometric entities
* BLOCKs and INSERTs
* Proprietary modeler data (ACIS entities - BODY, REGION, 3DSOLID)
* Graphical attributes (colors, layers, and names) with XDE

For more information, see the chapter [Mapping DXF entities to OCCT shapes](#products_user_guides__dxf_interface_5_2).

**Note:** Views, text, and annotations are not translated. Administrative data is not translated but can be queried from the DXF model. Name, color, layer attributes and assembly structure attached to topological entities are translated to XDE data structures. For more information on using XDE for reading, refer to the chapter [Reading with XDE](#products_user_guides__dxf_interface_2_4).

<h2><a id="products_user_guides__dxf_interface_2_3">General Procedure</a></h2>

You can translate a DXF file to Open CASCADE Technology shapes as follows:

1. Load the file.
2. Check the file consistency (optional).
3. Set the translation parameters.
4. Perform the file translation.
5. Retrieve the results.

The following sections explain how this is done and what resources to use.

<h3><a id="products_user_guides__dxf_interface_2_3_1">Loading a DXF file</a></h3>

Load a DXF file as follows:

~~~{.cpp}
   DxfControl_Reader aReader;
   IFSelect_ReturnStatus stat = aReader.ReadFile("MyFile.dxf");
~~~

Loading the file just places the data into memory, no translation actually occurs.

**For advanced users**

As soon as the DXF file is loaded into memory, the DXF model can be accessed for querying. Obtain the DXF model as follows:

~~~{.cpp}
   Handle(DxfSection_Model) aModel =
   DxfSection_Model::DownCast(aReader.WS()->Model());
~~~

This model can then be queried to get the number of entities, to iterate and access entities, etc. For more information, refer to the description of the root class `Interface_InterfaceModel` in the Open CASCADE Technology documentation.

<h3><a id="products_user_guides__dxf_interface_2_3_2">Checking the DXF file</a></h3>

Check the loaded file as follows:

~~~{.cpp}
   aReader.PrintCheckLoad (failsonly, mode);
~~~

Error messages are generated if there are invalid or incomplete DXF entities, giving you information on the cause of the error. The error messages are written to the current trace file (or stream). For more information on error messages, refer to the paragraph [Translation messages](#products_user_guides__dxf_interface_2_3_7).

**Parameters**

~~~{.cpp}
   Standard_Boolean failsonly;
~~~

If set to Standard_True, you see fail messages only. If set to Standard_False, you see both fail and warning messages.

Your analysis of the file can be either message-oriented or entity-oriented. Choose your preferences as follows:

~~~{.cpp}
   IFSelect_PrintCount mode = IFSelect_<option>;
~~~

where `<option>` can be one of the following:

<table>
  <tr>
    <th align="center">Option</th>
    <th align="center">Results in</th>
  </tr>
  <tr>
    <td>ItemsByEntity</td>
    <td>A sequential list of all messages per DXF entity.</td>
  </tr>
  <tr>
    <td>CountByItem</td>
    <td>The number of DXF entities with their types per message.</td>
  </tr>
  <tr>
    <td>ShortByItem</td>
    <td>The number of DXF entities with their types per message and displays the rank numbers of the first five DXF entities per message.</td>
  </tr>
  <tr>
    <td>ListByItem</td>
    <td>The number of DXF entities with their type and rank numbers per message.</td>
  </tr>
  <tr>
    <td>EntitiesByItem</td>
    <td>The number of DXF entities with their type and rank numbers and Directory Entry numbers per message.</td>
  </tr>
</table>

<h3><a id="products_user_guides__dxf_interface_2_3_3">Setting the translation parameters</a></h3>

You can use the methods provided by the class `Interface_Static` to query and set the translation parameters. For example:

Read the parameter **read.precision.val** as follows:

~~~{.cpp}
   Standard_Real rp =
   Interface_Static::RVal("read.precision.val");
~~~

Modify this parameter **read.precision.val** as follows:

~~~{.cpp}
   if (!Interface_Static::SetRVal ("read.precision.val",0.001))
   { /* error */; }
~~~

To read shapes from a DXF file, the following interface parameters are available:

<table>
  <tr>
    <th align="center">Name</th>
    <th align="center">Description</th>
  </tr>
  <tr>
    <td>read.encoderegularity.angle</td>
    <td>Defines an angle used for coding the regularity of edges in the shape</td>
  </tr>
  <tr>
    <td>read.maxprecision.mode</td>
    <td>If 1 (Forced), maximal tolerance is the rigid limit.<br>If 0 (Preferred), maximal tolerance is the limit but can be exceeded by some algorithms.</td>
  </tr>
  <tr>
    <td>read.maxprecision.val</td>
    <td>Value of maximal precision (used if previous parameter is 1)</td>
  </tr>
  <tr>
    <td>read.precision.mode</td>
    <td>Precision for input entities.<br>If 0 (File), precision from the file is used for the loaded shapes.<br>If 1 (Session), the value of the following parameter is used.</td>
  </tr>
  <tr>
    <td>read.precision.val</td>
    <td>Value of precision (used if previous parameter is 1)</td>
  </tr>
  <tr>
    <td>read.stdsameparameter.mode</td>
    <td>The same parameter check will not be done if this flag is 0 (Off).</td>
  </tr>
  <tr>
    <td>read.surfacecurve.mode</td>
    <td>Parametric curves will not be read if this flag is 0 (Off).</td>
  </tr>
  <tr>
    <td>read.dxf.resource.name</td>
    <td>The name of the resource file for shape processing performed after translation (not used currently).</td>
  </tr>
  <tr>
    <td>read.dxf.sequence</td>
    <td>Prefix of parameters in the resource file for shape processing performed after translation (not used currently).</td>
  </tr>
  <tr>
    <td>read.dxf.anonymous.blocks</td>
    <td>Anonymous blocks are not read if this flag is set to 0 (Off).</td>
  </tr>
  <tr>
    <td>read.dxf.insunits.scale</td>
    <td>Indicates whether the measurement units specified in $INSUNITS variable (if exists) must be used instead of those defined via $MEASUREMENT variable. This mode is disabled by default.</td>
  </tr>
  <tr>
    <td>read.dxf.textentities</td>
    <td>Text entities (TEXT, MTEXT) are not translated to shapes if this flag is set to 0 (Off).</td>
  </tr>
  <tr>
    <td>read.dxf.hatchentities</td>
    <td>Hatch entities (HATCH) are not translated to shapes if this flag is set to 0 (Off).</td>
  </tr>
</table>

<h3><a id="products_user_guides__dxf_interface_2_3_4">Performing the DXF file translation</a></h3>

You must first select the entities you want to translate and then perform their translation.

**Select the entities to translate**

You can select either:

* the entire file
* a list of entities
* a single entity.

**The entire file**

Transferring a whole file means transferring all root entities.

Evaluate the number of roots after loading the file and before translation as follows:

~~~{.cpp}
   Standard_Integer nbRoots = aReader.NbRootsForTransfer();
~~~

Translate a single root entity identified by its rank with:

~~~{.cpp}
   Standard_Boolean ok = aReader.TransferOneRoot(rank);
~~~

Perform the translation of all root entities as follows:

~~~{.cpp}
   Standard_Integer num = aReader.TransferRoots();
~~~

**A list of entities**

A list of entities can be formed with help of DxfControl_Reader::GiveList() method.

For example, to form a list containing entities with DXF handles 1A, 1B, and 1C:

~~~{.cpp}
   Handle(TColStd_HSequenceOfTransient) list =
   aReader.GiveList("(#lA,#lB,#lC)");
~~~

The result is a `TColStd_HSequenceOfTransient`.

You can either translate a list of entities all at once or one at a time. Translating entities one at a time lets you check the validity of each translated entity.

Translate a list of entities all at once as follows:

~~~{.cpp}
   Standard_Integer nbtrans = aReader.TransferList(list);
~~~

where `nbtrans` gives the number of items in the list that produced a shape.

Translate a list of entities one at a time as follows:

~~~{.cpp}
   Standard_Integer i, nb = list->Length();
   for (i = 1; i <= nb; i ++)
   {
     Standard_Boolean OK = aReader.TransferEntity(list->Value(i));
   }
~~~

**Selections**

There are several predefined selections that can be used to create entity lists:

<table>
  <tr>
    <th align="center">Selection</th>
    <th align="center">Selects all</th>
  </tr>
  <tr>
    <td>xst-model-all</td>
    <td>Entities</td>
  </tr>
  <tr>
    <td>xst-model-roots</td>
    <td>Roots</td>
  </tr>
  <tr>
    <td>xst-transferable-all</td>
    <td>Translatable entities</td>
  </tr>
  <tr>
    <td>xst-transferrable-roots</td>
    <td>Translatable roots (default). Refer to chapter [Mapping DXF entities to OCCT shapes](#products_user_guides__dxf_interface_5_2) for more information on which DXF entities can be translated.</td>
  </tr>
  <tr>
    <td>xst-sharing + &lt;selection&gt;</td>
    <td>Entities sharing at least one entity selected by &lt;selection&gt;.</td>
  </tr>
  <tr>
    <td>xst-shared + &lt;selection&gt;</td>
    <td>Entities shared by at least one entity selected by &lt;selection&gt;.</td>
  </tr>
  <tr>
    <td>dxf-sign-type</td>
    <td>Entities of a given DXF type. For example, dxf-sign-type (BLOCK) selects all the block entities in the DXF file.</td>
  </tr>
</table>

By default (no arguments), `DxfControl_Reader::GiveList()` returns a list of all roots.

**A single entity**

You can select an entity either by its rank or by its handle. The handle of a DXF entity can be obtained as follows:

~~~{.cpp}
   Handle(DxfSection_Model) aModel =
   DxfSection_Model::DownCast(aReader.WS()->Model());

   Handle(DxfSection_Block) aBlock = aModel->FindBlock("BLK1");
~~~

For more information on available methods, refer to the header file of the class `DxfSection_Model`.

**Selection by rank**

Select an entity or a block by its rank as follows:

~~~{.cpp}
   Standard_Boolean ok = aReader.TransferOne(rank);
~~~

where `<rank>` is a number of the entity.

The method returns `Standard_True` to indicate that an entity has been translated.

**Direct selection of an entity**

The argument is a `Handle(Standard_Transient)` pointing to an entity.

~~~{.cpp}
   Standard_Boolean ok = aReader.TransferEntity(ent);
~~~

The method returns `Standard_True` to indicate that an entity has been translated.

**Performing the translation**

Translate an entity identified by its rank in the model as follows:

~~~{.cpp}
   Standard_Boolean ok = aReader.TransferOne(rank);
~~~

Translate an entity identified by its handle as follows:

~~~{.cpp}
   Standard_Boolean ok = aReader.TransferEntity(ent);
~~~

Translate a list of entities in one operation as follows:

~~~{.cpp}
   Standard_Integer nbtrans = aReader.TransferList(list);
~~~

Translate a list of entities, entity by entity, as follows:

~~~{.cpp}
   Standard_Integer i, nb = list->Length();
   for (i = 1; i <= nb; i ++)
   {
     Standard_Boolean OK = aReader.TransferEntity(list->Value(i));
   }
~~~

<h3><a id="products_user_guides__dxf_interface_2_3_5">Getting translation results</a></h3>

Each successful translation operation outputs one shape. A series of translations gives a series of shapes.

Each time you invoke `TransferOne`, `TransferEntity` or `TransferList`, their results are accumulated and the counter returned by `NbShapes()` method increases.

You can clear the results between two translation operations with:

~~~{.cpp}
   aReader.ClearShapes();
~~~

If you do not, the results from the next translation will be added to the accumulation. `TransferRoots` operations automatically clear all existing results before they start.

Get the number of shapes recorded in the result as follows:

~~~{.cpp}
   Standard_Integer nbs = aReader.NbShapes();
~~~

Get the result `<num>` (`<num>` is an integer between 1 and `NbShapes`), as follows:

~~~{.cpp}
   TopoDS_Shape shape = aReader.Shape(num);
~~~

Get the first result in a translation operation as follows:

~~~{.cpp}
   TopoDS_Shape shape = aReader.Shape();
~~~

Get all the results in a single shape as follows:

~~~{.cpp}
   TopoDS_Shape shape = aReader.OneShape();
~~~

The shape can be:

* a null shape if there are no results,
* in the case of a single result, a shape that is specific to that result,
* a compound that contains the results if there are several results.

<h3><a id="products_user_guides__dxf_interface_2_3_6">Checking that translation was correctly performed</a></h3>

Each time you invoke `TransferOneRoot` or `TransferRoots`, you can display the related messages as follows:

~~~{.cpp}
   aReader.PrintCheckTransfer (failsonly, mode);
~~~

This check concerns only the last invocation of `TransferOneRoot` or `TransferRoots`.

<h3><a id="products_user_guides__dxf_interface_2_3_7">Translation messages</a></h3>

It is possible to print or display messages that concern the normal functioning of the processor (transfer, loading, etc).

Class `Message_Messenger` manages processing of messages. A unique static instance of this class can be accessed as follows:

~~~{.cpp}
   const Handle(Message_Messenger) &aMsgr =
   Message::DefaultMessenger();
~~~

The `Message_Messenger` class outputs messages to so called "printers". To output messages to a file it is possible to use class `Message_PrinterOStream` as follows:

~~~{.cpp}
   Handle(Message_PrinterOStream) aPrinter = new
   Message_PrinterOStream("logfile",Standard_False,Gravity);
~~~

To output messages to standard output (cout) use it as follows:

~~~{.cpp}
   Handle(Message_PrinterOStream) aPrinter = new
   Message_PrinterOStream(Gravity);
~~~

where Gravity is `Message_Gravity` representing the level of message to be output.

The created printer can be added to `Message_Messenger` as follows:

~~~{.cpp}
   aMsgr->AddPrinter(aPrinter);
~~~

The root class `XSControl_Reader` provides to `DxfControl_Reader` the following methods to print messages associated to the entities during loading and translation:

~~~{.cpp}
   XSControl_Reader::PrintCheckLoad(failsonly,mode)
   XSContriol_Reader::PrintCheckTransfer(failsonly,mode)
~~~

where `failsonly` set to True enables printing of fails only messages, or all messages if set to False, and mode determines the contents and the order of the messages according to the terms of the `IFSelect_PrintCount` enumeration.

<h2><a id="products_user_guides__dxf_interface_2_4">Reading with XDE</a></h2>

You can use XDE to read colors, names, layers, and assemblies. When using XDE, the standard process of reading a DXF file is extended by steps ensuring the correct interpretation of these attributes. Note the following:

* All DXF INSERTs are read as assemblies to XDE. All shapes produced from BLOCKs which are not compounds are stored as a single compound in the XDE assembly.
* Since DXF colors can be attached to each entity, colors are read to XDE and are associated with shapes produced from the colored entity. As far as translation of assemblies is concerned, colors attached to DXF INSERTs are associated with the instances of assemblies. Colors attached to DXF BLOCKs are associated directly to the shapes. Colors defined by layer are obtained from the layer and are processed like a color defined in the entity.
* Each entity lies in a layer in DXF. Layer is associated to the shape if the shape has been produced from the entity which has another layer than its owner.
* Names of BLOCKs are associated with the shapes which have been produced from them.

<h3><a id="products_user_guides__dxf_interface_2_4_1">Loading a DXF file</a></h3>

Load a DXF file as follows:

~~~{.cpp}
   DXFCAFControl_Reader aReader;
   IFSelect_ReturnStatus stat = aReader.ReadFile("MyFile.dxf");
~~~

Loading the file just places the data into memory, no translation actually occurs.

<h3><a id="products_user_guides__dxf_interface_2_4_2">Setting the translation parameters</a></h3>

The standard parameters are set first, see the paragraph [Setting the translation parameters](#products_user_guides__dxf_interface_2_3_3). Then the following additional parameters are available to control translation of attributes from  to XDE:

* For transferring colors:

~~~{.cpp}
   aReader.SetColorMode(mode);

// mode can be `Standard_True` or `Standard_False`
~~~

* For transferring names:
~~~{.cpp}
   aReader.SetNameMode(mode);

// mode can be `Standard_True` or `Standard_False`
~~~


<h3><a id="products_user_guides__dxf_interface_2_4_3">Performing the translation to an XDE document</a></h3>

Perform the translation of the whole document as follows:

~~~{.cpp}
   Standard_Boolean ok = aReader.Transfer(doc);
~~~

Where doc is a variable which contains a handle to the output document and has a type `Handle(TDocStd_Document)`.

<h2><a id="products_user_guides__dxf_interface_2_5">Example</a></h2>

An example for DXF file reading follows:

<i><b>Example</b></i>
~~~{.cpp}
   #include <DxfControl_Reader.hxx>
   #include <TopoDS_Shape.hxx>
   #include <IFSelect_ReturnStatus.hxx>
   int main()
   {
    DxfControl_Reader aReader;
    // Load file MyFile.dxf
    IFSelect_ReturnStatus stat = aReader.ReadFile("MyFile.dxf");
    // Get the number of transferrable roots
    Standard_Integer num, nbroots = aReader.NbRootsForTransfer();
    cout<<"Nb Roots recognized for transfer "<<nbroots<<endl;
    for (num = 1; num <= nbroots; num ++)
    {
     // Each DXF part is transferred individually
     cout<<"Processing root "<<num<<endl;
     aReader.ClearShapes();
     Standard_Boolean statone = aReader.TransferOneRoot(num);
     if (statone)
     {
      cout<<"Root "<<num<<" transferred to"<<aReader.NbShapes()
      <<"Shape(s)"<<endl;
      // Obtain the result of translation as one shape
      TopoDS_Shape sh = aReader.OneShape();
      if (sh.IsNull()) { cout<<"No result"<<endl; continue; }
     }
     else
     {
      cout<<"Root "<<num<<" not transferred"<<endl;
      continue;
     }
    }
    // Another way: all roots at once
    Standard_Integer nb = aReader.TransferRoots();
    return 0;
   }
~~~

<h1><a id="products_user_guides__dxf_interface_3">Writing DXF files</a></h1>

<h2><a id="products_user_guides__dxf_interface_3_1">Overview</a></h2>

This chapter explains how DXF files are written, and the methods available to get the most out of the process.

<h2><a id="products_user_guides__dxf_interface_3_2">General Procedure</a></h2>

You can write a DXF file as follows:

1. Initialize the process.
2. Set the translation parameters.
3. Perform the model translation.
4. Write the output DXF file.

The following sections explain how this is done and what resources to use.

<h3><a id="products_user_guides__dxf_interface_3_2_1">Initializing the process</a></h3>

Initialize the process as follows:

~~~{.cpp}
   DxfControl_Writer aWriter;
~~~

<h3><a id="products_user_guides__dxf_interface_3_2_2">Setting the translation parameters</a></h3>

You can use the methods provided by the class `Interface_Static` to query and set the translation parameters. For example:

Read the parameter <b>"write.dxf.deflection.factor"</b> as follows:

~~~{.cpp}
   Standard_Real rp =
   Interface_Static::RVal("write.dxf.deflection.factor");
~~~

Modify this parameter <b>"write.dxf.deflection.factor"</b> as follows:

~~~{.cpp}
   if (!Interface_Static::SetRVal
       ("write.dxf.deflection.factor ",0.001)) { /* error */; }
~~~

To write shapes to a DXF file, use the following interface parameters:

<table>
  <tr>
    <th align="center">Name</th>
    <th align="center">Description</th>
  </tr>
  <tr>
    <td>write.surfacecurve.mode</td>
    <td>Currently not used</td>
  </tr>
  <tr>
    <td>write.dxf.deflection.factor</td>
    <td>This parameter is used when a TopoDS_Face is translated to a DXF Polyline (a polyface mesh computed as the triangulation of the TopoDS_Face). The maximum deflection of the mesh from the original surface is equal to the maximum dimension of the surface (X, Y, or Z) multiplied by the value of this parameter. The default value is 0.05.</td>
  </tr>
  <tr>
    <td>write.dxf.version</td>
    <td>This parameter specifies the DXF version that the written file conforms to. Enumeration uses the possible values 11 (Undef), 12 (R12), 13 (R13), 14 (R14), 15 (R2000), 16 (R2004), 17 (R2007), 18 (R2010), 19 (R2013)</td>
  </tr>
  <tr>
    <td>write.dxf.acis.mode</td>
    <td>If set to 1 (On), OCCT shells and solids should be written to DXF entities wrapping ACIS bodies.</td>
  </tr>
  <tr>
    <td>write.dxf.resource.name</td>
    <td>The name of the resource file for shape processing performed prior to translation (not used currently).</td>
  </tr>
  <tr>
    <td>write.dxf.sequence</td>
    <td>Prefix of parameters in the resource file for shape processing performed prior to translation (not used currently).</td>
  </tr>
</table>

<h3><a id="products_user_guides__dxf_interface_3_2_3">Performing the shape translation</a></h3>

You can perform the translation in one or several operations. Translate one shape as follows:

~~~{.cpp}
   IFSelect_ReturnStatus stat = aWriter.TransferShape (shape);
~~~

where shape is a TopoDS_Shape.

You can translate several shapes before writing a file. Each shape will result in one or more DXF entities added to the section ENTITIES.

<h3><a id="products_user_guides__dxf_interface_3_2_4">Writing the output file</a></h3>

Write the DXF output file name as follows:

~~~{.cpp}
   IFSelect_ReturnStatus stat = aWriter.WriteFile ("MyFile.dxf");
~~~

<h3><a id="products_user_guides__dxf_interface_3_2_5">Tolerance management</a></h3>

During export of a shape to DXF file, the tolerance is not written. When writing a DXF entity (wrapping an ACIS body) to a file, the tolerance of each geometrical entity such as surface or curve equals to the average value of sub-shapes tolerances.

<h3><a id="products_user_guides__dxf_interface_3_2_6">Translation messages</a></h3>

It is possible to print or display messages that concern the normal functioning of the processor (transfer, loading, etc).

Class `Message_Messenger` manages processing of messages. A unique static instance of this class can be accessed as follows:

~~~{.cpp}
   const Handle(Message_Messenger) &aMsgr =
   Message::DefaultMessenger();
~~~

The `Message_Messenger` class outputs messages to so called "printers". To output messages to a file it is possible to use class `Message_PrinterOStream` as follows:

~~~{.cpp}
   Handle(Message_PrinterOStream) aPrinter = new
   Message_PrinterOStream("logfile",Standard_False,Gravity);
~~~

To output messages to standard output (cout) use it as follows:

~~~{.cpp}
   Handle(Message_PrinterOStream) aPrinter = new
   Message_PrinterOStream(Gravity);
~~~

where Gravity is `Message_Gravity` representing the level of message to be output.

The created printer can be added to `Message_Messenger` as follows:

~~~{.cpp}
   aMsgr->AddPrinter(aPrinter);
~~~

The root class `XSControl_Writer` provides to `DxfControl_Writer` the following method to print messages associated to the entities during translation:

~~~{.cpp}
   XSContriol_Writer::PrintStatsTransfer(what,mode)
~~~

<h2><a id="products_user_guides__dxf_interface_3_3">Writing using XDE</a></h2>

Using XDE, you can write colors, names, layers, and assemblies. When using XDE, the standard process of writing a DXF file is extended by steps ensuring the correct interpretation of attributes. Note the following:

* A form of nested BLOCKs and INSERTs is used to map Compound shapes and XDE assemblies. This nesting reflects the original structure of the compound shape or assembly.
* Colors from XDE are written to DXF by associating a color with an entity that has been produced from the corresponding shape.
* Layers from XDE are written to DXF by associating a layer with an entity that has been produced by the corresponding shape.
* Names of assemblies and components in XDE are written to DXF as names of corresponding BLOCK entities.

<h3><a id="products_user_guides__dxf_interface_3_3_1">Initializing the translation from XDE to DXF</a></h3>

Initialize the process as follows:

~~~{.cpp}
   DXFCAFControl_Writer aWriter;
~~~

<h3><a id="products_user_guides__dxf_interface_3_3_2">Setting the translation parameters</a></h3>

The standard parameters are set first, see the paragraph [Setting the translation parameters](#products_user_guides__dxf_interface_3_2_2). Then the following additional parameters are available to control translation of attributes from to DXF:

* For transferring colors:

~~~{.cpp}
   aWriter.SetColorMode(mode);
~~~

// mode can be `Standard_True` or `Standard_False`

* For transferring names:

~~~{.cpp}
   aWriter.SetNameMode(mode);
~~~

// mode can be `Standard_True` or `Standard_False`

<h3><a id="products_user_guides__dxf_interface_3_3_3">Performing translation of an XDE document</a></h3>

You can perform the translation of a document as follows:

~~~{.cpp}
   IFSelect_ReturnStatus aRetSt = aWriter.Transfer(doc);
~~~

where doc is a variable which contains a handle to the input document for transferring and should have a type `Handle(TDocStd_Document)`.

<h3><a id="products_user_guides__dxf_interface_3_3_4">Writing the DXF file</a></h3>

Write the DXF file as follows:

~~~{.cpp}
   IFSelect_ReturnStatus stat = aWriter.WriteFile("MyFile.dxf");
~~~

<h2><a id="products_user_guides__dxf_interface_3_4">Example</a></h2>

An example for DXF file writing follows:

<i><b>Example</b></i>
~~~{.cpp}
   #include <DxfControl_Writer.hxx>
   #include <TopoDS_Shape.hxx>
   #include <BRepTools.hxx>
   int main()
   {
    TopoDS_Shape sh;
    DxfControl_Writer aWriter;
    // Translate TopoDS_Shape into a DXF entity
    IFSelect_ReturnStatus stat = aWriter.TransferShape(sh);
    // Write resulting entity to the DXF file
    stat = aWriter.WriteFile("MyFile.dxf");
    return 0;
   }
~~~

<h1><a id="products_user_guides__dxf_interface_4">Draw commands interface</a></h1>

<h2><a id="products_user_guides__dxf_interface_4_1">Overview</a></h2>

This manual explains how to use Draw Test Harness (DRAWEXE) executable for testing OPEN CASCADE DXF Interface interactively in Draw environment. It describes a set of Draw commands specific to data exchange tasks, which allow loading and writing of data files and analysis of the resulting data structures and shapes.

**NOTE:** In the description of the commands, square brackets ([ ]) are used to indicate optional parameters. Parameters given in the angle brackets (< >) and numbers (#) are to be substituted with the appropriate values. When several exclusive variants are possible, vertical dash (|) is used.

<h2><a id="products_user_guides__dxf_interface_4_2">General description</a></h2>

<h3><a id="products_user_guides__dxf_interface_4_2_1">Loading plugin commands</a></h3>

A set of commands to work with Data Exchange interfaces can be loaded in Draw as follows:

~~~{.cpp}
   Draw[] pload DATAEXCHANGE
~~~

A set of commands to work with DXF interface in particular can be loaded in Draw as follows:

~~~{.cpp}
   Draw[] pload –DrawPluginProducts DXF
~~~

<h3><a id="products_user_guides__dxf_interface_4_2_2">Setting interface parameters</a></h3>

A set of parameters for Data Exchange interfaces is defined in XSTEP component of Open CASCADE Technology. In Draw, view or change these parameters as follows:

~~~{.cpp}
   param [<parameter_name> [<value>]]
~~~

The **param** command with no arguments gives a list of all parameters with their values. When the argument \<parameter_name\> is specified, information about this parameter is printed (current value and short description). The third argument is used to set a new value of the given parameter. The result of the setting is printed immediately.

In all interface operations, the process protocol (fail and warning messages, mapping of the loaded entities into the Open CASCADE Technology shapes, etc.) can be written to the trace file. Two parameters are defined in the DRAW session: trace level (integer value from 0 to 9, default is 0), and trace file (default is standard output).

To view and change these parameters, use **xtrace** command.

Print current settings (for example: 'Level=1 - Standard Output') as follows:

~~~{.cpp}
   xtrace
~~~

Set the trace level to <value> as follows:

~~~{.cpp}
   xtrace <value>
~~~

Set the trace file as **tracefile.log** as follows:

~~~{.cpp}
   xtrace tracefile.log
~~~

Direct all messages to the standard output as follows:

~~~{.cpp}
   xtrace
~~~

<h3><a id="products_user_guides__dxf_interface_4_2_3">Reading DXF files</a></h3>

Read a DXF file as follows:

~~~{.cpp}
   dxfread <file_name> <result_shape_name> [<entity> ...]
~~~

Where entity can be defined by a number in the:

* File using sign '-' or '$' with number (identifies an entity by its sequence number in the file).
* File using letter 'l' or 'L' with number (identifies an entity by its line number in the file).
* Number in the model using number only.

You can define all roots by using sign '*'.

To transfer entities from the last loaded model use "." instead of the file name.

<h3><a id="products_user_guides__dxf_interface_4_2_4">Analyzing the transferred data</a></h3>

Retrieve general statistics on the loaded data as follows:

~~~{.cpp}
   data <symbol>
~~~

The information printed by this command depends on the specified symbol:

<table>
  <tr>
    <th align="center">Symbol</th>
    <th align="center">Result</th>
  </tr>
  <tr>
    <td><b>g</b></td>
    <td>Prints information contained in the header of the file.</td>
  </tr>
  <tr>
    <td><b>c</b> or <b>f</b></td>
    <td>Runs check procedure of the integrity of the loaded data and prints resulting statistics (<b>f</b> works only with fails while <b>c</b> with both fail and warning messages)</td>
  </tr>
  <tr>
    <td><b>t</b></td>
    <td>The same like <b>c</b> or <b>f</b>, with list of failed or warned entities</td>
  </tr>
  <tr>
    <td><b>m</b> or <b>l</b></td>
    <td>The same as <b>t</b> but prints also a status for each entity</td>
  </tr>
  <tr>
    <td><b>e</b></td>
    <td>Lists all entities of the model with their numbers, types, status of validity etc.</td>
  </tr>
  <tr>
    <td><b>r</b></td>
    <td>The same like <b>e</b> but lists only root entities</td>
  </tr>
</table>

A set of special objects are used to operate with the loaded model. They can be of the following types:

<table>
  <tr>
    <th align="center">Special object type</th>
    <th align="center">Operation</th>
  </tr>
  <tr>
    <td>Selection</td>
    <td>Filters - allow to select subsets of entities of the loaded model</td>
  </tr>
  <tr>
    <td>Counters</td>
    <td>Calculate some statistics on the model data</td>
  </tr>
</table>

Print a list of objects defined in the current session as follows:

~~~{.cpp}
   listitems
~~~

**NOTE:** In the following commands, if several \<selection\> arguments are specified, the results of each following selection are applied to its preceding command:

Print a list of loaded entities defined by the selection argument as follows:

~~~{.cpp}
   givelist <selection_name> [<selection_name>]
~~~

Print a number of loaded entities defined by selection argument as follows:

~~~{.cpp}
   givecount <selection_name> [<selection_name>]
~~~

Calculate and print statistics on the entities in the model using the following commands:

Print only a number of entities per each type matching the criteria defined by arguments as follows:

~~~{.cpp}
   count <counter> [<selection> ...]
~~~

Print the total number of entities of all types matching the criteria defined by arguments with the biggest number corresponding to one type as follows:

~~~{.cpp}
   sumcount <counter> [<selection> ...]
~~~

Print a list of entities for each type matching the criteria defined by the arguments as follows:

~~~{.cpp}
   listcount <counter> [<selection> ...]
~~~

The optional \<selection\> argument, if specified, defines a subset of the entities which are to be taken into account.

The argument \<counter\> is one of the currently defined counters:

<table>
  <tr>
    <th align="center">Counter</th>
    <th align="center">Operation</th>
  </tr>
  <tr>
    <td>xst-types</td>
    <td>Calculates how many entities of each OCCT type exist</td>
  </tr>
</table>

List the entity types which were encountered in the last loaded file (with number of DXF entities of each type) as follows:

~~~{.cpp}
   listtypes <selection_name> ...
~~~

The list only shows a subset of all the entities. This subset is defined by the optional selection argument.

Get information on Open CASCADE Technology shape contents as follows:

~~~{.cpp}
   statshape <shape_name>
~~~

It outputs the count of each kind of subshape (vertex, edge, wire, etc.) in the shape.

**NOTE:** The number of faces is returned as a number of references. To obtain the number of single instances, use **nbshapes** command (from MODELING plugin).

Get the entity from which the shape is produced as follows:

~~~{.cpp}
   fromshape <shape_name>
~~~

Use "estatus" and "entity" to get information about the entity as follows:

~~~{.cpp}
   entity <entity> <level>
~~~

Where entity is defined by a:

* number in the file using sign '-' or '#' with number.
* line number in the file using letter 'l' or 'L' with number. number in the model using number only.
* subtype of the entity using letter 'r' with the subtype number.

and where level is an integer value from 0 to 3:

<table>
  <tr>
    <th align="center">Level value</th>
    <th align="center">Description</th>
  </tr>
  <tr>
    <td>0</td>
    <td>Displays only entity type</td>
  </tr>
  <tr>
    <td>1</td>
    <td>Displays entity type, sequence number and line number</td>
  </tr>
  <tr>
    <td>2</td>
    <td>Displays entity type, sequence number, line number and dump of entity data</td>
  </tr>
  <tr>
    <td>3</td>
    <td>Displays entity type, sequence number, line number and dump of entity data with expanding subtypes.</td>
  </tr>
</table>

Analyze the internal correctness of the shape as follows:

~~~{.cpp}
   checkshape <shape_name>
~~~

This checks the geometry and topology of the shape for different types of inconsistencies, like self-intersecting wires or incorrect orientation of trimming contours. If an error is found, erroneous parts of the shape are copied with the names `"faulty_#"` and an appropriate message is generated.

To explode the shape and calculate minimum, average and maximum tolerances of it, proceed as follows:

~~~{.cpp}
   tolerance <shape_name> [<min> [<max>] [<symbol>]]
~~~

You get the maximum, average and minimum values of tolerances for each kind of sub-shape having tolerances and also on the whole shape.

When specifying \<min\> and \<max\> arguments, this command outputs shapes with names \<shape_name\>_... and their total number with tolerances in the range [min, max].

\<Symbol\> is used for specifying the kinds of sub-shapes to be analyzed: v - for vertices, e - for edges, f - for faces, c - for shells and faces.

<h3><a id="products_user_guides__dxf_interface_4_2_5">Writing DXF files</a></h3>

Write the shape into the DXF file as follows:

~~~{.cpp}
   dxfwrite <shape_name> <file name>
~~~

Several shapes can be written into one file by making a compound of them. For making a compound refer to the "Test Harness User’s Guide" of Open CASCADE Technology.

<h3><a id="products_user_guides__dxf_interface_4_2_6">Index of useful commands</a></h3>

<table>
  <tr>
    <th align="center">
      Command
    </th>
    <th align="center">
      Description
    </th>
  </tr>
  <tr>
    <td>
      <b>Setting general parameters</b>
    </td>
  </tr>
  <tr>
    <td>
      xtrace [#|&lt;file&gt;|.]
    </td>
    <td>
      View and set parameters of the trace file
    </td>
  </tr>
  <tr>
    <td>
      param [&lt;param&gt; [&lt;val&gt;]]
    </td>
    <td>
      View and set parameters of transfer
    </td>
  </tr>
  <tr>
    <td>
      <b>Reading and writing SAT file</b>
    </td>
  </tr>
  <tr>
    <td>
      xload &lt;file&gt;
    </td>
    <td>
      Loads entities from the file into memory
    </td>
  </tr>
  <tr>
    <td>
      dxfread {&lt;file&gt;} &lt;name&gt;
    </td>
    <td>
      Translates DXF file into shape
    </td>
  </tr>
  <tr>
    <td>
      dxfwrite &lt;shape&gt; [&lt;file-name.sat&gt;]
    </td>
    <td>
      Translates a shape into DXF file
    </td>
  </tr>
  <tr>
    <td>
      <b>Checking the results of the load procedure</b>
    </td>
  </tr>
  <tr>
    <td>
      data &lt;symbol&gt;
    </td>
    <td>
      Gets statistics on the loaded file
    </td>
  </tr>
  <tr>
    <td>
      listitems
    </td>
    <td>
      Gets list of all defined special objects like selections and counters
    </td>
  </tr>
  <tr>
    <td>
      count &lt;counter&gt; [&lt;selection&gt;]
    </td>
    <td>
      Counts entities by counter
    </td>
  </tr>
  <tr>
    <td>
      listcount &lt;counter&gt; [&lt;selection&gt;]
    </td>
    <td>
      Counts entities by counter and list them
    </td>
  </tr>
  <tr>
    <td>
      givelist &lt;selection&gt;
    </td>
    <td>
      Gets list of subset of loaded entities defined by selection
    </td>
  </tr>
  <tr>
    <td>
      listtypes [&lt;selection&gt;]
    </td>
    <td>
      Gets statistics on the types of the entities loaded
    </td>
  </tr>
  <tr>
    <td>
      entity &lt;entity&gt; &lt;level&gt;
    </td>
    <td>
      Gets information about entities
    </td>
  </tr>
  <tr>
    <td>
      estatus
    </td>
    <td>
      Gets information about super- and sub- entities.
    </td>
  </tr>
  <tr>
    <td>
      <b>Analyzing of the loaded shapes</b>
    </td>
  </tr>
  <tr>
    <td>
      checkshape &lt;shape&gt;
    </td>
    <td>
      Checks shape for internal errors
    </td>
  </tr>
  <tr>
    <td>
      statshape &lt;shape&gt;
    </td>
    <td>
      Gets statistics on the shape
    </td>
  </tr>
  <tr>
    <td>
      tolerance &lt;shape&gt;
    </td>
    <td>
      Calculates tolerances for the given shape
    </td>
  </tr>
</table>

<h2><a id="products_user_guides__dxf_interface_4_3">BRep level commands</a></h2>

These commands are useful for translation of DXF models to and from OCCT shapes.

<h3><a id="products_user_guides__dxf_interface_4_3_1">dxfload</a></h3>

Purpose: Loads a DXF file and writes the model back to another file if necessary.

Syntax:  `dxfload <file_name> [<out_file_name>]`

Where:

* `<file_name>` is a name of file for loading;
* `<out_file_name>` is a name of file for writing. If `<out_file_name>` is omitted, writing is not performed.

<i><b>Example</b></i>
~~~{.cpp}
   # load a model from file
   dxfload D:/files/model.dxf
~~~

<h3><a id="products_user_guides__dxf_interface_4_3_2">dxfread</a></h3>

Purpose: Reads a DXF file to an OCCT shape.

Syntax:  `dxfread <file_name> <result_shape_name> [<entity>]`

Where:

* `<file_name>` is a name of file for reading;
* `<result_shape_name>` is a name of the resulting shape;
* `<entity>` is an auxiliary parameter. If `<entity>` is omitted, all roots will be translated as a set of shapes. If parameter `<entity>` is used as a number only the corresponding entity will be translated. If we use symbol ‘*’ as `<entity>` all roots will be translated as one compound.

<i><b>Example</b></i>
~~~{.cpp}
   # translate all roots from file to the shape "a"
   dxfread D:/files/model.dxf a *
~~~

<h3><a id="products_user_guides__dxf_interface_4_3_3">dxfreadsimple</a></h3>

Purpose: Reads a DXF file to an OCCT shape.

Syntax:  `dxfreadsimple <file_name> <result_shape_name>`

Where:

* `<file_name>` is a name of file for reading;
* `<result_shape_name>` is a name of the resulting shape

<i><b>Example</b></i>
~~~{.cpp}
   # translate all roots from file to the shape "a"
   dxfreadsimple D:/files/model.dxf a
~~~

<h3><a id="products_user_guides__dxf_interface_4_3_4">dxfwrite</a></h3>

Purpose: Writes an OCCT shape to a DXF file.
Syntax:  `dxfwrite <shape_name> <file_name>`

Where:

* `<shape_name>` is a shape name;
* `<file_name>` is a name of the file for writing

<i><b>Example</b></i>
~~~{.cpp}
   # write a shape "a" to a DXF file
   dxfwrite a D:/file/result.dxf
~~~

<h3><a id="products_user_guides__dxf_interface_4_3_5">dxfwritesimple</a></h3>

Purpose: Writes an OCCT shape to a DXF file.

Syntax:  `dxfwritesimple <shape_name> <file_name>`

Where:

* `<shape_name>` is a shape name;
* `<file_name>` is a name of the file for writing

<i><b>Example</b></i>
~~~{.cpp}
   # write a shape "a" to a DXF file
   dxfwritesimple a D:/file/result.dxf
~~~

<h2><a id="products_user_guides__dxf_interface_4_4">XDE level commands</a></h2>

<h3><a id="products_user_guides__dxf_interface_4_4_1">ReadDxf</a></h3>

Purpose: Reads a DXF file to an XCAF document.

Syntax:  `ReadDxf <doc_name> <file_name>`

Where:

* `<file_name>` is a name of the file for reading;
* `<doc_name>` is a name of the resulting XCAF document

<i><b>Example</b></i>
~~~{.cpp}
   # translate all roots from file to the document "D1"
   ReadDxf D1 D:/files/model.dxf
~~~

<h3><a id="products_user_guides__dxf_interface_4_4_2">WriteDxf</a></h3>

Purpose: Writes an XCAF document to a DXF file.

Syntax:  `WriteDxf <doc_name> <file_name>`

Where:

* `<doc_name>` is a name of the XCAF document;
* `<file_name>` is a name of the file for writing

<i><b>Example</b></i>
~~~{.cpp}
   # write document "D1" to a DXF file
   WriteDxf D1 D:/files/newmodel.dxf
~~~

<h1><a id="products_user_guides__dxf_interface_5">Appendix</a></h1>

<h2><a id="products_user_guides__dxf_interface_5_1">Packaging</a></h2>

<h3><a id="products_user_guides__dxf_interface_5_1_1">API classes of DXF Interface</a></h3>

**The DxfControl_Controller class**

This class performs all necessary initializations to use the DXF norm.

**The DxfControl_Reader class**

This class translates DXF entities into Open CASCADE Technology shapes.

**The DxfControl_Writer class**

This class translates Open CASCADE Technology shapes to DXF entities.

**The DxfSection_Model class**

This class represents a DXF file in the memory and provides access to DXF entities and blocks specified by rank number. This class can be used to consult and modify a DXF file before translation or writing.

**The DXFCAFControl_Reader class**

This class translates DXF entities into Open CASCADE Technology XCAF documents.

**The DXFCAFControl_Writer class**

This class translates Open CASCADE Technology XCAF documents to DXF entities.

<h2><a id="products_user_guides__dxf_interface_5_2">Mapping DXF entities to OCCT shapes</a></h2>

This chapter contains a table showing how the interface converts DXF entities to Open CASCADE Technology objects.

**NOTE:** DXF entity types that are not listed in the following tables are not translated.

<table>
  <tr>
    <th align="center">
      DXF entity
    </th>
    <th align="center">
      OCCT topology
    </th>
    <th align="center">
      OCCT geometry
    </th>
    <th align="center">
      Comments
    </th>
  </tr>
  <tr>
    <td>
      POINT
    </td>
    <td>
      TopoDS_Vertex
    </td>
    <td>
      Geom_CartesianPoint
    </td>
    <td>
      Extruded to TopoDS_Edge if thickness is non-zero.
    </td>
  </tr>
  <tr>
    <td>
      LINE
    </td>
    <td>
      TopoDS_Edge
    </td>
    <td>
      Trimmed Geom_Line
    </td>
    <td>
      Extruded to TopoDS_Edge if thickness is non-zero.
    </td>
  </tr>
  <tr>
    <td>
      CIRCLE
    </td>
    <td>
      TopoDS_Edge
    </td>
    <td>
      Geom_Circle
    </td>
    <td>
      Extruded to TopoDS_Edge if thickness is non-zero.
    </td>
  </tr>
  <tr>
    <td>
      ARC
    </td>
    <td>
      TopoDS_Edge
    </td>
    <td>
      Trimmed Geom_Circle
    </td>
    <td>
      Extruded to TopoDS_Edge if thickness is non-zero.
    </td>
  </tr>
  <tr>
    <td>
      XLINE
    </td>
    <td>
      TopoDS_Edge
    </td>
    <td>
      Geom_Line
    </td>
    <td>
      Untrimmed
    </td>
  </tr>
  <tr>
    <td>
      RAY
    </td>
    <td>
      TopoDS_Edge
    </td>
    <td>
      Trimmed Geom_Line
    </td>
    <td>
      Trimmed at one end only.
    </td>
  </tr>
  <tr>
    <td>
      ELLIPSE
    </td>
    <td>
      TopoDS_Edge
    </td>
    <td>
      Geom_Ellipse
    </td>
    <td>
    </td>
  </tr>
  <tr>
    <td>
      SPLINE
    </td>
    <td>
      TopoDS_Edge
    </td>
    <td>
      Geom_BSplineCurve
    </td>
    <td>
    </td>
  </tr>
  <tr>
    <td>
      LWPOLYLINE
    </td>
    <td>
      TopoDS_Wire
    </td>
    <td>
      Sequence of Geom_Line or Geom_Circle
    </td>
    <td>
      Extruded to TopoDS_Shell if thickness is non-zero.
    </td>
  </tr>
  <tr>
    <td>
      POLYLINE
    </td>
    <td>
      TopoDS_Wire TopoDS_Shell
    </td>
    <td>
      Sequence of trimmed Geom_Line or Geom_Circle
    </td>
    <td>
      For normal polyline (2D or D) Extruded to TopoDS_Edge if thickness is non-zero.
    </td>
  </tr>
  <tr>
    <td>
      VERTEX
    </td>
    <td>
      TopoDS_Vertex
    </td>
    <td>
      Geom_Point
    </td>
    <td>
      Used for the definition of
      POLYLINE entities
    </td>
  </tr>
  <tr>
    <td>
      MLINE
    </td>
    <td>
      Compound of TopoDS_Wire
    </td>
    <td>
      Sequence of Geom_Line
    </td>
    <td>
    </td>
  </tr>
  <tr>
    <td>
      3DFACE
    </td>
    <td>
      TopoDS_Face
    </td>
    <td>
      3-4 Trimmed Geom_Lines
    </td>
    <td>
      If no more than 2 neighboring corners coincide and no bounds intersect.
    </td>
  </tr>
  <tr>
    <td>
    </td>
    <td>
      Compound of TopoDS_Face and TopoDS_Edge
    </td>
    <td>
      3 Trimmed Geom_Lines
    </td>
    <td>
      If one corner lies on an opposite bound.
    </td>
  </tr>
  <tr>
    <td>
    </td>
    <td>
      TopoDS_Edge
    </td>
    <td>
      Trimmed Geom_Line
    </td>
    <td>
      If all corners are on the same line.
    </td>
  </tr>
  <tr>
    <td>
    </td>
    <td>
      TopoDS_Wire
    </td>
    <td>
      2 Trimmed Geom_Lines
    </td>
    <td>
      Else, if opposite corners coincide.
    </td>
  </tr>
  <tr>
    <td>
    </td>
    <td>
      TopoDS_Vertex
    </td>
    <td>
      Geom_Point
    </td>
    <td>
      If all corners coincide.
    </td>
  </tr>
  <tr>
    <td>
      TRACE, SOLID
    </td>
    <td>
      Same as 3DFACE
    </td>
    <td>
      Same as 3DFACE
    </td>
    <td>
      Extruded to TopoDS_Solid if thickness is non-zero.
    </td>
  </tr>
  <tr>
    <td>
      BODY, 3DSOLID, REGION
    </td>
    <td>
      TopoDS_Shape
    </td>
    <td>
    </td>
    <td>
      ACIS body: see ACIS Layer documentation for details
    </td>
  </tr>
  <tr>
    <td>
      BLOCK
    </td>
    <td>
      TopoDS_Compound
    </td>
    <td>
    </td>
    <td>
    </td>
  </tr>
  <tr>
    <td>
      INSERT
    </td>
    <td>
      TopoDS_Compound
    </td>
    <td>
    </td>
    <td>
      Either single block with location, or compound in the case of multiple instances.
    </td>
  </tr>
</table>

<h2><a id="products_user_guides__dxf_interface_5_3">Mapping OCCT shapes to DXF entities</a></h2>

This chapter contains tables showing how the interface converts Open CASCADE Technology objects to DXF entities.

**Geometrical entities**

<table>
  <tr>
    <th align="center">
      OCCT geometry
    </th>
    <th align="center">
      Target DXF entity
    </th>
    <th align="center">
      Comments
    </th>
  </tr>
  <tr>
    <td>
      Geom_Line
    </td>
    <td>
      XLINE
    </td>
    <td>
    </td>
  </tr>
  <tr>
    <td>
      Trimmed Geom_Line
    </td>
    <td>
      LINE or RAY
    </td>
    <td>
      Depending on whether trimming is on both ends or not
    </td>
  </tr>
  <tr>
    <td>
      Geom_Circle
    </td>
    <td>
      CIRCLE
    </td>
    <td>
    </td>
  </tr>
  <tr>
    <td>
      Trimmed Geom_Circle
    </td>
    <td>
      ARC
    </td>
    <td>
    </td>
  </tr>
  <tr>
    <td>
      Geom_Ellipse
    </td>
    <td>
      ELLIPSE
    </td>
    <td>
      Also for trimmed Geom_Ellipse
    </td>
  </tr>
  <tr>
    <td>
      Geom_BezierCurve, Geom_BSplineCurve, Geom_Parabola, Geom_Hyperbola, Geom_OffsetCurve, others
    </td>
    <td>
      SPLINE
    </td>
    <td>
    </td>
  </tr>
  <tr>
    <td>
      Geom_Surface
    </td>
    <td>
      See TOPOLOGICAL ENTITIES table below
    </td>
    <td>
      TopoDS_Face is built from the surface and then written to topology
    </td>
  </tr>
  <tr>
    <td>
      HArray or HSequence of geometries
    </td>
    <td>
      INSERT + BLOCK
    </td>
    <td>
    </td>
  </tr>
  <tr>
    <td>
      NOTE: 2D and 3D curves are processed in the same way.
    </td>
    <td colspan="2">
    </td>
  </tr>
</table>

**Topological entities**

<table>
  <tr>
    <th align="center">
      OCCT geometry
    </th>
    <th align="center">
      Target DXF entity
    </th>
    <th align="center">
      Comments
    </th>
  </tr>
  <tr>
    <td>
      TopoDS_Compound TopoDS_Compsolid
    </td>
    <td>
      INSERT + BLOCK
    </td>
    <td>
      Sharing is preserved on the level of blocks.
    </td>
  </tr>
  <tr>
    <td>
      TopoDS_Solid, TopoDS_Shell, TopoDS_Face
    </td>
    <td>
      BODY (or 3DSOLID, or REGION)
      POLYLINE or TRACE INSERT + BLOCK
    </td>
    <td>
      If parameter write.dxf.acis.mode is ON and shape is not facetted.
      If shape is facetted or parameter write.dxf.acis.mode is OFF
      Shape is exploded on subshapes if parameter write.dxf.acis.mode is OFF and shape cannot be transferred to POLYLINE or TRACE.
    </td>
  </tr>
  <tr>
    <td>
      TopoDS_Wire
    </td>
    <td>
      POLYLINE INSERT + BLOCK
    </td>
    <td>
      If all edges are straight. Exploded on edges.
    </td>
  </tr>
  <tr>
    <td>
      TopoDS_Edge
    </td>
    <td>
      See GEOMETRICAL ENTITIES table
    </td>
    <td>
      According to type of underlying curve
    </td>
  </tr>
  <tr>
    <td>
      TopoDS_Vertex
    </td>
    <td>
      POINT
    </td>
    <td>
    </td>
  </tr>
</table>

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
| Linux	     | GNU gcc 4.3 - 8.3.0 (in framework of listed above Linux OS)<br> LLVM Clang 3+ |     
| Mac OS X   | XCode 6 or newer |
| Android	 | GCC 7.1.0+ (android-ndk-r15+) - Windows<br> LLVM Clang 5.0.300080+ (android-ndk-r15+) - Linux |
| | |
| <b>Library dependencies</b> | |
| Open CASCADE Technology |  Open CASCADE Technology 7.9.0<br> https://dev.opencascade.org/release |
| Intel oneTBB (optional tool<br>for multithreaded algorithms) | oneTBB 2021.5.0<br> https://github.com/oneapi-src/oneTBB |

