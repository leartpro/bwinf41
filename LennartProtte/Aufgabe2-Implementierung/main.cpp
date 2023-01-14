#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <filesystem>


using namespace std;

struct Slice {
    Slice(int a, int b) {
        this->length = a;
        this->height = b;
    }

    Slice(Slice &slice) {
        this->length = slice.length;
        this->height = slice.height;
    }

    int length, height;
};

enum Dimension {
    FRONT = 0,
    TOP = 1,
    RIGHT = 2
};

bool calculate_cube(int cube[3], vector<pair<Slice*, Dimension>>* sorted, vector<Slice*> unsorted);

int main() {
    string input_dir = "../LennartProtte/Aufgabe2-Implementierung/TestInput";
    string output_dir = "../LennartProtte/Aufgabe2-Implementierung/TestOutput";

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
        vector<Slice*> slices;
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
        vector<pair<Slice*, Dimension>> order;
        bool success = calculate_cube(new int[3] {length, height, depth}, &order, slices);

        if (!success) {
            cout << "The cheese slices cannot be assembled into a complete cheese cube." << endl;
            fout << "The cheese slices cannot be assembled into a complete cheese cube." << endl;
        } else {
            cout << "The cheese slices can be assembled into a complete cheese cube in the following order:"
                 << std::endl;
            fout << "The cheese slices can be assembled into a complete cheese cube in the following order:"
                 << std::endl;
            for (const auto &current: order) {
                cout << "(" << current.first->length << ", " << current.first->height << ") " << ((current.second == 0) ? "RIGHT" : ((current.second == 1) ? "TOP" : "FRONT")) << endl;
                fout << "(" << current.first->length << ", " << current.first->height << ") " << ((current.second == 0) ? "RIGHT" : ((current.second == 1) ? "TOP" : "FRONT")) << endl;
            }
        }

        // Dateien schließen
        fin.close();
        fout.close();
    }
    return 0;
}
/*
 * Gegeben: int[3] cube, vector<pair<Slice*, Dimension>> sorted, vector<Slice> unsorted
 * Return: bool
 * init bool isValid mit false
 * Für jedes Element aus unsorted:
 *  kann an einer Seite (nicht) gedreht weggenommen werden?
 *  ja:
 *      verkleinere cube
 *      push_back current in sorted mit dimension
 *      remove von unsorted
 *      isValid init mit true
 *      if rekursion gleich true:
 *          return true
 *      else continue mit nächstem element
 *  wenn unsorted durchlaufen und isValid gleich false:
 *      return false
 *  wenn unsorted leer und cube{x,y,z} = 0
 *      return true
 */
bool calculate_cube(int cube[3], vector<pair<Slice*, Dimension>>* sorted, vector<Slice*> unsorted) {
    if (unsorted.empty()) {
        if(cube[0] == 0 || cube[1] == 0 || cube[2] == 0) {
            return true;
        } else {
            return false;
        }
    }
    bool valid = false;
    for (auto &slice: unsorted) {
        for (int i = 0; i < 3; i++) {
            for (int j = i + 1; j < 3; j++) {
                cout << "check slice {" << slice->length << "," << slice->height << "} on {" << cube[i] << "," << cube[j] << "}" <<endl;
                if ((slice->length == cube[i] && slice->height == cube[j]) ||
                    (slice->height == cube[i] && slice->length == cube[j])) {
                    cout << "piece fit on: " << ((3 - i - j == 0) ? "RIGHT" : ((3 - i - j == 1) ? "TOP" : "FRONT")) << " " <<  (3 - i - j) << endl;
                    cube[3 - i - j] -= 1; //decrease dimension
                    cout << "cube dimensions: length=" << cube[0] << ", height=" << cube[1] << ", depth=" << cube[2] << endl;
                    cout << endl;
                    sorted->emplace_back(slice, Dimension(3 - i - j));
                    unsorted.erase(unsorted.begin() + int(&slice - &unsorted[0]));
                    valid = true;
                    if (calculate_cube(cube, sorted, unsorted)) {
                        return true;
                    }
                }
            }
        }
    }
    if (!valid) {
        return false;
    }
    return false; //TODO: idk if this has to be true or false or if i can remove this
}


