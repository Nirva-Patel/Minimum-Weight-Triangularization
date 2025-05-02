# Minimum Weight Triangulation

## Overview

This project focuses on the **Minimum Weight Triangulation (MWT)** problem, a well-known challenge in computational geometry. MWT aims to triangulate a set of points such that the total edge weight is minimized. This has real-world applications in graphics rendering, terrain modeling, mesh generation, and satellite imaging.


## Problem Statement

Given a set of `n` points in a 2D plane, the goal is to create a triangulation that:
- Connects all points using non-intersecting edges
- Forms valid triangles
- Minimizes the total length of all edges

This problem is **NP-Hard** and has been reduced to the **3-SAT problem** in complexity studies.


## Algorithms Implemented

### 1. **Exponential-Time Algorithm**
- Generates all possible triangulations
- Time Complexity: `O(4^n)`

### 2. **Greedy Algorithm**
- Adds edges in increasing order of length without causing intersections
- Time Complexity: `O(n^4)`

### 3. **Dynamic Programming**
- Uses convex hull and divides polygon recursively
- More efficient in practice than exponential method
- Achieved triangulation weight example: `13.30`

### 4. **Local Search Algorithm**
- Iteratively improves triangulation via edge flips
- Time Complexity: `O(n * I)` where `I` = number of iterations

### 5. **Grid-Based Algorithm**
- Partitions space recursively using quad-trees
- Time Complexity: `O(n log n)`

## Applications

- **3D Face Reconstruction** in facial recognition
- **Aircraft Design**
- **Satellite Image Compression** (e.g., NASA’s Terra satellite)
- **Terrain Modeling** in geospatial simulations


## Conclusion

Minimum Weight Triangulation is a challenging yet fundamental problem. Multiple algorithmic approaches offer trade-offs in time complexity and approximation quality. Dynamic programming and grid-based algorithms yield efficient and practical solutions.


 
