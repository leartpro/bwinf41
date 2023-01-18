#include <iostream>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <cmath>
#include <fstream>

using namespace std;

struct Slice {
    Slice(int a, int b) {
        this->length = a;
        this->height = b;
    }
    int length, height;
};

bool calculate_cube(int length, int height, int depth, vector<pair<Slice, int>> &order, vector<Slice> &slices);


int main() {
    string input_dir = "../LennartProtte/Aufgabe2-Implementierung/Eingabedateien";
    string output_dir = "../LennartProtte/Aufgabe2-Implementierung/Ausgabedateien";

    //string input_dir = "./TestInput";
    //string output_dir = "./TestOutput";

    // Iterator erstellen, der alle Dateien im Eingabeordner durchläuft
    for (const auto &entry: filesystem::directory_iterator(input_dir)) {

        // Dateiname und -pfad aus dem Iterator auslesen
        string input_file = entry.path();
        string output_file = output_dir + "/" + entry.path().filename().string();

        // Eingabedatei öffnen
        ifstream fin(input_file);

        // Ausgabedatei öffnen
        ofstream fout(output_file);

        //Ausgabedatei einlesen
        vector<Slice> slices;
        int a, b, n;
        fin >> n;
        while (fin >> a >> b) {
            slices.emplace_back(a, b);
        }

        // Calculate the volume of the cheese cube
        int volume = 0;
        for (const auto &slice: slices) {
            volume += slice.length * slice.height;
        }

        // Calculate the dimensions of the cheese cube
        int length = 0, height = 0, depth = 0, buffer = 0;
        for(const auto &slice: slices) {
            if (slice.length > length || slice.height > length || slice.length > height ||slice.height > height) {
                length = slice.length;
                height = slice.height;
            }
        }
        depth = volume / (length * height);

        // Calculate the Buffer of the cheese cube
        if(length * height * depth != volume) {
            if (length * height * depth > volume) {
                cout << "Die Scheiben können nicht zu einem Quader zusammengesetzt werden." << endl;
                fout << "Die Scheiben können nicht zu einem Quader zusammengesetzt werden." << endl;
                fin.close();
                fout.close();
                continue;
            } else {
                while (length * height * (depth + buffer) < volume) {
                    buffer++;
                }
                if(length * height * depth != volume) {
                    cout << "Die Scheiben können nicht zu einem Quader zusammengesetzt werden." << endl;
                    fout << "Die Scheiben können nicht zu einem Quader zusammengesetzt werden." << endl;
                    fin.close();
                    fout.close();
                    continue;
                }
            }
        }

        cout << "Quader " << length << "x" << height << "x" << depth << " + buffer=" << buffer << endl << endl;
        fout << "Quader " << length << "x" << height << "x" << depth << " + buffer=" << buffer << endl << endl;
        //get result
        vector<pair<Slice, int>> order;
        sort(slices.begin(), slices.end(), [](Slice a, Slice b) {
                 return (a.length * a.height) > (b.length * b.height);
             }
        );

        if (calculate_cube(length, height, depth, order, slices)) {
            cout << "Die Scheiben können zu einem Quader zusammengesetzt werden:" << endl;
            fout << "Die Scheiben können zu einem Quader zusammengesetzt werden:" << endl;
            for (auto const &o: order) {
                cout << "Slice: (" << o.first.length << ", " << o.first.height << ") Dimension: " << o.second << endl;
                fout << "Slice: (" << o.first.length << ", " << o.first.height << ") Dimension: " << o.second << endl;
            }
        } else {
            cout << "Die Scheiben können nicht zu einem Quader zusammengesetzt werden." << endl;
            fout << "Die Scheiben können nicht zu einem Quader zusammengesetzt werden." << endl;
        }

        // Dateien schließen
        fin.close();
        fout.close();
    }
    return 0;
}

int canRemoveSlice(int length, int height, int depth, Slice slice) {
    if (slice.length == length && slice.height == height || slice.height == length && slice.length == height) {
        return 0; //FRONT
    } else if (slice.length == length && slice.height == depth || slice.height == length && slice.length == depth) {
        return 1; //TOP
    } else if (slice.length == height && slice.height == depth || slice.height == height && slice.length == depth) {
        return 2; //SIDE
    } else {
        return -1; //DOES NOT FIT
    }
}

bool calculate_cube(int length, int height, int depth, vector<pair<Slice, int>> &order, vector<Slice> &slices) {
    if (slices.empty()) {
        return (length == 0 || height == 0 || depth == 0);
    }
    vector<Slice> removed_slices;
    for (auto it = slices.begin(); it != slices.end(); ++it) {
        int dimension = canRemoveSlice(length, height, depth, *it);
        if (dimension >= 0) {
            int new_length = length - (dimension == 2 ? 1 : 0);
            int new_height = height - (dimension == 1 ? 1 : 0);
            int new_depth = depth - (dimension == 0 ? 1 : 0);
            order.emplace_back(*it, dimension);
            removed_slices.push_back(*it);
            slices.erase(it);
            if (calculate_cube(new_length, new_height, new_depth, order, slices)) {
                return true;
            } else {
                order.pop_back();
                for (auto& slice : removed_slices) {
                    slices.push_back(slice);
                }
                removed_slices.clear();
            }
        }
    }
    return false;
}


