#include <iostream>
#include <memory>
#include <vector>
#include <cmath>
#include <fstream>
#include <filesystem>


using namespace std;

struct Slice {
    Slice(int a, int b) {
        this->length = a;
        this->width = b;
    }

    int length, width;
};

void calculate_cube(int cube[3], vector<shared_ptr<Slice>> *unsorted, vector<shared_ptr<Slice>> *sorted);

int main() {
    string input_dir = "../LennartProtte/Aufgabe2-Implementierung/Eingabedateien";
    string output_dir = "../LennartProtte/Aufgabe2-Implementierung/Ausgabedateien";

    // Iterator erstellen, der alle Dateien im Eingabeordner durchläuft
    for (const auto &entry: filesystem::directory_iterator(input_dir)) {

        // Dateiname und -pfad aus dem Iterator auslesen
        string input_file = entry.path();
        string output_file = output_dir + "/" + entry.path().filename().string();

        // Eingabedatei öffnen
        ifstream fin(input_file);

        // Ausgabedatei öffnen
        ofstream fout(output_file);
        vector<shared_ptr<Slice>> slices;
        int a, b, n;
        fin >> n;
        while (fin >> a >> b) {
            slices.push_back(make_shared<Slice>(a, b));
        }

        // Calculate the volume of the cheese cube
        int volume = 0;
        for (const auto &slice: slices) {
            volume += slice->length * slice->width;
        }

        // Calculate the dimensions of the cheese cube //TODO: not calculated correctly yet
        int length = static_cast<int>(cbrt(volume)); //cubic root of volume
        int width = length; //TODO: the length dont have to be equal to the width
        int depth = volume / (length * width);

        // Check if the dimensions of the cheese cube are valid
        if (length * width * depth != volume) {
            cout << "The cheese slices cannot be assembled into a complete cheese cube." << endl;
            fout << "The cheese slices cannot be assembled into a complete cheese cube." << endl;
            fin.close();
            fout.close();
            continue; //TODO: not sure if this is correct
        }

        int cube[] = {length, width, depth};
        //get result
        vector<shared_ptr<Slice>> order = *new vector<shared_ptr<Slice>>;
        calculate_cube(cube, &order, &slices);

        if (order.empty()) {
            cout << "The cheese slices cannot be assembled into a complete cheese cube." << endl;
            fout << "The cheese slices cannot be assembled into a complete cheese cube." << endl;
        } else {
            cout << "The cheese slices can be assembled into a complete cheese cube in the following order:"
                 << std::endl;
            fout << "The cheese slices can be assembled into a complete cheese cube in the following order:"
                 << std::endl;
            for (const auto &current: order) {
                cout << "(" << current->length << ", " << current->width << ")" << endl;
                fout << "(" << current->length << ", " << current->width << ")" << endl;
            }
        }

        // Dateien schließen
        fin.close();
        fout.close();
    }
    return 0;
}

void calculate_cube(int cube[3], vector<shared_ptr<Slice>> *sorted, vector<shared_ptr<Slice>> *unsorted) {
    //Vergleiche jedes Stück, mit jeder Seite des Quaders

    /*
     * 1. isValid?
     * 2. führe für jedes Stück rekursion aus, wenn Lösung gibt, gebe Lösung weiter nach oben,
     *  wenn keine Lösung, fahre mit nächstem element fort
     */
    bool valid = false;
    for (auto &slice: *unsorted) {
        if (slice->length == cube[0] && slice->width == cube[1]) { //Wenn Seite => {laenge, breite}, dann ...
            cube[2] -= 1; //Tiefe minus eins
            sorted->push_back(slice);
            unsorted->erase(find(unsorted->begin(), unsorted->end(), slice));
            valid = true;
        } else if (slice->length == cube[0] && slice->width == cube[2]) { //Wenn Seite => {laenge, tiefe}, dann ...
            cube[1] -= 1; //Breite minus eins
            sorted->push_back(slice);
            unsorted->erase(find(unsorted->begin(), unsorted->end(), slice));
            valid = true;
        } else if (slice->length == cube[1] && slice->width == cube[2]) { //Wenn Seite => {breite, tiefe}, dann ...
            cube[0] -= 1; //Länge minus eins
            sorted->push_back(slice);
            unsorted->erase(find(unsorted->begin(), unsorted->end(), slice));
            valid = true;

            //Piece wird um 90° gedreht:
        } else if (slice->width == cube[0] && slice->length == cube[1]) { //Wenn Seite => {laenge, breite}, dann ...
            cube[2] -= 1; //Tiefe minus eins
            sorted->push_back(slice);
            unsorted->erase(find(unsorted->begin(), unsorted->end(), slice));
            valid = true;
        } else if (slice->width == cube[0] && slice->length == cube[2]) { //Wenn Seite => {laenge, tiefe}, dann ...
            cube[1] -= 1; //Breite minus eins
            sorted->push_back(slice);
            unsorted->erase(find(unsorted->begin(), unsorted->end(), slice));
            valid = true;
        } else if (slice->width == cube[1] && slice->length == cube[2]) { //Wenn Seite => {breite, tiefe}, dann ...
            cube[0] -= 1; //Länge minus eins
            sorted->push_back(slice);
            unsorted->erase(find(unsorted->begin(), unsorted->end(), slice));
            valid = true;
        }
    }
    if (!valid) {
        sorted->clear();
        return;
    }
}
