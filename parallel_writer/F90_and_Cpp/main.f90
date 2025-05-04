program vtk_writer_driver
    use mpi
    use iso_c_binding
    implicit none

    interface
        subroutine write_vtk_mpi(nx, ny, nz, rank, nprocs, scalar_data) bind(C)
            use iso_c_binding
            integer(c_int), value :: nx, ny, nz
            integer(c_int), value :: rank, nprocs
            real(c_float), dimension(*) :: scalar_data
        end subroutine write_vtk_mpi
    end interface

    integer :: ierr, rank, nprocs
    integer(c_int) :: nx, ny, nz
    integer :: local_nz, local_npts
    real(c_float), allocatable :: scalar_data(:)

    call MPI_Init(ierr)
    call MPI_Comm_rank(MPI_COMM_WORLD, rank, ierr)
    call MPI_Comm_size(MPI_COMM_WORLD, nprocs, ierr)

    ! Grid size
    nx = 128
    ny = 128
    nz = 128

    if (mod(nz, nprocs) /= 0) then
        if (rank == 0) print *, 'nz must be divisible by number of processes.'
        call MPI_Abort(MPI_COMM_WORLD, 1, ierr)
    end if

    local_nz = nz / nprocs
    local_npts = nx * ny * local_nz

    allocate(scalar_data(local_npts))

    ! Fill scalar field (e.g., with z-index)
    call fill_scalar(nx, ny, nz, local_nz, rank, scalar_data)

    ! Call the C++ function
    call write_vtk_mpi(nx, ny, nz, rank, nprocs, scalar_data)

    deallocate(scalar_data)
    call MPI_Finalize(ierr)
end program vtk_writer_driver


subroutine fill_scalar(nx, ny, nz, local_nz, rank, scalar_data)
    use iso_c_binding
    implicit none
    integer(c_int), intent(in) :: nx, ny, nz, local_nz, rank
    real(c_float), dimension(*), intent(out) :: scalar_data
    integer :: i, j, k, idx, z_start
    real(c_float) :: value

    z_start = rank * local_nz
    idx = 1
    do k = z_start, z_start + local_nz - 1
        do j = 0, ny - 1
            do i = 0, nx - 1
                value = real(k, c_float)
                scalar_data(idx) = value
                idx = idx + 1
            end do
        end do
    end do
end subroutine fill_scalar

