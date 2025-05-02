#include <bits/stdc++.h>
using namespace std;


struct Point {
    double x, y;
};


double euclideanDistance(const Point &a, const Point &b) {
    return hypot(a.x - b.x, a.y - b.y);
}


int orientation(const Point &p, const Point &q, const Point &r) {
    double val = (q.y - p.y)*(r.x - q.x) - (q.x - p.x)*(r.y - q.y);
    if (abs(val) < 1e-9) return 0;
    return (val > 0) ? 1 : 2;
}


bool onSegment(const Point &p, const Point &q, const Point &r) {
    return min(p.x, r.x) <= q.x && q.x <= max(p.x, r.x) &&
          min(p.y, r.y) <= q.y && q.y <= max(p.y, r.y);
}


bool doIntersect(Point p1, Point q1, Point p2, Point q2) {
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);


    if (o1 != o2 && o3 != o4) return true;
    if (o1 == 0 && onSegment(p1, p2, q1)) return true;
    if (o2 == 0 && onSegment(p1, q2, q1)) return true;
    if (o3 == 0 && onSegment(p2, p1, q2)) return true;
    if (o4 == 0 && onSegment(p2, q1, q2)) return true;
    return false;
}


bool edgeIntersectsExisting(int i, int j, const vector<Point> &points, const set<pair<int, int>> &existingEdges) {
    Point p1 = points[i], q1 = points[j];
    for (const auto &edge : existingEdges) {
        int u = edge.first, v = edge.second;
        if (i == u || i == v || j == u || j == v) continue;
        if (doIntersect(p1, q1, points[u], points[v])) return true;
    }
    return false;
}


bool triangleFormsIntersection(int i, int j, int k, const vector<Point> &points, const set<pair<int, int>> &existingEdges) {
    return edgeIntersectsExisting(i, k, points, existingEdges) ||
          edgeIntersectsExisting(j, k, points, existingEdges);
}


int orientation2D(const Point &a, const Point &b, const Point &c) {
    double cross = (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x);
    if (abs(cross) < 1e-9) return 0;
    return (cross > 0) ? 1 : -1;
}


vector<int> convexHullIndices(const vector<Point> &pts) {
    int n = pts.size();
    vector<int> hull;
    if (n < 3) return hull;


    vector<int> sorted(n);
    iota(sorted.begin(), sorted.end(), 0);
    sort(sorted.begin(), sorted.end(), [&](int i, int j) {
        return pts[i].x < pts[j].x || (pts[i].x == pts[j].x && pts[i].y < pts[j].y);
    });


    vector<int> lower, upper;
    for (int i : sorted) {
        while (lower.size() >= 2 && orientation2D(pts[lower[lower.size() - 2]], pts[lower[lower.size() - 1]], pts[i]) <= 0)
            lower.pop_back();
        lower.push_back(i);
    }
    for (int i = n - 1; i >= 0; --i) {
        int idx = sorted[i];
        while (upper.size() >= 2 && orientation2D(pts[upper[upper.size() - 2]], pts[upper[upper.size() - 1]], pts[idx]) <= 0)
            upper.pop_back();
        upper.push_back(idx);
    }


    lower.pop_back();
    upper.pop_back();
    hull = lower;
    hull.insert(hull.end(), upper.begin(), upper.end());
    return hull;
}


bool isFullyTriangulated(int triangleCount, int n, int hullSize) {
    return triangleCount == 2 * n - hullSize - 2;
}


int main() {
    int n;
    cin>>n;
   
    vector<Point> points(n);
        //{0, 1}, {1, 1}, {-5, 0}, {-1, -1},{-6,-3},{-1,-5},{2,-2},{1,-3},{3,-3},{1,-6}
    cout << "Enter the coordinates (x y) of each point:\n";
    for (int i = 0; i < n; ++i) {
        cin >> points[i].x >> points[i].y;
    }
    vector<int> hullIndices = convexHullIndices(points);
    int hullSize = hullIndices.size();


    struct Edge {
        int i, j;
        double weight;
        bool operator<(const Edge &e) const {
            return weight < e.weight;
        }
    };


    vector<Edge> edges;
    for (int i = 0; i < n; ++i)
        for (int j = i + 1; j < n; ++j)
            edges.push_back({i, j, euclideanDistance(points[i], points[j])});


    sort(edges.begin(), edges.end());


    set<pair<int, int>> existingEdges;
    unordered_map<int, unordered_set<int>> adjacency;
    set<tuple<int, int, int>> usedTriangles;
    vector<tuple<int, int, int>> triangulation;
    double totalWeight = 0;


    for (const auto &e : edges) {
        int i = e.i, j = e.j;


        if (edgeIntersectsExisting(i, j, points, existingEdges)) continue;


        vector<int> commonNeighbors;
        for (int ni : adjacency[i]) {
            if (adjacency[j].count(ni))
                commonNeighbors.push_back(ni);
        }


        for (int k : commonNeighbors) {
            if (!triangleFormsIntersection(i, j, k, points, existingEdges)) {
                vector<int> tri = {i, j, k};
                sort(tri.begin(), tri.end());
                tuple<int, int, int> triangle = {tri[0], tri[1], tri[2]};
                if (!usedTriangles.count(triangle)) {
                    triangulation.push_back(triangle);
                    usedTriangles.insert(triangle);
                }
            }
        }


        existingEdges.insert({i, j});
        existingEdges.insert({j, i});
        adjacency[i].insert(j);
        adjacency[j].insert(i);
        totalWeight += e.weight;


        if (isFullyTriangulated(triangulation.size(), n, hullSize))
            break;
    }


    cout << "Triangles formed:\n";
    for (auto &tri : triangulation) {
        int a, b, c;
        tie(a, b, c) = tri;
        cout << "(" << a << ", " << b << ", " << c << ")\n";
    }
    cout << "Total weight: " << totalWeight << endl;


    return 0;
}