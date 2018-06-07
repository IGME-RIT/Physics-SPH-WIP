
#include "All_Includes.h"




void GameObject::update(float dt, glm::mat4 PV)
{
	//Summing all forces acting on the object
	addForces();

	//Calculating position based of kinematics equations
	EularIntegration(dt);

	//translating object
	translation = glm::translate(glm::vec3(position));

	//Calculating model matrix and then updating the objects MVP
	calcTransform();
	MVP = PV * transformation;

	//Zeroing force and acceleration
	totalForce = acceleration = glm::vec3();


}

void GameObject::addForces()
{
	/*
	totalForce += GRAVITY * mass; //Gravitational force


	if (position.y < -1)
	{
		position.y=-1;

		if (velocity.y < 0)
			totalForce += velocity*-100.f; // Bounce force
	}

	acceleration = totalForce / mass; //Force=m * a so a = F/m 
	*/
}

//Calculates Transformation matrix -> T * R * S
void GameObject::calcTransform()
{
	transformation = translation * rotation * scale;
	
}


void GameObject::EularIntegration(float dt)
{
	
	
	position += velocity * dt;

	//Updating velocity
	velocity += acceleration * dt;
	
}


void GameObject::RK4Integration(float dt)
{
	/*
	K1 is the increment based on the slope at the beginning of the interval, uing y (euler's method)
	k2 is the increment based on the slope at the midpoint of the interval, using y + (h/2)k1
	k3 is the increment based on the slope at the midpoint of the interval, using y + (h/2)k2
	k4 is the increment based on the slope at the end of the interval, using y + h*k3

	k1-------------k2-----------------k3----------------k4
	|<--------------------- T -------------------------->|
	*/

	glm::vec3 k1, k2, k3, k4, vel;
	glm::vec3 k;

	vel = velocity;

	//Euler 1
	k = position + velocity * dt;
	vel += acceleration * dt;
	k1 = vel;

	//Euler 2
	k = (position + (dt*k1 / 2.0f)) + vel * dt / 2.0f;
	vel += acceleration * dt / 2.0f;
	k2 = vel;

	//Euler 3
	k = (position + (dt*k2 / 2.0f)) + vel * dt / 2.0f;
	vel += acceleration * dt / 2.0f;
	k3 = vel;

	//Euler 4
	k = (position + (dt*k3)) + vel * dt;
	vel += acceleration * dt;
	k4 = vel;

	//Use the velocity at the mid point to compute the displacement during the timestep h
	position += dt * (k1 + (2.0f * k2) + (2.0f * k3) + k4) / 6.0f;


	//Change the velocity to the value at the end of the timestep.
	velocity += acceleration * dt;

}

//Initalizing values
GameObject::GameObject(glm::mat4 mvp)
{
	velocity = totalForce = acceleration = position = glm::vec3();

	translation=rotation=scale=transformation= glm::mat4();

	mass = 1.0f;

	MVP = mvp;
}


GameObject::~GameObject()
{
}
