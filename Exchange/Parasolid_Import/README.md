## OPEN CASCADE Parasolid Import

OPEN CASCADE Parasolid Import allows Open CASCADE Technology-based applications to read and manipulate data from Parasolid files of all versions.

### Overview
OPEN CASCADE advanced data exchange tools enable Open CASCADE Technology-based applications to transfer geometric data from other software systems, ensuring openness and integration in a multi-software environment. With data exchange being crucial for interoperability between software systems in industrial settings, OPEN CASCADE is committed to providing tools that deliver high-quality data exchange.

The Parasolid Import module reads X_T and X_B files of all current versions (schemas) at two data levels:
1. Geometric and topological data for import into BRep models
2. Colors and Names, leveraging XDE (the Extended Data Exchange Module of Open CASCADE Technology)

BRep mapping can be used independently but is a prerequisite for complete mapping that includes attributes (Colors and Names).

### Benefits
This interface reads data from X_T (text) and X_B (binary) files of all current Parasolid versions into the Open CASCADE Technology BRep format.

It provides excellent exchange quality through Shape Healing, which corrects models before input into Open CASCADE Technology, and takes advantage of the Open CASCADE Technology data exchange framework. This framework provides file analysis, result verification, and allows total or partial transfer and use of attributes (with XDE) in a modular way.

### Features
**READ BRep data from Parasolid files**
- Loads Parasolid files of any version (schema)
- Converts geometric and topological data to Open CASCADE Technology shapes
- Utilizes Shape Healing to ensure data integrity
- Can be controlled by a resource file for special healing needs
- Applies default healing sequence when no resource file is defined

**READ Attributes from Parasolid files to XDE**
- Imports attributes like Colors and Names from Parasolid files
- Fills the Open CASCADE Technology XDE data structure with conversion results

### Prerequisites
The Parasolid Import interface is based on the current version of Open CASCADE Technology.

### Configuration
- Delivered as a separate module with source code available for a fee
- Available on all platforms supported by Open CASCADE Technology
- Packaged in two sub-parts:
  - BRep component (can be used independently)
  - XDE component (requires the BRep part)

## Getting Full Components
This is a preview repository. For full component access:
1. Visit [occt3d.com/components](https://occt3d.com/components/)
2. Select required components
3. Follow purchase process

## Technical documentation

- [Documentation](./Documentation/parasolid_interface.md)

## Contact & Support
- Website: [occt3d.com](https://occt3d.com)

© OPEN CASCADE SAS