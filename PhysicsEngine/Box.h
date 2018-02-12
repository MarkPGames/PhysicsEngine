#pragma once
#include "Rigidbody.h"

class Box : public Rigidbody
{
public:
	Box() = delete;
	Box(glm::vec2 position, glm::vec2 velocity,
		float mass, glm::vec2 size, glm::vec4 colour);
	~Box();

	virtual void makeGizmo() const override;
	virtual bool checkCollision(PhysicsObject* pOther);
	glm::vec4 getColour() { return m_colour; };
	glm::vec2 getSize() { return m_size; };

	glm::vec2 min() { return { m_position.x - m_size.x, m_position.y - m_size.y }; };
	glm::vec2 max() { return { m_position.x + m_size.x, m_position.y + m_size.y}; };

protected:
	glm::vec2 m_size;
	glm::vec4 m_colour;
};

