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

#ifndef _ColDetectionTest_HeaderFile
#define _ColDetectionTest_HeaderFile

#include <Standard_Macro.hxx>

class Draw_Interpretor;

//! Provides commands for Collision Detection <br>
class ColDetectionTest
{
public:
  //! Loads all Draw commands of TKColTest. <br>
  Standard_EXPORT static void Commands(Draw_Interpretor& theDI);
  //! Calls Coomands method. Used for plugin. <br>
  Standard_EXPORT static void Factory(Draw_Interpretor& theDI);
};

#endif
