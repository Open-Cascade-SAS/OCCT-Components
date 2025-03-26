## OPEN CASCADE Canonical Recognition

OPEN CASCADE Canonical Recognition is a specialized component that converts NURBS geometry (B-Spline/Bezier curves or surfaces) to their analytical form (primitive curves or surfaces).

### Overview
Canonical Recognition is a commercial component designed to convert NURBS geometry to analytical forms wherever possible. This capability is crucial for algorithms that are implemented or best fitted to work on canonical geometry, especially in CAM domains for Coordinate Measuring Machines (CMM) and other tooling applications.

The component is built upon the Open CASCADE Shape Healing module, leveraging its powerful capabilities to ensure high-quality conversions while maintaining topological integrity.

### Benefits
- Adds significant value to Open CASCADE-based applications dealing primarily with analytical geometry, such as CAM or Metrology applications
- Improves performance and robustness of numeric algorithms (intersection, projection, etc.)
- Enables access to all information specific to canonical geometry (radii, focuses, centers, equations, etc.), which can be used in applications for display, quick search, statistics, and more
- Features an easy-to-use API that facilitates integration into existing or new Open CASCADE-based applications, either as part of the CAD data import process or as a standalone functionality accessible via GUI
- Requires no third-party license

### Features
The Canonical Recognition component offers:

**1. Surfaces and curves simplification** for better reusability in downstream applications:

NURBS 3D curves can be translated to:
- Lines
- Circles or arcs of circles
- Ellipses or arcs of ellipses

NURBS surfaces can be translated to:
- Planes
- Spheres
- Cylinders
- Cones
- Tori

**2. Merging capability** for adjacent topological objects (edges or faces) built on NURBS geometry into a single object based on analytical geometry:
- Reduces complexity of the topological model
- Increases usability for other algorithms
- Decreases memory usage

**3. Tolerance management** for recognition algorithms:
- Process is controlled by user-defined tolerance
- Applied for both recognition and approximation with canonical geometry

The conversion preserves the topological structure of the original shape, maintaining connectivity of adjacent faces and edges by tracking the history of modifications.

### Applications
Canonical Recognition can be optimally used:
- After importing neutral or proprietary files (IGES, STEP, ACIS, etc.)
- In preparatory modules before using specialized algorithms
- As part of data cleanup and optimization workflows

## Getting Full Components
This is a preview repository. For full component access:
1. Visit [occt3d.com/components](https://occt3d.com/components/)
2. Select required components
3. Follow purchase process

## Contact & Support
- Website: [occt3d.com](https://occt3d.com)
- For questions or requests: [Contact Form](https://www.opencascade.com/contact/)

© OPEN CASCADE SAS