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
    int n_ants;          // Número de hormigas que explorarán en paralelo
    int n_iterations;    // Número máximo de iteraciones del algoritmo
    int k;               // Iteraciones sin mejora antes de detener (early stopping)
    double alpha;        // Exponente de importancia de la feromona (τ^α)
    double beta;         // Exponente de importancia de la heurística (η^β)
    double rho;          // Tasa de evaporación de feromonas (ρ)
    double Q;            // Constante para calcular depósito de feromonas (Q/L)

    if (case_id == 1) {
        // Caso pequeño: menos hormigas, menos iteraciones, beta moderado
        n_ants = 5;
        n_iterations = 30;
        alpha = 1.0;
        beta = 3.0;
        rho = 0.5;
        Q = 100.0;
        k = 3;
    }
    else if (case_id == 2) {
        // Caso mediano: más hormigas, más iteraciones, beta alto para preferir distancias cortas
        n_ants = 10;
        n_iterations = 60;
        alpha = 1.0;
        beta = 5.0;
        rho = 0.5;
        Q = 100.0;
        k = 4;
    }
    else {
        // Caso grande con estructura: muchas hormigas, beta muy alto, mayor depósito de feromona
        n_ants = 15;
        n_iterations = 80;
        alpha = 1.0;
        beta = 7.0;         
        rho = 0.4;   
        Q = 200.0;          
        k = 5;
    }

    auto [best_path, best_length] = ant_colony_optimization(
        cities,
        n_ants,
        n_iterations,
        alpha,
        beta,
        rho,
        Q,
        k
    );

    cout << "\nMejor longitud final: " << best_length << endl;
    return 0;
}
