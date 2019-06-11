/*
	Airfoil coordinate dat file parser for Gmsh.
	usage: ReadAirfoil.exe airfoil.dat
	The first line is reserved for the name, and points 
	should not be repeated. The curve generated is
	a closed B-spline. Points should be in a standard
	Selig format .dat file
*/

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

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

   airfoil_geo << "//+ \nBSpline(1) = {1:" << point_i -1 << "};" << endl;
   airfoil_dat.close();
   airfoil_geo.close();
   return 0;
}
