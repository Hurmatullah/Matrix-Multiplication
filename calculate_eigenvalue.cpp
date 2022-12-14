
#include "calculate_eigenvalue.h"
#include <random>
#include <mpi.h>


float** matrix(int size) {

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0, 1);

    float** a = new float* [size];
    for (int i = 0; i < size; i++) {
        a[i] = new float[size];
    }
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (i == j) {
                a[i][j] = dist(gen);
            }
            else {
                a[i][j] = 0;
            }
        }
    }

    return a;

}

float* copy_vector(const float* v, int n) {
    float* new_v = new float[n];
    for (int i = 0; i < n; i++) {
        new_v[i] = v[i];
    }
    return new_v;
}

float* vector(int size) {

    float* v = new float[size];
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, 10000000);

    for (int i = 0; i < size; i++) {
        v[i] = dist(gen);
    }

    return v;

}
