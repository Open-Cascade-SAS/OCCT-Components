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

#ifndef _CRDRAW_HeaderFile
#define _CRDRAW_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Draw_Interpretor.hxx>
#include <Standard_CString.hxx>
class CRDRAW_ShapeConvert;

//! Provides DRAW interface to the functionalities of CR
class CRDRAW
{
public:
  DEFINE_STANDARD_ALLOC

  //! Test for Canonical recognition
  //! Loads commands defined in CRDRAW
  Standard_EXPORT static void Init(Draw_Interpretor& theCommands);

  //! Loads all Draw commands of TKCRDRAW. Used for plugin.
  Standard_EXPORT static void Factory(Draw_Interpretor& theDI);

protected:
private:
  //! Returns the name of the DRAW group accumulating the
  //! commands from the classes prefixed with Shape*.
  //! Returns "Canonical Recognition".
  Standard_EXPORT static Standard_CString GroupName();

  friend class CRDRAW_ShapeConvert;
};

#endif // _CRDRAW_HeaderFile
