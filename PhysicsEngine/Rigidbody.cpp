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

void Rigidbody::applyForce(glm::vec2 force, ForceMode forceMode)
{
	//Apply the force differently depending on the type of force
	switch (forceMode)
	{
	case Rigidbody::Force:
		m_acceleration += force / m_mass;
		break;
	case Rigidbody::Acceleration:
		m_acceleration += force;
		break;
	case Rigidbody::Impulse:
		m_velocity += force / mass;
		break;
	case Rigidbody::VelocityChange:
		m_velocity += force;
		break;
	default:
		assert(false);
		break;

	}

}

void Rigidbody::applyForceToActor(Rigidbody * actor2, glm::vec2 force, ForceMode forceMode)
{
	actor2->applyForce(force, forceMode);
	applyForce(-force, forceMode);
}

void Rigidbody::resolveCollision(Rigidbody * actor2)
{
	glm::vec2 normal = glm::normalize(actor2->getPosition() - m_position);
	glm::vec2 relativeVelocity = actor2->getVelocity() - m_velocity;

	float elasticity = 1;
	float j = glm::dot(-(1 + elasticity) * (relativeVelocity), normal) /
		glm::dot(normal, normal * ((1 / m_mass) + (1 / actor2->getMass())));

	glm::vec2 force = normal * j;

	applyForceToActor(actor2, force);
}

void Rigidbody::setVelocity(glm::vec2 a_vel)
{
	m_velocity = a_vel;
}


