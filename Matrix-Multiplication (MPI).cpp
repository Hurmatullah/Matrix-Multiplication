#include <iostream>
#include <fstream>
#include <mpi.h>
#include <cstring>
#include "calculate_eigenvalue.h"
#include <cassert>


// Creating simple iterative method based on our formula
float* simpleMode(float** a, float* b, int n, int iterations = 20) {

    float** H = new float* [n];
    for (int i = 0; i < n; i++) {
        H[i] = new float[n];
    }
    float* g = new float[n];

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) {
                H[i][j] = 0;
            }
            else {
                H[i][j] = -1 * a[i][j] / a[i][i];
            }
        }
        g[i] = b[i] / a[i][i];
    }
    float* x = new float[n];

    for (int k = 0; k < iterations; k++) {
        float* new_x = new float[n];
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                new_x[i] += H[i][j] * x[j];
            }
            new_x[i] += g[i];
        }
        x = new_x;
    }
    return x;
}

// Creating parallel mode to get speed to our process using OpenMP
float* parallelMode(float** a, float* b, int n, int iterations = 20) {

    int numberOfProcessors, processorRank;
    int numberOfWorker;
    int rows;
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcessors);
    MPI_Comm_rank(MPI_COMM_WORLD, &processorRank);

    numberOfWorker = numberOfProcessors - 1;
    rows = n / numberOfWorker;


    float** H = new float* [n];
    for (int i = 0; i < n; i++) {
        H[i] = new float[n];
    }
    float* g = new float[n];

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) {
                H[i][j] = 0;
            }
            else {
                H[i][j] = -1 * a[i][j] / a[i][i];
            }
        }
        g[i] = b[i] / a[i][i];
    }
    float* x = new float[n];

    // Sharing some variables in our OpenMP method and initialize the number of thread
    for (int k = 0; k < iterations; k++) {
        float* new_x = new float[n];

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < rows; j++) {
                new_x[i] += H[i][j] * x[j];
            }
            x[i] = new_x[i] + g[i];
        }

        float global_sum;
        MPI_Reduce(&x, &global_sum, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
    }

    return x;
}

int main(int argc, char* argv[]) {

    // Intializing our parameters that have to be stored in our file
    int rank = atoi(argv[1]);
    int iterations = atoi(argv[2]);
    char* filename = argv[3];
    double start_time, end_time;
    float* result;
    // Creating our file dynamically
    std::ofstream parameter_store(filename, std::ios::app);

    // Calling our metrix-generator and vector-generator from another file that we have already created
    auto a = matrix(rank);
    auto b = vector(rank);

    MPI_Init(&argc, &argv);
    int n_threads, thread_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &n_threads);
    MPI_Comm_size(MPI_COMM_WORLD, &thread_size);


    MPI_Barrier(MPI_COMM_WORLD);
    if (n_threads == 0)
    {
        start_time = MPI_Wtime();
        result = simpleMode(a, b, rank, iterations);
        end_time = MPI_Wtime();
    }
    else
    {
        start_time = MPI_Wtime();
        result = parallelMode(a, b, rank, iterations);
        end_time = MPI_Wtime();
    }


    parameter_store << "(Threads=" << n_threads << "): ";
    parameter_store << "Rank=" << rank << " ";
    parameter_store << "Number of iteratons=" << iterations << " ";
    parameter_store << "Time=" << end_time - start_time << std::endl;
    parameter_store.close();

    MPI_Finalize();
}
