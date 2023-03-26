#include <iostream>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <unordered_set>
#include <map>
#include "main.h"

using namespace std;

//Hash-Funktion für Tupel
struct TupelHash {
    size_t operator()(const tuple<int, int, int> &x) const {
        return get<0>(x) ^ get<1>(x) ^ get<2>(x);
    }
};

/**
 * Sortiert Tupel mit drei Elementen in aufsteigender Reihenfolge
 * @param tupel der zu sortierende Tupel
 */
void sort_tupel(tuple<int, int, int> &tupel) {
    int temp;
    if (get<0>(tupel) > get<1>(tupel)) {
        temp = get<0>(tupel);
        get<0>(tupel) = get<1>(tupel);
        get<1>(tupel) = temp;
    }
    if (get<1>(tupel) > get<2>(tupel)) {
        temp = get<1>(tupel);
        get<1>(tupel) = get<2>(tupel);
        get<2>(tupel) = temp;
        if (get<0>(tupel) > get<1>(tupel)) {
            temp = get<0>(tupel);
            get<0>(tupel) = get<1>(tupel);
            get<1>(tupel) = temp;
        }
    }
}

/**
 * Ermittelt alle möglichen ganzzahligen Kombinationen von Längen, Breiten und Höhen für einen Quader
 * @param volume das maximale Volumen des Quaders
 * @param min die kürzeste Seite des Quaders
 * @param max die längste Seite des Quaders
 * @return alle möglichen Varianten des Quaders
 */
unordered_set<tuple<int, int, int>, TupelHash> find_dimensions(int volume, const int &min, const int &max) {
    unordered_set<tuple<int, int, int>, TupelHash> dimensions;
    for (int l = min; l <= max; l++) {
        for (int w = min; w <= max; w++) {
            for (int h = min; h <= max; h++) {
                if (l * w * h <= volume) {
                    tuple<int, int, int> tupel = make_tuple(l, w, h);
                    sort_tupel(tupel);
                    dimensions.insert(tupel);
                }
            }
        }
    }
    return dimensions;
}

/**
 * Ermittelt rekursiv alle möglichen Kombinationen von Abmessungen für beliebig viele Quader
 * @param volume das Gesamtvolumen für jede Kombination
 * @param squares die maximale Anzahl an Quadern in einer Kombination
 * @param min die minimale Länge einer Seite eines Quaders
 * @param max die maximale Länge einer Seite eines Quaders
 * @param combinations alle Kombinationen
 * @param currentCombination die aktuelle Kombination
 */
void find_combinations(int volume,
                       int squares,
                       const int &min,
                       const int &max,
                       vector<unordered_set<tuple<int, int, int>, TupelHash>> &combinations,
                       unordered_set<tuple<int, int, int>, TupelHash> &currentCombination) {
    //Wenn die aktuelle Kombination vollständig ist
    if (squares == 0 && volume == 0) {
        combinations.push_back(currentCombination);
        return;
    }
    // Wenn das Volumen die aktuelle Kombination nicht aufgeht
    if (volume == 0 || squares == 0) {
        return;
    }
    //Berechnet rekursiv den nächsten Quader der aktuellen Kombination
    unordered_set<tuple<int, int, int>, TupelHash> possibleDimensions = find_dimensions(volume, min, max);
    for (auto dimension: possibleDimensions) {
        currentCombination.insert(dimension);
        find_combinations(volume - get<0>(dimension) * get<1>(dimension) * get<2>(dimension),
                          squares - 1,
                          min,
                          max,
                          combinations,
                          currentCombination);
        currentCombination.erase(dimension);
    }
}

/**
 * Ermittelt alle möglichen Kombinationen von Abmessungen für beliebig viele Quader
 * @param volume das maximale Gesamtvolumen für jede Kombination
 * @param count_of_squares die Anzahl der Quader in jeder Kombination
 * @param slices die gegebene Menge an Scheiben
 * @return alle Kombinationen für die gegebenen Parameter
 */
vector<unordered_set<tuple<int, int, int>, TupelHash>> find_all_combinations(int volume,
                                                                             int count_of_squares,
                                                                             const vector<Slice> &slices) {
    vector<unordered_set<tuple<int, int, int>, TupelHash>> combinations;
    unordered_set<tuple<int, int, int>, TupelHash> currentCombination;

    int max = 0, min = volume;
    //Ermittelt die maximale Seitenlänge eines Quaders
    for (const auto &slice: slices) {
        int side = (slice.length > slice.height) ? slice.length : slice.height;
        if (side > max) {
            max = side;
        }
    }
    //Ermittelt die minimale Seitenlänge eines Quaders
    for (const auto &slice: slices) {
        int side = (slice.length < slice.height) ? slice.length : slice.height;
        if (side < min) {
            min = side;
        }
    }

    find_combinations(volume, count_of_squares, min, max, combinations, currentCombination);
    //Entfernt die Kombinationen, welche nicht der erwarteten Anzahl von Quadern entsprechen
    combinations.erase(
            std::remove_if(combinations.begin(), combinations.end(),
                           [&count_of_squares](auto combination) {
                               return combination.size() != count_of_squares;
                           }), combinations.end()
    );
    return combinations;
}



/**
 * Liest die Eingabedateien ein und versucht für jede Datei eine Lösung entsprechend der Aufgabenstellung zu finden
 * Die Lösung wird anschließend in die entsprechende Ausgabedatei geschrieben
 * Sollte es keine Lösung geben, wird dies ebenfalls in die Ausgabedatei geschrieben
 * @return 0, wenn es zu keinem RuntimeError oder keiner RuntimeException gekommen ist
 */
int main() {
    string input_dir = "../LennartProtte/Aufgabe2-Implementierung/Eingabedateien_b";
    string output_dir = "../LennartProtte/Aufgabe2-Implementierung/Ausgabedateien_b";

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
        int volume = 0;
        for (const auto &slice: slices) {
            volume += slice.length * slice.height;
        }

        //Sortiert die Scheiben nach ihrer Fläche
        sort(slices.begin(), slices.end(), [](Slice a, Slice b) {
                 return (a.length * a.height) > (b.length * b.height);
             }
        );

        vector<pair<Slice, Dimension>> order;
        map<tuple<int, int, int>, vector<pair<Slice, Dimension>>> solution;
        bool success = false;
        //Erhöht die Anzahl der Würfel schrittweise, bis es eine Lösung gibt
        for (int count_of_squares = 1; count_of_squares <= slices.size(); count_of_squares++) {
            const auto &combinations = find_all_combinations(volume, count_of_squares, slices);
            for (const auto &combination: combinations) {
                bool valid = true;
                vector<Slice> t_slices(slices);
                for (auto dimension: combination) {
                    order.clear();
                    if (!calculate_square(get<0>(dimension), get<1>(dimension), get<2>(dimension), order, t_slices)) {
                        valid = false;
                    } else {
                        solution.insert(make_pair(dimension, order));
                    }
                }
                if (valid && t_slices.empty()) {
                    success = true;
                    goto end;
                } else {
                    solution.clear();
                }
            }
        }
        end:
        //Schreibt die Ausgabe
        if (success) {
            fout << "Die Scheiben können zu " << solution.size() << " Quader(n) zusammengesetzt werden." << endl;
            for (auto & it : solution) {
                fout << "Quader: " << get<0>(it.first) << "x" << get<1>(it.first) << "x" << get<2>(it.first) << endl;
                for (auto item: it.second) {
                    fout << "Slice: (" << item.first.length << ", " << item.first.height
                         << ") Dimension: " << to_string(item.second) << endl;
                }
                fout << endl;
            }
        } else {
            fout << "Die Scheiben können zu keinem Quader zusammengesetzt werden." << endl;
        }

        //Dateien schließen
        fin.close();
        fout.close();
    }
    return 0;
}