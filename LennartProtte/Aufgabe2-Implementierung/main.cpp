#include <iostream>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <fstream>

using namespace std;

/**
 * Repräsentiert eine Käsescheibe
 */
struct Slice {
    Slice(int p_length, int p_height) {
        this->length = p_length;
        this->height = p_height;
    }

    int length, height;
};


/**
 * Repräsentiert eine Dimension
 */
enum Dimension {
    FRONT,
    TOP,
    SIDE,
    INVALID
};

/*
 * Dimension als String
 */
constexpr const char *to_string(Dimension dimension) {
    switch (dimension) {
        case Dimension::FRONT:
            return "front";
        case Dimension::TOP:
            return "top";
        case Dimension::SIDE:
            return "side";
        case Dimension::INVALID:
            return "invalid";
    }
}


/**
 * Prüft, ob eine Scheibe, von einem Quader abgeschnitten werden kann
 * @param length die Länge des Quaders
 * @param height die Höhe des Quaders
 * @param depth die Tiefe des Quaders
 * @param slice die Scheibe, auf die getestet wird
 * @return -1, wenn die Scheibe nicht entfernt werden kann,
 * ansonsten wird die Dimension zurückgegeben, wo diese entfernt werden kann
 * Dimensionen:
 * 0 => VORNE
 * 1 => OBEN
 * 2 => SEITE
 */
Dimension canRemoveSlice(int length, int height, int depth, Slice slice) {
    if (slice.length == length && slice.height == height || slice.height == length && slice.length == height) {
        return FRONT;
    } else if (slice.length == length && slice.height == depth || slice.height == length && slice.length == depth) {
        return TOP;
    } else if (slice.length == height && slice.height == depth || slice.height == height && slice.length == depth) {
        return SIDE;
    } else {
        return INVALID;
    }
}

// Hilfsfunktion, um alle möglichen Kombinationen von Längen, Breiten und Höhen für einen Quader zu finden
vector<tuple<int, int, int>> findDimensions(int volume) {
    vector<tuple<int, int, int>> dimensions;
    for (int l = 1; l <= volume; l++) {
        for (int w = 1; w <= volume; w++) {
            for (int h = 1; h <= volume; h++) {
                if (l * w * h <= volume) {
                    dimensions.emplace_back(l, w, h);
                }
            }
        }
    }
    return dimensions;
}

// Rekursive Funktion, um alle möglichen Kombinationen von Abmessungen für n-Quadern zu finden
void findCombinations(int remainingVolume,
                      int remainingQuads,
                      vector<vector<tuple<int, int, int>>> &combinations,
                      vector<tuple<int, int, int>> &currentCombination) {
    // Basisfall: Wenn alle Quadern ausgewählt wurden und das Gesamtvolumen erreicht wurde, fügen wir die aktuelle Kombination zur Liste der Kombinationen hinzu
    if (remainingQuads == 0 && remainingVolume == 0) {
        combinations.push_back(currentCombination);
        return;
    }
    // Wenn das Gesamtvolumen erreicht wurde, aber noch Quadern übrig sind, brechen wir ab
    if (remainingVolume == 0 || remainingQuads == 0) {
        return;
    }
    // Wir durchlaufen alle möglichen Abmessungen für den nächsten Quader
    vector<tuple<int, int, int>> possibleDimensions = findDimensions(remainingVolume);
    for (auto dimension: possibleDimensions) {
        currentCombination.push_back(dimension);
        findCombinations(remainingVolume - get<0>(dimension) * get<1>(dimension) * get<2>(dimension),
                         remainingQuads - 1, combinations, currentCombination);
        currentCombination.pop_back();
    }
}

// Hauptfunktion, um alle möglichen Kombinationen von Abmessungen für n-Quadern zu finden
vector<vector<tuple<int, int, int>>> findAllCombinations(int totalVolume, int numQuads) {
    vector<vector<tuple<int, int, int>>> combinations;
    vector<tuple<int, int, int>> currentCombination;
    findCombinations(totalVolume, numQuads, combinations, currentCombination);
    return combinations;
}

/**
 * Versucht rekursiv durch backtracking eine Lösungsreihenfolge für die gegebene Menge an Scheiben zu finden
 * Dabei werden Scheiben von slices nach order verschoben, so dass order am Ende die Lösungsmenge enthält
 * Am Anfang wird order ohne Elemente erwartet und sollte es eine Lösung geben, enthält order alle Scheiben aus slices
 * @param length die Länge des Quaders
 * @param height die Höhe des Quaders
 * @param depth die Tiefe des Quaders
 * @param order die Lösungsreihenfolge
 * @param slices die noch nicht verwendete Menge an Scheiben
 * @return true, wenn es eine lösung gibt, ansonsten false
 */
bool calculate_cube(int length, int height, int depth, vector<pair<Slice, Dimension>> &order, vector<Slice> &slices) {

    //Wenn mindestens eine der Seiten auf null ist (daher das Volumen des Quaders null ist)
    if(length == 0 || height == 0 || depth == 0) {
        return true;
    }
    vector<Slice> removed_slices;
    //Für jede noch nicht verwendete Schiebe
    for (auto it = slices.begin(); it != slices.end(); ++it) {
        Dimension dimension = canRemoveSlice(length, height, depth, *it);
        //Wenn die aktuelle Scheibe abgeschnitten werden kann
        if (dimension != INVALID) {
            //Aktualisiere die Maße des Quaders
            int new_length = length - (dimension == SIDE ? 1 : 0);
            int new_height = height - (dimension == TOP ? 1 : 0);
            int new_depth = depth - (dimension == FRONT ? 1 : 0);
            //Füge die Scheibe zur Lösungsmenge hinzu
            order.emplace_back(*it, dimension);
            removed_slices.push_back(*it);
            slices.erase(it);
            //Wenn es eine Lösung mit der aktuellen Scheibe gibt
            if (calculate_cube(new_length, new_height, new_depth, order, slices)) {
                return true;
            } else {
                //Entferne die aktuelle Scheibe von der Lösungsmenge
                order.pop_back();
                for (auto &slice: removed_slices) {
                    slices.push_back(slice);
                }
                removed_slices.clear();
            }
        }
    }
    //Wenn keine der noch nicht betrachteten Scheiben verwendet werden kann
    return false;
}

/**
 * Liest die Eingabedateien ein und versucht für jede Datei eine Lösung entsprechend der Aufgabenstellung zu finden
 * Die Lösung wird anschließend in die entsprechende Ausgabedatei geschrieben
 * Sollte es keine Lösung geben, wird dies ebenfalls in die Ausgabedatei geschrieben
 * @return 0, wenn es zu keinem RuntimeError oder keiner RuntimeException gekommen ist
 */
int main() {
    string input_dir = "../LennartProtte/Aufgabe2-Implementierung/erweiterte_Eingabedateien";
    string output_dir = "../LennartProtte/Aufgabe2-Implementierung/erweiterte_Ausgabedateien";

    //Durchläuft alle Dateien im Eingabeordner
    for (const auto &entry: filesystem::directory_iterator(input_dir)) {

        //Liest den Dateinamen aus
        string input_file = entry.path();
        string output_file = output_dir + "/" + entry.path().filename().string();

        //Öffnet die Eingabedatei
        ifstream fin(input_file);

        //Öffnet die Ausgabedatei
        ofstream fout(output_file);

        //Liest die Eingabedatei ein
        vector<Slice> slices;
        int a, b, n;
        fin >> n;
        while (fin >> a >> b) {
            slices.emplace_back(a, b);
        }

        //Berechnet das Volumen des Quaders
        int volume = 0, height = 0;
        for (const auto &slice: slices) {
            volume += slice.length * slice.height;
        }

        //Sortiert die Scheiben nach ihrer Fläche
        sort(slices.begin(), slices.end(), [](Slice a, Slice b) {
                 return (a.length * a.height) > (b.length * b.height);
             }
        );

        vector<pair<Slice, Dimension>> order;
        for (int count_of_cubes = 1; count_of_cubes < slices.size(); count_of_cubes++) {
            vector<vector<tuple<int, int, int>>> combinations = findAllCombinations(volume, count_of_cubes);
            for (const auto &combination: combinations) {
                bool valid = true;
                vector<Slice> t_slices(slices);
                for (auto dimension: combination) {
                    order.clear();
                    if (!calculate_cube(get<0>(dimension), get<1>(dimension), get<2>(dimension), order,t_slices)) {
                        valid = false;
                    }
                }
                if (valid && t_slices.empty()) {
                    fout << "Quader:" << endl;
                    for(auto dimension : combination) {
                        fout << get<0>(dimension) << "x" << get<1>(dimension) << "x" << get<2>(dimension) << endl;
                    }
                    break;
                }
            }
        }

        //Dateien schließen
        fin.close();
        fout.close();
    }
    return 0;
}