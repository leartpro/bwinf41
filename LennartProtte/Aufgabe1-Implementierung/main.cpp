#include <vector>
#include <iostream>
#include <unordered_map>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <algorithm>

using namespace std;

//TODO: everything is stored by index to entry in coordinates

bool is_reachable(const int &idx_from,
                  const int &idx_to,
                  const vector<int> &route,
                  const vector<vector<pair<int, double> > > &graph,
                  const vector<pair<double, double> > &coordinates
) {
    if (route.empty() || graph[idx_from][idx_to].first != -1) {
        pair<double, double> p, q;
        p = make_pair(coordinates[idx_from].first - coordinates[idx_to].first,
                      coordinates[idx_from].second - coordinates[idx_to].second);
        q = make_pair(coordinates[route.back()].first - coordinates[idx_from].first,
                      coordinates[route.back()].second - coordinates[idx_from].second);
        double dot_product = p.first * q.first + p.second * q.second;
        double angle = acos(dot_product / (graph[idx_from][idx_to].second * graph[route.back()][idx_from].second)) * 180 / M_PI;
        if (angle < 90) {
            return false;
        }
        return true;
    }
    return false;
}

bool sat_solver(vector<int> &route,
                const vector<vector<pair<int, double> > > &graph,
                const vector<pair<double, double> > &coordinates
                ) {
    if (route.size() == coordinates.size()) {
        return true;
    }
    for (int i = 0; i < coordinates.size(); i++) {
        if (is_reachable(route.back(), vertexes[i], route, graph)) {
            cout << "current route (size=" << int(route.size()) << ", i=" << i << "): ";
            for (const auto &p: route) {
                cout << p << " -> ";
            }
            cout << endl;
            route.emplace_back(vertexes[i]);
            vector<int> p_vertexes(vertexes);
            p_vertexes.erase(p_vertexes.begin() + i);
            if (sat_solver(route, graph, coordinates)) {
                return true;
            } else {
                route.pop_back();
            }
        }
    }
    return false;
}

/**
 * Liest die Eingabedateien ein und versucht für jede Datei eine Lösung entsprechend der Aufgabenstellung zu finden
 * Die Lösung wird anschließend in die entsprechende Ausgabedatei geschrieben
 * Sollte es keine Lösung geben, wird dies ebenfalls in die Ausgabedatei geschrieben
 * @return 0, wenn es zu keinem RuntimeError oder keiner RuntimeException gekommen ist
 */
int main() {
    string input_dir = "../LennartProtte/Aufgabe1-Implementierung/TestInput";
    string output_dir = "../LennartProtte/Aufgabe1-Implementierung/TestOutput";

    //Durchläuft alle Dateien im Eingabeordner
    for (const std::filesystem::directory_entry &entry: filesystem::directory_iterator(input_dir)) {

        //Liest den Dateinamen aus
        string input_file = entry.path();
        string output_file = output_dir + "/" + entry.path().filename().string();

        cout << "calculate solution for " << entry.path().filename().string() << endl;

        //Öffnet die Eingabedatei
        ifstream fin(input_file);

        //Öffnet die Ausgabedatei
        ofstream fout(output_file);

        int total_count_of_nodes = int(std::count(istreambuf_iterator<char>(fin), istreambuf_iterator<char>(), '\n'));

        //Liest die Eingabedatei ein
        vector<pair<double, double> > coordinates;
        vector<vector<pair<int, double>>> graph;
        coordinates.reserve(total_count_of_nodes);
        graph.reserve(total_count_of_nodes * total_count_of_nodes);
        double x, y;
        while (fin >> x >> y) {
            coordinates.emplace_back(x, y);
        }

        int identifier = 0;
        for (int i = 0; i < total_count_of_nodes; i++) {
            for (int j = 0; j < total_count_of_nodes; j++) {
                if (i != j) {
                    double distance = sqrt(pow((coordinates[i].first - coordinates[j].first), 2.0) +
                                           (pow((coordinates[i].second - coordinates[j].second), 2.0)));
                    graph[i][j] = make_pair(identifier, distance);
                    identifier++;
                } else {
                    graph[i][j] = make_pair(-1, 0);
                }
            }
        }

        cout << "Coordinates:" << endl;
        for (int i = 0; i < total_count_of_nodes; i++) {
            cout << "index = " << i << ", (" << coordinates[i].first << ", " << coordinates[i].second << ")" << endl;
        }
        cout << "Vertexes:" << endl;
        for (int i = 0; i < total_count_of_nodes; i++) {
            for (int j = 0; j < total_count_of_nodes; j++) {
                cout << "<" << graph[i][j].first << ", " << graph[i][j].second << "> ";
            }
            cout << endl;
        }
        /*
        std::cout << "not_together_clauses: " << std::endl;
        for (const auto &clause: not_together_clauses) {
            std::cout << clause.first << " " << clause.second;
            std::cout << std::endl;
        }
         */

        //init graph
        vector<int> result;
        if (sat_solver(result, graph, coordinates)) {
            cout << "solution" << endl;
        } else {
            cout << "no solution" << endl;
        }
    }
    return 0;
}