/*
	Airfoil coordinate dat file parser for Gmsh.
	usage: ReadAirfoil.exe airfoil.dat
	The first line is reserved for the name, and points 
	should not be repeated. The curve Generated is
	a closed B-spline.
*/

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

int main(int argc, char *argv[])
{
   string x_coord, y_coord;
   bool space1_flag, space2_flag, x_flag, y_flag, is_coord;
   ifstream airfoil_dat(argv[1]);
   ofstream airfoil_geo;

   bool first_line = 0;
   int point_i = 0;
   // While the string continues
   for (string line; getline(airfoil_dat, line); ) {
   	x_coord = y_coord = "";
   	space1_flag = space2_flag = x_flag = y_flag = 0;
   	if (point_i) { // Skip first line
	   	// For every position in the current line
	   	for (string::iterator it = line.begin(); it != line.end(); ++it) {
	   		string str(1,*it);
	   		is_coord = str != " ";
	   		
	   		// Determine position in the line
	        if ( !(space2_flag) && !(x_flag) && !(y_flag) && is_coord )
	           x_flag = 1;
	        if ( !(space2_flag) && x_flag && !(y_flag) && !(is_coord) )
	           space2_flag = 1;
	        if ( space2_flag && x_flag && !(y_flag) && is_coord )
	           y_flag = 1;

	         // Register coordinates
	        if ( !(space2_flag) && x_flag && !(y_flag) && is_coord )
	           x_coord = x_coord + *it;
	        if ( space2_flag && x_flag && y_flag && is_coord )
	           y_coord = y_coord + *it;
	   	}
	   	if (x_flag && y_flag)
	   		airfoil_geo << "Point(" << point_i << ") = {" << x_coord << "," << y_coord << ",0, 1};" << endl;
   	} else {
   		airfoil_geo.open(line + ".geo");
   		airfoil_geo << "SetFactory(\"OpenCASCADE\");" << endl;
   	}
   	if (y_flag || point_i == 0)
   		point_i++;
   }

   airfoil_geo << "//+ \nBSpline(1) = {1:" << point_i -2 << "};" << endl;
   airfoil_dat.close();
   airfoil_geo.close();
   return 0;
}