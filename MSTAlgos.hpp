#ifndef MSTALGOS_HPP
#define MSTALGOS_HPP

#include "Graph.hpp"
#include "Tree.hpp"

template<typename T>
class MSTAlgos {
public:
    // Abstract method to solve the Minimum Spanning Tree (MST) problem
    virtual Tree<T> solveMST(const Graph<T>& graph) = 0;

    // Virtual destructor (default implementation)
    virtual ~MSTAlgos() = default;
};

#endif // MSTALGOS_HPP