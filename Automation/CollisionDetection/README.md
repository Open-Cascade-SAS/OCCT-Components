## OPEN CASCADE Collision Detection

OPEN CASCADE Collision Detection is a specialized component for fast detection of collision or proximity between pairs of shapes.

### Overview
Collision Detection is a commercial component based on Open CASCADE Technology designed to detect collision (intersection) or proximity between two shapes. The fast shape-shape collision and proximity detection algorithm (called "Proxy") enables detecting collisions or proximity criterion violations between a pair of Open CASCADE shapes positioned at arbitrary locations.

### Benefits
- Combines high performance in both collision and proximity modes with flexibility and ease of control
- Provides an efficient foundation for implementing various positioning techniques, including path walking for Coordinate Measurement Machines
- Allows easy adjustment of computation precision by changing the corresponding mesh deflection
- Features a convenient API for fast integration into existing applications, with easy connection to any particular GUI
- Requires no third-party licenses

### Features
The Collision Detection component can:

**Collision Detection**
- Detect if shapes are in contact with each other according to a specified tolerance
- Provide detailed information about collision locations (points, triangles, or faces)

**Proximity Detection**
- Given a proximity value, detect if shapes are near each other
- Provide related locations (points, triangles, or faces) where proximity criterion is triggered

**Technical Implementation**
- Based on manipulating trees of Axis-Aligned Bounding Boxes (AABB) constructed around triangle meshes bound to Open CASCADE shapes
- Can be easily adjusted to accept non-Open CASCADE meshes
- Allows independent positioning of each shape in arbitrary locations without recalculation of AABB-trees of mesh triangles
- Precision defined by the corresponding mesh deflection, which can be easily adjusted
- Performance comparable to other open source and commercial collision detection solutions

**Flexibility**
- Works with a wide range of input geometry without strong restrictions
- No requirements for convexity, manifold shapes, or non-coplanar triangles that other algorithms might impose

## Getting Full Components
This is a preview repository. For full component access:
1. Visit [occt3d.com/components](https://occt3d.com/components/)
2. Select required components
3. Follow purchase process

## Contact & Support
- Website: [occt3d.com](https://occt3d.com)
- For questions or requests: [Contact Form](https://www.opencascade.com/contact/)

© OPEN CASCADE SAS