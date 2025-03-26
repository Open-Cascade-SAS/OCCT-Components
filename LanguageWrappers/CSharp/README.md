## What OCC C# Wrapper Is

The OCC C# Wrapper is a tool for wrapping Open CASCADE Technology (OCCT) C++ classes to C#, allowing their use within .NET applications. The C# interface reuses C++ DLLs and can call C++ methods through C# calls. The wrapping process generates a list of C# files that together form a single C# project interface. This results in a single DLL with modules that correspond to the wrapped OCCT classes.

## C# Wrapper Full Version
The full version of the C# wrapper is a complete package containing all the necessary scripts and files to generate C# files from a C++ OCCT project and build them into a single DLL. After purchasing the full version, you will have the freedom to update the generator based on your needs. The delivery includes the full source package, and you are free to use it in any application you are developing.

## C# Wrapper Code Sample

~~~cpp
// Reading CAD FILES Sample:
using OCC.TCollection;
using OCC.DE;
using OCC.TDocStd;
using OCC.XCAFApp;
using OCC.STEPCAFControl;

// Load a container for each DE component
var aWrapper = DE_Wrapper.GlobalWrapper().Copy();
// Manually updating parameters for reading CAD files
TCollection_AsciiString thePath = new TCollection_AsciiString("");
DE_Provider? aProvider = null;
if (!aWrapper.FindProvider(thePath, true, aProvider).)
{
  // Do something
}
// Modify string with parameters, described in the documentation:
DE_ConfigurationNode aConfig = aProvider.GetNode();
TCollection_AsciiString aParameters = aConfig.Save();
// https://dev.opencascade.org/doc/overview/html/occt_user_guides__de_wrapper.html
aConfig.Load(aParameters);

// Reading CAD file
TDocStd_Document? aDoc = null;
XCAFApp_Application anApp = XCAFApp_Application.GetApplication();
anApp.NewDocument(new TCollection_ExtendedString("BinXCAF"), aDoc);
if (!aProvider.Read(thePath, aDoc))
{
    // Do something
}

// Sample to Read using original reader functionality
var aReader = new STEPCAFControl_Reader();
aReader.SetColorMode(true);
if (!aReader.Perform(thePath, aDoc))
{
  // Do something
}
~~~

## C# Wrapper Overview in Visual Studio

The final result of the wrapping process is a single DLL that can be easily integrated into any C# project. Below is a simple example of how the wrapped DLL appears in the "VS Object Browser":

<img src="documentation/images/CSharp_sample.png" alt="">

## Windows C# Support:

- .NET Framework v3.5
- .NET Framework v4.0
- .NET Framework v4.5
- .NET Framework v4.5.1
- .NET Framework v4.5.2
- .NET Framework v4.6
- .NET Framework v4.6.1
- .NET Framework v4.6.2
- .NET Framework v4.7
- .NET Framework v4.7.1
- .NET Framework v4.7.2
- .NET 6.0
- .NET 7.0
- .NET 8.0

## Linux C# Support:

- .NET 6.0
- .NET 7.0
- .NET 8.0

## Getting Full Components
This is a preview repository. For full component access:
1. Visit [occt3d.com/components](https://occt3d.com/components/)
2. Select required components
3. Follow purchase process

## Contact & Support
- Website: [occt3d.com](https://occt3d.com)

© OPEN CASCADE SAS