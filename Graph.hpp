
#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "GraphBase.hpp"
#include <stdexcept>

template<typename T>
class Graph : public GraphBase<T> {
public:
    // Constructor to initialize a graph with 'V' vertices
    Graph(int V) : GraphBase<T>(V) {}

    // Add an edge to the graph (undirected)
    void addEdge(int u, int v, T weight) override {
        checkVertex(u); // Ensure the vertex 'u' is valid
        checkVertex(v); // Ensure the vertex 'v' is valid
        this->adj[u].push_back(std::make_pair(v, weight)); // u -> v
        this->adj[v].push_back(std::make_pair(u, weight)); // v -> u (undirected)
    }

    // Remove an edge from the graph (undirected)
    void removeEdge(int u, int v) override {
        checkVertex(u); // Ensure the vertex 'u' is valid
        checkVertex(v); // Ensure the vertex 'v' is valid
        // Remove the edge u -> v
        this->adj[u].remove_if([v](const std::pair<int, T>& edge) {
            return edge.first == v;
        });
        // Remove the edge v -> u (since it's undirected)
        this->adj[v].remove_if([u](const std::pair<int, T>& edge) {
            return edge.first == u;
        });
    }

    // Print the graph (list of vertices and their connections)
    void print() const override {
        for (size_t u = 0; u < this->adj.size(); ++u) {
            std::cout << "Vertex " << u << " is connected to :";
            for (auto [v, weight] : this->adj[u]) {
                std::cout << " (Vertex " << v << ", Weight " << weight << ")";
            }
            std::cout << std::endl;
        }
    }

private:
    // Check if the vertex index is valid
    void checkVertex(int v) const {
        if (v < 0 || v >= static_cast<int>(this->adj.size())) {
            throw std::out_of_range("Invalid vertex index");
        }
    }
};

#endif // GRAPH_HPP
