#ifndef KRUSKAL_HPP
#define KRUSKAL_HPP

#include "MSTAlgos.hpp"
#include <vector>
#include <algorithm>
#include <numeric>

template<typename T>
class Kruskal : public MSTAlgos<T> {
private:
    // Find function for the Union-Find (Disjoint Set) system with path compression
    int find(int u, std::vector<int>& parent) {
        if (u != parent[u]) {
            // Recursively find the root and compress the path
            parent[u] = find(parent[u], parent);
        }
        return parent[u];
    }

    // Union function for the Union-Find system with union by rank
    void unite(int u, int v, std::vector<int>& parent, std::vector<int>& rank) {
        int rootU = find(u, parent);
        int rootV = find(v, parent);
        if (rootU != rootV) {
            // Union by rank: attach the smaller tree to the larger tree
            if (rank[rootU] > rank[rootV]) {
                parent[rootV] = rootU;
            } else if (rank[rootU] < rank[rootV]) {
                parent[rootU] = rootV;
            } else {
                parent[rootV] = rootU;
                rank[rootU]++;
            }
        }
    }

public:
    // Implementation of Kruskal's algorithm to solve the MST problem
    Tree<T> solveMST(const Graph<T>& graph) override {
        int V = graph.getAdjacencyList().size(); // Number of vertices
        Tree<T> mst(V); // Initialize an empty MST
        std::vector<std::tuple<T, int, int>> edges;

        // Collect all edges from the graph
        for (int u = 0; u < V; ++u) {
            for (auto [v, weight] : graph.getAdjacencyList()[u]) {
                if (u < v) {
                    edges.push_back({weight, u, v}); // Only add one direction for undirected edges
                }
            }
        }

        // Sort the edges by weight (ascending)
        std::sort(edges.begin(), edges.end());

        // Initialize the Union-Find structure
        std::vector<int> parent(V), rank(V, 0);
        std::iota(parent.begin(), parent.end(), 0); // Set parent[i] = i (each node is its own parent initially)

        // Traverse the sorted edges
        for (auto [weight, u, v] : edges) {
            // If u and v are not already connected, add the edge to the MST
            if (find(u, parent) != find(v, parent)) {
                mst.addEdge(u, v, weight); // Add the edge to the MST
                unite(u, v, parent, rank); // Union the sets containing u and v
            }
        }

        return mst; // Return the final MST
    }
};

#endif // KRUSKAL_HPP
