#include <vector>
#include <iostream>
#include <unordered_map>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <algorithm>

using namespace std;

bool is_reachable(const int &from,
                  const int &to,
                  const vector<int> &route,
                  const unordered_map<int, pair<pair<double, double>, pair<double, double> > > &graph
) {
    if (route.empty() || graph.find(from)->second.second == graph.find(to)->second.first) {
        return true;
    }
    return false;
}

bool is_route_satisfied(const vector<int> &route,
                        const unordered_map<int, pair<pair<double, double>, pair<double, double> > > &graph,
                        const int &count_of_nodes,
                        const vector<pair<int, int> > &not_together_clauses,
                        const vector<vector<int> > &one_existing_clauses
) {
    if (route.empty()) {
        return false;
    }
    for (int i = 0; i < route.size() - 1; i++) {
        bool is_not_reachable = !is_reachable(route[i], route[i + 1], route, graph);
        if (is_not_reachable) {
            return false;
        }
    }
    if (route.size() == count_of_nodes) {
        return true;
    } else {
        return false;
    }
}

void remove_excluded(vector<int> &vertexes,
                     const vector<int> &route,
                     const vector<pair<int, int> > &not_together_clauses,
                     const vector<vector<int> > &one_existing_clauses
) {
    vector<int> remove_vertexes;
    for (int &unused_vertex: vertexes) {
        for (const pair<int, int> &not_together_clause: not_together_clauses) {
            if (not_together_clause.first == route.back() && not_together_clause.second == unused_vertex) {
                remove_vertexes.push_back(unused_vertex);
            }
        }
        for (const int &used_vertex: route) {
            if (used_vertex != unused_vertex) {
                for (const vector<int> &one_existing_clause: one_existing_clauses) {
                    if (find(one_existing_clause.begin(), one_existing_clause.end(), used_vertex) !=
                        one_existing_clause.end()) {
                        remove_vertexes.insert(remove_vertexes.end(), one_existing_clause.begin(),
                                               one_existing_clause.end());
                    }
                }
            }
        }
    }
    for (int &remove_vertex: remove_vertexes) {
        vertexes.erase(remove(vertexes.begin(), vertexes.end(), remove_vertex), vertexes.end());
    }
}

bool sat_solver(const vector<int> &vertexes,
                vector<int> &route,
                const unordered_map<int, pair<pair<double, double>, pair<double, double> > > &graph,
                const int &count_of_nodes,
                const vector<pair<int, int> > &not_together_clauses,
                const vector<vector<int> > &one_existing_clauses
) {
    if (is_route_satisfied(route, graph, count_of_nodes, not_together_clauses, one_existing_clauses)) {
        return true;
    }
    cout << "iterating through " << int(vertexes.size()) << " vertexes" << endl;
    for (int i = 0; i < vertexes.size(); i++) {
        if (is_reachable(route[int(route.size()) - 1], vertexes[i], route, graph)) {
            cout << "current route (size=" << int(route.size()) << ", i=" << i << "): ";
            for (const auto &p: route) {
                cout << p << " -> ";
            }
            cout << endl;
            route.emplace_back(vertexes[i]);
            vector<int> p_vertexes(vertexes);
            p_vertexes.erase(p_vertexes.begin() + i);
            remove_excluded(p_vertexes, route, not_together_clauses, one_existing_clauses);
            //TODO: after this lock for sole candidates, if at least one sole candidate exist try for this
            // if the sole candidate fails, return false
            // if there is no sole candidate, sort by lowest degree (degree ascending)

            //TODO: another option is to use the nearest neighbour heuristic at this point

            //TODO: replace graph by matrix array and coordinates array to increase performance
            // matrix contains at every position vertex_id & distance, coordinates contains each coordinate
            if (sat_solver(p_vertexes, route, graph, count_of_nodes, not_together_clauses, one_existing_clauses)) {
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
        vector<vector<pair<int, double>>> vertexes;
        vector<pair<int, int> > not_together_clauses; //they cant be after each other
        vector<vector<int> > one_existing_clauses; //only one of them can exist in solution
        coordinates.reserve(total_count_of_nodes);
        vertexes.reserve(total_count_of_nodes * total_count_of_nodes);
        not_together_clauses.reserve(total_count_of_nodes * (total_count_of_nodes - 1));
        one_existing_clauses.reserve(total_count_of_nodes * total_count_of_nodes);
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
                    vertexes[i][j] = make_pair(identifier, distance);
                    identifier++;
                } else {
                    vertexes[i][j] = make_pair(-1, 0);
                }
            }
        }

        //creates not_together_clauses that contains if two edges can not be behind each other
        for (int i = 0; i < total_count_of_nodes; i++) {
            for (int j = 0; j < total_count_of_nodes; j++) {
                if(i != j) {
                    for(int m = 0; m < total_count_of_nodes; m++) {
                        for(int n = 0; n < total_count_of_nodes; n++) {
                            if(m != n && !(m == i && n == j)) {
                                pair<double, double> p, q;
                                p = make_pair(coordinates[i].first - coordinates[j].first, coordinates[i].second - coordinates[j].second);
                                q = make_pair(coordinates[m].first - coordinates[n].first, coordinates[m].second - coordinates[n].second);
                                double dot_product = p.first * q.first + p.second * q.second;
                                double angle = acos(dot_product / (vertexes[i][j].second * vertexes[m][n].second)) * 180 / M_PI;
                                if (angle < 90) {
                                    not_together_clauses.emplace_back(i, j);
                                }
                            }
                        }
                    }
                }
            }
        }

        //create clauses entry for every node, it is only allowed to use two of all edges connected to him
        for (auto coordinate: coordinates) {
            vector<int> from_node, to_node;
            for (auto &i: graph) {
                if (i.second.first == coordinate) {
                    from_node.push_back(i.first);
                } else if (i.second.second == coordinate) {
                    to_node.push_back(i.first);
                }
            }
            one_existing_clauses.push_back(from_node);
            one_existing_clauses.push_back(to_node);
        }

        std::cout << "Vertexes: " << std::endl;
        for (auto &i: graph) {
            std::cout << i.first << " | ";
            std::cout
                    << "(" << i.second.first.first
                    << ", " << i.second.first.second
                    << ") -> (" << i.second.second.first
                    << ", " << i.second.second.second
                    << ")" << std::endl;
        }
        std::cout << std::endl;

        /*
        std::cout << "not_together_clauses: " << std::endl;
        for (const auto &clause: not_together_clauses) {
            std::cout << clause.first << " " << clause.second;
            std::cout << std::endl;
        }
         */
        /*
        std::cout << "one_existing_clauses: " << std::endl;
        for (const auto &clause: one_existing_clauses) {
            for (const auto &vertex: clause) {
                std::cout << vertex << " ";
            }
            std::cout << std::endl;
        }
         */


        //init vertexes
        vector<int> result, vertexes;
        for (auto &i: graph) {
            vertexes.push_back(i.first);
        }

        if (sat_solver(vertexes, result, graph, int(coordinates.size()), not_together_clauses, one_existing_clauses)) {
            cout << "solution" << endl;
            for (int r: result) {
                if (graph.find(r) != graph.end()) {
                    auto value = graph.find(r);
                    cout
                            << "(" << value->second.first.first
                            << ", " << value->second.first.second
                            << ") -> (" << value->second.second.first
                            << ", " << value->second.second.second
                            << ")" << endl;
                }
            }
        } else {
            cout << "no solution" << endl;
        }
    }
    return 0;
}