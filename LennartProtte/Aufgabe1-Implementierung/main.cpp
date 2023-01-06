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

// Berechnet den Winkel zwischen zwei Kanten, die den gleichen Knoten teilen
double angle(Edge* e1, Edge* e2) {
    double dotProduct = e1->distance * e2->distance;
    double lengthProduct = e1->distance * e2->distance;
    double angle = acos(dotProduct / lengthProduct);
    return angle /* * 180 / M_PI*/;
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

    // Erstelle Kanten zwischen allen Knoten im Graphen
    for (size_t i = 0; i < graph.size(); i++) {
        Node *n1 = &graph[i];

        for (size_t j = i + 1; j < graph.size(); j++) {
            Node *n2 = &graph[j];

            // Erstelle eine neue Kante zwischen den Knoten n1 und n2
            Edge *e = new Edge;
            e->distance = distance(n1->x, n1->y, n2->x, n2->y);

            // Füge die Kante den Kantenlisten der Knoten hinzu
            n1->edges.push_back(e);
            n2->edges.push_back(e);
        }
    }
    // Berechne Winkel zwischen allen Kanten, die den gleichen Knoten teilen
    for (auto &node: graph) {
        for (size_t i = 0; i < node.edges.size(); i++) {
            Edge *e1 = node.edges[i];

            for (size_t j = i + 1; j < node.edges.size(); j++) {
                Edge *e2 = node.edges[j];

                // Berechne den Winkel zwischen den Kanten e1 und e2 und speichere ihn in der Liste der Kanten von e1
                e1->edges.emplace_back(e2, angle(e1, e2));

                // Füge den Winkel auch in der Liste der Kanten von e2 hinzu
                e2->edges.emplace_back(e1, angle(e1, e2));
            }
        }
    }

    // Drucke den Graphen
    for (auto node : graph) {
        for (auto edge : node.edges) {
            cout << "(" << node.x << "," << node.y << "): ";
            for (auto [otherEdge, angle] : edge->edges) {
                // Prüfe, ob otherEdge und edge den gleichen Knoten teilen
                if (find(node.edges.begin(), node.edges.end(), otherEdge) != node.edges.end()) {
                    cout << "Winkel zu Kante von " << "(" << otherEdge->distance << ", " << otherEdge->distance << "): " << angle << ", ";
                }
            }

            cout << endl;
        }
    }


    return 0;
}
