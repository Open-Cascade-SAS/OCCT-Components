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
#include <DBRep.hxx>
#include <DDocStd.hxx>
#include <DDocStd_DrawDocument.hxx>
#include <Draw.hxx>
#include <Draw_PluginMacro.hxx>
#include <DEDXF_ConfigurationNode.hxx>
#include <DXFCAFControl_Reader.hxx>
#include <DXFCAFControl_Writer.hxx>
#include <DxfControl_Controller.hxx>
#include <DxfControl_Reader.hxx>
#include <DxfControl_Writer.hxx>
#include <DxfFile_FileReader.hxx>
#include <DxfFile_FileWriter.hxx>
#include <DxfFile_RWDxfFile.hxx>
#include <DxfSection_ConfParameters.hxx>
#include <DxfSection_DxfFile.hxx>
#include <DxfSection_HandledObject.hxx>
#include <DxfSection_HSequenceOfObject.hxx>
#include <DxfSection_HSequenceOfXAppData.hxx>
#include <DxfSection_Model.hxx>
#include <DxfSection_Section.hxx>
#include <DxfSection_XAppData.hxx>
#include <DxfSection_XData.hxx>
#include <ShapeProcess_OperLibrary.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TDataStd_Name.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>
#include <XSAlgo.hxx>
#include <XSAlgo_AlgoContainer.hxx>
#include <XSControl_WorkSession.hxx>
#include <XSDRAW.hxx>
#include <XSDRAWDXF.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <UnitsMethods.hxx>

//=======================================================================
// function : dxfload
// purpose  :
//=======================================================================

static Standard_Integer dxfload(Draw_Interpretor & /*theCommands*/, Standard_Integer argc, const char **argv)
{
  if (argc < 2)
  {
    std::cout << "Give file name !" << std::endl;
    return 1;
  }
  TCollection_AsciiString resfile, resvar;

  XSDRAW::FileAndVar(argv[1], argv[2], "DXF", resfile, resvar);

  DxfControl_Reader reader(XSDRAW::Session(), Standard_True);
  reader.SetNorm("DXF");

  IFSelect_ReturnStatus stat = reader.ReadFile(argv[1]);

  std::cout << "Status from reading DXF file " << argv[1] << " : ";
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
  Handle(DxfSection_Model) aDxfModel = Handle(DxfSection_Model)::DownCast(reader.Model());
  XSAlgo::AlgoContainer()->PrepareForTransfer();
  // Set CasCade Length Units as target units. Initial units are taken
  // from the file if present or set to millimeters otherwise.
  Standard_Real aLengthUnitMM = UnitsMethods::GetCasCadeLengthUnit();
  aDxfModel->SetSystemLengthUnit(aLengthUnitMM);
  Standard_Integer nbroots = reader.NbRootsForTransfer();
  std::cout << "Nb Roots recognized for Transfer " << nbroots << std::endl;

  if (argc > 2)
  {
    if (!aDxfModel.IsNull())
    {
      Standard_Integer aVersion = aDxfModel->GetConfParameters().WriteVersion;
      Handle(DxfSection_Variable) aVerVar;
      switch (aVersion)
      {
      case 12:
        aVerVar = aDxfModel->SetVariable("$ACADVER", 1, "AC1009");
        break;
      case 13:
        aVerVar = aDxfModel->SetVariable("$ACADVER", 1, "AC1012");
        break;
      case 14:
        aVerVar = aDxfModel->SetVariable("$ACADVER", 1, "AC1014");
        break;
      case 15:
        aVerVar = aDxfModel->SetVariable("$ACADVER", 1, "AC1015");
        break;
      default:;
      }
      if (aVerVar.IsNull())
        return Standard_False;

      std::ofstream anOutStream(argv[2]);
      Handle(DxfFile_FileWriter) aFileWriter = new DxfFile_FileWriter(aDxfModel);
      DxfFile_RWDxfFile::Write(anOutStream, aFileWriter, aDxfModel->GetDxfFile());
    }
  }

  return 0;
}

//=======================================================================
// function : dxfread
// purpose  :
//=======================================================================

static Standard_Integer dxfread(Draw_Interpretor & /*theCommands*/, Standard_Integer argc, const char **argv)
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
  Standard_Boolean modfic = XSDRAW::FileAndVar(argv[1], argv[2], "DXF", fnom, rnom);
  if (modfic)
    std::cout << " File SAT to read : " << fnom << std::endl;
  else
    std::cout << " Model taken from the session : " << fnom << std::endl;
  std::cout << " -- Names of variables BREP-DRAW prefixed by : " << rnom << std::endl;

  DxfControl_Reader reader(XSDRAW::Session(), Standard_False);

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
  Handle(DxfSection_Model) aDxfModel = Handle(DxfSection_Model)::DownCast(reader.Model());
  XSAlgo::AlgoContainer()->PrepareForTransfer();
  // Set CasCade Length Units as target units. Initial units are taken
  // from the file if present or set to millimeters otherwise.
  Standard_Real aLengthUnitMM = UnitsMethods::GetCasCadeLengthUnit();
  aDxfModel->SetSystemLengthUnit(aLengthUnitMM);
  Standard_Boolean oneShape = Standard_False;
  if (argc > 3)
  {
    if (argv[3][0] == '*')
      oneShape = Standard_True;
    else
    {
      reader.ClearShapes();
      Handle(TColStd_HSequenceOfTransient) list = new TColStd_HSequenceOfTransient;
      for (Standard_Integer i = 3; i < argc; i++)
      {
        Standard_Integer num = Draw::Atoi(argv[i]);
        if (num <= 0)
          num = aDxfModel->NextNumberForLabel(argv[i]);
        if (num <= 0)
        {
          std::cout << "Error: No entity with id " << argv[i] << " (" << num << ") found in the model" << std::endl;
          continue;
        }
        // add entity to be translated to the list
        Handle(Standard_Transient) ent = aDxfModel->Value(num);
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
  Standard_Integer recordsinmodel = aDxfModel->NbEntities();
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
// function : dxfreadsimple
// purpose  :
//=======================================================================

static Standard_Integer dxfreadsimple(Draw_Interpretor & /*di*/, Standard_Integer argc, const char **argv)
{
  if (argc != 3)
  {
    std::cout << "ERROR in " << argv[0] << "Wrong Number of Arguments." << std::endl;
    std::cout << " Usage : " << argv[0] << " file shape" << std::endl;
    return 1;
  }
  DxfControl_Reader Reader;
  Standard_CString filename = argv[1];
  IFSelect_ReturnStatus readstat = Reader.ReadFile(filename);
  std::cout << "Status from reading DXF file " << filename << " : ";
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
  Handle(DxfSection_Model) aDxfModel = Handle(DxfSection_Model)::DownCast(Reader.Model());
  XSAlgo::AlgoContainer()->PrepareForTransfer();
  // Set CasCade Length Units as target units. Initial units are taken
  // from the file if present or set to millimeters otherwise.
  Standard_Real aLengthUnitMM = UnitsMethods::GetCasCadeLengthUnit();
  aDxfModel->SetSystemLengthUnit(aLengthUnitMM);
  Reader.TransferRoots();
  TopoDS_Shape shape = Reader.OneShape();
  DBRep::Set(argv[2], shape);
  std::cout << "Count of shapes produced : " << Reader.NbShapes() << std::endl;
  return 0;
}

//=======================================================================
// function : dxfwrite
// purpose  :
//=======================================================================

static Standard_Integer dxfwrite(Draw_Interpretor & /*theCommands*/, Standard_Integer argc, const char **argv)
{
  if (argc < 2)
  {
    std::cout << "Give shape name + file name" << std::endl;
    return 1;
  }
  DxfControl_Writer aWriter(XSDRAW::Session(), Standard_False);
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
// function : dxfwritesimple
// purpose  :
//=======================================================================

static Standard_Integer dxfwritesimple(Draw_Interpretor & /*di*/, Standard_Integer n, const char **a)
{
  if (n != 3)
  {
    std::cout << "ERROR in " << a[0] << "Wrong Number of Arguments." << std::endl;
    std::cout << " Usage : " << a[0] << " file shape" << std::endl;
    return 1;
  }
  DxfControl_Writer Writer;
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
// function : ReadDxf
// purpose  : Read DXF to DECAF document
//=======================================================================

static Standard_Integer ReadDxf(Draw_Interpretor &di, Standard_Integer argc, const char **argv)
{
  if (argc < 3)
  {
    std::cout << "Use: " << argv[0] << " Doc filename [mode]: read DFX file to a document" << std::endl;
    return 0;
  }

  Handle(DxfControl_Controller) ctl = Handle(DxfControl_Controller)::DownCast(XSDRAW::Controller());
  if (ctl.IsNull())
    XSDRAW::SetNorm("DXF");

  DXFCAFControl_Reader reader(XSDRAW::Session(), Standard_True);

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
    std::cout << " File DXF to read : " << fnom << std::endl;
  else
    std::cout << " Model taken from the session : " << fnom << std::endl;

  Handle(Draw_ProgressIndicator) aProgress = new Draw_ProgressIndicator(di);
  Message_ProgressScope aRootScope(aProgress->Start(), "DXF import", modfic ? 2 : 1);

  IFSelect_ReturnStatus readstat = IFSelect_RetVoid;
  if (modfic)
  {
    Message_ProgressScope aReadScope(aRootScope.Next(), "File reading", 1);
    aReadScope.Show();
    readstat = reader.ReadFile(fnom.ToCString());
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
    //     di << "Document saved with name " << argv[1];
  }
  reader.ClearShapes();
  reader.NbRootsForTransfer(); // to initialize list of roots
  if (!reader.Transfer(doc, aRootScope.Next()))
  {
    std::cout << "Cannot read any relevant data from the SAT file" << std::endl;
    return 1;
  }

  di << "Document saved with name " << argv[1];

  return 0;
}

//=======================================================================
// function : WriteDxf
// purpose  : Write DECAF document to DXF
//=======================================================================

static Standard_Integer WriteDxf(Draw_Interpretor & /*di*/, Standard_Integer argc, const char **argv)
{
  if (argc < 3)
  {
    std::cout << "Use: " << argv[0] << " Doc filename [mode]: write document to DFX file" << std::endl;
    return 0;
  }

  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if (Doc.IsNull())
  {
    std::cout << argv[1] << " is not a document" << std::endl;
    return 1;
  }

  XSDRAW::SetNorm("DXF");

  DXFCAFControl_Writer writer(XSDRAW::Session(), Standard_True);
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

  std::cout << "Writing DXF model to file " << argv[2] << std::endl;
  if (writer.WriteFile(argv[2]))
    std::cout << " Write OK" << std::endl;
  else
    std::cout << " Write failed" << std::endl;

  return 0;
}

//=======================================================================
// function : dxf_extended_data_read
// purpose  :
//=======================================================================

static Standard_Integer dxf_extended_data_read(Draw_Interpretor &di, Standard_Integer argc, const char **argv)
{
  if (argc < 2)
  {
    std::cout << "Use:" << std::endl;
    std::cout << argv[0] << " <filename> [<variable>]" << std::endl;
    return 1;
  }

  TCollection_AsciiString fnom, rnom;
  Standard_Boolean modfic = XSDRAW::FileAndVar(argv[1], argv[2], "DXF", fnom, rnom);
  if (modfic)
    di << " File SAT to read : " << fnom << "\n";
  else
    di << " Model taken from the session : " << fnom << "\n";
  di << " -- Names of variables BREP-DRAW prefixed by : " << rnom << "\n";

  DxfControl_Reader reader(XSDRAW::Session(), Standard_False);

  if (modfic)
  {
    IFSelect_ReturnStatus stat = reader.ReadFile(argv[1]);
    di << "Status from reading DXF-SAT file " << argv[1] << " : ";
    switch (stat)
    {
    case IFSelect_RetVoid:
    {
      di << "empty file" << "\n";
      return 1;
    }
    case IFSelect_RetDone:
    {
      di << "file read" << "\n";
      break;
    }
    case IFSelect_RetError:
    {
      di << "file not found" << "\n";
      return 1;
    }
    case IFSelect_RetFail:
    {
      di << "error during read" << "\n";
      return 1;
    }
    default:
    {
      di << "failure" << "\n";
      return 1;
    }
    }
  }

  Handle(DxfSection_Model) aDxfModel = Handle(DxfSection_Model)::DownCast(reader.Model());
  // read extended data
  Standard_Integer i, j, k;
  for (i = 1; i <= aDxfModel->GetDxfFile()->GetSections()->Length(); i++)
  {
    Handle(DxfSection_Section) obj1 = Handle(DxfSection_Section)::DownCast(aDxfModel->GetDxfFile()->GetSections()->Value(i));
    if (!obj1->GetObjects().IsNull())
      for (j = 1; j <= obj1->GetObjects()->Length(); j++)
      {
        Handle(DxfSection_HandledObject) obj2 = Handle(DxfSection_HandledObject)::DownCast(obj1->GetObjects()->Value(j));
        if (!obj2.IsNull())
          if (!obj2->GetXAppData().IsNull())
            for (k = 1; k <= obj2->GetXAppData()->Length(); k++)
            {
              Handle(DxfSection_XAppData) XAppData = obj2->GetXAppData()->Value(k);
              di << XAppData->GetXAppName()->ToCString() << "\n"; // print out AppNames
              // XAppData->GetXData() ... - to print out Xdata
            }
      }
  }

  return 0;
}

//=======================================================================
// function : Init
// purpose  :
//=======================================================================

void XSDRAWDXF::Init()
{
  DxfControl_Controller::Init();
}

//=======================================================================
// function : InitCommands
// purpose  :
//=======================================================================

void XSDRAWDXF::InitCommands(Draw_Interpretor &theCommands)
{
  DxfControl_Controller::Init();

  const char *g = "XSTEP";

  theCommands.Add("dxfload", "dxfload inFileName [outFileName]", __FILE__, dxfload, g);
  theCommands.Add("dxfread", "dxfread filename [varname [entity]]", __FILE__, dxfread, g);
  theCommands.Add("dxfreadsimple", "dxfreadsimple filename shape", __FILE__, dxfreadsimple, g);
  theCommands.Add("dxfwrite", "dxfwrite varname filename", __FILE__, dxfwrite, g);
  theCommands.Add("dxfwritesimple", "dxfwritesimple filename shape", __FILE__, dxfwritesimple, g);

  theCommands.Add("ReadDxf", "Doc filename: Read DXF file to DECAF document", __FILE__, ReadDxf, "DECAF");
  theCommands.Add("WriteDxf", "Doc filename: Read DXF file to DECAF document", __FILE__, WriteDxf, "DECAF");

  theCommands.Add("dxf_extended_data_read", "dxf_extended_data_read filename [varname]", __FILE__, dxf_extended_data_read, g);
}

//==============================================================================
// XSDRAWDXF::Factory
//==============================================================================
void XSDRAWDXF::Factory(Draw_Interpretor &theDI)
{
  XSDRAW::LoadSession();

  XSDRAWDXF::InitCommands(theDI);

  ShapeProcess_OperLibrary::Init();

#ifdef DEB
  std::cout << "Draw Plugin : All TKXSDRAWDXF commands are loaded" << std::endl;
#endif
}

// Declare entry point PLUGINFACTORY
DPLUGIN(XSDRAWDXF)
