#include "Box.h"
#include <Gizmos.h>"


Box::Box(glm::vec2 position, glm::vec2 velocity, float mass, glm::vec2 size, glm::vec4 colour) : Rigidbody(BOX, position, velocity, 0, mass)
{
	m_size = size;
	m_colour = colour;
}


Box::~Box()
{
}

void Box::makeGizmo() const
{
	aie::Gizmos::add2DAABBFilled(m_position, m_size, m_colour);
}

bool Box::checkCollision(PhysicsObject * pOther)
{

	return false;
}