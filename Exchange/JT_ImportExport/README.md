## OPEN CASCADE JT Import-Export

OPEN CASCADE JT Import-Export allows Open CASCADE Technology-based applications to read, write, and manipulate data in JT format.

### Overview
The Open CASCADE JT Import-Export SDK provides Open CASCADE Technology-based applications with the capability to:
- Read a JT file into a JT model
- Write a JT model into a JT file
- Create, explore, and modify JT models

Based on this SDK, it's possible to implement powerful JT visualization functionality using LOD management, object culling, and multi-threaded processing of parts (integration in customers' applications is available as an additional service).

### Features
**Data Storage Capabilities**
The following kinds of data can be stored in a JT model provided by this SDK:
- Assembly tree
- Mesh representation (triangulation) at different Levels Of Detail (LOD) of the whole JT model and its parts
- Names, colors, and transparency of JT parts

**Advanced Features**
The Open CASCADE JT Import-Export SDK also provides:
- Late Loading mechanism to delay loading of data associated with parts of the JT model until they are actually needed
- Data compression by JT standard compression algorithms

**Format Support**
- Reading of JT format versions 8.1, 9.5, and 10.0, 10.5
- Writing in JT format version 8.1, 9.5, 10.0

**JT Format Capabilities Support**
This SDK supports the following capabilities of JT format:
- LOD by elements "Vertex", "Tri-Strip Set", and "Primitive Set"
- Attributes "Geometric Transform" and "Material"
- Properties "Late Loaded" and "String"
- PMI information
- All node elements except for "Switch"
- "Data Compression and Encoding" by all "Encoding Algorithms", compressions "ZLIB" and "LZMA"

**Conversion Utilities**
The SDK provides converters:
- From an OCAF (Open CASCADE Application Framework) document to a JT model, transferring the hierarchy of names, colors, and meshes
- From a JT model to an OCAF document, transferring the hierarchy of names, colors, and meshes
- Additional extension is available for importing BRep information.

The first converter is especially useful when data from other widely used CAD formats (such as STEP, IGES, DXF, ACIS, Parasolid, and other native formats) read by the corresponding interfaces need to be converted to JT format.

### Benefits
- Faster data reading and reduced size of written data thanks to parallel processing
- Data conversion between different versions of JT format
- Full access to supported JT structures
- Convenience and flexibility of exchanging JT data with OCAF documents
- No third-party license required
- Source code available for licensing

### Prerequisites
The JT SDK is based on the latest version of Open CASCADE Technology.

### Configuration
- Requires no other external software
- Available on all platforms supported by the corresponding version of Open CASCADE Technology
- Operational mode and possible limitations are described in the accompanying documentation

## Getting Full Components
This is a preview repository. For full component access:
1. Visit [occt3d.com/components](https://occt3d.com/components/)
2. Select required components
3. Follow purchase process

## Technical documentation

- [Documentation](./Documentation/jt_interface.md)

## Contact & Support
- Website: [occt3d.com](https://occt3d.com)

© OPEN CASCADE SAS