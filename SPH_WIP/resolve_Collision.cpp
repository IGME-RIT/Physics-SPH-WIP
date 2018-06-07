
#include "All_Includes.h"

void resolve_Collision::boundVelocities(float DAMPENING_CONSTANT, int Grid_Size, float BoundarySizeX, float BoundarySizeY, float BoundarySizeZ)
{
	std::vector<Particle *>::iterator it;

	// This function goes through all the edge grids.
	// For all the particles there, it checks if they leave the bounding area. 
	// If they are outside the bounding area, then check if they continue to 
	// move outwards of the bounding volume, then change the component of velocity
	// which is along the surface normal.

	for (int i = 0; i < Grid_Size; i++)
	{
		for (int j = 0; j < Grid_Size; j++)
		{
			//X-axis
			for (it = grid[0][i][j].begin(); it != grid[0][i][j].end(); it++)
			{
				if ((*it)->position.x < 0 && ((*it)->velocity.x < 0 || (*it)->acceleration.x < 0))
				{
					(*it)->velocity.x *= DAMPENING_CONSTANT;
					(*it)->position.x = 0.0f;
				}
			}
			for (it = grid[Grid_Size - 1][i][j].begin(); it != grid[Grid_Size - 1][i][j].end(); it++)
			{
				if ((*it)->position.x > BoundarySizeX && ((*it)->velocity.x > 0 || (*it)->acceleration.x > 0))
				{
					(*it)->velocity.x *= DAMPENING_CONSTANT;
					(*it)->position.x = BoundarySizeX;
				}
			}
			//Y-Axis
			for (it = grid[i][0][j].begin(); it != grid[i][0][j].end(); it++)
			{
				if ((*it)->position.y < 0 && ((*it)->velocity.y < 0 || (*it)->acceleration.y < 0))
				{
					(*it)->velocity.y *= -0.1f;// DAMPENING_CONSTANT;
					(*it)->position.y = 0.0f;
				}
			}

			for (it = grid[i][Grid_Size - 1][j].begin(); it != grid[i][Grid_Size - 1][j].end(); it++)
			{
				if ((*it)->position.y > BoundarySizeY && ((*it)->velocity.y > 0 || (*it)->acceleration.y > 0))
				{
					(*it)->velocity.y *= DAMPENING_CONSTANT;
					(*it)->position.y = BoundarySizeY;
				}
			}

			//Z-axis
			for (it = grid[i][j][0].begin(); it != grid[i][j][0].end(); it++)
			{
				if ((*it)->position.z < 0 && ((*it)->velocity.z < 0 || (*it)->acceleration.z < 0))
				{
					(*it)->velocity.z *= DAMPENING_CONSTANT;
					(*it)->position.z = 0.0f;
				}
			}
			for (it = grid[i][j][Grid_Size - 1].begin(); it != grid[i][j][Grid_Size - 1].end(); it++)
			{
				if ((*it)->position.z > BoundarySizeZ && ((*it)->velocity.z > 0 || (*it)->acceleration.z > 0))
				{
					(*it)->velocity.z *= DAMPENING_CONSTANT;
					(*it)->position.z = BoundarySizeZ;
				}
			}

		}
	}


	//NOT needed

	//for (int i = 0; i < Number_of_particels; i++)
	//{
	//	if (particles[i].position.y < 0 && particles[i].velocity.y < 0)
	//	{
	//		particles[i].velocity.y *= -0.7f;
	//		particles[i].position.y = 0.0f;
	//		//particles[i].acceleration.y += CONTAINER_SPRING_CONSTANT;
	//		//particles[i].acceleration.y += particles[i].velocity.y * CONTAINER_DAMPENING;
	//	}

	//	if (particles[i].position.x > BoundarySize && particles[i].velocity.x > 0)
	//	{
	//		particles[i].velocity.x *= -0.7f;
	//		particles[i].position.x = BoundarySize;
	//		//particles[i].acceleration.x -= CONTAINER_SPRING_CONSTANT;
	//		//particles[i].acceleration.x -= particles[i].velocity.x * CONTAINER_DAMPENING;
	//	}

	//	if (particles[i].position.x < 0 && particles[i].velocity.x < 0)
	//	{
	//		particles[i].velocity.x *= -0.7f;
	//		particles[i].position.x = 0.0f;
	//		//particles[i].acceleration.x += CONTAINER_SPRING_CONSTANT;
	//		//particles[i].acceleration.x += particles[i].velocity.x * CONTAINER_DAMPENING;
	//	}
	//}
}

void resolve_Collision::updateVelocities(int Number_of_particels, float H, float COLOR_FIELD_THRESHOLD, float SIGMA)
{
	/*
	This function updates the acceleration of each particle.
	*/

	std::vector<Particle *>::iterator it;
	float k;													// Smoothed Color
	glm::vec3 Fpressure, Fviscosity, n, Fexternal, Fsurface, Finternal, Ftotal;
	float l;
	for (int i = 0; i < Number_of_particels; i++)
	{
		//Reset the values in forces
		Fpressure = glm::vec3(0.0f);
		Fviscosity = glm::vec3(0.0f);
		Fexternal = glm::vec3(0.0f);
		Finternal = glm::vec3(0.0f);
		Fsurface = glm::vec3(0.0f);
		k = 0;
		n = Fpressure;

		for (it = neighbors[i].begin(); it != neighbors[i].end(); it++)
		{
			l = glm::length(particles[i].position - (*it)->position);

			if (l <= H && l > 0.0f)
			{
				Fpressure += pressureForcePerParticle(particles[i], *(*it));
				Fviscosity += viscosityForcePerParticle(particles[i], *(*it));
				// n is the direction of surface tension force.
				// For particles which are not in the outside surface teh n will sum upto 0.
				// For all the particles in the surface, the value will be non zero.
				n += ((*it)->mass * smoothKernelPoly6Gradient(particles[i].position - (*it)->position)) / (*it)->density;
				k += ((*it)->mass * smoothKernelPoly6Laplacian(particles[i].position - (*it)->position)) / (*it)->density;
			}
		}

		Fpressure *= -1.0f;
		Fviscosity *= particles[i].viscosity;

		float CFNLength = glm::length(n);
		//Calculate the surface tension force
		if (CFNLength > COLOR_FIELD_THRESHOLD && CFNLength != 0.0f)
		{
			Fsurface = (-SIGMA) * k * (n / CFNLength);
		}

		Finternal = Fviscosity + Fpressure;

		Fexternal = (G * particles[i].density) + Fsurface;

		Ftotal = Finternal + Fexternal;

		particles[i].acceleration = Ftotal / particles[i].density;
	}

	boundVelocities();
}

void resolve_Collision::integrate(float dt, int Number_of_particels)
{
	for (int i = 0; i < Number_of_particels; i++)
	{
		particles[i].position = EulerIntegrator(particles[i].position, dt, particles[i].velocity, particles[i].acceleration);
	}
}
