## OPEN CASCADE ACIS-SAT Import-Export

OPEN CASCADE ACIS-SAT Import-Export allows applications based on Open CASCADE Technology to interoperate with ACIS through reading and writing data in SAT format.

### Overview
OPEN CASCADE advanced data exchange tools enable applications based on Open CASCADE Technology (OCCT) to transfer data to and from other software systems. With proper data exchange being crucial for interoperability between applications in a collaborative environment, OPEN CASCADE is committed to providing tools that deliver high-quality data exchange.

The OPEN CASCADE ACIS-SAT Import-Export module reads and writes SAT files at two data levels:
1. Geometrical and topological data are mapped to OCCT shapes (BRep mapping)
2. Colors and names are mapped to XDE data structures when the XDE (OCCT eXtended Data Exchange) module is used

BRep mapping can be used independently but is a prerequisite for complete mapping that includes attributes (colors and names).

### Benefits
- Provides direct exchange of data with systems that support SAT format for OCCT-based applications, enabling users to create custom OCCT-based modules that extend the functionality of standard off-the-shelf products
- Delivers excellent exchange quality through Shape Healing, which corrects models during translation into OCCT shapes
- Does not require any third-party license
- Source code available as an additional option
- Can be combined with other OPEN CASCADE interfaces to connect different applications working with various formats

### Features
**READ BRep data from ACIS-SAT files up to version 14**
- Loads SAT files of any version up to 14
- Converts geometrical and topological data to OCCT shapes
- Utilizes Shape Healing to ensure data integrity
- Can be controlled by a resource file for special healing needs
- Uses approximation (available from ACIS version 5.0) when conversion issues occur

**WRITE BRep data to ACIS-SAT files up to version 7**
- Converts shapes from OCCT to ACIS data
- Writes files according to user-selected ACIS version (default is version 6.0)
- Can call Shape Healing under resource file control
- Allows adaptation of written shape configuration to the needs of the ACIS-based receiving application

**Reading/Writing Attributes from ACIS-SAT to XDE**
- When reading: retrieves attributes attached to BRep data and fills XDE data structures
- When writing: translates attributes attached to OCCT shapes in XDE to their SAT equivalents
- Processes colors and names present in SAT files

### Prerequisites
The ACIS-SAT Import-Export interface is based on the current version of Open CASCADE Technology.

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

- [Documentation](./Documentation/sat_interface.md)

## Contact & Support
- Website: [occt3d.com](https://occt3d.com)

© OPEN CASCADE SAS