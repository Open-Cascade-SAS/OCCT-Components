#     SAT Interface User's Guide 

<h1><a id="products_user_guides__sat_interface_1">Introduction</a></h1>

<h2><a id="products_user_guides__sat_interface_1_1">Overview</a></h2>

<h3><a id="products_user_guides__sat_interface_1_1_1">This manual</a></h3>

This manual provides technical documentation on the OPEN CASCADE SAT (Save As Text) interface and explains how to use this interface for reading and writing ACIS SAT files from within Open CASCADE Technology based software.

SAT files in format versions 1.02 to 1.0 can be read while the resulting files conform to the SAT 1.06 to 1.0 standards (the target version is user-selectable). Compatibility with the next versions is not ensured.

The interface offers two levels of SAT data support:

* The _basic interface_ supports only geometrical and topological SAT entities that are translated to and from Open CASCADE Technology shapes.
* The _XDE interface_ extends the basic interface with translation of attributes (colors and names) attached to entities, to and from XDE data structures.

The interface loads all data from the SAT file including data that cannot be translated.<br>During the translation process, the interface finds and translates entities that have correspondence in Open CASCADE Technology representation.

<h3><a id="products_user_guides__sat_interface_1_1_2">Reference documentation</a></h3>

It is not the purpose of this manual to document the file format that will be converted to the representation used by Open CASCADE Technology. Information on this file format is only provided insofar as necessary to explain the actual operations of the interface.

For more information on the SAT format, consult the appropriate external documentation.

<h3><a id="products_user_guides__sat_interface_1_1_3">What the SAT Interface does</a></h3>

The SAT Interface provides tools for bidirectional translation of geometrical and topological data between SAT files and Open CASCADE Technology native BRep models (known as shapes). It also supports attributes (names and colors) that can be translated to Open CASCADE Technology using XDE.

The SAT Interface provides high level of data exchange quality through the use of Shape Healing which corrects shapes after translation. The user of the interface benefits from the standard data exchange features of Open CASCADE Technology: file analysis, checking the transfer result, total or partial transfer, and use of attributes (with XDE).

For information on Shape Healing and XDE, see the _Shape Healing User’s Guide_ and the _XDE User’s Guide_ available within Open CASCADE Technology.

<h2><a id="products_user_guides__sat_interface_1_2">Testing with Draw</a></h2>

The standard DRAWEXE executable from Open CASCADE Technology can be used for testing SAT interface. The plugin library TKXSDRAWSAT provides specific commands for reading and writing SAT files.

For more information on standard DRAW commands, see the _Test Harness User’s Guide_ available within Open CASCADE Technology.

For more information on testing SAT interface with Draw see the [Draw commands interface](#products_user_guides__sat_interface_4).

<h1><a id="products_user_guides__sat_interface_2">Reading SAT files</a></h1>

<h2><a id="products_user_guides__sat_interface_2_1">Overview</a></h2>

This chapter explains what is read from SAT files, how it is done and the methods available to get the most out of the process.

<h2><a id="products_user_guides__sat_interface_2_2">Domains covered</a></h2>

The following kinds of entities can be read from SAT files:

* Wireframe
* Shells
* Solids

For more information, see the chapter [Mapping SAT entities to OCCT shapes](#products_user_guides__sat_interface_5_2).

**Note:** Name and color attributes attached to topological entities are translated to XDE data structures. Other attributes are just loaded from SAT but not translated. For more information on using XDE for reading, refer to the chapter [\"Reading with XDE](#products_user_guides__sat_interface_2_4).

<h2><a id="products_user_guides__sat_interface_2_3">General procedure</a></h2>

You can translate a SAT file to Open CASCADE Technology shapes as follows:

1. Load the file.
2. Check the file consistency (optional).
3. Set the translation parameters.
4. Perform the file translation.
5. Retrieve the results.

The following sections explain how this is done and what resources to use.

<h3><a id="products_user_guides__sat_interface_2_3_1">Loading a SAT file</a></h3>

Load a SAT file as follows:

~~~{.cpp}
   SatControl_Reader aReader;
   IFSelect_ReturnStatus stat = aReader.ReadFile("MyFile.sat");
~~~

Loading the file just places the data into memory, no translation actually occurs.

**For advanced users**

As soon as the SAT file is loaded into memory, the SAT model can be accessed for querying. Obtain the SAT model as follows:

~~~{.cpp}
   Handle(Interface_InterfaceModel) Model = aReader.WS()->Model();
~~~

This model can then be queried to get the number of entities, to iterate and access entities, etc. For more information, refer to the description of the class Interface_InterfaceModel in the Open CASCADE Technology documentation.

<h3><a id="products_user_guides__sat_interface_2_3_2">Checking the SAT file</a></h3>

Check the loaded file as follows:

~~~{.cpp}
   aReader.PrintCheckLoad (failsonly, mode);
~~~

Error messages are generated if there are invalid or incomplete SAT entities, giving you information on the cause of the error. The error messages are written to the current trace file (or stream). For more information on error messages, refer to the paragraph [Translation messages](#products_user_guides__sat_interface_2_3_7).

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
    <th align="center">Origin</th>
    <th align="center">Result in</th>
  </tr>
  <tr>
    <td>ItemsByEntity</td>
    <td>A sequential list of all messages per SAT entity.</td>
  </tr>
  <tr>
    <td>CountByItem</td>
    <td>The number of SAT entities with their types per message.</td>
  </tr>
  <tr>
    <td>ShortByItem</td>
    <td>The number of SAT entities with their types per message and displays the rank numbers of the first five SAT entities per message.</td>
  </tr>
  <tr>
    <td>ListByItem</td>
    <td>The number of SAT entities with their type and rank numbers per message.</td>
  </tr>
  <tr>
    <td>EntitiesByItem</td>
    <td>The number of SAT entities with their type and rank numbers and Directory Entry numbers per message.</td>
  </tr>
</table>

<h3><a id="products_user_guides__sat_interface_2_3_3">Setting the translation parameters</a></h3>

You can set translation parameters within the program or via the resource file.

You can use the methods provided by the class `Interface_Static` to query and set the translation parameters within the program. For example:

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

When you set the translation parameters, remember that if you provide a value for translation parameters outside the range of possible values, the parameter is ignored.

To read shapes from a SAT file, the following interface parameters are available:

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
    <td>read.sat.resource.name</td>
    <td>The name of the resource file for shape processing performed after translation.</td>
  </tr>
  <tr>
    <td>read.sat.sequence</td>
    <td>Prefix of parameters in the resource file for shape processing performed after translation.</td>
  </tr>
  <tr>
    <td>read.sat.parallel.mode</td>
    <td>If On, entities are read in parallel (to speed up reading on multi-core configurations)<br>If Off, entities are read sequentially</td>
  </tr>
</table>

<h3><a id="products_user_guides__sat_interface_2_3_4">Performing the SAT file translation</a></h3>

Select the entities you want to translate and then perform their translation.

**Select the entities to translate**

You can select either:

* the entire file,
* a list of entities,
* a single entity.

**The entire file**

Transferring a whole file means transferring all root entities (all ACIS bodies).

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

A list of entities can be formed with help of SatControl_Reader::GiveList() method.

For example, to form a list containing entities with sequence numbers 1, 2 and 3:

~~~{.cpp}
   Handle(TColStd_HSequenceOfTransient) list =
   aReader.GiveList("(-1,-2,-3)");
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

There are several predefined selections that can be used to create lists:

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
    <td>Translatable roots (default). Refer to chapter Mapping OCCT shapes to SAT entities for more information on which SAT entities can be translated.<td>
  </tr>
  <tr>
    <td>xst-sharing + &lt;selection&gt;</td>
    <td>Entities sharing at least one entity selected by &lt;selection&gt;.</td>
  </tr>
  <tr>
    <td>xst-shared + &lt;selection&gt;</td>
    <td>Entities shared by at least one entity selected by &lt;selection&gt;.</td>
  </tr>
</table>

By default (no arguments), `SatControl_Reader::GiveList()` returns a list of all roots.

**Selection by rank**

Select an entity by its rank as follows:

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

**Translate the selected entities**

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

<h3><a id="products_user_guides__sat_interface_2_3_5">Getting the translation results</a></h3>

Each successful translation operation outputs one shape. A series of translations gives a series of shapes.

Each time you invoke `TransferOne`, `TransferEntity` or `TransferList`, their results are accumulated and the counter returned by `NbShapes()` method increases.

You can clear the results between two translation operations with:

~~~{.cpp}
   aReader.ClearShapes();
~~~

If you do not, the results from the next translation will be added to the accumulation.
`TransferRoots` operations automatically clear all existing results before they start.

Get the number of shapes recorded in the result as follows:

~~~{.cpp}
   Standard_Integer nbs = aReader.NbShapes();
~~~

Get the result \<num\> (\<num\> is an integer between 1 and NbShapes), as follows:

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

<h3><a id="products_user_guides__sat_interface_2_3_6">Checking that translation was correctly performed</a></h3>

Each time you invoke `TransferOneRoot` or `TransferRoots`, you can display the related messages as follows:

~~~{.cpp}
   aReader.PrintCheckTransfer (failsonly, mode);
~~~

This check concerns only the last invocation of `TransferOneRoot` or `TransferRoots`.

<h3><a id="products_user_guides__sat_interface_2_3_7">Translation messages</a></h3>

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

The root class `XSControl_Reader` provides to `SatControl_Reader` the following methods to print messages associated to the entities during loading and translation:


~~~{.cpp}
   XSControl_Reader::PrintCheckLoad(failsonly,mode)
   XSContriol_Reader::PrintCheckTransfer(failsonly,mode)
~~~

where `failsonly` set to True enables printing of fails only messages, or all messages if set to False, and mode determines the contents and the order of the messages according to the terms of the `IFSelect_PrintCount` enumeration.

<h3><a id="products_user_guides__sat_interface_2_3_8">Tolerance management</a></h3>

During translation of a SAT file to Open CASCADE Technology the value of precision written for each geometrical SAT entity in the SAT file is used for all topological entities built from it.

<h3><a id="products_user_guides__sat_interface_2_3_9">Setting Shape Processing parameters</a></h3>

The last phase of translation of a SAT file to Open CASCADE Technology shapes is Shape Processing: a sequence of operations can be applied to the produced shape, to correct problems and adapt it for further use.

The sequence of Shape Processing operations and their parameters are defined in a resource file whose name is given by the interface parameter **read.sat.resource.name** (the default value is <b>"SAT"</b>).

The sample resource file is provided in the resource unit `XSTEPResourceAdv`. The environment variable with the corresponding name `CSF_SATDefaults` should point to the directory where that file is located.

The sequence whose name is given by the interface parameter **read.sat.sequence** (the default value is <b>"FromSAT"</b>) is used. If a resource file is not found, the default sequence containing the single operator `FixShape` with default parameters is applied.

<h2><a id="products_user_guides__sat_interface_2_4">Reading with XDE</a></h2>

You can use XDE to read colors, names, and assemblies. When using XDE, the standard process of reading a SAT file is extended by steps ensuring the correct interpretation of these attributes.

<h3><a id="products_user_guides__sat_interface_2_4_1">Loading a SAT file</a></h3>

Load a SAT file as follows:

~~~{.cpp}
   SATCAFControl_Reader aReader;
   IFSelect_ReturnStatus stat = aReader.ReadFile("MyFile.sat");
~~~

Loading the file just places the data into memory, no translation actually occurs.

<h3><a id="products_user_guides__sat_interface_2_4_2">Setting the translation parameters</a></h3>

The standard parameters are set first, see the paragraph [Setting the translation parameters](#products_user_guides__sat_interface_2_3_3).
 Then the following additional parameters are available to control translation of attributes from SAT to XDE:

* For transferring colors:

~~~{.cpp}
   aReader.SetColorMode(mode);
~~~

// mode can be `Standard_True` or `Standard_False`

* For transferring names:

~~~{.cpp}
   aReader.SetNameMode(mode);
~~~

// mode can be `Standard_True` or `Standard_False`

<h3><a id="products_user_guides__sat_interface_2_4_3">Performing the translation to an XDE documen</a></h3>

Perform the translation of the whole file into the document as follows:

~~~{.cpp}
   Standard_Boolean ok = aReader.Transfer(doc);
~~~

where doc is a variable which contains a handle to the output document and has a type `Handle(TDocStd_Document)`.

<h2><a id="products_user_guides__sat_interface_2_5">Example</a></h2>

An example for SAT file reading follows:

<i><b>Example</b></i>
~~~{.cpp}
   #include <SatControl_Reader.hxx>
   #include <TopoDS_Shape.hxx>
   #include <IFSelect_ReturnStatus.hxx>
   int main()
   {
    SatControl_Reader aReader;
    // Load file MyFile.sat
    IFSelect ReturnStatus stat = aReader.ReadFile ("MyFile.sat");
    // Get the number of transferrable roots
    Standard_Integer num, nbroots = aReader.NbRootsForTransfer();
    cout<<"Nb Roots recognized for transfer = "<<nbroots<<endl;
    for (num = 1; num <= nbroots; num ++)
    {
     // Each SAT root is transferred individually
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

<h1><a id="products_user_guides__sat_interface_3">Writing SAT files</a></h1>

<h2><a id="products_user_guides__sat_interface_3_1">Overview</a></h2>

This chapter explains how SAT files are written, and the methods available to get the most out of the process.

<h2><a id="products_user_guides__sat_interface_3_2">General procedure</a></h2>

You can write a SAT file as follows:

1. Initialize the process.
2. Set the translation parameters.
3. Perform the model translation.
4. Write the output SAT file.

The following sections explain how this is done and what resources to use.

<h3><a id="products_user_guides__sat_interface_3_2_1">Initializing the process</a></h3>

Initialize the process as follows:

~~~{.cpp}
   SatControl_Writer aWriter;
~~~

<h3><a id="products_user_guides__sat_interface_3_2_2">Setting the translation parameters</a></h3>

You can use the methods provided by the class `Interface_Static` to query and set the translation parameters. For example:

Read the parameter <b>"write.precision.val"</b> as follows:

~~~{.cpp}
   Standard_Real rp =
   Interface_Static::RVal("write.precision.val");
~~~

Modify this parameter <b>"write.precision.val"</b> as follows:

~~~{.cpp}
   if (!Interface_Static::SetRVal ("write.precision.val",0.001))
   { /* error */; }
~~~

To write shapes to a SAT file, use the following interface parameters:

<table>
  <tr>
    <th align="center">Name</th>
    <th align="center">Description</th>
  </tr>
  <tr>
    <td>write.precision.mode</td>
    <td>If -1 (Min), the uncertainty value is set to minimal tolerance of OCCT sub-shapes.<br>
        If 0 (Average), the uncertainty value is set to average tolerance of OCCT sub-shapes.<br>
        If 1 (Max), the uncertainty value is set to maximal tolerance of OCCT sub-shapes.<br>
        If 2 (User), the uncertainty value is set to write.precision.val
    </td>
  </tr>
  <tr>
    <td>write.precision.val</td>
    <td>Value of uncertainty (if write.precision.mode is 2)</td>
  </tr>
  <tr>
    <td>write.surfacecurve.mode</td>
    <td>Currently not used</td>
  </tr>
  <tr>
    <td>write.sat.header.product</td>
    <td>Product name that is written into the file.</td>
  </tr>
  <tr>
    <td>write.sat.header.version</td>
    <td>Version in which file is written.</td>
  </tr>
  <tr>
    <td>write.sat.resource.name</td>
    <td>The name of the resource file for shape processing performed prior to translation.</td>
  </tr>
  <tr>
    <td>write.sat.sequence</td>
    <td>Prefix of parameters in the resource file for shape processing performed prior to translation.</td>
  </tr>
</table>

<h3><a id="products_user_guides__sat_interface_3_2_3">Performing the shape translation</a></h3>

You can perform the translation in one or several operations. Translate one shape as follows:

~~~{.cpp}
   IFSelect_ReturnStatus stat = aWriter.TransferShape(shape);
~~~

where shape is a `TopoDS_Shape`.

<h3><a id="products_user_guides__sat_interface_3_2_4">Writing the output file</a></h3>

Write the SAT output file as follows:

~~~{.cpp}
   IFSelect_ReturnStatus stat = aWriter.WriteFile("MyFile.sat");
~~~

<h3><a id="products_user_guides__sat_interface_3_2_5">Tolerance management</a></h3>

When writing a SAT file, tolerance of each geometrical entity such as a surface or a curve is computed according to the parameters write.precision.mode and write.precision.val.

<h3><a id="products_user_guides__sat_interface_3_2_6">Translation messages</a></h3>

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

The root class `XSControl_Writer` provides to `SatControl_Writer` the following method to print messages associated to the entities during translation:

~~~{.cpp}
   XSContriol_Writer::PrintStatsTransfer(what,mode)
~~~

<h3><a id="products_user_guides__sat_interface_3_2_7">Setting Shape Processing parameters</a></h3>

The first phase of translation of an Open CASCADE Technology shape to a SAT file is Shape Processing: a sequence of operations can be applied to the input shape thereby preparing it for translation.

The sequence of Shape Processing operations and their parameters are defined in a resource file whose name is given by the interface parameter **write.sat.resource.name** (the default value is <b>"SAT"</b>).

The sample resource file is provided in the resource unit `XSTEPResourceAdv`. The environment variable with the corresponding name `CSF_SATDefaults` should point to the directory where that file is located.

The sequence whose name is given by the interface parameter **write.sat.sequence** (the default value is <b>"ToSAT"</b>) is used. If a resource file is not found, a default sequence containing the single operator DirectFaces is applied.

<h2><a id="products_user_guides__sat_interface_3_3">Writing using XDE</a></h2>

Using XDE, you can write colors, names, and assemblies. When using XDE, the standard process of writing a SAT file is extended by steps ensuring the correct interpretation of attributes.

<h3><a id="products_user_guides__sat_interface_3_3_1">Initializing the translation process</a></h3>

Initialize the process as follows:

~~~{.cpp}
   SATCAFControl_Writer aWriter;
~~~

<h3><a id="products_user_guides__sat_interface_3_3_2">Setting the translation parameters</a></h3>

The standard parameters are set first, see the paragraph [Setting the translation parameters](#products_user_guides__sat_interface_3_2_2). Then the following additional parameters are available to control translation of attributes from XDE to SAT:

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

<h3><a id="products_user_guides__sat_interface_3_3_3">Performing the translation of an XDE document</a></h3>

You can perform the translation of a document as follows:

~~~{.cpp}
   IFSelect_ReturnStatus aRetSt = aWriter.Transfer(doc);
~~~

where doc is a variable which contains a handle to the input document for transferring and should have a type `Handle(TDocstd_Document)`.

<h3><a id="products_user_guides__sat_interface_3_3_4">Writing the SAT file</a></h3>

Write the SAT file as follows:

~~~{.cpp}
   IFSelect_ReturnStatus stat = aWriter.WriteFile("MyFile.sat");
~~~

<h2><a id="products_user_guides__sat_interface_3_4">Example</a></h2>

An example for SAT file writing follows:

<i><b>Example</b></i>
~~~{.cpp}
   #include <SatControl_Writer.hxx>
   #include <TopoDS_Shape.hxx>
   #include <BRepTools.hxx>
   int main()
   {
     TopoDS_Shape sh;
     SatControl_Writer aWriter;
     // Translate TopoDS_Shape into a SAT entity
     IFSelect_ReturnStatus stat = aWriter.TransferShape(sh);
     // Write resulting entity to the SAT file
     stat = aWriter.WriteFile("MyFile.sat");
     return 0;
   }
~~~

<h1><a id="products_user_guides__sat_interface_4">Draw commands interface</a></h1>

<h2><a id="products_user_guides__sat_interface_4_1">Overview</a></h2>

This chapter explains how to use Draw Test Harness (DRAWEXE) executable for testing OPEN CASCADE SAT Interface interactively in Draw environment. It describes a set of Draw commands specific to data exchange tasks, which allow loading and writing of data files and analysis of the resulting data structures and shapes.

**NOTE:** In the description of the commands, square brackets ([ ]) are used to indicate optional parameters. Parameters given in the angle brackets (< >) and numbers (#) are to be substituted with the appropriate values. When several exclusive variants are possible, vertical dash (|) is used.

<h2><a id="products_user_guides__sat_interface_4_2">General description</a></h2>

<h3><a id="products_user_guides__sat_interface_4_2_1">Loading plugin commands</a></h3>

A set of commands to work with Data Exchange interfaces can be loaded in Draw as follows:

~~~{.cpp}
   Draw[] pload DATAEXCHANGE
~~~

A set of commands to work with SAT interface in particular can be loaded in Draw as follows:

~~~{.cpp}
   Draw[] pload –DrawPluginProducts SAT
~~~

<h3><a id="products_user_guides__sat_interface_4_2_2">Setting interface parameters</a></h3>

A set of parameters for Data Exchange interfaces is defined in XSTEP component of Open CASCADE Technology. In Draw, view or change these parameters as follows:

~~~{.cpp}
   param [<parameter_name> [<value>]]
~~~

The **param** command with no arguments gives a list of all parameters with their values. When the argument \<parameter_name\> is specified, information about this parameter is printed (current value and short description). The third argument is used to set a new value of the given parameter. The result of the setting is printed immediately.

In all interface operations, the process protocol (fail and warning messages, mapping of the loaded entities into the Open CASCADE Technology shapes, etc.) can be written to the trace file. Two parameters are defined in the Draw session: trace level (integer value from 0 to 9, default is 0), and trace file (default is standard output).

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

<h3><a id="products_user_guides__sat_interface_4_2_3">Reading SAT files</a></h3>

Read a SAT file as follows:

~~~{.cpp}
   satread <file_name> <result_shape_name> [<entity> ...]
~~~

Where entity can be defined by a number in the:

* File using sign '-' or '$' with number (identifies an entity by its sequence number in the file).
* File using letter 'l' or 'L' with number (identifies an entity by its line number in the file).
* Number in the model using number only.

You can define all roots by using sign '*'.

To transfer entities from the last loaded model use "." instead of the file name.

<h3><a id="products_user_guides__sat_interface_4_2_4">Analyzing the transferred data</a></h3>

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
    <td>Calculates how many entities of each CASCADE type exist</td>
  </tr>
</table>

List the entity types which were encountered in the last loaded file (with number of SAT entities of each type) as follows:

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

<h3><a id="products_user_guides__sat_interface_4_2_5">Writing SAT files</a></h3>

Write the shape into the SAT file as follows:

~~~{.cpp}
   satwrite <shape_name> <file name>
~~~

Several shapes can be written into one file by making a compound of them. For making a compound refer to the "Test Harness User’s Guide" of Open CASCADE Technology.

<h3><a id="products_user_guides__sat_interface_4_2_6">Index of useful commands</a></h3>

<table>
  <tr>
    <th align="center">Command</th>
    <th align="center">Description</th>
  </tr>
  <tr>
    <td colspan="2">
      <b>Setting general parameters</b>
    </td>
  </tr>
  <tr>
    <td>xtrace [#|&lt;file&gt;|.]</td>
    <td>View and set parameters of the trace file</td>
  </tr>
  <tr>
    <td>param [&lt;param&gt; [&lt;val&gt;]]</td>
    <td>View and set parameters of transfer</td>
  </tr>
  <tr>
    <td colspan="2">
      <b>Reading and writing SAT file</b>
    </td>
  </tr>
  <tr>
    <td>xload &lt;file&gt;</td>
    <td>Loads entities from the file into memory</td>
  </tr>
  <tr>
    <td>satread {&lt;file&gt;} &lt;name&gt;</td>
    <td>Translates SAT file into shape</td>
  </tr>
  <tr>
    <td>satwrite &lt;shape&gt; [&lt;file-name.sat&gt;]</td>
    <td>Translates a shape into SAT file</td>
  </tr>
  <tr>
    <td colspan="2">
      <b>Checking the results of the load procedure</b>
    </td>
  </tr>
  <tr>
    <td>data &lt;symbol&gt;</td>
    <td>Gets statistics on the loaded file</td>
  </tr>
  <tr>
    <td>listitems</td>
    <td>Gets list of all defined special objects like selec­tions and counters</td>
  </tr>
  <tr>
    <td>count &lt;counter&gt; [&lt;selection&gt;]</td>
    <td>Counts entities by counter</td>
  </tr>
  <tr>
    <td>listcount &lt;counter&gt; [&lt;selection&gt;]</td>
    <td>Counts entities by counter and list them</td>
  </tr>
  <tr>
    <td>givelist &lt;selection&gt;</td>
    <td>Gets list of subset of loaded entities defined by selection</td>
  </tr>
  <tr>
    <td>listtypes [&lt;selection&gt;]</td>
    <td>Gets statistics on the types of the entities loaded</td>
  </tr>
  <tr>
    <td>entity &lt;entity&gt; &lt;level&gt;</td>
    <td>Gets information about entities</td>
  </tr>
  <tr>
    <td>estatus</td>
    <td>Gets information about super- and sub- entities.</td>
  </tr>
  <tr>
    <td colspan="2">
      <b>Analyzing of the loaded shapes</b>
    </td>
  </tr>
  <tr>
    <td>checkshape &lt;shape&gt;</td>
    <td>Checks shape for internal errors</td>
  </tr>
  <tr>
    <td>statshape &lt;shape&gt;</td>
    <td>Gets statistics on the shape</td>
  </tr>
  <tr>
    <td>tolerance &lt;shape&gt;</td>
    <td>Calculates tolerances for the given shape</td>
  </tr>
</table>

<h2><a id="products_user_guides__sat_interface_4_3">BRep level commands</a></h2>

These commands are useful for translation of SAT models to and from OCCT shapes.

<h3><a id="products_user_guides__sat_interface_4_3_1">satload</a></h3>

Purpose: Loads a SAT file and writes the model back to another file if necessary.

Syntax:  `satload <file_name> [<out_file_name>]`

Where:

* `<file_name>` is a name of file for loading;
* `<out_file_name>` is a name of file for writing. If `<out_file_name>` is omitted writing is not performed.

<i><b>Example</b></i>
~~~{.cpp}
   # load a model from file
   satload D:/files/model.sat
~~~

<h3><a id="products_user_guides__sat_interface_4_3_2">satread</a></h3>

Purpose: Reads a SAT file to an OCCT shape.

Syntax:  `satread <file_name> <result_shape_name> [<entity>]`

Where:

* `<file_name>` is a name of file for reading;
* `<result_shape_name>` is a name of the resulting shape;
* `<entity>` is an auxiliary parameter. If `<entity>` is omitted all roots will be translated as a set of shapes. If parameter `<entity>` is used as a number only the corresponding entity will be translated. If symbol ‘*’ is used as `<entity>` all roots will be translated as one compound.

<i><b>Example</b></i>
~~~{.cpp}
   # translate all roots from file to the shape "a"
   satread D:/files/model.sat a *
~~~

<h3><a id="products_user_guides__sat_interface_4_3_3">satreadsimple</a></h3>

Purpose: Reads a SAT file to an OCCT shape.

Syntax:  `satreadsimple <file_name> <result_shape_name> `

Where:

* `<file_name>` is a name of file for reading;
* `<result_shape_name>` is a name of the resulting shape

<i><b>Example</b></i>
~~~{.cpp}
   # translate all roots from file to the shape "a"
   satreadsimple D:/files/model.sat a
~~~

<h3><a id="products_user_guides__sat_interface_4_3_4">satwrite</a></h3>

Purpose: Writes an OCCT shape to a SAT file.

Syntax:  `satwrite <shape_name> <file_name>`

Where:

* `<shape_name>` is a name of shape;
* `<file_name>` is a name of file for writing

<i><b>Example</b></i>
~~~{.cpp}
   # write a shape "a" to a SAT file
   satwrite a D:/files/result.sat
~~~

<h3><a id="products_user_guides__sat_interface_4_3_5">satwritesimple</a></h3>

Purpose: Writes an OCCT shape to a SAT file.

Syntax:  `satwritesimple <shape_name> <file_name>`

Where:

* `<shape_name>` is a name of shape;
* `<file_name>` is a name of file for writing

<i><b>Example</b></i>
~~~{.cpp}
   # write a shape "a" to a SAT file
   satwritesimple a D:/files/result.sat
~~~

<h3><a id="products_user_guides__sat_interface_4_3_6">satsave</a></h3>

Purpose: Writes a model from the current session to a SAT file.

Syntax:  `satsave <file_name>`

Where:

* `<file_name>` is a name of file for writing

<i><b>Example</b></i>
~~~{.cpp}
   # write a current model to a SAT file
   satsave D:/files/result.sat
~~~

<h3><a id="products_user_guides__sat_interface_4_3_7">satstatentity</a></h3>

Purpose: Dumps statistics for a given SAT file.

Syntax:  `satstatentity <file_name>`

Where:

* `<file_name>` is a name of file for reading

<i><b>Example</b></i>
~~~{.cpp}
   # dump statistics
   satstatentity D:/files/model.sat
~~~

<h2><a id="products_user_guides__sat_interface_4_4">XDE level commands</a></h2>

<h3><a id="products_user_guides__sat_interface_4_4_1">ReadSat</a></h3>

Purpose: Reads a SAT file to an XCAF document.

Syntax:  `ReadSat <doc_name> <file_name> `

Where:

* `<file_name>` is a name of file for reading;
* `<doc_name>` is a name of resulting XCAF document

<i><b>Example</b></i>
~~~{.cpp}
   # translate all roots from file to the document "D1"
   ReadSat D1 D:/files/model.sat
~~~

<h3><a id="products_user_guides__sat_interface_4_4_2">WriteSat</a></h3>

Purpose: Writes an XCAF document to a SAT file.

Syntax:  `WriteSat <doc_name> <file_name>`

Where:

* `<doc_name>` is a name of XCAF document;
* `<file_name>` is a name of file for writing

<i><b>Example</b></i>
~~~{.cpp}
   # write document "D1" to a DXF file
   WriteSat D1 D:/files/result.sat
~~~

<h1><a id="products_user_guides__sat_interface_5">Appendix</a></h1>

<h2><a id="products_user_guides__sat_interface_5_1">Packaging</a></h2>

<h3><a id="products_user_guides__sat_interface_5_1_1">API classes of SAT Interface</a></h3>

**The SatControl_Controller class**

This class performs all necessary initializations to use the SAT norm.

**The SatControl_Reader class**

This class translates SAT entities into Open CASCADE Technology shapes.

**The SatControl_Writer class**

This class translates Open CASCADE Technology shapes to SAT entities.

**The AcisData_AcisModel class**

This class represents a SAT file in the memory and provides access to SAT entities specified by rank number. This class can be used to consult and modify a SAT file before translation or writing.

**The SATCAFControl_Reader class**

This class translates SAT entities into Open CASCADE Technology XCAF documents.

**The SATCAFControl_Writer class**

This class translates Open CASCADE Technology XCAF documents to SAT entities.

<h2><a id="products_user_guides__sat_interface_5_2">Mapping SAT entities to OCCT shapes</a></h2>

This chapter contains tables showing how the interface converts SAT entities to Open CASCADE Technology objects.

**NOTE:** SAT entity types that are not listed in the following tables are not translated.

<h3><a id="products_user_guides__sat_interface_5_2_1">Geometrical entities</a></h3>

<table>
  <tr>
    <th align="center">ACIS class (SAT ID)</th>
    <th align="center">OCCT object</th>
    <th align="center">Comments</th>
  </tr>
  <tr>
    <td>curve (none)</td>
    <td>Geom_Curve</td>
    <td>Provides methods and data common to all curve subclasses.</td>
  </tr>
  <tr>
    <td>ellipse (ellipse)</td>
    <td>Geom_Ellipse,<br>Geom_Circle</td>
    <td>Defines an ellipse.</td>
  </tr>
  <tr>
    <td>intcurve (intcurve)</td>
    <td>Geom_BSplineCurve</td>
    <td>Defines an interpolated curve type.</td>
  </tr>
  <tr>
    <td>straight (straight)</td>
    <td>Geom_Line</td>
    <td>Defines an infinite straight line repre&shy;sented by a point and a unit vector specifying the direction.</td>
  </tr>
  <tr>
    <td>discontinuity_info (none)</td>
    <td>GeomAbs_Shape</td>
    <td>Stores discontinuity information for a curve or surface.</td>
  </tr>
  <tr>
    <td>pcurve (none)</td>
    <td>Geom2d</td>
    <td>Defines a 2D curve defined in the parameter space of a parametric sur&shy;face.</td>
  </tr>
  <tr>
    <td>int_cur (int_cur)</td>
    <td>Geom_BSplineCurve</td>
    <td>Defines interpolated curves.</td>
  </tr>
  <tr>
    <td>exact_int_cur (exactcur)</td>
    <td>Geom_BSplineCurve</td>
    <td>Represents an exact intersection spline curve.</td>
  </tr>
  <tr>
    <td>int_int_cur (surfintcur)</td>
    <td>Geom_BSplineCurve</td>
    <td>Represents the interpolated curves obtained from the intersection of two surfaces.</td>
  </tr>
  <tr>
    <td>law_int_cur (lawintcur)</td>
    <td>Geom_BSplineCurve *</td>
    <td>Defines a curve from a law.</td>
  </tr>
  <tr>
    <td>off_int_cur (offintcur)</td>
    <td>Geom_BSplineCurve</td>
    <td>Represents a spline curve obtained by the intersection of two surfaces that are offsets of the given surfaces.</td>
  </tr>
  <tr>
    <td>off_surf_int_cur (offsurfin)</td>
    <td>Geom_BSplineCurve *</td>
    <td>Defines an int_cur subtype which is offset from a curve lying on a surface along the surface normal.</td>
  </tr>
  <tr>
    <td>offset_int_cur (offsetint)</td>
    <td>Geom_BSplineCurve *</td>
    <td>Represents an offset curve, which is offset in any plane.</td>
  </tr>
  <tr>
    <td>proj_int_cur (projcur)</td>
    <td>Geom_BSplineCurve</td>
    <td>Implements an interpolated curve subtype to represent the perpendicu&shy;lar projection of a curve to a surface.</td>
  </tr>
  <tr>
    <td>par_int_cur (parcur)</td>
    <td>Geom_BSplineCurve</td>
    <td>Represents an exact spline curve in the parameter space of a surface.</td>
  </tr>
  <tr>
    <td>subset_int_cur (subsetin)</td>
    <td>Geom_BSplineCurve *</td>
    <td>Represents a subset of a longer curve.</td>
  </tr>
  <tr>
    <td>surf_int_cur (surfcur)</td>
    <td>Geom_BSplineCurve</td>
    <td>Represents spline curves on a sur&shy;face within the given fit tolerance.</td>
  </tr>
  <tr>
    <td>par_cur (exppc)</td>
    <td>Geom2d_BSplineCurve</td>
    <td>Defines methods for working with the fitted spline itself.</td>
  </tr>
  <tr>
    <td>exp_par_cur (exppc)</td>
    <td>Geom2d_BSplineCurve</td>
    <td>Defines an explicit parameter-space curve.</td>
  </tr>
  <tr>
    <td>imp_par_cur (imppc)</td>
    <td>Geom2d_BSplineCurve</td>
    <td>Defines an implicit parameter-space curve.</td>
  </tr>
  <tr>
    <td>law_par_cur (lawpc)</td>
    <td>Geom2d_BSplineCurve *</td>
    <td>Creates a law parameter curve as a subtype of par_cur.</td>
  </tr>
  <tr>
    <td>spl_sur (spl_sur)</td>
    <td>Geom_BSplineSurface</td>
    <td>Defines an abstract base class from which spline surface definitions are derived.</td>
  </tr>
  <tr>
    <td>exact_spl_sur (exactsur)</td>
    <td>Geom_BSplineSurface</td>
    <td>Represents an exact spline surface.</td>
  </tr>
  <tr>
    <td>law_spl_sur (lawsur)</td>
    <td>Geom_BSplineSurface *</td>
    <td>Creates a surface using a law.</td>
  </tr>
  <tr>
    <td>net_spl_sur (netsur)</td>
    <td>Geom_BSplineSurface *</td>
    <td>Defines a net surface between a list of curves.</td>
  </tr>
  <tr>
    <td>off_spl_sur (offsur)</td>
    <td>Geom_OffsetSurface of Geom_BSplineSurface</td>
    <td>Represents the offset of a surface.</td>
  </tr>
  <tr>
    <td>rot_spl_sur (rotsur)</td>
    <td>Geom_SurfaceOfRevoluti on</td>
    <td>Represents a surface of rotation.</td>
  </tr>
  <tr>
    <td>skin_spl_sur (skinsur)</td>
    <td>Geom_BSplineSurface *</td>
    <td>Defines a skin surface between a list of curves.</td>
  </tr>
  <tr>
    <td>sub_spl_sur (subsur)</td>
    <td>Geom_BSplineSurface *</td>
    <td>Represents the geometry of a spline surface, which is a subset region of another spl_sur.</td>
  </tr>
  <tr>
    <td>sum_spl_sur (sumsur)</td>
    <td>Geom_SurfaceOfLinearExtrusion</td>
    <td>Represents a linear sum of two curves.</td>
  </tr>
  <tr>
    <td>sweep_spl_sur (sweepsur)</td>
    <td>Geom_BSplineSurface</td>
    <td>Defines the perpendicular sweep of a planar profile curve along a path curve.</td>
  </tr>
  <tr>
    <td>taper_spl_sur (tapersur)</td>
    <td>Geom_BSplineSurface *</td>
    <td>Describes a tapered edge surface.</td>
  </tr>
  <tr>
    <td>edge_tpr_spl_sur (edgetap)</td>
    <td>Geom_BSplineSurface *</td>
    <td>Class to describe a tapered edge sur&shy;face.</td>
  </tr>
  <tr>
    <td>ruled_tpr_spl_sur (ruledtap)</td>
    <td>Geom_BSplineSurface *</td>
    <td>Describes a ruled-tapered surface, in which a surface is tapered about an edge by a constant angle relative to a draft angle.</td>
  </tr>
  <tr>
    <td>swept_tpr_spl_sur (sweptta)</td>
    <td>Geom_BSplineSurface *</td>
    <td>Describes a swept tapered surface.</td>
  </tr>
  <tr>
    <td>ortho_spl_sur (orthosur)</td>
    <td>Geom_BSplineSurface *</td>
    <td>Describes an orthogonal surface, which can arise in face_taper.</td>
  </tr>
  <tr>
    <td>shadow_tpr_spl_sur (shadowt)</td>
    <td>Geom_BSplineSurface *</td>
    <td>Describes a shadow-tapered surface that is tapered by a constant angle about a silhouette.</td>
  </tr>
  <tr>
    <td>tube_spl_sur (tubesur)</td>
    <td>Geom_BSplineSurface</td>
    <td>Describes a surface that is the enve&shy;lope of a fixed-radius circle.</td>
  </tr>
  <tr>
    <td>pipe_spl_sur (pipesur)</td>
    <td>Geom_BSplineSurface</td>
    <td>Describes a surface that is the enve&shy;lope of a fixed-radius circle centered on a point on a given curve.</td>
  </tr>
  <tr>
    <td>surface (none)</td>
    <td>Geom_Surface</td>
    <td>Base class for all ACIS surface types that defines the basic virtual func&shy;tions that are supplied for all specific surface classes.</td>
  </tr>
  <tr>
    <td>cone (cone)</td>
    <td>Geom_ConicalSurface, Geom_CylindricalSurface, Geom_Plane, Geom_BSplineSurface</td>
    <td>Defines the elliptical single cone.</td>
  </tr>
  <tr>
    <td>plane (plane)</td>
    <td>Geom_Plane</td>
    <td>Defines a planar surface.</td>
  </tr>
  <tr>
    <td>sphere (sphere)</td>
    <td>Geom_SphericalSurface</td>
    <td>Defines a spherical surface.</td>
  </tr>
  <tr>
    <td>spline (spline)</td>
    <td>Geom_BSplineSurface</td>
    <td>Records a B-spline surface.</td>
  </tr>
  <tr>
    <td>torus (torus)</td>
    <td>Geom_ToroidalSurface</td>
    <td>Represents torus.</td>
  </tr>
  <tr>
    <td>blend_int_cur (bldcur)</td>
    <td>Geom_BSplineCurve</td>
    <td>Defines an interpolated curve sub&shy;type.</td>
  </tr>
  <tr>
    <td>spring_int_cur (blndspr)</td>
    <td>Geom_BSplineCurve</td>
    <td>Defines one long edge of a general blend surface, such as a spring inter&shy;section curve.</td>
  </tr>
  <tr>
    <td>VBL_SURF (vertexbl)</td>
    <td>Geom_BSplineSurface</td>
    <td>Defines the vertex blend surface class.</td>
  </tr>
  <tr>
    <td>VBL_OFFSURF (offsetvb)</td>
    <td>Geom_OffsetSurface</td>
    <td>Representation of an offset vertex blend surface.</td>
  </tr>
  <tr>
    <td>blend_spl_sur (blend_s)</td>
    <td>Geom_BSplineSurface</td>
    <td>Provides common functionality and data for all blend surfaces.</td>
  </tr>
  <tr>
    <td>rb_blend_spl_sur (rbblnsur)</td>
    <td>Geom_BSplineSurface</td>
    <td>Implements the constant radius roll&shy;ing ball blend surface.</td>
  </tr>
  <tr>
    <td>var_blend_sdl_sur (varblend)</td>
    <td>Geom_BSplineSurface</td>
    <td>Implementation of the base class for variable radius and other non-pipe blends. Derived from blend_spl_sur.</td>
  </tr>
  <tr>
    <td>crv_crv_v_bl_spl_sur (crvcrvbl)</td>
    <td>Geom_BSplineSurface *</td>
    <td>Implements the variable-radius edge-edge blend surface.</td>
  </tr>
  <tr>
    <td>crv_srf_v_bl_spl_sur (crvsrfbl)</td>
    <td>Geom_BSplineSurface *</td>
    <td>Implements the variable-radius edge-face blend surface.</td>
  </tr>
  <tr>
    <td>sfcv_free_bl_spl_sur (sfcvfree)</td>
    <td>Geom_BSplineSurface *</td>
    <td>Implements the variable-radius sur&shy;face-curve/free blend surface.</td>
  </tr>
  <tr>
    <td>srf_srf_v_bl_spl_sur (srfsrfbln)</td>
    <td>Geom_BSplineSurface</td>
    <td>Implements the variable-radius face-face blend surface.</td>
  </tr>
</table>

<p>* these entities are translated using their b-spline representation (only when it is present)</p>

<h3><a id="products_user_guides__sat_interface_5_2_2">Topological entities</a></h3>

<table>
  <tr>
    <th align="center">ACIS class (SAT ID)</th>
    <th align="center">OCCT object</th>
    <th align="center">Comments</th>
  </tr>
  <tr>
    <td>BODY (body)</td>
    <td>TopoDS_Solid or TopoDS_Compound</td>
    <td>Represents a wire, sheet, or solid body.</td>
  </tr>
  <tr>
    <td>COEDGE (coedge)</td>
    <td>TopoDS_Edge</td>
    <td>Relates EDGEs with adjacent EDGEs and owning entities.</td>
  </tr>
  <tr>
    <td>EDGE (edge)</td>
    <td>TopoDS_Edge</td>
    <td>Represents a physical edge.</td>
  </tr>
  <tr>
    <td>FACE (Face)</td>
    <td>TopoDS_Face</td>
    <td>Represents a bounded portion of a SURFACE.</td>
  </tr>
  <tr>
    <td>LOOP (Loop)</td>
    <td>TopoDS_Wire</td>
    <td>Set of connected EDGEs that bound a FACE.</td>
  </tr>
  <tr>
    <td>LUMP (Lump)</td>
    <td>TopoDS_Solid</td>
    <td>Represents a bounded, connected portion of space.</td>
  </tr>
  <tr>
    <td>SHELL (Shell)</td>
    <td>TopoDS_Shell</td>
    <td>Represents the external boundary of a LUMP, or the internal boundary of a void (unoccupied space) within a LUMP</td>
  </tr>
  <tr>
    <td>SUBSHELL (Subshell)</td>
    <td>TopoDS_Shell</td>
    <td>Represents a subdivision of a SHELL or SUBSHELL.</td>
  </tr>
  <tr>
    <td>VERTEX (Vertex)</td>
    <td>TopoDS_vertex</td>
    <td>Represents a corner of a FACE and/ or the end of an EDGE.</td>
  </tr>
  <tr>
    <td>WIRE (Wire)</td>
    <td>TopoDS_Compound (of TopoDS_Edge)</td>
    <td>Represents a collection of edges that are owned by a body but are not attached to faces and do not enclose any volume.</td>
  </tr>
</table>

<h3><a id="products_user_guides__sat_interface_5_2_3">Attributes translated to XDE</a></h3>

<table>
  <tr>
    <th align="center">ACIS class (SAT ID)</th>
    <th align="center">OCCT object</th>
    <th align="center">Comments</th>
  </tr>
  <tr>
    <td>ATTRIB_COL (attrib_col)</td>
    <td>XCAFDoc_Color</td>
    <td>Identifies color by predefined index.</td>
  </tr>
  <tr>
    <td>ATTRIB_RGB (atrib_rgb)</td>
    <td>XCAFDoc_Color</td>
    <td>Defines color by RGB values.</td>
  </tr>
  <tr>
    <td>ATTRIB_GEN_NAME (attrib)</td>
    <td>TDataStd_Name</td>
    <td>Allows attaching arbitrary (application-dependent) string to an entity.</td>
  </tr>
  <tr>
    <td>ATTRIB_GEN_STRING (attrib)</td>
    <td>TDataStd_NamedData</td>
    <td>Allows attaching property pair Key : Value to an entity.</td>
  </tr>
</table>

<h2><a id="products_user_guides__sat_interface_5_3">Mapping OCCT shapes to SAT entities</a></h2>

This chapter contains tables showing how the interface converts Open CASCADE Technology objects to SAT entities.

<h3><a id="products_user_guides__sat_interface_5_3_1">Geometrical shapes</a></h3>

<table>
  <tr>
    <th align="center">OCCT object</th>
    <th align="center">SAT entity</th>
    <th align="center">Comments</th>
  </tr>
  <tr>
    <td>Points</td>
    <td> </td>
    <td> </td>
  </tr>
  <tr>
    <td>Geom_CartesianPoint</td>
    <td>Point</td>
    <td> </td>
  </tr>
  <tr>
    <td>Curves</td>
    <td> </td>
    <td> </td>
  </tr>
  <tr>
    <td>Geom_Line</td>
    <td>Line</td>
    <td> </td>
  </tr>
  <tr>
    <td>Geom_Circle</td>
    <td>Circle</td>
    <td> </td>
  </tr>
  <tr>
    <td>Geom_Ellipse</td>
    <td>Ellipse</td>
    <td> </td>
  </tr>
  <tr>
    <td>Geom_Hyperbola</td>
    <td>IntCurve</td>
    <td> </td>
  </tr>
  <tr>
    <td>Geom_Parabola</td>
    <td>IntCurve</td>
    <td> </td>
  </tr>
  <tr>
    <td>Geom_BezierCurve</td>
    <td>IntCurve</td>
    <td> </td>
  </tr>
  <tr>
    <td>Geom_BSplineCurve</td>
    <td>IntCurve</td>
    <td> </td>
  </tr>
  <tr>
    <td>
      Geom_TrimmedCurve<br>
      Geom_Line<br>
      Geom_Circle<br>
      Geom_Ellipse<br>
      Geom_Hyperbola<br>
      Geom_Parabola<br>
      Geom_BezierCurve<br>
      Geom_BSplineCurve<br>
    </td>
    <td>
      Line<br>
      Arc<br>
      Ellipse<br>
      IntCurve<br>
      IntCurve<br>
      IntCurve<br>
      IntCurve<br>
    </td>
    <td> </td>
  </tr>
  <tr>
    <td>Geom_OffseCurve</td>
    <td>IntCurve</td>
    <td> </td>
  </tr>
  <tr>
    <td>Surfaces</td>
    <td> </td>
    <td> </td>
  </tr>
  <tr>
    <td>Geom_Plane</td>
    <td>Plane</td>
    <td> </td>
  </tr>
  <tr>
    <td>Geom_ConicalSurface</td>
    <td>Cone</td>
    <td> </td>
  </tr>
  <tr>
    <td>Geom_CylindricalSurface</td>
    <td>Cone</td>
    <td> </td>
  </tr>
  <tr>
    <td>Geom_SphericalSurface</td>
    <td>Sphere</td>
    <td> </td>
  </tr>
  <tr>
    <td>Geom_ToroidalSurface</td>
    <td>Torus</td>
    <td> </td>
  </tr>
  <tr>
    <td>Geom_SurfaceOfLinearExtrusion</td>
    <td>
      Cone<br>
      Spline<br>
    </td>
    <td>
      Cone if the curve obtained from the projection of the basis curve to a plane, whose direction is the same as the extrusion direction, is a circle or an ellipse. Spline otherwise.
    </td>
  </tr>
  <tr>
    <td>Geom_SurfaceOfRevolution</td>
    <td>Spline</td>
    <td> </td>
  </tr>
  <tr>
    <td>Geom_BezierSurface</td>
    <td>Spline</td>
    <td> </td>
  </tr>
  <tr>
    <td>Geom_BSplineSurface</td>
    <td>Spline</td>
    <td> </td>
  </tr>
  <tr>
    <td>Geom_RectangularTrimmedSurface</td>
    <td> </td>
    <td>A Geom_RectangularTrimmedSurface is transferred as an ACIS Sur­face following its basis sur­face.</td>
  </tr>
  <tr>
    <td>OffsetSurface</td>
    <td>Spline</td>
    <td> </td>
  </tr>
</table>

<h3><a id="products_user_guides__sat_interface_5_3_2">Topological shapes</a></h3>

<table>
  <tr>
    <th align="center">OCCT object</th>
    <th align="center">SAT entity</th>
    <th align="center">Comments</th>
  </tr>
  <tr>
    <td>TopoDS_Vertex</td>
    <td>Vertex</td>
    <td> </td>
  </tr>
  <tr>
    <td>TopoDS_Edge</td>
    <td>Edge</td>
    <td> </td>
  </tr>
  <tr>
    <td>TopoDS_Wire</td>
    <td>Loop</td>
    <td>An ACIS loop contains Edge entities if TopoDS_Wire is isolated and CoEdge entities if Wire is the boundary of a shape.</td>
  </tr>
  <tr>
    <td>TopoDS_Face</td>
    <td>Face</td>
    <td> </td>
  </tr>
  <tr>
    <td>TopoDS_Shell</td>
    <td>Shell</td>
    <td> </td>
  </tr>
  <tr>
    <td>TopoDS_Solid</td>
    <td>Lump</td>
    <td> </td>
  </tr>
  <tr>
    <td>TopoDS_CompSolid TopoDS_Compound</td>
    <td>Body</td>
    <td>Sub-shapes of TopoDS_Compound or TopoDS_CompSolid become separate lumps in the body.</td>
  </tr>
</table>

<h3><a id="products_user_guides__sat_interface_5_3_3">Attributes</a></h3>

<table>
  <tr>
    <th align="center">OCCT object</th>
    <th align="center">SAT entity</th>
    <th align="center">Comments</th>
  </tr>
  <tr>
    <td>XCAFDoc_Color</td>
    <td>ATTRIB_COL ATTRIB_RGB</td>
    <td>Colors can be translated both as RGB attributes and attributes with standard colors.</td>
  </tr>
  <tr>
    <td>TDataStd_Name</td>
    <td>ATTRIB_GEN_NAME</td>
    <td> </td>
  </tr>
</table>

<h1><a id="products_user_guides__sat_requisites">System Requirements</a></h1>

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
| Intel oneTBB (optional tool<br>for multithreaded algorithms) | oneTBB 2021.5.0<br> https://www.threadingbuildingblocks.org/ |
