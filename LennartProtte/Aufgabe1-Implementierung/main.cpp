#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include "structs.h"


using namespace std;

// Berechnet den Abstand zwischen zwei Punkten (x1, y1) und (x2, y2)
double distance(double x1, double y1, double x2, double y2) {
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}


int main() {
    // Öffne Datei zum Lesen
    ifstream file("../LennartProtte/Aufgabe1-Implementierung/Eingabedateien/simplegraph.txt");
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
            edge->node1 = &node1;
            edge->node2 = &node2;
            edge->distance = distance(node1.x, node1.y, node2.x, node2.y);

            // Füge die Kante dem ersten Knoten hinzu
            node1.edges.push_back(edge);

            // Füge die Kante auch dem zweiten Knoten hinzu
            node2.edges.push_back(edge);

            // Berechne den Winkel zwischen den beiden Kanten
            double angle = atan2(node2.y - node1.y, node2.x - node1.x);

            // Füge den Winkel zu jeder Kante hinzu, die den gemeinsamen Knoten teilt
            for (Edge* otherEdge : node1.edges) {
                if (otherEdge == edge) {
                    continue;
                }
                edge->edges.emplace_back(otherEdge, angle - atan2(otherEdge->node2->y - node1.y, otherEdge->node2->x - node1.x));
            }

            // Füge den Winkel auch zu jeder Kante hinzu, die den gemeinsamen Knoten am zweiten Knoten teilt
            for (Edge* otherEdge : node2.edges) {
                if (otherEdge == edge) {
                    continue;
                }
                edge->edges.emplace_back(otherEdge, angle - atan2(otherEdge->node1->y - node2.y, otherEdge->node1->x - node2.x));
            }
        }
    }

    // Drucke den Graphen
    for (const auto& node : graph) {
        for (auto edge : node.edges) {
            /*
            // Ausgabe der ersten Kante
            cout << "(" << node.x << "," << node.y << ")-" << edge->distance << "->(";
            if (edge->node1 == &node) {
                cout << edge->node2->x << "," << edge->node2->y << "): ";
            } else {
                cout << edge->node1->x << "," << edge->node1->y << "): ";
            }
             */

            // Ausgabe der Kanten zu Kanten Beziehungen
            for (auto [otherEdge, angle] : edge->edges) {
                // Ausgabe der zweiten Kante
                cout << "(" << otherEdge->node1->x << "," << otherEdge->node1->y << ")-" << otherEdge->distance << "->(";
                if (otherEdge->node1 == edge->node1 || otherEdge->node1 == edge->node2) {
                    cout << otherEdge->node2->x << "," << otherEdge->node2->y << "), " << angle << "° zu ";
                } else {
                    cout << otherEdge->node1->x << "," << otherEdge->node1->y << "), " << angle << "° zu ";
                }
            }
            cout << endl;
        }
    }

    return 0;
}

