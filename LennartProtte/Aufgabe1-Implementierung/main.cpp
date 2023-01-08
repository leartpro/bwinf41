#include <iostream>
#include <vector>
#include <cmath>
#include <filesystem>
#include <fstream>

using namespace std;

// Struktur für einen Ort
struct Location {
    double x;
    double y;
};

// Struktur für einen Ort und dazugehörigen Abbiegwinkel
struct LocationWithAngle {
    Location location;
    double angle;
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

// Funktion zum Überprüfen, ob eine Route gültig ist (keine scharfen Abbiegungen enthält)
bool isValidRoute(vector<LocationWithAngle> locations) {
// Gehe alle aufeinanderfolgenden Orte durch
    for (int i = 1; i < locations.size(); i++) {
// Wenn der Abbiegwinkel größer als 90 Grad ist...
        if (abs(locations[i].angle) > 90) {
// Gib false zurück
            return false;
        }
    }
// Gib true zurück (keine scharfen Abbiegungen gefunden)
    return true;
}

double calculateTotalAngle(vector<Location> locations) {
    // Initialisiere den Gesamtabbiegwinkel mit 0
    double totalAngle = 0;

    // Gehe alle aufeinanderfolgenden Orte durch
    for (int i = 1; i < locations.size(); i++) {
        // Berechne Abbiegwinkel zwischen aktuellem und vorherigem Ort
        double angle = calculateAngle(locations[i - 1], locations[i]);
        // Addiere Abbiegwinkel zum Gesamtabbiegwinkel
        totalAngle += angle;
    }

    // Gib den Gesamtabbiegwinkel zurück
    return totalAngle;
}


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

void search(vector<LocationWithAngle> locations, vector<vector<LocationWithAngle>> &solutions,
            vector<LocationWithAngle> currentLocations) {
    // Wenn es keine Orte mehr gibt, die hinzugefügt werden können...
    if (locations.empty()) {
        // Wenn die aktuelle Route gültig ist (keine scharfen Abbiegungen enthält)...
        if (isValidRoute(currentLocations)) {
            // Füge die aktuelle Route zu solutions hinzu
            solutions.push_back(currentLocations);
        }
        return;
    }
    // Gehe alle verbleibenden Orte durch
    for (int i = 0; i < locations.size(); i++) {
        // Berechne Abbiegwinkel zwischen aktuellem und vorherigem Ort
        double angle = currentLocations.empty() ? 0.0 : calculateAngle(currentLocations.back().location,
                                                                       locations[i].location);
        // Erstelle LocationWithAngle Objekt für aktuellen Ort mit Abbiegwinkel
        LocationWithAngle currentLocationWithAngle = {locations[i].location, angle};
        // Füge aktuellen Ort zu aktueller Route hinzu
        currentLocations.push_back(currentLocationWithAngle);

        // Erstelle neue Liste der verbleibenden Orte
        vector<LocationWithAngle> remainingLocations;

        // Füge alle Orte außer dem aktuellen hinzu
        for (int j = 0; j < locations.size(); j++) {
            if (j != i) {
                remainingLocations.push_back(locations[j]);
            }
        }


        // Führe die Suche mit verbleibenden Orten fort
        search(remainingLocations, solutions, currentLocations);

        // Entferne aktuellen Ort wieder aus aktueller Route
        currentLocations.pop_back();
    }
}

vector<vector<LocationWithAngle>> solveProblem(vector<LocationWithAngle> locations) {
// Erstelle leere Liste von möglichen Lösungen
    vector<vector<LocationWithAngle>> solutions;

// Erstelle leere Liste von aktuellen Orten
    vector<LocationWithAngle> currentLocations;

// Führe die Brute-Force-Suche aus
    search(std::move(locations), solutions, currentLocations);

// Gebe die Liste der möglichen Lösungen zurück
    return solutions;
}


int main2() {
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
        vector<LocationWithAngle> locations;
        // Solange es noch Koordinaten in der Datei gibt
        double x, y;
        while (fin >> x >> y) {
            LocationWithAngle location{};
            location.location.x = x;
            location.location.y = y;
            location.angle = 0;
            cout << "Added new Node (" << location.location.x << ", " << location.location.y << ")" << endl;
            locations.push_back(location);
        }
        cout << endl;
        //Löse das Problem mithilfe der Brute-Force-Suche
        vector<vector<LocationWithAngle>> solutions = solveProblem(locations);
        // Gebe die möglichen Lösungen aus
        for (int i = 0; i < solutions.size(); i++) {
            fout << "Lösung " << i + 1 << ": ";
            for (auto &j: solutions[i]) {
                fout << "(" << j.location.x << ", " << j.location.y << ") " << j.angle << "° ";
            }
            fout << endl;
        }
        // Dateien schließen
        fin.close();
        fout.close();
    }
    return 0;
}

int main3() {
// Erstelle Liste mit Orten
    vector<Location> locations = {{400.0, 100.0}, {300.0, 150.0}, {200.0, 200.0}, {150.0, 300.0}, {100.0, 400.0},
                                  {150.0, 500.0}, {200.0, 600.0}, {300.0, 650.0}, {400.0, 700.0}, {500.0, 650.0},
                                  {600.0, 600.0}, {650.0, 500.0}, {700.0, 400.0}, {650.0, 300.0}, {600.0, 200.0},
                                  {500.0, 150.0}};
// Berechne Route mit calculateRoute
    vector<Location> route = calculateRoute(locations);
// Berechne Gesamt-Abbiegwinkel mit calculateTotalAngle
    double totalAngle = calculateTotalAngle(route);
// Gib die Länge der Route und den Gesamt-Abbiegwinkel aus
    //cout << "Route length: " << calculateDistance(route) << " km" << endl;
    cout << "Total angle: " << totalAngle << " degrees" << endl;

    // Gebe die möglichen Lösungen aus
    for (auto & i : route) {
            cout << "(" << i.x << ", " << i.y << ") ";
    }
    cout << endl;
    return 0;
}