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
#include <Draw_Appli.hxx>
#include <Draw_Interpretor.hxx>
#include <DrawTrSurf.hxx>
#include <DBRep.hxx>
#include <Geom_Plane.hxx>
#include <PolyAlgo_Offset2d.hxx>
#include <PolyAlgo_Outline.hxx>
#include <PolyAlgo_PlaneTrianglesSection.hxx>
#include <PolyAlgo_MakePocket.hxx>
#include <Poly2d_Curve.hxx>
#include <Poly_Polygon3D.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx> 
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Pln.hxx>
#include <BRep_Tool.hxx>

#ifdef _WIN32
Standard_IMPORT Draw_Viewer dout;
#endif

//=======================================================================
//function : CheckIsPlanar
//purpose  : 
//=======================================================================

static Standard_Boolean CheckIsPlanar (const Handle(Poly_Polygon3D) &thePoly,
                                       gp_Ax2&                      thePlane,
                                       const Standard_Real          theDefl)
{
  const TColgp_Array1OfPnt &aNodes = thePoly->Nodes();
  const Standard_Integer idLower(aNodes.Lower());
  const Standard_Integer idUpper(aNodes.Upper());

  Standard_Boolean aResult(Standard_False);
  Standard_Integer anUpper(idUpper);
  static const Standard_Real Preci2(Precision::Confusion() *
                                    Precision::Confusion());
  if (aNodes.Value(idLower).SquareDistance(aNodes.Value(anUpper)) < Preci2)
    anUpper--;

  Standard_Integer aLen = anUpper - idLower + 1;
  if (aLen < 3)
    return Standard_False;

  // Compute Newell normal of a contour
  gp_XYZ aNormal(0., 0., 0.);
  gp_XYZ aCenter (0., 0., 0.);
  Standard_Integer i;
  for (i = idLower; i <= anUpper; i++)
  {
    Standard_Integer im1(i - 1);
    if (im1 < idLower)
      im1 = anUpper;
    Standard_Integer ip1(i + 1);
    if (ip1 > idUpper)
      ip1 = idLower;
    const gp_XYZ& aP   = aNodes.Value(i).XYZ();
    const gp_XYZ& aPm1 = aNodes.Value(im1).XYZ();
    const gp_XYZ& aPp1 = aNodes.Value(ip1).XYZ();
    aNormal += gp_XYZ (aP.Y() * (aPp1.Z() - aPm1.Z()),
                       aP.Z() * (aPp1.X() - aPm1.X()),
                       aP.X() * (aPp1.Y() - aPm1.Y()));
    aCenter += aP;
  }

  Standard_Real anArea = aNormal.Modulus(); // twice the real area
  if (anArea > Precision::Confusion())
  {
    aResult = Standard_True;
    aNormal /= anArea;
    aCenter /= aLen;

    Standard_Real aDev2 = theDefl * theDefl;
    if (theDefl < Precision::Confusion()) {
      if (thePoly->Deflection() < Precision::Confusion())
        aDev2 = Precision::Confusion() * Precision::Confusion();
      else
        aDev2 = thePoly->Deflection() * thePoly->Deflection();
    }
    // compute max deviations of points from the plane (along normal)
    for (i = idLower; i <= anUpper; i++)
    {
      const gp_XYZ& aP = aNodes.Value(i).XYZ();
      const gp_XYZ  aVec = aP - aCenter;
      const Standard_Real aDist = aVec * aNormal;
      if (aDist * aDist > aDev2) {
        aResult = Standard_False;
        break;
      }
    }

    if (aResult) {
      thePlane.SetLocation(aCenter);
      thePlane.SetDirection(aNormal);
    }
  }
  return aResult;
}


//=======================================================================
//function : ConvertToPolyCurve2d
//purpose  : Compute the 2D curve from a 3D Polygon.
//=======================================================================

Handle(Poly2d_Curve) ConvertToPolyCurve2d (const Handle(Poly_Polygon3D) &thePoly)
{
  Handle(Poly2d_Curve) aResult;
  if (!thePoly.IsNull())
  {
    gp_Ax2 aPln;
	if (CheckIsPlanar(thePoly, aPln, 100. * Precision::Confusion()))
    {
      Handle(NCollection_BaseAllocator) anAlloc =
        NCollection_BaseAllocator::CommonBaseAllocator();

      gp_Trsf aTrsf;
      gp_Ax3  aPln3(aPln);
      aTrsf.SetTransformation(aPln3);
      aResult = Poly2d_Curve::Create(anAlloc);
      aResult->SetLoc(aPln3);
      const TColgp_Array1OfPnt& arrNodes = thePoly->Nodes();
      if (arrNodes(arrNodes.Lower()).SquareDistance(arrNodes(arrNodes.Upper()))
          < Precision::Confusion() * Precision::Confusion())
      {
        aResult->SetType(Poly2d_Curve::CurveType_ExternalBoundary);
      }
      gp_XYZ aPrevPnt = arrNodes(1).XYZ();
      aTrsf.Transforms(aPrevPnt);
      for (Standard_Integer i = arrNodes.Lower()+1; i <= arrNodes.Upper(); i++)
      {
        gp_XYZ aPnt = arrNodes(i).XYZ();
        aTrsf.Transforms(aPnt);
        aResult->CreateSegment(Poly2d_Segment(gp_XY(aPrevPnt.X(), aPrevPnt.Y()),
                                                  gp_XY(aPnt.X(), aPnt.Y())));
        aPrevPnt.SetX(aPnt.X());
        aPrevPnt.SetY(aPnt.Y());
      }
    }
  }
  return aResult;
}

//=======================================================================
//function : ConvertToListOfPolygon3D
//purpose  : 
//=======================================================================

static void ConvertToListOfPolygon3D
                        (const Handle(Poly2d_Curve)               &theCurv,
                         NCollection_List<Handle(Poly_Polygon3D)> &theLst)
{
  Standard_Integer i = theCurv->NbSegments();
  std::cout<<" nbsegments = "<<i;
  if (i > 0) {

    std::cout<<", perimeter = "<<theCurv->Perimeter()<<", area = "<<theCurv->Area();

    gp_Trsf aTrsf;
    aTrsf.SetTransformation(theCurv->GetLoc());
    aTrsf.Invert();

    Poly2d_SegmentIterator anIter = theCurv->SegmentIterator();

    TColgp_Array1OfPnt aPoints(1, i + 1);

    gp_XYZ aP;
    if (anIter.More()) {
      aP = gp_XYZ(anIter.Value().Point(0).X(),
                  anIter.Value().Point(0).Y(),
                  0.);
      aTrsf.Transforms(aP);
    }

    i = 1;
    aPoints(i).SetCoord(aP.X(), aP.Y(), aP.Z());

    for (; anIter.More(); anIter.Next()) {
      aP = gp_XYZ(anIter.Value().Point(1).X(),
                  anIter.Value().Point(1).Y(),
                  0.);
      aTrsf.Transforms(aP);
      aPoints(++i).SetCoord(aP.X(),aP.Y(),aP.Z());
    }

    Handle(Poly_Polygon3D) aPolygon = new Poly_Polygon3D(aPoints);
    theLst.Append(aPolygon);
  }

  std::cout<<", nbchildren = "<<theCurv->Children().Extent();
  Poly2d_ListOfCurve::Iterator aChildIter(theCurv->Children());
  i = 1;
  for (; aChildIter.More(); aChildIter.Next()) {
    std::cout<<", child "<<i++<<" = {";
    ConvertToListOfPolygon3D(aChildIter.Value(), theLst);
    std::cout<<" }";
  }
}

//=======================================================================
//function : createSetPoly3d
//purpose  : internal function
//=======================================================================

static void createSetPoly3d
                (Draw_Interpretor&                               theDI,
                 const NCollection_List<Handle(Poly_Polygon3D)>& lstPoly,
                 const char *                                    theNamePrefix)
{
  Standard_Integer aCount(0);
  //read result name
  TCollection_AsciiString aBaseName(theNamePrefix);
  aBaseName += "_";
  NCollection_List<Handle(Poly_Polygon3D)>::Iterator anIt(lstPoly);
  for (; anIt.More(); anIt.Next()) {
    const Handle(Poly_Polygon3D)& aPolygon = anIt.Value();
    const TCollection_AsciiString aName = aBaseName + ++aCount;
    DrawTrSurf::Set(aName.ToCString(), aPolygon);
    theDI << aName << " ";
  }
  theDI << "\n";
}

//=======================================================================
//function : CAMpsection
//purpose  : 
//=======================================================================

static Standard_Integer CAMpsection(Draw_Interpretor& theDI, Standard_Integer n, const char** a)
{
  // Check the command arguments
  if (n < 4) {
    std::cout<<"Error: "<<a[0]<<" - invalid number of arguments"<<std::endl;
    std::cout<<"Usage: type help "<<a[0]<<std::endl;
    return 1; //TCL_ERROR
  }

  //read shape
  TopoDS_Shape aShape = DBRep::Get(a[2]);
  if (aShape.IsNull()) {
    std::cout<<"Error: "<<a[2]<<" is null"<<std::endl;
    return 1; //TCL_ERROR
  }

  //read plane
  Handle(Geom_Surface) aSurf = DrawTrSurf::GetSurface(a[3]);
  const Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast(aSurf);
  if (aPlane.IsNull()) {
    std::cout<<"Error: "<<a[2]<<" is not a plane"<<std::endl;
    return 1; //TCL_ERROR
  }

  //read tolerance
  Standard_Real aTol(0.001);
  if (n > 4)
    aTol = Draw::Atof(a[4]);
  std::cout<<"Info: tolerance is set to "<<aTol<<std::endl;

  PolyAlgo_PlaneTrianglesSection aPTS(aPlane->Pln().Position(), aTol, aTol, NULL); 

  Standard_Boolean hasTri(Standard_False);
  TopExp_Explorer aEx(aShape, TopAbs_FACE);
  for (; aEx.More(); aEx.Next()) {
    const TopoDS_Face& aFace = TopoDS::Face(aEx.Current());
    TopLoc_Location aLoc;
    const Handle(Poly_Triangulation)& aTriang = BRep_Tool::Triangulation(aFace, aLoc);
    if (!aTriang.IsNull()) {
      aPTS.AddTriangulation(aTriang, aLoc, aFace.Orientation()==TopAbs_REVERSED);
      hasTri = Standard_True;
    }
  }
  if (!hasTri) {
    std::cout<<"Error: shape "<<a[2]<<" has no triangulations"<<std::endl;
    return 1; //TCL_ERROR
  }

  const Standard_Integer aStatus = aPTS.Perform();
  if (aStatus & PolyAlgo_Curve2dBuilder::CB_HangingSegments) {
    std::cout<<"Warning: not all segments have been added to the result"<<std::endl;
  }
  if (aStatus & PolyAlgo_Curve2dBuilder::CB_Empty) {
    std::cout<<"Warning: no one closed curve in the result"<<std::endl;
  }

  const Handle(Poly2d_Curve) &aRes = aPTS.GetResult();
  if (aRes.IsNull()) {
    std::cout<<"Error: no result"<<std::endl;
    return 1; //TCL_ERROR
  }

  //convert result to Collection of Poly_Polygon3D
  NCollection_List<Handle(Poly_Polygon3D)> aLst;
  std::cout<<"Info: curve = {";
  ConvertToListOfPolygon3D(aRes, aLst);
  std::cout<<" }"<<std::endl;

  createSetPoly3d(theDI, aLst, a[1]);
  return 0; //TCL_OK
}

//=======================================================================
//function : CAMpoutline
//purpose  : 
//=======================================================================

static Standard_Integer CAMpoutline(Draw_Interpretor& theDI, Standard_Integer n, const char** a)
{
  // Check the command arguments
  if (n < 4) {
    std::cout<<"Error: "<<a[0]<<" - invalid number of arguments"<<std::endl;
    std::cout<<"Usage: type help "<<a[0]<<std::endl;
    return 1; //TCL_ERROR
  }

  //read shape
  TopoDS_Shape aShape = DBRep::Get(a[2]);
  if (aShape.IsNull()) {
    std::cout<<"Error: "<<a[2]<<" is null"<<std::endl;
    return 1; //TCL_ERROR
  }

  //read plane
  Handle(Geom_Surface) aSurf = DrawTrSurf::GetSurface(a[3]);
  const Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast(aSurf);
  if (aPlane.IsNull()) {
    std::cout<<"Error: "<<a[2]<<" is not a plane"<<std::endl;
    return 1; //TCL_ERROR
  }

  //read tolerance
  Standard_Real aTol(0.001);
  if (n > 4)
    aTol = Draw::Atof(a[4]);
  std::cout<<"Info: tolerance is set to "<<aTol<<std::endl;

  PolyAlgo_Outline anAlgo(aTol);

  Standard_Boolean hasTri(Standard_False);
  TopExp_Explorer aEx(aShape, TopAbs_FACE);
  for (; aEx.More(); aEx.Next()) {
    const TopoDS_Face& aFace = TopoDS::Face(aEx.Current());
    TopLoc_Location aLoc;
    const Handle(Poly_Triangulation)& aTriang = BRep_Tool::Triangulation(aFace, aLoc);
    if (!aTriang.IsNull()) {
      anAlgo.AddTriangulation(aTriang, aLoc);
      hasTri = Standard_True;
    }
  }
  if (!hasTri) {
    std::cout<<"Error: shape "<<a[2]<<" has no triangulations"<<std::endl;
    return 1; //TCL_ERROR
  }

  if (!anAlgo.Perform(aPlane->Pln().Position())) {
    std::cout<<"Error: computation failed"<<std::endl;
    return 1; //TCL_ERROR
  }

  const Poly2d_ListOfCurve& lstRes = anAlgo.Result();
  if (lstRes.IsEmpty()) {
    std::cout<<"Error: no result"<<std::endl;
    return 1; //TCL_ERROR
  }
  else
    std::cout<<"Info: "<<lstRes.Extent()<<" curve(s) computed"<<std::endl;

  //convert result to Collection of Poly_Polygon3D
  NCollection_List<Handle(Poly_Polygon3D)> lstPoly;
  Poly2d_ListOfCurve::Iterator anIter(lstRes);
  Standard_Integer i = 1;
  for (; anIter.More(); anIter.Next()) {
    std::cout<<"Info: curve "<<i++<<" = {";
    ConvertToListOfPolygon3D(anIter.Value(), lstPoly);
	std::cout<<" }"<<std::endl;
  }

  createSetPoly3d(theDI, lstPoly, a[1]);
  return 0; //TCL_OK
}

//=======================================================================
//function : CAMpoffset
//purpose  : Compute offset of a planar 3d polygon 
//=======================================================================

static Standard_Integer CAMpoffset (Draw_Interpretor& theDI, Standard_Integer n, const char** a)
{
  // Check the command arguments
  if (n < 4) {
    std::cout<<"Error: "<<a[0]<<" - invalid number of arguments"<<std::endl;
    std::cout<<"Usage: type help "<<a[0]<<std::endl;
    return 1; //TCL_ERROR
  }

  // Store result name
  const char *szResultName = a[1];

  // Read first polygon
  Handle(Poly_Polygon3D) aPolySrc = DrawTrSurf::GetPolygon3D(a[2]);
  Handle(Poly2d_Curve) aPC2d = ConvertToPolyCurve2d(aPolySrc);
  if (aPC2d.IsNull()) {
    std::cout<<"Error: "<<a[2]<<" is not a planar 3D polygon"<<std::endl;
    return 1; //TCL_ERROR
  }

  Poly2d_ListOfCurve lstCurve;
  lstCurve.Append(aPC2d);

  // Read additional polygons
  Standard_Integer iArg = 3;
  for (; iArg < n; iArg++) {
    aPolySrc = DrawTrSurf::GetPolygon3D(a[iArg]);
    aPC2d = ConvertToPolyCurve2d(aPolySrc);
    if (aPC2d.IsNull())
      break;
    lstCurve.Append(aPC2d);
  }

  // Read radius
  const Standard_Real aRadius = Draw::Atof(a[iArg++]);
  if (aRadius < 0.001) {
    std::cout<<"Error: too small radius "<<aRadius<<std::endl;
    return 1; //TCL_ERROR
  }

  // Read optional flags
  Standard_Boolean isBuildZones = Standard_False;
  PolyAlgo_Offset2d::OffsetType anOffsetType = PolyAlgo_Offset2d::Any;
  for (; iArg < n; iArg++) {
    if (!strcmp (a[iArg], "-z")) {
      isBuildZones = Standard_True;
    } else if (!strcmp (a[iArg], "-e")) {
      anOffsetType = PolyAlgo_Offset2d::Expand;
    } else if (!strcmp (a[iArg], "-s")) {
      anOffsetType = PolyAlgo_Offset2d::Shrink;
    } else
      break;
  }

  // Read tolerance
  Standard_Real aTol = 0.0001;
  if (iArg < n) {
    const Standard_Real aVal = Draw::Atof(a[iArg]);
    if (aVal > 0.0001)
      aTol = aVal;
  }
  std::cout<<"Info: tolerance is set to "<<aTol<<std::endl;

  PolyAlgo_Offset2d anOffset(lstCurve, aTol);

  if (!anOffset.Perform(aRadius)) {
    std::cout<<"Error: computation failed"<<std::endl;
    return 1; //TCL_ERROR
  }

  Poly2d_ListOfCurve lstOffset;
  if (!anOffset.Result(lstOffset, anOffsetType, isBuildZones)) {
    std::cout<<"Error: no result"<<std::endl;
    return 1; //TCL_ERROR
  }

  //convert result to Collection of Poly_Polygon3D
  NCollection_List<Handle(Poly_Polygon3D)> lstResult;
  Poly2d_ListOfCurve::Iterator anIter(lstOffset);
  Standard_Integer i = 1;
  for (; anIter.More(); anIter.Next()) {
    std::cout<<"Info: curve "<<i++<<" = {";
    ConvertToListOfPolygon3D(anIter.Value(), lstResult);
	std::cout<<" }"<<std::endl;
  }

  createSetPoly3d(theDI, lstResult, szResultName);
  return 0; //TCL_OK
}

//=======================================================================
//function : CAMpocket
//purpose  :
//=======================================================================

static Standard_Integer CAMpocket (Draw_Interpretor& /*theDI*/, Standard_Integer n, const char** a)
{
  // Check the command arguments
  if (n < 6) {
    std::cout<<"Error: "<<a[0]<<" - invalid number of arguments"<<std::endl;
    std::cout<<"Usage: type help "<<a[0]<<std::endl;
    return 1; //TCL_ERROR
  }

  //read shape
  const TopoDS_Shape aShape = DBRep::Get(a[2]);
  if (aShape.IsNull()) {
    std::cout<<"Error: "<<a[2]<<" is null"<<std::endl;
    return 1; //TCL_ERROR
  }

  TopoDS_Face aFace = TopoDS::Face(aShape);
  if (aFace.IsNull()) {
    std::cout<<"Error: "<<a[2]<<" is not a Face"<<std::endl;
    return 1; //TCL_ERROR
  }

  const Standard_Real aGap = Draw::Atof(a[3]);
  const Standard_Real aRadius = Draw::Atof(a[4]);
  const Standard_Boolean aFromExternalFlag = (a[5][0] == 't');

  //read tolerance
  Standard_Real aTol(0.001);
  if (n > 6)
    aTol = Draw::Atof(a[6]);
  std::cout<<"Info: tolerance is set to "<<aTol<<std::endl;

  PolyAlgo_MakePocket mkPocket(aFace, aGap, aRadius, aFromExternalFlag, aTol);
  std::cout<<"Info: status = "<<mkPocket.GetAlgoStatus().ToCString()<<std::endl;

  TopoDS_Compound comp = mkPocket.GetResult();

  DBRep::Set(a[1], comp);
  return 0; //TCL_OK
}

//=======================================================================
//function : PolyCommands
//purpose  : 
//=======================================================================

void CAMTest::PolyCommands (Draw_Interpretor& theCommands)
{
  const char* g = "CAM commands";

  theCommands.Add("CAMpsection", "CAMpsection result shape plane [tol]", __FILE__, CAMpsection, g);
  theCommands.Add("CAMpoutline", "CAMpoutline result shape plane [tol]", __FILE__, CAMpoutline, g);
  theCommands.Add("CAMpoffset", "CAMpoffset result poly3d [poly3d ...] radius [-e|-s] [-z] [tol]", __FILE__, CAMpoffset, g);
  theCommands.Add("CAMpocket", "CAMpocket result face gap radius fromExt [tol]", __FILE__, CAMpocket, g);
}
