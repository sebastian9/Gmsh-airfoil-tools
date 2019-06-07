# Gmsh-airfoil-reader
A C++ code to format an airfoil .dat file into a .geo file for gmsh

Airfoil coordinate dat file parser for Gmsh.
usage: ReadAirfoil.exe airfoil.dat

The first line is reserved for the name, and points should not be repeated. The curve Generated is a closed B-spline.
