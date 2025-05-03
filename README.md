# VTKBinaryWriter
This repository contains routines to write vtk binary files for structured meshes

## Clone the git repository
```
git clone https://github.com/nataraj2/VTKBinaryWriter.git
cd VTKBinaryWriter
```
## Compilation and running
```
cd serial_writer
gfortran -fconvert=big-endian -o out writevtk_serial.f90
./out
```

## Results
The images show the mesh and the scalar field in VisIt and ParaView

![VisIt](Images/binary_vtk_VisIt.png)
![ParaView](Images/binary_vtk_ParaView.png)



