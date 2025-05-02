#include <bits/stdc++.h>
using namespace std;


struct Point {
    double x, y;
    int idx;  // original index
};


double cross(const Point &O, const Point &A, const Point &B) {
    return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}


// Euclidean distance
double dist(const Point &A, const Point &B) {
    return hypot(A.x - B.x, A.y - B.y);
}


// 1) Convex Hull: Andrew’s monotone chain, CCW order
vector<Point> convexHull(vector<Point> pts) {
    sort(pts.begin(), pts.end(),
         [](const Point &a, const Point &b){
           return a.x < b.x || (a.x==b.x && a.y < b.y);
         });
    int n = pts.size(), k = 0;
    if (n <= 1) return pts;
    vector<Point> H(2*n);
    // Lower hull
    for (int i = 0; i < n; ++i) {
        while (k >= 2 && cross(H[k-2], H[k-1], pts[i]) <= 0) k--;
        H[k++] = pts[i];
    }
    // Upper hull
    for (int i = n-2, t = k+1; i >= 0; --i) {
        while (k >= t && cross(H[k-2], H[k-1], pts[i]) <= 0) k--;
        H[k++] = pts[i];
    }
    H.resize(k-1);
    return H;
}


// 2) DP on convex polygon to get minimum-weight triangles
void triangulateHullDP(const vector<Point> &hull,
                       vector<array<int,3>> &tris)
{
    int m = hull.size();
    vector<vector<double>> dp(m, vector<double>(m, 0));
    vector<vector<int>> root(m, vector<int>(m, -1));


    auto cost = [&](int i, int j, int k) {
        return dist(hull[i], hull[j])
             + dist(hull[j], hull[k])
             + dist(hull[k], hull[i]);
    };


    // Fill DP by increasing gap
    for (int gap = 2; gap < m; ++gap) {
        for (int i = 0; i + gap < m; ++i) {
            int j = i + gap;
            dp[i][j] = 1e18;
            for (int k = i + 1; k < j; ++k) {
                double c = dp[i][k] + dp[k][j] + cost(i,j,k);
                if (c < dp[i][j]) {
                    dp[i][j] = c;
                    root[i][j] = k;
                }
            }
        }
    }


    // Reconstruct the triangulation
    function<void(int, int)> build = [&](int i, int j) {
        if (j <= i + 1) return;
        int k = root[i][j];
        tris.push_back({ hull[i].idx, hull[k].idx, hull[j].idx });
        build(i, k);
        build(k, j);
    };
    build(0, m-1);
}


// 3) Insert interior points by triangle splitting
bool pointInTriangle(const Point &p, const Point &A, const Point &B, const Point &C) {
    return cross(A, B, p) > 0 && cross(B, C, p) > 0 && cross(C, A, p) > 0;
}


void insertInteriorPoints(const vector<Point> &interior, vector<array<int,3>> &tris, const vector<Point> &allPts)
{
    for (auto &p : interior) {
        bool done = false;
        for (size_t t = 0; t < tris.size(); ++t) {
            auto tri = tris[t];
            const Point &A = allPts[tri[0]];
            const Point &B = allPts[tri[1]];
            const Point &C = allPts[tri[2]];
            if (pointInTriangle(p, A, B, C)) {
                tris.erase(tris.begin() + t);
                tris.insert(tris.begin() + t, {A.idx, B.idx, p.idx});
                tris.insert(tris.begin() + t + 1, {B.idx, C.idx, p.idx});
                tris.insert(tris.begin() + t + 2, {C.idx, A.idx, p.idx});
                done = true;
                break;
            }
        }
        if (!done) {
            cerr << "Error: no triangle found for point " << p.idx << "\n";
        }
    }
}


int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);


    int n;
    cin >> n;
    vector<Point> pts(n);
    for (int i = 0; i < n; i++) {
        cin >> pts[i].x >> pts[i].y;
        pts[i].idx = i;
    }


    // Step 1: Compute the convex hull
    auto hull = convexHull(pts);
    unordered_set<int> onH;
    for (auto &h : hull) onH.insert(h.idx);
    vector<Point> interior;
    for (auto &p : pts) if (!onH.count(p.idx)) interior.push_back(p);


    // Step 2: Triangulate hull by DP
    vector<array<int, 3>> triangles;
    triangulateHullDP(hull, triangles);


    // Step 3: Insert interior points
    insertInteriorPoints(interior, triangles, pts);


    // Step 4: Calculate the weight of the convex hull
    double convexHullWeight = 0.0;
    for (size_t i = 0; i < hull.size(); ++i) {
        const Point &A = hull[i];
        const Point &B = hull[(i + 1) % hull.size()];
        convexHullWeight += dist(A, B);
    }


    // Step 5: Calculate total weight (perimeter) of all triangles
    double totalWeight = 0.0;
    for (const auto &t : triangles) {
        const Point &A = pts[t[0]];
        const Point &B = pts[t[1]];
        const Point &C = pts[t[2]];
        totalWeight += dist(A, B) + dist(B, C) + dist(C, A);
    }


    // Step 6: Adjust the total weight
    totalWeight -= convexHullWeight;  // Subtract convex hull weight
    totalWeight /= 2.0;               // Divide by 2 to account for double counting
    totalWeight += convexHullWeight;  // Add back the convex hull weight


    // Output the total weight and the individual triangles
    cout << "Total weight: " << fixed << setprecision(10) << totalWeight << "\n";
    cout << "Triangles:\n";
    for (const auto &t : triangles) {
        cout << t[0] << " " << t[1] << " " << t[2] << "\n";
    }


    return 0;
}