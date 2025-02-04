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

#include <XSDRAWJTXT.hxx>

#include <DEJT_ConfigurationNode.hxx>
#include <Draw_PluginMacro.hxx>
#include <JtXt_BRepProvider.hxx>
#include <XSDRAWJT.hxx>

//=======================================================================
// function : XSDRAWJTXT::Factory
// purpose  :
//=======================================================================
void XSDRAWJTXT::Factory(Draw_Interpretor &)
{
  XSDRAWJT::SetXTBRepProvider(new JtXt_BRepProvider());
}

DPLUGIN(XSDRAWJTXT)
