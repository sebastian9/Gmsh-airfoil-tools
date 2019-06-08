/*
	Tool based on th airfoil coordinate dat file parser for Gmsh.
	usage: ReadAirfoil.exe airfoil.dat params
	The first line in the .dat file is reserved for the name, 
  and points should not be repeated. The curve Generated is
	a closed B-spline. Points should be in a standard
	.dat list form describing a continous closed curve
  The output is a airfoil.geo file which contains a structured
  mesh according to the parameters.
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
	   		is_coord = (str != " " && str != "\n");
	   		
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
	   		airfoil_geo << "Point(" << point_i << ") = {" << x_coord << "," << y_coord << ",0};" << endl;
   	} else {
   		airfoil_geo.open(line + ".geo");
   		airfoil_geo << "Geometry.LineNumbers = 1; Geometry.SurfaceNumbers = 1;" << endl;
   	}
   	if (y_flag || point_i == 0)
   		point_i++;
   }
   point_i--;

   airfoil_geo << "BSpline(1) = {1:" << 0.5*point_i << "};" << endl;
   airfoil_geo << "BSpline(2) = {" << 0.5*point_i << ":" << point_i -1 << ",1};" << endl;
   airfoil_geo << "Point(" << point_i +1 << ") = {1,5,0};" << endl;
   airfoil_geo << "Point(" << point_i +2 << ") = {6,5,0};" << endl;
   airfoil_geo << "Point(" << point_i +3 << ") = {6,0,0};" << endl;
   airfoil_geo << "Point(" << point_i +4 << ") = {6,-5,0};" << endl;
   airfoil_geo << "Point(" << point_i +5 << ") = {1,-5,0};" << endl;
   airfoil_geo << "Point(" << point_i +6 << ") = {-4,0,0};" << endl;
   airfoil_geo << "Line(3) = {1," << point_i +1 << "};" << endl;
   airfoil_geo << "Line(4) = {" << point_i +1 << "," << point_i +2 << "};" << endl;
   airfoil_geo << "Line(5) = {" << point_i +2 << "," << point_i +3 << "};" << endl;
   airfoil_geo << "Line(6) = {" << point_i +3 << ",1};" << endl;
   airfoil_geo << "Line(7) = {" << point_i +3 << "," << point_i +4 << "};" << endl;
   airfoil_geo << "Line(8) = {" << point_i +4 << "," << point_i +5 << "};" << endl;
   airfoil_geo << "Line(9) = {" << point_i +5 << ",1};" << endl;
   airfoil_geo << "Line(10) = {" << point_i +6 << "," << 0.5*point_i << "};" << endl;
   airfoil_geo << "Circle(11) = {" << point_i +1 << ",1," << point_i +6 << "};" << endl;
   airfoil_geo << "Circle(12) = {" << point_i +6 << ",1," << point_i +5 << "};" << endl;
   airfoil_geo << "Transfinite Curve{1,2,11,12} = 100;" << endl; // Add bias
   airfoil_geo << "Transfinite Curve {3:10} = 100;" << endl;
   airfoil_geo << "Curve Loop(1) = {11, 10, -1, 3};" << endl;
   airfoil_geo << "Plane Surface(1) = {1};" << endl;
   airfoil_geo << "Curve Loop(2) = {12, 9, -2, -10};" << endl;
   airfoil_geo << "Plane Surface(2) = {2};" << endl;
   airfoil_geo << "Curve Loop(3) = {3, 4, 5, 6};" << endl;
   airfoil_geo << "Plane Surface(3) = {3};" << endl;
   airfoil_geo << "Curve Loop(4) = {6, -9, -8, -7};" << endl;
   airfoil_geo << "Plane Surface(4) = {4};" << endl;
   airfoil_geo << "Transfinite Surface {4} = {77, 1, 75, 76};" << endl;
   airfoil_geo << "Transfinite Surface {3} = {1, 73, 74, 75};" << endl;
   airfoil_geo << "Transfinite Surface {2} = {1, 36, 78, 77};" << endl;
   airfoil_geo << "Transfinite Surface {1} = {36, 78, 73, 1};" << endl;
   airfoil_geo << "Recombine Surface{1,2,3,4}" << endl;  

   airfoil_dat.close();
   airfoil_geo.close();
   return 0;
}
   /* 
   
   BOUNDARY LAYER FOR UNSTRUCTURED MESHES 

   airfoil_geo << "Field[1] = Distance;" << endl;
   airfoil_geo << "Field[1].NNodesByEdge = 100;" << endl;
   airfoil_geo << "Field[1].EdgesList = {1,2};" << endl;
   airfoil_geo << "Field[2] = BoundaryLayer;" << endl;
   airfoil_geo << "Field[2].EdgesList = {1,2};" << endl;
   airfoil_geo << "Field[2].thickness = 0.1;" << endl;
   airfoil_geo << "Field[2].hwall_n = 1e-6;" << endl;
   airfoil_geo << "Field[2].ratio = 1.1;" << endl;
   airfoil_geo << "Field[2].hfar = 1;" << endl;
   airfoil_geo << "Background Field = 2;" << endl;

   */
