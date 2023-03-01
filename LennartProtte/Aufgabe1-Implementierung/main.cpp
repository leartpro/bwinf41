#include <vector>
#include <iostream>
#include <cmath>
#include <filesystem>
#include <fstream>

using namespace std;

/**
 * Berechnet den Winkel zwischen den Vektoren von from_node nach over_node und over_node nach to_node
 * @param over_node der zweite Knoten
 * @param to_node der dritte Knoten (Zielknoten)
 * @param from_node der erste Knoten
 * @return false, wenn der Winkel der Kanten kleiner als 90° beträgt, sonst true
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

void generate_adi_graph(const vector<pair<double, double>> &result, const string &file) {
    int fx = 24, fy = 24;
    cout << "\\begin{figure}[!h]\\centering" << endl;
    cout << "\\NewAdigraph{" << file.substr(0, file.size() - 4) << "}{" << endl;
    for (int i = 0; i < result.size(); i++) {
        if (i != 0 && i != result.size() - 1) {
            cout << "    " << i << ":" << result[i].first / fx << "," << result[i].second / fy << ":"
                 << (((float)((int)(cross_angle(result[i - 1], result[i], result[i + 1]) * 10))) / 10) << "° " << ";" << endl;
        } else if (i == 0) {
            cout << "    " << i << ":" << result[i].first / fx << "," << result[i].second / fy << ":start;" << endl;
        } else {
            cout << "    " << i << ":" << result[i].first / fx << "," << result[i].second / fy << ":end;" << endl;
        }
    }
    cout << "}{" << endl;
    for (int i = 0; i + 1 < result.size(); i++) {
        cout << i << "," << i + 1 << ";" << endl;
    }
    cout << "}\n\\" << file.substr(0, file.size() - 4) << "{" << endl;
    for (int i = 0; i < result.size(); i++) {
        cout << i << (i == result.size() - 1 ? "" : ",");
    }
    cout << "::red;" << endl;
    cout << "}" << endl;
    cout << "\\caption{Figur: " << file.substr(0, file.size() - 4) << "}\n\\label{fig:"
         << file.substr(0, file.size() - 4) << "}\n\\end{figure}" << endl;
    cout << "\\newpage" << endl;
}

/**
 * Versucht rekursiv mit backtracking eine möglichst kurze Route durch den Graphen zu finden,
 * welche die Kriterien der Aufgabenstellung erfüllt.
 * @param route die aktuelle Route
 * @param coordinates eine Menge aller eingelesenen Koordinaten
 * @return true, wenn alle Knoten in der Lösungsmenge (route) enthalten sind, sonst false
 */
bool solve(vector<pair<double, double> > &route, vector<pair<double, double> > &coordinates) {
    //Sortiere nach dem nächsten Knoten
    if (!route.empty()) {
        const auto &p = route.back();
        sort(coordinates.begin(), coordinates.end(),
             [p](const auto &lhs, const auto &rhs) {
                 return sqrt(pow((p.first - lhs.first), 2.0) + (pow((p.second - lhs.second), 2.0)))
                        < sqrt(pow((p.first - rhs.first), 2.0) + (pow((p.second - rhs.second), 2.0)));
             });
    }
    //Für jeden Knoten
    for (int i = 0; i < coordinates.size(); i++) {
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
            //Füge den Knoten hinzu
            route.push_back(coordinates[i]);
            //Wenn alle Knoten in der Lösungsmenge sind
            if (route.size() == coordinates.size()) {
                return true;
            }
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
        //TODO: Aufgabenkriterien beachten
        //Berechnet den Schwerpunkt der Punktewolke
        double sum_x, sum_y;
        for (auto &coordinate: coordinates) {
            sum_x += coordinate.first;
            sum_y += coordinate.second;
        }
        sum_x /= int(coordinates.size());
        sum_y /= int(coordinates.size());
        //Sortiert absteigend nach der Entfernung zum Schwerpunkt der Punktwolke
       /* sort(coordinates.begin(), coordinates.end(),
             [sum_x, sum_y](const auto &lhs, const auto &rhs) {
                 return sqrt(pow((lhs.first - sum_x), 2.0) + (pow((lhs.second - sum_y), 2.0)))
                        > sqrt(pow((rhs.first - sum_x), 2.0) + (pow((rhs.second - sum_y), 2.0)));
             });*/
        vector<pair<double, double> > result;
        //Schreibt das Ergebnis in die entsprechende Ausgabedatei
        if (solve(result, coordinates)) {
            fout << "Es konnte eine Flugstrecke durch alle Außenposten ermittelt werden" << endl;
            for (int i = 0; i < result.size(); i++) {
                if (i != 0 && i != result.size() - 1) {
                    fout << cross_angle(result[i - 1], result[i], result[i + 1]) << "° ";
                }
                fout << "[(" << result[i].first << ", " << result[i].second << ")] -> " << endl;
            }
        } else {
            fout << "Es konnte keine Flugstrecke durch alle Außenposten ermittelt werde" << endl;
        }
        generate_adi_graph(result, entry.path().filename().string());
    }
    return 0;
}