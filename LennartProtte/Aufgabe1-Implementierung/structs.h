//
// Created by Lenni Protte on 06.01.23.
//

#ifndef BWINF41_STRUCTS_H
#define BWINF41_STRUCTS_H

#pragma once

#include <vector>
#include <set>

struct Edge; // Vorwärtsdeklaration von Node

struct Node {
    double x{};
    double y{};
    std::set<Edge*> edges{};
};

struct Edge {
    double distance{};
    Node* node1{};
    Node* node2{};
    std::vector<std::pair<Edge*, double>> edges{};
};

#endif //BWINF41_STRUCTS_H
