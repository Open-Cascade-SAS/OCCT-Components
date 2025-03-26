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

using System;
using System.Threading;
using System.Diagnostics;
using System.Globalization;
using OCC;
using OCC.Standard;
using OCC.gp;
using OCC.OSD;
using OCC.Geom;
using OCC.TColStd;
using OCC.TColgp;
using OCC.TopoDS;
using OCC.TCollection;
using OCC.Message;
using OCC.Precision;
using OCC.BRepBuilderAPI;
using OCC.BRepPrimAPI;
using OCC.TopExp;
using OCC.BRepTools;
using OCC.BRep;
using OCC.STEPControl;
using OCC.IFSelect;

namespace CSharpTest
{
  /// <summary>
  /// Test cases for OCC wrapping to CSharp
  /// </summary>
  class Test
  {
    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [STAThread]
    static void Main(string[] args)
    {
      TestIntRef();
      TestEnumOut();
      TestVersion();
      TestDownCast();
      TestReference();
      TestNullHandle();
      TestXYZOperators();
      TestDispose();
      TestMMGR();
      TestCreateCircle();
      TestNestedIterator();
      TestErrorHandling();
      TestPrinter();
      if (Environment.OSVersion.Platform == PlatformID.Win32NT)
      {
        TestUnicode();
        TestStringMemory();
      }

      // this test should be last one -- if it fails, the system will go down
      TestGC();

      if (myNbErr > 0) 
        Console.WriteLine ( "*** Total number of tests FAILED: " + myNbErr );
      else
        Console.WriteLine ( "*** ALL TESTS COMPLETED" );
      Thread.Sleep (3000);
    }

    /// <summary>
    /// Report error if condition is true
    /// </summary>
    private static bool CheckError ( bool error, string msg )
    {
      if ( error ) 
      {
        myNbErr++;
        Console.WriteLine ( "*** TEST FAILED: " + msg );
      }
      else 
        Console.WriteLine ( "*** TEST OK: " + msg );
      return error;
    }

    /// <summary>
    /// Test memory deallocation by garbage collector.
    /// </summary>
    private static void TestGC ()
    {
      Console.WriteLine("Final test of garbage collector: if it fails, the system may go down!");

      long VMSize0 = Process.GetCurrentProcess().VirtualMemorySize64 / (1024 * 1024);
//      Console.WriteLine( "Allocating arrays for garbage collector to free; watch memory!" );
      Console.WriteLine( "Virtual memory occupied at start: " + VMSize0 + " Mb" );

      // Run multiple allocation - deallocation cycles, each time calling GC.Collect()
      // explicitly; the number of calls is selected so that in case of memory leak
      // it dies due to overallocation (~100 Gb of VRAM)
      for (int k=1; k < 100; k++)
      {
        // cycle allocating ~1 Gb (not more, otherwise it will die prematurely on 32-bit system)
        for (int i = 1; i < 1000; i++)
        {
//          Console.WriteLine( "Cycle " + i );
          TColStd_HArray1OfReal anArr = new TColStd_HArray1OfReal(1, 100000); // ~1 Mb
          anArr.SetValue(1, 10);
        }
//        long VMSize1 = Process.GetCurrentProcess().VirtualMemorySize64 / (1024 * 1024);
//        Thread.Sleep (5000);
//        Console.WriteLine("Virtual memory occupied: " + VMSize1 + " Mb; now calling GC.Collect() :-)");
        System.GC.Collect();
        System.GC.WaitForPendingFinalizers();
      }
      Thread.Sleep (2000); // needed for system to release virtual memory
      long VMSize2 = Process.GetCurrentProcess().VirtualMemorySize64 / (1024 * 1024);
      Console.WriteLine( "Virtual memory occupied after playing with ~100 Gb of memory: " + VMSize2 + " Mb" );
      // we expect that at least 300 Mb of memory shall be occupied and then released by GC
      CheckError ( VMSize2 - VMSize0 > 10000, // we are Ok in any case if arrived here
                   "TestGC: memory deallocation by garbage collector" );
//      Thread.Sleep (5000);
    }

    /// <summary>
    /// Test OCCT memory manager for concurrent allocation / deallocation (by garbage collector)
    /// </summary>
    private static void TestMMGR ()
    {
      bool ok = true;
      try 
      {
        for ( int i = 1; i < 100000; i++ ) 
        {
          TColStd_HArray1OfReal anArr = new TColStd_HArray1OfReal ( 1, 100 );
          anArr.SetValue (1, 10);
        }
      }
      catch (Exception e) 
      {
        Console.WriteLine ( "TestMMGR: Exception " + e.Message );
        ok = false;
      }
      CheckError ( !ok, "TestMMGR: OCCT memory manager in concurrent environment" );
    }

    /// <summary>
    /// Test forced memory deallocation with automatic scope ("using" statement).
    /// </summary>
    private static void TestDispose ()
    {
//      Console.WriteLine( "Allocating arrays with local scope; watch memory!" );
      long VMSize0 = Process.GetCurrentProcess().VirtualMemorySize64 / ( 1024 * 1024 );
      for ( int i = 1; i < 20000; i++ ) 
      {
//        Console.WriteLine( "Cycle " + i );
        using ( TColStd_HArray1OfReal anArr = new TColStd_HArray1OfReal ( 1, 100000 ) )
        {
          anArr.SetValue (i, 10);
        }
      }
      long VMSize1 = Process.GetCurrentProcess().VirtualMemorySize64 / ( 1024 * 1024 );
//      Thread.Sleep (1000);
//      Console.WriteLine( "Virtual memory occupied: " + VMSize0 + " Mb; now calling GC.Collect() :-)" );
      System.GC.Collect();
      System.GC.WaitForPendingFinalizers();
//      Thread.Sleep (5000); // give chance to the system to release virtual memory
      long VMSize2 = Process.GetCurrentProcess().VirtualMemorySize64 / ( 1024 * 1024 );
//      Console.WriteLine( "Virtual memory occupied after garbage collector: " + VMSize1 + " Mb" );
      // we do not expect any noticeable difference in memory 
      CheckError ( VMSize1 - VMSize0 > 2 || VMSize1 - VMSize2 > 2, 
                   "TestDispose: automatic deallocation of memory" );
//      Thread.Sleep (2000);
    }

    /// <summary>
    /// Test access to OCC objects by reference
    /// </summary>
    private static void TestReference ()
    {
//      Console.WriteLine( "Testing access by reference" );
      gp_Pnt p_ref;
      {
        TColgp_Array1OfPnt anArr = new TColgp_Array1OfPnt ( 1, 1 );
        p_ref = anArr.ChangeValue(1).AsReference();
        p_ref.SetCoord( 1, 2, 3 );
//        Console.WriteLine( "Modified point: {" + anArr.Value(1).X() + ", " + anArr.Value(1).Y() + ", " + anArr.Value(1).Z() + "}" );
        gp_Pnt p_check = anArr.Value (1);
        CheckError (p_check.X() != 1 || p_check.Y() != 2 || p_check.Z() != 3,
                    "TestReference: accessing gp_Pnt in array by reference");

        double x=0, y=0, z=0;
        p_check.Coord ( ref x, ref y, ref z );
//        Console.WriteLine( "Coordinates extracted by ref: {" + x + ", " + y + ", " + z + "}" );
        CheckError ( x != 1 || y != 2 || z != 3, "TestReference: coordinates returned by reference" );
      }
      System.GC.Collect();
      System.GC.WaitForPendingFinalizers();
      CheckError ( p_ref == null || p_ref.Distance (new gp_Pnt(1,2,3)) > Precision.Confusion(), 
                   "TestReference: get reference to element of array (gp_Pnt)" );

      // check case when reference to handle is returned in C++
      Standard_Transient anItem;
      {
        TColStd_Array1OfTransient anArrT = new TColStd_Array1OfTransient (1, 1);
        Standard_Transient aPnt = new Geom_CartesianPoint (p_ref);
        anArrT.SetValue (1, aPnt);
        anItem = anArrT.ChangeValue(1);
      }
      System.GC.Collect();
      System.GC.WaitForPendingFinalizers();
      CheckError ( anItem.IsNull() || ! anItem.IsKind(Geom_CartesianPoint.TypeOf()) ||
                   Geom_CartesianPoint.DownCast (anItem).Pnt().Distance (p_ref) > Precision.Confusion(), 
                   "TestReference: get element of array (Handle)" );

//      TColStd_Array1OfReal anArrR = new TColStd_Array1OfReal ( 1, 1, 0 );
//      anArrR.ChangeValue(1) = 5;
//      Console.WriteLine( "Modified value: " + anArrR.Value(1) );

//      Thread.Sleep (2000);
    }

    /// <summary>
    /// Test downcasting
    /// </summary>
    private static void TestDownCast ()
    {
//      Console.WriteLine( "Testing downcasting" );
      gp_Pnt p = new gp_Pnt ( 1, 2, 3 );
      gp_Ax2 ax = new gp_Ax2 ( p, new gp_Dir (1,0,0) );
      Geom_Curve c = new Geom_Circle ( ax, 10 );
      CheckError ( ! (c is Geom_Conic), "TestDownCast: using 'is' on derived type in C#" );

      TColStd_Array1OfTransient anArr = new TColStd_Array1OfTransient ( 1, 1 );
      anArr.SetValue ( 1, c );
      if ( anArr.Value(1) is Geom_Conic ) 
        Console.WriteLine( "ERROR: Circle in array IS a Curve! Very strange!" );
      else 
        Console.WriteLine( "OK: Circle in array of transients IS NOT a Curve in C#" );

      CheckError ( ! anArr.Value(1).IsKind(Geom_Circle.TypeOf()), 
                   "TestDownCast: using IsKind() on C++ derived type" );

      Thread.Sleep (2000);
    }

    /// <summary>
    /// Test passing enumeration as output argument to a function
    /// </summary>
    private static void TestEnumOut ()
    {
//      Console.WriteLine( "Testing enumeration out" );

      OSD_SingleProtection User = OSD_SingleProtection.OSD_None;
      OSD_SingleProtection Sys  = OSD_SingleProtection.OSD_None;
      OSD_SingleProtection Group= OSD_SingleProtection.OSD_None;
      OSD_SingleProtection World= OSD_SingleProtection.OSD_None;
      OSD_Protection aP = new OSD_Protection();
      aP.Values( ref User, ref Sys, ref Group, ref World );
//      Console.WriteLine( "Default protection values: user = " + User + ", system = " + Sys +
//                         ", group = " + Group + ", world = " + World );
      if (Environment.OSVersion.Platform == PlatformID.Win32NT)
      {
        CheckError ( User != OSD_SingleProtection.OSD_RWXD || 
                     Sys  != OSD_SingleProtection.OSD_RWXD || 
                     Group!= OSD_SingleProtection.OSD_RX || 
                     World!= OSD_SingleProtection.OSD_RX, 
                     "TestEnumOut: passing enumeration as output argument" );
      }
      else
      {
        CheckError ( User != OSD_SingleProtection.OSD_R || 
                     Sys  != OSD_SingleProtection.OSD_RWD || 
                     Group!= OSD_SingleProtection.OSD_R || 
                     World!= OSD_SingleProtection.OSD_R, 
                     "TestEnumOut: passing enumeration as output argument" );
      }

//      Thread.Sleep (2000);
    }

    /// <summary>
    /// Test returning non-const reference to integer or enum
    /// (incompatibility: reference mapped as by value)
    /// </summary>
    private static void TestIntRef ()
    {
      //      Console.WriteLine( "Testing enumeration out" );
      TColStd_ListOfInteger aList = new TColStd_ListOfInteger();
      aList.Append(1);
      aList.Append(2);
//      aList.First() = 5; // has no effect

      //      Console.WriteLine( "Default protection values: user = " + User + ", system = " + Sys +
      //                         ", group = " + Group + ", world = " + World );
      CheckError ( aList.First() != 1 || aList.Last() != 2, 
        "TestIntRef: returning integer by reference - mapped to value" );

      //      Thread.Sleep (2000);
    }

    /// <summary>
    /// Test null handles
    /// </summary>
    private static void TestNullHandle ()
    {
//      Console.WriteLine( "Testing null handles" );

      TColStd_Array1OfTransient anArr = new TColStd_Array1OfTransient ( 1, 2 );
      gp_Pnt p = new gp_Pnt ( 1, 2, 3 );
      gp_Ax2 ax = new gp_Ax2 ( p, new gp_Dir (1,0,0) );
      Geom_Curve c = new Geom_Circle ( ax, 10 );
      anArr.SetValue ( 1, c );

      Standard_Transient c1 = anArr.ChangeValue ( 1 );
      Standard_Transient c1x = anArr.ChangeValue ( 1 );
      Standard_Transient c2 = anArr.ChangeValue ( 2 );
//      if ( c1 == null || c1.IsNull() ) 
//        Console.WriteLine( "ERROR: First element is null!" );
      CheckError ( c1 == null || c1.IsNull(), "TestNullHandle: non-null value of array item" );

      if ( c2 != null ) 
        Console.WriteLine( "OK: Second element might be expected to be null, but C# wrapper is not a null object" );
      else 
        Console.WriteLine( "Wow: second element is Null in C#!? Magic!" );

//      if ( c2.IsNull() ) 
//        Console.WriteLine( "Well, it is Null actually..." );
//      else 
//        Console.WriteLine( "ERROR: IsNull() is expected to return True!!" );
      CheckError ( ! c2.IsNull(), "TestNullHandle: null value of array item" );

//      Thread.Sleep (2000);
    }

    /// <summary>
    /// Test arithmetic operators
    /// </summary>
    private static void TestXYZOperators ()
    {
//      Console.WriteLine( "Testing arithmetic operators" );

      gp_XYZ x = new gp_XYZ ( 1, 0, 0 );
      gp_XYZ y = new gp_XYZ ( 0, 1, 0 );
      gp_XYZ z = x ^ y;
//      Console.WriteLine( "Z = {" + z.X() + ", " + z.Y() + ", " + z.Z() + "}" );
      CheckError ( z.X() != 0 || z.Y() != 0 || z.Z() != 1, "TestXYZOperators: cross-product of gp_XYZ vectors" );
//      Thread.Sleep (2000);
    }

    /// <summary>
    /// Test OCC version number
    /// </summary>
    private static void TestVersion ()
    {
//      Console.WriteLine( "Testing OCC version" );
//      Console.WriteLine( "OCC " + Standard_Version.Major() + "." + Standard_Version.Minor() + 
//                         "." + Standard_Version.Maintenance() + " = " + Standard_Version.Number() +
//                         " = " + Standard_Version.String() );
        CheckError(Standard_Version.Number() < 6.1 ||
                   Standard_Version.Number() != double.Parse(Standard_Version.String(), CultureInfo.InvariantCulture),
                   "TestVersion: OCC version number (" + Standard_Version.Number().ToString(CultureInfo.InvariantCulture) + ")");
//      Thread.Sleep (2000);
    }

    /// <summary>
    /// Test creation of circle
    /// </summary>
    private static void TestCreateCircle()
    {
      double radius = 10;
      gp_Pnt p1 = new gp_Pnt (0, 0, 0);
      gp_Dir cNorm = new gp_Dir (0, 0, 1);
      gp_Ax2 coordSystem = new gp_Ax2(p1, cNorm);

      Geom_Circle circle = new Geom_Circle(coordSystem, radius);

      TopoDS_Edge edge = (new BRepBuilderAPI_MakeEdge(circle)).Edge();

      System.GC.Collect();
      System.GC.WaitForPendingFinalizers();

      CheckError ( edge.IsNull() || TopExp.FirstVertex(edge).IsNull(), 
                   "TestCreateCircle: creation of circular edge" );
    }

    /// <summary>
    /// Test work with instantiations of template NCollection classes
    /// </summary>
    private static void TestNestedIterator()
    {
      //      Console.WriteLine( "Testing collections" );

      // create list of integers
      TColStd_ListOfInteger aList = new TColStd_ListOfInteger();
      aList.Append(1);
      aList.Append(2);
      aList.Append(3);
      CheckError(aList.Size() != 3,
        "TestNestedIterator: initialization of TColStd_ListOfInteger");

      // use iterator to remove element in the middle
      TColStd_ListOfInteger_Iterator anIt = new TColStd_ListOfInteger_Iterator(aList);
      for (; anIt.More(); anIt.Next())
      {
        if (anIt.Value() == 2)
          aList.Remove (anIt);
      }
      CheckError(aList.First() != 1 || aList.Last() != 3 || aList.Size() != 2,
        "TestNestedIterator: removal of element from the list using iterator");
    }

    /// <summary>
    /// Class inheriting Message_Printer and redefining its method
    /// </summary>
    class Printer : Message_Printer
    {
      protected override void send(TCollection_AsciiString theString, Message_Gravity theGravity)
      {
        myMessage += theString;
      }

      public string myMessage;
    };

    /// <summary>
    /// Test triggers VectorWithNullMagnitude Exception and catches it
    /// </summary>
    private static void TestErrorHandling()
    {
      gp_Vec2d aVec1 = new gp_Vec2d(0, 1);
      gp_Vec2d aVec2 = new gp_Vec2d(0, 0);
      try {
        aVec1.Angle(aVec2);
      } catch (gp_VectorWithNullMagnitude ) {
        CheckError(false, "TestErrorHandling: Exception was caught at C# level");
        return;
      } catch (Exception e) {
        CheckError(true, "TestErrorHandling: Wrong Exception " + e.Message + " was caught at C# level");
        return;
      }
      CheckError(true, "TestErrorHandling: Exception was not caught at C# level");
    }

    /// <summary>
    /// Test subclassing of C++ class with SWIG directors
    /// </summary>
    private static void TestPrinter()
    {
      //      Console.WriteLine( "Testing subclassing" );
      Printer aPrinter = new Printer();
      Message_Messenger aMess = Message.DefaultMessenger();
      aMess.AddPrinter(aPrinter);

      string aTestString = "test string: \u6709\u7528"; // Traditional Chinese "it works"
      aMess.Send(aTestString);
      aMess.RemovePrinter(aPrinter);

      CheckError(aPrinter.myMessage != aTestString,
        "TestPrinter: message capturing");
    }

    /// <summary>
    /// Test passing Unicode strings to and from OCCT
    /// </summary>
    private static void TestUnicode()
    {
      // Probe string with text "test file" converted to Japanese
      string aProbeText = "\u30C6\u30B9\u30C8\u30D5\u30A1\u30A4\u30EB";

      // Test conversion of strings
      TCollection_AsciiString aStr = new TCollection_AsciiString(aProbeText);
      TCollection_ExtendedString anExtStr = new TCollection_ExtendedString(aStr);
      CheckError(anExtStr.ToString() != aProbeText , "TestUnicode: string conversion - test 1");
      CheckError(aStr.ToString() != aProbeText, "TestUnicode: string conversion - test 2");

      // Test writing and reading a BREP file
      string aBRepFile = aProbeText + ".brep";
      if (System.IO.File.Exists(aBRepFile))
        System.IO.File.Delete(aBRepFile);
      TopoDS_Shape aBox = new BRepPrimAPI_MakeBox (10, 20, 30).Shape();
      CheckError(!BRepTools.Write(aBox, aBRepFile), "TestUnicode: writing BRep file");
      TopoDS_Shape aBox2 = new TopoDS_Shape();
      BRep_Builder aB = new BRep_Builder();
      CheckError(!BRepTools.Read(ref aBox2, aBRepFile, aB), "TestUnicode: reading BRep file");

      // Test writing and reading a STEP file
      string aSTEPFile = aProbeText + ".stp";
      if (System.IO.File.Exists(aSTEPFile))
        System.IO.File.Delete(aSTEPFile);
      STEPControl_Writer aSTEPWriter = new STEPControl_Writer();
      aSTEPWriter.Transfer(aBox, STEPControl_StepModelType.STEPControl_AsIs);
      CheckError(aSTEPWriter.Write(aSTEPFile) != IFSelect_ReturnStatus.IFSelect_RetDone, "TestUnicode: writing STEP file");
      STEPControl_Reader aSTEPReader = new STEPControl_Reader();
      CheckError(aSTEPReader.ReadFile(aSTEPFile) != IFSelect_ReturnStatus.IFSelect_RetDone, "TestUnicode: reading STEP file");

      // Remove files created by test
      System.IO.File.Delete(aBRepFile);
      System.IO.File.Delete(aSTEPFile);
    }

    /// <summary>
    /// Test possible memory leaks in passing strings to and from OCCT
    /// </summary>
    private static void TestStringMemory()
    {
      long VMSize0 = Process.GetCurrentProcess().VirtualMemorySize64 / (1024 * 1024);
      Console.WriteLine("Virtual memory occupied at start: " + VMSize0 + " Mb");

      // create string with 5000 *s
      string aTestStr = new string ('*', 5000);

      // Run multiple cycles, each time calling GC.Collect() explicitly
      for (int k = 1; k < 100; k++)
      {
        // cycle allocating 
        for (int i = 1; i < 1000; i++)
        {
          TCollection_ExtendedString aStr = new TCollection_ExtendedString (aTestStr);
          if (aStr.ToString() != aTestStr)
          {
            CheckError (true, "Error: strings are not equal!");
            k = 10000;
            break;
          }
        }
        System.GC.Collect();
        System.GC.WaitForPendingFinalizers();
      }
      Thread.Sleep(2000); // needed for system to release virtual memory
      long VMSize2 = Process.GetCurrentProcess().VirtualMemorySize64 / (1024 * 1024);
      Console.WriteLine("Virtual memory occupied at end: " + VMSize2 + " Mb");
      // around 500 Mb of memory shall be allocated and released during this test
      CheckError(VMSize2 - VMSize0 > 1, "TestStringMemory: memory used by string conversion operations");
    }

    // field
    private static int myNbErr = 0;
  }
}
