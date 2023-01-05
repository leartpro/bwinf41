#include <iostream>
#include <queue>
#include <unordered_map>
#include <cmath>
#include <fstream>

using namespace std;

// Struktur, um Koordinaten zu speichern
struct Coordinate {
    double x;
    double y;
};

// Funktion, um den Winkel zwischen zwei Koordinaten in Grad zu berechnen
double calculate_angle(Coordinate c1, Coordinate c2) {
    double x_diff = c2.x - c1.x;
    double y_diff = c2.y - c1.y;
    return atan2(y_diff, x_diff) * 180 / M_PI;
}

// Funktion, um den gewichteten Graphen in Matrixform zu erstellen
vector<vector<double>> create_weighted_graph(vector<Coordinate> coordinate_list) {
    int num_nodes = coordinate_list.size();
    vector<vector<double>> graph(num_nodes, vector<double>(num_nodes, 0));
    for (int i = 0; i < num_nodes; i++) {
        for (int j = i+1; j < num_nodes; j++) {
            double edge_weight = calculate_angle(coordinate_list[i], coordinate_list[j]);
            graph[i][j] = edge_weight;
            graph[j][i] = edge_weight;
        }
    }
    return graph;
}

// Struktur, um die Informationen über den aktuellen Zustand beim Durchlaufen des Graphen zu speichern
struct State {
    int node;
    int cost;
};

// Vergleichsfunktion für States, um sie in der Prioritätswarteschlange sortieren zu können
struct CompareState {
    bool operator()(const State& s1, const State& s2) {
        return s1.cost > s2.cost;
    }
};

// Funktion, um den Weg durch den Graphen zu finden
vector<int> find_path(vector<vector<double>>& graph, int start, int end) {
    // Prioritätswarteschlange, um die Knoten nach Kosten sortiert zu speichern
    priority_queue<State, vector<State>, CompareState> pq;
    // Hash-Tabelle, um den günstigsten bisher gefundenen Kosten für jeden Knoten zu speichern
    unordered_map<int, int> cost_so_far;
    // Hash-Tabelle, um den Vorgängerknoten für jeden Knoten zu speichern
    unordered_map<int, int> came_from;
    // Füge den Startknoten zur Prioritätswarteschlange hinzu
    pq.push({start, 0});
    //
    // Initialisiere die Kosten für den Startknoten auf 0
    cost_so_far[start] = 0;
    // Solange es noch Knoten in der Prioritätswarteschlange gibt
    while (!pq.empty()) {
        // Hole den Knoten mit den geringsten Kosten aus der Warteschlange
        State current = pq.top();
        pq.pop();
        // Wenn der aktuelle Knoten der Zielknoten ist, breche die Suche ab
        if (current.node == end) {
            break;
        }
        // Durchlaufe alle Nachbarknoten des aktuellen Knotens
        for (int i = 0; i < graph.size(); i++) {
            // Wenn es keine Kante zwischen den Knoten gibt oder das Kantengewicht größer als 90 ist, überspringe diesen Nachbarknoten
            if (graph[current.node][i] == 0 || graph[current.node][i] > 90) {
                continue;
            }
            // Berechne die Kosten, um zu diesem Nachbarknoten zu gelangen
            int new_cost = cost_so_far[current.node] + graph[current.node][i];
            // Wenn die bisherigen Kosten größer sind als die Kosten, um zu diesem Knoten zu gelangen, aktualisiere die Kosten und merke den aktuellen Knoten als Vorgänger
            if (!cost_so_far.count(i) || new_cost < cost_so_far[i]) {
                cost_so_far[i] = new_cost;
                came_from[i] = current.node;
                // Füge den Knoten zur Prioritätswarteschlange hinzu
                pq.push({i, new_cost});
            }
        }
    }
    // Erstelle den Weg durch den Graphen, indem der Vorgängerknoten für jeden Knoten verfolgt wird
    vector<int> path;
    int current = end;
    while (current != start) {
        path.push_back(current);
        current = came_from[current];
    }
    path.push_back(start);
    // Kehre den Weg um, damit er von start nach end verläuft
    reverse(path.begin(), path.end());
    // Wenn kein Weg gefunden wurde, gib eine leere Liste zurück
    if (path.empty()) {
        cout << "No path found" << endl;
        return {};
    }
    return path;
}

int main() {
    // Öffne die Datei "coordinates.txt" zum Lesen
    ifstream input_file("coordinates.txt");
    // Vector, um die Koordinaten zu speichern
    vector<Coordinate> coordinate_list;
    // Solange es noch Koordinaten in der Datei gibt
    while (input_file.good()) {
        Coordinate coord{};
        // Lese die x- und y-Koordinate aus der Datei ein
        input_file >> coord.x >> coord.y;
        // Füge die Koordinate zur Liste hinzu
        coordinate_list.push_back(coord);
    }
    vector<vector<double>> graph = create_weighted_graph(coordinate_list);
    // Frage den Benutzer nach dem Start- und Endknoten ab
    int start, end;
    cout << "Enter start node: ";
    cin >> start;
    cout << "Enter end node: ";
    cin >> end;
    vector<int> path = find_path(graph, start, end);
    // Drucke den gefundenen Weg aus
    cout << "Path: ";
    for (int node : path) {
        cout << node << " ";
    }
    cout << endl;
    return 0;
}

