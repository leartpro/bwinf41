#include <iostream>
#include <vector>
#include <cmath>
#include <filesystem>
#include <fstream>

using namespace std;

// Struktur für einen Ort
struct Location {
    Location(double px, double py) {
        x = px;
        y  = py;
    }
    double x{};
    double y{};
};

// Funktion zum Berechnen des Abbiegwinkels zwischen zwei Orten
double calculateAngle(Location loc1, Location loc2) {
    double xDiff = loc1.x - loc2.x;
    double yDiff = loc1.y - loc2.y;
    return atan2(yDiff, xDiff) * 180 / M_PI;
}

double calculateDistance(double x1, double y1, double x2, double y2) {
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

/*
 * Für jedes Element der Route:
 *
 */
vector<Location> calculateRoute(vector<Location> locations) {
    // Erstelle leere Liste für die berechnete Route
    vector<Location> route;
    // Wähle den ersten Ort als Startpunkt aus
    Location currentLocation = locations[0];
    // Füge den Startpunkt zur Route hinzu
    route.push_back(currentLocation);
    // Entferne den Startpunkt aus der Liste der verbleibenden Orte
    locations.erase(locations.begin());
    // Solange es noch Orte gibt, die besucht werden müssen...
    while (!locations.empty()) {
        // Setze die minimale Entfernung auf den maximalen Wert
        double minDistance = numeric_limits<double>::max();
        // Setze den Index des nächsten Orts auf -1
        int nextLocationIndex = -1;
        // Gehe alle verbleibenden Orte durch
        for (int i = 0; i < locations.size(); i++) {
            // Berechne die Entfernung zwischen aktuellem Ort und dem verbleibenden Ort
            double distance = calculateDistance(currentLocation.x, currentLocation.y, locations[i].x, locations[i].y);
             // Wenn die Entfernung kleiner als die bisher minimale Entfernung ist...
            if (distance < minDistance) {
                // Setze die minimale Entfernung auf die aktuelle Entfernung
                minDistance = distance;
                // Setze den Index des nächsten Orts auf den Index des aktuellen Orts
                nextLocationIndex = i;
            }
        }
        // Wenn ein nächster Ort gefunden wurde...
        if (nextLocationIndex != -1) {
            // Füge den nächsten Ort zur Route hinzu
            route.push_back(locations[nextLocationIndex]);
             // Setze den aktuellen Ort auf den nächsten Ort
            currentLocation = locations[nextLocationIndex];
            // Entferne den nächsten Ort aus der Liste der verbleibenden Orte
            locations.erase(locations.begin() + nextLocationIndex);
        }
    }
    // Gib die berechnete Route zurück
    return route;
}

int main() {
    std::string input_dir = "../LennartProtte/Aufgabe1-Implementierung/TestInput";
    std::string output_dir = "../LennartProtte/Aufgabe1-Implementierung/TestOutput";
    // Iterator erstellen, der alle Dateien im Eingabeordner durchläuft
    for (const auto &entry: std::filesystem::directory_iterator(input_dir)) {
        // Dateiname und -pfad aus dem Iterator auslesen
        std::string input_file = entry.path();
        std::string output_file = output_dir + "/" + entry.path().filename().string();
        cout << "run for " << entry.path().filename().string() << endl;
        // Eingabedatei öffnen
        std::ifstream fin(input_file);
        // Ausgabedatei öffnen
        std::ofstream fout(output_file);
        vector<Location> locations;
        // Solange es noch Koordinaten in der Datei gibt
        double x, y;
        while (fin >> x >> y) {
            cout << "Added new Node (" << x << ", " << y << ")" << endl;
            locations.push_back(*new Location(x, y));
        }
        cout << endl;
        // Berechne Route mit calculateRoute
        vector<Location> route = calculateRoute(locations);
        // Gebe die mögliche Lösung aus
        for (auto &i: route) {
            cout << "(" << i.x << ", " << i.y << ")" << endl;
        }
        cout << endl;
        // Dateien schließen
        fin.close();
        fout.close();
    }
    return 0;
}