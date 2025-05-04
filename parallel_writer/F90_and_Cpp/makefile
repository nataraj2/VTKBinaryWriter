# Extended Makefile for Fortran + C++ MPI VTK writer with parallel build and dependency tracking

# Compiler wrappers
CXX = mpic++
FC  = mpif90

# Source and object files
CXX_SRC = vtk_writer.cpp
FC_SRC  = main.f90

CXX_OBJ = $(CXX_SRC:.cpp=.o)
FC_OBJ  = $(FC_SRC:.f90=.o)
DEPS    = $(CXX_SRC:.cpp=.d)

# Executable name
EXEC = vtk_driver

# Flags
CXXFLAGS = -O2 -MMD -MP
FFLAGS   = -O2
LDFLAGS  = -lstdc++

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(FC_OBJ) $(CXX_OBJ)
	$(FC) $^ $(LDFLAGS) -o $@

# Compile C++ with dependency generation
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile Fortran normally
%.o: %.f90
	$(FC) $(FFLAGS) -c $< -o $@

# Include generated dependencies
-include $(DEPS)

clean:
	rm -f *.o *.mod *.d $(EXEC)

