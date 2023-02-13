#include <vector>
#include <iostream>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <algorithm>

using namespace std;

/**
 * Überprüft, anhand drei aufeinanderfolgender Knoten,
 * ob das Winkelkriterium der Aufgabenstellung für diese drei Knoten erfüllt ist
 * @param from_node der zweite Knoten
 * @param to_node der dritte Knoten (Zielknoten)
 * @param last_node der erste Knoten
 * @return false, wenn der Winkel der Kanten kleiner als 90° beträgt, sonst true
 */
bool is_reachable(const pair<double, double> &from_node,
                  const pair<double, double> &to_node,
                  const pair<double, double> &last_node
) {
    pair<double, double> p, q;
    p = make_pair(from_node.first - to_node.first,
                  from_node.second - to_node.second);
    q = make_pair(from_node.first - last_node.first,
                  from_node.second - last_node.second);
    double angle = acos(
            p.first * q.first + p.second * q.second / (
                    sqrt(pow((from_node.first - to_node.first), 2.0) +
                         (pow((from_node.second - to_node.second), 2.0))) *
                    sqrt(pow((from_node.first - last_node.first), 2.0) +
                         (pow((from_node.second - last_node.second), 2.0)))
            )
    ) * 180 / M_PI; //Umrechnung von Radian nach Grad
    if (angle < 90) {
        return false;
    }
    return true;
}

/**
 * Versucht rekursiv eine möglichst kurze Route durch den Graphen zu finden,
 * welche die Kriterien der Aufgabenstellung erfüllt.
 * Dazu wird die Nearest-Neighbour-Heuristik, als auch Backtracking verwendet.
 * In jedem Schritt werden alle Kanten des zuletzt eingefügten Knoten betrachtet
 * und der Knoten an der kürzesten Kante als nächster Knoten gesetzt.
 *
 * @param route die aktuelle Route durch den Graphen
 * @param graph der aus der Eingabedatei generierte Graph
 * @param coordinates eine Menge aller eingelesenen Koordinaten
 * @return true, wenn alle Knoten in der Lösungsmenge (route) enthalten sind,
 * andernfalls wird false zurückgegeben
 */
bool solve(vector<pair<double, double> > &route,
           const vector<vector<double> > &graph,
           const vector<pair<double, double> > &coordinates
) {
    //Wenn alle Knoten in der Lösungsmenge sind
    if (route.size() == coordinates.size()) {
        return true;
    }
    //Für jeden Knoten
    //TODO: sort coordinates by nearest
    for (int i = 0; i < coordinates.size(); i++) {
        //Wenn dieser Knoten bereits in der Lösungsmenge existiert, überspringe diesen
        if (std::find(route.begin(), route.end(), coordinates[i]) != route.end()) {
            continue;
        }
        auto edges = graph[i];
        //Für alle Kanten des aktuellen Knotens
        //TODO: sort edges by nearest
        for (int j = 0; j < edges.size(); j++) {
            //Wenn es eine Kante zu ihm selbst ist, überspringe diese
            if (edges[j] == 0) {
                continue;
            }
            //Wenn es noch keinen Knoten in der Lösungsmenge gibt
            if (route.empty()) {
                //Füge den Knoten hinzu
                route.push_back(coordinates[i]);
                //Wenn es der letzte fehlende Knoten in der Lösungsmenge ist
            } else if (route.size() == coordinates.size() - 1) {
                //Wenn dieser nach den Winkelkriterien erreichbar ist
                if (is_reachable(route.back(),
                                 coordinates[i],
                                 route[route.size() - 2]
                )) {
                    //Füge den Knoten hinzu und gebe true zurück
                    route.push_back(coordinates[i]);
                    return true;
                }
                //Wenn es der Knoten noch nicht in der Lösungsmenge enthalten ist
                //und nach den Winkelkriterien erreichbar ist
            } else if (
                    std::find(route.begin(),
                              route.end(),
                              coordinates[j]) == route.end() &&
                    is_reachable(route.back(),
                                 coordinates[j],
                                 route[route.size() - 2]
                    )
                    ) {
                //Füge den Knoten hinzu
                route.push_back(coordinates[j]);
                //Wenn es eine Lösung mit der aktuellen Route gibt
                if (solve(route, graph, coordinates)) {
                    return true;
                } else {
                    route.pop_back();
                }
            }
        }
    }
    //Wenn es mit der aktuellen Route keine Lösung geben kann
    return false;
}

/**
 * Liest die Eingabedateien ein und versucht für jede Datei eine Lösung entsprechend der Aufgabenstellung zu finden
 * Die Lösung wird anschließend in die entsprechende Ausgabedatei geschrieben
 * Sollte es keine Lösung geben, wird dies ebenfalls in die Ausgabedatei geschrieben
 * @return 0, wenn es zu keinem RuntimeError oder keiner RuntimeException gekommen ist
 */
int main() {
    string input_dir = "../LennartProtte/Aufgabe1-Implementierung/Eingabedateien";
    string output_dir = "../LennartProtte/Aufgabe1-Implementierung/Ausgabedateien";

    //Durchläuft alle Dateien im Eingabeordner
    for (const std::filesystem::directory_entry &entry: filesystem::directory_iterator(input_dir)) {

        //Liest den Dateinamen aus
        string input_file = entry.path();
        string output_file = output_dir + "/" + entry.path().filename().string();

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

        //Initialisiert den Graphen
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

        //Berechnet die Lösung
        vector<pair<double, double> > result;
        if (solve(result, graph, coordinates)) {
            fout << "Es konnte eine Flugstrecke durch alle Außenposten ermittelt werden" << endl;
            for (const auto v: result) {
                fout << "(" << v.first << ", " << v.second << ") -> " << endl;
            }
        } else {
            fout << "Es konnte keine Flugstrecke durch alle Außenposten ermittelt werde" << endl;
        }

    }
    return 0;
}