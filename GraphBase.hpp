#ifndef GRAPHBASE_HPP
#define GRAPHBASE_HPP

#include <vector>
#include <list>
#include <utility>
#include <iostream>
#include <stdexcept>

template<typename T>
class GraphBase {
protected:
    // Adjacency list where each vertex is associated with a list of pairs (neighbor, weight)
    std::vector<std::list<std::pair<int, T>>> adj; // Adjacency list with weights of generic type T

public:
    // Constructor to initialize a graph with 'V' vertices
    GraphBase(int V) {
        adj.resize(V); // Resize the adjacency list to hold V vertices
    }

    // Method to access the adjacency list
    const std::vector<std::list<std::pair<int, T>>>& getAdjacencyList() const {
        return adj;
    }

    // Pure virtual method to add an edge (must be implemented in derived classes)
    virtual void addEdge(int u, int v, T weight) = 0;

    // Pure virtual method to remove an edge (must be implemented in derived classes)
    virtual void removeEdge(int u, int v) = 0;

    // Pure virtual method to print the graph (must be implemented in derived classes)
    virtual void print() const = 0;

    // Virtual destructor (default implementation)
    virtual ~GraphBase() = default;
};

#endif // GRAPHBASE_HPP