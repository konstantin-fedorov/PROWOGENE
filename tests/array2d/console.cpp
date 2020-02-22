#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <map>
#include <string>

#include "array2d.h"

using std::cout;
using std::endl;
using std::string;
using prowogene::utils::Array2D;

bool Array2DConstructor() {
    Array2D<int> t;
    if (t.Width() || t.Height() || t.Size()) {
        return false;
    }
    return true;
}

bool Array2DResize() {
    Array2D<int> t;
    if (t.Width() || t.Height() || t.Size()) {
        return false;
    }
    int new_w = 5;
    int new_h = 7;
    int default_val = 1000;
    t.Resize(new_w, new_h, default_val);
    if (t(new_w - 1, new_h - 1) != default_val ||
            t(new_h - 1, new_w - 1) != int()) {
        return false;
    }
    return true;
}

bool Array2DIterator() {
    Array2D<int> t;
    int val = 10;
    int side = 5;
    t.Resize(side, side, 10);
    int elements_processed = 0;
    for (auto& elem : t) {
        ++elem;
        ++elements_processed;
    }
    if (elements_processed != side * side) {
        return false;
    }
    int val_to_compare = val + 1;
    for (auto& elem : t) {
        if (elem != val_to_compare) {
            return false;
        }
    }
    return true;
}

bool Array2DIndexes() {
    Array2D<int> t;
    int val = 10;
    int side = 5;
    t.Resize(side, side, 10);
    for (int i = 0; i < side; ++i) {
        for (int j = 0; j < side; ++j) {
            t(i, j) = i * side + j;
        }
    }
    for (int i = 0; i < side; ++i) {
        for (int j = 0; j < side; ++j) {
            if (t(i, j) / side != i ||
                    t(i, j) % side != j) {
                return false;
            }
        }
    }
    return true;
}

typedef bool (*TestFuncPtr)();
const std::map<string, TestFuncPtr> kTests = {
    {"array2d-constructor", Array2DConstructor},
    {"array2d-resize", Array2DResize},
    {"array2d-iterator", Array2DIterator},
    {"array2d-indexes", Array2DIndexes}
};

int main(int argc, const char **argv) {
    if (argc != 2) {
        std::cout << "No command line arguements" << std::endl;
        return -1;
    }

    string test_name = argv[1];
    auto test_func = kTests.find(test_name);
    if (test_func == kTests.end()) {
        return -1;
    } else {
        if (test_func->second()) {
            std::cout << "Passed test \"" << test_name << "\"" << std::endl;
            return 0;
        } else {
            std::cout << "Not passed test \"" << test_name << "\"" << std::endl;
            return -1;
        }
    }
}
