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

# License

Copyright 2019 Sebastián López Sánchez

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
