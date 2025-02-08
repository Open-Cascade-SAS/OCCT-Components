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

#include <CRDRAW.hxx>
#include <CRDRAW_ShapeConvert.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_PluginMacro.hxx>

//=======================================================================
// function : Init
// purpose  :
//=======================================================================

void CRDRAW::Init(Draw_Interpretor& theCommands)
{
  CRDRAW_ShapeConvert::InitCommands(theCommands);
}

//=======================================================================
// function : GroupName
// purpose  :
//=======================================================================

Standard_CString CRDRAW::GroupName()
{
  return "Canonical Recognition";
}

//==============================================================================
// CRDRAW::Factory
//==============================================================================

void CRDRAW::Factory(Draw_Interpretor& theDI)
{
  CRDRAW::Init(theDI);

#ifdef DEB
  std::cout << "Draw Plugin : All TKCRDRAW commands are loaded\n";
#endif
}

// Declare entry point PLUGINFACTORY
DPLUGIN(CRDRAW)
