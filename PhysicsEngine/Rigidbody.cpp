#include "Rigidbody.h"

Rigidbody::Rigidbody(ShapeType shapeID, glm::vec2 position, glm::vec2 velocity, float rotation, float mass) : PhysicsObject(shapeID)
{
	m_shapeID = shapeID;
	m_position = position;
	m_velocity = velocity;
	m_rotation = rotation;
	m_mass = mass;
	m_acceleration = { 0,0 };
}

Rigidbody::~Rigidbody()
{

}

void Rigidbody::fixedUpdate(glm::vec2 gravity, float timeStep)
{
	applyForce(gravity * m_mass);
	m_velocity += m_acceleration * timeStep;
	m_position += m_velocity * timeStep;
	m_acceleration = { 0,0 };
}

void Rigidbody::debug()
{
}

void Rigidbody::applyForce(glm::vec2 force)
{
	m_acceleration = force / m_mass;
}

void Rigidbody::applyForceToActor(Rigidbody * actor2, glm::vec2 force)
{
	actor2->applyForce(force);
	applyForce(-force);
}


