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

#ifndef _XSDRAWJTXT_HeaderFile
#define _XSDRAWJTXT_HeaderFile

#include <Standard.hxx>
#include <Standard_Macro.hxx>

class Draw_Interpretor;

//! Plugin XSDRAWJTXT - intended to enable XT loader (TKDEXT) within ReadJt command of XSDRAWJT plugin.
class XSDRAWJTXT
{
public:
  DEFINE_STANDARD_ALLOC

  //! Loads Draw commands of this plugin.
  Standard_EXPORT static void Factory(Draw_Interpretor &theDI);
};

#endif // _XSDRAWJTXT_HeaderFile
