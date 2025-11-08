#ifndef ACO_H
#define ACO_H

#include <vector>
#include <utility>

struct Point {
    double x, y;
};

// Ejecuta el algoritmo ACO sobre una lista de puntos 2D
std::pair<std::vector<int>, double> ant_colony_optimization(
    const std::vector<Point>& points,
    int n_ants = 5,
    int n_iterations = 30,
    double alpha = 1.0,
    double beta = 2.0,
    double evaporation_rate = 0.5,
    double Q = 100.0,
    int patience = 3
);

#endif // ACO_H