#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <cmath>
#include <queue>
#include <unordered_map>

using namespace std;

// Struktur, um Koordinaten zu speichern
struct Coordinate {
    double x;
    double y;
};

// Funktionsprototyp
void process_files(const std::string &input_dir, const std::string &output_dir);

double calculate_angle(Coordinate c1, Coordinate c2) {
    double x_diff = c2.x - c1.x;
    double y_diff = c2.y - c1.y;
    return atan2(y_diff, x_diff) * 180 / M_PI;
}

vector<vector<double>> create_weighted_graph(vector<Coordinate> coordinate_list) {
    int num_nodes = coordinate_list.size();
    vector<vector<double>> graph(num_nodes, vector<double>(num_nodes, 0));
    for (int i = 0; i < num_nodes; i++) {
        for (int j = i + 1; j < num_nodes; j++) {
            double edge_weight = calculate_angle(coordinate_list[i], coordinate_list[j]);
            graph[i][j] = edge_weight;
            graph[j][i] = edge_weight;
        }
    }
    return graph;
}

void process_files(const std::string &input_dir, const std::string &output_dir) {
    // Iterator erstellen, der alle Dateien im Eingabeordner durchläuft
    for (const auto &entry: std::filesystem::directory_iterator(input_dir)) {
        // Dateiname und -pfad aus dem Iterator auslesen
        std::string input_file = entry.path();
        std::string output_file = output_dir + "/" + entry.path().filename().string();
        // Eingabedatei öffnen
        std::ifstream fin(input_file);
        // Ausgabedatei öffnen
        std::ofstream fout(output_file);
        // Vector, um die Koordinaten zu speichern
        vector<Coordinate> coordinate_list;
        // Solange es noch Koordinaten in der Datei gibt
        while (fin.good()) {
            Coordinate coord{};
            // Lese die x- und y-Koordinate aus der Datei ein
            fin >> coord.x >> coord.y;
            // Füge die Koordinate zur Liste hinzu
            coordinate_list.push_back(coord);
        }
        vector<vector<double>> graph = create_weighted_graph(coordinate_list);
        // Graph in Ausgabedatei schreiben
        for (auto & row : graph) {
            for (double val : row) {
                fout << val << ' ';
            }
            fout << endl;
        }
        // Dateien schließen
        fin.close();
        fout.close();
    }
}

int main() {
    // Aktuelles Arbeitsverzeichnis ermitteln
    std::string current_path = std::filesystem::current_path();
    std::cout << "Aktuelles Arbeitsverzeichnis: " << current_path << std::endl;
    // Pfade zu den Ordnern angeben
    std::string input_dir = "../LennartProtte/Aufgabe1-Implementierung/Eingabedateien";
    std::string output_dir = "../LennartProtte/Aufgabe1-Implementierung/Ausgabedateien";

    process_files(input_dir, output_dir);

    return 0;
}
