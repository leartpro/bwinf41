#include <vector>
#include <iostream>
#include <cmath>

// Umrechnung der 2D-Punktkoordinaten in 1D-Variablen
int getVariable(int i, int j, int n) {
    return i * n + j;
}

bool backtrack(const std::vector<std::vector<int>>& clauses, std::vector<int> vertexes, std::vector<bool> assignment, int index) {
    // Wenn alle Variablen zugewiesen sind, überprüfen Sie, ob die Klauseln erfüllt sind
    if (index == vertexes.size()) {
        for (const auto& clause : clauses) {
            bool clauseSatisfied = false;
            for (auto vertex : clause) {
                if (assignment[vertex] == true) {
                    clauseSatisfied = true;
                    break;
                }
            }
            if (!clauseSatisfied) {
                return false;
            }
        }
        return true;
    }
    // Weisen Sie der aktuellen Variable einen Wert zu
    assignment[vertexes[index]] = true;
    if (backtrack(clauses, vertexes, assignment, index + 1)) {
        return true;
    }
    assignment[vertexes[index]] = false;
    return backtrack(clauses, vertexes, assignment, index + 1);
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
    std::vector<std::vector<int>> clauses;
    std::vector<bool> assignment(n*n, -1);
    std::vector<int> vertexes;
    vertexes.reserve((n*(n-1))); //maximum count of vertexes (because every vertex is stored for every direction)

    //1. clause that every edge can only used once
    //2. creates vector with all possible edges
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (i != j) {
                vertexes.push_back(getVariable(i, j, n));
                vertexes.push_back(getVariable(j, i, n));
                clauses.push_back({-getVariable(i, j, n), -getVariable(j, i, n)});
            }
        }
    }

    //creates clauses that contains if two edges can not be behind each other
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            for (int k = j + 1; k < n; k++) {
                if (i != j && i != k && j != k) {
                    for (int l = 0; l < 3; l++) {
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
                        if(angle < 90) {
                            clauses.push_back({-getVariable(i, j, n), -getVariable(k, j, n)});
                        }
                        int t = i;
                        i = j;
                        j = k;
                        k = t;
                    }
                    int t = i;
                    i = j;
                    j = k;
                    k = t;
                }
            }
        }
    }

    //creates clauses that every edge have to be connected with at least on vertex
    //=> at least one vertex per claus hav to be used
    for (int i = 0; i < n; i++) {
        std::vector<int> clause;
        for (int j = 0; j < n; j++) {
            if (i != j) {
                clause.push_back(getVariable(i, j, n));
            }
        }
        clauses.push_back(clause);
    }

    // Find an assignment of variables that satisfies the equation
    if(backtrack(clauses, vertexes, assignment, 0)) {
        std::cout << "solution" << std::endl;
    } else {
        std::cout << "no solution" << std::endl;
    }
    return 0;
}