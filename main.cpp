#include "ACO.h"
#include <iostream>
#include <random>

using namespace std;

int main() {
    vector<Point> cities;
    mt19937 gen(42);
    uniform_real_distribution<> dis(0.0, 10.0);

    // 10 ciudades aleatorias en un plano 10x10
    for (int i = 0; i < 10; ++i) {
        cities.push_back({dis(gen), dis(gen)});
        cout << "Ciudad " << i << ": " << "x: " << cities[i].x << ", " << "y: " << cities[i].y << endl;
    }

    auto [best_path, best_length] = ant_colony_optimization(cities, 5, 30, 1.0, 2.0, 0.5, 100.0, 3);

    cout << "\nMejor recorrido encontrado:\n";
    for (int i : best_path) cout << i << " -> ";
    cout << best_path.front() << endl;

    cout << "\nLongitud: " << best_length << "\n";
    return 0;
}