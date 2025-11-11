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
    int k
) {
    // --- Imprimir parámetros iniciales ---
    cout << "Parametros ACO:\n"
         << "  Hormigas: " << n_ants << "\n"
         << "  Iteraciones: " << n_iterations << "\n"
         << "  Alpha: " << alpha << "\n"
         << "  Beta: " << beta << "\n"
         << "  Rho: " << rho << "\n"
         << "  Q: " << Q << "\n"
         << "  k: " << k << "\n\n";

    int n_cities = cities.size(); // Número total de ciudades en el problema TSP

    // --- Precalcular distancias y visibilidad ---
    // Matriz de distancias euclidianas entre cada par de ciudades (i,j)
    vector<vector<double>> dist(n_cities, vector<double>(n_cities, 0.0));
    // Matriz de visibilidad (heurística): η(i,j) = 1/distancia(i,j)
    // Ciudades más cercanas tienen mayor visibilidad
    vector<vector<double>> vis(n_cities, vector<double>(n_cities, 0.0));
    for (int i = 0; i < n_cities; ++i)
        for (int j = 0; j < n_cities; ++j)
            if (i != j) {
                dist[i][j] = distance(cities[i], cities[j]);
                vis[i][j] = 1.0 / (dist[i][j] + 1e-9); // +1e-9 evita división por cero
            }

    // --- Inicialización de feromonas ---
    // Matriz de feromonas τ(i,j): inicializada en 1.0 para todas las aristas
    // Representa el rastro de feromona depositado entre cada par de ciudades
    vector<vector<double>> pher(n_cities, vector<double>(n_cities, 1.0));

    // Mejor camino encontrado globalmente (secuencia de índices de ciudades)
    vector<int> best_path;
    // Longitud del mejor camino encontrado (inicializado a infinito)
    double best_len = numeric_limits<double>::infinity();

    // Generador de números aleatorios con semilla fija para reproducibilidad
    mt19937 gen(42);
    // Distribución uniforme [0,1] para selección probabilística (ruleta)
    uniform_real_distribution<> uniform(0.0, 1.0);

    // Última mejor longitud registrada (para detectar mejoras)
    double last_best = numeric_limits<double>::infinity();
    // Contador de iteraciones consecutivas sin mejora (early stopping)
    int no_improve = 0;

    for (int iter = 0; iter < n_iterations; ++iter) {
        // Almacena las rutas construidas por todas las hormigas en esta iteración
        vector<vector<int>> paths;
        // Almacena las longitudes correspondientes a cada ruta
        vector<double> lengths;

        // --- Cada hormiga construye una ruta ---
        for (int ant = 0; ant < n_ants; ++ant) {
            // Marca las ciudades ya visitadas por esta hormiga
            vector<bool> visited(n_cities, false);
            // Ciudad inicial aleatoria para esta hormiga
            int current = gen() % n_cities;
            // Camino que construirá la hormiga (secuencia de ciudades)
            vector<int> path = {current};
            visited[current] = true;

            // Construcción de la ruta
            for (int step = 0; step < n_cities - 1; ++step) {
                // Lista de ciudades aún no visitadas por esta hormiga
                vector<int> unvisited;
                for (int i = 0; i < n_cities; ++i)
                    if (!visited[i]) unvisited.push_back(i);

                // Probabilidades de transición a cada ciudad no visitada
                vector<double> probs;
                // Suma total de probabilidades (para normalización)
                double sum_p = 0.0;
                for (int j : unvisited) {
                    // Fórmula: P(i→j) ∝ τ(i,j)^α × η(i,j)^β
                    // α controla la importancia de la feromona
                    // β controla la importancia de la heurística (distancia)
                    double p = pow(pher[current][j], alpha) * pow(vis[current][j], beta);
                    probs.push_back(p);
                    sum_p += p;
                }

                // Normalizar probabilidades para que sumen 1.0
                for (double& p : probs) p /= sum_p;

                // Selección de la siguiente ciudad mediante ruleta
                double r = uniform(gen); // Número aleatorio [0,1]
                double cum = 0.0; // Probabilidad acumulada
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
            // Suma de distancias: ciudad[0]→ciudad[1]→...→ciudad[n-1]→ciudad[0]
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
        // Calcular longitud promedio de todas las rutas en esta iteración
        double avg_len = 0.0;
        for (double L : lengths) avg_len += L;
        avg_len /= n_ants;

        // --- Evaporación de feromonas ---
        // Fórmula: τ(i,j) ← (1-ρ) × τ(i,j)
        // Reduce gradualmente todas las feromonas para olvidar soluciones antiguas
        for (int i = 0; i < n_cities; ++i)
            for (int j = 0; j < n_cities; ++j)
                pher[i][j] *= (1.0 - rho);

        // --- Depositar feromonas ---
        // Cada hormiga deposita feromona en su ruta: Δτ = Q/L
        // Rutas más cortas depositan más feromona (refuerzo positivo)
        for (size_t l = 0; l < paths.size(); ++l) {
            double deposit = Q / lengths[l]; // Cantidad inversamente proporcional a la longitud
            const auto& path = paths[l];
            for (int i = 0; i < n_cities; ++i) {
                int a = path[i];
                int b = path[(i + 1) % n_cities];
                pher[a][b] += deposit; // Grafo no dirigido: actualizar ambas direcciones
                pher[b][a] += deposit;
            }
        }

        // --- Calcular promedio de feromonas en la mejor ruta ---
        // Métrica de convergencia: indica cuánta feromona se acumula en el mejor camino
        double sum_best_pher = 0.0;
        for (int i = 0; i < (int)best_path.size(); ++i) {
            int a = best_path[i];
            int b = best_path[(i + 1) % n_cities];
            sum_best_pher += pher[a][b];
        }
        double avg_best_pher = sum_best_pher / best_path.size();

        // --- Mostrar solo las 3 métricas ---
        cout << "Iteracion " << setw(3) << iter + 1
             << " | Mejor global: " << fixed << setprecision(3) << best_len
             << " | Promedio: " << avg_len
             << " | Feromonas promedio (mejor ruta): " << setprecision(4) << avg_best_pher
             << endl;

        // --- EARLY STOPPING ---
        // Detectar convergencia: si no hay mejora significativa, terminar anticipadamente
        if (best_len < last_best - 1e-6) { // Mejora detectada (tolerancia numérica)
            last_best = best_len;
            no_improve = 0; // Reiniciar contador
        } else {
            ++no_improve; // Incrementar iteraciones sin mejora
        }

        // Si no hay mejora por 'patience' iteraciones consecutivas, detener
        if (no_improve >= k) {
            cout << "\nConvergencia alcanzada en iteracion " << iter + 1
                 << " (sin mejora por " << k << " iteraciones)\n";
            break;
        }
    }

    // Retornar el mejor camino encontrado y su longitud
    return {best_path, best_len};
}
