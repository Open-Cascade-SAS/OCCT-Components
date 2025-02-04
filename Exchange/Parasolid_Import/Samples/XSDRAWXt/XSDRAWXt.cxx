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

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BSplCLib.hxx>
#include <DBRep.hxx>
#include <DDocStd.hxx>
#include <DDocStd_DrawDocument.hxx>
#include <Draw.hxx>
#include <Draw_PluginMacro.hxx>
#include <DrawTrSurf.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <GeomFill_Pipe.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Message_Messenger.hxx>
#include <OSD_Parallel.hxx>
#include <Precision.hxx>
#include <ShapeBuild_Edge.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TDataStd_Name.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TopAbs.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <XSAlgo.hxx>
#include <XSAlgo_AlgoContainer.hxx>
#include <XSControl_WorkSession.hxx>
#include <XSDRAW.hxx>
#include <XSDRAWXt.hxx>
#include <DEXT_ConfigurationNode.hxx>
#include <XtCAFControl_Reader.hxx>
#include <XtControl_Controller.hxx>
#include <XtControl_Reader.hxx>
#include <XtToGeom_MakeBlendedEdge.hxx>
#include <UnitsMethods.hxx>

#include <stdio.h>

static Standard_Integer xtread(Draw_Interpretor & /*theCommands*/, Standard_Integer argc, const char **argv)
{

  DeclareAndCast(XtControl_Controller, ctl, XSDRAW::Controller());
  if (ctl.IsNull())
    XSDRAW::SetNorm("Xt");

  XtControl_Reader xtr(XSDRAW::Session(), Standard_False);
  TCollection_AsciiString fnom, rnom;
  Standard_Boolean modfic = XSDRAW::FileAndVar(argv[1], argv[2], "Xt", fnom, rnom);
  if (modfic)
    std::cout << " File Xt to read : " << fnom << std::endl;
  else
    std::cout << " Model taken from the session : " << fnom << std::endl;
  std::cout << " -- Names of variables BREP-DRAW prefixed by : " << rnom << std::endl;
  XSAlgo::AlgoContainer()->PrepareForTransfer();
  const Standard_Real aLengthUniteFromMeter = 1000. / UnitsMethods::GetCasCadeLengthUnit();
  IFSelect_ReturnStatus readstat = IFSelect_RetVoid;
  if (modfic)
    readstat = xtr.ReadFile(fnom.ToCString(), aLengthUniteFromMeter);
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

  Standard_Boolean fromtcl = (argc > 3);
  //   nom = "." -> fichier deja lu
  Standard_Integer i, num, nbs, modepri = 1;
  if (fromtcl)
    modepri = 4;
  while (modepri)
  {
    num = xtr.NbRootsForTransfer();
    if (!fromtcl)
    {
      // make messenger by default connected to std::cout
      std::cout << "NbRootsForTransfer=" << num << " :\n";
      for (i = 1; i <= num; i++)
      {
        std::cout << "Root." << i << ", Ent. ";
        xtr.Model()->Print(xtr.RootForTransfer(i), std::cout);
        std::cout << " Type:" << xtr.RootForTransfer(i)->DynamicType()->Name() << std::endl;
      }

      std::cout << "Mode (0 End, 1 root n0 1, 2 one root/n0, 3 one entity/n0, 4 Selection) : " << std::flush;
      std::cin >> modepri;
    }

    if (modepri == 0)
    {
      std::cout << "End Reading Xt" << std::endl;
      return 0;
    }
    if (modepri <= 2)
    {
      num = 1;
      if (modepri == 2)
      {
        std::cout << "Root N0 : " << std::flush;
        std::cin >> num;
      }
      if (!xtr.TransferRoot(num))
        std::cout << "Transfer root n0 " << num << " : no result" << std::endl;
      else
      {
        nbs = xtr.NbShapes();
        char shname[30];
        Sprintf(shname, "%s_%d", rnom.ToCString(), nbs);
        std::cout << "Transfer root n0 " << num << " OK  -> DRAW Shape: " << shname << std::endl;
        std::cout << "Now, " << nbs << " Shapes produced" << std::endl;
        TopoDS_Shape sh = xtr.Shape(nbs);
        DBRep::Set(shname, sh);
      }
    }
    else if (modepri == 3)
    {
      std::cout << "Entity : " << std::flush;
      num = XSDRAW::GetEntityNumber();
      if (!xtr.TransferOne(num))
        std::cout << "Transfer entity n0 " << num << " : no result" << std::endl;
      else
      {
        nbs = xtr.NbShapes();
        char shname[30];
        Sprintf(shname, "%s_%d", rnom.ToCString(), num);
        std::cout << "Transfer entity n0 " << num << " OK  -> DRAW Shape: " << shname << std::endl;
        std::cout << "Now, " << nbs << " Shapes produced" << std::endl;
        TopoDS_Shape sh = xtr.Shape(nbs);
        DBRep::Set(shname, sh);
      }
    }
    else if (modepri == 4)
    {
      //      char snm[100];  Standard_Integer answer = 1;
      Handle(TColStd_HSequenceOfTransient) list;

      //  Selection, nommee ou via tcl. tcl : raccourcis admis
      //   * donne xst-transferrable-roots
      if (fromtcl)
      {
        modepri = 0; // d ioffice une seule passe
        if (argv[3][0] == '*' && argv[3][1] == '\0')
        {
          std::cout << "Transferrable Roots : ";
          list = XSDRAW::GetList("xst-transferrable-roots");
        }
        else
        {
          std::cout << "List given by " << argv[3];
          if (argc > 4)
            std::cout << " " << argv[4];
          std::cout << " : ";
          list = XSDRAW::GetList(argv[3], argv[4]);
        }
        if (list.IsNull())
        {
          std::cout << "No list defined. Give a selection name or * for all transferrable roots" << std::endl;
          continue;
        }
      }
      else
      {
        std::cout << "Name of Selection :" << std::flush;
        list = XSDRAW::GetList();
        if (list.IsNull())
        {
          std::cout << "No list defined" << std::endl;
          continue;
        }
      }

      Standard_Integer ill, nbl = list->Length();
      std::cout << "Nb entities selected : " << nbl << std::endl;
      if (nbl == 0)
        continue;

      for (ill = 1; ill <= nbl; ill++)
      {
        num = xtr.Model()->Number(list->Value(ill));
        if (num == 0)
          continue;
        if (!xtr.TransferOne(num))
          std::cout << "Transfer entity n0 " << num << " : no result" << std::endl;
        else
        {
          nbs = xtr.NbShapes();
          char shname[30];
          Sprintf(shname, "%s_%d", rnom.ToCString(), nbs);
          std::cout << "Transfer entity n0 " << num << " OK  -> DRAW Shape: " << shname << std::endl;
          std::cout << "Now, " << nbs << " Shapes produced" << std::endl;
          TopoDS_Shape sh = xtr.Shape(nbs);
          DBRep::Set(shname, sh);
        }
      }
    }
    else
      std::cout << "Unknown mode n0 " << modepri << std::endl;
  }
  return 0;
}

//=======================================================================
// function : testreadxt
// purpose  :
//=======================================================================
static Standard_Integer testread(Draw_Interpretor & /*di*/, Standard_Integer argc, const char **argv)
{
  if (argc != 3)
  {
    std::cout << "ERROR in " << argv[0] << "Wrong Number of Arguments." << std::endl;
    std::cout << " Usage : " << argv[0] << " file_name shape_name" << std::endl;
    return 1;
  }
  XtControl_Reader Reader;
  Standard_CString filename = argv[1];
  XSAlgo::AlgoContainer()->PrepareForTransfer();
  const Standard_Real aLengthUniteFromMeter = 1000. / UnitsMethods::GetCasCadeLengthUnit();
  IFSelect_ReturnStatus readstat = Reader.ReadFile(filename, aLengthUniteFromMeter);
  std::cout << "Status from reading Xt file " << filename << " : ";
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

static Standard_Integer buildC3dFromC2d(Draw_Interpretor & /*di*/,
                                        Standard_Integer n, const char **a)
{
  if (n < 4)
  {
    std::cout << "Use correct number of argument" << std::endl;
    return 1;
  }
  Handle(Geom_Curve) aCurve3d;
  Handle(Geom2d_Curve) theCurve2d = DrawTrSurf::GetCurve2d(a[2]);
  Handle(Geom_Surface) theSurf = DrawTrSurf::GetSurface(a[3]);

  Standard_Real aTol;
  if (n >= 5)
    aTol = Draw::Atof(a[4]);
  else
    aTol = Precision::Confusion();

  Standard_Real aFirst = theCurve2d->FirstParameter();
  Standard_Real aLast = theCurve2d->LastParameter();
  GeomAbs_Shape theContinuity = GeomAbs_C1;
  aCurve3d = XtToGeom_MakeBlendedEdge::MakeCurve3dFrom2d(theCurve2d,
                                                         theSurf,
                                                         aTol,
                                                         theContinuity,
                                                         aFirst, aLast);
  if (!aCurve3d.IsNull())
  {
    std::cout << "Made curve 3d" << std::endl;
    DrawTrSurf::Set(a[1], aCurve3d);
  }

  return 0;
}

//=======================================================================
// function : ReadXt
// purpose  : Read Xt file to DECAF document
//=======================================================================

static Standard_Integer ReadXt(Draw_Interpretor &di, Standard_Integer argc, const char **argv)
{
  if (argc < 3)
  {
    std::cout << "Use: " << argv[0] << " Doc filename [mode]: read Xt file to a document" << std::endl;
    return 0;
  }

  Handle(XtControl_Controller) ctl = Handle(XtControl_Controller)::DownCast(XSDRAW::Controller());
  if (ctl.IsNull())
    XSDRAW::SetNorm("Xt");

  XtCAFControl_Reader reader(XSDRAW::Session(), Standard_True);

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
        //    case 'l' : reader.SetLayerMode (mode); break;
        //    case 'v' : reader.SetPropsMode (mode); break;
      }
  }

  TCollection_AsciiString fnom, rnom;
  Standard_Boolean modfic = XSDRAW::FileAndVar(argv[2], argv[1], "Xt", fnom, rnom);
  if (modfic)
    std::cout << " File Xt to read : " << fnom << std::endl;
  else
    std::cout << " Model taken from the session : " << fnom << std::endl;
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
  if (!reader.Transfer(doc))
  {
    std::cout << "Cannot read any relevant data from the Xt file" << std::endl;
    return 1;
  }

  Handle(DDocStd_DrawDocument) DD = new DDocStd_DrawDocument(doc);
  Draw::Set(argv[1], DD);
  di << "Document saved with name " << argv[1];

  return 0;
}

class MyActor
{
public:
  MyActor()
  {
  }

  void operator()(const Standard_Integer & /*theIndex*/) const
  {
    const Standard_Integer aMaxReaderNb = 1000;
    for (Standard_Integer anIdx = 0; anIdx <= aMaxReaderNb; ++anIdx)
    {
      XtControl_Reader reader;
    }
  }

private:
  void operator=(const MyActor & /*theOther*/)
  {
  }
};

//=======================================================================
// function : testparallel
// purpose  : Tests thread-safety of the XtControl_Reader.
//=======================================================================

static int testparallel(Draw_Interpretor &di,
                        Standard_Integer /*argc*/,
                        const char ** /*argv*/)
{
  const Standard_Integer nbProc = OSD_Parallel::NbLogicalProcessors();
  di << "Info: Nb logical processors = " << nbProc << "\n";

  XtControl_Controller::Init();

  MyActor anActor;
  OSD_Parallel::For(0, 100, anActor);

  return 0; // TCL_OK
}

void XSDRAWXt::InitCommands(Draw_Interpretor &theCommands)
{
  const char *g = "XSTEP-Xt"; // Step transfer file commands

  XtControl_Controller::Init();

  theCommands.Add("xtread", "xtread  [file]", __FILE__, xtread, g);
  theCommands.Add("testreadxt", "testreadxt [file] [name DRAW]", __FILE__, testread, g);
  theCommands.Add("ReadXt", "Doc filename: Read Xt file to DECAF document", __FILE__, ReadXt, g);
  // commands added for test case in Parasolid translator
  theCommands.Add("geomfillpipe",
                  "geomfillpipe result Path Radius Curve2 Curve3 make pipe by 3 curves and radius ",
                  __FILE__, geomfillpipe, g);
  theCommands.Add("buildC3dFromC2d", "buildC3dFromC2d resultC3d Curve2d Surafce [tolerance]",
                  __FILE__, buildC3dFromC2d, g);
  theCommands.Add("scaleSurfParamsbyV", "scaleSurfParamsbyV shape scale byV[1\0]",
                  __FILE__, scaleSurfParamsbyV, g);
  theCommands.Add("testparxt", "testparxt", __FILE__, testparallel, g);
}

//==============================================================================
// XSDRAWXt::Factory
//==============================================================================
void XSDRAWXt::Factory(Draw_Interpretor &theDI)
{
  XSDRAW::LoadSession();

  XSDRAWXt::InitCommands(theDI);

#ifdef DEB
  std::cout << "Draw Plugin : All TKXSDRAWXt commands are loaded" << std::endl;
#endif
}

// Declare entry point PLUGINFACTORY
DPLUGIN(XSDRAWXt)
