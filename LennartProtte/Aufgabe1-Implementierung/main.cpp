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

// Berechnet den geschätzten Restweg (h-Wert) zwischen zwei Knoten
double heuristic(const Node &from, const Node &to) {
// Berechne die euklidische Distanz zwischen den beiden Knoten als Schätzung
// für den Restweg. Es gibt jedoch auch andere Möglichkeiten, den h-Wert zu
// berechnen, wie z.B. die Manhattan-Distanz oder die Diagonaldistanz.
    return std::sqrt(std::pow(from.x - to.x, 2) + std::pow(from.y - to.y, 2));
}

std::vector<Node *> aStar(Node &start, Node &goal) {
    std::vector<Node *> path; // wird am Ende den Pfad vom Start zum Ziel enthalten
    std::vector<OpenListEntry> openList; // die offene Liste des Algorithmus
    std::unordered_set<Node *> closedList; // die geschlossene Liste des Algorithmus

    // Füge den Startknoten in die offene Liste ein und initialisiere seine
    // g- und f-Werte
    openList.push_back({&start, 0, heuristic(start, goal), nullptr});

    // Solange die offene Liste nicht leer ist, wiederhole:
    while (!openList.empty()) {
        // Suche den Eintrag in der offenen Liste mit dem kleinsten f-Wert
        auto minIt = std::min_element(openList.begin(), openList.end(), openListEntryCompare);

        // Entferne den Eintrag aus der offenen Liste und füge ihn zur geschlossenen Liste hinzu
        Node *currentNode = minIt->node;
        openList.erase(minIt);
        closedList.insert(currentNode);

        // Wenn der aktuelle Knoten das Ziel ist, ist der Pfad gefunden
        if (currentNode == &goal) {
            // Erstelle den Pfad, indem du den Vorgängerknoten jedes Knotens zurückverfolgst
            while (currentNode != nullptr) {
                path.push_back(currentNode);
                currentNode = minIt->predecessor;
            }
            // Kehre den Pfad um, damit er von Start zu Ziel verläuft
            std::reverse(path.begin(), path.end());
            return path;
        }

        // Für jede ausgehende Kante des aktuellen Knotens:
        for (const auto &edge: currentNode->edges) {
            Node *successor = edge.first;
            // Wenn der Nachfolgeknoten bereits in der geschlossenen Liste ist, ignoriere ihn
            if (closedList.count(successor)) continue;

            // Berechne den g-Wert des Nachfolgeknotens aus dem g-Wert des aktuellen Knotens und
            // der Kosten der Kante zwischen ihnen
            double tentativeG = minIt->g + heuristic(*currentNode, *successor);

            // Wenn der Nachfolgeknoten noch nicht in der offenen Liste ist, füge ihn ein und
            // initialisiere seine g- und f-Werte
            bool isNewNode = true;
            for (auto &entry: openList) {
                if (entry.node == successor) {
                    isNewNode = false;
                    break;
                }
            }

            if (isNewNode) {
                openList.push_back({successor, tentativeG, tentativeG + heuristic(*successor, goal), currentNode});
            } else {
                // Wenn der Nachfolgeknoten bereits in der offenen Liste ist, überprüfe, ob der aktuelle
                // Pfad zu ihm eine kürzere Gesamtlänge hat als der bisherige Pfad. Wenn ja, aktualisiere
                // den Eintrag in der offenen Liste entsprechend.
                for (auto &entry: openList) {
                    if (entry.node == successor) {
                        if (tentativeG < entry.g) {
                            entry.g = tentativeG;
                            entry.f = tentativeG + heuristic(*successor, goal);
                            entry.predecessor = currentNode;
                        }
                        break;
                    }
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
