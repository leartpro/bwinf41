#include <vector>
#include <iostream>
#include <cmath>

// Umrechnung der 2D-Punktkoordinaten in 1D-Variablen
int getVariable(int i, int j, int n) {
    return i * n + j;
}

std::pair<int, int> getCoordinateIndexes(int v, int n) {
    int i = v / n;
    int j = v % n;
    return std::make_pair(i, j);
}

std::vector<int> sat_solver(const std::vector<std::pair<int, int>> &not_existing_clauses,
                const std::vector<std::pair<int, int>> &not_together_clauses,
                std::vector<int> &vertexes) {

    std::vector<int> indices;
    for (int i = 0; i < vertexes.size(); i++) {
        if (i % 2 == 0) indices.push_back(i);
    }

    do { //for every not_existing clauses (that every node is visited only once)
        std::vector<int> not_existing_result;
        not_existing_result.reserve(indices.size());
        for (const auto &index: indices) {
            not_existing_result.push_back(vertexes[index]);
        }

        do { //for every not_together clauses, that not illegal transmission is used (angle criteria)
            bool satisfied = true;
            for (const auto &not_clause: not_together_clauses) {
                const auto it = std::find(not_existing_result.begin(), not_existing_result.end(), not_clause.first);
                if (it != not_existing_result.end()) {
                    auto index = std::distance(not_existing_result.begin(), it);
                    if(index == not_existing_result.size()) index = -1;
                    if (not_existing_result[index + 1] == not_clause.second) {
                        satisfied = false;
                        goto next_permutation;
                        //invalid
                    }
                }
            }
            next_permutation:
            if(satisfied) {
                return not_existing_result;
            }
        } while (next_permutation(not_existing_result.begin(), not_existing_result.end()));

    } while (next_permutation(indices.begin(), indices.end()));
    return *new std::vector<int>;
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
    //TODO: maybe transform not_existing_clauses into one_existing_clauses
    std::vector<std::pair<int, int>> not_existing_clauses; //both cant exist in solution
    std::vector<std::vector<int>> one_existing_clauses; //only one of them can exist in solution
    // => stores for each node two entries, one for outgoing edges and one for ingoing

    std::vector<int> vertexes;
    vertexes.reserve((n * (n - 1))); //maximum count of vertexes (because every vertex is stored for every direction)

    //1. clause that every edge can only used once
    //2. creates vector with all possible edges
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (i != j) {
                vertexes.push_back(getVariable(i, j, n));
                vertexes.push_back(getVariable(j, i, n));
                not_existing_clauses.emplace_back(getVariable(i, j, n), getVariable(j, i, n));
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
                            not_together_clauses.emplace_back(getVariable(i, j, n), getVariable(k, j, n));
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

    //TODO: create clauses entry for every node, all edges connected to him
    // than it is only allowed to use two of them
    for (int i = 0; i < n; i++) {
        std::vector<int> from_node, to_node;
        from_node.reserve(vertexes.size() -1);
        to_node.reserve(vertexes.size() -1);
        for(const auto& vertex: vertexes) {
            if(getCoordinateIndexes(vertex, n).first == i) {
                from_node.push_back(vertex);
            } else if(getCoordinateIndexes(vertex, n).second == i) {
                to_node.push_back(vertex);
            }
        }
        one_existing_clauses.push_back(from_node);
        one_existing_clauses.push_back(to_node);
    }

    std::cout << "Vertexes: " << std::endl;
    for (const auto &v: vertexes) {
        std::cout << v << " ";
    }
    std::cout << std::endl;

    std::cout << "not_together_clauses: " << std::endl;
    for (const auto &clause: not_together_clauses) {
        std::cout << clause.first << " " << clause.second;
        std::cout << std::endl;
    }

    std::cout << "not_existing_clauses: " << std::endl;
    for (const auto &clause: not_existing_clauses) {
        std::cout << clause.first << " " << clause.second;
        std::cout << std::endl;
    }

    std::cout << "one_existing_clauses: " << std::endl;
    for (const auto &clause: one_existing_clauses) {
        for(const auto& vertex : clause) {
            std::cout << vertex << " ";
        }
        std::cout << std::endl;
    }

    //TODO: problem ist, dass die bedingung fehlt, dass jeder knoten nur einmal besucht werden muss
    // Find an assignment of variables that satisfies the equation

    /*
     * Solve:
     * 1. check if completed
     * 2. insert next
     * 3. call recursive
     */
    std::vector<int> result = sat_solver(not_existing_clauses, not_together_clauses, vertexes);
    if (!result.empty()) {
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