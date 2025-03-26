## OPEN CASCADE DXF Import-Export

OPEN CASCADE DXF Import-Export allows applications based on Open CASCADE Technology to interoperate with AutoCAD and other CAD systems through reading and writing data in DXF format.

### Overview
OPEN CASCADE advanced data exchange tools enable applications based on Open CASCADE Technology (OCCT) to transfer data to and from other software systems. With proper data exchange being crucial for interoperability between applications in a collaborative environment, OPEN CASCADE is committed to providing tools that deliver high-quality data exchange.

The OPEN CASCADE DXF Import-Export module reads and writes DXF files at two data levels:
1. Geometrical and topological data are mapped to OCCT shapes (BRep mapping)
2. Colors, names and assembly structure are mapped to XDE data structures when the XDE (OCCT eXtended Data Exchange) module is used

BRep mapping can be used independently but is a prerequisite for complete mapping that includes attributes (colors and names) and assembly structure.

### Benefits
- Provides direct exchange of data with systems that support DXF format (AutoCAD, MegaCAD, Microstation, etc.) for OCCT-based applications
- Enables users to create custom OCCT-based modules extending the functionality of standard off-the-shelf products
- Delivers excellent exchange quality through Shape Healing, which corrects models during translation into OCCT shapes
- Does not require any third-party license
- Source code available as an additional option
- Can be combined with other OPEN CASCADE interfaces to connect different applications working with various formats

### Features
**Reading/Writing Geometrical and Topological Information**
- Supports DXF files of the following versions: R10 and earlier, R12, R13, 2000, 2002, 2004, 2007, 2010, 2013
- Reads and writes DXF entities embedding ACIS bodies (BODY, 3DSOLID and REGION)
- Offers option to convert OCCT shells and solids either as polyface meshes (pure DXF) or as ACIS bodies embedded into DXF entities
- Utilizes Shape Healing and OCCT modeling algorithms to ensure translation quality
- Can be controlled by a resource file for special healing needs
- Adapts written shape configuration to the needs of the DXF-enabled receiving application

**Reading/Writing Attributes from DXF to XDE**
- When reading: retrieves attributes attached to BRep data and fills XDE data structures
- When writing: translates attributes attached to OCCT shapes in XDE to their DXF equivalents
- Processes colors, names and assembly structure present in DXF files
- Allows attributes to be used through the XDE API like any other external attribute (read from IGES, STEP)

### Prerequisites
The DXF Import-Export interface is based on the current version of Open CASCADE Technology.

### Configuration
- Delivered as a stand-alone module requiring no other external software
- Available on all platforms supported by the latest version of OCCT
- Packaged in two parts:
  - BRep conversion (can be used independently)
  - XDE component (requires the BRep part)

## Getting Full Components
This is a preview repository. For full component access:
1. Visit [occt3d.com/components](https://occt3d.com/components/)
2. Select required components
3. Follow purchase process

## Technical documentation

- [Documentation](./Documentation/dxf_interface.md)

## Contact & Support
- Website: [occt3d.com](https://occt3d.com)

© OPEN CASCADE SAS