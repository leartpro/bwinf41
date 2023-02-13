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
 * @param over_node der zweite Knoten
 * @param to_node der dritte Knoten (Zielknoten)
 * @param from_node der erste Knoten
 * @return false, wenn der Winkel der Kanten kleiner als 90° beträgt, sonst true
 */
double cross_angle(const pair<double, double> &from_node,
                   const pair<double, double> &over_node,
                   const pair<double, double> &to_node

) {
    pair<double, double> p, q;
    p = make_pair(over_node.first - to_node.first,
                  over_node.second - to_node.second);
    q = make_pair(over_node.first - from_node.first,
                  over_node.second - from_node.second);
    double angle = acos(
            p.first * q.first + p.second * q.second / (
                    sqrt(pow((over_node.first - to_node.first), 2.0) +
                         (pow((over_node.second - to_node.second), 2.0))) *
                    sqrt(pow((over_node.first - from_node.first), 2.0) +
                         (pow((over_node.second - from_node.second), 2.0)))
            )
    ) * 180 / M_PI; //Umrechnung von Radian nach Grad
    return angle;
}

/**
 * Versucht rekursiv eine möglichst kurze Route durch den Graphen zu finden,
 * welche die Kriterien der Aufgabenstellung erfüllt.
 * Dazu wird die Nearest-Neighbour-Heuristik und Backtracking verwendet.
 *
 * @param route die aktuelle Route durch den Graphen
 * @param coordinates eine Menge aller eingelesenen Koordinaten
 * @return true, wenn alle Knoten in der Lösungsmenge (route) enthalten sind,
 * andernfalls wird false zurückgegeben
 */
bool solve(vector<pair<double, double> > &route, vector<pair<double, double> > &coordinates) {
    /*
    cout << "route (depth=" << route.size() << "): ";
    for (const auto &p: route) {
        cout << "(" << p.first << "," << p.second << ") -> ";
    }
    cout << endl;
     */
    //Wenn alle Knoten in der Lösungsmenge sind
    if (route.size() == coordinates.size()) {
        return true;
    }
    //Sortiere nach dem nächsten Knoten
    /*
    if (!route.empty()) {
        const auto &p = route.back();
        sort(coordinates.begin(), coordinates.end(),
             [p](const auto &lhs, const auto &rhs) {
                 return sqrt(pow((p.first - lhs.first), 2.0) + (pow((p.second - lhs.second), 2.0)))
                 > sqrt(pow((p.first - rhs.first), 2.0) + (pow((p.second - rhs.second), 2.0)));
             });
    }
     */
    //Für jeden Knoten
    for (int i = 0; i < coordinates.size(); i++) {
        //Wenn dieser Knoten bereits in der Lösungsmenge existiert, überspringe diesen
        if (std::find(route.begin(), route.end(), coordinates[i]) != route.end()) {
            continue;
        }
        //Wenn es noch keinen Knoten in der Lösungsmenge gibt
        if (route.empty()) {
            //Füge den Knoten hinzu
            route.push_back(coordinates[i]);
            //Wenn es der Knoten noch nicht in der Lösungsmenge enthalten ist
            //und nach den Winkelkriterien erreichbar ist
        } else if (
                std::find(route.begin(), route.end(), coordinates[i]) == route.end() &&
                (
                        route.size() < 3 ||
                        cross_angle(route[route.size() - 2], route.back(), coordinates[i]) >= 90
                )
                ) {
            //Füge den Knoten hinzu
            route.push_back(coordinates[i]);
            //Wenn es eine Lösung mit der aktuellen Route gibt
            if (solve(route, coordinates)) {
                return true;
            } else {
                route.pop_back();
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

        cout << output_file << ":" << endl;

        //Öffnet die Eingabedatei
        ifstream fin(input_file);

        //Öffnet die Ausgabedatei
        ofstream fout(output_file);

        //Liest die Eingabedatei ein
        vector<pair<double, double> > coordinates;
        double x, y;
        while (fin >> x >> y) {
            coordinates.emplace_back(x, y);
        }

        //Berechnet die Lösung
        vector<pair<double, double> > result;
        if (solve(result, coordinates)) {
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