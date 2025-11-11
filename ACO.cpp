#include "ACO.h"
#include <iostream>
#include <cmath>
#include <random>
#include <limits>
#include <iomanip>
#include <algorithm>

using namespace std;

static double distance(const Point& a, const Point& b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

pair<vector<int>, double> ant_colony_optimization(
    const vector<Point>& cities,
    int n_ants,
    int n_iterations,
    double alpha,
    double beta,
    double rho,
    double Q,
    int patience
) {
    // --- Imprimir parámetros iniciales ---
    cout << "Parametros ACO:\n"
         << "  Hormigas: " << n_ants << "\n"
         << "  Iteraciones: " << n_iterations << "\n"
         << "  Alpha: " << alpha << "\n"
         << "  Beta: " << beta << "\n"
         << "  Tasa de evaporacion: " << rho << "\n"
         << "  Q: " << Q << "\n"
         << "  Paciencia: " << patience << "\n\n";

    int n_cities = cities.size(); // Número total de ciudades en el problema TSP

    vector<vector<double>> dist(n_cities, vector<double>(n_cities, 0.0));

    vector<vector<double>> vis(n_cities, vector<double>(n_cities, 0.0));
    for (int i = 0; i < n_cities; ++i)
        for (int j = 0; j < n_cities; ++j)
            if (i != j) {
                dist[i][j] = distance(cities[i], cities[j]);
                vis[i][j] = 1.0 / (dist[i][j] + 1e-9); // +1e-9 evita división por cero
            }

    vector<vector<double>> pher(n_cities, vector<double>(n_cities, 1.0));

    vector<int> best_path;

    double best_len = numeric_limits<double>::infinity();

    mt19937 gen(42);

    uniform_real_distribution<> uniform(0.0, 1.0);

    double last_best = numeric_limits<double>::infinity();

    int no_improve = 0;

    for (int iter = 0; iter < n_iterations; ++iter) {
        vector<vector<int>> paths;
        vector<double> lengths;

        for (int ant = 0; ant < n_ants; ++ant) {
            vector<bool> visited(n_cities, false);

            // Ciudad inicial aleatoria para esta hormiga
            int current = gen() % n_cities;

            vector<int> path = {current};
            visited[current] = true;

            // Construcción de la ruta
            for (int step = 0; step < n_cities - 1; ++step) {
                vector<int> unvisited;
                for (int i = 0; i < n_cities; ++i)
                    if (!visited[i]) unvisited.push_back(i);

                // Probabilidades de transición a cada ciudad no visitada
                vector<double> probs;
                // Suma total de probabilidades
                double sum_p = 0.0;
                for (int j : unvisited) {
                    // Fórmula: P(i→j) ∝ τ(i,j)^α × η(i,j)^β
                    double p = pow(pher[current][j], alpha) * pow(vis[current][j], beta);
                    probs.push_back(p);
                    sum_p += p;
                }

                // Normalizar probabilidades para que sumen 1.0
                for (double& p : probs) p /= sum_p;

                // Selección de la siguiente ciudad mediante ruleta
                double r = uniform(gen); 
                double cum = 0.0; 
                int next_city = unvisited.back(); // Ciudad por defecto
                for (size_t i = 0; i < probs.size(); ++i) {
                    cum += probs[i];
                    if (r <= cum) {
                        next_city = unvisited[i];
                        break;
                    }
                }

                path.push_back(next_city);
                visited[next_city] = true;
                current = next_city;
            }

            // --- Calcular longitud total ---
            double length = 0.0;
            for (int i = 0; i < n_cities; ++i)
                length += dist[path[i]][path[(i + 1) % n_cities]]; // % cierra el ciclo

            paths.push_back(path);
            lengths.push_back(length);

            // Actualizar mejor solución global si esta hormiga encontró una mejor
            if (length < best_len) {
                best_len = length;
                best_path = path;
            }
        }

        // --- Estadísticas ---
        double avg_len = 0.0;
        for (double L : lengths) avg_len += L;
        avg_len /= n_ants;

        // --- Evaporación de feromonas ---
        // Fórmula: τ(i,j) ← (1-ρ) × τ(i,j)
        for (int i = 0; i < n_cities; ++i)
            for (int j = 0; j < n_cities; ++j)
                pher[i][j] *= (1.0 - rho);

        // --- Depositar feromonas ---
        // Cada hormiga deposita feromona en su ruta: Δτ = Q/L
        for (size_t k = 0; k < paths.size(); ++k) {
            double deposit = Q / lengths[k]; 
            const auto& path = paths[k];
            for (int i = 0; i < n_cities; ++i) {
                int a = path[i];
                int b = path[(i + 1) % n_cities];
                pher[a][b] += deposit;
                pher[b][a] += deposit;
            }
        }

        // --- Calcular promedio de feromonas en la mejor ruta ---
        double sum_best_pher = 0.0;
        for (int i = 0; i < (int)best_path.size(); ++i) {
            int a = best_path[i];
            int b = best_path[(i + 1) % n_cities];
            sum_best_pher += pher[a][b];
        }
        double avg_best_pher = sum_best_pher / best_path.size();

        // --- Mostrar las 3 métricas ---
        cout << "Iteracion " << setw(3) << iter + 1
             << " | Mejor global: " << fixed << setprecision(3) << best_len
             << " | Promedio: " << avg_len
             << " | Feromonas promedio (mejor ruta): " << setprecision(4) << avg_best_pher
             << endl;

        // --- EARLY STOPPING ---
        if (best_len < last_best - 1e-6) {
            last_best = best_len;
            no_improve = 0; 
        } else {
            ++no_improve; 
        }

        if (no_improve >= patience) {
            cout << "\nConvergencia alcanzada en iteracion " << iter + 1
                 << " (sin mejora por " << patience << " iteraciones)\n";
            break;
        }
    }

    return {best_path, best_len};
}
