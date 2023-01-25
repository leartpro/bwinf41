#include <vector>
#include <iostream>

using namespace std;

void add_exactly_one_constraint(vector<int> variables);

const int N = 100; // maximum number of nodes
int n = N;
vector<vector<bool>> x(N, vector<bool>(N)); // decision variables x_ij
vector<vector<int>> clauses; // to store the clauses

// function to add constraint that x_ij and x_kj cannot be true at the same time
void add_turn_constraint(int i, int j, int k) {
    clauses.push_back({x[i][j], x[k][j]});
}


// function to add constraint that exactly one x_ij is true for each i
void add_visit_constraint(int i) {
    vector<int> variables;
    for (int j = 1; j <= n; j++) {
        variables.push_back(x[i][j]);
    }
    add_exactly_one_constraint(variables);
}

// function to add constraint that x_1i and x_i1 are both true
void add_loop_constraint(int i) {
    clauses.push_back({x[1][i], x[i][1]});
}

// function to add constraint that exactly one variable in the given vector is true
void add_exactly_one_constraint(vector<int> variables) {
    for (int i = 0; i < variables.size(); i++) {
        for (int j = i+1; j < variables.size(); j++) {
            clauses.push_back({-variables[i], -variables[j]});
        }
    }
    for (int & variable : variables) {
        clauses.push_back({variable});
    }
}

bool backtrack(int var, vector<int> & model) {
    if (var == n*n) {
        // all variables have been assigned, check if the model is a valid solution
        for (auto & clause : clauses) {
            bool clause_satisfied = false;
            for (auto & variable : clause) {
                if (model[abs(variable)-1] == variable) {
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
    // try both possible assignments for this variable
    model[var] = var + 1;
    if (backtrack(var+1, model)) {
        return true;
    }
    model[var] = -var - 1;
    if (backtrack(var+1, model)) {
        return true;
    }
    return false;
}

bool solve_SAT() {
    vector<int> model(n*n, -1); // stores the model found by the solver
    return backtrack(0, model);
}


int main() {
    cout << "starting main..." << endl;
    // your code to read input
    //initialize decision variables x_ij and angle_ij
    int angle_ij[4][4] = {{0, 90, 180, 90},
                          {270, 0, 90, 180},
                          {90, 270, 0, 90},
                          {180, 90, 270, 0}
    };

    // add constraints
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            if (angle_ij[i][j] > 90) {
                for (int k = 1; k <= n; k++) {
                    add_turn_constraint(i, j, k);
                }
            }
        }
        add_visit_constraint(i);
    }
    for (int i = 1; i <= n; i++) {
        add_loop_constraint(i);
    }

    if (solve_SAT()) {
        cout << "solution found" << endl;
        // your code to extract the solution and return the route
    } else {
        cout << "no solution found" << endl;
    }
    return 0;
}