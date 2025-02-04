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

#include <XSDRAWUSD.hxx>

#include <DDocStd.hxx>
#include <DDocStd_DrawDocument.hxx>
#include <DE_Wrapper.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_PluginMacro.hxx>
#include <DEUSD_ConfigurationNode.hxx>
#include <XSDRAWUSD_ExportTest.hxx>
#include <XSDRAWUSD_ImportTest.hxx>

//=======================================================================
// function : Init
// purpose  :
//=======================================================================
void XSDRAWUSD::Init()
{
}

//=======================================================================
// function : InitCommands
// purpose  :
//=======================================================================
void XSDRAWUSD::InitCommands(Draw_Interpretor &theCommands)
{
  XSDRAWUSD_ExportTest::InitCommands(theCommands);
  XSDRAWUSD_ImportTest::InitCommands(theCommands);
}

//==============================================================================
// XSDRAWUSD::Factory
//==============================================================================
void XSDRAWUSD::Factory(Draw_Interpretor &theDI)
{
  XSDRAWUSD::InitCommands(theDI);

  DE_Wrapper::GlobalWrapper()->Bind(new DEUSD_ConfigurationNode());

#ifdef DEB
  std::cout << "Draw Plugin : All TKXSDRAWUSD commands are loaded" << std::endl;
#endif
}

// Declare entry point PLUGINFACTORY
DPLUGIN(XSDRAWUSD)
