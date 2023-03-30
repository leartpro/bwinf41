//
// Created by lenni on 26.03.23.
//

#ifndef BWINF_MAIN_H
#define BWINF_MAIN_H

#include <vector>

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
            return "vorne / hinten";
        case Dimension::TOP:
            return "oben / unten";
        case Dimension::SIDE:
            return "links / rechts";
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
Dimension can_remove_slice(int length, int height, int depth, Slice slice) {
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
 * Ermittelt rekursiv durch Backtracking eine Lösungsreihenfolge für die gegebene Menge an Scheiben.
 * @param length die Länge des Quaders
 * @param height die Höhe des Quaders
 * @param depth die Tiefe des Quaders
 * @param order die Lösungsreihenfolge
 * @param slices die noch nicht verwendete Menge an Scheiben
 * @return true, wenn es eine lösung gibt, ansonsten false
 */
bool calculate_square(int length, int height, int depth, std::vector<std::pair<Slice, Dimension>> &order, std::vector<Slice> &slices) {

    //Wenn mindestens eine der Seiten auf null ist (daher das Volumen des Quaders null ist)
    if (length == 0 || height == 0 || depth == 0) {
        return true;
    }
    std::vector<Slice> removed_slices;
    //Für jede noch nicht verwendete Schiebe
    for (auto it = slices.begin(); it != slices.end(); ++it) {
        Dimension dimension = can_remove_slice(length, height, depth, *it);
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

#endif //BWINF_MAIN_H