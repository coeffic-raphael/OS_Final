#ifndef TREE_HPP
#define TREE_HPP

#include "GraphBase.hpp"
#include <queue>
#include <limits>
#include <cmath>
#include <numeric>

template<typename T>
class Tree : public GraphBase<T> {
private:
    T totalWeight;  // Total weight of the tree

public:
    // Constructor to initialize a tree with 'V' vertices
    Tree(int V) : GraphBase<T>(V), totalWeight(0) {}

    // Add an edge to the tree and update the total weight
    void addEdge(int u, int v, T weight) override {
        checkVertex(u);  // Ensure the vertex 'u' is valid
        checkVertex(v);  // Ensure the vertex 'v' is valid
        this->adj[u].push_back(std::make_pair(v, weight)); // u -> v
        this->adj[v].push_back(std::make_pair(u, weight)); // v -> u (undirected)
        totalWeight += weight;  // Update total weight
    }

    // Remove an edge from the tree and update the total weight
    void removeEdge(int u, int v) override {
        checkVertex(u);  // Ensure the vertex 'u' is valid
        checkVertex(v);  // Ensure the vertex 'v' is valid
        bool found = false;
        // Remove the edge u -> v
        for (auto it = this->adj[u].begin(); it != this->adj[u].end(); ++it) {
            if (it->first == v) {
                totalWeight -= it->second;  // Update total weight
                this->adj[u].erase(it);  // Remove the edge
                found = true;
                break;
            }
        }
        if (found) {
            // Remove the edge v -> u
            for (auto it = this->adj[v].begin(); it != this->adj[v].end(); ++it) {
                if (it->first == u) {
                    this->adj[v].erase(it);  // Remove the reverse edge
                    break;
                }
            }
        }
    }

    // Print the tree
    void print() const override {
        for (size_t u = 0; u < this->adj.size(); ++u) {
            std::cout << "Vertex " << u << " is connected to:";
            for (auto [v, weight] : this->adj[u]) {
                std::cout << " " << v << " (Weight " << weight << ")";
            }
            std::cout << std::endl;
        }
        std::cout << "Total weight of the tree: " << totalWeight << std::endl;
    }

    // Get the total weight of the MST
    T getTotalWeight() const {
        return totalWeight;
    }

    // Find the longest distance between two vertices (tree diameter)
    T findLongestDistance() const {
        int farthestNode = 0;
        T maxDistance = 0;

        // First BFS to find the farthest node from vertex 0
        BFS(0, farthestNode, maxDistance);

        // Second BFS starting from the farthest node found
        maxDistance = 0;  // Reset max distance
        BFS(farthestNode, farthestNode, maxDistance);

        return maxDistance;
    }

    // Calculate the average distance between all pairs of vertices
    double findAverageDistance() const {
        int V = this->adj.size();
        T totalDistance = 0;
        int count = 0;

        // Perform BFS for each vertex and calculate distances
        for (int u = 0; u < V; ++u) {
            std::vector<T> distances = BFS(u);
            for (int v = u + 1; v < V; ++v) {
                totalDistance += distances[v];
                count++;
            }
        }

        if (count == 0) return 0;
        return static_cast<double>(totalDistance) / count;
    }

    // Find the shortest distance between two distinct vertices
    T findShortestDistance() const {
        T minDistance = std::numeric_limits<T>::max();
        int V = this->adj.size();

        // Traverse all edges and find the minimum weight
        for (int u = 0; u < V; ++u) {
            for (auto [v, weight] : this->adj[u]) {
                if (weight < minDistance) {
                    minDistance = weight;
                }
            }
        }

        return minDistance;
    }

private:
    // Check if the vertex index is valid
    void checkVertex(int v) const {
        if (v < 0 || v >= static_cast<int>(this->adj.size())) {
            throw std::out_of_range("Invalid vertex index");
        }
    }

    // Breadth-First Search (BFS) to find distances from a source vertex
    std::vector<T> BFS(int src) const {
        int V = this->adj.size();
        std::vector<T> dist(V, std::numeric_limits<T>::max());  // Distance vector initialized to max
        std::queue<int> q;

        dist[src] = 0;  // Distance from source to itself is 0
        q.push(src);

        // BFS traversal
        while (!q.empty()) {
            int u = q.front();
            q.pop();

            for (auto [v, weight] : this->adj[u]) {
                if (dist[v] == std::numeric_limits<T>::max()) {  // If not visited
                    dist[v] = dist[u] + weight;  // Update distance
                    q.push(v);  // Visit neighbor
                }
            }
        }

        return dist;  // Return distance array
    }

    // BFS to find the farthest node and the maximum distance
    void BFS(int src, int& farthestNode, T& maxDistance) const {
        int V = this->adj.size();
        std::vector<T> dist(V, std::numeric_limits<T>::max());  // Distance vector initialized to max
        std::queue<int> q;

        dist[src] = 0;  // Distance from source to itself is 0
        q.push(src);

        // BFS traversal
        while (!q.empty()) {
            int u = q.front();
            q.pop();

            for (auto [v, weight] : this->adj[u]) {
                if (dist[v] == std::numeric_limits<T>::max()) {  // If not visited
                    dist[v] = dist[u] + weight;  // Update distance
                    if (dist[v] > maxDistance) {  // Track the maximum distance
                        maxDistance = dist[v];
                        farthestNode = v;  // Update the farthest node
                    }
                    q.push(v);  // Visit neighbor
                }
            }
        }
    }
};

#endif // TREE_HPP