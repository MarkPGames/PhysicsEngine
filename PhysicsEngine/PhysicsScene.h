#pragma once

#include <glm/glm.hpp>
#include <vector>



class PhysicsObject;
class Sphere;
class Plane;

struct CollisionData
{
	CollisionData() { overlap = 0, normal = { 0, 0 }, collision = false; };
	CollisionData(float a_overlap, glm::vec2 a_normal, bool a_collision) { overlap = a_overlap; normal = a_normal; collision = a_collision;};
	CollisionData(bool a_collision) { collision = a_collision; };

	bool collision;
	float overlap;
	glm::vec2 normal;

};

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
	void resolveCollision(PhysicsObject * object1, PhysicsObject* object2 = nullptr);
	void seperateCollision(PhysicsObject * object1, PhysicsObject* object2 = nullptr);

	static CollisionData plane2Plane(PhysicsObject* object1, PhysicsObject* object2);
	static CollisionData plane2Box(PhysicsObject* object1, PhysicsObject* object2);

	static CollisionData sphere2Plane(PhysicsObject* object1, PhysicsObject* object2);
	static CollisionData sphere2Sphere(PhysicsObject* object1, PhysicsObject* object2);
	static CollisionData sphere2Box(PhysicsObject* object1, PhysicsObject* object2);

	static CollisionData box2Box(PhysicsObject* object1, PhysicsObject* object2);

	static CollisionData box2Plane(PhysicsObject* object1, PhysicsObject* object2);
	static CollisionData box2Sphere(PhysicsObject* object1, PhysicsObject* object2);
	static CollisionData plane2Sphere(PhysicsObject* object1, PhysicsObject* object2);


protected:
	glm::vec2 m_gravity;
	float m_timeStep;
	std::vector<PhysicsObject*> m_actors;
	CollisionData collisionData;
};

