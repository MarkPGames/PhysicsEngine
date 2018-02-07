#include "PhysicsScene.h"
#include "PhysicsObject.h"
#include "algorithm"
#include <cassert>

#define SHAPE_COUNT 2

PhysicsScene::PhysicsScene() : m_timeStep(0.01f), m_gravity(glm::vec2(0, 0))
{
	m_gravity = glm::vec2(0, -9.8f);
}


PhysicsScene::~PhysicsScene()
{
	for (auto& actor : m_actors)
	{
		delete actor;
	}
}

void PhysicsScene::update(float dt)
{
	// update physics at a fixed time step
	static float accumulatedTime = 0.0f;
	accumulatedTime += dt;

	while (accumulatedTime >= m_timeStep)
	{
		for (auto pActor : m_actors)
		{
			pActor->fixedUpdate(m_gravity, m_timeStep);
		}

		accumulatedTime -= m_timeStep;
	}
	
}

void PhysicsScene::updateGizmos()
{
	for (auto pActor : m_actors) {
		pActor->makeGizmo();
	}
}

// function pointer array for doing our collisions
typedef bool(*fn)(PhysicsObject*, PhysicsObject*);

static fn collisionFunctionArray[] =
{
	PhysicsScene::plane2Plane, PhysicsScene::plane2Sphere,
	PhysicsScene::sphere2Plane, PhysicsScene::sphere2Sphere,
};

void PhysicsScene::checkForCollisions()
{
	int actorCount = m_actors.size();

	//need to check for collisions against all objects except this one.
	for (int outer = 0; outer < actorCount - 1; outer++)
	{
		for (int inner = outer + 1; inner < actorCount; inner++)
		{
			PhysicsObject* object1 = m_actors[outer];
			PhysicsObject* object2 = m_actors[inner];
			int shapeId1 = object1->getShapeID();
			int shapeId2 = object2->getShapeID();

			// using function pointers
			int functionIdx = (shapeId1 * SHAPE_COUNT) + shapeId2;
			fn collisionFunctionPtr = collisionFunctionArray[functionIdx];
			if (collisionFunctionPtr != nullptr)
			{
				// did a collision occur?
				collisionFunctionPtr(object1, object2);
			}


		}
	}
}

bool PhysicsScene::plane2Plane(PhysicsObject* object1, PhysicsObject* object2)
{
	return false;
}

bool PhysicsScene::plane2Sphere(PhysicsObject * object1, PhysicsObject * object2)
{
	return false;
}

bool PhysicsScene::plane2Box(PhysicsObject * object1, PhysicsObject * object2)
{
	return false;
}

bool PhysicsScene::sphere2Plane(PhysicsObject * object1, PhysicsObject * object2)
{
	return false;
}

bool PhysicsScene::sphere2Sphere(PhysicsObject * object1, PhysicsObject * object2)
{
	return false;
}

bool PhysicsScene::sphere2Box(PhysicsObject * object1, PhysicsObject * object2)
{
	return false;
}

bool PhysicsScene::box2Plane(PhysicsObject * object1, PhysicsObject * object2)
{
	return false;
}

bool PhysicsScene::box2Sphere(PhysicsObject * object1, PhysicsObject * object2)
{
	return false;
}

bool PhysicsScene::box2Box(PhysicsObject * object1, PhysicsObject * object2)
{
	return false;
}

void PhysicsScene::addActor(PhysicsObject* actor)
{
	assert(actor != nullptr);
	assert(std::find(std::begin(m_actors), std::end(m_actors), actor) == m_actors.end());
	m_actors.push_back(actor);
}

void PhysicsScene::removeActor(PhysicsObject* actor)
{
	std::remove(std::begin(m_actors), std::end(m_actors), actor);
}

