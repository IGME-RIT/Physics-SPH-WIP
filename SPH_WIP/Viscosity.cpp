
#include "All_Includes.h"

#pragma region
//=================================================================
//						VISCOSITY
//=================================================================

//Calculate the force experienced by a particle due to another another particle's viscosity
glm::vec3 viscosityForcePerParticle(Particle &r, Particle &p)
{
	glm::vec3 fv;

	fv = (p.velocity - r.velocity) * p.mass * smoothKernelPoly6Laplacian(r.position - p.position) / p.density;

	return fv;
}
//------------------------------------------------------------------

#pragma endregion Viscosity
