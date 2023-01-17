#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <filesystem>
#include <algorithm>


using namespace std;

struct Slice {
    Slice(int a, int b) {
        this->length = a;
        this->height = b;
    }

    int length, height;
};

enum Dimension {
    FRONT = 0,
    TOP = 1,
    RIGHT = 2
};

bool calculate_cube(int length, int height, int depth, vector<pair<Slice, Dimension>> &order, vector<Slice> &slices);

int main() {
    string input_dir = "../LennartProtte/Aufgabe2-Implementierung/TestInput";
    string output_dir = "../LennartProtte/Aufgabe2-Implementierung/TestOutput";

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
        vector<Slice *> slices;
        int a, b, n;
        fin >> n;
        while (fin >> a >> b) {
            slices.push_back(new Slice(a, b));
        }

        // Calculate the volume of the cheese cube
        int volume = 0;
        for (const auto &slice: slices) {
            volume += slice->length * slice->height;
        }

        // Calculate the dimensions of the cheese cube //TODO: not calculated correctly yet
        int length = static_cast<int>(cbrt(volume)); //cubic root of volume
        int height = length; //TODO: the length dont have to be equal to the height
        int depth = volume / (length * height);

        // Check if the dimensions of the cheese cube are valid
        if (length * height * depth != volume) {
            cout << "The cheese slices cannot be assembled into a complete cheese cube." << endl;
            fout << "The cheese slices cannot be assembled into a complete cheese cube." << endl;
            fin.close();
            fout.close();
            continue; //TODO: not sure if this is correct
        }

        cout << "cube dimensions: length=" << length << ", height=" << height << ", depth=" << depth << endl << endl;
        //get result
        vector<pair<Slice *, Dimension>> order;
        bool success = calculate_cube(new int[3]{length, height, depth}, &order, slices);

        if (!success) {
            cout << "The cheese slices cannot be assembled into a complete cheese cube." << endl;
            fout << "The cheese slices cannot be assembled into a complete cheese cube." << endl;
        } else {
            cout << "The cheese slices can be assembled into a complete cheese cube in the following order:"
                 << std::endl;
            fout << "The cheese slices can be assembled into a complete cheese cube in the following order:"
                 << std::endl;
            for (const auto &current: order) {
                cout << "(" << current.first->length << ", " << current.first->height << ") "
                     << ((current.second == 0) ? "RIGHT" : ((current.second == 1) ? "TOP" : "FRONT")) << endl;
                fout << "(" << current.first->length << ", " << current.first->height << ") "
                     << ((current.second == 0) ? "RIGHT" : ((current.second == 1) ? "TOP" : "FRONT")) << endl;
            }
        }

        // Dateien schließen
        fin.close();
        fout.close();
    }
    return 0;
}

bool calculate_cube(int length, int height, int depth, vector<pair<Slice, Dimension>> &order, vector<Slice> &slices) {
    if (slices.empty()) {
        return (length == 0 || height == 0 || depth == 0);
    }
    for (auto it = slices.begin(); it != slices.end(); ++it) {
        if (it->length == length && it->height == height || it->height == length && it->length == height) {
            depth--;
            order.emplace_back(*it, Dimension(FRONT));
            slices.erase(it);
            if (calculate_cube(length, height, depth, order, slices)) {
                return true;
            } else {
                depth++;
                order.pop_back();
                slices.push_back(*it);
            }
        } else if (it->length == length && it->height == depth || it->height == length && it->length == depth) {
            height--;
            order.emplace_back(*it, Dimension(TOP));
            slices.erase(it);
            if (calculate_cube(length, height, depth, order, slices)) {
                return true;
            } else {
                height++;
                order.pop_back();
                slices.push_back(*it);
            }
        } else if (it->length == height && it->height == depth || it->height == height && it->length == depth) {
            length--;
            order.emplace_back(*it, Dimension(FRONT));
            slices.erase(it);
            if (calculate_cube(length, height, depth, order, slices)) {
                return true;
            } else {
                length++;
                order.pop_back();
                slices.push_back(*it);
            }
        }
    }
    return false;
}


