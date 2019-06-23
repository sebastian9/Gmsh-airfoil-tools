# Gmsh 2D Airfoil Mesher (mesh.cpp)

2D Airfoil mesher for Gmsh.

Usage:

        mesh.exe bl_thickness hwall_n ratio hfar airfoil_list

The first line is reserved for the name, and points
should not be repeated. The curve generated is
a closed B-spline.  Points should be in a standard
Selig format airfoil coordinate file.

Usage example:

        mesh.exe 0.05 0.000001 1.3 1.5 "CH10-(smoothed).dat" e423.dat FX-84-W-150.dat S1223.dat

Refer to http://gmsh.info/doc/texinfo/gmsh.html for documentation on Gmsh

License: MIT License Copyright 2019 Sebastian Lopez Sanchez http://github.com/sebastian9

# Gmsh-airfoil (ReadAirfoil.cpp)

A C++ code to format an airfoil .dat file into a .geo file for gmsh

Airfoil coordinate dat file parser for Gmsh.
usage: ReadAirfoil.exe airfoil.dat

The first line is reserved for the name, and points should not be repeated. The curve Generated is a closed B-spline.


