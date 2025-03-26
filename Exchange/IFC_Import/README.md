## OPEN CASCADE IFC Import

OPEN CASCADE IFC Import allows Open CASCADE Technology-based applications to read and process Building Information Modeling (BIM) data from Industry Foundation Classes (IFC) format files.

### Overview
The Open CASCADE IFC Import SDK provides applications with the capability of reading BIM data from files in IFC format. The component supports IFC2x3 and IFC4 versions, which are used by most modern applications.

### Features
**Reading Geometric and Visual Data**
- Converts the hierarchy of objects, their geometry (shapes), and generic attributes such as names, colors, and transparency to the XDE format
- Enables immediate use of this data in applications for:
  - 3D visualization
  - Geometric analysis and measuring
  - Conversion to other formats
  - And more

Note: Objects in IFC files may be defined by pure tessellation (mesh) without underlying surfaces and topology. Such objects will not be suitable for OCCT algorithms that expect CAD objects in B-Rep (boundary representation) form.

**Accessing Properties and Attributes**
- Provides dedicated interfaces to access properties and attributes associated with IFC objects:
  - GUIDs
  - Object types
  - Materials
  - BIM properties
  - 2D sketches
  - And more

### Benefits
- Fast and robust conversion of complex IFC models
- Ability to translate only specific data types to avoid unnecessary conversions
- Uses standard XDE API, allowing easy integration into applications that use Open CASCADE Technology for CAD data
- Source code available for licensing

### Prerequisites
The IFC Import interface is based on:
- The latest version of Open CASCADE Technology
- An optimized version of IfcOpenShell library (also based on Open CASCADE Technology)

Both are open source libraries available under LGPL license.

### Configuration
- Available on all platforms supported by the corresponding versions of Open CASCADE Technology and IfcOpenShell
- Operational mode and possible limitations are described in the documentation accompanying the module

## Getting Full Components
This is a preview repository. For full component access:
1. Visit [occt3d.com/components](https://occt3d.com/components/)
2. Select required components
3. Follow purchase process

## Contact & Support
- Website: [occt3d.com](https://occt3d.com)

© OPEN CASCADE SAS