
#include "All_Includes.h"



void Particle::setParticles(int Grid_Size, int Number_of_particels, float BoundarySizeX, float BoundarySizeY, float BoundarySizeZ)
{
	float divisionX = BoundarySizeX / Grid_Size, divisionY = BoundarySizeY / Grid_Size, divisionZ = BoundarySizeZ / Grid_Size;

	int i, j, k, l;
	for (i = 0; i < Number_of_particels; i++)
	{
		particles[i].position.x = (float)(i % (Grid_Size - 1))*divisionX + (0.1f);	//* (i%2)
		particles[i].position.y = (float)(i / (Grid_Size - 1))*divisionY + 1.0f;
		particles[i].position.z = (float)(i / ((Grid_Size - 1)* (Grid_Size - 1)))*divisionZ + 0.1f;

		particles[i].density = DENSITY;
		particles[i].mass = MASS;
		particles[i].viscosity = VISCOSITY;
		particles[i].velocity = glm::vec3(0.0f);
		particles[i].acceleration = glm::vec3(0.0f);
	}

	for (i = 0; i < Grid_Size; i++)
	{
		for (j = 0; j < Grid_Size; j++)
		{
			for (k = 0; k < Grid_Size; k++)
			{
				grid[i][j][k].clear();
			}
		}
	}
}


void Particle::clear_tree(int Grid_Size)
{
	// Clear the grid. ( matrix of vectors )
	int i, j, k;
	for (i = 0; i < Grid_Size; i++)
	{
		for (j = 0; j < Grid_Size; j++)
		{
			for (k = 0; k < Grid_Size; k++)
			{
				grid[i][j][k].clear();
			}
		}
	}
}

void Particle::catergorizeParticles(int Number_of_particels, float BoundarySizeX, float BoundarySizeY, float BoundarySizeZ, int Grid_Size)
{
	// This function categorizes each particle into the respectivel grid using its position's first value as the index for the grid.
	clear_tree();
	Particle p;
	float divisionX = BoundarySizeX / Grid_Size, divisionY = BoundarySizeY / Grid_Size, divisionZ = BoundarySizeZ / Grid_Size;
	int x, y, z;
	for (int i = 0; i < Number_of_particels; i++)
	{
		p = particles[i];
		x = (int)floor(p.position.x / (divisionX));
		y = (int)floor(p.position.y / (divisionY));
		z = (int)floor(p.position.z / (divisionZ));

		x = (x < 0) ? 0 : x;
		y = (y < 0) ? 0 : y;
		z = (z < 0) ? 0 : z;

		x = (x > Grid_Size - 1) ? Grid_Size - 1 : x;
		y = (y > Grid_Size - 1) ? Grid_Size - 1 : y;
		z = (z > Grid_Size - 1) ? Grid_Size - 1 : z;

		grid[x][y][z].push_back(&particles[i]);


	}
}

//get all the neighbors for respective particle
void Particle::getNeighbors(int Number_of_particels, float BoundarySizeX, float BoundarySizeY, float BoundarySizeZ, int Grid_Size);
{
	Particle p;
	float divisionX = BoundarySizeX / Grid_Size, divisionY = BoundarySizeY / Grid_Size, divisionZ = BoundarySizeZ / Grid_Size;
	int x, y, z;

	for (int i = 0; i < Number_of_particels; i++)
	{
		p = particles[i];

		x = (int)floor(p.position.x / divisionX);
		y = (int)floor(p.position.y / divisionY);
		z = (int)floor(p.position.z / divisionZ);

		if (x == 1)
			x = 1;
		/*if (!((x < 0) || (x > Grid_Size - 1) || (y < 0) || (y > Grid_Size - 1) || (z < 0) || (z > Grid_Size - 1)))
		neighbors[i] = getNeighborsforPoint(x, y, z, particles[i]);
		else
		neighbors[i].clear();*/

		x = (x < 0) ? 0 : x;
		y = (y < 0) ? 0 : y;
		z = (z < 0) ? 0 : z;

		x = (x > Grid_Size - 1) ? Grid_Size - 1 : x;
		y = (y > Grid_Size - 1) ? Grid_Size - 1 : y;
		z = (z > Grid_Size - 1) ? Grid_Size - 1 : z;

		neighbors[i] = getNeighborsforPoint(x, y, z, particles[i]);
	}
}

std::vector<Particle *> getNeighborsforPoint(int x, int y, int z, Particle r)
{
	// Get neighbors for a particle in a specific grid. 

	std::vector<Particle *> p;
	bool xFlagL = false, yFlagL = false, zFlagL = false;
	bool xFlagM = false, yFlagM = false, zFlagM = false;
	std::vector<Particle *>::iterator it;

	p.clear();

	for (it = grid[x][y][z].begin(); it != grid[x][y][z].end(); it++)
	{
		p.push_back((*it));
	}

	// We have to get the particles within the support raius. So, we take the number of grids which encompass 1 support radius. (H/boundarySize)
	for (int i = 1; i <= (H*Grid_Size) / BoundarySizeX; i++)
	{
		xFlagL = false; yFlagL = false; zFlagL = false;
		xFlagM = false; yFlagM = false; zFlagM = false;
		// X-axis
		if (x - i >= 0)
		{
			for (it = grid[x - i][y][z].begin(); it != grid[x - i][y][z].end(); it++)
			{
				p.push_back((*it));
			}
			xFlagL = true;
		}
		if (x + i < Grid_Size)
		{
			for (it = grid[x + i][y][z].begin(); it != grid[x + i][y][z].end(); it++)
			{
				p.push_back((*it));
			}
			xFlagM = true;
		}

		//Y axis
		if (y - i >= 0)
		{
			for (it = grid[x][y - i][z].begin(); it != grid[x][y - i][z].end(); it++)
			{
				p.push_back((*it));
			}
			yFlagL = true;

		}
		if (y + i < Grid_Size)
		{
			for (it = grid[x][y + i][z].begin(); it != grid[x][y + i][z].end(); it++)
			{
				p.push_back((*it));
			}
			yFlagM = true;
		}

		// Z-axis
		if (z - i >= 0)
		{
			for (it = grid[x][y][z - i].begin(); it != grid[x][y][z - i].end(); it++)
			{
				p.push_back((*it));
			}
			zFlagL = true;
		}
		if (z + i < Grid_Size)
		{
			for (it = grid[x][y][z + i].begin(); it != grid[x][y][z + i].end(); it++)
			{
				p.push_back((*it));
			}
			zFlagM = true;
		}

		/*
		if (xFlagL && yFlagL)
		{
			for (it = grid[x - i][y - i][z].begin(); it != grid[x - i][y - i][z].end(); it++)
			{
				p.push_back((*it));
			}

			if (zFlagL)
			{
				for (it = grid[x - i][y - i][z - i].begin(); it != grid[x - i][y - i][z - i].end(); it++)
				{
					p.push_back((*it));
				}
			}
			if (zFlagM)
			{
			for (it = grid[x - i][y - i][z + i].begin(); it != grid[x - i][y - i][z + i].end(); it++)
			{
				p.push_back((*it));
			}
			}
		}

		if (xFlagM && yFlagM)
		{
		for (it = grid[x + i][y + i][z].begin(); it != grid[x + i][y + i][z].end(); it++)
		{
		p.push_back((*it));
		}

		if (zFlagL)
		{
		for (it = grid[x + i][y + i][z - i].begin(); it != grid[x + i][y + i][z - i].end(); it++)
		{
		p.push_back((*it));
		}
		}
		if (zFlagM)
		{
		for (it = grid[x + i][y + i][z + i].begin(); it != grid[x + i][y + i][z + i].end(); it++)
		{
		p.push_back((*it));
		}
		}
		}

		if (xFlagL && yFlagM)
		{
		for (it = grid[x - i][y + i][z].begin(); it != grid[x - i][y + i][z].end(); it++)
		{
		p.push_back((*it));
		}

		if (zFlagL)
		{
		for (it = grid[x - i][y + i][z - i].begin(); it != grid[x - i][y + i][z - i].end(); it++)
		{
		p.push_back((*it));
		}
		}
		if (zFlagM)
		{
		for (it = grid[x - i][y + i][z + i].begin(); it != grid[x - i][y + i][z + i].end(); it++)
		{
		p.push_back((*it));
		}
		}
		}

		if (xFlagM && yFlagL)
		{
		for (it = grid[x + i][y - i][z].begin(); it != grid[x + i][y - i][z].end(); it++)
		{
		p.push_back((*it));
		}

		if (zFlagL)
		{
		for (it = grid[x + i][y - i][z - i].begin(); it != grid[x + i][y - i][z - i].end(); it++)
		{
		p.push_back((*it));
		}
		}
		if (zFlagM)
		{
		for (it = grid[x + i][y - i][z + i].begin(); it != grid[x + i][y - i][z + i].end(); it++)
		{
		p.push_back((*it));
		}
		}
		}*/
	}
	return p;
}



