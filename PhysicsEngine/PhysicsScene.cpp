#include "PhysicsScene.h"
#include "PhysicsObject.h"
#include "algorithm"
#include <cassert>
#include "Sphere.h"
#include "Plane.h"
#include "Box.h"
#include <iostream>

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
	checkForCollisions();
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
	//              Plane							 Sphere							 Box
	/* Plane */	PhysicsScene::plane2Plane,	PhysicsScene::sphere2Plane,		PhysicsScene::plane2Box,
	/* Sphere */PhysicsScene::sphere2Plane, PhysicsScene::sphere2Sphere,	PhysicsScene::sphere2Box,
	/* Box */	PhysicsScene::plane2Box,	PhysicsScene::sphere2Box,		PhysicsScene::box2Box,
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

bool PhysicsScene::plane2Box(PhysicsObject * object1, PhysicsObject * object2)
{
	return false;
}

bool PhysicsScene::sphere2Plane(PhysicsObject * object1, PhysicsObject * object2)
{
	Sphere *sphere = dynamic_cast<Sphere*>(object1);
	Plane *plane = dynamic_cast<Plane*>(object2);

	//if we are successful then test for collision
	if (sphere != nullptr && plane != nullptr)
	{
		glm::vec2 collisionNormal = plane->getNormal();
		float sphereToPlane = glm::dot(sphere->getPosition(), plane->getNormal()) - plane->getDistance();

		// if we are behind plane then we flip the normal
		if (sphereToPlane < 0) {
			collisionNormal *= -1;
			sphereToPlane *= -1;
		}

		float intersection = sphere->getRadius() - sphereToPlane;
		if (intersection > 0) {
			//set sphere velocity to zero here
			sphere->setVelocity({ 0,0 });
			return true;
		}
	}
	return false;
}

bool PhysicsScene::sphere2Sphere(PhysicsObject * object1, PhysicsObject * object2)
{
	//try to cast objects to sphere and sphere
	Sphere* sphere1 = dynamic_cast<Sphere*>(object1);
	Sphere *sphere2 = dynamic_cast<Sphere*>(object2);
	assert((sphere1 != nullptr && sphere2 != nullptr));
	//if we are successful then test for collision
	if (sphere1 != nullptr && sphere2 != nullptr) {
		//this is where the collision detection happens
		//you need code which sets the velocity of the two spheres to zero

		//if they are overlapping
		// get distance between 2 spheres
		float distBetween = 0;
		distBetween = glm::distance(sphere2->getPosition(), sphere1->getPosition());

		// if distance is less than the combined radius of 
		// both spheres, then a collision occurred so set the
		if (distBetween < (sphere1->getRadius() + sphere2->getRadius()))
		{
			// velocity of both spheres to 0 (we’ll add collision resolution later)
			sphere1->setVelocity({ 0, 0 });
			sphere2->setVelocity({ 0, 0 });
			std::cout<< "SPHERES COLLIDING";
		}
		
	}
	return false;
}

bool PhysicsScene::sphere2Box(PhysicsObject * object1, PhysicsObject * object2)
{
	return false;
}

//bool PhysicsScene::box2Plane(PhysicsObject * object1, PhysicsObject * object2)
//{
//	return false;
//}

//bool PhysicsScene::box2Sphere(PhysicsObject * object1, PhysicsObject * object2)
//{
//
//	return false;
//}

bool PhysicsScene::box2Box(PhysicsObject * object1, PhysicsObject * object2)
{
	Box *box1 = dynamic_cast<Box*>(object1);
	Box *box2 = dynamic_cast<Box*>(object2);

	if (box1 != nullptr && box2 != nullptr) 
	{
		if (box1->min().x < box2->max().x
			&& box1->max().x > box2->min().x
			&& box1->min().y < box2->max().y
			&& box1->max().y > box2->min().y)
		{
			box1->setVelocity({ 0,0 });
			box2->setVelocity({ 0,0 });

			std::cout << "BOXES COLLIDING";
		}
	}
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

