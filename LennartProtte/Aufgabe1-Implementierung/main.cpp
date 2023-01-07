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

// Funktion zum Berechnen des Abbiegwinkels zwischen zwei Orten
double calculateAngle(Location loc1, Location loc2) {
    double xDiff = loc1.x - loc2.x;
    double yDiff = loc1.y - loc2.y;
    return atan2(yDiff, xDiff) * 180 / M_PI;
}

// Funktion zum Überprüfen, ob eine Route gültig ist (keine scharfen Abbiegungen enthält)
bool isValidRoute(vector<Location> locations) {
    // Gehe alle aufeinanderfolgenden Orte durch
    for (int i = 1; i < locations.size(); i++) {
        // Berechne Abbiegwinkel zwischen aktuellem und vorherigem Ort
        double angle = calculateAngle(locations[i-1], locations[i]);
        // Wenn der Abbiegwinkel größer als 90 Grad ist...
        if (abs(angle) > 90) {
            // Gib false zurück
            return false;
        }
    }
    // Gib true zurück (keine scharfen Abbiegungen gefunden)
    return true;
}

void search(vector<Location> locations, vector<vector<Location>>& solutions, vector<Location> currentLocations) {
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
        // Füge aktuellen Ort zu aktueller Route hinzu
        currentLocations.push_back(locations[i]);

        // Kopiere verbleibende Orte in neue Liste
        vector<Location> remainingLocations = locations;

        // Entferne aktuellen Ort aus verbleibenden Orten
        remainingLocations.erase(remainingLocations.begin() + i);

        // Führe die Suche mit verbleibenden Orten fort
        search(remainingLocations, solutions, currentLocations);

        // Entferne aktuellen Ort wieder aus aktueller Route
        currentLocations.pop_back();
    }
}


// Funktion zur Lösung des Problems mithilfe der Brute-Force-Suche
vector<vector<Location>> solveProblem(vector<Location> locations) {
    // Erstelle leere Liste von möglichen Lösungen
    vector<vector<Location>> solutions;

    // Erstelle leere Liste von aktuellen Orten
    vector<Location> currentLocations;

    // Führe die Brute-Force-Suche aus
    search(std::move(locations), solutions, currentLocations);

    // Gebe die Liste der möglichen Lösungen zurück
    return solutions;
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
        vector<Location> locations;
        // Solange es noch Koordinaten in der Datei gibt
        double x, y;
        while (fin >> x >> y) {
            Location location{};
            location.x = x;
            location.y = y;
            cout << "Added new Node (" << location.x << ", " << location.y << ")" << endl;
            locations.push_back(location);
        }
        //Löse das Problem mithilfe der Brute-Force-Suche
        vector<vector<Location>> solutions = solveProblem(locations);

        // Gebe die möglichen Lösungen aus
        for (int i = 0; i < solutions.size(); i++) {
            cout << "Lösung " << i + 1 << ": ";
            for (auto &j: solutions[i]) {
                cout << "(" << j.x << ", " << j.y << ") ";
            }
            cout << endl;
        }
        // Dateien schließen
        fin.close();
        fout.close();
    }
    return 0;
}