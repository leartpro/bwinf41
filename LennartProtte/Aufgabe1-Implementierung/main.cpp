#include <vector>
#include <iostream>
#include <cmath>

// Umrechnung der 2D-Punktkoordinaten in 1D-Variablen
int getVariable(int i, int j, int n) {
    return i * n + j + 1;
}

// Funktion zur Überprüfung, ob eine bestimmte Konfiguration der Variablen gültig ist
bool check_satisfiability(std::vector<bool> assignment, std::vector<std::vector<int>> &clauses) {
    // Überprüfe jede Klausel
    for (const auto& clause : clauses) {
        bool clause_satisfied = false;
        // Überprüfe jede Variable in der Klausel
        for (auto variable : clause) {
            // Wenn eine Variable in der Klausel negiert ist, muss der Wert der zugehörigen
            // Variablen in der Zuweisung negiert werden
            int variable_value = (variable > 0) ? assignment[variable - 1] : !assignment[-variable - 1];
            if (variable_value == 1) {
                clause_satisfied = true;
                break;
            }
        }
        if (!clause_satisfied) {
            return false;
        }
    }
    return true;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
bool backtracking(std::vector<std::vector<int>> &clauses, std::vector<bool> &assignment, int variableIndex) {
    if (variableIndex == assignment.size()) {
        // Prüfe, ob alle Klauseln erfüllt sind
        for (const std::vector<int> &clause : clauses) {
            bool clauseSatisfied = false;
            for (const int &literal : clause) {
                int variable = abs(literal);
                bool value = (literal > 0) ? assignment[variable - 1] : !assignment[variable - 1];
                if (value) {
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

    // Teste die Variable auf wahr
    assignment[variableIndex] = true;
    if (backtracking(clauses, assignment, variableIndex + 1)) {
        return true;
    }

    // Teste die Variable auf falsch
    assignment[variableIndex] = false;
    if (backtracking(clauses, assignment, variableIndex + 1)) {
        return true;
    }

    return false;
}
#pragma clang diagnostic pop

int main() {
    // Anzahl der Städte
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
    std::vector<bool> assignment(n*n, -1);
    std::vector<std::vector<int>> clauses;
    std::vector<std::pair<int, int>> vertexes((n*(n-1))/2); //size=maximum count of vertexes

    //(¬x_ik ∨ ¬x_kj ∨ ¬(α_ij >= 90)) ∧ ...
    std::cout << "every vertex used once" << std::endl;
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            if (i != j) {
                std::cout << i << " -> " << j << " v " << j << " -> " << i;
                std::cout << " ==> " << -getVariable(i, j, n) << " v " << -getVariable(j, i, n) << std::endl;
                clauses.push_back({-getVariable(i, j, n), -getVariable(j, i, n)});
            }
        }
    }
    std::cout << "no transition between edges with angle less than 90 degree" << std::endl;
    for (int i = 0; i < coordinates.size(); i++) {
        for (int j = i + 1; j < coordinates.size(); j++) {
            for (int k = j + 1; k < coordinates.size(); k++) {
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
                            //std::cout << " (" << v1.first << ", " << v1.second << ") - " << angle << "° -> (" << v2.first << ", " << v2.second << ") < 90°";
                            //std::cout << " ? ==> " << -getVariable(i, j, n) << " " << -getVariable(k, j, n) << std::endl;
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


    //(∑(x_ij) = 1 ∀ i ∈ {1,2,3,...,n}) ∧ ...
    for (int i = 1; i <= n; i++) {
        std::vector<int> clause;
        for (int j = 1; j <= n; j++) {
            if (i != j) {
                clause.push_back(getVariable(i, j, n));
            }
        }
        clauses.push_back(clause);
    }

    //print line's
    for (const std::vector<int> &clause : clauses) {
        for (const int &variable : clause) {
            std::cout << variable << " ";
        }
        std::cout << std::endl;
    }

    // Find an assignment of variables that satisfies the equation
    if (backtracking(clauses, assignment, n)) {
        std::cout << "valid route=" << bool(check_satisfiability(assignment, clauses));
        // Print result
        std::cout << "Lösungsroute: ";
        for (int i = 0; i < assignment.size(); i++) {
            if (assignment[i] == 1) {
                std::cout << " -> " << coordinates[i].first << "," << coordinates[i].second;
            }
        }
        std::cout << std::endl;
    } else {
        std::cout << "valid route=" << bool(check_satisfiability(assignment, clauses));
        std::cout << "keine Lösung gefunden" << std::endl;
    }
    return 0;
}