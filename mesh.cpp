/*
	Airfoil coordinate dat file parser for Gmsh.
	usage: ReadAirfoil.exe airfoil.dat
	The first line is reserved for the name, and points
	should not be repeated. The curve Generated is
	a closed B-spline. Points should be in a standard
	.dat list form describing a continous closed curve
   beginning and ending in 1,0
*/

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;

int main(int argc, char *argv[])
{
   /* Read .dat airfoil file */
   vector<double> x,y;
   string x_coord, y_coord;
   bool space1_flag, space2_flag, x_flag, y_flag, is_coord;
   ifstream airfoil_dat(argv[1]);
   ofstream airfoil_geo;

   int curve_i = 0;
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
            x.push_back(stod(x_coord));
            y.push_back(stod(y_coord));
      } else {
         airfoil_geo.open(line + ".geo");
         airfoil_geo << "Geometry.LineNumbers = 1; Geometry.SurfaceNumbers = 1;" << endl;
         //airfoil_geo << "Geometry.PointNumbers = 1;" << endl;
      }
      if (point_i == 0)
         point_i++;
   }
   x.pop_back();y.pop_back(); // erase (0,0) at the end

   /* Airfoil Points Creation in Gmsh File */
   for (int i =  0; i <= x.size(); i++)
      airfoil_geo << "Point(" << point_i++ << ") = {" << x[i] << "," << y[i] << ",0};" << endl;
      //cout << x[i]  << "," << y[i] << endl;
   // Points corresponding to the different sections
   int sp_start = 1;
   int sp_end = point_i-1;
   int le_up = floor(0.5*0.75*(sp_end-sp_start)+sp_start);
   int sp_50 = floor(0.5*(sp_end-sp_start)+sp_start);
   int le_do = floor(0.625*(sp_end-sp_start)+sp_start);

   /* Airfoil Curve Creation */
   int curve_loop_i = 0;
   airfoil_geo << "BSpline(" << ++curve_i << ") = {" << sp_start << ":" << le_up << "};" << endl;
   airfoil_geo << "Transfinite Curve {-" << curve_i << "} = 150 Using Progression 1.01;" << endl;
   airfoil_geo << "BSpline(" << ++curve_i << ") = {" << le_up << ":" << sp_50 << "};" << endl;
   airfoil_geo << "Transfinite Curve {-" << curve_i << "} = 50 Using Bump 1.01;" << endl;
   airfoil_geo << "BSpline(" << ++curve_i << ") = {" << sp_50 << ":" << le_do << "};" << endl;
   airfoil_geo << "Transfinite Curve {" << curve_i << "} = 50 Using Bump 1.01;" << endl;
   airfoil_geo << "BSpline(" << ++curve_i << ") = {" << le_do << ":" << sp_end << "};" << endl;
   airfoil_geo << "Transfinite Curve {" << curve_i << "} = 150 Using Progression 1.01;" << endl;
   airfoil_geo << "Line(" << ++curve_i << ") = {" << sp_end << "," << sp_start << "};" << endl; // Close Trailing Edge
   airfoil_geo << "Curve Loop(" << ++curve_loop_i << ") = {" << curve_i-4 << "," << curve_i-3 << "," << curve_i-2 << "," << curve_i-1 << "," << curve_i << "};" << endl;

   airfoil_geo << "Extrude { Curves {" << curve_i-4 << "," << curve_i-3 << "," << curve_i-2 << "," << curve_i-1 << "," << curve_i << "}; Layers{ {1,4,2}, {0.5, 0.6, 1.6} } }" << endl;

   /* Control Volume */
   airfoil_geo << "Point(" << point_i +1 << ") = {1,100,0};" << endl;
   airfoil_geo << "Point(" << point_i +2 << ") = {1,-100,0};" << endl;
   airfoil_geo << "Circle(" << ++curve_i << ") = {" << point_i +1 << ",1," << point_i +2 << "};" << endl;
   airfoil_geo << "Circle(" << ++curve_i << ") = {" << point_i +2 << ",1," << point_i +1 << "};" << endl;
   airfoil_geo << "Curve Loop(" << ++curve_loop_i << ") = {" << curve_i-1 << "," << curve_i << "};" << endl;

   /* Surface Creation */
   int surface_i = 0;
   airfoil_geo << "Plane Surface(" << ++surface_i << ") = {" << curve_loop_i << "," << curve_loop_i-1 << "};" << endl;


   // airfoil_geo << "Field[1] = BoundaryLayer;" << endl;
   // airfoil_geo << "Field[1].EdgesList = {1,2,3,4};" << endl;
   // airfoil_geo << "Field[1].thickness = 0.1;" << endl;
   // airfoil_geo << "Field[1].hwall_n = 1e-6;" << endl;
   // airfoil_geo << "Field[1].ratio = 1.1;" << endl;
   // airfoil_geo << "Field[1].hfar = 1;" << endl;
   // airfoil_geo << "Background Field = 2;" << endl;

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
