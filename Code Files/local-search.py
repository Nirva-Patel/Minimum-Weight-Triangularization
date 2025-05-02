import numpy as np
import matplotlib.pyplot as plt
from scipy.spatial import Delaunay
import itertools

def plot_triangulation(points, triangles, title, pause=1):
    plt.triplot(points[:, 0], points[:, 1], triangles, color='black')
    plt.plot(points[:, 0], points[:, 1], 'o', color='red')
    plt.title(title)
    plt.gca().set_aspect('equal')
    plt.pause(pause)
    plt.clf()

def edge_length(p1, p2):
    return np.linalg.norm(p1 - p2)

def triangle_weight(triangle, points):
    a, b, c = points[triangle]
    return edge_length(a, b) + edge_length(b, c) + edge_length(c, a)

def flip_edge(triangles, edge, points):
    """Flips the given edge in the triangulation if it's part of a convex quad and reduces weight."""
    t1, t2 = edge_to_triangles[edge]
    v1 = list(set(t1) - set(edge))[0]
    v2 = list(set(t2) - set(edge))[0]

    quad = [v1, edge[0], v2, edge[1]]
    p = points[quad]

    def is_convex(p):
        cross1 = np.cross(p[1] - p[0], p[2] - p[1])
        cross2 = np.cross(p[2] - p[1], p[3] - p[2])
        return cross1 * cross2 > 0

    if not is_convex(p):
        return False

    old_weight = triangle_weight(t1, points) + triangle_weight(t2, points)
    new_tri1 = [v1, v2, edge[0]]
    new_tri2 = [v1, v2, edge[1]]
    new_weight = triangle_weight(new_tri1, points) + triangle_weight(new_tri2, points)

    if new_weight < old_weight:
        triangles.remove(t1)
        triangles.remove(t2)
        triangles.append(new_tri1)
        triangles.append(new_tri2)
        return True
    return False

def local_search_triangulation(points, triangles):
    global edge_to_triangles
    triangles = [list(tri) for tri in triangles]
    iteration = 0

    while True:
        improved = False
        # Create edge to triangle mapping
        edge_to_triangles = {}
        for tri in triangles:
            for a, b in itertools.combinations(tri, 2):
                edge = tuple(sorted((a, b)))
                if edge not in edge_to_triangles:
                    edge_to_triangles[edge] = []
                edge_to_triangles[edge].append(tri)

        # Try flipping edges
        for edge, tris in edge_to_triangles.items():
            if len(tris) == 2:
                if flip_edge(triangles, edge, points):
                    improved = True
                    plot_triangulation(points, triangles, f'Iteration {iteration + 1}')
                    break
        if not improved:
            break
        iteration += 1
    return triangles

# Generate sample points
np.random.seed(42)
points = np.random.rand(13, 2)

# Initial Delaunay triangulation
delaunay = Delaunay(points)
triangles = delaunay.simplices.tolist()

plt.figure(figsize=(6, 6))
plot_triangulation(points, triangles, "Initial Triangulation", pause=2)

# Perform local search
final_triangles = local_search_triangulation(points, triangles)

plot_triangulation(points, final_triangles, "Final Triangulation", pause=0)
plt.show()
