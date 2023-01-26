#include <iostream>
#include <vector>
using namespace std;

// Datenstruktur zur Speicherung der Klauseln
vector<vector<int>> clauses;

// Funktion zur Überprüfung, ob eine bestimmte Konfiguration der Variablen gültig ist
bool check_satisfiability(vector<int> assignment) {
    // Überprüfe jede Klausel
    for (auto clause : clauses) {
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

// Backtracking-Suche nach einer gültigen Zuweisung der Variablen
bool backtrack_search() {
    // Erstelle eine Zuweisung mit allen Variablen auf unbestimmt (0)
    vector<int> assignment(clauses.size(), 0);
    int variable_index = 0;
    while (variable_index >= 0) {
        // Wenn alle Variablen zugewiesen sind, prüfe ob die Zuweisung gültig ist
        if (variable_index == clauses.size()) {
            if (check_satisfiability(assignment)) {
                return true;
            } else {
                variable_index--;
                continue;
            }
        }
        // Versuche die nächste Variable zuzuweisen
        assignment[variable_index]++;
        // Wenn die Zuweisung ungültig ist, gehe zurück und versuche eine andere Zuweisung
        if (assignment[variable_index] > 1) {
            assignment[variable_index] = 0;
            variable_index--;
        } else {
            variable_index++;
        }
    }
    return false;
}

int main() {
    // Beispielklauseln
    clauses.push_back({1, -2});
    clauses.push_back({-1, 2});
    clauses.push_back({-2, 3});
    clauses.push_back({3});
    // Führe die Backtracking-Suche aus
    if (backtrack_search()) {
        cout << "Die Klauseln sind erfüllbar." << endl;
    } else {
        cout << "Die Klauseln sind nicht erfüllbar." << endl;
    }
    return 0;
}