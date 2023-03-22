#include <iostream>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <unordered_set>

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

//Wird für die Tupel benötigt
struct hashFunction {
    size_t operator()(const tuple<int, int, int> &x) const {
        return get<0>(x) ^ get<1>(x) ^ get<2>(x);
    }
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
 * @return die Dimension, wo die Scheibe abgeschnitten werden kann
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
unordered_set<tuple<int, int, int>, hashFunction> findDimensions(int volume, const int &min, const int &max) {
    unordered_set<tuple<int, int, int>, hashFunction> dimensions;
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
 * @param min die minimale Länge einer Quaderseite
 * @param max die maximale Länge einer Quaderseite
 * @param combinations alle Kombinationen
 * @param currentCombination die aktuelle Kombination
 */
void findCombinations(int volume,
                      int squares,
                      const int &min,
                      const int &max,
                      vector<unordered_set<tuple<int, int, int>, hashFunction>> &combinations,
                      unordered_set<tuple<int, int, int>, hashFunction> &currentCombination) {
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
    unordered_set<tuple<int, int, int>, hashFunction> possibleDimensions = findDimensions(volume, min, max);
    for (auto dimension: possibleDimensions) {
        currentCombination.insert(dimension);
        findCombinations(volume - get<0>(dimension) * get<1>(dimension) * get<2>(dimension),
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
vector<unordered_set<tuple<int, int, int>, hashFunction>> findAllCombinations(int volume,
                                                                              int count_of_squares,
                                                                              const vector<Slice> &slices) {
    vector<unordered_set<tuple<int, int, int>, hashFunction>> combinations;
    unordered_set<tuple<int, int, int>, hashFunction> currentCombination;

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

    findCombinations(volume, count_of_squares, min, max, combinations, currentCombination);
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
 * Ermittelt rekursiv durch Backtracking eine Lösungsreihenfolge für die gegebene Menge an Scheiben.
 * @param length die Länge des Quaders
 * @param height die Höhe des Quaders
 * @param depth die Tiefe des Quaders
 * @param order die Lösungsreihenfolge
 * @param slices die noch nicht verwendete Menge an Scheiben
 * @return true, wenn es eine lösung gibt, ansonsten false
 */
bool calculate_square(int length, int height, int depth, vector<pair<Slice, Dimension>> &order, vector<Slice> &slices) {

    //Wenn mindestens eine der Seiten auf null ist (daher das Volumen des Quaders null ist)
    if (length == 0 || height == 0 || depth == 0) {
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
            if (calculate_square(new_length, new_height, new_depth, order, slices)) {
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
    string input_dir = "../LennartProtte/Aufgabe2-Implementierung/eigene_Eingabedateien";
    string output_dir = "../LennartProtte/Aufgabe2-Implementierung/eigene_Ausgabedateien";

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
        unordered_set<tuple<int, int, int>, hashFunction> solution;
        bool success = false;
        //Erhöht die Anzahl der Würfel schrittweise, bis es eine Lösung gibt
        for (int count_of_squares = 1; count_of_squares < slices.size(); count_of_squares++) {
            const auto &combinations = findAllCombinations(volume, count_of_squares, slices);
            for (const auto &combination: combinations) {
                bool valid = true;
                vector<Slice> t_slices(slices);
                for (auto dimension: combination) {
                    order.clear();
                    if (!calculate_square(get<0>(dimension), get<1>(dimension), get<2>(dimension), order, t_slices)) {
                        valid = false;
                    }
                }
                if (valid && t_slices.empty()) {
                    success = true;
                    solution = combination;
                    goto end;
                }
            }
        }
        end:
        //Schreibt die Ausgabe
        if (success) {
            fout << "Quader:" << endl;
            for (auto dimension: solution) {
                fout << get<0>(dimension) << "x" << get<1>(dimension) << "x" << get<2>(dimension) << endl;
            }
        }

        //Dateien schließen
        fin.close();
        fout.close();
    }
    return 0;
}