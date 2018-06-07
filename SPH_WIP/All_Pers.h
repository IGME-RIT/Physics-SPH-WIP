#ifndef _All_PERS_H
#define _All_PERS_H


#define BoundarySizeX 1.0f
#define BoundarySizeY 1.0f
#define BoundarySizeZ 0.5f
#define Number_of_particels 150
#define	Grid_Size 10
#define K 1.0f											// Gas stiffness
#define DENSITY 998.29f
#define MASS (1000.0f/Number_of_particels)
#define VISCOSITY 0.001003f
#define SIGMA  0.07280f									// Surface tension
#define DAMPENING_CONSTANT -0.3f
#define COLOR_FIELD_THRESHOLD 7.065f 
#define POINTSIZE 20.0f
#define RADIUS (POINTSIZE/600.0f)
#define H  RADIUS * 4.0f									// Kernel Radius

std::vector<GameObject *> grid[Grid_Size][Grid_Size][Grid_Size];
std::vector<GameObject *> neighbors[Number_of_particels];
std::vector<GameObject *> particles[Number_of_particels];


#endif _All_INCLUDES_H
