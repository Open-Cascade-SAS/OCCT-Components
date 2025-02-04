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
#include <Draw_ProgressIndicator.hxx>
#include <DEIFC_ConfigurationNode.hxx>
#include <IFCCAFControl_Controller.hxx>
#include <IFCCAFControl_Model.hxx>
#include <IFCCAFControl_Reader.hxx>
#include <Interface_Static.hxx>
#include <ShapeProcess_OperLibrary.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TDataStd_Name.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XSControl_WorkSession.hxx>
#include <XSDRAW.hxx>
#include <XSDRAWIFC.hxx>

//=======================================================================
// function : parseOnOff
// purpose  : parses the on/off or 0/1 value and returns true if the correct
//           value for the parameter was found, false otherwise
//=======================================================================
static Standard_Boolean parseOnOff(const char *theParam,
                                   Standard_Boolean &theEnabled,
                                   Standard_Integer &theIter)
{
  Standard_Boolean isFound = Standard_False;
  TCollection_AsciiString aVal(theParam);
  aVal.LowerCase();
  if (aVal == "1" || aVal == "on")
  {
    theEnabled = Standard_True;
    ++theIter;
    isFound = Standard_True;
  }
  else if (aVal == "0" || aVal == "off")
  {
    theEnabled = Standard_False;
    ++theIter;
    isFound = Standard_True;
  }
  return isFound;
}

//=======================================================================
// function : ReadIfc
// purpose  : Read IFC to DECAF document
//=======================================================================

static Standard_Integer ReadIfc(Draw_Interpretor &di, Standard_Integer argc, const char **argv)
{
  TCollection_AsciiString aDocName, aFileName;
  Standard_Boolean toDiscardProjTrsf = Standard_False;
  Standard_Boolean toExpandProductRepr = Standard_True;
  Standard_Boolean toUseTypeAsName = Standard_False;
  for (Standard_Integer anArgIter = 1; anArgIter < argc; ++anArgIter)
  {
    TCollection_AsciiString anArg(argv[anArgIter]);
    anArg.LowerCase();
    if (anArg == "-discardprojecttrsf" || anArg == "-discardprojecttransformation" || anArg == "-discardprojectlocation" || anArg == "-discardroottrsf" || anArg == "-discardroottransformation" || anArg == "-discardrootlocation")
    {
      toDiscardProjTrsf = Standard_True;
      if (anArgIter + 1 < argc)
      {
        parseOnOff(argv[anArgIter + 1], toDiscardProjTrsf, anArgIter);
      }
    }
    else if (anArg == "-expandprodrepr" || anArg == "-expandproductrepr" || anArg == "-expandproductrepresentation")
    {
      if (anArgIter + 1 < argc)
      {
        parseOnOff(argv[anArgIter + 1], toExpandProductRepr, anArgIter);
      }
    }
    else if (anArg == "-usetypeasname")
    {
      if (anArgIter + 1 < argc)
      {
        parseOnOff(argv[anArgIter + 1], toUseTypeAsName, anArgIter);
      }
    }
    else if (aDocName.IsEmpty())
    {
      aDocName = argv[anArgIter];
    }
    else if (aFileName.IsEmpty())
    {
      aFileName = argv[anArgIter];
    }
    else
    {
      std::cout << "Syntax error at '" << argv[anArgIter] << "'\n";
      return 1;
    }
  }
  if (aDocName.IsEmpty() || aFileName.IsEmpty())
  {
    std::cout << "Syntax error: wrong number of arguments.\n";
    return 1;
  }

  XSDRAW::LoadSession();
  Handle(IFCCAFControl_Controller) ctl = Handle(IFCCAFControl_Controller)::DownCast(XSDRAW::Controller());
  if (ctl.IsNull())
    XSDRAW::SetNorm("IFC");
  IFCCAFControl_Reader reader(XSDRAW::Session(), Standard_True);
  reader.SetDiscardProjectTransformation(toDiscardProjTrsf);
  reader.SetExpandIfcProductRepresentation(toExpandProductRepr);
  reader.SetUseTypeAsName(toUseTypeAsName);

  Handle(Draw_ProgressIndicator) aProgress = new Draw_ProgressIndicator(di, 1);

  Handle(TDocStd_Document) aDoc;
  Standard_CString aDocNameStr = aDocName.ToCString();

  // Disable auto naming
  Standard_Boolean autoNaming = XCAFDoc_ShapeTool::AutoNaming();
  XCAFDoc_ShapeTool::SetAutoNaming(Standard_False);

  if (!DDocStd::GetDocument(aDocNameStr, aDoc, Standard_False))
  {
    Handle(TDocStd_Application) A = DDocStd::GetApplication();
    A->NewDocument("BinXCAF", aDoc);
    TDataStd_Name::Set(aDoc->GetData()->Root(), argv[1]);
    Handle(DDocStd_DrawDocument) DD = new DDocStd_DrawDocument(aDoc);
    Draw::Set(aDocName.ToCString(), DD);
  }

  if (!reader.Perform(aFileName, aDoc, aProgress->Start()))
  {
    di << "Reading has failed";
  }
  else
  {
    di << "Document saved with name " << aDocName;
  }

  // Restore the auto naming option
  XCAFDoc_ShapeTool::SetAutoNaming(autoNaming);

  return 0;
}

//=======================================================================
// function : IfcIdFromShape
// purpose  : Return Id of IFC entity for given shape
//=======================================================================

static Standard_Integer IfcIdFromShape(Draw_Interpretor &di, Standard_Integer argc, const char **argv)
{
  if (argc < 2)
  {
    std::cout << "Use: " << argv[0] << " shape : Return Id of IFC entity for given shape" << std::endl;
    return 0;
  }

  TopoDS_Shape aS = DBRep::Get(argv[1]);
  if (aS.IsNull())
  {
    di << " null shape are not allowed \n";
    return 0;
  }

  Handle(IFCCAFControl_Model) aModel = Handle(IFCCAFControl_Model)::DownCast(XSDRAW::Session()->Model());
  Standard_Integer Id = aModel->IdByShape(aS);
  if (Id > 0)
  {
    di << "this shape is translated from entity " << Id << "\n";
  }
  else
  {
    di << "no info about entity for this shape\n";
  }

  return 0;
}

//=======================================================================
// function : Init
// purpose  :
//=======================================================================

void XSDRAWIFC::Init()
{
  IFCCAFControl_Controller::Init();
}

//=======================================================================
// function : InitCommands
// purpose  :
//=======================================================================

void XSDRAWIFC::InitCommands(Draw_Interpretor &theCommands)
{
  IFCCAFControl_Controller::Init();

  theCommands.Add("ReadIfc", "Read IFC file to DECAF document"
                             "\nUsage: ReadIfc Doc filename [-discardProjectTrsf {0|1}] [-expandProdRepr {0|1}] [-useTypeAsName {0|1}]"
                             "\nWhere:"
                             "\n\t -discardProjectTrsf - allows ignoring the project (root) transformation"
                             "\n\t                       (false by default)"
                             "\n\t -expandProdRepr     - allows enabling/disabling the expansion of"
                             "\n\t                       IfcProductRepresentation items (enabled by default)"
                             "\n\t -useTypeAsName      - allows using the type of Ifc items as their names"
                             "\n\t                       (false by default)",
                  __FILE__, ReadIfc, "DECAF");
  theCommands.Add("IfcIdFromShape", "shape : Return Id of IFC entity for given shape", __FILE__, IfcIdFromShape, "DECAF");
}

//==============================================================================
// XSDRAWIFC::Factory
//==============================================================================
void XSDRAWIFC::Factory(Draw_Interpretor &theDI)
{
  // XSDRAW::LoadSession();

  XSDRAWIFC::InitCommands(theDI);

  // ShapeProcess_OperLibrary::Init();

#ifdef DEB
  std::cout << "Draw Plugin : All TKXSDRAWIFC commands are loaded" << std::endl;
#endif
}

// Declare entry point PLUGINFACTORY
DPLUGIN(XSDRAWIFC)
