#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

struct Slice {
    Slice(int a, int b) {
        this->length = a;
        this->height = b;
    }

    int length, height;
};

bool calculate_cube(int length, int height, int depth, vector<pair<Slice, int>> &order, vector<Slice> &slices, int counter);

int main() {
    int length = 6, height = 6, depth = 6;
    vector<pair<Slice, int>> order;

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
    /*
    sort(slices.begin(), slices.end(), [](Slice a, Slice b) {
             return (a.length * a.height) > (b.length * b.height);
         }
    );
    */
    if (calculate_cube(length, height, depth, order, slices, 1)) {
        cout << "Die Käsescheiben können zu einem Quader zusammengesetzt werden:" << endl;
        for (auto const &o: order) {
            cout << "Slice: (" << o.first.length << ", " << o.first.height << ") Dimension: " << o.second << endl;
        }
    } else {
        cout << "Die Käsescheiben können nicht zu einem Quader zusammengesetzt werden." << endl;
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
bool calculate_cube(int length, int height, int depth, vector<pair<Slice, int>> &order, vector<Slice> &slices, int counter) {
    cout << "recursion: " << counter << endl;
    cout << "cube: " << length << "x" << height << "x" << depth << "x" << endl;
    cout << "ORDER: " << endl;
    for (auto const &o: order) {
        cout << "slice: (" << o.first.length << ", " << o.first.height << ") dimension: " << o.second << endl;
    }
    cout << "UNORDER: " << endl;
    for (auto const &o: slices) {
        cout << "slice: (" << o.length << ", " << o.height << ")" << endl;
    }
    if (slices.empty()) {
        cout << "CUBE COMPLETED" << endl;
        cout << "RETURN recursion " << counter << endl;
        return (length == 0 || height == 0 || depth == 0);
    }
    for (auto it = slices.begin(); it != slices.end(); ++it) {
        int dimension = canRemoveSlice(length, height, depth, *it);
        if (dimension >= 0) {
            int new_length = length - (dimension == 2 ? 1 : 0);
            int new_height = height - (dimension == 1 ? 1 : 0);
            int new_depth = depth - (dimension == 0 ? 1 : 0);
            order.emplace_back(*it, dimension);
            slices.erase(it);
            cout << endl;
            if (calculate_cube(new_length, new_height, new_depth, order, slices, counter+1)) {
                cout << "CUBE COMPLETED" << endl;
                cout << "RETURN recursion " << counter << endl;
                return true;
            } else {
                order.pop_back();
                slices.push_back(*it);
            }
        }
    }
    cout << "ITERATION FAILED => RETURN FALSE" << endl;
    cout << "RETURN recursion " << counter << endl;
    return false;
}





