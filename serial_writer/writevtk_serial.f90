program write_vtk_binary_structured
  implicit none
  integer, parameter :: nx = 20, ny = 20, nz = 20
  integer :: i, j, k, idx
  integer :: npts
  real(4), allocatable :: coords(:)
  real(4), allocatable :: scalar1(:), scalar2(:)
  character(len=100) :: line
  character(len=1) :: nl

  nl = char(10)
  npts = nx * ny * nz

  allocate(coords(3 * npts))     ! x, y, z for each point
  allocate(scalar1(npts))
  allocate(scalar2(npts))

  ! Fill coordinates and scalar values
  idx = 0
  do k = 0, nz-1
     do j = 0, ny-1
        do i = 0, nx-1
           idx = idx + 1
           coords(3*(idx-1)+1) = real(i)
           coords(3*(idx-1)+2) = real(j)
           coords(3*(idx-1)+3) = real(k)
           scalar1(idx) = real(i)  ! Example scalar field
           scalar2(idx) = real(j)  ! Example scalar field
        end do
     end do
  end do

  ! Open file in binary stream mode
  open(unit=10, file="grid.vtk", form="unformatted", access="stream", status="replace", convert="big_endian")

  ! Write VTK legacy header
  write(10) "# vtk DataFile Version 3.0"//nl
  write(10) "Explicit mesh example"//nl
  write(10) "BINARY"//nl
  write(10) "DATASET STRUCTURED_GRID"//nl

  write(line, '(A,I0,A,I0,A,I0)') "DIMENSIONS ", nx, " ", ny, " ", nz
  write(10) trim(line)//nl

  write(line, '(A,I0)') "POINTS ", npts
  write(10) trim(line)//" float"//nl

  ! Write coordinates
  write(10) coords

  ! Scalar field
  write(line, '(A,I0)') "POINT_DATA ", npts
  write(10) trim(line)//nl
  write(10) "SCALARS scalar1 float 1"//nl
  write(10) "LOOKUP_TABLE default"//nl
  write(10) scalar1
  
  ! Second scalar
  write(10) "SCALARS scalar2 float 1"//nl
  write(10) "LOOKUP_TABLE default"//nl
  write(10) scalar2

  close(10)
end program write_vtk_binary_structured
