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

#include <Draw_Interpretor.hxx>
#include <Draw_PluginMacro.hxx>

//=======================================================================
//function : Factory
//purpose  :
//=======================================================================
void CAMTest::Factory(Draw_Interpretor& theDI)
{
  static Standard_Boolean done = Standard_False;
  if (!done)
  {
    done = Standard_True;
    CAMTest::ConvCommands(theDI);
    CAMTest::PolyCommands(theDI);
    CAMTest::GeodesicCommands(theDI);
  }

#ifdef DEB
  theDI << "Draw Plugin : CAMTest commands are loaded" << "\n";
#endif
}

// Declare entry point PLUGINFACTORY
DPLUGIN(CAMTest)
