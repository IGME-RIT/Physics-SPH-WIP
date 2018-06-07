
#include "All_Includes.h"


#pragma region
//================================================================
//						PRESSURE
//================================================================
//spike kernel for calculating the pressure force
glm::vec3 spikeKernelPoly6Gradient(glm::vec3 r)
{
	/*
	We are using spike kernel to smooth pressure. We are using the spike
	kernel because we need the pressure to increase alsmot exponentially as
	the distance between the two positons decreases.
	*/
	glm::vec3 grad(0.0f);;
	float R = glm::length(r);
	grad = glm::normalize(r) * (H - R) * (H - R) * (-45.0f);
	grad /= (PI * powf(H, 6));// *fmax(R, FLT_EPSILON));

	return grad;
}

//Calculate the force experienced by a particle due to another particle
glm::vec3 pressureForcePerParticle(Particle &r, Particle &p)
{
	/*
	PV = nRT

	n = mass/MolarMAss = 1000g /18 = 55.55555
	R = 0.0083144621(75) amu (km/s)2 K−1
	T= 293.15 K

	V = mass/density
	P = nRT * mass / density
	*/

	float P1 = K * 13.533444f  * r.density / (r.mass), P2 = K * 13.533444f * p.density / (p.mass);		// Calculating the pressure.

	/*
	Here we compute the force caused due to pressure difference between the particles.
	It is done by calculating the pressure difference between the two positions,
	and use the spike kernel to calculatethe force.
	*/
	glm::vec3 fp = (P1 + P2) * p.mass * spikeKernelPoly6Gradient(r.position - p.position) / (2.0f * p.density);

	return fp;
}
//-----------------------------------------------------------------

#pragma endregion Pressure