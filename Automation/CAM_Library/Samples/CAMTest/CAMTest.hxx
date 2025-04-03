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

#ifndef CAMTest_HeaderFile
#define CAMTest_HeaderFile

class Draw_Interpretor;

//! @mainpage
//! This is the documentation of the Draw commands of CAM Library component.<br>
//! See the modules:
//! - \ref PolyAlgo
//! - \ref Geodesic
//! The listed Draw commands can be executed in DRAW command prompt after starting DRAWEXE.exe executable.
//! Before execution, the commands should be loaded as plugin using the command <tt><b>pload</b></tt>

class CAMTest
{
 public:
  static void Factory(Draw_Interpretor& theDI);

 protected:

  //! @defgroup CAMconvert Commands for conversion of contours
  //! @page test
  //! @ingroup CAMconvert

  //! @section CAMconvert CAMconvert

  static void ConvCommands(Draw_Interpretor& theDI);

  //! @defgroup PolyAlgo Commands for 2D polygons
  //! @page test
  //! @ingroup PolyAlgo

  //! @section CAMpsection CAMpsection
  //! Intersection between a shape's triangulation and a plane.<br>
  //! This command takes a shape, builds a triangulation on it and creates a set of curves
  //! according to intersection between the triangulation and a given plane.
  //! The shape must be a closed volume, because this command only produces closed 2d curves.
  //! <p>
  //! This command interfaces the class PolyAlgo_PlaneTrianglesSection.
  //! @par Syntax
  //! <tt><b>CAMpsection result shape plane [tol]</b></tt><br>
  //! where:
  //! - <tt>result</tt> is the prefix for naming the result 3d polygons.
  //!  If we provide here 'res' and there are 3 closed polygons
  //!  then they will be named 'res_1', 'res_2', 'res_3'.
  //! - <tt>shape</tt> is the input shape.
  //!  It should either SOLID or SHELL or a compound thereof.
  //!  This shape should be previously triangulated using the Draw command <tt>'incmesh'</tt>.
  //! - <tt>plane</tt> is the plane object previously created in Draw.
  //! - <tt>tol</tt> - optional parameter, the tolerance for meshing as well as
  //!  the precision of the algorithm collecting closed loops.

  //! @section CAMpoutline CAMpoutline
  //! This command interfaces the class PolyAlgo_Outline
  //! @par Syntax
  //! <tt><b>CAMpoutline result shape plane [tol]</b></tt><br>
  //! where:
  //! - <tt>result</tt> is the prefix for naming the result 3d polygons.
  //!  If we provide here 'res' and there are 3 closed polygons
  //!  then they will be named 'res_1', 'res_2', 'res_3'.
  //! - <tt>shape</tt> is the input shape.
  //!  It may be SOLID or SHELL or even a set of isolated faces.
  //!  This shape should be previously triangulated using the Draw command <tt>'incmesh'</tt>.
  //! - <tt>plane</tt> is the plane object previously created in Draw.
  //!  The result is one or more closed outline contours obtained by orthogonal projection on this plane.
  //! - <tt>tol</tt> - optional parameter, the tolerance for the outline contour.

  //! @section CAMpoffset CAMpoffset
  //! This command interfaces the class PolyAlgo_Offset2d
  //! @par Syntax
  //! <tt><b>CAMpoffset [-e|-s] [-z] result poly3d radius [tol]</b></tt><br>
  //! where:
  //! - <tt>result</tt> is the prefix for naming the result 3d polygons. If we
  //!  If we provide here 'res' and there are 3 closed polygons
  //!  then they will be named 'res_1', 'res_2', 'res_3'.
  //! - <tt>poly3d</tt> - one or more (if more then - space-delimited list) 3d planar polygons that should be offset.
  //!  The result from \ref CAMpsection can be used here immediately.
  //! - <tt>radius</tt> - offset radius.
  //! - <tt>tol</tt> - optional parameter, the tolerance for the offset contour,
  //!  meaning the maximal deflection from the exact solution.
  //! - Switch -z orders to build zones from 2d contours on the result of Offset
  //! algorithm.<br>
  //! - Switch -e or -s tells that the result will contain only Expanded or Shrunk contours, respectively.
  //! Without any of these switches the result contains both expanded and shrunk contours.

  //! @section CAMpocket CAMpocket
  //! This command interfaces the class PolyAlgo_MakePocket.
  //! @par Syntax
  //! <tt><b>CAMpocket result face gap radius fromExternalFlag tol</b></tt><br>
  //! where:
  //! - <tt>result</tt> - the resulting face.
  //! - <tt>face</tt> - the input face.
  //! - <tt>gap</tt> - the distance to be added only to the most external shape.
  //! - <tt>radius</tt> - the tool radius.
  //! - <tt>fromExternalFlag</tt> - the flag indicating where to start connecting shapes
  //!   (from external (true) or from internal (false)).
  //! - <tt>tol</tt> - the tolerance for computation.

  static void PolyCommands(Draw_Interpretor& theDI);

  //! @defgroup Geodesic Commands for shortest path computation
  //! @page test
  //! @ingroup Geodesic

  //! @section CAMgoffset CAMgoffset
  //! This command interfaces the class Geodesic_MakeOffset.
  //! @par Syntax
  //! <tt><b>CAMgoffset result shape frontWire [NbLines [Step [Tol [ForceRemesh [ Precision [AngularTol]]]]]]</b></tt><br>
  //! where:
  //! - <tt>result</tt> - the resulting compound of wires.
  //! - <tt>shape</tt> - the input shape.
  //! - <tt>frontWire</tt> - the input front wire.
  //! - <tt>NbLines</tt> - the number of offset lines.
  //! - <tt>Step</tt> - the offset step.
  //! - <tt>Tol</tt> - the tolerance for computation.
  //! - <tt>ForceRemesh</tt> - the flag indicating if remeshing is needed (set to 0 if omitted).
  //! - <tt>Precision</tt> - the meshing precision (set to 0.001 if omitted).
  //! - <tt>AngularTol</tt> - the meshing angular tolerance (set to 0.1 if omitted).

  //! @section CAMgpath CAMgpath
  //! This command interfaces the class Geodesic_MakeOffset.
  //! @par Syntax
  //! <tt><b>CAMgpath result shape source destination [tol]</b></tt><br>
  //! where:
  //! - <tt>result</tt> - the resulting compound of wires.
  //! - <tt>shape</tt> - the input shape.
  //! - <tt>source</tt> - the input start vertex.
  //! - <tt>destination</tt> - the input end vertex.
  //! - <tt>tol</tt> - the tolerance for computation.

  static void GeodesicCommands(Draw_Interpretor& theDI);
};

#endif
