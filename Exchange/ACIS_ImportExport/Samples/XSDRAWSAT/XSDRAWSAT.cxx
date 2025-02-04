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

#include <AcisAbs_IndexedDataMapOfObjectType.hxx>
#include <AcisAbs_SaveApproxLevel.hxx>
#include <AcisData_AcisModel.hxx>
#include <AcisData_InterfaceModel.hxx>
#include <AcisEnt_AcisObject.hxx>
#include <AcisEnt_HSequenceOfAcisObject.hxx>
#include <AcisGeom_IntCur.hxx>
#include <AcisGeom_IntCurve.hxx>
#include <AcisGeom_Spline.hxx>
#include <AcisGeom_SplSur.hxx>
#include <AcisGeom_SubtypeData.hxx>
#include <AcisOther_BlendFunc.hxx>
#include <Approx_SweepApproximation.hxx>
#include <BRep_Builder.hxx>
#include <DBRep.hxx>
#include <DDocStd.hxx>
#include <DDocStd_DrawDocument.hxx>
#include <Draw.hxx>
#include <Draw_PluginMacro.hxx>
#include <DrawTrSurf.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <OSD_FileSystem.hxx>
#include <Precision.hxx>
#include <DESAT_ConfigurationNode.hxx>
#include <SATCAFControl_Reader.hxx>
#include <SATCAFControl_Writer.hxx>
#include <SatControl_Controller.hxx>
#include <SatControl_Reader.hxx>
#include <SatControl_Writer.hxx>
#include <ShapeProcess_OperLibrary.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_Array2OfInteger.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TDataStd_Name.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_WorkSession.hxx>
#include <XSDRAW.hxx>
#include <XSDRAWSAT.hxx>

//=======================================================================
// function : satread
// purpose  :
//=======================================================================

static Standard_Integer satread(Draw_Interpretor & /*theCommands*/, Standard_Integer argc, const char **argv)
{
  if (argc < 2)
  {
    std::cout << "Use:" << std::endl;
    std::cout << argv[0] << " <filename> [<variable>]" << std::endl;
    std::cout << "mode is empty:         translate all roots" << std::endl;
    std::cout << "mode is *:             translate all roots as one shape" << std::endl;
    std::cout << "mode is entity number: translate given entity" << std::endl;
    return 1;
  }

  TCollection_AsciiString fnom, rnom;
  Standard_Boolean modfic = XSDRAW::FileAndVar(argv[1], argv[2], "SAT", fnom, rnom);
  if (modfic)
    std::cout << " File SAT to read : " << fnom << std::endl;
  else
    std::cout << " Model taken from the session : " << fnom << std::endl;
  std::cout << " -- Names of variables BREP-DRAW prefixed by : " << rnom << std::endl;

  SatControl_Reader reader(XSDRAW::Session(), Standard_False);

  if (modfic)
  {
    IFSelect_ReturnStatus stat = reader.ReadFile(argv[1]);
    std::cout << "Status from reading DXF-SAT file " << argv[1] << " : ";
    switch (stat)
    {
    case IFSelect_RetVoid:
    {
      std::cout << "empty file" << std::endl;
      return 1;
    }
    case IFSelect_RetDone:
    {
      std::cout << "file read" << std::endl;
      break;
    }
    case IFSelect_RetError:
    {
      std::cout << "file not found" << std::endl;
      return 1;
    }
    case IFSelect_RetFail:
    {
      std::cout << "error during read" << std::endl;
      return 1;
    }
    default:
    {
      std::cout << "failure" << std::endl;
      return 1;
    }
    }
  }

  Standard_Boolean oneShape = Standard_False;
  if (argc > 3)
  {
    if (argv[3][0] == '*')
      oneShape = Standard_True;
    else
    {
      Handle(AcisData_InterfaceModel) model =
          Handle(AcisData_InterfaceModel)::DownCast(reader.Model());
      reader.ClearShapes();
      Handle(TColStd_HSequenceOfTransient) list = new TColStd_HSequenceOfTransient;
      for (Standard_Integer i = 3; i < argc; i++)
      {
        Standard_Integer num = Draw::Atoi(argv[i]);
        if (num <= 0)
          num = model->NextNumberForLabel(argv[i]);
        if (num <= 0)
        {
          std::cout << "Error: No entity with id " << argv[i] << " (" << num << ") found in the model" << std::endl;
          continue;
        }
        // add entity to be translated to the list
        Handle(Standard_Transient) ent = model->Value(num);
        list->Append(ent);
      }
      Standard_Boolean statone = reader.TransferList(list) != 0;
      if (!statone)
      {
        std::cout << "Entity " << argv[3] << " not transferred" << std::endl;
        return 1;
      }
      TopoDS_Shape sh = reader.OneShape();
      if (sh.IsNull())
      {
        std::cout << "No result" << std::endl;
        return 1;
      }
      std::cout << "Result saved in DRAW shape " << rnom << std::endl;
      DBRep::Set(rnom.ToCString(), sh);
      return 0;
    }
  }

  // translate all roots
  Standard_Integer nbroots;
  Standard_Integer recordsinmodel = reader.Model()->NbEntities();
  if (recordsinmodel == 0)
  {
    nbroots = 0;
    std::cout << "   ***** model is empty, transfer not perform *****" << std::endl;
  }
  else
  {
    nbroots = reader.NbRootsForTransfer();
    std::cout << "Nb Roots recognized for Transfer " << nbroots << std::endl;
  }

  // Standard_Integer nbroots = reader.NbRootsForTransfer();
  // std::cout<<"Nb Roots recognized for Transfer "<<nbroots<<std::endl;
  TopoDS_Compound C;
  TopoDS_Shape S;
  BRep_Builder B;
  B.MakeCompound(C);
  Standard_Integer nbRes = 0;
  for (Standard_Integer num = 1; num <= nbroots; num++)
  {
    std::cout << "Processing " << num << "-th root" << std::endl;
    // Each DXF-SAT part is transferred individually
    reader.ClearShapes();
    Standard_Boolean statone = reader.TransferOneRoot(num);
    if (statone)
    {
      std::cout << num << "-th root transferred to " << reader.NbShapes() << " Shape(s)" << std::endl;
      TopoDS_Shape sh = reader.OneShape();
      if (sh.IsNull())
      {
        std::cout << "No result" << std::endl;
        continue;
      }
      S = sh;
      B.Add(C, S);
      nbRes++;
      if (oneShape)
        continue;
      char name[200];
      Sprintf(name, "%s_%d", rnom.ToCString(), num);
      std::cout << "Result saved in DRAW shape " << name << std::endl;
      DBRep::Set(name, sh);
    }
    else
    {
      std::cout << num << "-th root  not transferred" << std::endl;
      continue;
    }
  }
  if (oneShape && nbRes > 0)
  {
    std::cout << "Result saved in DRAW shape " << rnom << std::endl;
    DBRep::Set(rnom.ToCString(), (nbRes > 1 ? (TopoDS_Shape &)C : S));
  }
  // another way,all roots in once  Standard_Integer nb = reader.TransferRoots();
  return 0;
}

//=======================================================================
// function : satreadsimple
// purpose  :
//=======================================================================

static Standard_Integer satreadsimple(Draw_Interpretor & /*di*/, Standard_Integer argc, const char **argv)
{
  if (argc != 3)
  {
    std::cout << "ERROR in " << argv[0] << "Wrong Number of Arguments." << std::endl;
    std::cout << " Usage : " << argv[0] << " file shape" << std::endl;
    return 1;
  }
  SatControl_Reader Reader;
  Standard_CString filename = argv[1];
  IFSelect_ReturnStatus readstat = Reader.ReadFile(filename);
  std::cout << "Status from reading SAT file " << filename << " : ";
  switch (readstat)
  {
  case IFSelect_RetVoid:
  {
    std::cout << "empty file" << std::endl;
    return 1;
  }
  case IFSelect_RetDone:
  {
    std::cout << "file read" << std::endl;
    break;
  }
  case IFSelect_RetError:
  {
    std::cout << "file not found" << std::endl;
    return 1;
  }
  case IFSelect_RetFail:
  {
    std::cout << "error during read" << std::endl;
    return 1;
  }
  default:
  {
    std::cout << "failure" << std::endl;
    return 1;
  }
  }
  Reader.TransferRoots();
  TopoDS_Shape shape = Reader.OneShape();
  DBRep::Set(argv[2], shape);
  std::cout << "Count of shapes produced : " << Reader.NbShapes() << std::endl;
  return 0;
}

//=======================================================================
// function : satwrite
// purpose  :
//=======================================================================

static Standard_Integer satwrite(Draw_Interpretor & /*theCommands*/, Standard_Integer argc, const char **argv)
{
  if (argc < 2)
  {
    std::cout << "Give shape name + file name" << std::endl;
    return 1;
  }
  SatControl_Writer aWriter(XSDRAW::Session(), Standard_False);
  TopoDS_Shape sh = DBRep::Get(argv[1]);
  if (sh.IsNull())
  {
    std::cout << "Shape " << argv[1] << " unknown" << std::endl;
    return 1;
  }
  IFSelect_ReturnStatus statr = aWriter.TransferShape(sh);
  if (statr == IFSelect_RetError || statr == IFSelect_RetFail)
  {
    std::cout << "Could not transfer shape " << argv[1] << std::endl;
    return 1;
  }
  if (argc < 3)
    return 0;

  IFSelect_ReturnStatus statw = aWriter.WriteFile(argv[2]);

  if (statw != IFSelect_RetDone)
  { // statw == IFSelect_RetError || statw == IFSelect_RetFail) {
    std::cout << "Could not write file " << argv[2] << std::endl;
    return 1;
  }
  return 0;
}

//=======================================================================
// function : satwritesimple
// purpose  :
//=======================================================================

static Standard_Integer satwritesimple(Draw_Interpretor & /*di*/, Standard_Integer n, const char **a)
{
  if (n != 3)
  {
    std::cout << "ERROR in " << a[0] << "Wrong Number of Arguments." << std::endl;
    std::cout << " Usage : " << a[0] << " file shape" << std::endl;
    return 1;
  }
  SatControl_Writer Writer;
  Standard_CString filename = a[1];
  TopoDS_Shape shape = DBRep::Get(a[2]);
  IFSelect_ReturnStatus stat = Writer.TransferShape(shape);
  stat = Writer.WriteFile(filename);
  if (stat != IFSelect_RetDone)
  {
    std::cout << "Error on writing file" << std::endl;
    return 1;
  }
  std::cout << "File Is Written" << std::endl;
  return 0;
}

//=======================================================================
// function : satload
// purpose  :
//=======================================================================

static Standard_Integer satload(Draw_Interpretor & /*theCommands*/, Standard_Integer argc, const char **argv)
{
  if (argc < 2)
  {
    std::cout << "Give file name !" << std::endl;
    return 1;
  }
  TCollection_AsciiString resfile, resvar;

  XSDRAW::FileAndVar(argv[1], argv[2], "SAT", resfile, resvar);

  XSControl_Reader reader(XSDRAW::Session(), Standard_True);
  reader.SetNorm("SAT");

  IFSelect_ReturnStatus stat = reader.ReadFile(argv[1]);

  std::cout << "Status from reading DXF-SAT file " << argv[1] << " : ";
  switch (stat)
  {
  case IFSelect_RetVoid:
  {
    std::cout << "empty file" << std::endl;
    return 1;
  }
  case IFSelect_RetDone:
  {
    std::cout << "file read" << std::endl;
    break;
  }
  case IFSelect_RetError:
  {
    std::cout << "file not found" << std::endl;
    return 1;
  }
  case IFSelect_RetFail:
  {
    std::cout << "error during read" << std::endl;
    return 1;
  }
  default:
  {
    std::cout << "failure" << std::endl;
    return 1;
  }
  }
  // stat is an enum. Integer counterpart : 0 Void  1 Done  2 Error  3 Fail

  Standard_Integer nbroots = reader.NbRootsForTransfer();
  std::cout << "Nb Roots recognized for Transfer " << nbroots << std::endl;

  if (argc > 2)
  {
    Handle(AcisData_InterfaceModel) anIterfaceModel = Handle(AcisData_InterfaceModel)::DownCast(reader.Model());
    if (!anIterfaceModel.IsNull())
      anIterfaceModel->WriteSaveFile(argv[2]);
  }

  return 0;
}

//=======================================================================
// function : satsave
// purpose  :
//=======================================================================

static Standard_Integer satsave(Draw_Interpretor & /*theCommands*/, Standard_Integer argc, const char **argv)
{
  if (argc < 2)
  {
    std::cout << "Give file name !" << std::endl;
    return 1;
  }
  Handle(IFSelect_SessionPilot) pilot = XSDRAW::Pilot();

  Handle(XSControl_WorkSession) WS = XSDRAW::Session(); // Handle(XSControl_WorkSession)::DownCast(pilot->Session());
  if (WS.IsNull())
  {
    std::cout << "Work session is null" << std::endl;
    return 1;
  }

  if (WS.IsNull())
    return 1;

  Handle(XSControl_TransferReader) aReader = WS->TransferReader();
  Handle(AcisData_InterfaceModel) anIterfaceModel = Handle(AcisData_InterfaceModel)::DownCast(aReader->Model());
  if (!anIterfaceModel.IsNull())
    anIterfaceModel->WriteSaveFile(argv[1]);

  return 0;
}

//=======================================================================
// function : satstatentity
// purpose  :
//=======================================================================

static void statlevel(Standard_Integer aLevel, Standard_Integer num, TColStd_Array2OfInteger &Approx)
{
  switch (aLevel)
  {
  case AcisAbs_SaveApproxFull:
    Approx.ChangeValue(num, 1)++;
    break;
  case AcisAbs_SaveApproxSummary:
    Approx.ChangeValue(num, 2)++;
    break;
  case AcisAbs_SaveApproxUnset:
    Approx.ChangeValue(num, 3)++;
    break;
  default:
    break;
  }
}

static Standard_Integer satstatentity(Draw_Interpretor & /*theCommands*/, Standard_Integer argc, const char **argv)
{
  if (argc < 1)
  {
    std::cout << "Give file name !" << std::endl;
    return 1;
  }

  XSControl_Reader reader(XSDRAW::Session(), Standard_True);
  reader.SetNorm("SAT");
  Standard_Boolean aModeReadEntity = Standard_False;
  IFSelect_ReturnStatus stat = reader.ReadFile(argv[1]);
  if (argc > 2)
    aModeReadEntity = (argv[2][0] == 'e' ? Standard_True : Standard_False);

  std::cout << "Status from reading DXF-SAT file " << argv[1] << " : ";
  switch (stat)
  {
  case IFSelect_RetVoid:
  {
    std::cout << "empty file" << std::endl;
    return 1;
  }
  case IFSelect_RetDone:
  {
    std::cout << "file read" << std::endl;
    break;
  }
  case IFSelect_RetError:
  {
    std::cout << "file not found" << std::endl;
    return 1;
  }
  case IFSelect_RetFail:
  {
    std::cout << "error during read" << std::endl;
    return 1;
  }
  default:
  {
    std::cout << "failure" << std::endl;
    return 1;
  }
  }
  // stat is an enum. Integer counterpart : 0 Void  1 Done  2 Error  3 Fail

  Standard_Integer nbroots = reader.NbRootsForTransfer();
  std::cout << "Nb Roots recognized for Transfer " << nbroots << std::endl;

  Handle(AcisData_InterfaceModel) anIterfaceModel = Handle(AcisData_InterfaceModel)::DownCast(reader.Model());
  if (anIterfaceModel.IsNull())
    return 1;
  const Handle(AcisData_AcisModel) &anAcisModel = anIterfaceModel->Data();
  Standard_Integer NbEnt = anAcisModel->NbEntities();
  Handle(AcisEnt_HSequenceOfAcisObject) aSeqEntities = anAcisModel->GetEntities();
  Standard_Integer aVer = anAcisModel->Version();

  const AcisAbs_IndexedDataMapOfObjectType &aMapSubtypes = AcisGeom_SubtypeData::GetMapSubtypes();
  Standard_Integer aNbSubTypes = aMapSubtypes.Extent();
  TColStd_Array2OfInteger TypesOfSpline(1, aNbSubTypes, 1, 3);
  TypesOfSpline.Init(0);
  TColStd_Array2OfInteger TypesOfIntCurve(1, aNbSubTypes, 1, 3);
  TypesOfIntCurve.Init(0);
  Standard_Integer aNumberSpline = 0, aNumberIntCurve = 0;
  Handle(AcisEnt_HSequenceOfAcisObject) aSeqSubtypes = new AcisEnt_HSequenceOfAcisObject;
  NbEnt = anIterfaceModel->NbEntities();

  Standard_Integer aNbUnRecSurf = 0;
  Standard_Integer aNbUnRecCur = 0;

  for (Standard_Integer j = 1; j <= NbEnt; j++)
  {
    Handle(AcisEnt_AcisObject) anObjectSub = Handle(AcisEnt_AcisObject)::DownCast(anIterfaceModel->Value(j)); // aSeqSubtypes->Value(j);
    if (anObjectSub.IsNull())
      continue;
    Handle(AcisGeom_SplSur) aSplSur;
    Handle(AcisGeom_IntCur) aIntCur;
    if (aModeReadEntity && anObjectSub->GetType() == AcisAbs_GeomSpline)
    {
      aNumberSpline++;
      aSplSur = Handle(AcisGeom_Spline)::DownCast(anObjectSub)->mySplSur;
      if (aSplSur.IsNull())
      {
        aNbUnRecSurf++;
        continue;
      }
    }

    else if (aModeReadEntity && anObjectSub->GetType() == AcisAbs_GeomIntCurve)
    {
      aNumberIntCurve++;
      aIntCur = Handle(AcisGeom_IntCurve)::DownCast(anObjectSub)->myIntCur;
      if (aIntCur.IsNull())
      {
        aNbUnRecCur++;
        continue;
      }
    }
    else if (!aModeReadEntity)
    {
      aSplSur = Handle(AcisGeom_SplSur)::DownCast(anObjectSub);
      if (!aSplSur.IsNull())
        aNumberSpline++;
      else
      {
        aIntCur = Handle(AcisGeom_IntCur)::DownCast(anObjectSub);
        if (!aIntCur.IsNull())
          aNumberIntCurve++;
      }
    }

    if (!aSplSur.IsNull())
    {
      const Standard_Integer aIndex = aMapSubtypes.FindIndex(aSplSur->KeyWord());
      statlevel(aSplSur->mySaveApproxLevel, aIndex, TypesOfSpline);
    }
    if (!aIntCur.IsNull())
    {
      const AcisAbs_SaveApproxLevel aLevel = aIntCur->myApproxLevel;
      Standard_CString aKeyWord = aIntCur->KeyWord();
      const Standard_Integer aIndex = aMapSubtypes.FindIndex(aKeyWord);
      statlevel(aLevel, aIndex, TypesOfIntCurve);
    }
  }

  std::cout << "File: " << argv[1] << std::endl;
  std::cout << "--------------------------------" << std::endl;
  std::cout << "Version: " << aVer << std::endl;
  std::cout << "Number of spline surfaces: " << aNumberSpline << std::endl;
  std::cout << "-------------------------------------------------------------" << std::endl;
  if (aNumberSpline)
  {
    // char str[128];
    //   Sprintf(str,"Subtype of spline : full"
    std::cout << "Subtype of spline : " << "full" << '\t' << "summary" << '\t' << "unset" << std::endl;
    std::cout << "-------------------------------------------------------------" << std::endl;
    for (Standard_Integer i = 1; i <= aNbSubTypes; i++)
    {
      Standard_Integer sumnumber = 0;
      for (Standard_Integer k = 1; k <= 3; k++)
        sumnumber += TypesOfSpline.Value(i, k);
      if (!sumnumber)
        continue;
      TCollection_AsciiString aKeyWord = aMapSubtypes.FindKey(i);
      char str[128];
      Sprintf(str, "%-20s %-10d %-10d %-10d\n", aKeyWord.ToCString(), TypesOfSpline.Value(i, 1), TypesOfSpline.Value(i, 2), TypesOfSpline.Value(i, 3));
      std::cout << str << std::endl;
    }
  }
  std::cout << "Number of intcurves : " << aNumberIntCurve << std::endl;
  if (aNumberIntCurve)
  {
    std::cout << "Subtype of intcurve : " << "full" << '\t' << "summary" << '\t' << "unset" << std::endl;
    std::cout << "-------------------------------------------------------------" << std::endl;
    for (Standard_Integer i = 1; i <= aNbSubTypes; i++)
    {
      Standard_Integer sumnumber = 0;
      for (Standard_Integer k = 1; k <= 3; k++)
        sumnumber += TypesOfIntCurve.Value(i, k);
      if (!sumnumber)
        continue;
      TCollection_AsciiString aKeyWord = aMapSubtypes.FindKey(i);
      char string[128];
      Sprintf(string, "%-20s %-10d %-10d %-10d", aKeyWord.ToCString(), TypesOfIntCurve.Value(i, 1), TypesOfIntCurve.Value(i, 2), TypesOfIntCurve.Value(i, 3));
      std::cout << string << std::endl;
    }
  }
  return 0;
}

//=======================================================================
// function : blendcurves
// purpose  :
//=======================================================================

static Standard_Integer blendcurves(Draw_Interpretor & /*theCommands*/, Standard_Integer argc, const char **argv)
{
  if (argc < 5)
  {
    std::cout << "wrong arguments number" << std::endl;
    return 1;
  }

  Handle(Geom_Curve) aPath = Handle(Geom_Curve)::DownCast(DrawTrSurf::Get(argv[2]));
  if (aPath.IsNull() || Precision::IsInfinite(aPath->FirstParameter()) || Precision::IsInfinite(aPath->LastParameter()))
  {
    std::cout << "wrong path curve" << std::endl;
    return 1;
  }

  Handle(Geom_Curve) aCurve1 = Handle(Geom_Curve)::DownCast(DrawTrSurf::Get(argv[3]));
  if (aCurve1.IsNull() || Precision::IsInfinite(aCurve1->FirstParameter()) || Precision::IsInfinite(aCurve1->LastParameter()))
  {
    std::cout << "wrong first curve" << std::endl;
    return 1;
  }

  Handle(Geom_Curve) aCurve2 = Handle(Geom_Curve)::DownCast(DrawTrSurf::Get(argv[4]));
  if (aCurve2.IsNull() || Precision::IsInfinite(aCurve2->FirstParameter()) || Precision::IsInfinite(aCurve2->LastParameter()))
  {
    std::cout << "wrong second curve" << std::endl;
    return 1;
  }

  Handle(AcisOther_BlendFunc) Func = new AcisOther_BlendFunc(new GeomAdaptor_Curve(aPath),
                                                             new GeomAdaptor_Curve(aCurve1),
                                                             new GeomAdaptor_Curve(aCurve2));

  Approx_SweepApproximation App(Func);
  App.Perform(aPath->FirstParameter(),
              aPath->LastParameter(),
              Precision::Confusion(), Precision::Confusion(), Precision::Confusion(), 0.01);

  if (App.IsDone())
  {
    Handle(Geom_BSplineSurface) aSpline;

    if (App.IsDone())
    {
      aSpline = new Geom_BSplineSurface(App.SurfPoles(),
                                        App.SurfWeights(),
                                        App.SurfUKnots(),
                                        App.SurfVKnots(),
                                        App.SurfUMults(),
                                        App.SurfVMults(),
                                        App.UDegree(),
                                        App.VDegree());
    }
    if (aSpline.IsNull())
    {
      std::cout << "null surface" << std::endl;
      return 1;
    }

    DrawTrSurf::Set(argv[1], aSpline);
  }
  else
  {
    std::cout << "null surface" << std::endl;
    return 1;
  }

  return 0;
}

//=======================================================================
// function : ReadSat
// purpose  : Read SAT to DECAF document
//=======================================================================

static Standard_Integer ReadSat(Draw_Interpretor &di, Standard_Integer argc, const char **argv)
{
  if (argc < 3)
  {
    std::cout << "Use: " << argv[0] << " Doc filename [mode]: read SAT file to a document" << std::endl;
    return 0;
  }

  Handle(SatControl_Controller) ctl = Handle(SatControl_Controller)::DownCast(XSDRAW::Controller());
  if (ctl.IsNull())
    XSDRAW::SetNorm("SAT");

  SATCAFControl_Reader reader(XSDRAW::Session(), Standard_True);

  if (argc == 4)
  {
    Standard_Boolean mode = Standard_True;
    for (Standard_Integer i = 0; argv[3][i]; i++)
      switch (argv[3][i])
      {
      case '-':
        mode = Standard_False;
        break;
      case '+':
        mode = Standard_True;
        break;
      case 'c':
        reader.SetColorMode(mode);
        break;
      case 'n':
        reader.SetNameMode(mode);
        break;
      }
  }

  TCollection_AsciiString fnom, rnom;
  Standard_Boolean modfic = XSDRAW::FileAndVar(argv[2], argv[1], "SAT", fnom, rnom);
  if (modfic)
    std::cout << " File SAT to read : " << fnom << std::endl;
  else
    std::cout << " Model taken from the session : " << fnom << std::endl;
  //  std::cout<<" -- Names of variables BREP-DRAW prefixed by : "<<rnom<<std::endl;
  IFSelect_ReturnStatus readstat = IFSelect_RetVoid;
  if (modfic)
    readstat = reader.ReadFile(fnom.ToCString());
  else if (XSDRAW::Session()->NbStartingEntities() > 0)
    readstat = IFSelect_RetDone;
  if (readstat != IFSelect_RetDone)
  {
    if (modfic)
      std::cout << "Could not read file " << fnom << " , abandon" << std::endl;
    else
      std::cout << "No model loaded" << std::endl;
    return 1;
  }

  Handle(TDocStd_Document) doc;
  if (!DDocStd::GetDocument(argv[1], doc, Standard_False))
  {
    Handle(TDocStd_Application) A = DDocStd::GetApplication();
    A->NewDocument("BinXCAF", doc);
    TDataStd_Name::Set(doc->GetData()->Root(), argv[1]);
    Handle(DDocStd_DrawDocument) DD = new DDocStd_DrawDocument(doc);
    Draw::Set(argv[1], DD);
    //     di << "Document saved with name " << argv[1];
  }
  reader.ClearShapes();
  reader.NbRootsForTransfer(); // to initialize list of roots
  if (!reader.Transfer(doc))
  {
    std::cout << "Cannot read any relevant data from the SAT file" << std::endl;
    return 1;
  }

  //  Handle(DDocStd_DrawDocument) DD = new DDocStd_DrawDocument(doc);
  //  Draw::Set(argv[1],DD);
  di << "Document saved with name " << argv[1];

  return 0;
}

//=======================================================================
// function : WriteSat
// purpose  : Write DECAF document to SAT
//=======================================================================

static Standard_Integer WriteSat(Draw_Interpretor & /*di*/, Standard_Integer argc, const char **argv)
{
  if (argc < 3)
  {
    std::cout << "Use: " << argv[0] << " Doc filename [mode]: write document to SAT file" << std::endl;
    return 0;
  }

  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if (Doc.IsNull())
  {
    std::cout << argv[1] << " is not a document" << std::endl;
    return 1;
  }

  XSDRAW::SetNorm("SAT");

  SATCAFControl_Writer writer(XSDRAW::Session(), Standard_True);
  if (argc == 4)
  {
    Standard_Boolean mode = Standard_True;
    for (Standard_Integer i = 0; argv[3][i]; i++)
      switch (argv[3][i])
      {
      case '-':
        mode = Standard_False;
        break;
      case '+':
        mode = Standard_True;
        break;
      case 'c':
        writer.SetColorMode(mode);
        break;
      case 'n':
        writer.SetNameMode(mode);
        break;
      }
  }
  writer.Transfer(Doc);

  std::cout << "Writing SAT model to file " << argv[2] << std::endl;
  if (writer.WriteFile(argv[2]))
    std::cout << " Write OK" << std::endl;
  else
    std::cout << " Write failed" << std::endl;

  return 0;
}

//=======================================================================
// function : satreadstream
// purpose  :
//=======================================================================

static Standard_Integer satreadstream(Draw_Interpretor & /*di*/, Standard_Integer /*argc*/, const char **argv)
{
  SatControl_Reader Reader;
  Standard_CString filename = argv[1];

  std::ifstream strm(filename);
  IFSelect_ReturnStatus readstat = Reader.ReadStream(strm);
  std::cout << "Status from reading SAT file " << filename << " : ";
  switch (readstat)
  {
  case IFSelect_RetVoid:
  {
    std::cout << "empty file" << std::endl;
    return 1;
  }
  case IFSelect_RetDone:
  {
    std::cout << "file read" << std::endl;
    break;
  }
  case IFSelect_RetError:
  {
    std::cout << "file not found" << std::endl;
    return 1;
  }
  case IFSelect_RetFail:
  {
    std::cout << "error during read" << std::endl;
    return 1;
  }
  default:
  {
    std::cout << "failure" << std::endl;
    return 1;
  }
  }
  Reader.TransferRoots();
  TopoDS_Shape shape = Reader.OneShape();
  DBRep::Set(argv[2], shape);
  std::cout << "Count of shapes produced : " << Reader.NbShapes() << std::endl;
  return 0;
}

//=======================================================================
// function : ReadSatStream
// purpose  : Read SAT to DECAF document
//=======================================================================

static Standard_Integer ReadSatStream(Draw_Interpretor &di, Standard_Integer argc, const char **argv)
{
  if (argc < 3)
  {
    std::cout << "Use: " << argv[0] << " Doc filename [mode]: read SAT file to a document" << std::endl;
    return 0;
  }

  Handle(SatControl_Controller) ctl =
      Handle(SatControl_Controller)::DownCast(XSDRAW::Controller());
  if (ctl.IsNull())
    XSDRAW::SetNorm("SAT");

  SATCAFControl_Reader reader(XSDRAW::Session(), Standard_True);

  if (argc == 4)
  {
    Standard_Boolean mode = Standard_True;
    for (Standard_Integer i = 0; argv[3][i]; i++)
      switch (argv[3][i])
      {
      case '-':
        mode = Standard_False;
        break;
      case '+':
        mode = Standard_True;
        break;
      case 'c':
        reader.SetColorMode(mode);
        break;
      case 'n':
        reader.SetNameMode(mode);
        break;
      }
  }

  TCollection_AsciiString fnom, rnom;
  Standard_Boolean modfic = XSDRAW::FileAndVar(argv[2], argv[1], "SAT", fnom, rnom);
  if (modfic)
    std::cout << " File SAT to read : " << fnom << std::endl;
  else
    std::cout << " Model taken from the session : " << fnom << std::endl;
  IFSelect_ReturnStatus readstat = IFSelect_RetVoid;
  if (modfic)
  {
    std::ifstream strm(fnom.ToCString());
    readstat = reader.ReadStream(strm);
  }
  else if (XSDRAW::Session()->NbStartingEntities() > 0)
    readstat = IFSelect_RetDone;
  if (readstat != IFSelect_RetDone)
  {
    if (modfic)
      std::cout << "Could not read file " << fnom << " , abandon" << std::endl;
    else
      std::cout << "No model loaded" << std::endl;
    return 1;
  }

  Handle(TDocStd_Document) doc;
  if (!DDocStd::GetDocument(argv[1], doc, Standard_False))
  {
    Handle(TDocStd_Application) A = DDocStd::GetApplication();
    A->NewDocument("BinXCAF", doc);
    TDataStd_Name::Set(doc->GetData()->Root(), argv[1]);
    Handle(DDocStd_DrawDocument) DD = new DDocStd_DrawDocument(doc);
    Draw::Set(argv[1], DD);
  }
  reader.ClearShapes();
  reader.NbRootsForTransfer(); // to initialize list of roots
  if (!reader.Transfer(doc))
  {
    std::cout << "Cannot read any relevant data from the SAT file" << std::endl;
    return 1;
  }

  di << "Document saved with name " << argv[1];

  return 0;
}

//=======================================================================
// function : WriteSatStream
// purpose  : Write DECAF document to SAT
//=======================================================================

static Standard_Integer WriteSatStream(Draw_Interpretor & /*di*/, Standard_Integer argc, const char **argv)
{
  if (argc < 3)
  {
    std::cout << "Use: " << argv[0] << " Doc filename [mode]: write document to SAT file" << std::endl;
    return 0;
  }

  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if (Doc.IsNull())
  {
    std::cout << argv[1] << " is not a document" << std::endl;
    return 1;
  }

  XSDRAW::SetNorm("SAT");

  SATCAFControl_Writer writer(XSDRAW::Session(), Standard_True);
  if (argc == 4)
  {
    Standard_Boolean mode = Standard_True;
    for (Standard_Integer i = 0; argv[3][i]; i++)
      switch (argv[3][i])
      {
      case '-':
        mode = Standard_False;
        break;
      case '+':
        mode = Standard_True;
        break;
      case 'c':
        writer.SetColorMode(mode);
        break;
      case 'n':
        writer.SetNameMode(mode);
        break;
      }
  }
  writer.Transfer(Doc);

  TCollection_AsciiString ascStr(argv[2]);
  ascStr.LowerCase();
  std::cout << "Writing SAT model to stream " << argv[2] << std::endl;
  Standard_Boolean isText = (ascStr.Search(".sab") == -1);

  const Handle(OSD_FileSystem) &aFileSystem = OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::ostream> aStream = aFileSystem->OpenOStream(argv[2], isText ? std::ios::out : (std::ios::binary | std::ios::out));

  if (writer.WriteStream(*aStream, isText))
    std::cout << " Write OK" << std::endl;
  else
    std::cout << " Write failed" << std::endl;

  return 0;
}

//=======================================================================
// function : Init
// purpose  :
//=======================================================================

void XSDRAWSAT::Init()
{
  SatControl_Controller::Init();
}

//=======================================================================
// function : InitCommands
// purpose  :
//=======================================================================

void XSDRAWSAT::InitCommands(Draw_Interpretor &theCommands)
{
  SatControl_Controller::Init();

  const char *g = "XSTEP";

  theCommands.Add("satreadsimple", "satreadsimple filename shape", __FILE__, satreadsimple, g);
  theCommands.Add("satwritesimple", "satwritesimple filename shape", __FILE__, satwritesimple, g);
  theCommands.Add("satread", "satread filename [varname [entity]]", __FILE__, satread, g);
  theCommands.Add("satwrite", "satwrite varname filename", __FILE__, satwrite, g);
  theCommands.Add("satload", "satload filename [<name of file to save>]", __FILE__, satload, g);
  theCommands.Add("satsave", "satsave filename", __FILE__, satsave, g);
  theCommands.Add("satstatentity", "satstatentity filename", __FILE__, satstatentity, g);
  theCommands.Add("blendcurves", "blendcurves result path curve1 curve2", __FILE__, blendcurves, g);

  theCommands.Add("ReadSat", "Doc filename: Read SAT file to DECAF document", __FILE__, ReadSat, "DECAF");
  theCommands.Add("WriteSat", "Doc filename: Write DECAF document to SAT file", __FILE__, WriteSat, "DECAF");
  theCommands.Add("satreadstream", "testsatreadstream filename shape", __FILE__, satreadstream, g);
  theCommands.Add("ReadSatStream", "Doc filename: Read stream to DECAF document", __FILE__, ReadSatStream, "DECAF");
  theCommands.Add("WriteSatStream", "Doc filename: Write DECAF document to stream", __FILE__, WriteSatStream, "DECAF");
}

//==============================================================================
// XSDRAWSAT::Factory
//==============================================================================

void XSDRAWSAT::Factory(Draw_Interpretor &theDI)
{
  XSDRAW::LoadSession();

  XSDRAWSAT::InitCommands(theDI);

  ShapeProcess_OperLibrary::Init();

#ifdef DEB
  std::cout << "Draw Plugin : All TKXSDRAWSAT commands are loaded" << std::endl;
#endif
}

// Declare entry point PLUGINFACTORY
DPLUGIN(XSDRAWSAT)
