#pragma once

#include <glm/glm.hpp>
#include <vector>



class PhysicsObject;
class Sphere;
class Plane;

class PhysicsScene
{

public:
	PhysicsScene();
	~PhysicsScene();
	void addActor(PhysicsObject* actor);
	void removeActor(PhysicsObject* actor);
	void update(float dt);
	void updateGizmos();

	void setGravity(const glm::vec2 gravity) { m_gravity = gravity; }
	glm::vec2 getGravity() const {return m_gravity;}

	void setTimeStep(const float timeStep) { m_timeStep = timeStep; }
	float getTimeStep() const { return m_timeStep; }

	void checkForCollisions();

	static bool plane2Plane(PhysicsObject* object1, PhysicsObject* object2);
	static bool plane2Box(PhysicsObject* object1, PhysicsObject* object2);

	static bool sphere2Plane(PhysicsObject* object1, PhysicsObject* object2);
	static bool sphere2Sphere(PhysicsObject* object1, PhysicsObject* object2);
	static bool sphere2Box(PhysicsObject* object1, PhysicsObject* object2);

	static bool box2Box(PhysicsObject* object1, PhysicsObject* object2);

	//static bool box2Plane(PhysicsObject* object1, PhysicsObject* object2);
	//static bool box2Sphere(PhysicsObject* object1, PhysicsObject* object2);
	//static bool plane2Sphere(PhysicsObject* object1, PhysicsObject* object2);


protected:
	glm::vec2 m_gravity;
	float m_timeStep;
	std::vector<PhysicsObject*> m_actors;
};

