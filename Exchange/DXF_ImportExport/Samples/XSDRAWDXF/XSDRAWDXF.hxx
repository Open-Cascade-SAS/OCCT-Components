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

#ifndef _XSDRAWDXF_HeaderFile
#define _XSDRAWDXF_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Draw_Interpretor.hxx>

//! XSDRAW for DXF interface
class XSDRAWDXF
{
public:
  DEFINE_STANDARD_ALLOC

  //! Inits DXF interface for Draw
  Standard_EXPORT static void Init();

  //! Loads DXF interface commands into the Draw interpretor
  Standard_EXPORT static void InitCommands(Draw_Interpretor &theCommands);

  //! Loads all Draw commands of TKXSDRAWDXF. Used for plugin.
  Standard_EXPORT static void Factory(Draw_Interpretor &theDI);

protected:
private:
};

#endif // _XSDRAWDXF_HeaderFile
