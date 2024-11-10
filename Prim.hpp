#ifndef PRIM_HPP
#define PRIM_HPP

#include "MSTAlgos.hpp"
#include <vector>
#include <queue>
#include <limits>
#include <functional>
#include <algorithm>

template<typename T>
class Prim : public MSTAlgos<T> {
public:
    // Implementation of Prim's algorithm to solve the Minimum Spanning Tree (MST) problem
    Tree<T> solveMST(const Graph<T>& graph) override {
        int V = graph.getAdjacencyList().size(); // Number of vertices
        Tree<T> mst(V);  // Create an empty Minimum Spanning Tree (MST)
        std::vector<T> key(V, std::numeric_limits<T>::max());  // Key values to pick the minimum weight edge
        std::vector<int> parent(V, -1);  // To store the parent vertices of the MST
        std::vector<bool> inMST(V, false);  // To check if a vertex is included in the MST

        // Priority queue to select the edge with the minimum weight
        using pii = std::pair<T, int>;
        std::priority_queue<pii, std::vector<pii>, std::greater<pii>> pq;

        // Loop through all vertices to handle disconnected graphs
        for (int startVertex = 0; startVertex < V; ++startVertex) {
            if (!inMST[startVertex]) {
                // Initialize the starting vertex
                key[startVertex] = 0;
                pq.push({0, startVertex});

                while (!pq.empty()) {
                    int u = pq.top().second;  // Select the vertex with the minimum weight
                    pq.pop();

                    // Skip if the vertex is already included in the MST
                    if (inMST[u]) continue;

                    // Mark this vertex as part of the MST
                    inMST[u] = true;

                    // Add the edge to the MST if it's not the starting vertex
                    if (parent[u] != -1) {
                        mst.addEdge(parent[u], u, key[u]);
                    }

                    // Traverse the neighbors of vertex u
                    for (auto [v, weight] : graph.getAdjacencyList()[u]) {
                        // If v is not yet in the MST and the edge u-v has a smaller weight than the current key value for v
                        if (!inMST[v] && weight < key[v]) {
                            key[v] = weight;  // Update the key with the minimum weight
                            pq.push({key[v], v});  // Add the neighbor to the priority queue
                            parent[v] = u;  // Update the parent of vertex v
                        }
                    }
                }
            }
        }

        return mst;  // Return the constructed Minimum Spanning Tree (MST)
    }
};

#endif // PRIM_HPP