#ifndef _PARTICLE_H
#define _PARTICLE_H

#include "All_Includes.h"

class Particle
{
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	float mass;
	float density;
	float viscosity;

public:
	void setParticles(int Grid_Size, int Number_of_particels, float BoundarySizeX, float BoundarySizeY, float BoundarySizeZ);

	void clear_tree(int Grid_Size);

	void getNeighbors(int Number_of_particels, float BoundarySizeX, float BoundarySizeY, float BoundarySizeZ, int Grid_Size);

	void catergorizeParticles(int Number_of_particels, float BoundarySizeX, float BoundarySizeY, float BoundarySizeZ, int Grid_Size);

};
#endif _PARTICLE_H