## OPEN CASCADE Surfaces from Scattered Points

OPEN CASCADE Surfaces from Scattered Points is a specialized component that provides advanced capabilities for surface reconstruction from point data.

### Overview
The Surfaces from Scattered Points (SSP) algorithm provides the groundwork to perform various operations in the field of surface reconstruction. In particular, it allows:
- Constructing a surface conforming to given point, tangential, scalar, and linear constraints (commonly called "Surface from Scattered Points" or "Surface from Cloud of Points")
- Creating a surface that conforms to certain boundary constraints ("Hole Filling")
- Modifying an input surface to satisfy certain spatial constraints ("Surface Morphing")
- Modifying an input surface (of a face) to reduce gaps between surface and wireframe ("Gaps Filling")

The algorithm is based on a variational method (minimization of energy-like functional) producing a B-Spline surface that satisfies the input criteria.

### Benefits
The SSP component is a highly customizable tool that brings the power of variational methods to solve specific tasks of surface reconstruction. It can be used in a wide range of industrial applications providing high-quality solutions in:
- Automatic measurements
- Reverse engineering
- Generative shape design
- Shapes customization
- Morphing
- Other surface reconstruction scenarios

### Features
To generate the desired surface using the OPEN CASCADE Surfaces from Scattered Points algorithm, the following basic steps are performed:
1. Loading the constraints
2. Creating or loading the initial surface
3. Generating the final surface

The surface is modified according to constraints, sometimes called target points. All other types of constraints (boundary constraints, morphing constraints) are translated to point constraints. Custom constraints can be configured via combinations of several other constraints.

**Customizable Parameters**
To construct the final surface, the SSP algorithm provides numerous configurable parameters:
- Smoothing coefficient: Directly affects the appearance of the final surface and how strictly it adheres to point constraints
- Order of continuity: Determines how the patches that compose the surface are made continuous
- Number of iterations: Represents the number of calculations performed after computing the first result
- Energy coefficients: Simulate the energy needed to bend the initial shape
- U and V degrees: Controls the flexibility of the surface
- Number of knots in U and V directions: Controls the level of detail

**Initial Surface Types**
The most important parameter for "Surface from Scattered Points" and "Hole Filling" operations is the initial surface, as the entire cloud of point constraints is applied to it. The algorithm can create three types of initial surfaces:
- A plane
- A patch (trapezium on 4 points)
- A conical patch (a conical surface built on 6 points)

All these types of input surfaces are internally converted to B-Spline. Users can choose between these surfaces or build a custom one to fit particular requirements.

## Getting Full Components
This is a preview repository. For full component access:
1. Visit [occt3d.com/components](https://occt3d.com/components/)
2. Select required components
3. Follow purchase process

## Contact & Support
- Website: [occt3d.com](https://occt3d.com)
- For questions or requests: [Contact Form](https://www.opencascade.com/contact/)

© OPEN CASCADE SAS