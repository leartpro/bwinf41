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
enum Dimension{
    FRONT,
    TOP,
    SIDE,
    INVALID
};

/*
 * Dimension als String
 */
constexpr const char* to_string(Dimension dimension) {
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
    //Wenn alle Scheiben in der Lösungsmenge enthalten sind
    if (slices.empty()) {
        //Wenn mindestens eine der Seiten auf null ist (daher das Volumen des Quaders null ist)
        return (length == 0 || height == 0 || depth == 0);
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
    string input_dir = "../LennartProtte/Aufgabe2-Implementierung/debug";
    string output_dir = "../LennartProtte/Aufgabe2-Implementierung/Ausgabedateien";

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

        //Findet die Seite, wo der größte Wert maximal ist und setzt die Länge auf diesen Wert
        for (const auto &slice: slices) {
            int side = (slice.length > slice.height) ? slice.length : slice.height;
            if (side > height) {
                height = side;
            }
        }

        //Findet alle anderen möglichen Seiten zu der gesetzten Länge
        vector<pair<int, int>> result; //TODO: e.g Quader 210x210x90V(9261000) is very suspicious
        int base = volume / height;
        for(int side_a = 1; side_a <= base; side_a++) {
            for(int side_b = 1; side_b <= base; side_b++) {
                if(side_a * side_b * height == volume) {
                    result.emplace_back(side_a, side_b);
                }
            }
        }

        //Sortiert die Scheiben nach ihrer Fläche
        vector<pair<Slice, Dimension>> order;
        sort(slices.begin(), slices.end(), [](Slice a, Slice b) {
                 return (a.length * a.height) > (b.length * b.height);
             }
        );

        auto it = result.begin();
        bool success = false;
        //Für jede mögliche Kombination der Seitenlängen
        while (it != result.end()) {
            int t_height = height;
            vector<Slice> t_slices(slices);
            order.clear();
            //Wenn es eine Lösung gibt
            if (calculate_cube( it->first, t_height, it->second, order, t_slices)) {
                success = true;
                fout << "Quader " << height << "x" << it->first << "x" << it->second << " V(" << volume << ")" << endl
                     << endl;
                fout << "Die Scheiben können zu einem Quader zusammengesetzt werden:" << endl;
                for (auto const &o: order) {
                    fout << "Slice: (" << o.first.length << ", " << o.first.height << ") Dimension: " << to_string(o.second)
                         << endl;
                }
                break;
            }
            it++;
        }
        //Wenn es keine Lösung gab
        if (!success) {
            fout << "Die Scheiben können nicht zu einem Quader zusammengesetzt werden." << endl;
        }
        //Dateien schließen
        fin.close();
        fout.close();
    }
    return 0;
}