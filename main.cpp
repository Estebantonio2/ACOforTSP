#include "ACO.h"
#include <iostream>
#include <random>
#include <cmath>
#include <vector>
using namespace std;

int main(int argc, char* argv[]) {
    int case_id = 1;
    if (argc > 1) case_id = atoi(argv[1]);

    vector<Point> cities;
    mt19937 gen(42);
    uniform_real_distribution<> dis(0.0, 10.0);

    cout << "=== CASO " << case_id << " ===" << endl;

    if (case_id == 1) {
        // ----- Caso 1: 10 ciudades en posiciones irregulares -----
        cities = {
            {0,0}, {2,1}, {3,5}, {5,4}, {6,1},
        {1,3}, {4,6}, {7,3}, {2,6}, {5,0}
        };
    } 
    else if (case_id == 2) {
        // ----- Caso 2: 20 ciudades aleatorias -----
        for (int i = 0; i < 20; ++i)
            cities.push_back({dis(gen), dis(gen)});
    } 
    else if (case_id == 3) {
        // ----- Caso 3: 2 clústeres de 15 ciudades -----
        normal_distribution<> clusterA(0.0, 2.5);
        normal_distribution<> clusterB(15.0, 2.5);
        for (int i = 0; i < 15; ++i)
            cities.push_back({clusterA(gen), clusterA(gen)});
        for (int i = 0; i < 15; ++i)
            cities.push_back({clusterB(gen), clusterB(gen)});
    } 
    else {
        cout << "Caso no válido." << endl;
        return 1;
    }

    cout << "Número de ciudades: " << cities.size() << endl;

    // --- Parámetros por caso ---
    int n_ants, n_iterations, patience;
    double alpha, beta, evaporation, Q;

    if (case_id == 1) {
        n_ants = 5;
        n_iterations = 30;
        alpha = 1.0;
        beta = 3.0;
        evaporation = 0.5;
        Q = 100.0;
        patience = 5;
    }
    else if (case_id == 2) {
        n_ants = 10;
        n_iterations = 60;
        alpha = 1.0;
        beta = 5.0;
        evaporation = 0.5;
        Q = 100.0;
        patience = 7;
    }
    else { // caso 3
        n_ants = 15;
        n_iterations = 80;
        alpha = 1.0;
        beta = 7.0;
        evaporation = 0.4;
        Q = 200.0;
        patience = 10;
    }

    auto [best_path, best_length] = ant_colony_optimization(
        cities,
        n_ants,
        n_iterations,
        alpha,
        beta,
        evaporation,
        Q,
        patience
    );

    cout << "\nMejor longitud final: " << best_length << endl;
    return 0;
}
