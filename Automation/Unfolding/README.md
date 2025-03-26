## OPEN CASCADE Unfolding

OPEN CASCADE Unfolding is a specialized component providing tools to convert curved shells into their planar equivalents.

### Overview
There is a common need in industries dealing with sheet material products (shipbuilding in the sphere of vessel design, construction, aerospace, manufacturing (for example, in clothes fitting software), etc.) to accurately calculate the shape of a sheet material blank to be used for production of a particular detail of the product (a plate). Open CASCADE Unfolding Library addresses this need by providing a tool to convert curved shells into their planar equivalents.

### Features
3D unfolding software utilizes the unfolding process that is opposite to the process of deformation of a flat sheet into a curved plate occurring during production. If the shape of a plate is a developable surface, the curved plate can be obtained by pure bending. In this case the unfolding result is well defined, and corresponds to that surface developed on a plane without distortions.

If a plate is not developable (i.e., has non-zero Gaussian curvature in some areas), then additional in-plane deformations (stretching or compression) are applied during production. The unfolding algorithm simulates this process by applying minimal deformations necessary to make the plate planar. The energy of deformations is minimized so that the result is quite accurate when distortions are small.

The distribution of deformations along the plate surface is one of the algorithm results; it provides numerical criteria to evaluate the unfolding accuracy, and can be used for deciding how to organize the plate forming process.

### Benefits
- Easy and straightforward integration in OCCT-based applications for modeling sheet material products
- Mapping of points and lines located on a plate from a 3D (curved) shape to a 2D (unfolded) model and back
- Adjustable accuracy by tuning the mesh generation algorithm and its parameters
- Evaluation of deformations by value of strains at any point on the plate
- Export to PLY and VTK formats for easy transfer of results to other applications
- No third-party licensing is required to use this component

### Application Areas
- Shipbuilding and vessel design
- Aerospace engineering
- Sheet metal manufacturing
- Clothing and textile design
- Any industry working with sheet material products

### Configuration
The Unfolding SDK is based on the latest version of Open CASCADE Technology and requires no other external software. It is available on all platforms supported by the corresponding version of Open CASCADE Technology. Operational mode and possible limitations are described in the documentation accompanying the module.

## Getting Full Components
This is a preview repository. For full component access:
1. Visit [occt3d.com/components](https://occt3d.com/components/)
2. Select required components
3. Follow purchase process

## Contact & Support
- Website: [occt3d.com](https://occt3d.com)

© OPEN CASCADE SAS