#include <vector>
#include <iostream>
#include <stack>

using namespace std;

vector<vector<bool>> x; // decision variables x_ij
vector<vector<int>> clauses; // to store the clauses

// Initialisierung von x
void init_x(int n) {
    x.resize(n + 1);
    for (int i = 1; i <= n; i++) {
        x[i].resize(n + 1);
        for (int j = 1; j <= n; j++) {
            x[i][j] = false;
        }
    }
}

bool backtrack_iterative(vector<int> & model, int n) {
    stack<int> var_stack;
    var_stack.push(0);
    while (!var_stack.empty()) {
        int var = var_stack.top();
        var_stack.pop();
        if (var == n*n) {
            // all variables have been assigned, check if the model is a valid solution
            bool valid_solution = true;
            for (auto & clause : clauses) {
                bool clause_satisfied = false;
                for (auto & variable : clause) {
                    if (model[abs(variable)-1] == variable) {
                        clause_satisfied = true;
                        break;
                    }
                }
                if (!clause_satisfied) {
                    valid_solution = false;
                    break;
                }
            }
            if (valid_solution) {
                return true;
            } else {
                continue;
            }
        }
        // try both possible assignments for this variable
        model[var] = var + 1;
        var_stack.push(var+1);
        model[var] = -var - 1;
        var_stack.push(var+1);
    }
    return false;
}

int main() {
    //TODO: code to read input
    //initialize decision variables x_ij and angle_ij
        int angle_ij[4][4] = {{0, 90, 180, 270},
                              {90, 0, 90, 180},
                              {180, 270, 0, 90},
                              {270, 180, 90, 0}
        };
        int n = 4;
    init_x(n);

    // add constraints
    for (int i = 1; i < n; i++) {
        for (int j = 1; j < n; j++) {
            if (angle_ij[i][j] > 90) {
                for (int k = 1; k <= n; k++) {
                    //add constraint that x_ij and x_kj cannot be true at the same time
                    clauses.push_back({x[i][j], x[k][j]});  //TODO: BAD_ACCESS
                }
            }
        }
        //add constraint that exactly one x_ij is true for each i
        vector<int> variables;
        for (int j = 1; j <= n; j++) {
            variables.push_back(x[i][j]);
        }
        //add constraint that exactly one variable in the given vector is true
        for (int m = 0; m < variables.size(); m++) {
            for (int j = m + 1; j < variables.size(); j++) {
                clauses.push_back({-variables[m], -variables[j]});
            }
        }
        for (int & variable : variables) {
            clauses.push_back({variable});
        }
    }
    for (int i = 1; i <= n; i++) {
        //add constraint that x_1i and x_i1 are both true
        clauses.push_back({x[1][i], x[i][1]});
    }
    // stores the model found by the solver
    vector<int> model(n*n, -1);
    //solve SAT via backtracking
    if (backtrack_iterative(model, n)) {
        cout << "solution found" << endl;
        for (int i = 0; i < model.size(); i++) {
            if (model[i] > 0) {
                cout << i + 1 << " is true" << endl;
            }
        }
    } else {
        cout << "no solution found" << endl;
    }
    return 0;
}