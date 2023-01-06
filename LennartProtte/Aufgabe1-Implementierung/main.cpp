#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <unordered_set>

struct Node {
    double x{};
    double y{};
    std::vector<std::pair<Node *, double>> edges;
};

// Repräsentiert einen Eintrag in der offenen Liste des A*-Algorithmus
struct OpenListEntry {
    Node *node; // der Knoten, der von diesem Eintrag repräsentiert wird
    double g; // der tatsächlich zurückgelegte Weg vom Startknoten aus
    double f; // die Summe aus g und dem geschätzten Restweg zum Ziel
    Node *predecessor; // der Vorgängerknoten auf dem Weg vom Startknoten zu diesem Knoten
};

// Vergleicht zwei OpenListEntry-Objekte nach ihrem f-Wert
bool openListEntryCompare(const OpenListEntry &lhs, const OpenListEntry &rhs) {
    return lhs.f < rhs.f;
}

double heuristic(const Node &from, const Node &to) {
    // Berechne die gewichtete Distanz zwischen den beiden Knoten als Schätzung
    // für den Restweg.
    double minDistance = std::numeric_limits<double>::max();
    for (const auto &edge : from.edges) {
        if (edge.first == &to) {
            minDistance = std::min(minDistance, edge.second);
        }
    }
    return minDistance;
}

std::vector<Node *> aStar(Node &start, Node &goal) {
    std::vector<Node *> path; // wird am Ende den Pfad vom Start zum Ziel enthalten
    std::vector<OpenListEntry> openList; // die offene Liste des Algorithmus
    std::unordered_set<Node *> closedList; // die geschlossene Liste des Algorithmus
    // Füge den Startknoten in die offene Liste ein und initialisiere seine
    // Eintrags-Werte.
    openList.push_back({&start, 0.0, heuristic(start, goal), nullptr});
    while (!openList.empty()) {
        // Suche den Knoten mit dem geringsten f-Wert in der offenen Liste
        auto currentIt = std::min_element(openList.begin(), openList.end(), openListEntryCompare);
        OpenListEntry current = *currentIt;
        openList.erase(currentIt);
        closedList.insert(current.node);
        // Wurde das Ziel erreicht?
        if (current.node == &goal) {
            // Baue den Pfad auf
            path.push_back(current.node);
            while (current.node != &start) {
                path.push_back(current.predecessor);
                current = *std::find_if(openList.begin(), openList.end(), [&](const OpenListEntry &entry) {
                    return entry.node == current.predecessor;
                });
            }
            // Kehre den Pfad um, damit er von Start zu Ziel verläuft
            std::reverse(path.begin(), path.end());
            return path;
        }
        // Für jeden Nachbarknoten des aktuellen Knotens...
        for (const auto &edge: current.node->edges) {
            Node *neighbor = edge.first;
            // ..., der noch nicht in der geschlossenen Liste ist...
            if (closedList.find(neighbor) == closedList.end()) {
                // ..., berechne den tatsächlich zurückgelegten Weg...
                double g = current.g + edge.second;
                // ... und den geschätzten Restweg...
                double h = heuristic(*neighbor, goal);
                // ... und die f-Summe...
                double f = g + h;
                // ... und suche den Knoten in der offenen Liste...
                auto openListNeighborIt = std::find_if(openList.begin(), openList.end(),
                                                       [&](const OpenListEntry &entry) {
                                                           return entry.node == neighbor;
                                                       });
                // ..., falls er dort enthalten ist.
                if (openListNeighborIt != openList.end()) {
                    OpenListEntry &openListNeighbor = *openListNeighborIt;
                    // Wurde ein besserer Weg zu diesem Knoten gefunden?
                    if (g < openListNeighbor.g) {
                    // Aktualisiere den Eintrag in der offenen Liste
                        openListNeighbor.g = g;
                        openListNeighbor.f = f;
                        openListNeighbor.predecessor = current.node;
                    }
                } else {
                    openList.push_back({neighbor, g, f, current.node});
                }
            }
            // Wenn die offene Liste leer ist, konnte kein Pfad gefunden werden
            return {};
        }
    }
    return path;
}


double distance(Node *a, Node *b) {
    double dx = a->x - b->x;
    double dy = a->y - b->y;
    return std::sqrt(dx * dx + dy * dy);
}

double angle(Node *a, Node *b, Node *c) {
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
    std::ifstream file("../LennartProtte/Aufgabe1-Implementierung/Eingabedateien/wenigerkrumm1.txt");
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
    for (auto &node: nodes) {
        for (auto &edge: node.edges) {
            Node *other = edge.first;
            double angleSum = 0;
            for (auto &otherEdge: other->edges) {
                if (otherEdge.first != &node) {
                    angleSum += angle(&node, other, otherEdge.first);
                }
            }
            edge.second = angleSum;
        }
    }

    // Drucke den Graphen aus
    int numNodes = nodes.size();
    std::cout << std::setw(10) << " ";
    for (int i = 0; i < numNodes; i++) {
        if (i == 0) {
            std::cout << std::setw(10) << "(" << nodes[i].x << ", " << nodes[i].y << ")";
        } else {
            std::cout << std::setw(3) << "(" << nodes[i].x << ", " << nodes[i].y << ")";
        }
    }
    std::cout << std::endl;
    for (int i = 0; i < numNodes; i++) {
        std::cout << std::setw(10) << "(" << nodes[i].x << ", " << nodes[i].y << ")";
        for (int j = 0; j < numNodes; j++) {
            double angle = 0;
            if (i != j) {
                for (auto &edge: nodes[i].edges) {
                    if (edge.first == &nodes[j]) {
                        angle = edge.second;
                        break;
                    }
                }
            }
            std::cout << std::setw(10) << angle;
        }
        std::cout << std::endl;
    }

    // Rufe den A*-Algorithmus auf und speichere das Ergebnis
    auto result = aStar(nodes[0], nodes[nodes.size() - 1]);
    std::cout << "Path: " << std::endl;
    // Gehe das Ergebnis durch und gebe den Pfad aus
    for (auto &node: result) {
        std::cout << "(" << node->x << ", " << node->y << ")" << std::endl;
    }
    return 0;
}
