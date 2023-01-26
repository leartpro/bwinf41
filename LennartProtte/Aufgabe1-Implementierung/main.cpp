#include <vector>
#include <iostream>


int getVariable(int i, int j, int n) {
    // Umrechnung der 2D-Punktkoordinaten in 1D-Variablen
    return i * n + j + 1;
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
    int n = 5;
    std::vector<bool> assignment(n*n, -1);
    std::vector<std::vector<int>> clauses;
    // Beispieldatensatz von 2d-Punktkoordinaten
    std::vector<std::pair<double, double>> coordinates{
            {2.0, 4.0},
            {0.0, 1.0},
            {0.0, 3.0},
            {2.0, 0.0},
            {4.0, 2.0},
    };


    // Erstellen Sie die Klauseln für jede Bedingung in der Gleichung
    // ¬x_ik ∨ ¬x_kj
    for (int i = 1; i <= n; i++) {
        for (int k = 1; k <= n; k++) {
            for (int j = 1; j <= n; j++) {
                if (i != k && i != j && k != j) {
                    clauses.push_back({-getVariable(i, k, n), -getVariable(k, j, n)});
                }
            }
        }
    }

    // ∑(x_ij) = 1 ∀ i ∈ {1,2,3,...,n}
    for (int i = 1; i <= n; i++) {
        std::vector<int> clause;
        for (int j = 1; j <= n; j++) {
            if (i != j) {
                clause.push_back(getVariable(i, j, n));
            }
        }
        clauses.push_back(clause);
    }
    for (const std::vector<int> &clause : clauses) {
        for (const int &variable : clause) {
            std::cout << variable << " ";
        }
        std::cout << std::endl;
    }
    // Find an assignment of variables that satisfies the equation
    backtracking(clauses, assignment, n);
    // Print result
    std::cout << "Lösungsroute: ";
    for (int i = 0; i < assignment.size(); i++) {
        if (assignment[i] == 1) {
            std::cout << " -> " << coordinates[i].first << "," << coordinates[i].second;
        }
    }
    std::cout << std::endl;
    return 0;
}