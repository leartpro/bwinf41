#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <cmath>
#include <queue>
#include <unordered_map>

using namespace std;

double calculate_angle(pair<double, double> node1, pair<double, double> node2) {
    double x1 = node1.first;
    double y1 = node1.second;
    double x2 = node2.first;
    double y2 = node2.second;
    double angle = atan2(y2 - y1, x2 - x1);
    angle = angle * 180 / M_PI;
    return angle;
}

vector<pair<pair<double, double>, vector<pair<int, double>>>> read_and_store_coordinates(
        std::vector<std::pair<double, double>>& coordinate_list) {
    vector<pair<pair<double, double>, vector<pair<int, double>>>> graph;
    // Iteriere über jede Koordinate in der Liste
    for (const auto & i : coordinate_list) {
        // Splitte die Koordinaten in x und y
        // Füge die Koordinate als Knoten zum Graphen hinzu
        graph.push_back({{i.first, i.second}, {}});
    }
    // Iteriere über jeden Knoten im Graphen
    for (int i = 0; i < graph.size(); i++) {
        // Iteriere über jeden anderen Knoten im Graphen
        for (int j = 0; j < graph.size(); j++) {
            if (i == j) continue;
            // Berechne den Winkel zwischen dem aktuellen Knoten und dem anderen Knoten
            double angle = calculate_angle(graph[i].first, graph[j].first);
            // Füge eine Kante von dem aktuellen Knoten zu dem anderen Knoten hinzu
            graph[i].second.emplace_back(j, angle);
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
        std::vector<std::pair<double, double>> coordinate_list;
        // Solange es noch Koordinaten in der Datei gibt
        double x, y;
        while (fin >> x >> y) {
            coordinate_list.emplace_back(x, y);
        }
        auto graph = read_and_store_coordinates(coordinate_list);
        // Graph in Ausgabedatei schreiben
        for (int i = 0; i < graph.size(); i++) {
            fout << "Knoten " << i << ": (" << graph[i].first.first << ", " << graph[i].first.second << ")" << endl;
            fout << "Kanten:" << endl;
            for (auto & j : graph[i].second) {
                fout << "  - Knoten " << j.first << ": Winkel " << j.second << endl;
            }
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
