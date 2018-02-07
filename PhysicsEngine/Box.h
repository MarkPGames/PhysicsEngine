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
	glm::vec4 getColour() { return m_colour; }

protected:
	glm::vec2 m_size;
	glm::vec4 m_colour;
};

