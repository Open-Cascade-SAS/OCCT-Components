## OPEN CASCADE CAM Library

OPEN CASCADE CAM Library is a specialized component providing dedicated CAM algorithms for manufacturing applications.

### Overview
The Open CASCADE CAM Library SDK provides a set of dedicated algorithms allowing developers to:

- Convert arbitrary 2D curves into sequences of circular arcs and linear segments (treating 2D curves contained within edges that form planar wires/contours)
- Build planar sections of a shape (using its mesh representation)
- Offset planar sections
- Build path contours for pocketing operations
- Build geodesic curves on the surface of a shape:
  - Shortest paths between two given points
  - Isolines for given start points (seeds) and distances

### Features
The Open CASCADE CAM Library enables the following operations:

**Convert Contours**
- Special package providing algorithms to convert contours of arbitrary 2D curve types into contours consisting only of 2D circular arcs and 2D linear segments

**Compute Planar Sections**
- Dedicated algorithm for computing planar sections on sets of triangular meshes

**Compute Planar Outlines**
- Algorithm for computing outlines (planar projections similar to HLR) of sets of triangular meshes

**Compute Planar Offsets**
- 2D offset algorithm capable of building offsets of polygonal contours (contours composed of linear segments)
- Supports outer offsets (expand-type polygons), inner offsets (shrink-type polygons), or both simultaneously

**Compute Paths for Pocketing Operations**
- Dedicated algorithm for computing tool paths for pocketing operations on planes
- Current implementation provides an internal pocketing algorithm that calculates sets of offset contours placed in pocket border interiors

**Geodesic Algorithm**
- Algorithm for building geodesic curves implemented in a dedicated class
- Works on discrete (triangulated) models represented by triangulations of input shape faces
- Process begins with building a precise distance field on triangulations (inaccuracy defined only by roundoff errors)
- Enables computation of shortest paths and isolines

### Benefits
- Critical for CAM applications requiring efficient, ready-to-use computational algorithms
- Seamless integration of CAM functionality with CAD modules written with Open CASCADE Technology
- Combined elementary algorithms solve complex CAM tasks
- Computation results easily convertible to formats needed by various machines
- Backward compatibility of results with OCCT modeling functionality, enabling further processing using OCCT algorithms

### Prerequisites
The Open CASCADE CAM Library is based on the current version of Open CASCADE Technology.

### Configuration
- Available on all platforms supported by the corresponding version of Open CASCADE Technology
- Requires no other external software
- Operational mode and possible limitations described in the accompanying documentation

## Getting Full Components
This is a preview repository. For full component access:
1. Visit [occt3d.com/components](https://occt3d.com/components/)
2. Select required components
3. Follow purchase process

## Contact & Support
- Website: [occt3d.com](https://occt3d.com)

© OPEN CASCADE SAS