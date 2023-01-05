#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>

struct Node {
    double x{};
    double y{};
    std::vector<std::pair<Node*, double>> edges;
};

double distance(Node* a, Node* b) {
    double dx = a->x - b->x;
    double dy = a->y - b->y;
    return std::sqrt(dx * dx + dy * dy);
}

double angle(Node* a, Node* b, Node* c) {
    double dx1 = b->x - a->x;
    double dy1 = b->y - a->y;
    double dx2 = c->x - a->x;
    double dy2 = c->y - a->y;
    double dot = dx1 * dx2 + dy1 * dy2;
    double det = dx1 * dy2 - dx2 * dy1;
    double angle = std::atan2(det, dot) * 180 / M_PI;
    return angle;
}

int main() {
    std::vector<Node> nodes;

    // Lies die Datei ein
    std::ifstream file("../LennartProtte/Aufgabe1-Implementierung/Eingabedateien/simplegraph.txt");
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        Node node;
        if (!(iss >> node.x >> node.y)) {
            break; // Error beim Parsen der Koordinaten
        }
        nodes.push_back(node);
    }

    // Erstelle den Graphen
    for (int i = 0; i < nodes.size(); i++) {
        for (int j = i + 1; j < nodes.size(); j++) {
            double dist = distance(&nodes[i], &nodes[j]);
            nodes[i].edges.emplace_back(&nodes[j], dist);
            nodes[j].edges.emplace_back(&nodes[i], dist);
        }
    }

    // Berechne den Winkel an den Knoten
    for (auto& node : nodes) {
        for (auto& edge : node.edges) {
            Node* other = edge.first;
            double angleSum = 0;
            for (auto& otherEdge : other->edges) {
                if (otherEdge.first != &node) {
                    angleSum += angle(&node, other, otherEdge.first);
                }
            }
            edge.second = angleSum;
        }
    }

    for (int i = 0; i < nodes.size(); i++) {
        std::cout << "Knoten " << i << ": (" << nodes[i].x << ", " << nodes[i].y << ")" << std::endl;
        std::cout << "Kanten:" << std::endl;
        for (auto& edge : nodes[i].edges) {
            int otherIndex = edge.first - &nodes[0];
            double angle = edge.second;
            std::cout << "  - Knoten " << otherIndex << ": Winkel " << angle << std::endl;
        }
    }


    return 0;
}
