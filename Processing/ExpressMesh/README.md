## OPEN CASCADE Express Mesh

OPEN CASCADE Express Mesh is an advanced algorithm for high-performance triangular meshing of surfaces represented with BRep shapes.

### Overview
Express Mesh is a commercial component featuring an advanced algorithm for meshing surfaces represented with BRep shapes. It provides triangular meshing of a Shape containing Faces (Solid, Shell, or a compound of Faces). Each face is tessellated individually, while maintaining connectivity between triangles built on connected faces (on faces that share edges).

Express Mesh takes several input parameters, such as element size, chordal deflection, and angular deviation, giving users an extended level of control over the mesh quality and the number of generated mesh elements.

### Features
**Parameters**
Express Mesh has the following input parameters:

1. **Minimal element size**: Defines the lower size limit of an element side. Increasing this parameter speeds up the meshing process at the expense of quality (surface feature details). Users can intentionally coarsen the model, skipping insignificant surface details.

2. **Maximal element size**: Defines the upper size limit of an element side. Can be used to avoid creation of very large triangles on flat areas of surfaces, or very long and thin triangles along cylindrical surfaces or fillets.

3. **Deflection**: Defines the maximal allowable chordal deviation of a mesh element from the original surface.

4. **Angular deviation**: Defines the maximal angular deviation of triangles' normals from the surface normals. Lower values result in more triangles on curved surface areas. Together with deflection and minimal size, this parameter controls output mesh quality in terms of respecting small surface features.

5. **Computation mode**: Defines the mode in which the algorithm will be performed. The algorithm supports both sequential (standard for monocore systems) and parallel modes (utilizing multiple threads on multicore systems for high performance).

The Deflection and Angular deviation parameters are not mutually exclusive and can be turned on/off depending on the user needs. Angular deviation controls mesh smoothness, while Deflection controls linear deviation from the original surface.

**Discretisation and Healing**
To increase robustness, Express Mesh features several healing operations:

- Special treatment to achieve well-spaced discrete segments along edges of the input shape, even with badly parameterized edge curves
- Detection and resolution of self-intersections on discrete curves, including removal of small loops
- Connection of adjacent discrete curves of a wire to achieve a contiguous closed contour
- Detection and resolution of mutual intersections between wires on a face

**Output Format**
The current version of Express Mesh writes the output triangulation into the same shape, similar to the BRepMesh algorithm. Future versions will include the ability to redirect output to a specified container, such as the OMF mesh data structure, without requiring additional memory for temporary storage.

### Benefits
- Provides very high performance and superior control over mesh quality compared to the open source BRepMesh algorithm from OCCT
- Ideal for applications where surface meshing speed is crucial, such as collision detection between shapes
- Generated meshes with controlled element size and quality can be used in FEA applications
- Can serve as a high-speed alternative algorithm for visualization purposes

## Getting Full Components
This is a preview repository. For full component access:
1. Visit [occt3d.com/components](https://occt3d.com/components/)
2. Select required components
3. Follow purchase process

## Contact & Support
- Website: [occt3d.com](https://occt3d.com)
- For questions or requests: [Contact Form](https://www.opencascade.com/contact/)

© OPEN CASCADE SAS