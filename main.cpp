#include "ACO.h"
#include <iostream>
#include <random>
#include <cmath>
#include <vector>
using namespace std;

int main(int argc, char* argv[]) {
    // Identificador del caso de prueba (1, 2 o 3)
    int case_id = 1;
    if (argc > 1) case_id = atoi(argv[1]);

    // Vector que almacenará las coordenadas de todas las ciudades
    vector<Point> cities;
    // Generador de números aleatorios con semilla fija para reproducibilidad
    mt19937 gen(42);
    // Distribución uniforme para generar coordenadas aleatorias [0.0, 10.0]
    uniform_real_distribution<> dis(0.0, 10.0);

    cout << "=== CASO " << case_id << " ===" << endl;

    if (case_id == 1) {
        // ----- Caso 1: 10 ciudades en posiciones irregulares -----
        // Configuración manual de ciudades para probar el algoritmo en un escenario controlado
        cities = {
            {0,0}, {2,1}, {3,5}, {5,4}, {6,1},
        {1,3}, {4,6}, {7,3}, {2,6}, {5,0}
        };
    } 
    else if (case_id == 2) {
        // ----- Caso 2: 20 ciudades aleatorias -----
        // Genera ciudades con distribución uniforme en el espacio [0,10] × [0,10]
        for (int i = 0; i < 20; ++i)
            cities.push_back({dis(gen), dis(gen)});
    } 
    else if (case_id == 3) {
        // ----- Caso 3: 2 clústeres de 15 ciudades -----
        // Distribución normal centrada en (0,0) con desviación estándar 2.5
        normal_distribution<> clusterA(0.0, 2.5);
        // Distribución normal centrada en (15,15) con desviación estándar 2.5
        normal_distribution<> clusterB(15.0, 2.5);
        // Generar primer clúster (15 ciudades alrededor de origen)
        for (int i = 0; i < 15; ++i)
            cities.push_back({clusterA(gen), clusterA(gen)});
        // Generar segundo clúster (15 ciudades alrededor de (15,15))
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
    int patience;        // Iteraciones sin mejora antes de detener (early stopping)
    double alpha;        // Exponente de importancia de la feromona (τ^α)
    double beta;         // Exponente de importancia de la heurística (η^β)
    double evaporation;  // Tasa de evaporación de feromonas (ρ)
    double Q;            // Constante para calcular depósito de feromonas (Q/L)

    if (case_id == 1) {
        // Caso pequeño: menos hormigas, menos iteraciones, beta moderado
        n_ants = 5;
        n_iterations = 30;
        alpha = 1.0;
        beta = 3.0;
        evaporation = 0.5;
        Q = 100.0;
        patience = 5;
    }
    else if (case_id == 2) {
        // Caso mediano: más hormigas, más iteraciones, beta alto para preferir distancias cortas
        n_ants = 10;
        n_iterations = 60;
        alpha = 1.0;
        beta = 5.0;
        evaporation = 0.5;
        Q = 100.0;
        patience = 7;
    }
    else { // caso 3
        // Caso grande con estructura: muchas hormigas, beta muy alto, mayor depósito de feromona
        n_ants = 15;
        n_iterations = 80;
        alpha = 1.0;
        beta = 7.0;          // Beta alto explota la estructura de clústeres
        evaporation = 0.4;    // Menor evaporación para mantener más memoria
        Q = 200.0;            // Mayor Q para reforzar buenos caminos
        patience = 10;
    }

    // Ejecutar el algoritmo de optimización de colonia de hormigas
    // Retorna: mejor camino encontrado (vector de índices) y su longitud total
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
