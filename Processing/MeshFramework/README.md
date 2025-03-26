## OPEN CASCADE Mesh Framework (OMF)

OPEN CASCADE Mesh Framework (OMF) provides a clear and extensible architecture for operations with mesh data structures.

### Overview
Open CASCADE Mesh Framework offers a comprehensive solution for storing and manipulating mesh data structures. OMF supports a variety of linear elements - from 0D up to 3D, providing tools for reading meshes from NASTRAN or STL files and writing them in these formats.

The framework delivers rich capabilities for mesh editing with highly efficient data storage organization that enables quick access to any mesh data. OMF supports both direct connections (element to nodes) and inverse connections (node to elements), significantly enhancing the performance of update operations.

Take advantage of Open CASCADE Mesh Framework to develop your pre- and post-processing applications while reducing development costs!

### Benefits
- High performance operations on mesh data structures
- Comprehensive functionality for storing, reading, writing, editing, and visualizing meshes
- Efficient data organization with optimized access
- Seamless integration with OCCT document framework

### Features

**Creating a Mesh**
A mesh can be created in two ways:
- From scratch
- By import from file

The framework supports multiple element types:
- 0D elements: nodes
- 1D elements: edges
- 2D elements: triangles and quadrangles
- 3D elements: tetrahedrons, pyramids, prisms and hexahedrons

Users can also create hierarchical mesh structures.

**Exploration of the Mesh**
OMF provides functionality to:
- Iterate on elements of a given type (nodes, edges, faces, etc.)
- Create groups from these elements
- Define various positions of elements related to underlying geometry (CAD model)
- Extract free boundaries from a mesh

**Editing the Mesh**
Mesh editing capabilities include adding and removing nodes and elements:
- Direct connections ensure that when nodes are modified, all elements containing these nodes are updated automatically
- Inverse connections ensure that when nodes are deleted, all elements containing these nodes will be deleted automatically

**Visualization of the Mesh**
A mesh can be displayed in three standard modes:
- Wire frame
- Shading
- Shrink view (with configurable shrink coefficient)

Additionally, users can easily create custom display modes. OMF provides dynamic detection and selection of mesh elements.

**Transformation of the Mesh**
The framework allows applying transformations to:
- A whole mesh
- Individual mesh elements

**Integration into Application Framework**
OMF includes special capabilities to integrate mesh structures into the document-oriented application framework of Open CASCADE (OCAF):
- Special OCAF attributes encapsulate the mesh structure
- Support for all standard document operations such as undo/redo, store on disk, and retrieve

### Configuration
- Delivered as a stand-alone module requiring no other software except Open CASCADE Technology
- Requires no third-party licenses
- Available on all platforms supported by Open CASCADE Technology

## Getting Full Components
This is a preview repository. For full component access:
1. Visit [occt3d.com/components](https://occt3d.com/components/)
2. Select required components
3. Follow purchase process

## Contact & Support
- Website: [occt3d.com](https://occt3d.com)

© OPEN CASCADE SAS