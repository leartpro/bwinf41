#include <vector>
#include <unordered_map>
#include <iostream>

std::vector<std::vector<int>> clauses;
std::unordered_map<int, bool> assignments;
std::vector<int> route;

int getVariable(int i, int j, int n) {
    // Umrechnung der 2D-Punktkoordinaten in 1D-Variablen
    return i * n + j + 1;
}

bool isSatisfiable() {
    for (const auto &clause: clauses) {
        bool clauseSatisfied = false;
        for (const auto &variable: clause) {
            if ((assignments[abs(variable)] == true && variable > 0) ||
                (assignments[abs(variable)] == false && variable < 0)) {
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

void backtrack() {
    if (route.size() == assignments.size()) {
        return;
    }
    int variable = int(route.size());
    for (int i = variable; i < assignments.size(); i++) {
        if (!assignments[i]) {
            variable = i;
            break;
        }
    }
    assignments[variable] = true;
    route.push_back(variable);
    if (isSatisfiable()) {
        return;
    }
    assignments[variable] = false;
    route.pop_back();

    backtrack();
}


int main() {
    // Anzahl der Städte
    int n = 5;

    // Beispieldatensatz von 2d-Punktkoordinaten
    std::vector<std::pair<double, double>> coordinates{
            {0.0, 1.0},
            {0.0, 3.0},
            {2.0, 4.0},
            {4.0, 2.0},
            {2.0, 0.0}
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

    // Find an assignment of variables that satisfies the equation
    backtrack();

    // Print result
    for (int stadt: route) {
        std::cout << stadt << " -> ";
    }
    std::cout << "0" << std::endl;
    if (isSatisfiable()) {
        std::cout << "Satisfiable" << std::endl;
    } else {
        std::cout << "Not satisfiable" << std::endl;
    }
    return 0;
}