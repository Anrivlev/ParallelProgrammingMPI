// Ivlev Andrey B19 - 511
// Variant generation:
const int ID = 4; // Student ID
const int G = 511; // Group
const int X = G * 2 + ID; // X = 1026
const int A = X % 4; // A = 2
const int B = 5 + X % 5; // B = 6

#include <iostream>
#include <mpi.h>
#include <cstdio>

int isSuitable(int R, int G, int B)
{
    if (R * G * B < 1000) return 1;
    else return 0;
}

int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);
    MPI_Status status;
    int size = 0;
    unsigned char* data;
    if (world_rank == 0)
    {
        // BMP file reading
        const char* filename = (char*) "images/img01.bmp";

        int i;
        FILE* f = fopen(filename, "rb");
        unsigned char info[54];

        // read the 54-byte header
        fread(info, sizeof(unsigned char), 54, f);

        // extract image height and width from header
        int width = *(int*)&info[18];
        int height = *(int*)&info[22];

        // allocate 3 bytes per pixel
        size = 3 * width * height;
        data = new unsigned char[size];

        // read the rest of the data at once
        fread(data, sizeof(unsigned char), size, f);
        fclose(f);
        for (i = 0; i < size; i += 3)
        {
            // flip the order of every 3 bytes in order to get RGB instead of BGR
            unsigned char tmp = data[i];
            data[i] = data[i + 2];
            data[i + 2] = tmp;
        }
        // data is read

        for (i = 1; i < world_size; i++)
        {
            MPI_Send(&size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&data, size, MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD);
        }
    } else
    {
        MPI_Recv(&size, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        data = new unsigned char[size];
        MPI_Recv(&data, size, MPI_UNSIGNED_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    }

    int start = 3 * world_rank;
    int numberOfPixels = 0;
    for (int j = start; j < size; j += 3 * B)
    {
        numberOfPixels += isSuitable((int)data[j], (int)data[j + 1], (int)data[j + 2]);
    }
    int totalNumberOfPixels = 0;
    if (world_rank == 0)
    {
        std::cout << "Thread results: [";
        std::cout << numberOfPixels << " ";
        totalNumberOfPixels += numberOfPixels;
        for (int i = 1; i < world_size; i++)
        {
            MPI_Recv(&numberOfPixels, 1, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            std::cout << numberOfPixels;
            if (i != B - 1)  std::cout << " ";
            totalNumberOfPixels += numberOfPixels;
        }
        std::cout << "]" << std::endl;
    } else
    {
        MPI_Send(&numberOfPixels, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    if (world_rank == 0)
    {
        std::cout << "Total number of suitable pixels (with MPI): " << totalNumberOfPixels << std::endl;
        std::cout << std::endl;
    }
    // Finalize the MPI environment.
    MPI_Finalize();
}
