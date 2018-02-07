#include "Rocket.h"


Rocket::Rocket(glm::vec2 position, float radius, glm::vec4 colour) : Sphere(position, glm::vec2{0,0}, 5.0f, radius, colour)
{
	m_fuelConsumption = 1.0f;
	m_ejectionVelocity = 30.0f;
	m_radius = m_mass;
}

void Rocket::UpdateRocket(class PhysicsScene* scene, float dt)
{

}

void Rocket::SetFuelConsumption(float massPerSecond)
{
	m_fuelConsumption = massPerSecond;
}

void Rocket::SetFueEjectionVelocity(float metersPerSecond)
{

}
