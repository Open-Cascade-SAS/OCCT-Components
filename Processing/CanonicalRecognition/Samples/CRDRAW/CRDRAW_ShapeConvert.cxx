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

#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <CRDRAW.hxx>
#include <CRDRAW_ShapeConvert.hxx>
#include <DBRep.hxx>
#include <Draw.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <Precision.hxx>
#include <ShapeConvert.hxx>
#include <ShapeConvert_CanonicAPI.hxx>
#include <GeomConvert_SurfToAnaSurf.hxx>
#include <ShapeConvert_UnionEdges.hxx>
#include <ShapeConvert_UnionFaces.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

//=======================================================================
// function : converttoanalytic
// purpose  :
//=======================================================================
static Standard_Integer converttoanalytic(Draw_Interpretor& /*di*/,
                                          Standard_Integer argc,
                                          const char**     argv)
{
  if (argc < 3)
  {
    std::cout << "Error: " << argv[0] << " - invalid number of arguments" << std::endl;
    std::cout << "Usage: type help " << argv[0] << std::endl;
    return 1;
  }

  Standard_CString arg1 = argv[1];
  Standard_CString arg2 = argv[2];

  TopoDS_Shape Shape = DBRep::Get(arg2);
  if (Shape.IsNull())
  {
    std::cout << "Shape unknown : " << arg2 << std::endl;
    return 1;
  }

  const Standard_Real toler = (argc > 3 ? Draw::Atof(argv[3]) : Precision::Confusion());
  TopoDS_Shape        result =
    ShapeConvert::ConvertToAnalytical(Shape, toler, Handle(ShapeBuild_ReShape)());
  if (result.IsNull())
  {
    std::cout << "NO RESULT" << std::endl;
    return 1;
  }
  if (result == Shape)
  {
    std::cout << "No modif, result shape " << arg1 << " is equal to " << arg2 << std::endl;
    DBRep::Set(arg1, result);
  }
  else
  {
    std::cout << "Result stored in : " << arg1 << std::endl;
    DBRep::Set(arg1, result);
  }
  return 0; // Done
}

//=======================================================================
// function : samesurface
// purpose  :
//=======================================================================
static Standard_Integer samesurface(Draw_Interpretor& /*di*/,
                                    Standard_Integer argc,
                                    const char**     argv)
{
  if (argc < 3)
  {
    std::cout << "Error: " << argv[0] << " - invalid number of arguments" << std::endl;
    std::cout << "Usage: type help " << argv[0] << std::endl;
    return 1;
  }

  Standard_CString     arg1 = argv[1];
  Standard_CString     arg2 = argv[2];
  TopoDS_Face          F1   = TopoDS::Face(DBRep::Get(arg1));
  Handle(Geom_Surface) S1   = BRep_Tool::Surface(F1);
  Handle(Geom_Surface) S2   = BRep_Tool::Surface(TopoDS::Face(DBRep::Get(arg2)));

  Standard_Real U1, U2, V1, V2;
  BRepTools::UVBounds(F1, U1, U2, V1, V2);

  const Standard_Real toler = (argc > 3 ? Draw::Atof(argv[3]) : Precision::Confusion());
  if (ShapeConvert::IsSameSurface(S1, S2, U1, U2, V1, V2, toler))
    std::cout << "The surfaces are the same" << std::endl;
  else
    std::cout << "The surfaces are not the same" << std::endl;
  return 0; // Done
}

//=======================================================================
// function : unionfaces
// purpose  :
//=======================================================================
static Standard_Integer unionfaces(Draw_Interpretor& /*di*/,
                                   Standard_Integer argc,
                                   const char**     argv)
{
  if (argc < 3)
  {
    std::cout << "Error: " << argv[0] << " - invalid number of arguments" << std::endl;
    std::cout << "Usage: type help " << argv[0] << std::endl;
    return 1;
  }

  Standard_CString arg1  = argv[1];
  Standard_CString arg2  = argv[2];
  TopoDS_Shape     Shape = DBRep::Get(arg2);
  if (Shape.IsNull())
  {
    std::cout << "Shape unknown : " << arg2 << std::endl;
    return 1;
  }

  const Standard_Real     toler = (argc > 3 ? Draw::Atof(argv[3]) : Precision::Confusion());
  ShapeConvert_UnionFaces anUnifier;
  anUnifier.GetTolerance() = toler;
  TopoDS_Shape result      = anUnifier.Perform(Shape);
  if (result.IsNull())
  {
    std::cout << "NO RESULT" << std::endl;
    return 1;
  }
  if (result == Shape)
  {
    std::cout << "No modif, result shape " << arg1 << " is equal to " << arg2 << std::endl;
    DBRep::Set(arg1, result);
  }
  else
  {
    std::cout << "Result stored in : " << arg1 << std::endl;
    DBRep::Set(arg1, result);
  }
  return 0; // Done
}

//=======================================================================
// function : unionedges
// purpose  :
//=======================================================================
static Standard_Integer unionedges(Draw_Interpretor& /*di*/,
                                   Standard_Integer argc,
                                   const char**     argv)
{
  if (argc < 3)
  {
    std::cout << "Error: " << argv[0] << " - invalid number of arguments" << std::endl;
    std::cout << "Usage: type help " << argv[0] << std::endl;
    return 1;
  }

  Standard_CString arg1  = argv[1];
  Standard_CString arg2  = argv[2];
  TopoDS_Shape     Shape = DBRep::Get(arg2);
  if (Shape.IsNull())
  {
    std::cout << "Shape unknown : " << arg2 << std::endl;
    return 1;
  }

  const Standard_Real     toler = (argc > 3 ? Draw::Atof(argv[3]) : Precision::Confusion());
  ShapeConvert_UnionEdges anUnifier;
  TopoDS_Shape            result = anUnifier.Perform(Shape, toler);
  if (result.IsNull())
  {
    std::cout << "NO RESULT" << std::endl;
    return 1;
  }
  if (result == Shape)
  {
    std::cout << "No modif, result shape " << arg1 << " is equal to " << arg2 << std::endl;
    DBRep::Set(arg1, result);
  }
  else
  {
    std::cout << "Result stored in : " << arg1 << std::endl;
    DBRep::Set(arg1, result);
  }
  return 0; // Done
}

//=======================================================================
// function : shapeconvert
// purpose  :
//=======================================================================
static Standard_Integer shapeconvert(Draw_Interpretor& /*di*/,
                                     Standard_Integer argc,
                                     const char**     argv)
{
  if (argc < 3)
  {
    std::cout << "Error: " << argv[0] << " - invalid number of arguments" << std::endl;
    std::cout << "Usage: type help " << argv[0] << std::endl;
  }

  Standard_CString arg1  = argv[1];
  Standard_CString arg2  = argv[2];
  TopoDS_Shape     Shape = DBRep::Get(arg2);
  if (Shape.IsNull())
  {
    std::cout << "Shape unknown : " << arg2 << std::endl;
    return 1;
  }

  const Standard_Real     toler = (argc > 3 ? Draw::Atof(argv[3]) : Precision::Confusion());
  ShapeConvert_CanonicAPI aTool;
  aTool.Tolerance() = toler;
  aTool.SetShape(Shape);
  aTool.UnifyMode() = Standard_True;
  if (argc > 4)
  {
    Standard_Integer ToUnify = Draw::Atoi(argv[4]);
    if (ToUnify == 0)
      aTool.UnifyMode() = Standard_False;
  }
  aTool.Perform();

  TopoDS_Shape result = aTool.Shape();
  if (result.IsNull())
  {
    std::cout << "NO RESULT" << std::endl;
    return 1;
  }
  if (result == Shape)
  {
    std::cout << "No modif, result shape " << arg1 << " is equal to " << arg2 << std::endl;
    DBRep::Set(arg1, result);
  }
  else
  {
    std::cout << "Result stored in : " << arg1 << std::endl;
    DBRep::Set(arg1, result);
  }
  return 0; // Done
}

//=======================================================================
// function : nbcansurf
// purpose  :
//=======================================================================
static Standard_Integer nbcansurf(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 2)
  {
    std::cout << "Error: " << argv[0] << " - invalid number of arguments" << std::endl;
    std::cout << "Usage: type help " << argv[0] << std::endl;
    return 1;
  }

  Standard_CString arg1  = argv[1];
  TopoDS_Shape     Shape = DBRep::Get(arg1);
  if (Shape.IsNull())
  {
    std::cout << "Shape unknown : " << arg1 << std::endl;
    return 1;
  }

  TopLoc_Location  L;
  Standard_Integer nbSurf = 0;
  for (TopExp_Explorer exp(Shape, TopAbs_FACE); exp.More(); exp.Next())
  {
    TopoDS_Face          aFace    = TopoDS::Face(exp.Current());
    Handle(Geom_Surface) aSurface = BRep_Tool::Surface(aFace, L);
    if (aSurface->IsKind(STANDARD_TYPE(Geom_Plane))
        || GeomConvert_SurfToAnaSurf::IsCanonical(aSurface))
      nbSurf++;
  }
  di << "Number of Canonical surfaces is " << nbSurf << "\n";
  return 0; // Done
}

//=======================================================================
// function : InitCommands
// purpose  :
//=======================================================================

void CRDRAW_ShapeConvert::InitCommands(Draw_Interpretor& theCommands)
{
  static bool isLoaded = false;
  if (isLoaded)
  {
    return;
  }
  isLoaded = true;

  Standard_CString g = CRDRAW::GroupName();

  theCommands.Add(
    "CRcanonical",
    "Converts geometry of shape to analytical form. Usage: CRcanonical result shape [tolerance]",
    __FILE__,
    converttoanalytic,
    g);
  theCommands.Add(
    "CRsame",
    "Checks if faces are based on the same surfaces. Usage: CRsame face face [tolerance]",
    __FILE__,
    samesurface,
    g);

  theCommands.Add("CRunionfaces",
                  "Unify (merge) faces with the same analytical support. Usage: CRunionfaces "
                  "result shape [tolerance]",
                  __FILE__,
                  unionfaces,
                  g);
  theCommands.Add("CRunionedges",
                  "Unify (merge) edges with the same analytical support. Usage: CRunionedges "
                  "result shape [tolerance]",
                  __FILE__,
                  unionedges,
                  g);

  theCommands.Add(
    "CRshapeconvert",
    "Makes all conversions using API. Usage: CRshapeconvert result shape [tolerance [UnifyFlag]]",
    __FILE__,
    shapeconvert,
    g);

  theCommands.Add("CRnbcansurf",
                  "Returns the number of canonical surfaces. Usage: CRnbcansurf shape",
                  __FILE__,
                  nbcansurf,
                  g);
}
