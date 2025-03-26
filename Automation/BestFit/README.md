## OPEN CASCADE BestFit

OPEN CASCADE BestFit is a specialized component for optimally aligning point clouds with 3D CAD models.

### Overview
The Best Fit Algorithm is used to solve the problem of finding the proper position of a cloud of points relative to a 3D CAD model in order to globally minimize the distances between an input data set (a cloud of points, e.g. obtained by measurement of the real part) and its corresponding nominal model (CAD model of the real part).

The output of this algorithm is a transformation matrix – translation & rotation – that should be applied to the cloud of points in order to put them into the best-fit position regarding the CAD model.

### Features
The Best Fit Algorithm takes three steps for finding the best-fit position:

1. **Deviation Calculation**: Calculates the deviation of each input point from the nominal shape.

2. **Error Function Computation**: Computes the error function evaluating the deviation of the initial set of points from the nominal shape, based on calculated deviation at each point and its known dependency on transformation parameters.

3. **Optimization**: Solves the optimization problem to find the best-fit transformation.

### Benefits
- Convenient C++ API for easy integration into applications
- Supports projection on surfaces and/or wire-frames
- Provides accurate and fast deviation computation
- Requires no third-party licensing

### Applications
Best Fit is particularly useful in:
- Quality control and inspection
- Reverse engineering
- Part alignment for manufacturing
- Dimensional metrology
- Point cloud registration to CAD models

### Prerequisites
The BestFit component is based on the current version of Open CASCADE Technology.

### Configuration
- Available on all platforms supported by the corresponding version of Open CASCADE Technology
- Requires no other external software
- Operational mode and possible limitations are described in the accompanying documentation

## Getting Full Components
This is a preview repository. For full component access:
1. Visit [occt3d.com/components](https://occt3d.com/components/)
2. Select required components
3. Follow purchase process

## Contact & Support
- Website: [occt3d.com](https://occt3d.com)

© OPEN CASCADE SAS