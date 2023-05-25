#include <vector>
#include <iostream>
#include <cmath>
#include <filesystem>
#include <fstream>

using namespace std;

/**
 * Berechnet den Winkel in Grad zwischen den Vektoren von from_node nach over_node und over_node nach to_node
 * @param over_node der zweite Knoten
 * @param to_node der dritte Knoten (Zielknoten)
 * @param from_node der erste Knoten
 * @return false, wenn der Innenwinkel zwischen den Kanten kleiner als 90° beträgt, andernfalls true
 */
double cross_angle(const pair<double, double> &from_node,
                   const pair<double, double> &over_node,
                   const pair<double, double> &to_node) {
    double a, b, c;
    a = sqrt(pow((from_node.first - to_node.first), 2.0) + (pow((from_node.second - to_node.second), 2.0)));
    b = sqrt(pow((from_node.first - over_node.first), 2.0) + (pow((from_node.second - over_node.second), 2.0)));
    c = sqrt(pow((over_node.first - to_node.first), 2.0) + (pow((over_node.second - to_node.second), 2.0)));
    double angle = acos((pow(a, 2.0) - pow(b, 2.0) - pow(c, 2.0)) / (-2 * b * c)) * 180 / M_PI;
    return angle;
}

/**
 * Berechnet rekursiv mit Backtracking eine möglichst kurze Route durch den Graphen,
 * welche die Aufgabenkriterien erfüllt.
 * @param route die aktuelle Route
 * @param coordinates die Menge aller eingelesenen Koordinaten
 * @return true, wenn alle Knoten in der Lösungsmenge (route) enthalten sind, andernfalls false
 */
bool solve(vector<pair<double, double> > &route, vector<pair<double, double> > &coordinates) {
    if (!route.empty()) { //Wenn die Route nicht leer ist, sortiere nach dem nächsten Knoten
        const auto &last = route.back();
        sort(coordinates.begin(), coordinates.end(),
             [last](const auto &lhs, const auto &rhs) {
                 return sqrt(pow((last.first - lhs.first), 2.0) + (pow((last.second - lhs.second), 2.0)))
                        < sqrt(pow((last.first - rhs.first), 2.0) + (pow((last.second - rhs.second), 2.0)));
             });
    }
    for (int i = 0; i < coordinates.size(); i++) { //Für jeden Knoten
        //Wenn dieser Knoten bereits in der Lösungsmenge existiert, überspringe diesen
        if (std::find(route.begin(), route.end(), coordinates[i]) != route.end()) {
            continue;
        }
        double angle = -1;
        if (route.size() >= 2) {
            angle = cross_angle(route[route.size() - 2], route.back(), coordinates[i]);
        }
        if (route.empty() ||
            (std::find(route.begin(), route.end(), coordinates[i]) == route.end() &&
             (route.size() < 2 || angle >= 90))
                ) {
            route.push_back(coordinates[i]); //Füge den Knoten hinzu
            if (route.size() == coordinates.size()) { //Wenn alle Knoten in der Lösungsmenge sind
                return true;
            }
            if (solve(route, coordinates)) { //Wenn es eine Lösung mit der aktuellen Route gibt
                return true;
            } else {
                route.pop_back();
            }
        }
    }
    return false; //Wenn es mit der aktuellen Route keine Lösung geben kann
}


/**
 * Liest die Eingabedateien ein und berechnet für jede Datei eine Lösung entsprechend der Aufgabenstellung.
 * Die Lösung wird anschließend in die entsprechende Ausgabedatei geschrieben.
 * Sollte es keine Lösung geben, wird dies ebenfalls in die Ausgabedatei geschrieben.
 * @return Exitcode
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
        double x, y;
        while (fin >> x >> y) {
            coordinates.emplace_back(x, y);
        }

        //Berechnet die Lösung und schreibt das Ergebnis in die entsprechende Ausgabedatei
        vector<pair<double, double> > result;
        if (solve(result, coordinates)) {
            double distance = 0;
            for(int i = 0; i + 1 < result.size(); i++) {
                distance += sqrt(pow((result[i].first - result[i+1].first), 2.0) + (pow((result[i].second - result[i+1].second), 2.0)));
            }
            fout << "Es konnte eine " << distance << " km lange Flugstrecke durch alle Außenposten ermittelt werden." << endl;
            for (int i = 0; i < result.size(); i++) {
                fout << i + 1 << ". [" << result[i].first << ", " << result[i].second << "] ";
                if (i != 0 && i != result.size() - 1) {
                    fout << cross_angle(result[i - 1], result[i], result[i + 1]) << "° " << endl;
                } else {
                    fout << endl;
                }
            }
        } else {
            fout << "Es konnte keine Flugstrecke durch alle Außenposten ermittelt werden." << endl;
        }
    }
    return 0;
}