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
   

   /* Boundary Layer - Unit Normal calculation at each point for offset curve */
   double x_coord_d, y_coord_d;
   double dx; double dy; double ds;
   double dx_u; double dy_u; double ds_u;
   double dx_d; double dy_d; double ds_d;
   vector<double> delta_x; vector<double> delta_y;
   delta_x.push_back(0); // Assign later, assume continous slope in the ends.
   delta_y.push_back(0);
   for(int i = 1; i <= x.size()-1; i++) {
      // Upward Difference
      dx_u = x[i]-x[i+1];
      dy_u = y[i]-y[i+1];
      ds_u = pow(pow(dx_u,2)+pow(dy_u,2),0.5);
      // Downward Difference
      dx_d = x[i-1]-x[i];
      dy_d = y[i-1]-y[i];
      ds_d = pow(pow(dx_d,2)+pow(dy_d,2),0.5);
      delta_x.push_back(-0.5*(dy_u/ds_u + dy_d/ds_d));
      delta_y.push_back(0.5*(dx_u/ds_u + dx_d/ds_d));
      //cout << i << ": " << delta_x[i] << "," << delta_y[i] << " (" << x[i] << "," << y[i] << ")" << endl;
   }
   delta_x[0] = delta_x[1]; // Assume continous slope in the ends.
   delta_y[0] = delta_y[1];
   delta_x.push_back(*(delta_x.end()-1));
   delta_y.push_back(*(delta_y.end()-1));
   delta_x.pop_back();delta_y.pop_back(); // erase (0,0) at the end, something I don't understand about C++ and vectors
   //cout << x.size() << ": " << delta_x[delta_x.size()] << "," << delta_y[delta_y.size()] << " (" << *x.end() << "," << *y.end() << ")" << endl;
   //for(int i = 0; i <= delta_x.size(); i++) 
      //cout << i << ": " << delta_x[i] << "," << delta_y[i] << " (" << x[i] << "," << y[i] << ")" << endl;

   /* Boundary Layer - Offset curve generation */
   int layers = 3;
   float offset = 1e-1;
   for (int j = 1; j <= layers; j++) 
      for (int i = 0; i <= delta_x.size(); i++) 
         airfoil_geo << "Point(" << point_i++ << ") = {" << x[i] + delta_x[i]*offset*j << "," << y[i] + delta_y[i]*offset*j << ",0};" << endl;
   
   /* Airfoil Curve Creation */
   airfoil_geo << "BSpline(" << ++curve_i << ") = {" << sp_start << ":" << le_up << "};" << endl;
   airfoil_geo << "Transfinite Curve {-" << curve_i << "} = 150 Using Progression 1.01;" << endl;
   airfoil_geo << "BSpline(" << ++curve_i << ") = {" << le_up << ":" << sp_50 << "};" << endl;
   airfoil_geo << "Transfinite Curve {-" << curve_i << "} = 50 Using Bump 1.01;" << endl;
   airfoil_geo << "BSpline(" << ++curve_i << ") = {" << sp_50 << ":" << le_do << "};" << endl;
   airfoil_geo << "Transfinite Curve {" << curve_i << "} = 50 Using Bump 1.01;" << endl;
   airfoil_geo << "BSpline(" << ++curve_i << ") = {" << le_do << ":" << sp_end << "};" << endl;
   airfoil_geo << "Transfinite Curve {" << curve_i << "} = 150 Using Progression 1.01;" << endl;
   airfoil_geo << "Line(" << ++curve_i << ") = {" << sp_start << "," << sp_end << "};" << endl; // Close Trailing Edge
   airfoil_geo << "Transfinite Curve {" << curve_i << "} = 10;" << endl;

   /* Boundary Layer Curve Surface Creation */
   int af_points = sp_end - sp_start +1;
   for (int j = 1; j <= layers; j++) {
      sp_start += af_points; le_up += af_points; sp_50 += af_points; le_do += af_points; sp_end += af_points;
      airfoil_geo << "BSpline(" << ++curve_i << ") = {" << sp_start << ":" << le_up << "};" << endl;
      airfoil_geo << "Transfinite Curve {-" << curve_i << "} = 150 Using Progression 1.01;" << endl;
      airfoil_geo << "BSpline(" << ++curve_i << ") = {" << le_up << ":" << sp_50 << "};" << endl;
      airfoil_geo << "Transfinite Curve {-" << curve_i << "} = 50 Using Bump 1.01;" << endl;
      airfoil_geo << "BSpline(" << ++curve_i << ") = {" << sp_50 << ":" << le_do << "};" << endl;
      airfoil_geo << "Transfinite Curve {" << curve_i << "} = 50 Using Bump 1.01;" << endl;
      airfoil_geo << "BSpline(" << ++curve_i << ") = {" << le_do << ":" << sp_end << "};" << endl;
      airfoil_geo << "Transfinite Curve {" << curve_i << "} = 150 Using Progression 1.01;" << endl;
      airfoil_geo << "Line(" << ++curve_i << ") = {" << sp_start - af_points << "," << sp_start << "};" << endl;
      airfoil_geo << "Transfinite Curve {" << curve_i << "} = 1;" << endl;
      airfoil_geo << "Line(" << ++curve_i << ") = {" << le_up - af_points << "," << le_up << "};" << endl;
      airfoil_geo << "Transfinite Curve {" << curve_i << "} = 1;" << endl;
      airfoil_geo << "Line(" << ++curve_i << ") = {" << sp_50 - af_points << "," << sp_50 << "};" << endl;
      airfoil_geo << "Transfinite Curve {" << curve_i << "} = 1;" << endl;
      airfoil_geo << "Line(" << ++curve_i << ") = {" << le_do - af_points << "," << le_do << "};" << endl;
      airfoil_geo << "Transfinite Curve {" << curve_i << "} = 1;" << endl;
      airfoil_geo << "Line(" << ++curve_i << ") = {" << sp_end - af_points << "," << sp_end << "};" << endl;
      airfoil_geo << "Transfinite Curve {" << curve_i << "} = 1;" << endl;
   }

   /* Boundary Layer Surface Creation */
   int curve_loop_i = 0; int surface_i = 0;
   for (int j = 1; j <= layers; j++)
      for (int i = 1; i <= 4; i++) {
         if (j == 1) 
            airfoil_geo << "Curve Loop(" << ++curve_loop_i << ") = {" << i << "," << 10+i+(j-1)*9 << ", -" << i+5 << ", -" << 9+i+(j-1)*9 << "};" << endl;
         else 
            airfoil_geo << "Curve Loop(" << ++curve_loop_i << ") = {" << i+5-9+(j-1)*9 << "," << 10+i+(j-1)*9 << ", -" << i+5+(j-1)*9 << ", -" << 9+i+(j-1)*9 << "};" << endl;
         airfoil_geo << "Plane Surface(" << ++surface_i << ") = {" << curve_loop_i << "};" << endl;
         airfoil_geo << "Transfinite Surface {" << surface_i << "};" << endl;
      }
   airfoil_geo << "Recombine Surface{1:" << surface_i << "};" << endl;

   /* Cotrol Volume 
   airfoil_geo << "Point(" << point_i +1 << ") = {1,5,0};" << endl;
   airfoil_geo << "Point(" << point_i +2 << ") = {6,5,0};" << endl;
   airfoil_geo << "Point(" << point_i +3 << ") = {6,0,0};" << endl;
   airfoil_geo << "Point(" << point_i +4 << ") = {6,-5,0};" << endl;
   airfoil_geo << "Point(" << point_i +5 << ") = {1,-5,0};" << endl;
   airfoil_geo << "Point(" << point_i +6 << ") = {-4,0,0};" << endl;
   airfoil_geo << "Line(" << ++curve_i << ") = {1," << point_i +1 << "};" << endl;
   airfoil_geo << "Line(" << ++curve_i << ") = {" << point_i +1 << "," << point_i +2 << "};" << endl;
   airfoil_geo << "Line(" << ++curve_i << ") = {" << point_i +2 << "," << point_i +3 << "};" << endl;
   airfoil_geo << "Line(" << ++curve_i << ") = {" << point_i +3 << ",1};" << endl;
   airfoil_geo << "Line(" << ++curve_i << ") = {" << point_i +3 << "," << point_i +4 << "};" << endl;
   airfoil_geo << "Line(" << ++curve_i << ") = {" << point_i +4 << "," << point_i +5 << "};" << endl;
   airfoil_geo << "Line(" << ++curve_i << ") = {" << point_i +5 << ",1};" << endl;
   airfoil_geo << "Line(" << ++curve_i << ") = {" << point_i +6 << "," << point_0 << "};" << endl;
   airfoil_geo << "Circle(" << ++curve_i << ") = {" << point_i +1 << ",1," << point_i +6 << "};" << endl;
   airfoil_geo << "Circle(" << ++curve_i << ") = {" << point_i +6 << ",1," << point_i +5 << "};" << endl;
   airfoil_geo << "Transfinite Curve{-1,2} = 100 Using Progression 1.05;" << endl; // Add bias
   airfoil_geo << "Transfinite Curve{3,-5,7,-9,-10,-11,12} = 100 Using Progression 1.025;" << endl;
   airfoil_geo << "Transfinite Curve {4,6,8} = 200;" << endl;
   airfoil_geo << "Curve Loop(1) = {11, 10, -1, 3};" << endl;
   airfoil_geo << "Plane Surface(1) = {1};" << endl;
   airfoil_geo << "Curve Loop(2) = {12, 9, -2, -10};" << endl;
   airfoil_geo << "Plane Surface(2) = {2};" << endl;
   airfoil_geo << "Curve Loop(3) = {3, 4, 5, 6};" << endl;
   airfoil_geo << "Plane Surface(3) = {3};" << endl;
   airfoil_geo << "Curve Loop(4) = {6, -9, -8, -7};" << endl;
   airfoil_geo << "Plane Surface(4) = {4};" << endl;
   airfoil_geo << "Transfinite Surface {4} = {" << point_i +5 << ", 1, " << point_i +3 << ", " << point_i +4 << "};" << endl;
   airfoil_geo << "Transfinite Surface {3} = {1, " << point_i +1 << ", " << point_i +2 << ", " << point_i +3 << "};" << endl;
   airfoil_geo << "Transfinite Surface {2} = {1, " << point_0 << ", " << point_i +6 << ", " << point_i +5 << "};" << endl;
   airfoil_geo << "Transfinite Surface {1} = {" << point_0 << ", " << point_i +6 << ", " << point_i +1 << ", 1};" << endl;
   airfoil_geo << "Recombine Surface{1,2,3,4};" << endl;  */

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
