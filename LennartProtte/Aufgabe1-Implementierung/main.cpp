#include <vector>
#include <iostream>
#include <unordered_map>
#include <cmath>

//Checks if a vertex is reachable from another vertex
//TODO: this method can be replaced by graph.find()
// because if an edge exist between both nodes, the current node is reachable from the last node of the route
bool is_reachable(const int &current,
                  std::vector<int> &route,
                  const int &last,
                  const std::unordered_map<int, std::pair<std::pair<double, double>, std::pair<double, double>>>& graph
                  ) {
    if (route.empty()) {
        return true;
    }
    if (graph.find(current)->second.first == graph.find(last)->second.second) { //TODO: validate this statement
        return true;
    }
    return false;
}

//Checks if current is not excluded by the clauses set by route
bool is_not_excluded(const int &current,
                     std::vector<int> &route,
                     const std::vector<std::pair<int, int>> &not_together_clauses,
                     const std::vector<std::vector<int>> &one_existing_clauses) {
    if (route.empty()) {
        return true;
    }
    for (const auto &not_together_clause: not_together_clauses) {
        if (not_together_clause.first == route.back() && not_together_clause.second == current) {
            return false;
        }
    }
    for (int &vertex: route) {
        for (const auto &clause: one_existing_clauses) {
            if (std::find(clause.begin(), clause.end(), vertex) != clause.end() &&
                std::find(clause.begin(), clause.end(), current) != clause.end()) {
                return false;
            }
        }
    }
    return true;
}

bool route_satisfied(std::vector<int> &route,
                     const std::unordered_map<int, std::pair<std::pair<double, double>, std::pair<double, double>>>& graph,
                     const std::vector<std::pair<int, int>> &not_together_clauses,
                     const std::vector<std::vector<int>> &one_existing_clauses) {
    for (auto it = route.begin(); it != route.end(); ++it) {
        std::cout << "is reachable: " << is_reachable(*it, route, route[int(std::distance(route.begin(), it+ 1))], graph)
        << " is not excluded: " << is_not_excluded(*it, route, not_together_clauses, one_existing_clauses) << std::endl;
        std::cout << "current: " << *it << " , last: " << route[int(std::distance(route.begin(), it+ 1))] << std::endl;
        //TODO: both methods doesnt seem to work properly
        if (!is_reachable(*it, route, route[int(std::distance(route.begin(), it + 1))], graph) ||
            !is_not_excluded(*it, route, not_together_clauses, one_existing_clauses)) {
            return false;
        }
    }
    if (route.size() == graph.size() / 2) { //TODO: total count of nodes graph.size() is 12 not 8
        return true;
    } else {
        return false;
    }
}

bool sat_solver(std::vector<int> &vertexes,
                const std::unordered_map<int, std::pair<std::pair<double, double>, std::pair<double, double>>>& graph,
                std::vector<int> &route,
                const std::vector<std::pair<int, int>> &not_together_clauses,
                const std::vector<std::vector<int>> &one_existing_clauses) {
    if (route_satisfied(route, graph, not_together_clauses, one_existing_clauses)) { //TODO: does not work correctly
        return true;
    }
    std::vector<int> removed_vertexes;
    for (auto it = vertexes.begin(); it != vertexes.end(); ++it) {
        std::cout << "check for vertex: " << *it << std::endl;
        if (is_reachable(*it, route, route[int(route.size()) - 1], graph) &&
            is_not_excluded(*it, route, not_together_clauses, one_existing_clauses)) {
            std::cout << "is valid try in recursion for  " << *it << std::endl;
            route.emplace_back(*it);
            removed_vertexes.push_back(*it);
            vertexes.erase(it);
            if (sat_solver(vertexes, graph, route, not_together_clauses, one_existing_clauses)) {
                return true;
            } else {
                route.pop_back();
                for (auto &vertex: removed_vertexes) {
                    vertexes.push_back(vertex);
                }
                removed_vertexes.clear();
            }
        }
    }
    std::cout << "recursion FAILED " << std::endl;
    return false;
}

int main() {
    // Beispieldatensatz von 2d-Punktkoordinaten

    /*
    std::vector<std::pair<double, double>> coordinates{
            {2.0, 4.0},
            {0.0, 1.0},
            {0.0, 3.0},
            {2.0, 0.0},
            {4.0, 2.0},
    };
     */
    std::vector<std::pair<double, double>> coordinates{
            {0.0, 0.0},
            {1.0, 0.0},
            {0.0, 1.0},
            {1.0, 1.0},
    };
    int n = int(coordinates.size());
    std::vector<std::pair<int, int>> not_together_clauses; //they cant be after each other
    std::vector<std::vector<int>> one_existing_clauses; //only one of them can exist in solution
    std::unordered_map<int, std::pair<std::pair<double, double>, std::pair<double, double>>> graph;
    graph.reserve(n * (n - 1)); //maximum count of vertexes (because every vertex is stored for every direction)

    //TODO: validate exclusions
    //1. clause that every edge can only used once
    //2. creates vector with all possible edges
    int key = 0;
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (i != j) {
                auto value_1 = std::make_pair(coordinates[i], coordinates[j]);
                auto value_2 = std::make_pair(coordinates[j], coordinates[i]);
                graph.insert(std::make_pair(key, value_1));
                key++;
                graph.insert(std::make_pair(key, value_2));
                key++;
                one_existing_clauses.push_back({key - 2, key - 1});
            }
        }
    }

    //creates not_together_clauses that contains if two edges can not be behind each other
    for (auto i = graph.begin(); i != graph.end(); i++) {
        for (auto j = graph.begin(); j != graph.end(); j++) {
            if (i != j && i->second.second == j->second.first && i->second.first != j->second.second) {
                std::pair<double, double> v1;
                v1.first = i->second.second.first - i->second.first.first;
                v1.second = i->second.second.second - i->second.first.second;
                std::pair<double, double> v2;
                v2.first = j->second.first.first - j->second.second.first;
                v2.second = j->second.first.second - j->second.second.second;
                double dot_product = v1.first * v2.first + v1.second * v2.second;
                double v1_length = sqrt(v1.first * v1.first + v1.second * v1.second);
                double v2_length = sqrt(v2.first * v2.first + v2.second * v2.second);
                double angle = acos(dot_product / (v1_length * v2_length));
                angle = angle * 180 / M_PI; // Umrechnung in Grad des innen Winkels alpha
                std::cout << i->first << " | [(" << i->second.first.first << "," << i->second.first.second << ") -> ("
                          << i->second.second.first << "," << i->second.second.second << ")] -> "
                          << j->first << " | [(" << j->second.first.first
                          << "," << j->second.first.second << ") -> (" << j->second.second.first << ","
                          << j->second.second.second << ")] => " << angle << "°" << std::endl;
                if (angle < 90) {
                    not_together_clauses.emplace_back(i->first, j->first);
                }
            }
        }
    }

    //create clauses entry for every node, it is only allowed to use two of all edges connected to him
    for (auto coordinate: coordinates) {
        std::vector<int> from_node, to_node;
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

    std::cout << "not_together_clauses: " << std::endl;
    for (const auto &clause: not_together_clauses) {
        std::cout << clause.first << " " << clause.second;
        std::cout << std::endl;
    }

    std::cout << "one_existing_clauses: " << std::endl;
    for (const auto &clause: one_existing_clauses) {
        for (const auto &vertex: clause) {
            std::cout << vertex << " ";
        }
        std::cout << std::endl;
    }

    std::vector<int> result, vertexes;
    for (auto & i : graph) {
        vertexes.push_back(i.first);
    }
        if (sat_solver(vertexes, graph, result, not_together_clauses, one_existing_clauses)) {
        std::cout << "solution" << std::endl;
        for (int r : result) {
            if (graph.find(r) == graph.end()) {
                auto it = graph.find(r);
                std::cout
                        << "(" << it->second.first.first
                        << ", " << it->second.first.second
                        << ") -> (" << it->second.second.first
                        << ", " << it->second.second.second
                        << ")" << std::endl;
            }
        }
    } else {
        std::cout << "no solution" << std::endl;
    }
    return 0;
}