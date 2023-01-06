#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>

using namespace std;


struct Edge {
    double distance{};
    vector<pair<Edge*, double>> edges;
};

struct Node {
    double x{};
    double y{};
    vector<Edge*> edges;
};

// Berechnet den Abstand zwischen zwei Punkten (x1, y1) und (x2, y2)
double distance(double x1, double y1, double x2, double y2) {
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}


int main() {
    // Öffne Datei zum Lesen
    ifstream file("../LennartProtte/Aufgabe1-Implementierung/Eingabedateien/circlepath.txt");
    if (!file.is_open()) {
        cerr << "Datei konnte nicht geöffnet werden." << endl;
        return 1;
    }

    vector<Node> graph;

    // Lese jede Zeile der Datei ein
    string line;
    while (getline(file, line)) {
        stringstream lineStream(line);

        // Erstelle einen neuen Knoten
        Node node;

        // Lese die x- und y-Koordinate aus der Zeile ein
        lineStream >> node.x >> node.y;

        // Füge den Knoten dem Graphen hinzu
        graph.push_back(node);
    }

    // Schließe die Datei
    file.close();

    // Generiere den Graphen
    for (int i = 0; i < graph.size(); i++) {
        Node& node1 = graph[i];
        for (int j = 0; j < graph.size(); j++) {
            if (i == j) {
                continue;
            }
            Node& node2 = graph[j];

            // Erstelle eine neue Kante
            Edge* edge = new Edge;
            edge->distance = distance(node1.x, node1.y, node2.x, node2.y);

            // Füge die Kante dem ersten Knoten hinzu
            node1.edges.push_back(edge);

            // Füge die Kante auch dem zweiten Knoten hinzu
            node2.edges.push_back(edge);

            // Berechne den Winkel zwischen den beiden Kanten
            double angle = atan2(node2.y - node1.y, node2.x - node1.x);

            // Füge den Winkel zu jeder Kante hinzu, die den gemeinsamen Knoten teilt
            for (Edge* otherEdge : node1.edges) {
                edge->edges.emplace_back(otherEdge, angle - atan2(node1.y - otherEdge->y, node1.x - otherEdge->x));
            }

            // Füge den Winkel auch zu jeder Kante hinzu, die den gemeinsamen Knoten am zweiten Knoten teilt
            for (Edge* otherEdge : node2.edges) {
                edge->edges.emplace_back(otherEdge, angle - atan2(node2.y - otherEdge->y, node2.x - otherEdge->x));
            }
        }
    }

    // Drucke den Graphen
    for (const auto& node : graph) {
        for (auto edge : node.edges) {
            cout << "(" << node.x << "," << node.y << "): ";
            cout << edge->edges.size() << " => ";
            for (auto [otherEdge, angle] : edge->edges) {
                // Suche den Knoten, zu dem die andere Kante geht (ungleich node)
                Node otherNode;
                for (auto n : graph) {
                    if (find(n.edges.begin(), n.edges.end(), otherEdge) != n.edges.end() && n.x != node.x && n.y != node.y) {
                        otherNode = n;
                        break;
                    }
                }
                cout << "(" << otherNode.x << "," << otherNode.y << "): " << angle << ", ";
            }
            cout << endl;
        }
    }
    return 0;
}

