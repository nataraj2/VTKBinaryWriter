#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <cstring>
#include <cassert>

// Swap to big endian if needed
inline void SwapEndian(float &val) {
    unsigned char *bytes = reinterpret_cast<unsigned char*>(&val);
    std::swap(bytes[0], bytes[3]);
    std::swap(bytes[1], bytes[2]);
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    // Global grid size
    int nx = 128, ny = 128, nz = 128;

    assert(nz % nprocs == 0);
    int z_per_rank = nz / nprocs;
    int z_start = rank * z_per_rank;
    int local_nz = z_per_rank;
    int local_npts = nx * ny * local_nz;
    int total_npts = nx * ny * nz;

    // Generate local coordinates (float32)
    std::vector<float> coords(3 * local_npts);
	std::vector<float> scalar1(local_npts), scalar2(local_npts);  // temperature

    int idx = 0;
    for (int k = z_start; k < z_start + local_nz; ++k) {
        for (int j = 0; j < ny; ++j) {
            for (int i = 0; i < nx; ++i) {
                float x = static_cast<float>(i);
                float y = static_cast<float>(j);
                float z = static_cast<float>(k);
                SwapEndian(x);
                SwapEndian(y);
                SwapEndian(z);
                coords[3 * idx + 0] = x;
                coords[3 * idx + 1] = y;
                coords[3 * idx + 2] = z;

			    float temp = static_cast<float>(k); // simple scalar field
                SwapEndian(temp);
                scalar1[idx] = temp;
                scalar2[idx] = temp;

                idx++;
            }
        }
    }

    std::string filename = "grid.vtk";
    MPI_Offset header_size = 0;
    MPI_Offset scalar_offset = 0;


    if (rank == 0) {
        std::ofstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "Failed to open file on rank 0.\n";
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Write header
        std::ostringstream oss;
        oss << "# vtk DataFile Version 3.0\n";
        oss << "Structured grid example\n";
        oss << "BINARY\n";
        oss << "DATASET STRUCTURED_GRID\n";
        oss << "DIMENSIONS " << nx << " " << ny << " " << nz << "\n";
        oss << "POINTS " << total_npts << " float\n";

        std::string header = oss.str();
        file.write(header.c_str(), header.size());
        header_size = header.size();
   }

    // Broadcast header size to all ranks
    MPI_Bcast(&header_size, 1, MPI_OFFSET, 0, MPI_COMM_WORLD);
	MPI_Bcast(&scalar_offset, 1, MPI_OFFSET, 0, MPI_COMM_WORLD);

    MPI_File fh;
    MPI_File_open(MPI_COMM_WORLD, filename.c_str(),
                  MPI_MODE_WRONLY | MPI_MODE_APPEND,
                  MPI_INFO_NULL, &fh);

    // Write coordinates
    MPI_Offset offset_coords = header_size + static_cast<MPI_Offset>(3 * sizeof(float)) * nx * ny * z_start;
    MPI_File_write_at_all(fh, offset_coords, coords.data(), coords.size(), MPI_FLOAT, MPI_STATUS_IGNORE);

    MPI_File_close(&fh);

int scalar_header_size = 0;
if (rank == 0) {
    std::ostringstream scalar_ss;
    scalar_ss << "\nPOINT_DATA " << total_npts << "\n";
    scalar_ss << "SCALARS temperature float 1\n";
    scalar_ss << "LOOKUP_TABLE default\n";
    std::string scalar_hdr = scalar_ss.str();
    scalar_header_size = scalar_hdr.size();

    std::ofstream sfile(filename, std::ios::binary | std::ios::app);
    sfile.write(scalar_hdr.c_str(), scalar_header_size);
    sfile.close();
}

// Share scalar header size with all ranks
MPI_Bcast(&scalar_header_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

MPI_File_open(MPI_COMM_WORLD, filename.c_str(),
              MPI_MODE_WRONLY, MPI_INFO_NULL, &fh);

MPI_Offset scalar_data_offset = header_size
                               + static_cast<MPI_Offset>(3 * sizeof(float)) * total_npts
                               + scalar_header_size;

MPI_Offset my_scalar_offset = scalar_data_offset + static_cast<MPI_Offset>(sizeof(float)) * nx * ny * z_start;

MPI_File_write_at_all(fh, my_scalar_offset,
                      scalar1.data(), scalar1.size(), MPI_FLOAT, MPI_STATUS_IGNORE);

MPI_File_close(&fh);


    MPI_Finalize();
    return 0;
}

