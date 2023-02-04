#include <vector>
#include <iostream>
#include <unordered_map>
#include <cmath>
#include <filesystem>
#include <fstream>

using namespace std;

/**
 *
 * @param from
 * @param to
 * @param route
 * @param graph
 * @return
 */
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

/**
 *
 * @param current
 * @param route
 * @param not_together_clauses
 * @param one_existing_clauses
 * @return
 */
bool is_not_excluded(const int &current,
                     const vector<int> &route,
                     const vector<pair<int, int> > &not_together_clauses,
                     const vector<vector<int> > &one_existing_clauses) {
    if (route.empty()) {
        return true;
    }
    for (const pair<int, int> &not_together_clause: not_together_clauses) {
        if (not_together_clause.first == route.back() && not_together_clause.second == current) {
            return false;
        }
    }
    for (const int &vertex: route) {
        if (vertex != current) {
            for (const vector<int> &clause: one_existing_clauses) {
                if (find(clause.begin(), clause.end(), vertex) != clause.end() &&
                    find(clause.begin(), clause.end(), current) != clause.end()) {
                    return false;
                }
            }
        }
    }
    return true;
}

/**
 *
 * @param route
 * @param graph
 * @param count_of_nodes
 * @param not_together_clauses
 * @param one_existing_clauses
 * @return
 */
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
        bool is_excluded = !is_not_excluded(route[i], route, not_together_clauses, one_existing_clauses);
        if (is_not_reachable || is_excluded) {
            return false;
        }
    }
    if (!is_not_excluded(route[route.size() - 1], route, not_together_clauses, one_existing_clauses)) {
        return false;
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
    /*
vertexes.erase(remove_if(
                   vertexes.begin(),
                   vertexes.end(),
                   [&route, &not_together_clauses](int i) {
                       return any_of(not_together_clauses.begin(), not_together_clauses.end(),
                                     [&route, &i](pair<int, int> clause) {
                                         return route[route.size() - 1] == clause.first && i == clause.second;
                                     });
                   }),
           vertexes.end());
           */
    for (int i = 0; i < vertexes.size(); i++) {
        for (const pair<int, int> &clause: not_together_clauses) {
            if (route.back()== clause.first && vertexes[i] == clause.second) {
                vertexes.erase(vertexes.begin() + i);
            }
        }
    }
    /*
    for(int i = 0; i < route.size(); i++) {
        for(const vector<int> &clause : one_existing_clauses) {
            for(int j = 0; j < clause.size(); j++) {
                if(i == clause[j]) {
                    for(int k = 0; k < clause.size(); k++) {
                        if(j != k) {
                            const auto it = find(vertexes.begin(), vertexes.end(), clause[k]);
                            if(it != vertexes.end()) {
                                vertexes.erase(it);
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
*/
    vector<int> remove_vertexes;
    for(int & unused_vertex : vertexes) {
        for (const pair<int, int> &not_together_clause: not_together_clauses) {
            if (not_together_clause.first == route.back() && not_together_clause.second == unused_vertex) {
                remove_vertexes.push_back(unused_vertex);
            }
        }
        for (const int &used_vertex : route) {
            if (used_vertex != unused_vertex) {
                for (const vector<int> &one_existing_clause : one_existing_clauses) {
                    if (find(one_existing_clause.begin(), one_existing_clause.end(), used_vertex) != one_existing_clause.end() &&
                        find(one_existing_clause.begin(), one_existing_clause.end(), unused_vertex) != one_existing_clause.end()) {
                        remove_vertexes.push_back(unused_vertex);
                    }
                }
            }
        }
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
    for (int i = 0; i < vertexes.size(); i++) {
        if (is_reachable(route[int(route.size()) - 1], vertexes[i], route, graph) &&
            is_not_excluded(vertexes[i], route, not_together_clauses, one_existing_clauses)) {
            cout << "current route (size=" << int(route.size()) << ", i=" << i << "): ";
            for (const auto &p: route) {
                cout << p << " -> ";
            }
            cout << endl;
            route.emplace_back(vertexes[i]);
            vector<int> p_vertexes(vertexes);
            p_vertexes.erase(p_vertexes.begin() + i);
            remove_excluded(p_vertexes, route, not_together_clauses, one_existing_clauses);
            //TODO: sort vertexes here by distance
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
    for (const auto &entry: filesystem::directory_iterator(input_dir)) {

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
        double x, y;
        while (fin >> x >> y) {
            coordinates.emplace_back(x, y);
        }

        int n = int(coordinates.size());
        vector<pair<int, int> > not_together_clauses; //they cant be after each other
        vector<vector<int> > one_existing_clauses; //only one of them can exist in solution
        unordered_map<int, pair<pair<double, double>, pair<double, double> > > graph;
        graph.reserve(n * (n - 1)); //maximum count of vertexes (because every vertex is stored for every direction)

        //1. clause that every edge can only used once
        //2. creates vector with all possible edges
        int key = 0;
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                if (i != j) {
                    auto value_1 = make_pair(coordinates[i], coordinates[j]);
                    auto value_2 = make_pair(coordinates[j], coordinates[i]);
                    graph.insert(make_pair(key, value_1));
                    key++;
                    graph.insert(make_pair(key, value_2));
                    key++;
                    one_existing_clauses.push_back({key - 2, key - 1});
                }
            }
        }

        //creates not_together_clauses that contains if two edges can not be behind each other
        for (int i = 0; i < graph.size(); i++) {
            for (int j = 0; j < graph.size(); j++) {
                if (graph[i] != graph[j] && graph[i].second == graph[j].first && graph[i].first != graph[j].second) {
                    pair<double, double> v1;
                    v1.first = graph[i].second.first - graph[i].first.first;
                    v1.second = graph[i].second.second - graph[i].first.second;
                    pair<double, double> v2;
                    v2.first = graph[j].first.first - graph[j].second.first;
                    v2.second = graph[j].first.second - graph[j].second.second;
                    double dot_product = v1.first * v2.first + v1.second * v2.second;
                    double v1_length = sqrt(v1.first * v1.first + v1.second * v1.second);
                    double v2_length = sqrt(v2.first * v2.first + v2.second * v2.second);
                    double angle = acos(dot_product / (v1_length * v2_length));
                    angle = angle * 180 / M_PI; // Umrechnung in Grad des innen Winkels alpha
                    if (angle < 90) {
                        not_together_clauses.emplace_back(i, j);
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