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

#include <CAMTest.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <DBRep.hxx>

#include <BOPAlgo_Builder.hxx>
#include <BRep_Builder.hxx>
#include <Geodesic_MakeOffset.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <ShapeFix_ShapeTolerance.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TCollection_AsciiString.hxx>

//=======================================================================
//function : CAMgoffset
//purpose  : 
//=======================================================================

static Standard_Integer CAMgoffset(Draw_Interpretor& /*dout*/, Standard_Integer n, const char** a)
{
  // Check the command arguments
  if (n < 3) {
    std::cout<<"Error: "<<a[0]<<" - invalid number of arguments"<<std::endl;
    std::cout<<"Usage: type help "<<a[0]<<std::endl;
    return 1; //TCL_ERROR
  }

  // Get the straightening mode.
  Geodesic_StraighteningMode aStraighteningMode = Geodesic_StraighteningMode_NONE;
  if (n != 0 && strlen(a[n - 1]) >= 11)
  {
    TCollection_AsciiString aParam = a[n - 1];
    aParam.Trunc(11);
    if (!strcmp(aParam.ToCString(), "-straighten"))
    {
      aStraighteningMode = Geodesic_StraighteningMode_ALL;
      aParam = a[n - 1];
      aParam.Remove(1, 11);
      --n;
      if (aParam.IsIntegerValue())
      {
        aStraighteningMode = (Geodesic_StraighteningMode )aParam.IntegerValue();
      }
    }
  }

  //read shape
  TopoDS_Shape aShape = DBRep::Get(a[2]);
  if (aShape.IsNull()) {
    std::cout<<"Error: "<<a[2]<<" is null"<<std::endl;
    return 1; //TCL_ERROR
  }

  TopoDS_Shape aFront;
  if ( n > 3 )
    aFront = DBRep::Get( a[3] );

  Standard_Integer ip = 4;
  if ( aFront.IsNull() )
  {
    ip = 3;
    std::cout<<"Info: using free boundaries as front shape"<<std::endl;

    ShapeAnalysis_FreeBounds aFreeBndTool( aShape );
    const TopoDS_Compound& aGuide = aFreeBndTool.GetClosedWires();
    if ( aGuide.IsNull() ) {
      std::cout<<"Error: no closed free boundaries"<<std::endl;
      return 1; //TCL_ERROR
    }

    aFront = aGuide;
  }

  const Standard_Integer aNbLines = ( n > ip )? Draw::Atoi( a[ip++] ) : 6;
  const Standard_Real    anOffset = ( n > ip )? Draw::Atof( a[ip++] ) : 1.0;
  const Standard_Real  aTolerance = ( n > ip )? Draw::Atof( a[ip++] ) : 0.001;
  const Standard_Boolean isRemesh = ( n > ip )? ( Draw::Atoi( a[ip++] ) != 0 ) : Standard_True;
  const Standard_Real  aPrecision = ( n > ip )? Draw::Atof( a[ip++] ) : 0.001;
  const Standard_Real  anAngleTol = ( n > ip )? Draw::Atof( a[ip++] ) : 0.1;

  Geodesic_MakeOffset aTool;
  aTool.SetForcedMesh( isRemesh );
  aTool.SetMeshingPrecision( aPrecision );
  aTool.SetMeshingAngleTol ( anAngleTol );
  aTool.SetShape( aShape );
  aTool.SetFront( aFront );
  aTool.SetNbLines( aNbLines );
  aTool.SetOffsetStep( anOffset );
  aTool.SetTolerance( aTolerance );

  Handle(TopTools_HSequenceOfShape) anIsos = aTool.Compute(aStraighteningMode);

  if ( anIsos.IsNull() ) {
    std::cout<<"Error: no result"<<std::endl;
    return 1; //TCL_ERROR
  }

  BRep_Builder B;
  TopoDS_Compound aResult;

  for ( Standard_Integer i = 1; i <= anIsos->Length(); ++i )
  {
    const TopoDS_Shape& anIso = anIsos->Value( i );
    if ( anIso.IsNull() )
      continue;

    if ( aResult.IsNull() )
      B.MakeCompound( aResult );

    B.Add( aResult, anIso );
  }

  if ( aResult.IsNull() ) {
    std::cout<<"Error: no result"<<std::endl;
    return 1; //TCL_ERROR
  }

  DBRep::Set( a[1], aResult );
  return 0; //TCL_OK
}

//==============================================================================
// Function: CAMgpath.
//
// Description.
// Builds a shortest path from vertex a[3] to vertex a[4] on shape a[2].
// Stores teh result in a[1] variable, as TopoDS_Wire object.
// Uses a[5] optional parameter as triangulation precision.
//==============================================================================

static Standard_Integer CAMgpath(Draw_Interpretor & /*dout*/, Standard_Integer n, const char ** a)
{
  // Check the command arguments
  if (n < 5) {
    std::cout<<"Error: "<<a[0]<<" - invalid number of arguments"<<std::endl;
    std::cout<<"Usage: type help "<<a[0]<<std::endl;
    return 1; //TCL_ERROR
  }

  //read shapes
  TopoDS_Shape aShape = DBRep::Get(a[2]);
  if (aShape.IsNull()) {
    std::cout<<"Error: "<<a[2]<<" is null"<<std::endl;
    return 1; //TCL_ERROR
  }

  TopoDS_Shape aSeed  = DBRep::Get(a[3]);
  if (aSeed.IsNull() || aSeed.ShapeType() != TopAbs_VERTEX) {
    std::cout<<"Error: "<<a[3]<<" is not a vertex"<<std::endl;
    return 1; //TCL_ERROR
  }

  TopoDS_Shape aDestS = DBRep::Get(a[4]);
  if (aDestS.IsNull() || aDestS.ShapeType() != TopAbs_VERTEX) {
    std::cout<<"Error: "<<a[4]<<" is not a vertex"<<std::endl;
    return 1; //TCL_ERROR
  }

  //read tolerance
  Standard_Boolean anRemesh(Standard_False);
  Standard_Real aTol(0.1);
  if (n > 5) {
    aTol = Draw::Atof(a[5]);
    anRemesh = Standard_True;
  }
  std::cout<<"Info: tolerance is set to "<<aTol<<std::endl;

  Geodesic_MakeOffset aTool;
  aTool.SetForcedMesh(anRemesh);
  aTool.SetMeshingPrecision(aTol);
  aTool.SetMeshingAngleTol(0.1);
  aTool.SetShape(aShape);
  aTool.SetFront(aSeed);
  if (!aTool.BuildField()) {
    std::cout<<"Error: field building failed"<<std::endl;
    return 1; //TCL_ERROR
  }

  TopoDS_Wire aPath;
  TopoDS_Vertex aDestV = TopoDS::Vertex(aDestS);
  aTool.BuildPath(aDestV, aPath);
  if (aPath.IsNull()) {
    std::cout<<"Error: no result"<<std::endl;
    return 1; //TCL_ERROR
  }

  DBRep::Set(a[1], aPath);
  return 0; //TCL_OK
}

//=======================================================================
//function : CAMproject
//purpose  : 
//=======================================================================

static Standard_Integer CAMproject (Draw_Interpretor& dout,
                                    Standard_Integer n, const char** a)
{
  // Check the command arguments
  if (n < 5) {
    std::cout << "Not enough arguments!" << std::endl;
    return 1;
  }
  Standard_Real aTol(-1.);
  if (n > 5) {
    const Standard_Real tol = atof(a[5]);
    if (tol > 0.001)
      aTol = tol;
  }
  const TopoDS_Shape aShape = DBRep::Get(a[3]);
  const TopoDS_Shape aEdges = DBRep::Get(a[4]);
  if (aShape.IsNull() || aEdges.IsNull()) {
    std::cout << "Invalid surface or wire!" << std::endl;
    return 1;
  }

  Standard_Boolean isProjectionNeeded(Standard_False);
  // Find all edges that have no PCurve representation on faces in 'aShape'
  // and collect these edges in a compound shape
  for (TopExp_Explorer expEdge(aEdges, TopAbs_EDGE); expEdge.More(); expEdge.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(expEdge.Current());
    TopExp_Explorer expFace(aShape, TopAbs_FACE);
    for (; expFace.More(); expFace.Next()) {
      Standard_Real dummy[2];
      const TopoDS_Face& aFace = TopoDS::Face(expFace.Current());
      const Handle(Geom2d_Curve) aPCurve =
        BRep_Tool::CurveOnSurface(anEdge, aFace, dummy[0], dummy[1]);
      if (aPCurve.IsNull() == Standard_False)
        break;
    }
    if (expFace.More() == Standard_False) {
      const Standard_Real aTolEdge = BRep_Tool::Tolerance(anEdge);
      if (aTol < aTolEdge)
        aTol = aTolEdge;
      isProjectionNeeded = Standard_True;
    }
  }

  if (!isProjectionNeeded) {
    DBRep::Set(a[1], aShape);
    DBRep::Set(a[2], aEdges);
  } else {
    ShapeFix_ShapeTolerance().SetTolerance(aEdges, aTol);
    BOPAlgo_Builder aBld;
    aBld.AddArgument(aShape);
    for (TopExp_Explorer expEdge(aEdges, TopAbs_EDGE); expEdge.More(); expEdge.Next())
	{
      TopoDS_Edge anEdge = TopoDS::Edge(expEdge.Current());
      aBld.AddArgument(anEdge);
    }

    aBld.Perform();
    aBld.DumpErrors (std::cout);
//    aBld.DumpWarnings (std::cout);
    if (aBld.HasErrors()) {
      return 1;
    }
    // Extract the Shell/Face from the result compound
    TopoDS_Iterator itSh(aBld.Shape());
    for (; itSh.More(); itSh.Next())
    {
      const TopoDS_Shape& aResShape = itSh.Value();
      if (aResShape.ShapeType() == TopAbs_SHELL ||
          aResShape.ShapeType() == TopAbs_FACE  ||
          aResShape.ShapeType() == TopAbs_SOLID)
      {
        DBRep::Set(a[1], aResShape);
        break;
      }
    }
    if (itSh.More() == Standard_False) {
      std::cout << "No valid result after projection of Wire to Surface" << std::endl;
      return 1;
    }
    TopoDS_Compound aComp;
    BRep_Builder().MakeCompound(aComp);
    for (TopExp_Explorer expEdge(aEdges, TopAbs_EDGE); expEdge.More(); expEdge.Next())
	{
      const TopTools_ListOfShape& lstMod = aBld.Modified(expEdge.Current());
      TopTools_ListIteratorOfListOfShape itRes(lstMod);
      for (; itRes.More(); itRes.Next())
        BRep_Builder().Add(aComp, itRes.Value());
    }
    DBRep::Set(a[2], aComp);
    dout << "DONE projection to surface\n";
  }
  return 0;
}

//=======================================================================
//function : GeodesicCommands
//purpose  : 
//=======================================================================

void CAMTest::GeodesicCommands (Draw_Interpretor& theCommands)
{
  const char * g = "CAM commands";

  theCommands.Add("CAMgoffset", "CAMgoffset result shape [frontWire] [NbLines [Step [Tol [Remesh [Precision [AngularTol]]]]]] [-straighten]", __FILE__, CAMgoffset, g);
  theCommands.Add("CAMgpath", "CAMgpath result shape start end [tol]", __FILE__, CAMgpath, g);
  theCommands.Add("CAMproject", "CAMproject rsurf redges surf edges [tol]", __FILE__, CAMproject, g);
}
