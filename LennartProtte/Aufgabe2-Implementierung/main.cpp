#include <iostream>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <fstream>

using namespace std;

struct Slice {
    Slice(int a, int b) {
        this->length = a;
        this->height = b;
    }
    int length, height;
};

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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
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
                for (auto &slice: removed_slices) {
                    slices.push_back(slice);
                }
                removed_slices.clear();
            }
        }
    }
    return false;
}
#pragma clang diagnostic pop

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

        //Ausgabedatei einlesen
        vector<Slice> slices;
        int a, b, n;
        fin >> n;
        while (fin >> a >> b) {
            slices.emplace_back(a, b);
        }

        // Calculate the volume of the cheese cube
        int volume = 0, length = 0;
        for (const auto &slice: slices) {
            volume += slice.length * slice.height;
        }

        // Find the largest side
        for (const auto &slice: slices) {
            int side = (slice.length > slice.height) ? slice.length : slice.height;
            if (side > length) {
                length = side;
            }
        }
        //Find all possible other sides
        vector<pair<int, int>> result; //TODO: e.g Quader 210x210x90V(9261000) is very suspicious
        for (int i = 1; i <= volume / length; i++) {
            if (volume % (length * i) == 0) {
                result.emplace_back(length, i);
            }
        }

        vector<pair<Slice, int>> order;
        sort(slices.begin(), slices.end(), [](Slice a, Slice b) {
                 return (a.length * a.height) > (b.length * b.height);
             }
        );
        auto it = result.begin();
        bool success = false;
        while (it != result.end() && !success) {
            //cout << "Quader " << length << "x" << it->first << "x" << it->second << "V(" << volume << ")" << endl << endl;
            int t_length = length;
            vector<Slice> t_slices(slices);
            order.clear();
            if (calculate_cube(t_length, it->first, it->second, order, t_slices)) {
                success = true;
                fout << "Quader " << length << "x" << it->first << "x" << it->second << " V(" << volume << ")" << endl << endl;
                fout << "Die Scheiben können zu einem Quader zusammengesetzt werden:" << endl;
                for (auto const &o: order) {
                    fout << "Slice: (" << o.first.length << ", " << o.first.height << ") Dimension: " << o.second << endl;
                }
            }
            it++;
        }
        if(!success) {
                fout << "Die Scheiben können nicht zu einem Quader zusammengesetzt werden." << endl;
        }
        // Dateien schließen
        fin.close();
        fout.close();
    }
    return 0;
}