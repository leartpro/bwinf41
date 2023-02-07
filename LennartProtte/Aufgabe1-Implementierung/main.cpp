#include <vector>
#include <iostream>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <algorithm>

using namespace std;

bool is_reachable(const pair<double, double> &from_node,
                  const pair<double, double> &to_node,
                  const pair<double, double> &last_node,
                  const double &distance_from_to,
                  const double &distance_last_from
) {
        pair<double, double> p, q;
        p = make_pair(from_node.first - to_node.first,
                      from_node.second - to_node.second);
        q = make_pair(last_node.first - from_node.first,
                      last_node.second - from_node.second);
        double dot_product = p.first * q.first + p.second * q.second;
        double angle = acos(dot_product / (distance_from_to * distance_last_from)) * 180 / M_PI;
        if (angle < 90) {
            return false;
        }
        return true;
}


bool solve(vector<pair<double, double> > &route,
           const vector<vector<double> > &graph,
           const vector<pair<double, double> > &coordinates
) {
    if (route.size() == coordinates.size()) {
        return true;
    }
    for (int i = 0; i < coordinates.size(); i++) {
        if (std::find(route.begin(), route.end(), coordinates[i]) != route.end()) {
            continue;
        }
        auto edges = graph[i];
        for (int j = 0; j < edges.size(); j++) {
            if (edges[j] != 0 && (
                    route.empty() || (
                            std::find(route.begin(),
                                      route.end(),
                                      coordinates[j]) == route.end() &&
                            is_reachable(route.back(),
                                         coordinates[j],
                                         route[route.size() - 2],
                                         graph[i][j],
                                         graph[j - 1][i])
                    )
            )
                    ) {
                route.push_back(coordinates[j]);
                if (solve(route, graph, coordinates)) {
                    return true;
                } else {
                    route.pop_back();
                }
            }
        }
    }
    return false;
}


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

        //Liest die Eingabedatei ein
        vector<pair<double, double> > coordinates;
        vector<vector<double>> graph;
        double x, y;
        while (fin >> x >> y) {
            coordinates.emplace_back(x, y);
        }
        int total_count_of_nodes = int(coordinates.size());
        graph.reserve(total_count_of_nodes * total_count_of_nodes);

        for (int i = 0; i < total_count_of_nodes; i++) {
            vector<double> vertexes;
            for (int j = 0; j < total_count_of_nodes; j++) {
                if (i != j) {
                    double distance = sqrt(pow((coordinates[i].first - coordinates[j].first), 2.0) +
                                           (pow((coordinates[i].second - coordinates[j].second), 2.0)));
                    vertexes.push_back(distance);
                } else {
                    vertexes.push_back(0);
                }
            }
            graph.emplace_back(vertexes);
        }

        cout << "Coordinates:" << endl;
        for (int i = 0; i < total_count_of_nodes; i++) {
            cout << "index = " << i << ", (" << coordinates[i].first << ", " << coordinates[i].second << ")" << endl;
        }
        cout << "Vertexes:" << endl;
        for (int i = 0; i < total_count_of_nodes; i++) {
            for (int j = 0; j < total_count_of_nodes; j++) {
                cout << "<" << graph[i][j] << "> ";
            }
            cout << endl;
        }

        //init graph

        vector<pair<double, double> > result;
        if (solve(result, graph, coordinates)) {
            cout << "solution" << endl;
        } else {
            cout << "no solution" << endl;
        }

    }
    return 0;
}