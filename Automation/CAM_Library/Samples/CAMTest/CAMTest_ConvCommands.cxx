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
#include <DrawTrSurf.hxx>
#include <Draw_Interpretor.hxx>
#include <DBRep.hxx>
#include <Geom_Line.hxx>
#include <TopoDS.hxx>
#include <BRep_Builder.hxx>
#include <BRepAdaptor_CompCurve.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <Poly_Polygon3D.hxx>
#include <Precision.hxx>

//=======================================================================
//function : CAMwire2poly
//purpose  : Convert a planar wire to polygon
//=======================================================================

static Standard_Integer CAMwire2poly(Draw_Interpretor& /*dout*/, Standard_Integer n, const char** a)
{
  // Check the command arguments
  if (n < 3) {
    std::cout << "Error: " << a[0] << " - invalid number of arguments" << std::endl;
    std::cout << "Usage: type help " << a[0] << std::endl;
    return 1; //TCL_ERROR
  }

  // Read input wire
  TopoDS_Wire aWire = TopoDS::Wire(DBRep::Get(a[2], TopAbs_WIRE));
  if (aWire.IsNull())
  {
    std::cout << "Error: " << a[2] << " is not a wire" << std::endl;
    return 1; //TCL_ERROR
  }

  //read tolerance
  Standard_Real aTol(0.01);
  if (n > 3)
    aTol = Draw::Atof(a[3]);

  // tessellate wire
  BRepAdaptor_CompCurve aCompCurve(aWire);
  GCPnts_QuasiUniformDeflection aQUD(aCompCurve, aTol);
  if (!aQUD.IsDone())
  {
    std::cout << "Error: failure tessellating the wire";
    return 1;
  }
  TColgp_Array1OfPnt aPoints(1, aQUD.NbPoints());
  for (Standard_Integer i = 1; i <= aQUD.NbPoints(); i++)
    aPoints(i) = aQUD.Value(i);

  // create polygon
  Handle(Poly_Polygon3D) aPoly3d = new Poly_Polygon3D(aPoints);

  DrawTrSurf::Set(a[1], aPoly3d);
  return 0; //TCL_OK
}

//=======================================================================
//function : CAMpoly2wire
//purpose  : Convert a polygon to wire
//=======================================================================

static Standard_Integer CAMpoly2wire(Draw_Interpretor& /*dout*/, Standard_Integer n, const char** a)
{
  // Check the command arguments
  if (n < 3) {
    std::cout << "Error: " << a[0] << " - invalid number of arguments" << std::endl;
    std::cout << "Usage: type help " << a[0] << std::endl;
    return 1; //TCL_ERROR
  }

  // Read input polygon
  Handle(Poly_Polygon3D) aPoly = DrawTrSurf::GetPolygon3D(a[2]);
  if (aPoly.IsNull())
  {
    std::cout << "Error: " << a[2] << " is not a polygon" << std::endl;
    return 1; //TCL_ERROR
  }

  // create result wire
  TopoDS_Wire aWire;
  BRep_Builder().MakeWire(aWire);
  TopoDS_Vertex aLastVer;
  gp_Pnt aLastP;
  const TColgp_Array1OfPnt& aNodes = aPoly->Nodes();
  for (Standard_Integer i = aNodes.Lower(); i <= aNodes.Upper(); i++)
  {
    const gp_Pnt& aP = aNodes(i);
    TopoDS_Vertex aVer;
    BRep_Builder().MakeVertex(aVer, aP, Precision::Confusion());
    if (!aLastVer.IsNull())
    {
      gp_Vec aVec(aLastP, aP);
      Standard_Real aLen = aVec.Magnitude();
      if (aLen > Precision::Confusion())
      {
        TopoDS_Edge aE;
        Handle(Geom_Line) aCrv = new Geom_Line(aLastP, aVec);
        BRep_Builder().MakeEdge(aE, aCrv, Precision::Confusion());
        BRep_Builder().Add(aE, aLastVer);
        BRep_Builder().Add(aE, aVer.Oriented(TopAbs_REVERSED));
        BRep_Builder().Range(aE, 0., aLen);
        BRep_Builder().Add(aWire, aE);
      }
    }
    aLastVer = aVer;
    aLastP = aP;
  }

  DBRep::Set(a[1], aWire);
  return 0; //TCL_OK
}

//=======================================================================
//function : ConvCommands
//purpose  :
//=======================================================================

void CAMTest::ConvCommands (Draw_Interpretor& theCommands)
{
  const char* g = "CAM commands";

  theCommands.Add("CAMwire2poly", "CAMwire2poly result wire [tol]", __FILE__, CAMwire2poly, g);
  theCommands.Add("CAMpoly2wire", "CAMpoly2wire result poly", __FILE__, CAMpoly2wire, g);
}
