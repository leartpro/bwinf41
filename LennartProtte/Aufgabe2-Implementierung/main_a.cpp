#include <iostream>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include "main.h"

using namespace std;

/**
 * Liest die Eingabedateien ein und versucht für jede Datei eine Lösung entsprechend der Aufgabenstellung zu finden
 * Die Lösung wird anschließend in die entsprechende Ausgabedatei geschrieben
 * Sollte es keine Lösung geben, wird dies ebenfalls in die Ausgabedatei geschrieben
 * @return 0, wenn es zu keinem RuntimeError oder keiner RuntimeException gekommen ist
 */
int main() {
    string input_dir = "../LennartProtte/Aufgabe2-Implementierung/Eingabedateien";
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
        vector<pair<int, int>> result;
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
            if (calculate_square( it->first, t_height, it->second, order, t_slices)) {
                success = true;
                fout << "Die Scheiben können zu einem Quader zusammengesetzt werden." << endl;
                fout << "Quader: " << height << "x" << it->first << "x" << it->second << " V(" << volume << ")" << endl
                     << endl;
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
            fout << "Die Scheiben können zu keinem Quader zusammengesetzt werden." << endl;
        }
        //Dateien schließen
        fin.close();
        fout.close();
    }
    return 0;
}