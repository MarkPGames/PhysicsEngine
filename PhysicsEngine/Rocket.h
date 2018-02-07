#pragma once
#include "Sphere.h"
#include <glm/glm.hpp>

class Rocket : public Sphere
{
public:
	Rocket(glm::vec2 position, float radius, glm::vec4 colour);

	void UpdateRocket(class PhysicsScene* scene, float dt);

	void SetFuelConsumption(float massPerSecond);
	void SetFueEjectionVelocity(float metersPerSecond);

private:
	float m_fuelConsumption;
	float m_ejectionVelocity;
};