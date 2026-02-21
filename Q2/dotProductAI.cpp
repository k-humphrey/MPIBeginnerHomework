/*Kayla Humphrey
ChatGPT vector vector multiplication*/
#include <mpi.h>
#include <iostream>
#include <vector>
#include <cmath>

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 2)
    {
        if (rank == 0)
            std::cout << "Usage: mpirun -np P ./a.out N\n";
        MPI_Finalize();
        return 0;
    }

    int N = atoi(argv[1]);

    // ---- Compute balanced partition ----
    int base = N / size;
    int remainder = N % size;

    int local_n = base + (rank < remainder ? 1 : 0);

    int start_index;
    if (rank < remainder)
        start_index = rank * (base + 1);
    else
        start_index = rank * base + remainder;

    // ---- Allocate and initialize local vectors ----
    std::vector<double> A(local_n, 2.0);
    std::vector<double> B(local_n, 2.0);

    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();

    // ---- Local element-wise multiply + partial dot ----
    double local_sum = 0.0;
    for (int i = 0; i < local_n; i++)
        local_sum += A[i] * B[i];

    // ---- Manual Tree Reduction ----
    double global_sum = local_sum;
    int step = 1;

    while (step < size)
    {
        if (rank % (2 * step) == 0)
        {
            if (rank + step < size)
            {
                double received;
                MPI_Recv(&received, 1, MPI_DOUBLE,
                         rank + step, 0,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                global_sum += received;
            }
        }
        else
        {
            int dest = rank - step;
            MPI_Send(&global_sum, 1, MPI_DOUBLE,
                     dest, 0, MPI_COMM_WORLD);
            break;
        }
        step *= 2;
    }

    double end_time = MPI_Wtime();

    if (rank == 0)
    {
        std::cout << "N = " << N
                  << "  P = " << size
                  << "  Dot Product = " << global_sum
                  << "  Time = " << end_time - start_time
                  << " seconds\n";
    }

    MPI_Finalize();
    return 0;
}