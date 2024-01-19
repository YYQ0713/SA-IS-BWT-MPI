#include <iostream>
#include <fstream>
#include "SAIS.h"
#include <mpi.h>
#include <string>
int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_File file, file_mpi;
    MPI_File_open(MPI_COMM_WORLD, "test_100M.txt", MPI_MODE_CREATE | MPI_MODE_RDWR, MPI_INFO_NULL, &file);
    MPI_File_open(MPI_COMM_WORLD, "100M.mpiout", MPI_MODE_CREATE | MPI_MODE_RDWR, MPI_INFO_NULL, &file_mpi);
    MPI_Offset file_size;
    MPI_File_get_size(file, &file_size);
    
    MPI_Offset chunk_size = file_size / size;
    char *input;
    input = (char*)malloc(chunk_size * sizeof(char));

    
    MPI_Offset offset = rank * chunk_size;
    //MPI_File_set_size(file, file_size);
    //MPI_File_set_size(file_mpi, file_size);
    MPI_File_set_view(file, offset, MPI_CHAR, MPI_CHAR, "native", MPI_INFO_NULL);
    MPI_File_set_view(file_mpi, offset, MPI_CHAR, MPI_CHAR, "native", MPI_INFO_NULL);
    
    MPI_File_read_at_all(file, 0, input, chunk_size, MPI_CHAR, MPI_STATUS_IGNORE);

    //std::cout << input << std::endl;

    SAIS data;
    data.addChar(input, chunk_size);
    free(input);
    data.makeSuffixArray();
    //std::cout << "test" << std::endl;
    //data.printSuffixArray();
    //data.printBWT();
    std::vector<char> buffer(data.dataBWT());
    char resbuf[chunk_size];
    
    
    for (size_t i = 0; i < chunk_size; i++) {
        resbuf[i] = buffer[i];
    }


    MPI_File_write_at_all(file_mpi, 0, resbuf, chunk_size, MPI_CHAR, MPI_STATUS_IGNORE);
    
    MPI_File_close(&file);
    MPI_File_close(&file_mpi);
    MPI_Finalize();

    return 0;
}
