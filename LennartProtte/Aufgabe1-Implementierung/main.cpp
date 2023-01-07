#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include "structs.h"

/**
Ein Graph besteht aus Knoten (Node) und Kanten (Edge), die die Knoten verbinden.

Die Funktion distance berechnet den Abstand zwischen zwei Punkten (x1, y1) und (x2, y2).

Die Funktion angle berechnet den Winkel zwischen zwei Kanten, die den gleichen Knoten teilen.

Die Funktion generate_graph erstellt einen Graph,
indem sie zunächst zwei Knoten aus dem übergebenen Vektor graph auswählt
und dann zwischen diesen Knoten eine Kante erstellt.
Dazu wird zunächst der Abstand zwischen den beiden Knoten berechnet
und dann die Kante mit diesem Abstand erstellt.
Anschließend wird der Winkel zwischen den beiden Kanten berechnet
und dieser Winkel jeder Kante hinzugefügt, die den gemeinsamen Knoten teilt.

Anschließend wird in der main-Funktion der Pfad zu einem Eingabeordner festgelegt
und ein Iterator erstellt, der alle Dateien im Eingabeordner durchläuft.
Für jede Datei wird der Dateiname und -pfad ausgelesen und eine entsprechende Ausgabedatei erstellt.
Danach wird ein ifstream-Objekt erstellt, um die Eingabedatei zu öffnen und ein ofstream-Objekt,
um die Ausgabedatei zu öffnen.

Es wird dann ein Vektor von Knoten erstellt und für jede Zeile in der Eingabedatei wird ein neuer Knoten hinzugefügt.
Der Knoten enthält die x- und y-Koordinaten, die in der Eingabedatei gelesen werden.
Sobald alle Knoten erstellt wurden, wird die generate_graph-Funktion aufgerufen, um den Graph zu erstellen.

Schließlich wird der Graph in der Ausgabedatei gespeichert,
indem für jeden Knoten alle dazugehörigen Kanten und deren Winkel ausgegeben werden.
 */

using namespace std;

// Berechnet den Abstand zwischen zwei Punkten (x1, y1) und (x2, y2)
double distance(double x1, double y1, double x2, double y2) {
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

// Berechnet den Winkel zwischen zwei Kanten, die den gleichen Knoten teilen
double angle(Edge *e1, Edge *e2) {
    // Berechne den Winkel zwischen den Kanten e1 und e2
    double angle = atan2(e2->node2->y - e2->node1->y, e2->node2->x - e2->node1->x) -
                   atan2(e1->node2->y - e1->node1->y, e1->node2->x - e1->node1->x);
    // Konvertiere den Winkel in Grad
    angle = angle * 180 / M_PI;
    if (angle < 0) {
        angle += 360;
    }
    return angle;
}


vector<Node> generate_graph(vector<Node> graph) {
    for (int i = 0; i < graph.size(); i++) {
        Node node1 = graph[i];
        for (int j = 0; j < graph.size(); j++) {
            if (i == j) {
                continue;
            }
            Node &node2 = graph[j];
            // Erstelle eine neue Kante
            Edge *edge = new Edge;
            edge->node1 = &node1;
            edge->node2 = &node2;
            edge->distance = distance(node1.x, node1.y, node2.x, node2.y);
            // Füge die Kante dem ersten Knoten hinzu
            node1.edges.emplace(edge);
            // Füge die Kante auch dem zweiten Knoten hinzu
            node2.edges.emplace(edge);
            // Füge den Winkel zu jeder Kante hinzu, die den gemeinsamen Knoten teilt
            for (Edge *otherEdge : node1.edges) {
                if (otherEdge == edge) {
                    continue;
                }
                edge->edges.emplace_back(otherEdge, angle(edge, otherEdge));
            }
            // Füge den Winkel auch zu jeder Kante hinzu, die den gemeinsamen Knoten am zweiten Knoten teilt
            for (Edge *otherEdge: node2.edges) {
                if (otherEdge == edge || node1.edges.count(otherEdge) > 0) {
                    continue;
                }
                otherEdge->edges.emplace_back(edge, angle(otherEdge, edge));
            }
        }
    }
    return graph;
}


int main() {
    std::string input_dir = "../LennartProtte/Aufgabe1-Implementierung/TestInput";
    std::string output_dir = "../LennartProtte/Aufgabe1-Implementierung/TestOutput";
    // Iterator erstellen, der alle Dateien im Eingabeordner durchläuft
    for (const auto &entry: std::filesystem::directory_iterator(input_dir)) {
        // Dateiname und -pfad aus dem Iterator auslesen
        std::string input_file = entry.path();
        std::string output_file = output_dir + "/" + entry.path().filename().string();
        // Eingabedatei öffnen
        std::ifstream fin(input_file);
        // Ausgabedatei öffnen
        std::ofstream fout(output_file);

        vector<Node> graph;
        // Solange es noch Koordinaten in der Datei gibt
        double x, y;
        while (fin >> x >> y) {
            Node node;
            node.x = x;
            node.y = y;
            cout << "Added new Node (" << node.x << ", " << node.y << ")" << endl;
            graph.push_back(node);
        }
        // Generiere den Graphen
        graph = generate_graph(graph);
        // Graph in Console schreiben
        for (unsigned int i = 0; i < graph.size(); ++i) {
            std::cout << "Knoten " << i << ": (" << graph[i].x << ", " << graph[i].y << ")" << std::endl;
            std::cout << "Kanten:" << std::endl;
            for (Edge *edge: graph[i].edges) {
                Node *otherNode = (edge->node1 == &graph[i]) ? edge->node2 : edge->node1;
                double angle = atan2(otherNode->y - graph[i].y, otherNode->x - graph[i].x);
                std::cout << " - zu Knoten (" << otherNode->x << ", " << otherNode->y << "): Winkel " << angle << std::endl;
                for (auto &e: edge->edges) {
                    std::cout << " - Kante zwischen (" << edge->node1->x << ", " << edge->node1->y << ") und (" << edge->node2->x << ", " << edge->node2->y << ") hat Winkel " << e.second << " zu Kante von (" << e.first->node1->x << ", " << e.first->node1->y << ") zu (" << e.first->node2->x << ", " << e.first->node2->y << ")" << std::endl;
                }
            }
        }
        std::cout << endl;
        // Graph in Ausgabedatei schreiben
        for (unsigned int i = 0; i < graph.size(); ++i) {
            auto currentNode = graph[i];
            fout << "Knoten " << i << ": (" << currentNode.x << ", " << currentNode.y << ")" << std::endl;
            fout << "Kanten:" << endl;
            for (Edge *edge: graph[i].edges) {
                Node* otherNode;
                if (edge->node1 == &graph[i]) {
                    otherNode = edge->node2;
                } else {
                    otherNode = edge->node1;
                }
                for (auto &e : edge->edges) {
                    fout << "Kanten zwischen ("
                         << currentNode.x << ", " << currentNode.y << ") und ("
                         << otherNode->x << ", " << otherNode->y << ") hat Winkel ";
                    fout << e.second << " zu Kante zwischen ("
                            << e.first->node1->x << ", " << e.first->node1->y << ") und ("
                            << e.first->node2->x << ", " << e.first->node2->y << ")"
                            << std::endl;
                }
            }
        }

        // Dateien schließen
        fin.close();
        fout.close();
    }
    return 0;
}

