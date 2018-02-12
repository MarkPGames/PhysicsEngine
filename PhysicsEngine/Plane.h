#pragma once
#include "PhysicsObject.h"

class Plane : public PhysicsObject
{
public:
	Plane();
	Plane(const glm::vec2& normal, float distance, glm::vec4 colour);
	~Plane();

	virtual void fixedUpdate(glm::vec2 gravity, float timeStep) override {};
	virtual void debug() override {}
	virtual void makeGizmo() const override;

	glm::vec2 getNormal() { return m_normal; }
	glm::vec4 getColour() { return m_colour; }
	float getDistance() { return m_distanceToOrigin; }

protected:
	glm::vec2 m_normal;
	glm::vec4 m_colour;
	float m_distanceToOrigin;
};

