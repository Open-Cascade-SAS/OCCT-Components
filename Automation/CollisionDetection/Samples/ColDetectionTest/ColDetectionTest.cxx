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

#include <ColDetectionTest.hxx>

#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <DBRep.hxx>
#include <gp_Pnt.hxx>
#include <gp_Ax1.hxx>
#include <TopoDS_Shape.hxx>
#include <ColDetectionProxy_Intersector.hxx>
#include <ColDetectionBase_MeshingMode.hxx>
#include <Draw_PluginMacro.hxx>
#include <ColDetectionPath.hxx>
#include <ColDetectionPath_Path.hxx>
#include <ColDetectionPath_Element.hxx>
#include <ColDetectionPath_LinearElement.hxx>
#include <TopTools_Array1OfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_Array1.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <OSD_Timer.hxx>
#include <OSD_Parallel.hxx>
#include <ColDetectionProxy_Result.hxx>
#include <TopOpeBRepTool.hxx>
#include <Precision.hxx>
#include <BRep_Builder.hxx>

namespace
{

typedef NCollection_Sequence<TColgp_SequenceOfPnt> AllPointsOnSegment;
typedef NCollection_Sequence<AllPointsOnSegment>   AllPointsOnElement;
typedef NCollection_Array1<AllPointsOnElement>     AllPointsOnPath;

class ActorPath
{
private:
  ColDetectionProxy_Intersector*                              myInter;
  const NCollection_Array1<Handle(ColDetectionPath_Element)>* myPath;
  Standard_Real                                               myOffset;
  Standard_Real                                               myStep;
  const TopTools_Array1OfShape*                               myStaticShapes;
  const TopoDS_Shape*                                         myMovShape;
  const gp_Trsf*                                              myShift;
  AllPointsOnPath*                                            myResult;

  void EvaluatePathElement(const Standard_Integer theIndex) const;

public:
  void operator()(const int theInd) const { EvaluatePathElement(theInd); }

  ActorPath(const Handle(ColDetectionProxy_Intersector)&                theInter,
            const NCollection_Array1<Handle(ColDetectionPath_Element)>& thePath,
            const Standard_Real                                         theOffset,
            const Standard_Real                                         theStep,
            const TopTools_Array1OfShape&                               theStaticShapes,
            const TopoDS_Shape&                                         theMovShape,
            const gp_Trsf&                                              theShift,
            AllPointsOnPath&                                            theResults)
      : myInter(theInter.operator->()),
        myPath(&thePath),
        myOffset(theOffset),
        myStep(theStep),
        myStaticShapes(&theStaticShapes),
        myMovShape(&theMovShape),
        myShift(&theShift),
        myResult(&theResults)
  {
  }

  void Run(const Standard_Boolean IsParallel)
  {
    if (IsParallel)
    {
      OSD_Parallel::For(1, myPath->Length() + 1, *this);
    }
    else
    {
      for (Standard_Integer elem = 1; elem <= myPath->Length(); elem++)
      {
        EvaluatePathElement(elem);
      }
    }
  }
};

//=======================================================================
// function : ActorPath::EvaluatePathElement
// purpose  :
//=======================================================================

void ActorPath::EvaluatePathElement(const Standard_Integer theIndex) const
{
  const Handle(ColDetectionPath_Element)& aLin = myPath->Value(theIndex);
  if (aLin.IsNull())
    return;

  Standard_Real aRange = aLin->Range();
  Standard_Real aParam = 0;

  TopoDS_Shape aMovShape = *myMovShape;
  while (aParam <= aRange)
  {
    AllPointsOnSegment aPointsOnSegment;
    gp_Trsf            T = aLin->Position(aParam) * (*myShift);

    TopLoc_Location L(T);
    aMovShape.Location(L);
    if (aParam < aRange && (aParam + myStep) > aRange)
      aParam = aRange;
    else
      aParam += myStep;

    for (Standard_Integer j = 1; j <= myStaticShapes->Length(); j++)
    {
      ColDetectionProxy_Result aResult1, aResult2;
      myInter->Evaluate(aMovShape, myStaticShapes->Value(j), aResult1, aResult2, myOffset);
      if (aResult1.IsFound())
      {
        aPointsOnSegment.Append(TColgp_SequenceOfPnt());

        TColgp_SequenceOfPnt& aSeqInter = aPointsOnSegment.ChangeLast();

        ColDetectionProxy_MapOfPnt::Iterator aIt(aResult1.Points());
        for (; aIt.More(); aIt.Next())
        {
          aSeqInter.Append(aIt.Value());
        }
      }
    }
    myResult->ChangeValue(theIndex).Append(aPointsOnSegment);
  }
}

}; // namespace

//=======================================================================
// function : MyIntersector
// purpose  : Provides intersector object
//=======================================================================

static const Handle(ColDetectionProxy_Intersector)& MyIntersector()
{
  static Handle(ColDetectionProxy_Intersector) myProxyInter;

  if (myProxyInter.IsNull())
    myProxyInter = new ColDetectionProxy_Intersector;
  return myProxyInter;
}

//=======================================================================
// function : ClearData
// purpose  : Clear data of the algorithm
//=======================================================================

static Standard_Integer ClearData(Draw_Interpretor& /*theDI*/,
                                  const Standard_Integer /*argc*/,
                                  const char** /*argv*/)
{
  MyIntersector()->UnloadAllTShapes();
  return 0;
}

//=======================================================================
// function : SetDeflection
// purpose  : Set or display deflection parameter
//=======================================================================

static Standard_Integer SetDeflection(Draw_Interpretor&      theDI,
                                      const Standard_Integer argc,
                                      const char**           argv)
{
  // Retrieve intersector
  const Handle(ColDetectionProxy_Intersector)& aInter = MyIntersector();

  if (argc > 1)
  {
    const Standard_Real aDeflection = Draw::Atof(argv[1]);
    if (aDeflection <= 0.0)
    {
      theDI << "Error: invalid deflection\n";
      return 1; // TCL_ERROR
    }
    aInter->SetDeflection(aDeflection);
  }

  const Standard_Real aDeflection = aInter->Deflection();
  theDI << "Info: current deflection is " << aDeflection << "\n";

  return 0;
}

//=======================================================================
// function : SetMode
// purpose  : Set or display triangulation mode
//=======================================================================

static Standard_Integer SetMode(Draw_Interpretor&      theDI,
                                const Standard_Integer argc,
                                const char**           argv)
{
  // Retrieve intersector
  const Handle(ColDetectionProxy_Intersector)& aInter = MyIntersector();

  if (argc > 1)
  {
    const Standard_Integer aMode = Draw::Atoi(argv[1]);
    if (aMode < 0 || aMode > 2)
    {
      theDI << "Error: invalid meshing mode\n";
      return 1; // TCL_ERROR
    }
    aInter->SetMeshingMode((ColDetectionBase_MeshingMode)aMode);
  }

  const ColDetectionBase_MeshingMode aMode = aInter->MeshingMode();
  theDI << "Info: current meshing mode is ";
  switch (aMode)
  {
    case ColDetectionBase_MeshingMode_Exact:
      theDI << "0 - ColDetectionBase_MeshingMode_Exact\n";
      break;
    case ColDetectionBase_MeshingMode_Incremental:
      theDI << "1 - ColDetectionBase_MeshingMode_Incremental\n";
      break;
    case ColDetectionBase_MeshingMode_Existing:
      theDI << "2 - ColDetectionBase_MeshingMode_Existing\n";
      break;
    default:
      theDI << "UNKNOWN\n";
  }

  return 0;
}

//=======================================================================
// function : CollectTriangles
// purpose  : Collect intersecting triangles, store them to the
//           Poly_Triangulation of a TopoDS_Face and put it to resulting
//           compound.
//=======================================================================

static void CollectTriangles(TopoDS_Compound&                           theC,
                             const ColDetectionBase_SequenceOfTriangle& theTriangles)
{
  BRep_Builder aBuilder;
  aBuilder.MakeCompound(theC);
  for (Standard_Integer aTriIt = 1; aTriIt <= theTriangles.Length(); ++aTriIt)
  {
    TopoDS_Face aF;
    aBuilder.MakeFace(aF);

    Handle(Poly_Triangulation) aT = new Poly_Triangulation(3, 1, false);

    const ColDetectionBase_Triangle& aTriangle = theTriangles.Value(aTriIt);

    aT->SetNode(1, aTriangle.Point(1));
    aT->SetNode(2, aTriangle.Point(2));
    aT->SetNode(3, aTriangle.Point(3));

    aT->SetTriangle(1, Poly_Triangle(1, 2, 3));

    aBuilder.UpdateFace(aF, aT);
    aBuilder.Add(theC, aF);
  }
}

//=======================================================================
// function : EvalCollision
// purpose  : Evaluate collision or proximity
//=======================================================================

static Standard_Integer EvalCollision(Draw_Interpretor&      theDI,
                                      const Standard_Integer argc,
                                      const char**           argv)
{
  if (argc < 3)
  {
    theDI << "Error: " << argv[0] << " - invalid number of arguments\n";
    theDI << "Usage: type help " << argv[0] << "\n";
    return 1; // TCL_ERROR
  }

  TopoDS_Shape aShape1 = DBRep::Get(argv[1]);
  if (aShape1.IsNull())
  {
    theDI << "Error: shape " << argv[1] << " is null\n";
    return 1; // TCL_ERROR
  }

  TopoDS_Shape aShape2 = DBRep::Get(argv[2]);
  if (aShape2.IsNull())
  {
    theDI << "Error: shape " << argv[2] << " is null\n";
    return 1; // TCL_ERROR
  }

  Standard_Real                anOffset        = 0.0;
  Standard_Real                aMeshDeflection = 0.01;
  Standard_Boolean             isFindAll       = Standard_False;
  ColDetectionBase_MeshingMode aMeshMode       = ColDetectionBase_MeshingMode_Exact;

  for (Standard_Integer aArgIt = 3; aArgIt < argc; ++aArgIt)
  {
    TCollection_AsciiString aParam = argv[aArgIt];
    aParam.LowerCase();
    if (aParam == "-meshdeflection" && aArgIt + 1 < argc)
    {
      aParam = argv[aArgIt + 1];
      if (aParam.IsRealValue(Standard_True))
      {
        aMeshDeflection = aParam.RealValue();
        if (aMeshDeflection < Precision::Confusion())
        {
          theDI << "Error: invalid meshing deflection.";
          return 1;
        }

        ++aArgIt;
      }
    }
    else if (aParam == "-all")
    {
      isFindAll = Draw::ParseOnOffNoIterator(argc, argv, aArgIt);
    }
    else if (aParam == "-meshmode")
    {
      aParam = argv[aArgIt + 1];
      aParam.LowerCase();
      if (aParam == "exact")
      {
        aMeshMode = ColDetectionBase_MeshingMode_Exact;
      }
      else if (aParam == "incremental")
      {
        aMeshMode = ColDetectionBase_MeshingMode_Incremental;
      }
      else if (aParam == "existing")
      {
        aMeshMode = ColDetectionBase_MeshingMode_Existing;
      }
      else
      {
        theDI << "Error: invalid meshing mode.";
        return 1;
      }

      ++aArgIt;
    }
    else if (aParam.IsRealValue(Standard_True))
    {
      anOffset = aParam.RealValue();
      if (anOffset < 0.0)
      {
        theDI << "Error: invalid value of offset\n";
        return 1; // TCL_ERROR
      }
    }
    else
    {
      theDI << "Error: unknown parameter " << aParam << "\n";
    }
  }

  Handle(ColDetectionProxy_Intersector) aInter = MyIntersector();
  aInter->SetMeshingMode(aMeshMode);
  aInter->SetDeflection(aMeshDeflection);

  {
    ColDetectionProxy_Result aResult1, aResult2;
    if (isFindAll)
    {
      aInter->Perform(aShape1, aShape2, aResult1, aResult2, anOffset);
    }
    else
    {
      aInter->Evaluate(aShape1, aShape2, aResult1, aResult2, anOffset);
    }

    if (aResult1.IsFound())
    {
      if (anOffset < Precision::Confusion())
      {
        // Collision
        const ColDetectionProxy_MapOfPnt& aCollisionPoints = aResult1.Points();

        theDI << "Info: collision points:" << aCollisionPoints.Size() << "\n";

        for (ColDetectionProxy_MapOfPnt::Iterator aIt(aCollisionPoints); aIt.More(); aIt.Next())
        {
          const gp_Pnt& P = aIt.Value();
          theDI << "Coords: (" << P.X() << ", " << P.Y() << ", " << P.Z() << ")\n";
        }

        // Print out faces
        TColStd_SequenceOfInteger aNbF1, aNbF2;
        aResult1.Faces(aNbF1);
        aResult2.Faces(aNbF2);
        theDI << "Info: collision faces:\n";
        theDI << "\tOn shape " << argv[1] << " : " << aNbF1.First() << "\n";
        theDI << "\tOn shape " << argv[2] << " : " << aNbF2.First() << "\n";
      }
      else
      {
        // Proximity
        const ColDetectionProxy_MapOfPnt& aCollisionPoints1 = aResult1.Points();
        const ColDetectionProxy_MapOfPnt& aCollisionPoints2 = aResult2.Points();

        theDI << "Info: Non-null offset collision:\n";
        theDI << "\tOn shape " << argv[1] << "\n";
        for (ColDetectionProxy_MapOfPnt::Iterator aIt1(aCollisionPoints1); aIt1.More(); aIt1.Next())
        {
          const gp_Pnt& P = aIt1.Value();
          theDI << "Coords: (" << P.X() << ", " << P.Y() << ", " << P.Z() << ")\n";
        }
        theDI << "\tOn shape " << argv[2] << "\n";

        for (ColDetectionProxy_MapOfPnt::Iterator aIt2(aCollisionPoints2); aIt2.More(); aIt2.Next())
        {
          const gp_Pnt& P = aIt2.Value();
          theDI << "Coords: (" << P.X() << ", " << P.Y() << ", " << P.Z() << ")\n";
        }
      }

      // Print out triangles
      ColDetectionBase_SequenceOfTriangle aTriangles1, aTriangles2;
      aResult1.Triangles(aTriangles1);
      aResult2.Triangles(aTriangles2);
      const Standard_Integer aNbT1 = aTriangles1.Length();
      const Standard_Integer aNbT2 = aTriangles2.Length();
      theDI << "Info: collision triangles (" << aNbT1 << ", " << aNbT2 << ")\n";

      TopoDS_Compound c1, c2;

      CollectTriangles(c1, aTriangles1);
      CollectTriangles(c2, aTriangles2);

      TCollection_AsciiString aName1(argv[1]);
      TCollection_AsciiString aName2(argv[2]);

      aName1 += "_t";
      aName2 += "_t";

      theDI << "Info: collision triangles set:\n";
      theDI << "\tOn shape " << argv[1] << " : " << aName1 << "\n";
      theDI << "\tOn shape " << argv[2] << " : " << aName2 << "\n";

      DBRep::Set(aName1.ToCString(), c1);
      DBRep::Set(aName2.ToCString(), c2);
    }
    else
      theDI << "Info: no interference detected\n";
  }
  return 0;
}

//=======================================================================
// function : RayCollision
// purpose  : Ray collision
//=======================================================================

static Standard_Integer RayCollision(Draw_Interpretor&      theDI,
                                     const Standard_Integer argc,
                                     const char**           argv)
{
  if (argc < 8)
  {
    theDI << "Error: " << argv[0] << " - invalid number of arguments\n";
    theDI << "Usage: type help " << argv[0] << "\n";
    return 1; // TCL_ERROR
  }

  TopoDS_Shape aShape = DBRep::Get(argv[1]);
  if (aShape.IsNull())
  {
    theDI << "Error: shape " << argv[1] << " is null\n";
    return 1; // TCL_ERROR
  }

  const Standard_Real x = Draw::Atof(argv[2]);
  const Standard_Real y = Draw::Atof(argv[3]);
  const Standard_Real z = Draw::Atof(argv[4]);

  const Standard_Real dx = Draw::Atof(argv[5]);
  const Standard_Real dy = Draw::Atof(argv[6]);
  const Standard_Real dz = Draw::Atof(argv[7]);

  const gp_Vec v(dx, dy, dz);
  if (v.Magnitude() < gp::Resolution())
  {
    theDI << "Error: invalid direction\n";
    return 1; // TCL_ERROR
  }

  const gp_Ax1 aRay(gp_Pnt(x, y, z), gp_Dir(v));

  {
    ColDetectionProxy_Result aResult;
    MyIntersector()->Evaluate(aShape, aRay, aResult);
    if (aResult.IsFound())
    {
      const ColDetectionProxy_MapOfPnt& aCollisionPoints = aResult.Points();

      ColDetectionProxy_MapOfPnt::Iterator aIt(aCollisionPoints);
      for (; aIt.More(); aIt.Next())
      {
        const gp_Pnt& P = aIt.Value();
        theDI << "Point in intersection: " << P.X() << "  " << P.Y() << "  " << P.Z() << " \n ";
      }
      // Print out triangles
      ColDetectionBase_SequenceOfTriangle aTriangles;
      aResult.Triangles(aTriangles);
      theDI << "Triangles in intersection: (" << aTriangles.Length() << ")\n";
    }
    else
      theDI << "Info: no interference detected\n";
  }

  return 0;
}

//=======================================================================
// function : SphereCollision
// purpose  : Sphere collision
//=======================================================================

static Standard_Integer SphereCollision(Draw_Interpretor&      theDI,
                                        const Standard_Integer argc,
                                        const char**           argv)
{
  if (argc < 6)
  {
    theDI << "Error: " << argv[0] << " - invalid number of arguments\n";
    theDI << "Usage: type help " << argv[0] << "\n";
    return 1; // TCL_ERROR
  }

  TopoDS_Shape aShape = DBRep::Get(argv[1]);
  if (aShape.IsNull())
  {
    theDI << "Error: shape " << argv[1] << " is null\n";
    return 1; // TCL_ERROR
  }

  const Standard_Real x = Draw::Atof(argv[2]);
  const Standard_Real y = Draw::Atof(argv[3]);
  const Standard_Real z = Draw::Atof(argv[4]);

  const Standard_Real R = Draw::Atof(argv[5]);

  if (R <= 0.0)
  {
    theDI << "Error: invalid radius\n";
    return 1; // TCL_ERROR
  }

  {
    ColDetectionProxy_Result aResult;
    MyIntersector()->Evaluate(aShape, gp_Pnt(x, y, z), R, aResult);
    if (aResult.IsFound())
    {
      ColDetectionBase_SequenceOfTriangle aTriangles;
      aResult.Triangles(aTriangles);
      theDI << "Triangles in intersection: (" << aTriangles.Length() << ")\n";
    }
    else
      theDI << "Info: no interference detected\n";
  }

  return 0;
}

//=======================================================================
// function : PathCollision
// purpose  : Implements cdpath command
//=======================================================================

static Standard_Integer PathCollision(Draw_Interpretor&      theDI,
                                      const Standard_Integer argc,
                                      const char**           argv)
{
  if (argc < 6)
  {
    theDI << "Error: " << argv[0] << " - invalid number of arguments\n";
    theDI << "Usage: type help " << argv[0] << "\n";
    return 1; // TCL_ERROR
  }

  TopoDS_Shape aMovShape = DBRep::Get(argv[1]);
  if (aMovShape.IsNull())
  {
    theDI << "Error: shape " << argv[1] << " is null\n";
    return 1; // TCL_ERROR
  }

  gp_Pnt                   aTranslation(0, 0, 0);
  TopTools_SequenceOfShape aStaticShapes;

  TopoDS_Shape aStaticShape = DBRep::Get(argv[2]);

  Standard_Integer i = 2;
  if (aStaticShape.IsNull())
  {
    aTranslation.SetCoord(Atof(argv[2]), Atof(argv[3]), Atof(argv[4]));
    aStaticShape = DBRep::Get(argv[5]);
    if (aStaticShape.IsNull())
    {
      theDI << "Error: shape " << argv[5] << " is null\n";
      return 1; // TCL_ERROR
    }
    i = 5;
  }

  while (!aStaticShape.IsNull())
  {
    if (aStaticShape != aMovShape)
      aStaticShapes.Append(aStaticShape);
    i++;
    if (i >= argc)
      break;
    aStaticShape = DBRep::Get(argv[i]);
  }

  if (aStaticShapes.IsEmpty())
  {
    theDI << "Error: static shapes list is empty \n";
    return 1; // TCL_ERROR
  }

  if ((i + 3) >= argc)
  {
    theDI << "Error: " << argv[0] << " - invalid number of arguments\n";
    theDI << "Usage: type help " << argv[0] << "\n";
    return 1; // TCL_ERROR
  }

  const Standard_Real aStep    = Atof(argv[i++]);
  const Standard_Real anOffset = Atof(argv[i++]);

  Handle(ColDetectionPath_Path) aPath = new ColDetectionPath_Path();
  aPath->Load(argv[i++]);

  const Standard_Boolean InParallel = Draw::Atoi(argv[i]) != 0;

  gp_Trsf aShift;
  aShift.SetTranslation(aTranslation, gp_Pnt(0., 0., 0.));

  // Copy shapes for safe parallel execution
  const Standard_Integer nbStaticShapes = aStaticShapes.Length();
  TopTools_Array1OfShape arrStaticShapes(1, nbStaticShapes);
  for (i = 1; i <= nbStaticShapes; i++)
    arrStaticShapes.SetValue(i, aStaticShapes(i));

  // Copy path elements for safe parallel execution
  const Standard_Integer                               nbPathElements = aPath->NbElements();
  NCollection_Array1<Handle(ColDetectionPath_Element)> arrPathElements(1, nbPathElements);
  for (i = 1; i <= nbPathElements; i++)
    arrPathElements.SetValue(i, aPath->Element(i));

  AllPointsOnPath aResults(1, aPath->NbElements());

  ActorPath anActor(MyIntersector(),
                    arrPathElements,
                    anOffset,
                    aStep,
                    arrStaticShapes,
                    aMovShape,
                    aShift,
                    aResults);

  OSD_Timer aTimer;
  aTimer.Start();

  anActor.Run(InParallel);

  aTimer.Stop();
  aTimer.Show();

  for (i = 1; i <= aPath->NbElements(); i++)
  {
    const AllPointsOnElement& AllPointsOnElem = aResults.Value(i);
    const Standard_Integer    nbe             = AllPointsOnElem.Length();
    for (Standard_Integer e = 1; e <= nbe; e++)
    {
      const AllPointsOnSegment& AllPointsOnSeg = AllPointsOnElem.Value(e);
      const Standard_Integer    nbs            = AllPointsOnSeg.Length();
      for (Standard_Integer s = 1; s <= nbs; s++)
      {
        Standard_Integer            k;
        const TColgp_SequenceOfPnt& AllPoints = AllPointsOnSeg.Value(s);
        for (k = 1; k <= AllPoints.Length(); k++)
        {
          const gp_Pnt& P(AllPoints.Value(k));
          theDI << "Collision on element: " << i << "\tOn moving shape : (" << P.X() << ", "
                << P.Y() << ", " << P.Z() << ")\n";
        }
      }
    }
  }

  return 0;
}

//=======================================================================
// function : GenerateSinPath
// purpose  : Implements cdsin command
//=======================================================================

static Standard_Integer GenerateSinPath(Draw_Interpretor&      theDI,
                                        const Standard_Integer argc,
                                        const char**           argv)
{
  if (argc != 4)
  {
    theDI << "Error: " << argv[0] << " - invalid number of arguments\n";
    theDI << "Usage: type help " << argv[0] << "\n";
    return 1; // TCL_ERROR
  }

  const Standard_Real    anAmpl = Draw::Atof(argv[1]);
  const Standard_Integer aNbSeq = Draw::Atoi(argv[2]);
  if (aNbSeq <= 0)
  {
    theDI << "Error: number of elements = 0\n";
    return 1;
  }

  Handle(ColDetectionPath_Path) aPath = new ColDetectionPath_Path();
  gp_XYZ                        anAngles(0, 0, 0);
  for (Standard_Integer i = 0; i < aNbSeq; i++)
    aPath->AddLinear(gp_Pnt(anAmpl * sin((100 * (Standard_Real)i / (Standard_Real)(aNbSeq))),
                            0,
                            (100 * (Standard_Real)i / (Standard_Real)aNbSeq)),
                     anAngles);
  if (!aPath->Save(argv[3]))
  {
    theDI << "Error while saving to file\n";
    return 1;
  }

  theDI << "Info: path saved to file " << argv[3] << "\n";
  return 0;
}

//=======================================================================
// function : Commands
// purpose  :
//=======================================================================

void ColDetectionTest::Commands(Draw_Interpretor& theCommands)
{
  const char* grp = "Collision Detection commands";

  theCommands.Add("cdclear", "Clear data of the algorithm : cdclear", __FILE__, ClearData, grp);
  theCommands.Add("cddeflection",
                  "Set or get deflection value for the algorithm : cddeflection [deflection]",
                  __FILE__,
                  SetDeflection,
                  grp);
  theCommands.Add("cdtrimode",
                  "Set or get triangulation mode for the algorithm : cdtrimode [mode]",
                  __FILE__,
                  SetMode,
                  grp);
  theCommands.Add(
    "cdeval",
    "cdeval shape1 shape2 [offset]\n"
    "    [-meshMode exact|incremental|existing] [-meshDeflection <deflection>] [-all]\n"
    "  Evaluate collision using the algorithm with the meshing mode and\n"
    "  deflection.",
    __FILE__,
    EvalCollision,
    grp);
  theCommands.Add("cdray",
                  "Ray collision using the algorithm : cdeval shape x y z dx dy dz",
                  __FILE__,
                  RayCollision,
                  grp);
  theCommands.Add("cdsphere",
                  "Sphere collision using the algorithm : cdsphere shape x y z R",
                  __FILE__,
                  SphereCollision,
                  grp);
  theCommands.Add("cdpath",
                  "Evaluate collision on a path using the algorithm : cdpath m_shape [x0 y0 z0] "
                  "s_shape1 [s_shape2 ..  s_shapeN] step offset path InPar",
                  __FILE__,
                  PathCollision,
                  grp);
  theCommands.Add("cdsin",
                  "Generates sinus path with given amplitude & number of elements",
                  __FILE__,
                  GenerateSinPath,
                  grp);
}

//==============================================================================
// ColDetectionTest::Factory
//==============================================================================

void ColDetectionTest::Factory(Draw_Interpretor& theDI)
{
  ColDetectionTest::Commands(theDI);

#ifdef DEB
  theDI << "Draw Plugin : All TKColTest commands are loaded\n";
#endif
}

// Declare entry point PLUGINFACTORY
DPLUGIN(ColDetectionTest)
