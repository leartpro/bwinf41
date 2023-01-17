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
    int length = 6, height = 6, depth = 6;
    vector<pair<Slice, Dimension>> order;
    /*
    vector<Slice> slices = {
            {2, 4},
            {4, 6},
            {6, 6},
            {3, 4},
            {4, 6},
            {3, 6},
            {2, 4},
            {2, 4},
            {4, 6},
            {3, 3},
            {3, 3},
            {6, 6}
    };
            */
    //vector<Slice> slices = { {6, 4}, {4, 6}, {4, 4}, {4, 2}, {2, 4} };
    vector<Slice> slices = {
            {2, 3},
            {1, 2},
            {2, 2}
    };
    if (calculate_cube(length, height, depth, order, slices)) {
        cout << "Die Käsescheiben können zu einem Quader zusammengesetzt werden:" << endl;
        for (auto const &o: order) {
            cout << "Slice: (" << o.first.length << ", " << o.first.height << ") Dimension: " << o.second << endl;
        }
    } else {
        cout << "Die Käsescheiben können nicht zu einem Quader zusammengesetzt werden." << endl;
    }

    return 0;
}

/*
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
        bool success = calculate_cube(length, height, depth}, order, slices);

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
}*/

bool canRemoveSlice(int length, int height, int depth, Slice slice, Dimension &dimension) {
    if (slice.length == length && slice.height == height) {
        dimension = Dimension::FRONT;
    } else if (slice.height == length && slice.length == height) {
        dimension = Dimension::FRONT;
    } else if (slice.length == length && slice.height == depth) {
        dimension = Dimension::TOP;
    } else if (slice.height == length && slice.length == depth) {
        dimension = Dimension::TOP;
    } else if (slice.length == height && slice.height == depth) {
        dimension = Dimension::RIGHT;
    } else if (slice.height == height && slice.length == depth) {
        dimension = Dimension::RIGHT;
    } else {
        return false;
    }
    return true;
}

bool calculate_cube(int length, int height, int depth, vector<pair<Slice, Dimension>> &order, vector<Slice> &slices) {
    if (slices.empty()) {
        return (length == 0 || height == 0 || depth == 0);
    }
    for (auto it = slices.begin(); it != slices.end(); ++it) {
        Dimension dimension;
        if (canRemoveSlice(length, height, depth, *it, dimension)) {
            int new_length = length - (dimension == Dimension::FRONT ? it->length : 0);
            int new_height = height - (dimension == Dimension::TOP ? it->height : 0);
            int new_depth = depth - (dimension == Dimension::RIGHT ? it->length : 0);
            order.emplace_back(*it, dimension);
            slices.erase(it);
            if (calculate_cube(new_length, new_height, new_depth, order, slices)) {
                return true;
            } else {
                order.pop_back();
                slices.push_back(*it);
            }
        }
    }
    return false;
}





