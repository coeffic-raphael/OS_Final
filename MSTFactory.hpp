#ifndef MSTFACTORY_HPP
#define MSTFACTORY_HPP

#include "MSTAlgos.hpp"
#include "Prim.hpp"
#include "Kruskal.hpp"
#include <memory>
#include <string>
#include <stdexcept>
#include <algorithm>

template<typename T>
class MSTFactory {
public:
    // Static method to create the appropriate MST solver
    static std::unique_ptr<MSTAlgos<T>> createSolver(const std::string& algorithm) {
        std::string alg = algorithm;
        // Convert the algorithm name to lowercase for case-insensitive comparison
        std::transform(alg.begin(), alg.end(), alg.begin(), ::tolower);

        // Return the appropriate MST algorithm based on the input
        if (alg == "prim") {
            return std::make_unique<Prim<T>>(); // Return Prim's algorithm solver
        } else if (alg == "kruskal") {
            return std::make_unique<Kruskal<T>>(); // Return Kruskal's algorithm solver
        }
        // Throw an error if the algorithm is not supported
        throw std::invalid_argument("Unsupported MST algorithm: " + algorithm);
    }
};

#endif // MSTFACTORY_HPP