#include <vector>
#include <iostream>
#include <cmath>

// Umrechnung der 2D-Punktkoordinate in 1D-Variable
int getVariable(int i, int j, int n) {
    return (i - 1) * n + j - 1;
}

// Umrechnung der 1D-Variable in 2D-Punktkoordinate
std::pair<int, int> getCoordinateIndexes(int v, int n) {
    int i = v / n + 1;
    int j = v % n + 1;
    return std::make_pair(i, j);
}

//Checks if a vertex is reachable from another vertex
bool is_reachable(const int &current,
                  std::vector<int> &route,
                  const int &last_index,
                  const int &count_of_nodes) {
    if (route.empty()) {
        return true;
    }
    const int &last = route[last_index];
    if (getCoordinateIndexes(current, count_of_nodes).first ==
        getCoordinateIndexes(last, count_of_nodes).second
            ) {
        //TODO: for the given data this should be 6 for 1=last and not 8
        std::cout << current << "(index=" << getCoordinateIndexes(current, count_of_nodes).first
                  << ") comes after " << last << "(index=" << getCoordinateIndexes(last, count_of_nodes).second << ")"
                  << std::endl;
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
                     const int &count_of_nodes,
                     const std::vector<std::pair<int, int>> &not_together_clauses,
                     const std::vector<std::vector<int>> &one_existing_clauses) {
    for (auto it = route.begin(); it != route.end(); ++it) {
        if (!is_reachable(*it, route, int(std::distance(route.begin(), it)), count_of_nodes) ||
            !is_not_excluded(*it, route, not_together_clauses, one_existing_clauses)) {
            return false;
        }
    }
    if (route.size() == count_of_nodes) {
        return true;
    } else {
        return false;
    }
}

bool sat_solver(std::vector<int> &vertexes,
                std::vector<int> &route,
                const std::vector<std::pair<int, int>> &not_together_clauses,
                const std::vector<std::vector<int>> &one_existing_clauses) {
    if (route_satisfied(route, int(vertexes.size()) / 2, not_together_clauses, one_existing_clauses)) {
        return true;
    }
    std::vector<int> removed_vertexes;
    for (auto it = vertexes.begin(); it != vertexes.end(); ++it) {
        std::cout << "check for vertex: " << *it << std::endl;
        if (is_reachable(*it, route, int(route.size()) - 1, int(vertexes.size()) / 2) &&
            is_not_excluded(*it, route, not_together_clauses, one_existing_clauses)) {
            std::cout << "is valid try in recursion for  " << *it << std::endl;
            route.emplace_back(*it);
            removed_vertexes.push_back(*it);
            vertexes.erase(it);
            if (sat_solver(vertexes, route, not_together_clauses, one_existing_clauses)) {
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
    std::vector<int> vertexes;
    vertexes.reserve(
            (n * (n - 1))); //maximum count of vertexes (because every vertex is stored for every direction)

    //1. clause that every edge can only used once
    //2. creates vector with all possible edges
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (i != j) {
                vertexes.push_back(getVariable(i, j, n));
                vertexes.push_back(getVariable(j, i, n));
                one_existing_clauses.push_back({getVariable(i, j, n), getVariable(j, i, n)});
            }
        }
    }

    //creates not_together_clauses that contains if two edges can not be behind each other
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            for (int k = j + 1; k < n; k++) {
                if (i != j && i != k && j != k) { //find all tripple
                    for (int l = 0; l < 3; l++) { //for ech possible order of the current tripple
                        std::pair<double, double> v1;
                        v1.first = coordinates[j].first - coordinates[i].first;
                        v1.second = coordinates[j].second - coordinates[i].second;
                        std::pair<double, double> v2;
                        v2.first = coordinates[j].first - coordinates[k].first;
                        v2.second = coordinates[j].second - coordinates[k].second;
                        double dot_product = v1.first * v2.first + v1.second * v2.second;
                        double v1_length = sqrt(v1.first * v1.first + v1.second * v1.second);
                        double v2_length = sqrt(v2.first * v2.first + v2.second * v2.second);
                        double angle = acos(dot_product / (v1_length * v2_length));
                        angle = angle * 180 / M_PI; // Umrechnung in Grad des innen Winkels alpha
                        if (angle < 0) {
                            angle += 360; // Winkel im Bereich von 0 bis 360
                        }
                        if (angle < 90) {
                            not_together_clauses.emplace_back(getVariable(i, j, n), getVariable(j, k, n));
                        }
                        int t = i;
                        i = j;
                        j = k;
                        k = t;
                    }
                    //rotate to default
                    int t = i;
                    i = j;
                    j = k;
                    k = t;
                }
            }
        }
    }

    //create clauses entry for every node, it is only allowed to use two of all edges connected to him
    for (int i = 0; i < n; i++) {
        std::vector<int> from_node, to_node;
        from_node.reserve(vertexes.size() - 1);
        to_node.reserve(vertexes.size() - 1);
        for (const auto &vertex: vertexes) {
            if (getCoordinateIndexes(vertex, n).first == i) {
                from_node.push_back(vertex);
            } else if (getCoordinateIndexes(vertex, n).second == i) {
                to_node.push_back(vertex);
            }
        }
        one_existing_clauses.push_back(from_node);
        one_existing_clauses.push_back(to_node);
    }

    std::cout << "Vertexes: " << std::endl;
    for (const auto &v: vertexes) {
        std::cout << v << " | ";
        std::cout
                << "FIRST:(" << coordinates[getCoordinateIndexes(v, n).first].first
                << ", " << coordinates[getCoordinateIndexes(v, n).first].second
                << ") -> SECOND:(" << coordinates[getCoordinateIndexes(v, n).second].first
                << ", " << coordinates[getCoordinateIndexes(v, n).second].second
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

    std::vector<int> result;
    if (sat_solver(vertexes, result, not_together_clauses, one_existing_clauses)) {
        std::cout << "solution" << std::endl;
        for (int vertex: result) {
            std::cout
                    << "(" << coordinates[getCoordinateIndexes(vertex, n).first].first
                    << ", " << coordinates[getCoordinateIndexes(vertex, n).first].second
                    << ") -> (" << coordinates[getCoordinateIndexes(vertex, n).second].first
                    << ", " << coordinates[getCoordinateIndexes(vertex, n).second].second
                    << ")" << std::endl;
        }
    } else {
        std::cout << "no solution" << std::endl;
    }
    return 0;
}