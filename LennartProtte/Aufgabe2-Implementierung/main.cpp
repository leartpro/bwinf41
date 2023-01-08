#include <iostream>
#include <vector>
#include <cmath>

struct CheeseSlice {
    int length, width;
};

int main() {
    int n;
    std::cout << "enter the count of slices here" << std::endl;
    std::cin >> n;

    std::vector<CheeseSlice> slices;
    std::cout << "enter your slices here" << std::endl;
    for (int i = 0; i < n; i++) {
        int a, b;
        std::cin >> a >> b;
        std::cout << "slice " << i+1 << "/" << n << ": (" << a << ", " << b << ")" << std::endl;
        slices.push_back({a, b});
    }

    // Calculate the volume of the cheese cube
    int volume = 0;
    for (const auto &slice: slices) {
        volume += slice.length * slice.width;
    }

// Calculate the dimensions of the cheese cube
    int length = std::cbrt(volume);
    int width = length;
    int height = volume / (length * width);

// Check if the dimensions of the cheese cube are valid
    if (length * width * height != volume) {
        std::cout << "The cheese slices cannot be assembled into a complete cheese cube." << std::endl;
        return 0;
    }


    // Check if the cheese slices can be assembled into a complete cheese cube
    std::vector<std::vector<int>> layers(height, std::vector<int>(width, 0));
    std::vector<int> used_slices;
    for (int i = 0; i < n; i++) {
        // Check if the current cheese slice fits in the current layer
        bool fits = false;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (layers[y][x] == 0 && slices[i].length <= width - x && slices[i].width <= height - y) {
                    // Place the cheese slice in the current layer
                    for (int dy = 0; dy < slices[i].width; dy++) {
                        for (int dx = 0; dx < slices[i].length; dx++) {
                            layers[y + dy][x + dx] = 1;
                        }
                    }
                    used_slices.push_back(i);
                    fits = true;
                    break;
                }
            }
            if (fits) {
                break;
            }
        }
        if (!fits) {
            // If the current cheese slice doesn't fit, try the next one
            continue;
        }
        // Check if all cheese slices have been used
        if (used_slices.size() == slices.size()) {
            std::cout << "The cheese slices can be assembled into a complete cheese cube in the following order:"
                      << std::endl;
            for (const auto &index: used_slices) {
                std::cout << "(" << slices[index].length << ", " << slices[index].width << ")" << std::endl;
            }
            return 0;
        }
        // If not all cheese slices have been used, start a new layer
        layers = std::vector<std::vector<int>>(height, std::vector<int>(width, 0));
    }
    std::cout << "The cheese slices cannot be assembled into a complete cheese cube." << std::endl;
    return 0;
}
